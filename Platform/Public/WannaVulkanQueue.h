#ifndef WANNA_VULKAN_QUEUE_H
#define WANNA_VULKAN_QUEUE_H

#include "WannaVulkanCommon.h"

namespace WannaEngine {
    class WannaVulkanQueue {
        public:
            WannaVulkanQueue(uint32_t familyIndex, uint32_t index, VkQueue queue, bool canPresent);
            ~WannaVulkanQueue() = default;

            void waitIdle() const;

        private:
            uint32_t mFamilyIndex;
            uint32_t mIndex;
            VkQueue mQueue;
            bool canPresent;

    };
}

#endif