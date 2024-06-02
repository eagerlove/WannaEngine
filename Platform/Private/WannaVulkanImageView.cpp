#include "WannaVulkanImageView.h"
#include "WannaVulkanDevice.h"

namespace WannaEngine {
    WannaVulkanImageView::WannaVulkanImageView(WannaVulkanDevice *device, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags) 
        : mDevice(device) {
        
        VkImageViewCreateInfo imageViewInfo = {
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .image = image,
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .format = format,
            .components = {
                VK_COMPONENT_SWIZZLE_IDENTITY,
                VK_COMPONENT_SWIZZLE_IDENTITY,
                VK_COMPONENT_SWIZZLE_IDENTITY,
                VK_COMPONENT_SWIZZLE_IDENTITY
            },
            .subresourceRange = {
                .aspectMask = aspectFlags,
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 1
            }
        };
        CALL_VK(vkCreateImageView(device->getHandle(), &imageViewInfo, nullptr, &mHandle));
    }

    WannaVulkanImageView::~WannaVulkanImageView() {
        VK_DESTROY(ImageView, mDevice->getHandle(), mHandle);
    }
}