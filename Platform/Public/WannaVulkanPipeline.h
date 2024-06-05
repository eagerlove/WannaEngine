#ifndef WANNA_VULKAN_PIPELINT_H
#define WANNA_VULKAN_PIPELINE_H

#include "WannaVulkanCommon.h"

namespace WannaEngine {
    class WannaVulkanDevice;

    struct ShaderLayout {
        std::vector<VkDescriptorSetLayout> descriptorSetLayouts;
        std::vector<VkPushConstantRange> pushConstants;
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
            

    };
}


#endif