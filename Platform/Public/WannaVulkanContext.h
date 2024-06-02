#ifndef WANNA_VULKAN_CONTEXT_H
#define WANNA_VULKAN_CONTEXT_H

#include "WannaGraphicContext.h"
#include "WannaVulkanCommon.h"

namespace WannaEngine {
    typedef struct QueueFamilyInfo {
        int32_t queueFamilyIndex = -1;
        uint32_t queueCount;
    } QueueFamilyInfo;

    class WannaVulkanContext : public WannaGraphicContext{
        public:
            WannaVulkanContext(WannaWindow *win);
            ~WannaVulkanContext() override;

            VkInstance GetInstance() const { return mInstance; }
            VkSurfaceKHR GetSurface() const { return mSurface; }
            VkPhysicalDevice GetPhysicalDevice() const { return mPhysicalDevice; }
            const QueueFamilyInfo &GetGraphicQueueFamilyInfo() const { return mGraphicQueueFamily; }
            const QueueFamilyInfo &GetPresentQueueFamilyInfo() const { return mPresentQueueFamily; }
            VkPhysicalDeviceMemoryProperties GetPhysicalDeviceMemoryProperties() const { return mPhysicalDeviceMemoryProperties;}
            bool IsSameGraphicPresentQueueFamily() const { return mGraphicQueueFamily.queueFamilyIndex == mPresentQueueFamily.queueFamilyIndex; }

        private:
            static void printPhysicalDeviceInfo(VkPhysicalDeviceProperties *properties);
            static uint32_t countPhysicalDeviceScore(VkPhysicalDeviceProperties *properties);

            void CreateInstance();
            void CreateSurface(WannaWindow *win);
            void SelectPhysicalDevice();

            bool bValidate = true;
            VkInstance mInstance;
            VkSurfaceKHR mSurface;

            VkPhysicalDevice mPhysicalDevice;
            QueueFamilyInfo mGraphicQueueFamily;
            QueueFamilyInfo mPresentQueueFamily;
            VkPhysicalDeviceMemoryProperties mPhysicalDeviceMemoryProperties;
    };
}

#endif