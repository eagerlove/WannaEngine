#ifndef WANNA_VULKAN_QUEUE_H
#define WANNA_VULKAN_QUEUE_H

#include "WannaVulkanCommon.h"

namespace WannaEngine {
    class WannaVulkanQueue {
        public:
            WannaVulkanQueue(uint32_t familyIndex, uint32_t index, VkQueue queue, bool canPresent);
            ~WannaVulkanQueue() = default;

            VkQueue getHandle() const { return mHandle; }
            void waitIdle() const;
            void submit(std::vector<VkCommandBuffer> commandBuffers);

        private:
            uint32_t mFamilyIndex;
            uint32_t mIndex;
            VkQueue mHandle;
            bool canPresent;

    };
}

#endif