#include "WannaVulkanPipeline.h"
#include "WannaVulkanFileUtil.h"
#include "WannaVulkanDevice.h"


namespace WannaEngine {
    WannaVulkanPipelineLayout::WannaVulkanPipelineLayout(WannaVulkanDevice *device, const std::string &vertexShaderFile, const std::string &fragShaderFile, const ShaderLayout &shaderLayout) {
        // 编译着色器
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
}