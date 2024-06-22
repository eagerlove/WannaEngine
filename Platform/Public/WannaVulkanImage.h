#ifndef WANNA_VULKAN_IMAGE_H
#define WANNA_VULKAN_IMAGE_H

#include "WannaVulkanCommon.h"

namespace WannaEngine {
    class WannaVulkanDevice;
    class WannaVulkanImage {
        public:
            WannaVulkanImage(WannaVulkanDevice *device, VkExtent3D extent, VkFormat format,  VkImageUsageFlags usage);
            WannaVulkanImage(WannaVulkanDevice *device, VkImage image, VkExtent3D extent, VkFormat format,  VkImageUsageFlags usage);
            ~WannaVulkanImage();

            VkImage getHandle() const { return mHandle; }
            VkFormat getFormat() const { return mFormat; }
        private:
            WannaVulkanDevice *mDevice;

            bool bCreateImage = true;

            VkImage mHandle = VK_NULL_HANDLE;
            VkDeviceMemory mMemory = VK_NULL_HANDLE;

            VkFormat mFormat;
            VkExtent3D mExtent;
            VkImageUsageFlags mUsage;
    };
}

#endif