#include "WannaVulkanSwapChain.h"
#include "WannaVulkanContext.h"
#include "WannaVulkanDevice.h"
#include "WannaVulkanQueue.h"

namespace WannaEngine {
    // 交换链构造函数
    WannaVulkanSwapChain::WannaVulkanSwapChain(WannaVulkanContext *context, WannaVulkanDevice *device) 
        : mContext(context), mDevice(device){
            ReCreate();
    }

    // 交换链析构函数
    WannaVulkanSwapChain::~WannaVulkanSwapChain() {
        VK_DESTROY(SwapchainKHR, mDevice->getHandle(), mHandle);
    }

    // 当窗口发生变更时重建交换链
    bool WannaVulkanSwapChain::ReCreate() {
        LOG_DEBUG("-----------------------------");
        setSurfaceCapabilities();
        LOG_DEBUG("currentExtent : {0} x {1}", mSurfaceInfo.capabilities.currentExtent.width, mSurfaceInfo.capabilities.currentExtent.height);
        LOG_DEBUG("surfaceFormat : {0}", vk_format_string(mSurfaceInfo.surfaceFormat.format));
        LOG_DEBUG("presentMode   : {0}", vk_present_mode_string(mSurfaceInfo.presentMode));
        LOG_DEBUG("-----------------------------");

        // 交换链暂存图片数量
        uint32_t imageCount = mDevice->getSettings().swapChainImageCount;
        if (imageCount < mSurfaceInfo.capabilities.minImageCount && mSurfaceInfo.capabilities.minImageCount > 0) {
            imageCount = mSurfaceInfo.capabilities.minImageCount;
        }
        if (imageCount > mSurfaceInfo.capabilities.maxImageCount && mSurfaceInfo.capabilities.maxImageCount > 0) {
            imageCount = mSurfaceInfo.capabilities.maxImageCount;
        }

        VkSharingMode imageSharingMode;
        uint32_t queueFamilyIndexCount;
        uint32_t pQueueFamilyIndices[2] = { 0, 0 };
        if (mContext->IsSameGraphicPresentQueueFamily()) {
            imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            queueFamilyIndexCount = 0;
        } else {
            imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            queueFamilyIndexCount = 2;
            pQueueFamilyIndices[0] = mContext->GetGraphicQueueFamilyInfo().queueFamilyIndex;
            pQueueFamilyIndices[1] = mContext->GetPresentQueueFamilyInfo().queueFamilyIndex;
        }

        VkSwapchainKHR oldSwapchain = mHandle;

        VkSwapchainCreateInfoKHR swapChainCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
            .pNext = nullptr,
            .flags = 0,
            .surface = mContext->GetSurface(),
            .minImageCount = imageCount,
            .imageFormat = mSurfaceInfo.surfaceFormat.format,
            .imageColorSpace = mSurfaceInfo.surfaceFormat.colorSpace,
            .imageExtent = mSurfaceInfo.capabilities.currentExtent,
            .imageArrayLayers = 1,
            .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
            .imageSharingMode = imageSharingMode,
            .queueFamilyIndexCount = queueFamilyIndexCount,
            .pQueueFamilyIndices = pQueueFamilyIndices,
            .preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
            .compositeAlpha = VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR, // VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR is a non-supported value for nvidia
            .presentMode = mSurfaceInfo.presentMode,
            .clipped = VK_FALSE,
            .oldSwapchain = oldSwapchain
        };
        VkResult ret = vkCreateSwapchainKHR(mDevice->getHandle(), &swapChainCreateInfo, nullptr, &mHandle);
        if (ret != VK_SUCCESS) {
            LOG_ERROR("{0}, {1}", __FUNCTION__, vk_result_string(ret));
            return false;
        }

        LOG_DEBUG("Swapchain {0} : old: {1}, new: {2}, image count: {3}, format: {4}, present mode : {5}", __FUNCTION__, (void*)oldSwapchain, (void*)mHandle, imageCount,
            vk_format_string(mSurfaceInfo.surfaceFormat.format), vk_present_mode_string(mSurfaceInfo.presentMode));

        // 获取交换链图像
        uint32_t swapchainImageCount;
        ret = vkGetSwapchainImagesKHR(mDevice->getHandle(), mHandle, &swapchainImageCount, nullptr);
        mImages.resize(swapchainImageCount);
        ret = vkGetSwapchainImagesKHR(mDevice->getHandle(), mHandle, &swapchainImageCount, mImages.data());
        return ret == VK_SUCCESS;
    }

    // 设置surface
    void WannaVulkanSwapChain::setSurfaceCapabilities() {
        // surface性能参数
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(mContext->GetPhysicalDevice(), mContext->GetSurface(), &mSurfaceInfo.capabilities);

        // 读取surface默认设置
        WannaVulkanSetting settings = mDevice->getSettings();

        // surface支持格式
        uint32_t formatCount;
        CALL_VK(vkGetPhysicalDeviceSurfaceFormatsKHR(mContext->GetPhysicalDevice(), mContext->GetSurface(), &formatCount, nullptr));
        if (formatCount == 0) {
            LOG_ERROR("{0}: num of surface format is 0", __FUNCTION__);
            return;
        }
        VkSurfaceFormatKHR surfaceFormats[formatCount];
        CALL_VK(vkGetPhysicalDeviceSurfaceFormatsKHR(mContext->GetPhysicalDevice(), mContext->GetSurface(), &formatCount, surfaceFormats));
        int32_t foundFormatIndex = -1;
        for (int i = 0; i < formatCount; i++) {
            if (surfaceFormats[i].format == settings.surfaceFormat && surfaceFormats[i].colorSpace == VK_COLORSPACE_SRGB_NONLINEAR_KHR) {
                foundFormatIndex = i;
                break;
            }
        }
        if (foundFormatIndex == -1) {
            foundFormatIndex = 0;
        }
        mSurfaceInfo.surfaceFormat = surfaceFormats[foundFormatIndex];

        // 选择显示模式
        uint32_t presentModeCount;
        CALL_VK(vkGetPhysicalDeviceSurfacePresentModesKHR(mContext->GetPhysicalDevice(), mContext->GetSurface(), &presentModeCount, nullptr));
        if (presentModeCount == 0) {
            LOG_ERROR("{0}: num of surface present format is 0", __FUNCTION__);
            return;
        }
        VkPresentModeKHR presentModes[presentModeCount];
        CALL_VK(vkGetPhysicalDeviceSurfacePresentModesKHR(mContext->GetPhysicalDevice(), mContext->GetSurface(), &presentModeCount, presentModes));
        int32_t foundPresentModeIndex = -1;
        VkPresentModeKHR preferedPresentMode = mDevice->getSettings().presentMode;
        for (int i = 0; i < presentModeCount; i++) {
            if (presentModes[i] == preferedPresentMode) {
                foundPresentModeIndex = i;
                break;
            }
        }
        if (foundPresentModeIndex >= 0) {
            mSurfaceInfo.presentMode = presentModes[foundPresentModeIndex];
        } else {
            mSurfaceInfo.presentMode = presentModes[0];
        }
    }

    int32_t WannaVulkanSwapChain::AcquireImage(VkSemaphore semaphore, VkFence fence) const{
        uint32_t imageIndex;
        CALL_VK(vkAcquireNextImageKHR(mDevice->getHandle(), mHandle, UINT64_MAX, semaphore, fence, &imageIndex));
        if (fence != VK_NULL_HANDLE) {
            CALL_VK(vkWaitForFences(mDevice->getHandle(), 1, &fence, VK_FALSE, UINT64_MAX));
            CALL_VK(vkResetFences(mDevice->getHandle(), 1, &fence));
        }
        return imageIndex;
    }

    void WannaVulkanSwapChain::Present(int32_t imageIndex, const std::vector<VkSemaphore> &waitSemaphores) const {
        VkPresentInfoKHR presentInfo = {
            .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
            .pNext = nullptr,
            .waitSemaphoreCount = static_cast<uint32_t>(waitSemaphores.size()),
            .pWaitSemaphores = waitSemaphores.data(),
            .swapchainCount = 1,
            .pSwapchains = &mHandle,
            .pImageIndices = reinterpret_cast<const uint32_t *>(&imageIndex)
        };
        CALL_VK(vkQueuePresentKHR(mDevice->GetFirstPresentQueue()->getHandle(), &presentInfo));
        mDevice->GetFirstPresentQueue()->waitIdle();
    }
}