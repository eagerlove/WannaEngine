#include <iostream>
#include "WannaLog.h"
#include "WannaWindow.h"
#include "WannaGraphicContext.h"
#include "WannaVulkanContext.h"
#include "WannaVulkanDevice.h"
#include "WannaVulkanSwapChain.h"
#include "WannaVulkanRenderPass.h"


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
    while (!win->CLOSE())
    {
        win->PollEvent();
        win->SwapBuffer();
    }
    
    return EXIT_SUCCESS;
}
