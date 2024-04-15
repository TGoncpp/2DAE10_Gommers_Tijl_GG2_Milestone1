#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>

class Texture final
{
public:
    Texture(const std::string& path, VkPhysicalDevice physicalDevice, VkDevice LogicalDevice, VkCommandPool commandPool, VkQueue graphicsQueue, int maxFramesInFlight);
       
    ~Texture() = default;
   
    void Init();
    void createDiscripterSet(VkDescriptorSetLayout layout, std::vector<VkBuffer> commandBuffer);
    void Destroy();

    std::vector<VkDescriptorSet> GetDescriptorSets()const { return m_vDescriptorSet; };

private:
	std::string      m_TexturePath;
    VkImage          m_TextureImage;
    VkDeviceMemory   m_TextureImageMemory;
    VkImageView      m_TextureImageView;
    VkDescriptorPool m_DescriptorPool;
    std::vector<VkDescriptorSet>m_vDescriptorSet;
    uint32_t         m_MipLvl;
    VkSampler        m_TextureSampler;
    VkPhysicalDevice m_PysicalDevice;
    VkDevice         m_LogicalDevice;
    VkCommandPool    m_CommandPool;
    VkQueue          m_GraphicsQueue;
    const int       MAX_FRAMES_IN_FLIGHT;


    void createTextureImage();
    void createTextureImageView();
    VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLvls);
    void createImage(uint32_t width, uint32_t height, uint32_t mipLvls, VkSampleCountFlagBits numSamples,
        VkFormat format, VkImageTiling tiling,
        VkImageUsageFlags usage, VkMemoryPropertyFlags properties,
        VkImage& image, VkDeviceMemory& imageMemory);

    void createDescriptorPool();
    void createDescriptorSets(VkDescriptorSetLayout layout, std::vector<VkBuffer> commandBuffer);
    void createTextureSampler();

    void generateMipmaps(VkImage image, VkFormat format, int32_t texWidth, int32_t texHeight, uint32_t mipLevels);
    VkCommandBuffer beginSingleCommands();
    void endSingleCommands(VkCommandBuffer commandBuffer);

    //----------------------------------------------------
    void createBuffer(VkDeviceSize bufferSize,
                      VkBufferUsageFlags flags,
                      VkMemoryPropertyFlags memryProps,
                      VkBuffer& vertexBuffer, VkDeviceMemory& vertexBufferMemory);
    void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
    void transitionImageLayout(VkImage image, VkFormat format,
                               VkImageLayout oldLayout, VkImageLayout newLayout,
                               uint32_t mipLvls);

    bool hasStencilComponent(VkFormat format);
    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);


};





