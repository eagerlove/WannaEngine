#include "WannaVulkanPipeline.h"
#include "WannaVulkanFileUtil.h"
#include "WannaVulkanDevice.h"


namespace WannaEngine {

// 渲染管线布局
    WannaVulkanPipelineLayout::WannaVulkanPipelineLayout(WannaVulkanDevice *device,
            const std::string &vertexShaderFile,
            const std::string &fragShaderFile,
            const ShaderLayout &shaderLayout) : mDevice(device) {
        // 编译完成的着色器文件作为模块加入
        CALL_VK(createShaderModule(vertexShaderFile + ".spv", &mVertexShaderModule));
        CALL_VK(createShaderModule(fragShaderFile + ".spv", &mFragShaderModule));

        // 管线布局
        VkPipelineLayoutCreateInfo pipelineLayoutInfo = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .setLayoutCount = static_cast<uint32_t>(shaderLayout.descriptorSetLayouts.size()),
            .pSetLayouts = shaderLayout.descriptorSetLayouts.data(),
            .pushConstantRangeCount = static_cast<uint32_t>(shaderLayout.pushConstants.size()),
            .pPushConstantRanges = shaderLayout.pushConstants.data()
        };
        CALL_VK(vkCreatePipelineLayout(mDevice->getHandle(), &pipelineLayoutInfo, nullptr, &mHandle));
    }

    WannaVulkanPipelineLayout::~WannaVulkanPipelineLayout() {
        VK_DESTROY(PipelineLayout, mDevice->getHandle(), mHandle);
    }

    VkResult WannaVulkanPipelineLayout::createShaderModule(const std::string &filePath, VkShaderModule *shaderMoudle) {

        std::vector<char> content= ReadCharArrayFromFile(filePath);

        VkShaderModuleCreateInfo shaderModuleInfo = {
            .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .codeSize = static_cast<uint32_t>(content.size()),
            .pCode = reinterpret_cast<const uint32_t *>(content.data()),
        };

        return vkCreateShaderModule(mDevice->getHandle(), &shaderModuleInfo, nullptr, shaderMoudle);
    }

// 渲染管线
    WannaVulkanPipeline::WannaVulkanPipeline(WannaVulkanDevice *device, WannaVulkanRenderPass *renderPass, WannaVulkanPipelineLayout *pipelineLayout) : mDevice(device), mRenderPass(renderPass) ,mPipelineLayout(pipelineLayout) {
        
    }

    WannaVulkanPipeline::~WannaVulkanPipeline() {
        // vkDestroyPipelineCache
        // vkDestroyPipelineLayout
        // vkDestroyPipeline
    }

    WannaVulkanPipeline *WannaVulkanPipeline::SetVertexInputState(const std::vector<VkVertexInputBindingDescription> &vertexBindings,
                                                                   const std::vector<VkVertexInputAttributeDescription> &vertexAttrs) {
        mPipelineConfig.vertexInputState.vertexBindings = vertexBindings;
        mPipelineConfig.vertexInputState.vertexAttributes = vertexAttrs;
        return this;
    }

    WannaVulkanPipeline *WannaVulkanPipeline::SetInputAssemblyState(VkPrimitiveTopology topology, VkBool32 primitiveRestartEnable = VK_FALSE) {
        mPipelineConfig.inputAssemblyState.topology = topology;
        mPipelineConfig.inputAssemblyState.primitiveRestartEnable = primitiveRestartEnable;
        return this;
    }

    WannaVulkanPipeline *WannaVulkanPipeline::SetRasterizationState(const PipelineRasterizationState &rasterizationState) {
        mPipelineConfig.rasterizationState.depthClampEnable = rasterizationState.depthClampEnable;
        mPipelineConfig.rasterizationState.rasterizerDiscardEnable = rasterizationState.rasterizerDiscardEnable;
        mPipelineConfig.rasterizationState.polygonMode = rasterizationState.polygonMode;
        mPipelineConfig.rasterizationState.cullMode = rasterizationState.cullMode;
        mPipelineConfig.rasterizationState.frontFace = rasterizationState.frontFace;
        mPipelineConfig.rasterizationState.depthBiasEnable = rasterizationState.depthBiasEnable;
        mPipelineConfig.rasterizationState.depthBiasConstantFactor = rasterizationState.depthBiasConstantFactor;
        mPipelineConfig.rasterizationState.depthBiasClamp = rasterizationState.depthBiasClamp;
        mPipelineConfig.rasterizationState.depthBiasSlopeFactor = 0.f; rasterizationState.depthBiasSlopeFactor;
        mPipelineConfig.rasterizationState.lineWidth = rasterizationState.lineWidth;
        return this;
    }

    WannaVulkanPipeline *WannaVulkanPipeline::SetMultisampleState(VkSampleCountFlagBits samples, VkBool32 sampleShadingEnable, float minSampleShading = 0.f) {
        mPipelineConfig.multisampleState.rasterizationSamples = samples;
        mPipelineConfig.multisampleState.sampleShadingEnable = sampleShadingEnable;
        mPipelineConfig.multisampleState.minSampleShading = minSampleShading;
        return this;
    }
    
    WannaVulkanPipeline *WannaVulkanPipeline::SetDepthStencilState(const PipelineDepthStencilState &depthStencilState) {
        mPipelineConfig.depthStencilState.depthTestEnable = depthStencilState.depthTestEnable;
        mPipelineConfig.depthStencilState.depthWriteEnable = depthStencilState.depthWriteEnable;
        mPipelineConfig.depthStencilState.depthCompareOp = depthStencilState.depthCompareOp;
        mPipelineConfig.depthStencilState.depthBoundsTestEnable = depthStencilState.depthBoundsTestEnable;
        mPipelineConfig.depthStencilState.stencilTestEnable = depthStencilState.stencilTestEnable;
        return this;
    }

    WannaVulkanPipeline *WannaVulkanPipeline::SetColorBlendAttachmentState(VkBool32 blendEnable,
                                            VkBlendFactor srcColorBlendFactor = VK_BLEND_FACTOR_ONE, VkBlendFactor dstColorBlendFactor = VK_BLEND_FACTOR_ZERO, VkBlendOp colorBlendOp = VK_BLEND_OP_ADD,
                                            VkBlendFactor srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE, VkBlendFactor dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO, VkBlendOp alphaBlendOp = VK_BLEND_OP_ADD) {
        mPipelineConfig.colorBlendAttachmentState.blendEnable = blendEnable;
        mPipelineConfig.colorBlendAttachmentState.srcColorBlendFactor = srcColorBlendFactor;
        mPipelineConfig.colorBlendAttachmentState.dstColorBlendFactor = dstColorBlendFactor;
        mPipelineConfig.colorBlendAttachmentState.srcAlphaBlendFactor = srcAlphaBlendFactor;
        mPipelineConfig.colorBlendAttachmentState.dstAlphaBlendFactor = dstAlphaBlendFactor;
        mPipelineConfig.colorBlendAttachmentState.alphaBlendOp = alphaBlendOp;
        return this;
    }

    WannaVulkanPipeline *WannaVulkanPipeline::SetDynamicState(const std::vector<VkDynamicState> &dynamicStates) {
        mPipelineConfig.dynamicState.dynamicStates = dynamicStates;
        return this;
    }

    WannaVulkanPipeline *WannaVulkanPipeline::EnableAlphaBlend() {
        mPipelineConfig.colorBlendAttachmentState = {
            .blendEnable = VK_TRUE,
            // 启用以渲染半透明物体
            .srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
            .dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
            .colorBlendOp = VK_BLEND_OP_ADD,
            .srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
            .dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
            .alphaBlendOp = VK_BLEND_OP_ADD
        };
        return this;
    }

    WannaVulkanPipeline *WannaVulkanPipeline::EnableDepthTest() {
        mPipelineConfig.depthStencilState.depthTestEnable = VK_TRUE;
        mPipelineConfig.depthStencilState.depthWriteEnable = VK_TRUE;
        mPipelineConfig.depthStencilState.depthCompareOp = VK_COMPARE_OP_LESS;
        return this;
    }

    // 管线创建函数
    void WannaVulkanPipeline::create() {
        // 着色器创建参数
        VkPipelineShaderStageCreateInfo shaderStageInfo[] = {
            // 顶点着色器
            {
                .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                .pNext = nullptr,
                .flags = 0,
                .stage = VK_SHADER_STAGE_VERTEX_BIT,
                .module = mPipelineLayout->getVertexShaderMoudle(),
                .pName = "main",
                .pSpecializationInfo = nullptr
            },
            // 片元着色器
            {
                .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                .pNext = nullptr,
                .flags = 0,
                .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
                .module = mPipelineLayout->getFragShaderMoudle(),
                .pName = "main",
                .pSpecializationInfo = nullptr
            }
        };

        // 顶点输入创建参数
        VkPipelineVertexInputStateCreateInfo vertexInputStateInfo = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .vertexBindingDescriptionCount = static_cast<uint32_t>(mPipelineConfig.vertexInputState.vertexBindings.size()),
            .pVertexBindingDescriptions = mPipelineConfig.vertexInputState.vertexBindings.data(),
            .vertexAttributeDescriptionCount = static_cast<uint32_t>(mPipelineConfig.vertexInputState.vertexAttributes.size()),
            .pVertexAttributeDescriptions = mPipelineConfig.vertexInputState.vertexAttributes.data()
        };

        // 输入装配创建参数
        VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateInfo = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .topology = mPipelineConfig.inputAssemblyState.topology,
            .primitiveRestartEnable = mPipelineConfig.inputAssemblyState.primitiveRestartEnable
        };

        // 视口创建参数
        VkViewport defaultViewport = {
            .x = 0,
            .y = 0,
            .width = 100,
            .height = 100,
            .minDepth = 0,
            .maxDepth = 1
        };

        VkRect2D defaultScissor = {
            .offset = { 0, 0 },
            .extent = { 100, 100 }
        };

        VkPipelineViewportStateCreateInfo viewportStateInfo = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .viewportCount = 1,
            .pViewports = &defaultViewport,
            .scissorCount = 1,
            .pScissors = &defaultScissor
        };
        
        // 光栅化创建参数
        VkPipelineRasterizationStateCreateInfo rasterizationStateInfo = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .depthClampEnable = mPipelineConfig.rasterizationState.depthClampEnable,
            .rasterizerDiscardEnable = mPipelineConfig.rasterizationState.rasterizerDiscardEnable,
            .polygonMode = mPipelineConfig.rasterizationState.polygonMode,
            .cullMode = mPipelineConfig.rasterizationState.cullMode,
            .frontFace = mPipelineConfig.rasterizationState.frontFace,
            .depthBiasEnable = mPipelineConfig.rasterizationState.depthBiasEnable,
            .depthBiasConstantFactor = mPipelineConfig.rasterizationState.depthBiasConstantFactor,
            .depthBiasClamp = mPipelineConfig.rasterizationState.depthBiasClamp,
            .depthBiasSlopeFactor = mPipelineConfig.rasterizationState.depthBiasSlopeFactor,
            .lineWidth = mPipelineConfig.rasterizationState.lineWidth
        };

        // 图形管线创建参数
        VkGraphicsPipelineCreateInfo pipelineInfo = {
            .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .stageCount = ARRAY_SIZE(shaderStageInfo),
            .pStages = shaderStageInfo,
            .pVertexInputState = &vertexInputStateInfo,
            .pInputAssemblyState = &inputAssemblyStateInfo,
            .pTessellationState = nullptr, // 曲面细分默认不设置
            .pViewportState = &viewportStateInfo,
            .pRasterizationState = &rasterizationStateInfo,

        };
        CALL_VK(vkCreateGraphicsPipelines(mDevice->getHandle(), mDevice->getPipelineCache(), 1, &pipelineInfo, nullptr ,&mHandle));

    }

}