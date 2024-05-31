#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "Structs.h"
#include "InstanceStruct.h"
#include <string>
#include <memory>

class Pipeline;

class SceneObject
{
public:
    SceneObject(const std::string& modelPath, bool isColored)
        : m_ModelPath{ modelPath }, m_IsCollored{ isColored }, m_Is3D{ true } {};
    SceneObject(const std::vector<Vertex2D>& vVertex, const std::vector<uint32_t>& vIndices)
        : m_vVertices2D{ vVertex }, m_vIndices{ vIndices }, m_Is3D{ false } {};

	~SceneObject() = default;
    void Init(VkPhysicalDevice& m_PhysicalDevicem ,VkDevice& logicDevice, VkCommandPool& commandPool, const int FrmasInFlight, VkQueue& graphicsQueue, bool instanceRendering);
    void Record(VkCommandBuffer commandBuffer);
    void Destroy(VkDevice& logicDevice);

    VkBuffer GetVertexBuffer()const { return m_VertexBuffer; };
    VkBuffer GetIndexBuffer()const { return m_IndexBuffer; };
    std::vector<uint32_t> GetIndices()const { return m_vIndices; };
    void SetInstanceCountWithDistance(int count, float distance);

private:
    bool m_Is3D{ true };
    bool m_IsCollored{ true };
    bool m_IsInstanceRendering{ true };
    float m_Distance{ 2.f };
    //VkCommandPool m_CommandPool;
    //std::vector<VkCommandBuffer> m_vCommandBuffers;
    std::vector<Vertex3D> m_vVertices3D;
    std::vector<Vertex2D> m_vVertices2D;
    std::vector<uint32_t> m_vIndices;//use uint32_t when amount get above 65535
    VkBuffer m_VertexBuffer;
    VkDeviceMemory m_VertexBufferMemory;
    VkBuffer m_IndexBuffer;
    VkDeviceMemory m_IndexBufferMemory;
    std::string m_ModelPath{ "" };

    //InstanceRenderingVariables
    uint16_t m_InstanceCount{ 1000 };
    std::vector<InstanceVertex> m_vInstanceDate;
    VkBuffer m_InstanceBuffer;
    VkDeviceMemory m_InstanceBufferMemory;


    //init functions
    void loadModel();
    void createInstanceValues(float distance);
    void createVertexBuffer(VkPhysicalDevice& physicalDevice, VkDevice& logicDevice, VkCommandPool& commandPool, VkQueue& graphicsQueue);
    void createIndexBuffer(VkPhysicalDevice& physicalDevice, VkDevice& logicDevice, VkCommandPool& commandPool, VkQueue& graphicsQueue);
    void createInstanceVertexBuffer(VkPhysicalDevice& physicalDevice, VkDevice& logicDevice, VkCommandPool& commandPool, VkQueue& graphicsQueue);
    //void createCommandBuffers(VkDevice& logicDevice, VkCommandPool& commandPool, const int FrmasInFlight);

    //HelperFunctions
    VkCommandBuffer BeginSingleCommand(VkDevice& logicDevice, VkCommandPool& commandPool);
    void EndSingleCommand(VkDevice& logicDevice, VkCommandPool& commandPool, VkQueue& graphicsQueue, VkCommandBuffer& commandBuffer);
    void copyBuffer(VkDevice& logicDevice, VkCommandPool& commandPool, VkQueue& graphicsQueue, VkBuffer& srcBuffer, VkBuffer& dstBuffer, VkDeviceSize& size);
    void createBuffer(VkPhysicalDevice& m_PhysicalDevice, VkDevice& logicDevice, VkDeviceSize& bufferSize,
        VkBufferUsageFlags flags,
        VkMemoryPropertyFlags memryProps,
        VkBuffer& vertexBuffer, VkDeviceMemory& vertexBufferMemory);
    uint32_t findMemoryType(VkPhysicalDevice& m_PhysicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties);


};
