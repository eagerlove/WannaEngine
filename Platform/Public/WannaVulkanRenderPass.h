#ifndef WANNA_VULKAN_RENDER_PASS_H
#define WANNA_VULKAN_RENDER_PASS_H

#include "WannaVulkanCommon.h"


namespace WannaEngine {
    class WannaVulkanDevice;
    // 附件
    typedef struct Attachment {
        int32_t ref = -1; // 指定被引用附着的索引
        VkImageLayout layout = VK_IMAGE_LAYOUT_UNDEFINED; // 不关心之前的布局
    } Attachment;

    // 渲染子流程
    typedef struct RenderSubPass {
        Attachment inputAttachment; // 着色器的输入附着(图像资源)
        Attachment colorAttachment; // 颜色缓冲附着
        Attachment depthStencilAttachment; // 深度和模板数据附着
        Attachment resolveAttachment; // 多重采样的颜色附着
    } RenderSubPass;

    // 渲染流程
    class WannaVulkanRenderPass {
        public:
            WannaVulkanRenderPass(WannaVulkanDevice *device, const std::vector<VkAttachmentDescription> &attachments = {}, const std::vector<RenderSubPass> &renderSubPasses = {});
            ~WannaVulkanRenderPass();
            VkRenderPass getHandle() const {return myHandle;}

        private:
            VkRenderPass myHandle = VK_NULL_HANDLE;
            WannaVulkanDevice *myDevice;
            
            std::vector<VkAttachmentDescription> myAttachments;
            std::vector<RenderSubPass> myRenderSubPasses;

    };
}


#endif