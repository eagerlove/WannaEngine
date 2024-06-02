#include "WannaVulkanRenderPass.h"
#include "WannaVulkanDevice.h"

namespace WannaEngine {
    WannaVulkanRenderPass::WannaVulkanRenderPass(WannaVulkanDevice *device, const std::vector<VkAttachmentDescription> &attachments, const std::vector<RenderSubPass> &renderSubPasses) 
    : myDevice(device), myAttachments(attachments), myRenderSubPasses(renderSubPasses) {

        // 1. 默认子流程及附着的构建
        if (renderSubPasses.empty()) {
            if (attachments.empty()) {
                VkAttachmentDescription defaultColorAttachment = {
                    .flags = 0,
                    .format = device->getSettings().surfaceFormat,
                    .samples = VK_SAMPLE_COUNT_1_BIT, // 采样一次
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
            myRenderSubPasses.push_back(defaultSubPass);
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
            // 各附着记录
            VkAttachmentReference inputAttachmentRef[] = {{ static_cast<uint32_t>(subpass.inputAttachment.ref), subpass.inputAttachment.layout }};
            VkAttachmentReference colorAttachmentRef[] = {{ static_cast<uint32_t>(subpass.colorAttachment.ref), subpass.colorAttachment.layout }};
            VkAttachmentReference depthStencilAttachmentRef[] = {{ static_cast<uint32_t>(subpass.depthStencilAttachment.ref), subpass.depthStencilAttachment.layout }};
            VkAttachmentReference resolveAttachmentRef[] = {{ static_cast<uint32_t>(subpass.resolveAttachment.ref), subpass.resolveAttachment.layout }};
            // description保存子流程信息
            subpassDescription[i].flags = 0;
            subpassDescription[i].pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS; // 
            subpassDescription[i].inputAttachmentCount = subpass.inputAttachment.ref >= 0 ? ARRAY_SIZE(inputAttachmentRef) : 0;
            subpassDescription[i].pInputAttachments = subpass.inputAttachment.ref >= 0 ? inputAttachmentRef : nullptr;
            subpassDescription[i].colorAttachmentCount = subpass.colorAttachment.ref >= 0 ? ARRAY_SIZE(colorAttachmentRef) : 0;
            subpassDescription[i].pColorAttachments = subpass.colorAttachment.ref >= 0 ? colorAttachmentRef : nullptr;
            subpassDescription[i].pResolveAttachments = subpass.resolveAttachment.ref >= 0 ? resolveAttachmentRef : nullptr;
            subpassDescription[i].pDepthStencilAttachment = subpass.depthStencilAttachment.ref >= 0 ? depthStencilAttachmentRef : nullptr;
            subpassDescription[i].preserveAttachmentCount = 0;
            subpassDescription[i].pPreserveAttachments = nullptr;
        }

        // 子流程之间的依赖关系
        std::vector<VkSubpassDependency> dependencies(myRenderSubPasses.size() - 1);
        if (myRenderSubPasses.size() > 1) {
            for (int i = 0; i < myRenderSubPasses.size(); i++) {
                dependencies[i].srcSubpass      = i;
                dependencies[i].dstSubpass      = i + 1;
                dependencies[i].srcStageMask    = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
                dependencies[i].dstStageMask    = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
                dependencies[i].srcAccessMask   = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
                dependencies[i].dstAccessMask   = VK_ACCESS_INPUT_ATTACHMENT_READ_BIT;
                dependencies[i].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
            }
        }

        // 3. 渲染流程创建信息
        VkRenderPassCreateInfo renderPassInfo = {
            .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .attachmentCount = static_cast<uint32_t>(myAttachments.size()),
            .pAttachments = myAttachments.data(),
            .subpassCount = static_cast<uint32_t>(myRenderSubPasses.size()),
            .pSubpasses = subpassDescription.data(),
            .dependencyCount = static_cast<uint32_t>(dependencies.size()),
            .pDependencies = dependencies.data()
        };

        // 创建渲染流程
        CALL_VK(vkCreateRenderPass(myDevice->getHandle(), &renderPassInfo, nullptr, &myHandle));
        LOG_TRACE("RenderPass {0} : {1}, attachment count: {2}, subpass count: {3}", __FUNCTION__, (void*)myHandle, myAttachments.size(), myRenderSubPasses.size());
    }
    WannaVulkanRenderPass::~WannaVulkanRenderPass()
    {
        VK_DESTROY(RenderPass, myDevice->getHandle(), myHandle);
    }
}