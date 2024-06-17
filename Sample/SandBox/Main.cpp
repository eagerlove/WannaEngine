#include <iostream>
#include "WannaLog.h"
#include "WannaWindow.h"
#include "WannaGraphicContext.h"
#include "WannaVulkanContext.h"
#include "WannaVulkanDevice.h"
#include "WannaVulkanSwapChain.h"
#include "WannaVulkanRenderPass.h"
#include "WannaVulkanFrameBuffer.h"
#include "WannaVulkanPipeline.h"
#include "WannaVulkanFileUtil.h"
#include "WannaVulkanCommandBuffer.h"

int main() {
    std::cout << "Hello Wanna Engine!" << std::endl;

    WannaEngine::WannaLog::Init();
    LOG_TRACE("Hello spdlog: {0}, {1}, {3}", __FUNCTION__, 1, 0.14f, true);
    LOG_DEBUG("Hello spdlog: {0}, {1}, {3}", __FUNCTION__, 1, 0.14f, true);
    LOG_INFO("Hello spdlog: {0}, {1}, {3}", __FUNCTION__, 1, 0.14f, true);
    LOG_WARNING("Hello spdlog: {0}, {1}, {3}", __FUNCTION__, 1, 0.14f, true);
    LOG_ERROR("Hello spdlog: {0}, {1}, {3}", __FUNCTION__, 1, 0.14f, true);

    std::unique_ptr<WannaEngine::WannaWindow> win = WannaEngine::WannaWindow::Create(640, 480, "I WANNA");
    std::unique_ptr<WannaEngine::WannaGraphicContext> graphicContext = WannaEngine::WannaGraphicContext::Create(win.get());
    auto vkContext = dynamic_cast<WannaEngine::WannaVulkanContext*>(graphicContext.get());
    std::shared_ptr<WannaEngine::WannaVulkanDevice> device = std::make_shared<WannaEngine::WannaVulkanDevice>(vkContext, 1, 1);
    std::shared_ptr<WannaEngine::WannaVulkanSwapChain> swapChain = std::make_shared<WannaEngine::WannaVulkanSwapChain>(vkContext, device.get());
    swapChain->ReCreate();
    std::shared_ptr<WannaEngine::WannaVulkanRenderPass> renderPass = std::make_shared<WannaEngine::WannaVulkanRenderPass>(device.get());
    
    std::vector<VkImage> swapChainImages = swapChain->getImages();
    std::vector<std::shared_ptr<WannaEngine::WannaVulkanFrameBuffer>> frameBuffers;
    for (const auto &image: swapChainImages) {
        std::vector<VkImage> images = { image };
        frameBuffers.push_back(std::make_shared<WannaEngine::WannaVulkanFrameBuffer>(device.get(), renderPass.get(), images, swapChain->getWidth(), swapChain->getHeight()));
    }

    std::shared_ptr<WannaEngine::WannaVulkanPipelineLayout> pipelineLayout = std::make_shared<WannaEngine::WannaVulkanPipelineLayout>(device.get(), 
                                                                                                                                      WAN_RES_SHADER_DIR"shader.vert",
                                                                                                                                      WAN_RES_SHADER_DIR"shader.frag");
    std::shared_ptr<WannaEngine::WannaVulkanPipeline> pipeline = std::make_shared<WannaEngine::WannaVulkanPipeline>(device.get(), renderPass.get(), pipelineLayout.get());
    pipeline->SetInputAssemblyState(VK_PRIMITIVE_TOPOLOGY_LINE_LIST)->EnableDepthTest();
    pipeline->SetDynamicState({ VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR });
    pipeline->create();

    std::shared_ptr<WannaEngine::WannaVulkanCommandPool> commandPool = std::make_shared<WannaEngine::WannaVulkanCommandPool>(device.get(), vkContext->GetGraphicQueueFamilyInfo().queueFamilyIndex);
    std::vector<VkCommandBuffer> commandBuffers = commandPool->AllocateCommandBuffers(swapChainImages.size());

    while (!win->CLOSE())
    {
        win->PollEvent();

        // 1. 获取交换链图像
        int32_t imageIndex = swapChain->AcquireImage();

        // 2. 开启命令缓冲
        // 3. 开启渲染流程并绑定帧缓冲
        // 4. 绑定资源
        // 5. 绘制
        // 6. 结束渲染流程
        // 7. 结束命令缓冲
        // 8. 提交缓冲
        // 9. 显示
        swapChain->Present(imageIndex);

        win->SwapBuffer();
    }
    
    return EXIT_SUCCESS;
}
