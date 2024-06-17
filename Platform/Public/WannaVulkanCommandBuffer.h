#ifndef WANNA_VULKAN_COMMAND_BUFFER_H
#define WANNA_VULKAN_COMMAND_BUFFER_H

#include "WannaVulkanCommon.h"

namespace WannaEngine {
    class WannaVulkanDevice;

    class WannaVulkanCommandPool {
        public:
            WannaVulkanCommandPool(WannaVulkanDevice *device, uint32_t queueFamilyIndex);
            ~WannaVulkanCommandPool();

            std::vector<VkCommandBuffer> AllocateCommandBuffers(uint32_t count) const;

            VkCommandPool getHandle() const { return mHandle; }
        private:
            VkCommandPool mHandle;

            WannaVulkanDevice *mDevice;
    };
}

#endif