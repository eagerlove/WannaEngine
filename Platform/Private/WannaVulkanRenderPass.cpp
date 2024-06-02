#include "WannaVulkanRenderPass.h"
#include "WannaVulkanDevice.h"

namespace WannaEngine {
    WannaVulkanRenderPass::WannaVulkanRenderPass(WannaVulkanDevice *device, const std::vector<VkAttachmentDescription> &attachments, const std::vector<RenderSubPass> &renderSubPasses) 
    : myDevice(device), myAttachments(attachments), myRenderSubPasses(renderSubPasses) {

        // 1. 默认子流程和附着的构建
        if (renderSubPasses.empty()) {
            if (attachments.empty()) {
                VkAttachmentDescription defaultColorAttachment = {
                    .flags = 0,
                    .format = device->getSettings().surfaceFormat,
                    .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR, // 使用一个常量值来清除附着的内容
                    .storeOp = VK_ATTACHMENT_STORE_OP_STORE, // 渲染的内容会被存储起来，以便之后读取
                    .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE, // 不关心附着现存的内容
                    .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE, // 渲染后，不会读取帧缓冲的内容
                    .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED, // 不关心之前的图像布局方式
                    .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR // 图像被用在交换链中进行呈现操作
                };
                myAttachments.push_back(defaultColorAttachment);
            }
            RenderSubPass defaultSubPass = {
                .colorAttachment = { 0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL }
            };
        }

        // 2. 子流程配置
        std::vector<VkSubpassDescription> subpassDescription(myRenderSubPasses.size());
        for (int i = 0; i < myRenderSubPasses.size(); i++) {
            RenderSubPass subpass = myRenderSubPasses[i];
            if (subpass.inputAttachment.ref >= 0 && subpass.inputAttachment.layout == VK_IMAGE_LAYOUT_UNDEFINED) {
                subpass.inputAttachment.layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL; // 默认只读
            }
            if (subpass.colorAttachment.ref >= 0 && subpass.colorAttachment.layout == VK_IMAGE_LAYOUT_UNDEFINED) {
                subpass.colorAttachment.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL; // 默认色彩
            }
            if (subpass.depthStencilAttachment.ref >= 0 && subpass.depthStencilAttachment.layout == VK_IMAGE_LAYOUT_UNDEFINED) {
                subpass.depthStencilAttachment.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL; // 默认深度
            }
            if (subpass.resolveAttachment.ref >= 0 && subpass.resolveAttachment.layout == VK_IMAGE_LAYOUT_UNDEFINED) {
                subpass.resolveAttachment.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL; // 只读
            }

            VkAttachmentReference inputAttachmentRef[] = {{ static_cast<uint32_t>(subpass.inputAttachment.ref), subpass.inputAttachment.layout }};
            VkAttachmentReference colorAttachmentRef[] = {{ static_cast<uint32_t>(subpass.colorAttachment.ref), subpass.colorAttachment.layout }};
            VkAttachmentReference depthStencilAttachmentRef[] = {{ static_cast<uint32_t>(subpass.depthStencilAttachment.ref), subpass.depthStencilAttachment.layout }};
            VkAttachmentReference resolveAttachmentRef[] = {{ static_cast<uint32_t>(subpass.resolveAttachment.ref), subpass.resolveAttachment.layout }};
            subpassDescription[i].flags = 0;
            subpassDescription[i].pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS; // 
            subpassDescription[i].inputAttachmentCount = subpass.inputAttachment.ref >= 0 ? ARRAY_SIZE(inputAttachmentRef) : 0;
            subpassDescription[i].pInputAttachments = subpass.inputAttachment.ref >= 0 ? inputAttachmentRef : nullptr;
        }
    
    // VkStructureType                   sType;
    // const void*                       pNext;
    // VkRenderPassCreateFlags           flags;
    // uint32_t                          attachmentCount;
    // const VkAttachmentDescription*    pAttachments;
    // uint32_t                          subpassCount;
    // const VkSubpassDescription*       pSubpasses;
    // uint32_t                          dependencyCount;
    // const VkSubpassDependency*        pDependencies;
        
        VkRenderPassCreateInfo renderPassInfo = {
            .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            


        };

        // 创建渲染流程
        CALL_VK(vkCreateRenderPass(myDevice->getHandle(), &renderPassInfo, nullptr, &myHandle));
    }
    WannaVulkanRenderPass::~WannaVulkanRenderPass()
    {
        
    }
}