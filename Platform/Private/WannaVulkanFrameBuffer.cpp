#include "WannaVulkanFrameBuffer.h"
#include "WannaVulkanDevice.h"
#include "WannaVulkanRenderPass.h"
#include "WannaVulkanImageView.h"

namespace WannaEngine {

    WannaVulkanFrameBuffer::WannaVulkanFrameBuffer(WannaVulkanDevice *device, WannaVulkanRenderPass *renderPass, const std::vector<VkImage> &images, uint32_t width, uint32_t height)
                        : mDevice(device), mRenderPass(renderPass), mWidth(width), mHeight(height) {
        Recreate(images, width, height);
    }

    WannaVulkanFrameBuffer::~WannaVulkanFrameBuffer() {
        VK_DESTROY(Framebuffer, mDevice->getHandle(), mHandle);
    }

    bool WannaVulkanFrameBuffer::Recreate(const std::vector<VkImage> &images, uint32_t width, uint32_t height) {
        VkResult ret;

        VK_DESTROY(Framebuffer, mDevice->getHandle(), mHandle);

        mWidth = width;
        mHeight = height;
        mImageViews.clear();

        VkImageView attachments[images.size()];
        for (int i = 0; i < images.size(); i++) {
            mImageViews.push_back(std::make_shared<WannaVulkanImageView>(mDevice, images[i], mDevice->getSettings().surfaceFormat, VK_IMAGE_ASPECT_COLOR_BIT));
            attachments[i] = mImageViews[i]->getHandle();
        }
        VkFramebufferCreateInfo frameBufferInfo = {
            .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .renderPass = mRenderPass->getHandle(),
            .attachmentCount = static_cast<uint32_t>(mImageViews.size()),
            .pAttachments = attachments,
            .width = width,
            .height = height,
            .layers = 1
        };
        ret = vkCreateFramebuffer(mDevice->getHandle(), &frameBufferInfo, nullptr, &mHandle);
        LOG_TRACE("FrameBuffer {0}, new: {1}, width: {2}, height: {3}, view count: {4}", __FUNCTION__, (void*)mHandle, mWidth, mHeight, mImageViews.size());
        return ret == VK_SUCCESS;
    }
}