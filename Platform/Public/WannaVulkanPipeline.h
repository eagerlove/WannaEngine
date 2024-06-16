#ifndef WANNA_VULKAN_PIPELINT_H
#define WANNA_VULKAN_PIPELINE_H

#include "WannaVulkanCommon.h"

namespace WannaEngine {
    class WannaVulkanDevice;
    class WannaVulkanRenderPass;

    // 着色器布局
    struct ShaderLayout {
        std::vector<VkDescriptorSetLayout> descriptorSetLayouts;
        std::vector<VkPushConstantRange> pushConstants;
    };

    // 管线顶点输入
    struct PipelineVertexInputState {
        std::vector<VkVertexInputBindingDescription> vertexBindings;
        std::vector<VkVertexInputAttributeDescription> vertexAttributes;
    };

    // 管线输入装配
    struct PipelineInputAssemblyState {
        VkPrimitiveTopology topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST; // 默认图元：三角形
        VkBool32 primitiveRestartEnable = VK_FALSE; // 是否启用几何图元重启
    };

    // 管线光栅化
    struct PipelineRasterizationState {
        VkBool32        depthClampEnable = VK_FALSE; // 是否启用深度夹
        VkBool32        rasterizerDiscardEnable = VK_FALSE; // 栅格化后是否丢弃基元
        VkPolygonMode   polygonMode = VK_POLYGON_MODE_FILL; // 填充渲染
        VkCullModeFlags cullMode = VK_CULL_MODE_NONE; // 不作剔除
        VkFrontFace     frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE; // 面向前面的三角形方向
        VkBool32        depthBiasEnable = VK_FALSE; // 是否启用片段深度值偏置
        float           depthBiasConstantFactor = 0.f; // 添加到每个片段的恒定深度值
        float           depthBiasClamp = 0.f; // 片段最大(或最小)的深度偏差
        float           depthBiasSlopeFactor = 0.f; // 片段斜率的标量因子
        float           lineWidth = 1.f; // 栅格化线段的宽度
    };

    // 管线多重采样
    struct PipelineMultisampleState {
        VkSampleCountFlagBits rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        VkBool32 sampleShadingEnable = VK_FALSE;
        float minSampleShading = 0.2f;
    };

    // 管线深度模板 默认不开启
    struct PipelineDepthStencilState {
        VkBool32 depthTestEnable = VK_FALSE;
        VkBool32 depthWriteEnable = VK_FALSE;
        VkCompareOp depthCompareOp = VK_COMPARE_OP_NEVER;
        VkBool32 depthBoundsTestEnable = VK_FALSE;
        VkBool32 stencilTestEnable = VK_FALSE;
    };

    // 管线动态参数
    struct PipelineDynamicState {
        std::vector<VkDynamicState> dynamicStates;
    };

    // 管线配置参数
    struct PipelineConfig {
        PipelineVertexInputState vertexInputState;
        PipelineInputAssemblyState inputAssemblyState;
        PipelineRasterizationState rasterizationState;
        PipelineMultisampleState multisampleState;
        PipelineDepthStencilState depthStencilState;
        VkPipelineColorBlendAttachmentState colorBlendAttachmentState;
        PipelineDynamicState dynamicState;
    };

    // 管线布局
    class WannaVulkanPipelineLayout {
        public:
            WannaVulkanPipelineLayout(WannaVulkanDevice *device, const std::string &vertexShaderFile, const std::string &fragShaderFile, const ShaderLayout &shaderLayout = {});
            ~WannaVulkanPipelineLayout();

            VkPipelineLayout getHandle() const { return mHandle; }
            VkShaderModule getVertexShaderMoudle() const { return mVertexShaderModule; }
            VkShaderModule getFragShaderMoudle() const { return mFragShaderModule; }

        private:
            WannaVulkanDevice *mDevice;

            VkShaderModule mVertexShaderModule = VK_NULL_HANDLE;
            VkShaderModule mFragShaderModule = VK_NULL_HANDLE;
            VkPipelineLayout mHandle = VK_NULL_HANDLE;

            VkResult createShaderModule(const std::string &filePath, VkShaderModule *shaderMoudle);
    };

    // 管线
    class WannaVulkanPipeline {
        public:
            WannaVulkanPipeline(WannaVulkanDevice *device, WannaVulkanRenderPass *renderPass, WannaVulkanPipelineLayout *pipelineLayout);
            ~WannaVulkanPipeline();

            WannaVulkanPipeline *SetVertexInputState(const std::vector<VkVertexInputBindingDescription> &vertexBindings, const std::vector<VkVertexInputAttributeDescription> &vertexAttrs);
            WannaVulkanPipeline *SetInputAssemblyState(VkPrimitiveTopology topology, VkBool32 primitiveRestartEnable = VK_FALSE);
            WannaVulkanPipeline *SetRasterizationState(const PipelineRasterizationState &rasterizationState);
            WannaVulkanPipeline *SetMultisampleState(VkSampleCountFlagBits samples, VkBool32 sampleShadingEnable, float minSampleShading = 0.f);
            WannaVulkanPipeline *SetDepthStencilState(const PipelineDepthStencilState &depthStencilState);
            WannaVulkanPipeline *SetColorBlendAttachmentState(VkBool32 blendEnable,
                                                    VkBlendFactor srcColorBlendFactor = VK_BLEND_FACTOR_ONE, VkBlendFactor dstColorBlendFactor = VK_BLEND_FACTOR_ZERO, VkBlendOp colorBlendOp = VK_BLEND_OP_ADD,
                                                    VkBlendFactor srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE, VkBlendFactor dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO, VkBlendOp alphaBlendOp = VK_BLEND_OP_ADD);
            WannaVulkanPipeline *SetDynamicState(const std::vector<VkDynamicState> &dynamicStates);
            WannaVulkanPipeline *EnableAlphaBlend(); // 开启混合
            WannaVulkanPipeline *EnableDepthTest(); // 启用深度测试

            void create();

            VkPipeline getHandle() const { return mHandle; }
        private:
            WannaVulkanDevice *mDevice;
            WannaVulkanRenderPass *mRenderPass;
            WannaVulkanPipelineLayout *mPipelineLayout;

            VkPipeline mHandle = VK_NULL_HANDLE;

            PipelineConfig mPipelineConfig;

    };
}


#endif