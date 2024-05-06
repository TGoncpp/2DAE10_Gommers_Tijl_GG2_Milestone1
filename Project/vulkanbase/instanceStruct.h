#pragma once

#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm-1.0.0/glm/glm.hpp>
#include <array>

struct InstanceVertex
{
	~InstanceVertex() = default;


	glm::mat4 modelTransform;
	glm::vec2 texcoord;

	static VkVertexInputBindingDescription getBindDescription()
	{
		VkVertexInputBindingDescription bindingDescription{};
		bindingDescription.binding = 1;//-> put to 1 for instance rendering
		bindingDescription.stride = sizeof(InstanceVertex);
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_INSTANCE;
		return bindingDescription;
	}

	static const int attributeNum{ 5 };
	static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions(uint32_t location)
	{
		int binding = 1;
		std::vector<VkVertexInputAttributeDescription> attributeDescriptions(attributeNum);
		for (int i{}; i < 4; ++i)
		{
			attributeDescriptions[i].binding  = binding;
			attributeDescriptions[i].location = location + i;
			attributeDescriptions[i].format   = VK_FORMAT_R32G32B32A32_SFLOAT;
			attributeDescriptions[i].offset   = sizeof(float)*4 * i; //offsetof gives the number off bytes between start off the struct and start off specified member
		}

		attributeDescriptions[4].binding  = binding;
		attributeDescriptions[4].location = location + 4;
		attributeDescriptions[4].format   = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[4].offset   = offsetof(InstanceVertex, texcoord);

		return attributeDescriptions;
	}

};