cmake_minimum_required(VERSION 3.22)

# 源文件
add_library(wanna_platform
    Private/WannaLog.cpp
    Private/WannaWindow.cpp
    Private/WannaGlfwWindow.cpp
    Private/WannaGeomtryUtil.cpp
    Private/WannaGraphicContext.cpp
    Private/WannaVulkanContext.cpp
    Private/WannaVulkanDevice.cpp
    Private/WannaVulkanQueue.cpp
    Private/WannaVulkanSwapChain.cpp
    Private/WannaVulkanRenderPass.cpp
    Private/WannaVulkanFrameBuffer.cpp
    Private/WannaVulkanImage.cpp
    Private/WannaVulkanImageView.cpp
    Private/WannaVulkanPipeline.cpp
    Private/WannaVulkanBuffer.cpp
    Private/WannaVulkanCommandBuffer.cpp

)

target_include_directories(wanna_platform PUBLIC External)

# glm库配置
add_subdirectory(External/glm)
target_link_libraries(wanna_platform PRIVATE glm)

# glfw库配置
option(GLFW_BUILD_DOCS OFF)
option(GLFW_BUILD_TESTS OFF)
option(GLFW_BUILD_EXAMPLES OFF)
option(GLFW_INSTALL OFF)
add_subdirectory(External/glfw)
target_link_libraries(wanna_platform PRIVATE glfw)

# vulkan库配置
find_package(Vulkan REQUIRED)
if (Vulkan_FOUND)
    message("Vulkan Exist: ${Vulkan_INCLUDE_DIRS}")
endif()

target_include_directories(wanna_platform PUBLIC ${Vulkan_INCLUDE_DIRS})
target_link_libraries(wanna_platform PRIVATE ${Vulkan_LIBRARY})