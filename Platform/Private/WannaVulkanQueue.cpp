#include "WannaVulkanQueue.h"

namespace WannaEngine {
    WannaVulkanQueue::WannaVulkanQueue(uint32_t familyIndex, uint32_t index, VkQueue queue, bool canPresent) 
        : mFamilyIndex(familyIndex), mIndex(index), mQueue(queue), canPresent(canPresent) {
            LOG_TRACE("Create a new queue: {0} - {1} - {2}, present: {3}", mFamilyIndex, index, (void*)queue, canPresent);
    }

    // 等待队列任务完成
    void WannaVulkanQueue::waitIdle() const {
        CALL_VK(vkQueueWaitIdle(mQueue));
    }

}