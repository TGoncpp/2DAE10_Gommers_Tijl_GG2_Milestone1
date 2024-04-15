#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>

class Texture
{
public:
    Texture(const std::string& path, VkPhysicalDevice physicalDevice, VkDevice LogicalDevice, VkCommandPool commandPool, VkQueue graphicsQueue)
        :m_TexturePath{ path },
        m_PysicalDevice{physicalDevice},
        m_LogicalDevice{ LogicalDevice },
        m_CommandPool{commandPool},
        m_GraphicsQueue{graphicsQueue}
        {
        
        }
    ~Texture();
   

private:
	std::string      m_TexturePath;
    VkImage          m_TextureImage;
    VkDeviceMemory   m_TextureImageMemory;
    VkImageView      m_TextureImageView;
    VkDescriptorPool m_DescriptorPool;
    VkDescriptorSet  m_DescriptorSet;
    uint32_t         m_MipLvl;
    //VkSampler m_TextureSampler;
    VkPhysicalDevice m_PysicalDevice;
    VkDevice         m_LogicalDevice;
    VkCommandPool    m_CommandPool;
    VkQueue          m_GraphicsQueue;



    void createTextureImage(VkImage& image, VkDeviceMemory& imageMemory, const std::string& path);
    void createImage(uint32_t width, uint32_t height, uint32_t mipLvls, VkSampleCountFlagBits numSamples,
        VkFormat format, VkImageTiling tiling,
        VkImageUsageFlags usage, VkMemoryPropertyFlags properties,
        VkImage& image, VkDeviceMemory& imageMemory);

    void createDescriptorPool(VkDescriptorPool& descriptorpool);
    void createDescriptorSets(VkDescriptorPool& descriptorpool, std::vector<VkDescriptorSet>& vDescriptorSets, VkImageView imageView);
    //void createTextureSampler();

    void generateMipmaps(VkImage image, VkFormat format, int32_t texWidth, int32_t texHeight, uint32_t mipLevels);
    VkCommandBuffer beginSingleCommands();
    void endSingleCommands(VkCommandBuffer commandBuffer);



};