#include <iostream>
#include "WannaLog.h"
#include "WannaWindow.h"
#include "WannaGraphicContext.h"
#include "WannaVulkanContext.h"
#include "WannaVulkanDevice.h"
#include "WannaVulkanSwapChain.h"
#include "WannaVulkanQueue.h"
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
    pipeline->SetInputAssemblyState(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)->EnableDepthTest();
    pipeline->SetDynamicState({ VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR });
    pipeline->create();

    std::shared_ptr<WannaEngine::WannaVulkanCommandPool> commandPool = std::make_shared<WannaEngine::WannaVulkanCommandPool>(device.get(), vkContext->GetGraphicQueueFamilyInfo().queueFamilyIndex);
    std::vector<VkCommandBuffer> commandBuffers = commandPool->AllocateCommandBuffers(swapChainImages.size());

    WannaEngine::WannaVulkanQueue *graphicQueue = device->GetFirstGraphicQueue();

    const std::vector<VkClearValue> clearValues = {
        {0.1f, 0.2f, 0.3f, 1.f}
    };

    // 信号量 目前需要两个：1.交换链图像 2.提交队列信号量
    const uint32_t numBuffer = 2;
    std::vector<VkSemaphore> imageAvaiableSemaphores(numBuffer);
    std::vector<VkSemaphore> submitedSemaphores(numBuffer);
    // 栅栏
    std::vector<VkFence> frameFences(numBuffer);

    VkSemaphoreCreateInfo semaphoreInfo = {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0
    };
    VkFenceCreateInfo fenceInfo = {
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        .pNext = nullptr,
        .flags = VK_FENCE_CREATE_SIGNALED_BIT
    };

    // 创建信号量和栅栏
    for (int i = 0; i < numBuffer; i++) {
        CALL_VK(vkCreateSemaphore(device->getHandle(), &semaphoreInfo, nullptr, &imageAvaiableSemaphores[i]));
        CALL_VK(vkCreateSemaphore(device->getHandle(), &semaphoreInfo, nullptr, &submitedSemaphores[i]));
        CALL_VK(vkCreateFence(device->getHandle(), &fenceInfo, nullptr, &frameFences[i]));
    }

    uint32_t currentBuffer = 0;
    while (!win->CLOSE())
    {
        win->PollEvent();

        CALL_VK(vkWaitForFences(device->getHandle(), 1, &frameFences[currentBuffer], VK_FALSE, UINT64_MAX));
        CALL_VK(vkResetFences(device->getHandle(), 1, &frameFences[currentBuffer]));

        // 1. 获取交换链图像
        int32_t imageIndex = swapChain->AcquireImage(imageAvaiableSemaphores[currentBuffer]);

        // 2. 开启命令缓冲
        WannaEngine::WannaVulkanCommandPool::BeginCommandBuffer(commandBuffers[imageIndex]);

        // 3. 开启渲染流程并绑定帧缓冲
        renderPass->BeginRenderPass(commandBuffers[imageIndex], frameBuffers[imageIndex].get(), clearValues);

        // 4. 绑定资源
        pipeline->bind(commandBuffers[imageIndex]);

        VkViewport viewPort = {
            .x = 0,
            .y = 0,
            .width = static_cast<float>(frameBuffers[imageIndex]->getWidth()),
            .height = static_cast<float>(frameBuffers[imageIndex]->getHeight()),
        };
        vkCmdSetViewport(commandBuffers[imageIndex], 0, 1, &viewPort);
        VkRect2D scissor = {
            .offset = { 0, 0 },
            .extent = { frameBuffers[imageIndex]->getWidth(), frameBuffers[imageIndex]->getHeight() }
        };
        vkCmdSetScissor(commandBuffers[imageIndex], 0, 1, &scissor);

        // 5. 绘制
        vkCmdDraw(commandBuffers[imageIndex], 3, 1, 0, 0);

        // 6. 结束渲染流程
        renderPass->EndRenderPass(commandBuffers[imageIndex]);

        // 7. 结束命令缓冲
        WannaEngine::WannaVulkanCommandPool::EndCommandBuffer(commandBuffers[imageIndex]);

        // 8. 提交缓冲
        graphicQueue->submit({ commandBuffers[imageIndex]}, { imageAvaiableSemaphores[currentBuffer] }, { submitedSemaphores[currentBuffer] }, frameFences[currentBuffer]);

        // 9. 显示
        swapChain->Present(imageIndex, { submitedSemaphores[currentBuffer] });

        win->SwapBuffer();
        currentBuffer = (currentBuffer + 1) % numBuffer;
    }

    // 程序结束 销毁信号量和栅栏
    for (int i = 0; i < numBuffer; i++) {
        vkDeviceWaitIdle(device->getHandle()); // 等待逻辑设备处理完成
        VK_DESTROY(Semaphore, device->getHandle(), imageAvaiableSemaphores[i]);
        VK_DESTROY(Semaphore, device->getHandle(), submitedSemaphores[i]);
        VK_DESTROY(Fence, device->getHandle(), frameFences[i]);
    }

    return EXIT_SUCCESS;
}
