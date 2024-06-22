#ifndef WANNA_VULKAN_FRAME_BUFFER_H
#define WANNA_VULKAN_FRAME_BUFFER_H

#include "WannaVulkanCommon.h"

namespace WannaEngine {
    class WannaVulkanDevice;
    class WannaVulkanRenderPass;
    class WannaVulkanImageView;
    class WannaVulkanImage;

    // 帧缓冲
    class WannaVulkanFrameBuffer {
        public:
            WannaVulkanFrameBuffer(WannaVulkanDevice *device, WannaVulkanRenderPass *renderPass, const std::vector<std::shared_ptr<WannaVulkanImage>> &images, uint32_t width, uint32_t height);
            ~WannaVulkanFrameBuffer();

            bool Recreate(const std::vector<std::shared_ptr<WannaVulkanImage>> &images, uint32_t width, uint32_t height);

            VkFramebuffer getHandle() const { return mHandle; }
            uint32_t getWidth() const { return mWidth; }
            uint32_t getHeight() const { return mHeight; }
        private:
            VkFramebuffer mHandle = VK_NULL_HANDLE;
            WannaVulkanDevice *mDevice;
            WannaVulkanRenderPass *mRenderPass;
            uint32_t mWidth;
            uint32_t mHeight;
            std::vector<std::shared_ptr<WannaVulkanImage>> mImages;
            std::vector<std::shared_ptr<WannaVulkanImageView>> mImageViews;

    };
}

#endif