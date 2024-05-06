# Source files
set(SHADER_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/shaders")
set(SHADER_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/shaders")

file(GLOB_RECURSE GLSL_SOURCE_FILES
    "${SHADER_SOURCE_DIR}/*.frag"
    "${SHADER_SOURCE_DIR}/*.vert"
)

foreach(GLSL ${GLSL_SOURCE_FILES})
    get_filename_component(FILE_NAME ${GLSL} NAME)
    set(SPIRV "${SHADER_BINARY_DIR}/${FILE_NAME}.spv")
    add_custom_command(
        OUTPUT ${SPIRV}
        COMMAND ${Vulkan_GLSLC_EXECUTABLE} ${GLSL} -o ${SPIRV}
        DEPENDS ${GLSL}
    )
    list(APPEND SPIRV_BINARY_FILES ${SPIRV})
endforeach(GLSL)

add_custom_target(
    Shaders 
    DEPENDS ${SPIRV_BINARY_FILES}
)


# Source files Textures
set(TEXTURE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/textures")
set(TEXTURE_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/textures")

file(GLOB_RECURSE REQUIRED_FILES
    "${TEXTURE_SOURCE_DIR}/*.png"
)

# Create the executable
#add_executable(${PROJECT_NAME} ${SOURCES} ${GLSL_SOURCE_FILES}  "vulkanbase/Game.h" "vulkanbase/Game.cpp")
add_executable(Vulkan 
				"vulkanbase/game.h" "vulkanbase/game.cpp" 
				"vulkanbase/pipeline.h" "vulkanbase/pipeline.cpp"
				"vulkanbase/Object.h" "vulkanbase/Object.cpp"
				"vulkanbase/semaphore.h"
				"vulkanbase/structs.h"
				"vulkanbase/instanceStruct.h"
				"vulkanbase/Time.h" "vulkanbase/Time.cpp" 
                "vulkanbase/Texture.h" "vulkanbase/Texture.cpp"
                "vulkanbase/newCamera.h" "vulkanbase/newCamera.cpp"
                "vulkanbase/UniformBufferObject.h"
				"main.cpp"  "vulkanbase/instanceStruct.h")
add_dependencies(Vulkan Shaders)

# Link libraries
target_include_directories(Vulkan PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(Vulkan PRIVATE ${Vulkan_LIBRARIES} glfw)

add_custom_target(copy_resources ALL)
add_custom_command(
    TARGET copy_resources PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/project/models
    $<TARGET_FILE_DIR:Vulkan>/models)
add_custom_command(    TARGET copy_resources PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
   ${CMAKE_SOURCE_DIR}/project/textures
    $<TARGET_FILE_DIR:Vulkan>/textures)

