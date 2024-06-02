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
            const std::vector<VkImage> &getImages() const { return mImages; }
            uint32_t getWidth() const { return mSurfaceInfo.capabilities.currentExtent.width; }
            uint32_t getHeight() const { return mSurfaceInfo.capabilities.currentExtent.height; }

        
        private:
            void setSurfaceCapabilities();

            VkSwapchainKHR mHandle = VK_NULL_HANDLE;
            WannaVulkanContext *mContext;
            WannaVulkanDevice *mDevice;
            SurfaceInfo mSurfaceInfo;
            std::vector<VkImage> mImages;
    };
}

#endif