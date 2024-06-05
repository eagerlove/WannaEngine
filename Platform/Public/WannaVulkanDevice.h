#ifndef WANNA_VULKAN_DEVICE_H
#define WANNA_VULKAN_DEVICE_H

#include "WannaVulkanCommon.h"

namespace WannaEngine {
    class WannaVulkanContext;
    class WannaVulkanQueue;

    // 默认设置
    typedef struct WannaVulkanSetting {
        VkFormat surfaceFormat = VK_FORMAT_B8G8R8A8_UNORM;
        VkPresentModeKHR presentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
        uint32_t swapChainImageCount = 3;
    } WannaVulkanSetting;

    // 逻辑设备
    class WannaVulkanDevice {
        public:
            WannaVulkanDevice(WannaVulkanContext *context, uint32_t graphicQueueCount, uint32_t presentQueueCount, const WannaVulkanSetting &settings = {});
            ~WannaVulkanDevice();

            VkDevice getHandle() const { return mHandle; }
            const WannaVulkanSetting &getSettings() const { return mSettings; }
        private:
            VkDevice mHandle = VK_NULL_HANDLE;
            WannaVulkanSetting mSettings;

            std::vector<std::shared_ptr<WannaVulkanQueue>> mGraphicQueues;
            std::vector<std::shared_ptr<WannaVulkanQueue>> mPresentQueues;
    };
}


#endif