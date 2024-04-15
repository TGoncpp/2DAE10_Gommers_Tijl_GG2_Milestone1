#include "Texture.h"
#include <stb-master/stb-master/stb_image.h>
#include <array>
#include "uniformBufferObject.h"


Texture::Texture(const std::string& path, VkPhysicalDevice physicalDevice, VkDevice LogicalDevice, VkCommandPool commandPool, VkQueue graphicsQueue, int maxFramesInFlight)
    :m_TexturePath{ path },
    m_PysicalDevice{ physicalDevice },
    m_LogicalDevice{ LogicalDevice },
    m_CommandPool{ commandPool },
    m_GraphicsQueue{ graphicsQueue },
    MAX_FRAMES_IN_FLIGHT{ maxFramesInFlight }
{
    Init();
}

void Texture::Init()
{
    createTextureImage();
    createTextureImageView();
    createTextureSampler();

    createDescriptorPool();
}

void Texture::createDiscripterSet(VkDescriptorSetLayout layout, std::vector<VkBuffer> commandBuffer)
{
    createDescriptorSets(layout, commandBuffer);
}

void Texture::Destroy()
{
    vkDestroySampler(m_LogicalDevice, m_TextureSampler, nullptr);
    vkDestroyImageView(m_LogicalDevice, m_TextureImageView, nullptr);
    vkDestroyImage(m_LogicalDevice, m_TextureImage, nullptr);
    vkFreeMemory(m_LogicalDevice, m_TextureImageMemory, nullptr);
    vkDestroyDescriptorPool(m_LogicalDevice, m_DescriptorPool, nullptr);
}


void Texture::createTextureImage()
{
    //load image
    int texWidth{}, textHeight{}, textChannels{};
    stbi_uc* pixels = stbi_load(m_TexturePath.c_str(), &texWidth, &textHeight, &textChannels, STBI_rgb_alpha);
    VkDeviceSize imageSize = texWidth * textHeight * 4;

    m_MipLvl = static_cast<uint32_t>(std::floor(std::log2(std::max(texWidth, textHeight)))) + 1;

    if (!pixels)
    {
        throw std::runtime_error{ "failed to load texture image" };
    }

    //create staging buffer to load in host memory
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingbufferMemory;
    createBuffer(imageSize,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
        stagingBuffer, stagingbufferMemory);

    void* data;
    vkMapMemory(m_LogicalDevice, stagingbufferMemory, 0, imageSize, 0, &data);
    memcpy(data, pixels, static_cast<size_t>(imageSize));
    vkUnmapMemory(m_LogicalDevice, stagingbufferMemory);

    stbi_image_free(pixels);

    //create image to transfer to and bind
    createImage(texWidth, textHeight, m_MipLvl, VK_SAMPLE_COUNT_1_BIT,
        VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        m_TextureImage, m_TextureImageMemory);

    transitionImageLayout(m_TextureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, m_MipLvl);
    copyBufferToImage(stagingBuffer, m_TextureImage, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(textHeight));
    //transitionImageLayout(m_TextureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, m_MipLvl);
    generateMipmaps(m_TextureImage, VK_FORMAT_R8G8B8A8_SRGB, texWidth, textHeight, m_MipLvl);

    vkDestroyBuffer(m_LogicalDevice, stagingBuffer, nullptr);
    vkFreeMemory(m_LogicalDevice, stagingbufferMemory, nullptr);
}

void Texture::createImage(uint32_t width, uint32_t height, uint32_t mipLvls, VkSampleCountFlagBits numSamples, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory)
{
    VkImageCreateInfo imageInfo{};
    imageInfo.sType           = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType       = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width    = static_cast<uint32_t>(width);
    imageInfo.extent.height   = static_cast<uint32_t>(height);
    imageInfo.extent.depth    = 1;
    imageInfo.mipLevels       = mipLvls;
    imageInfo.arrayLayers     = 1;
    imageInfo.format          = format; //!!use same format as used with the pixels
    imageInfo.tiling          = tiling; //use LINEAR if you want direct acces to the pixels
    imageInfo.initialLayout   = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage           = usage;
    imageInfo.sharingMode     = VK_SHARING_MODE_EXCLUSIVE;
    imageInfo.samples         = numSamples;
    imageInfo.flags           = 0;

    if (vkCreateImage(m_LogicalDevice, &imageInfo, nullptr, &image) != VK_SUCCESS)
    {
        throw std::runtime_error{ "failed to create image" };
    }

    //transfer to fast allocation memory
    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(m_LogicalDevice, image, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize  = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

    if (vkAllocateMemory(m_LogicalDevice, &allocInfo, nullptr, &imageMemory) != VK_SUCCESS)
    {
        throw std::runtime_error{ "failed to allocate image memory" };
    }

    vkBindImageMemory(m_LogicalDevice, image, imageMemory, 0);

}

void Texture::createTextureImageView()
{
    m_TextureImageView = createImageView(m_TextureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, m_MipLvl);
}

VkImageView Texture::createImageView(VkImage image, VkFormat format,
                                    VkImageAspectFlags aspectFlags, uint32_t mipLvls)
{
    VkImageViewCreateInfo viewInfo{};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = image;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = format;
    viewInfo.subresourceRange.aspectMask = aspectFlags;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = mipLvls;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;


    VkImageView imageView;
    if (vkCreateImageView(m_LogicalDevice, &viewInfo, nullptr, &imageView) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create texture image view!");
    }

    return imageView;
}


void Texture::createDescriptorPool()
{
    std::array<VkDescriptorPoolSize, 2> poolSizes{};
    poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSizes[0].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

    poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    poolSizes[1].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    poolInfo.pPoolSizes = poolSizes.data();
    poolInfo.maxSets = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
    poolInfo.flags = 0;

    if (vkCreateDescriptorPool(m_LogicalDevice, &poolInfo, nullptr, &m_DescriptorPool) != VK_SUCCESS)
    {
        throw std::runtime_error{ "failed to create decriptor pool" };
    }
}

void Texture::createDescriptorSets(VkDescriptorSetLayout layout, std::vector<VkBuffer> commandBuffer)
{
    std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, layout);
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool     = m_DescriptorPool;                                 //get fixed after first init
    allocInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
    allocInfo.pSetLayouts        = layouts.data();

    m_vDescriptorSet.resize(MAX_FRAMES_IN_FLIGHT);
    if (vkAllocateDescriptorSets(m_LogicalDevice, &allocInfo, m_vDescriptorSet.data()) != VK_SUCCESS)
    {
        throw std::runtime_error{ "failed to allocate discriptorSets" };
    }
    //will be destroyed automaticly when descriptorpool is destroyed

    for (size_t i{}; i < MAX_FRAMES_IN_FLIGHT; ++i)
    {
        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = commandBuffer[i];
        bufferInfo.offset = 0;
        bufferInfo.range = sizeof(UniformBufferObject);

        VkDescriptorImageInfo imageInfo{};
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageInfo.imageView = m_TextureImageView;
        imageInfo.sampler = m_TextureSampler;

        //here they get combined
        std::array<VkWriteDescriptorSet, 2> descriptorWrites{};
        descriptorWrites[0].sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[0].dstSet          = m_vDescriptorSet[i];
        descriptorWrites[0].dstBinding      = 0;
        descriptorWrites[0].dstArrayElement = 0;
        descriptorWrites[0].descriptorType  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrites[0].descriptorCount = 1;
        descriptorWrites[0].pBufferInfo = &bufferInfo;

        descriptorWrites[1].sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[1].dstSet          = m_vDescriptorSet[i];
        descriptorWrites[1].dstBinding      = 1;
        descriptorWrites[1].dstArrayElement = 0;
        descriptorWrites[1].descriptorType  = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        descriptorWrites[1].descriptorCount = 1;
        descriptorWrites[1].pImageInfo = &imageInfo;


        vkUpdateDescriptorSets(m_LogicalDevice,
            static_cast<uint32_t>(descriptorWrites.size()),
            descriptorWrites.data(), 0, nullptr);
    }
}

void Texture::createTextureSampler()
{
    VkSamplerCreateInfo samplerInfo{};
    samplerInfo.sType            = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter        = VK_FILTER_LINEAR;
    samplerInfo.minFilter        = VK_FILTER_LINEAR;
    samplerInfo.addressModeU     = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeV     = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeW     = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.anisotropyEnable = VK_TRUE;

    VkPhysicalDeviceProperties properties{};
    vkGetPhysicalDeviceProperties(m_PysicalDevice, &properties);
    samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;

    //if you dont want to use anisotropic behaviour
    //samplerInfo.anisotropyEnable = VK_FALSE;
    //samplerInfo.maxAnisotropy = 1.0f;

    samplerInfo.borderColor             =  VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK;
    samplerInfo.unnormalizedCoordinates = VK_FALSE; //-> false returns 0-1| true returns 0-width, 0-height
    samplerInfo.compareEnable           = VK_FALSE;
    samplerInfo.compareOp               = VK_COMPARE_OP_ALWAYS;
    samplerInfo.mipmapMode              = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerInfo.mipLodBias              = 0.0f;
    samplerInfo.minLod                  = static_cast<float>(m_MipLvl / 4.f);
    samplerInfo.maxLod                  = static_cast<float>(m_MipLvl);

    if (vkCreateSampler(m_LogicalDevice, &samplerInfo, nullptr, &m_TextureSampler) != VK_SUCCESS)
    {
        throw std::runtime_error{ "failed to create texture sampler" };
    }

}

void Texture::generateMipmaps(VkImage image, VkFormat format, int32_t texWidth, int32_t texHeight, uint32_t mipLevels)
{
    VkCommandBuffer commandBuffer = beginSingleCommands();

    //check if physical device supports linear filtering
    VkFormatProperties formatProperties;
    vkGetPhysicalDeviceFormatProperties(m_PysicalDevice, format, &formatProperties);
    if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT))
    {
        throw std::runtime_error("texture image format does not support linear blitting!");
    }
    //--

    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.image = image;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;
    barrier.subresourceRange.levelCount = 1;

    int32_t mipWidth = texWidth;
    int32_t mipHeight = texHeight;
    for (uint32_t i{ 1 }; i < mipLevels; ++i)
    {
        barrier.subresourceRange.baseMipLevel = i - 1;
        barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

        vkCmdPipelineBarrier(commandBuffer,
            VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
            0, nullptr,
            0, nullptr,
            1, &barrier);

        VkImageBlit blit{};
        blit.srcOffsets[0] = { 0, 0, 0 };
        blit.srcOffsets[1] = { mipWidth, mipHeight, 1 };
        blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        blit.srcSubresource.mipLevel = i - 1;
        blit.srcSubresource.baseArrayLayer = 0;
        blit.srcSubresource.layerCount = 1;
        blit.dstOffsets[0] = { 0, 0, 0 };
        blit.dstOffsets[1] = { mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1 };
        blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        blit.dstSubresource.mipLevel = i;
        blit.dstSubresource.baseArrayLayer = 0;
        blit.dstSubresource.layerCount = 1;

        vkCmdBlitImage(commandBuffer,
            image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
            image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            1, &blit,
            VK_FILTER_LINEAR);
        barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        vkCmdPipelineBarrier(commandBuffer,
            VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
            0, nullptr,
            0, nullptr,
            1, &barrier);

        if (mipWidth > 1) mipWidth /= 2;
        if (mipHeight > 1) mipHeight /= 2;

    }
    barrier.subresourceRange.baseMipLevel = mipLevels - 1;
    barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

    vkCmdPipelineBarrier(commandBuffer,
        VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
        0, nullptr,
        0, nullptr,
        1, &barrier);


    endSingleCommands(commandBuffer);
}

void Texture::copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height)
{
    VkCommandBuffer commandBuffer = beginSingleCommands();
    
    VkBufferImageCopy region{};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;
    
    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;
    
    region.imageOffset = { 0,0,0 };
    region.imageExtent = { width, height, 1 };
    
    vkCmdCopyBufferToImage(commandBuffer, buffer, image,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
    
    endSingleCommands(commandBuffer);

}

void Texture::createBuffer(VkDeviceSize bufferSize, VkBufferUsageFlags flags, VkMemoryPropertyFlags memryProps, VkBuffer& vertexBuffer, VkDeviceMemory& vertexBufferMemory)
{
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = bufferSize;
    bufferInfo.usage = flags;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE; //only used by graphicsqueue so exlusive is enough

    if (vkCreateBuffer(m_LogicalDevice, &bufferInfo, nullptr, &vertexBuffer) != VK_SUCCESS)
        throw std::runtime_error("creation off vertex buffer failed");

    VkMemoryRequirements memRequirements{};
    vkGetBufferMemoryRequirements(m_LogicalDevice, vertexBuffer, &memRequirements);

    //Memory allocation
    VkMemoryAllocateInfo allocateInfo{};
    allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocateInfo.allocationSize = memRequirements.size;
    allocateInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, memryProps);

    if (vkAllocateMemory(m_LogicalDevice, &allocateInfo, nullptr, &vertexBufferMemory) != VK_SUCCESS)
        throw std::runtime_error("failed to allocate memory for vertex buffer");

    //Filling the Vertex buffer
    vkBindBufferMemory(m_LogicalDevice, vertexBuffer, vertexBufferMemory, 0);
}

VkCommandBuffer Texture::beginSingleCommands()
{
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = m_CommandPool;
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(m_LogicalDevice, &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    return commandBuffer;
}

void Texture::endSingleCommands(VkCommandBuffer commandBuffer)
{
    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    vkQueueSubmit(m_GraphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(m_GraphicsQueue);

    vkFreeCommandBuffers(m_LogicalDevice, m_CommandPool, 1, &commandBuffer);

}

void Texture::transitionImageLayout(VkImage image, VkFormat format,
                                    VkImageLayout oldLayout, VkImageLayout newLayout,
                                    uint32_t mipLvls)
{
    VkCommandBuffer commandbuffer = beginSingleCommands();

    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = image;

    if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
    {
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
        if (hasStencilComponent(format))
            barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
    }
    else
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;

    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = mipLvls;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;
    barrier.srcAccessMask = 0;
    barrier.dstAccessMask = 0;
    VkPipelineStageFlags srceStage;
    VkPipelineStageFlags dstStage;

    if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED
        && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
    {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        srceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        dstStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    }
    else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
        && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
    {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        srceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        dstStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    }
    else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED
        && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
    {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

        srceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        dstStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    }
    else
    {
        throw std::invalid_argument("unsupported layout transition!");
    }

    vkCmdPipelineBarrier(
        commandbuffer,
        srceStage, dstStage,
        0,
        0, nullptr,
        0, nullptr,
        1, &barrier
    );

    endSingleCommands(commandbuffer);

}

bool Texture::hasStencilComponent(VkFormat format)
{
    return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
}

uint32_t Texture::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
    VkPhysicalDeviceMemoryProperties memProps{};
    vkGetPhysicalDeviceMemoryProperties(m_PysicalDevice, &memProps);

    for (uint32_t i{}; i < memProps.memoryTypeCount; ++i)
    {
        if (typeFilter & (1 << i) && (memProps.memoryTypes[i].propertyFlags & properties) == properties)
        {
            return i;
        }
    }

    throw std::runtime_error("failed to find suitable memory type");
}


