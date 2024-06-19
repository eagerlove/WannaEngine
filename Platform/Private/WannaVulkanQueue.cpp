#include "WannaVulkanQueue.h"

namespace WannaEngine {
    WannaVulkanQueue::WannaVulkanQueue(uint32_t familyIndex, uint32_t index, VkQueue queue, bool canPresent) 
        : mFamilyIndex(familyIndex), mIndex(index), mHandle(queue), canPresent(canPresent) {
            LOG_TRACE("Create a new queue: {0} - {1} - {2}, present: {3}", mFamilyIndex, index, (void*)queue, canPresent);
    }

    // 等待队列任务完成
    void WannaVulkanQueue::waitIdle() const {
        CALL_VK(vkQueueWaitIdle(mHandle));
    }

    void WannaVulkanQueue::submit(std::vector<VkCommandBuffer> commandBuffers) {
        VkPipelineStageFlags waitDstStageMask[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
        VkSubmitInfo submitInfo = {
            .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
            .pNext = nullptr,
            .waitSemaphoreCount = 0,
            .pWaitSemaphores = nullptr,
            .pWaitDstStageMask = waitDstStageMask,
            .commandBufferCount = static_cast<uint32_t>(commandBuffers.size()),
            .pCommandBuffers = commandBuffers.data(),
            .signalSemaphoreCount = 0,
            .pSignalSemaphores = nullptr
        };
        CALL_VK(vkQueueSubmit(mHandle, 1, &submitInfo, VK_NULL_HANDLE));
    }

}