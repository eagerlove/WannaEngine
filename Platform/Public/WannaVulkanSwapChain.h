#ifndef WANNA_SWAP_CHAIN_H
#define WANNA_SWAP_CHAIN_H

#include "WannaVulkanCommon.h"

namespace WannaEngine {
    class WannaVulkanContext;
    class WannaVulkanDevice;

    typedef struct SurfaceInfo{
        VkSurfaceCapabilitiesKHR capabilities; // 表面支持
        VkSurfaceFormatKHR surfaceFormat; // 表面格式
        VkPresentModeKHR presentMode; // 显示模式
    } SurfaceInfo;

    class WannaVulkanSwapChain {
        public:
            WannaVulkanSwapChain(WannaVulkanContext *context, WannaVulkanDevice *device);
            ~WannaVulkanSwapChain();

            bool ReCreate(); 

        private:
            void setSurfaceCapabilities();

            VkSwapchainKHR myHandle = VK_NULL_HANDLE;
            WannaVulkanContext *myContext;
            WannaVulkanDevice *myDevice;
            SurfaceInfo mySurfaceInfo;
            std::vector<VkImage> mImages;
    };
}

#endif