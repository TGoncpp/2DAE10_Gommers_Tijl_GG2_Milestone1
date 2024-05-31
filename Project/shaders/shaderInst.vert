#version 450

layout(binding = 0) uniform uniformBufferObject
{
    mat4 model;
    mat4 view;
    mat4 proj;
}
ubo;
layout(push_constant) uniform pushConstant
{
    mat4 model;
}
ps;

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inTexCoord;

layout (location = 3) in vec4 inVec1;
layout (location = 4) in vec4 inVec2;
layout (location = 5) in vec4 inVec3;
layout (location = 6) in vec4 inVec4;
layout (location = 7) in vec2 inTexCoordOffset;
mat4 model = mat4(inVec1, inVec2, inVec3, inVec4);

layout(location = 0) out vec3 fragNormal;
layout(location = 1) out vec2 fragTexCoord; 

void main() {
    gl_Position = ubo.proj * ubo.view * model * vec4(inPosition.xyz, 1.0);
    vec4 tNormal =  ps.model * vec4(inNormal,0);

    fragNormal = normalize(tNormal.xyz); // interpolation of normal attribute in fragment shader.
    fragTexCoord = inTexCoord;
    //fragTexCoord = (inTexCoord + inTexCoordOffset)/32.0f;
}