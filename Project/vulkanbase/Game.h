#pragma once

//#include <vulkan/vulkan.h> been replaced with belowed Define AND include
//GLFW is interface for window Handle
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <memory> //used for RAI manegment

#include <iostream>
#include <stdexcept>
#include <cstdlib>

#include <vector>
#include <string>

#include "vulkanbase/Structs.h"

//Abstract classes
//#include "vulkanbase/newCamera.h"
#include "vulkanbase/Pipeline.h"
#include "vulkanbase/Object.h"
#include "vulkanbase/texture.h"


class Camera final
{
public:
    Camera(const glm::vec3& position, float fov, float ar)
        :m_Position{ position }, m_AspectRatio{ ar }, m_FieldOfView{ fov } {}
    ~Camera() = default;

    void keyEvent(int key, int scancode, int action, int mods);
    void mouseMove(GLFWwindow* window, double xpos, double ypos);
    void mouseEvent(GLFWwindow* window, int button, int action, int mods);

    glm::mat4 CalculateViewMat();
    glm::mat4 CalculateProjMat();

private:
    glm::vec3 m_Position{};
    glm::vec3 m_WorldPosition{};
    float m_FieldOfView{ 1.f };
    float m_AspectRatio{ 1.f };
    const float m_NearPlane{ 0.1f };
    const float m_FarPlane{ 100.f };

    glm::vec3 m_Forward{};
    glm::vec3 m_Right{};
    glm::vec3 m_Up{ glm::vec3{ 0.f,0.f,1.f } };
    glm::vec2 m_DragStart{};
    float m_Yaw{ glm::radians(120.f)};
    float m_Pitch{ glm::radians(100.f)};

};

//enable validationLayers while on debug mode
const std::vector<const char*> vValidationLayers = { "VK_LAYER_KHRONOS_validation" };
#ifdef NDEBUG
    const bool enableValidationLayers = false;
#else
    const bool enableValidationLayers = true;
#endif

    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData) 
    {
        std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

        //if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
        //    // Message is important enough to show
        //}
        return VK_FALSE;
    }


class Game final
{
public:
    void run();
   
    bool m_FramebufferResiezed{ false };

private:
    //Window variables
    GLFWwindow* m_Window = nullptr;
    const uint32_t m_WindowWidth{ 800 };
    const uint32_t m_WindowHeight{ 600 };
    const std::string m_WindowTitle{ "My Awesome screen" };

    //Vulkan variable
    VkInstance m_Instance;
    VkDebugUtilsMessengerEXT m_DebugMessenger;
    VkSurfaceKHR m_Surface;
    VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE; //Implicitly destroyed??
    VkDevice m_LogicalDevice = VK_NULL_HANDLE;
    VkQueue m_GraphicsQueue;
    VkQueue m_PresentQueue;
    const std::vector<const char*> m_vDeviceExtensions = {
                                                        VK_KHR_SWAPCHAIN_EXTENSION_NAME
                                                       };
    VkSwapchainKHR m_SwapChain;
    std::vector <VkImage> m_vSwapChainImages;
    VkFormat m_SwapChainImageFormat;
    VkExtent2D m_SwapChainExtent;
    std::vector<VkImageView> m_vSwapChainImageViews;
    VkRenderPass m_RenderPass;
    VkDescriptorSetLayout m_DescriptorSetLayout;
   
    std::vector<VkFramebuffer> m_vSwapchainFramebuffers;


   VkCommandPool m_CommandPool;
     
    std::vector < VkSemaphore> m_vImageAvailableSemaphores;
    std::vector < VkSemaphore> m_vRenderFinishedAvailableSemaphores;
    std::vector < VkFence> m_vInFlightFences;

    std::vector<VkCommandBuffer> m_vCommandBuffers;
    std::vector<VkCommandBuffer> m_vCommandBuffers2D;
    std::vector<VkBuffer> m_vUniformBuffers;
    std::vector<VkDeviceMemory> m_vUniformBuffersMemory;
    std::vector<void*> m_vUniformBuffersMapped;
    VkDescriptorPool m_DescriptorPool;
    VkDescriptorPool m_DescriptorPool2D;
    
    VkSampleCountFlagBits m_MsaaSamples = VK_SAMPLE_COUNT_1_BIT;

   
    VkImage m_DepthImage;
    VkDeviceMemory m_DepthImageMemory;
    VkImageView m_DepthImageView;

    VkImage m_ColorImage;
    VkDeviceMemory m_ColorImageMemory;
    VkImageView m_ColorImageView;

    std::unique_ptr<Camera> m_pCamera;
    std::unique_ptr<Pipeline> m_p3DPipeline;
    std::unique_ptr<Pipeline> m_p3DInstancePipeline;
    std::unique_ptr<Pipeline> m_p3DDynamicPipeline;
    std::unique_ptr<SceneObject> m_p3DPlane;
    std::unique_ptr<SceneObject> m_p3DRoom;
    std::unique_ptr<SceneObject> m_p3DSkyDome;

    
    std::vector< std::unique_ptr<Texture>> m_vTextures;
    std::vector<float> m_vScales;
    void SetRandomScale(std::vector<float>& v, int amount, float maxScale);

    std::vector<Vertex2D> m_vsQuare2D
    { 
       {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f},{0.0f, 0.f}},
       {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f},{.0f, 1.0f} },
       {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, { 1.0f, 1.f }},
       {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f},{ 1.0f, 0.f }}
    };
    std::vector<uint32_t> m_vSquraInd
    {
        0, 1, 2, 0, 2, 3
    };
    std::vector<Vertex2D> m_vOval2D;
    std::vector<uint32_t> m_vOvalInd;
    void FillOvalResources(const glm::vec2& pos, float radius, int numOfCorners, std::vector<Vertex2D>& vertices, std::vector<uint32_t>& indices);
    std::unique_ptr<Pipeline> m_p2DPipeline;
    std::unique_ptr<SceneObject> m_p2DObject;
    std::unique_ptr<SceneObject> m_p2DOvalObject;


    //gloabal variables for keeping track off rendering frames and the max off frames to deal with
    const int MAX_FRAMES_IN_FLIGHT = 2;
    uint32_t m_CurrentFrame        = 0;


    float m_RotationSpeed{ 50.f };


    //-----------------------------------------------------------
    //Main functions
    void initWindow();
    static void framebufferResizeCallBack(GLFWwindow* window, int width, int height);
    
    //create an instance to create a link between the application and the Vulkan library
    void initVulkan();

    void mainLoop(); 

    void cleanup();

    //Help functions
    //Instance
    void  createInstance();

    //validation layers
    bool checkValidationLayerSupport();
    std::vector<const char*> getRequiredExtensions();
    void setupDebugMessenger();
    VkResult CreateDebugUtilsMessengerEXT(VkInstance instance,
        const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
        const VkAllocationCallbacks* pAllocator,
        VkDebugUtilsMessengerEXT* pDebugMessenger);
    static void DestroyDebugUtilMessengerEXT(VkInstance instance,
        VkDebugUtilsMessengerEXT debugMessenger,
        const VkAllocationCallbacks* pAllocator);

    //surface creation
    void createSurface();

    //physicalDevice
    void pickPhysicalDevice();
    bool isDeviceSuitable(VkPhysicalDevice device)const;

    //Queue families
    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device)const;

    //logical Device
    void createLogicalDevice();

    //SWAPCHAIN
    //check if valid extension is available
    bool checkDeviceExtensionSupport(VkPhysicalDevice phDevice)const;
    //fill in property details for swapchain
    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice phDevice)const;
    //setting the properties for the swapchain to check if your prefered one is available 
        //->Format (color depth)
    VkSurfaceFormatKHR  chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
        //Presentation Modes !!Most IMPORTANT ONE (swapping conditions)
    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
        //SwapExtent (resolution)
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& availableExtents);
    //creating the swapchain
    void createSwapChain();
    //this is used for when surface become invallid and needs to recalculate
    void recreateSwapchain();
    void cleanupSwapchain();

    //IMAGE VIEW
    void createImageViews();

   // //RENDER PASS
    void createRenderPass();
    void beginRenderPass(VkCommandBuffer commandBuffer, uint32_t imageIndex);

    //DRAWING
    //----------------------------------
    void createFramebuffer();
    void createCommandPool();
    void createCommandBuffers(std::vector<VkCommandBuffer>& commandBuffers);
    void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
    void drawFrame();

    //SEMAPHORE AND FENCE
    void createSyncObjects();

    //BUFFERS
    //void createVertexBuffer();
    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
    //void createIndexBuffer();
    void createUniformBuffers();
    //void createDescriptorPool(VkDescriptorPool& descriptorpool);
    //void createDescriptorSets(VkDescriptorPool& descriptorpool, std::vector<VkDescriptorSet>& vDescriptorSets, VkImageView imageView);

    public:
    //Abstraction
    void createBuffer(VkDeviceSize bufferSize, 
        VkBufferUsageFlags flags,
        VkMemoryPropertyFlags memryProps, 
        VkBuffer& vertexBuffer, VkDeviceMemory& vertexBufferMemory);
    void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

    private:
    //Descripters
    void createDescriptorSetLayout();


    //UPDATE
    void updateUniformBuffer(uint32_t currentImage);

    //TEXTURES
    void createTextureImage(VkImage& image, VkDeviceMemory& imageMemory, const std::string& path);
    void createImage(uint32_t width, uint32_t height, uint32_t mipLvls, VkSampleCountFlagBits numSamples,
                     VkFormat format, VkImageTiling tiling,
                     VkImageUsageFlags usage, VkMemoryPropertyFlags properties,
                     VkImage& image, VkDeviceMemory& imageMemory);
    void createTextureImageView(VkImageView& imageView, const VkImage& image);
    //void createTextureSampler();
    void createDepthResources();//for all depth resources
    void createColorResources();// for all multisampling resources

    //helper functions
    VkCommandBuffer beginSingleCommands();
    void endSingleCommands(VkCommandBuffer commandBuffer);
    void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
    void transitionImageLayout(VkImage image, VkFormat format, 
                                VkImageLayout oldLayout, VkImageLayout newLayout,
                                uint32_t mipLvls);

    VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLvls);
    VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
    VkFormat findDepthFormat();
    bool hasStencilComponent(VkFormat format);
    void generateMipmaps(VkImage image, VkFormat format, int32_t texWidth, int32_t texHeight, uint32_t mipLevels);
    VkSampleCountFlagBits getMaxUsableSampleCount();

};
