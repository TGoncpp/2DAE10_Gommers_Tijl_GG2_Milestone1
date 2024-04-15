#pragma once
#include <glm-1.0.0/glm/glm.hpp>

struct UniformBufferObject
{
	alignas(16) glm::mat4 model; //-> when not using the define off force aligned gentypes
	alignas(16) glm::mat4 view; // But it can break when using nested variables, (like selfmade structs)
	alignas(16) glm::mat4 proj; //Safer to always manually allign

};