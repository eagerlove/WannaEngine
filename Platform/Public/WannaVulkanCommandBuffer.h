#ifndef WANNA_VULKAN_COMMAND_BUFFER_H
#define WANNA_VULKAN_COMMAND_BUFFER_H

#include "WannaVulkanCommon.h"

namespace WannaEngine {
    class WannaVulkanDevice;

// 命令池 用于申请多个命令缓冲区
    class WannaVulkanCommandPool {
        public:
            WannaVulkanCommandPool(WannaVulkanDevice *device, uint32_t queueFamilyIndex);
            ~WannaVulkanCommandPool();

            static void BeginCommandBuffer(VkCommandBuffer commandBuffer);
            static void EndCommandBuffer(VkCommandBuffer commandBuffer);

            std::vector<VkCommandBuffer> AllocateCommandBuffer(uint32_t count) const;
            VkCommandBuffer AllocateOneCommandBuffer() const;
            VkCommandPool getHandle() const { return mHandle; }
        private:
            VkCommandPool mHandle;

            WannaVulkanDevice *mDevice;
    };
}

#endif