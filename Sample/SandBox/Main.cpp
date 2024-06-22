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
#include "WannaGeometryUtil.h"
#include "WannaVulkanImage.h"
#include "WannaVulkanBuffer.h"
#include "WannaVulkanCommandBuffer.h"

// 旋转矩阵
struct PushConstants{
    glm::mat4 matrix{ 1.f };
};

int main() {
    std::cout << "Hello Wanna Engine!" << std::endl;

    WannaEngine::WannaLog::Init();
    LOG_TRACE("Hello spdlog: {0}, {1}, {3}", __FUNCTION__, 1, 0.14f, true);
    LOG_DEBUG("Hello spdlog: {0}, {1}, {3}", __FUNCTION__, 1, 0.14f, true);
    LOG_INFO("Hello spdlog: {0}, {1}, {3}", __FUNCTION__, 1, 0.14f, true);
    LOG_WARNING("Hello spdlog: {0}, {1}, {3}", __FUNCTION__, 1, 0.14f, true);
    LOG_ERROR("Hello spdlog: {0}, {1}, {3}", __FUNCTION__, 1, 0.14f, true);

    std::unique_ptr<WannaEngine::WannaWindow> win = WannaEngine::WannaWindow::Create(640, 480, "Wanna Engine");
    std::unique_ptr<WannaEngine::WannaGraphicContext> graphicContext = WannaEngine::WannaGraphicContext::Create(win.get());
    auto vkContext = dynamic_cast<WannaEngine::WannaVulkanContext*>(graphicContext.get());
    std::shared_ptr<WannaEngine::WannaVulkanDevice> device = std::make_shared<WannaEngine::WannaVulkanDevice>(vkContext, 1, 1);
    std::shared_ptr<WannaEngine::WannaVulkanSwapChain> swapChain = std::make_shared<WannaEngine::WannaVulkanSwapChain>(vkContext, device.get());
    swapChain->ReCreate();

    // 深度格式
    VkFormat depthFormat = VK_FORMAT_D32_SFLOAT;

    // 渲染流程附件
    std::vector<VkAttachmentDescription> attachments = {
        // 颜色附件
        {
            .flags = 0,
            .format = device->getSettings().surfaceFormat, // 格式
            .samples = VK_SAMPLE_COUNT_1_BIT, // 采样一次
            .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR, // 使用一个常量值来清除附着的内容
            .storeOp = VK_ATTACHMENT_STORE_OP_STORE, // 渲染的内容会被存储起来，以便之后读取
            .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE, // 不关心附着现存的内容
            .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE, // 渲染后，不会读取帧缓冲的内容
            .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED, // 不关心之前的图像布局方式
            .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR // 设置图像用于在交换链中进行呈现操作
        },

        // 深度模板附件
        {
            .flags = 0,
            .format = depthFormat, // 格式
            .samples = VK_SAMPLE_COUNT_1_BIT, // 采样一次
            .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR, // 使用一个常量值来清除附着的内容
            .storeOp = VK_ATTACHMENT_STORE_OP_STORE, // 渲染的内容会被存储起来，以便之后读取
            .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE, // 不关心附着现存的内容
            .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE, // 渲染后，不会读取帧缓冲的内容
            .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED, // 不关心之前的图像布局方式
            .finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL // 指定深度模板格式图像的布局
        }
    };

    // 绑定附件到子流程
    std::vector<WannaEngine::RenderSubPass> subpasses = {
        {
            .colorAttachment = { 0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL },
            .depthStencilAttachment = { 1, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL }
        }
    };

    // 创建渲染流程
    std::shared_ptr<WannaEngine::WannaVulkanRenderPass> renderPass = std::make_shared<WannaEngine::WannaVulkanRenderPass>(device.get(), attachments, subpasses);

    // 交换链图像配置
    std::vector<VkImage> swapChainImages = swapChain->getImages();
    uint32_t swapChainImageSize = swapChainImages.size();
    std::vector<std::shared_ptr<WannaEngine::WannaVulkanFrameBuffer>> frameBuffers;

    VkExtent3D imageEvent = { swapChain->getWidth(), swapChain->getHeight(), 1 }; // 图像区域
    for (int i = 0; i < swapChainImageSize; i++) {
        std::vector<std::shared_ptr<WannaEngine::WannaVulkanImage>> images = {
            std::make_shared<WannaEngine::WannaVulkanImage>(device.get(), swapChainImages[i], imageEvent, device->getSettings().surfaceFormat, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT),
            std::make_shared<WannaEngine::WannaVulkanImage>(device.get(), imageEvent, depthFormat, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT)
        };
        frameBuffers.push_back(std::make_shared<WannaEngine::WannaVulkanFrameBuffer>(device.get(), renderPass.get(), images, swapChain->getWidth(), swapChain->getHeight()));
    }

    // 渲染管线配置
    WannaEngine::ShaderLayout shaderLayout = { // 着色器布局
        .pushConstants = {
            {
                .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
                .offset = 0,
                .size = sizeof(PushConstants)
            }
        }
    };

    std::shared_ptr<WannaEngine::WannaVulkanPipelineLayout> pipelineLayout = std::make_shared<WannaEngine::WannaVulkanPipelineLayout>(device.get(), 
                                                                                                                                      WAN_RES_SHADER_DIR"shader.vert",
                                                                                                                                      WAN_RES_SHADER_DIR"shader.frag",
                                                                                                                                      shaderLayout);
    std::shared_ptr<WannaEngine::WannaVulkanPipeline> pipeline = std::make_shared<WannaEngine::WannaVulkanPipeline>(device.get(), renderPass.get(), pipelineLayout.get());
    std::vector<VkVertexInputBindingDescription> vertexBindings = {
        {
            .binding = 0,
            .stride = sizeof(WannaEngine::WannaVertex),
            .inputRate = VK_VERTEX_INPUT_RATE_VERTEX
        }
    };
    std::vector<VkVertexInputAttributeDescription> vertexAttrs = {
        {
            .location = 0,
            .binding = 0,
            .format = VK_FORMAT_R32G32B32_SFLOAT,
            .offset = offsetof(WannaEngine::WannaVertex, position)
        },
        {
            .location = 1,
            .binding = 0,
            .format = VK_FORMAT_R32G32_SFLOAT,
            .offset = offsetof(WannaEngine::WannaVertex, texcoord0)
        },
        {
            .location = 2,
            .binding = 0,
            .format = VK_FORMAT_R32G32B32_SFLOAT,
            .offset = offsetof(WannaEngine::WannaVertex, normal)
        },
    };
    pipeline->SetVertexInputState(vertexBindings, vertexAttrs); // 设置顶点输入绑定状态和输入属性
    pipeline->SetInputAssemblyState(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);//->EnableDepthTest(); // 开启深度测试
    pipeline->SetDynamicState({ VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR }); // 设置动态视口和动态裁剪
    pipeline->create(); // 创建渲染管线

    // 命令队列配置
    std::shared_ptr<WannaEngine::WannaVulkanCommandPool> commandPool = std::make_shared<WannaEngine::WannaVulkanCommandPool>(device.get(), vkContext->GetGraphicQueueFamilyInfo().queueFamilyIndex);
    std::vector<VkCommandBuffer> commandBuffers = commandPool->AllocateCommandBuffer(swapChainImages.size());

    // 几何体数据
    std::vector<WannaEngine::WannaVertex> vertices;
    std::vector<uint32_t> indices;
    WannaEngine::WannaGeometryUtil::CreateCube(-0.3f, 0.3f, -0.3f, 0.3f, -0.3f, 0.3f, vertices, indices);
    std::shared_ptr<WannaEngine::WannaVulkanBuffer> vertexBuffer = std::make_shared<WannaEngine::WannaVulkanBuffer>(device.get(), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                                                                                                                    sizeof(vertices[0]) * vertices.size(), vertices.data()); // 顶点数据缓冲
    std::shared_ptr<WannaEngine::WannaVulkanBuffer> indexBuffer = std::make_shared<WannaEngine::WannaVulkanBuffer>(device.get(), VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                                                                                                                    sizeof(indices[0]) * indices.size(), indices.data()); // 顶点索引数据缓冲
    PushConstants pc{}; // 输入常量
    WannaEngine::WannaVulkanQueue *graphicQueue = device->GetFirstGraphicQueue();
    const std::vector<VkClearValue> clearValues = { // 颜色附件和深度附件的清空值
        { .color = { 0.1f, 0.2f, 0.3f, 1.f }}, { .depthStencil = {1, 0} }
    };
    

    // 信号量 目前需要两个：1.交换链图像读入信号 2.提交队列信号
    const uint32_t numBuffer = 2;
    std::vector<VkSemaphore> imageAvaiableSemaphores(numBuffer);
    std::vector<VkSemaphore> submitedSemaphores(numBuffer);
    VkSemaphoreCreateInfo semaphoreInfo = {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0
    };

    // 栅栏
    std::vector<VkFence> frameFences(numBuffer);
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

    std::chrono::time_point lastTimePoint = std::chrono::steady_clock::now(); // 起始时间点

    uint32_t currentBuffer = 0; // 当前缓冲索引值
    // 渲染主循环
    while (!win->CLOSE())
    {
        win->PollEvent();

        CALL_VK(vkWaitForFences(device->getHandle(), 1, &frameFences[currentBuffer], VK_TRUE, UINT64_MAX));
        CALL_VK(vkResetFences(device->getHandle(), 1, &frameFences[currentBuffer]));

        // 1. 获取交换链图像
        int32_t imageIndex = swapChain->AcquireImage(imageAvaiableSemaphores[currentBuffer]);

        float time = std::chrono::duration<float>(std::chrono::steady_clock::now() - lastTimePoint).count();// 计时
        pc.matrix = glm::rotate(glm::mat4(1.f), glm::radians(-20.f), glm::vec3(1, 0, 0)); // 绕x轴旋转-17度
        pc.matrix = glm::rotate(pc.matrix, glm::radians(time * 100.f), glm::vec3(0, 1, 0)); // 随时间绕y轴旋转

        pc.matrix = glm::ortho(-1.f, 1.f, -1.f, 1.f, -1.f, 1.f) * pc.matrix; // 正交投影

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
            .minDepth = 0.f,
            .maxDepth = 1.f
        };
        vkCmdSetViewport(commandBuffers[imageIndex], 0, 1, &viewPort);
        VkRect2D scissor = {
            .offset = { 0, 0 },
            .extent = { frameBuffers[imageIndex]->getWidth(), frameBuffers[imageIndex]->getHeight() }
        };
        vkCmdSetScissor(commandBuffers[imageIndex], 0, 1, &scissor);
        vkCmdPushConstants(commandBuffers[imageIndex], pipelineLayout->getHandle(), VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(pc), &pc);

        // 绑定缓冲区
        VkBuffer vertexBuffers[] = { vertexBuffer->getHandle() };
        VkDeviceSize offsets[] = { 0 };
        vkCmdBindVertexBuffers(commandBuffers[imageIndex], 0, 1, vertexBuffers, offsets);
        vkCmdBindIndexBuffer(commandBuffers[imageIndex], indexBuffer->getHandle(), 0, VK_INDEX_TYPE_UINT32);

        // 5. 绘制
        // vkCmdDraw(commandBuffers[imageIndex], 3, 1, 0, 0);
        vkCmdDrawIndexed(commandBuffers[imageIndex], indices.size(), 1, 0, 0, 0); // 按顶点绘制

        // 6. 结束渲染流程
        renderPass->EndRenderPass(commandBuffers[imageIndex]);

        // 7. 结束命令缓冲
        WannaEngine::WannaVulkanCommandPool::EndCommandBuffer(commandBuffers[imageIndex]);

        // 8. 提交缓冲
        graphicQueue->submit({ commandBuffers[imageIndex] }, { imageAvaiableSemaphores[currentBuffer] }, { submitedSemaphores[currentBuffer] }, frameFences[currentBuffer]);

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
