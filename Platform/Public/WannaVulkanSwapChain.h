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

// 交换链
    class WannaVulkanSwapChain {
        public:
            WannaVulkanSwapChain(WannaVulkanContext *context, WannaVulkanDevice *device);
            ~WannaVulkanSwapChain();

            bool ReCreate(); 
            int32_t AcquireImage(VkSemaphore semaphore, VkFence fence = VK_NULL_HANDLE) const;
            void Present(int32_t imageIndex, const std::vector<VkSemaphore> &waitSemaphores) const;
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