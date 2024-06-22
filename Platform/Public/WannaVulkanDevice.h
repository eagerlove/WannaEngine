#ifndef WANNA_VULKAN_DEVICE_H
#define WANNA_VULKAN_DEVICE_H

#include "WannaVulkanCommon.h"

namespace WannaEngine {
    class WannaVulkanContext;
    class WannaVulkanQueue;
    class WannaVulkanCommandPool;

    // 默认设置
    typedef struct WannaVulkanSetting {
        VkFormat surfaceFormat = VK_FORMAT_B8G8R8A8_UNORM; // 表面纹理
        VkPresentModeKHR presentMode = VK_PRESENT_MODE_IMMEDIATE_KHR; // 显示模式
        uint32_t swapChainImageCount = 3; // 交换链图像缓存最大个数
    } WannaVulkanSetting;

    // 逻辑设备
    class WannaVulkanDevice {
        public:
            WannaVulkanDevice(WannaVulkanContext *context, uint32_t graphicQueueCount, uint32_t presentQueueCount, const WannaVulkanSetting &settings = {});
            ~WannaVulkanDevice();

            VkDevice getHandle() const { return mHandle; }
            VkPipelineCache getPipelineCache() const { return mPipelineCache; }
            const WannaVulkanSetting &getSettings() const { return mSettings; }

            WannaVulkanQueue* GetGraphicQueue(uint32_t index) const { return mGraphicQueues.size() < index + 1 ? nullptr : mGraphicQueues[index].get(); }
            WannaVulkanQueue* GetFirstGraphicQueue() const { return mGraphicQueues.empty() ? nullptr : mGraphicQueues[0].get(); }
            WannaVulkanQueue* GetPresentQueue(uint32_t index) const { return mPresentQueues.size() < index + 1 ? nullptr : mPresentQueues[index].get(); }
            WannaVulkanQueue* GetFirstPresentQueue() const { return mPresentQueues.empty() ? nullptr : mPresentQueues[0].get(); }
            WannaVulkanCommandPool *getDefaultCmdPool() const { return mDefaultCmdPool.get(); }

            VkCommandBuffer beginDefaultCommandBuffer();
            void submitOneCommandBuffer(VkCommandBuffer commandBuffer);

            // 获取内存类型索引
            int32_t getMemroyIndex(VkMemoryPropertyFlags memProps, uint32_t memoryTypeBits) const;

        private:
            VkDevice mHandle = VK_NULL_HANDLE;
            VkPipelineCache mPipelineCache = VK_NULL_HANDLE;

            WannaVulkanSetting mSettings;
            WannaVulkanContext *mContext;

            std::vector<std::shared_ptr<WannaVulkanQueue>> mGraphicQueues;
            std::vector<std::shared_ptr<WannaVulkanQueue>> mPresentQueues;
            std::shared_ptr<WannaVulkanCommandPool> mDefaultCmdPool;

            void CreatePipelineCache(); // 创建管线缓存
            void CreateDefaultCommandPool(); // 创建默认命令池
    };
}


#endif