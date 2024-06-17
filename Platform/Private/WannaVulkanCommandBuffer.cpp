#include "WannaVulkanCommandBuffer.h"
#include "WannaVulkanDevice.h"

namespace WannaEngine {
// 命令池 用于申请多个命令缓冲区
    WannaVulkanCommandPool::WannaVulkanCommandPool(WannaVulkanDevice *device, uint32_t queueFamilyIndex) : mDevice(device){
        VkCommandPoolCreateInfo commandPoolInfo = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
            .pNext = nullptr,
            .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
            .queueFamilyIndex = queueFamilyIndex
        };
        CALL_VK(vkCreateCommandPool(mDevice->getHandle(), &commandPoolInfo, nullptr, &mHandle));
        LOG_TRACE("Create command pool successfully : {0}", (void*)mHandle);
    }

    WannaVulkanCommandPool::~WannaVulkanCommandPool() {
        VK_DESTROY(CommandPool, mDevice->getHandle(), mHandle);
    }

    // 创建命令缓冲区
    std::vector<VkCommandBuffer> WannaVulkanCommandPool::AllocateCommandBuffers(uint32_t count) const {
        std::vector<VkCommandBuffer> commandBuffers(count);
        VkCommandBufferAllocateInfo allocateInfo = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
            .pNext = nullptr,
            .commandPool = mHandle,
            .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
            .commandBufferCount = count
        };
        CALL_VK(vkAllocateCommandBuffers(mDevice->getHandle(), &allocateInfo, commandBuffers.data()));
        return commandBuffers;
    }
}