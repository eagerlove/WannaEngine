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

            VkInstance GetInstance() const { return myInstance; }
            VkSurfaceKHR GetSurface() const { return mySurface; }
            VkPhysicalDevice GetPhysicalDevice() const { return myPhysicalDevice; }
            const QueueFamilyInfo &GetGraphicQueueFamilyInfo() const { return myGraphicQueueFamily; }
            const QueueFamilyInfo &GetPresentQueueFamilyInfo() const { return myPresentQueueFamily; }
            VkPhysicalDeviceMemoryProperties GetPhysicalDeviceMemoryProperties() const { return myPhysicalDeviceMemoryProperties;}
            bool IsSameGraphicPresentQueueFamily() const { return myGraphicQueueFamily.queueFamilyIndex == myPresentQueueFamily.queueFamilyIndex; }

        private:
            static void printPhysicalDeviceInfo(VkPhysicalDeviceProperties *properties);
            static uint32_t countPhysicalDeviceScore(VkPhysicalDeviceProperties *properties);

            void CreateInstance();
            void CreateSurface(WannaWindow *win);
            void SelectPhysicalDevice();

            bool bValidate = true;
            VkInstance myInstance;
            VkSurfaceKHR mySurface;

            VkPhysicalDevice myPhysicalDevice;
            QueueFamilyInfo myGraphicQueueFamily;
            QueueFamilyInfo myPresentQueueFamily;
            VkPhysicalDeviceMemoryProperties myPhysicalDeviceMemoryProperties;
    };
}

#endif