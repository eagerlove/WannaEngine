#ifndef WANNA_VULKAN_IMAGE_VIEW_H
#define WANNA_VULKAN_IMAGE_VIEW_H

#include "WannaVulkanCommon.h"

namespace WannaEngine {
    class WannaVulkanDevice;

    // 图像视图
    class WannaVulkanImageView {
        public:
            WannaVulkanImageView(WannaVulkanDevice *device, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);
            ~WannaVulkanImageView();

            VkImageView getHandle() const { return mHandle; }
        private:
            VkImageView mHandle = VK_NULL_HANDLE;
            WannaVulkanDevice *mDevice;
    };
}


#endif