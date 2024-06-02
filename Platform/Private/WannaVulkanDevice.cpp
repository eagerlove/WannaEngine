#include "WannaVulkanDevice.h"
#include "WannaVulkanContext.h"
#include "WannaVulkanQueue.h"

namespace WannaEngine {

    // 交换链拓展
    const DeviceFeature requestedExtensions[] = {
        { VK_KHR_SWAPCHAIN_EXTENSION_NAME, true },
#ifdef WANNA_ENGINE_PLATFORM_WIN32
#elif WANNA_ENGINE_PLATFORM_MACOS
        { "VK_KHR_protability_subset", true },
#elif WANNA_ENGINE_PLATFORM_LINUX
#endif
    };

    // 构造函数
    WannaVulkanDevice::WannaVulkanDevice(WannaVulkanContext *context, uint32_t graphicQueueCount, uint32_t presentQueueCount, const WannaVulkanSetting &settings) 
        : mySettings(settings){
        if (!context) {
            LOG_ERROR("Must create a Vulkan graphic context before create device.");
            exit(EXIT_FAILURE);
        }

        // 错误处理 防止队列数量不一致
        QueueFamilyInfo graphicQueueInfo = context->GetGraphicQueueFamilyInfo();
        QueueFamilyInfo presentQueueInfo = context->GetPresentQueueFamilyInfo();
        if (graphicQueueCount > graphicQueueInfo.queueCount ) {
            LOG_ERROR("This queue family has {0} queue, but request {1}", graphicQueueInfo.queueCount, graphicQueueCount);
            exit(EXIT_FAILURE);
        }
        if (presentQueueCount > presentQueueInfo.queueCount) {
            LOG_ERROR("This queue family has {0} queue, but request {1}", presentQueueInfo.queueCount, presentQueueCount);
            exit(EXIT_FAILURE);
        }

        // 队列优先级 显示队列优先级更高
        std::vector<float> graphicQueuePriorities(graphicQueueCount, 0.0f);
        std::vector<float> presentQueuePriorities(presentQueueCount, 1.0f);

        // 判断是否为同一队列
        bool bSameQueueFamilyIndex = context->IsSameGraphicPresentQueueFamily();
        // 队列数量
        uint32_t sameQueueCount = graphicQueueCount;
        if (bSameQueueFamilyIndex) {
            sameQueueCount += presentQueueCount;
            if (sameQueueCount > graphicQueueInfo.queueCount) {
                sameQueueCount = graphicQueueInfo.queueCount;
            }
            graphicQueuePriorities.insert(graphicQueuePriorities.end(), presentQueuePriorities.begin(), presentQueuePriorities.end());
        }

        // 显示队列与图形队列一致时使用queueInfos[0]
        VkDeviceQueueCreateInfo queueInfos[2] = {
            {
                .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
                .pNext = nullptr,
                .flags = 0,
                .queueFamilyIndex = static_cast<uint32_t>(graphicQueueInfo.queueFamilyIndex),
                .queueCount = sameQueueCount,
                .pQueuePriorities = graphicQueuePriorities.data(),
            },
        };

        // 显示队列与图形队列不一致时使用queueInfos[1]
        if (!bSameQueueFamilyIndex) {
            queueInfos[1] = {
                .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
                .pNext = nullptr,
                .flags = 0,
                .queueFamilyIndex = static_cast<uint32_t>(presentQueueInfo.queueFamilyIndex),
                .queueCount = presentQueueCount,
                .pQueuePriorities = presentQueuePriorities.data(),
            };
        }

        // 拓展数量及名称
        uint32_t availableExtensionCount;
        CALL_VK(vkEnumerateDeviceExtensionProperties(context->GetPhysicalDevice(), "", &availableExtensionCount, nullptr));
        VkExtensionProperties availableExtensions[availableExtensionCount];
        CALL_VK(vkEnumerateDeviceExtensionProperties(context->GetPhysicalDevice(), "", &availableExtensionCount, availableExtensions));
        uint32_t enableExtensionCount;
        const char *enableExtensions[32];
        if(!findDeviceFeatures("Device Extension", true, availableExtensionCount, availableExtensions,
                                ARRAY_SIZE(requestedExtensions), requestedExtensions, &enableExtensionCount, enableExtensions)) {
            exit(EXIT_FAILURE);
        }

        // 逻辑设备创建信息
        VkDeviceCreateInfo deviceInfo {
            .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .queueCreateInfoCount = static_cast<uint32_t>(bSameQueueFamilyIndex ? 1 : 2),
            .pQueueCreateInfos = queueInfos,
            .enabledLayerCount = 0,
            .ppEnabledLayerNames = nullptr,
            .enabledExtensionCount = enableExtensionCount,
            .ppEnabledExtensionNames = enableExtensionCount > 0 ? enableExtensions : nullptr,
            .pEnabledFeatures = nullptr
        };
        
        CALL_VK(vkCreateDevice(context->GetPhysicalDevice(), &deviceInfo, nullptr, &mHandle));
        LOG_INFO("VkDevice: {0}", (void*)mHandle);

        // 获取所有图形队列
        for (int i = 0; i < graphicQueueCount; i++) {
            VkQueue queue;
            vkGetDeviceQueue(mHandle, graphicQueueInfo.queueFamilyIndex, i, &queue);
            myGraphicQueues.push_back(std::make_shared<WannaVulkanQueue>(graphicQueueInfo.queueFamilyIndex, i, queue, false));
        }

        // 获取所有显示队列
        for (int j = 0; j < presentQueueCount; j++) {
            VkQueue queue;
            vkGetDeviceQueue(mHandle, presentQueueInfo.queueFamilyIndex, j, &queue);
            myPresentQueues.push_back(std::make_shared<WannaVulkanQueue>(presentQueueInfo.queueFamilyIndex, j, queue, true));
        }
    }

    // 析构函数
    WannaVulkanDevice::~WannaVulkanDevice() {
        vkDeviceWaitIdle(mHandle);
        vkDestroyDevice(mHandle, nullptr);
    }
}