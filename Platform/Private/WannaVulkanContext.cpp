#include "WannaVulkanContext.h"
#include "WannaGlfwWindow.h"

namespace WannaEngine {
    // 所需验证层
    const DeviceFeature requestedLayers[] = {
        { "VK_LAYER_KHRONOS_validation", true },
    };

    // 所需拓展
    const DeviceFeature requestedExtensions[] = {
        { VK_KHR_SURFACE_EXTENSION_NAME, true },

#ifdef WANNA_ENGINE_PLATFORM_WIN32
        { VK_KHR_WIN32_SURFACE_EXTENSION_NAME, true },
#elif WANNA_ENGINE_PLATFORM_MACOS
        { VK_MVK_MACOS_SURFACE_EXTENSION_NAME, true },
#elif WANNA_ENGINE_PLATFORM_LINUX
        { VK_KHR_XCB_SURFACE_EXTENSION_NAME, true },
#endif

    };

    // 构造函数
    WannaVulkanContext::WannaVulkanContext(WannaWindow* win) {
        CreateInstance();
        CreateSurface(win);
        SelectPhysicalDevice();
    }

    // 析构函数
    WannaVulkanContext::~WannaVulkanContext() {
        vkDestroySurfaceKHR(mInstance, mSurface, nullptr);
        vkDestroyInstance(mInstance, nullptr);
    }

    // 错误报告
    static VkBool32 VkDebugReportCallback(VkDebugReportFlagsEXT     flags,
                                      VkDebugReportObjectTypeEXT    objectType,
                                      uint64_t                      object,
                                      size_t                        location,
                                      int32_t                       messageCode,
                                      const char*                   pLayerPrefix,
                                      const char*                   pMessage,
                                      void*                         pUserData) {
        if (flags & VK_DEBUG_REPORT_ERROR_BIT_EXT) {
            LOG_ERROR("{0}", pMessage);
        }
        if (flags & VK_DEBUG_REPORT_WARNING_BIT_EXT || flags & VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT) {
            LOG_WARNING("{0}", pMessage);
        }
        return VK_TRUE;
    }

    // 创建实例
    void WannaVulkanContext::CreateInstance() {

        // 1. 构建layers 拿到当前设备所支持的层
        uint32_t availableLayerCount = 0;
        CALL_VK(vkEnumerateInstanceLayerProperties(&availableLayerCount, nullptr));
        VkLayerProperties availableLayers[availableLayerCount];
        CALL_VK(vkEnumerateInstanceLayerProperties(&availableLayerCount, availableLayers));
        
        uint32_t enableLayerCount = 0;
        const char* enableLayers[32];
        if (bValidate) {
            if(!findDeviceFeatures("Instance Layers", false, availableLayerCount, availableLayers,
                            ARRAY_SIZE(requestedLayers), requestedLayers, &enableLayerCount, enableLayers)) {
                exit(EXIT_FAILURE);
            }
        }

        // 2. 构建extension
        uint32_t availableExtensionCount = 0;
        CALL_VK(vkEnumerateInstanceExtensionProperties("", &availableExtensionCount, nullptr));
        VkExtensionProperties availableExtensions[availableExtensionCount];
        CALL_VK(vkEnumerateInstanceExtensionProperties("", &availableExtensionCount, availableExtensions));

        // 添加glfw拓展
        uint32_t glfwRequestedExtensionCount;
        std::unordered_set<const char*> allRequestedExtensionSet;
        std::vector<DeviceFeature> allRequestedExtensions;
        const char ** glfwRequestedExtensions = glfwGetRequiredInstanceExtensions(&glfwRequestedExtensionCount);
        for (const auto &item: requestedExtensions){
            if(allRequestedExtensionSet.find(item.name) == allRequestedExtensionSet.end()) {
                allRequestedExtensionSet.insert(item.name);
                allRequestedExtensions.push_back(item);
            }
        }

        for (int i = 0; i < glfwRequestedExtensionCount; i++){
            const char *extensionName = glfwRequestedExtensions[i];
            if(allRequestedExtensionSet.find(extensionName) == allRequestedExtensionSet.end()) {
                allRequestedExtensionSet.insert(extensionName);
                allRequestedExtensions.push_back({ extensionName, true });
            }
        }

        uint32_t enableExtensionCount;
        const char *enableExtensions[32];
        if(!findDeviceFeatures("Instance Extension", true, availableExtensionCount, availableExtensions,
                                allRequestedExtensions.size(), allRequestedExtensions.data(), &enableExtensionCount, enableExtensions)){
            exit(EXIT_FAILURE);
        }

        // 3. 创建Vulkan实例
        VkApplicationInfo applicationInfo = {
            .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
            .pNext = nullptr,
            .pApplicationName = "Wanna Engine",
            .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
            .pEngineName = "Wanna",
            .engineVersion = VK_MAKE_VERSION(1, 0, 0),
            .apiVersion = VK_API_VERSION_1_3,
        };// 应用信息

        // 错误回调函数信息
        VkDebugReportCallbackCreateInfoEXT debugReportCallbackInfoExt = {
            .sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CREATE_INFO_EXT,
            .pNext = nullptr,
            .flags = VK_DEBUG_REPORT_WARNING_BIT_EXT
                    | VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT
                    | VK_DEBUG_REPORT_ERROR_BIT_EXT,
            .pfnCallback = &VkDebugReportCallback,
        };

        // Vulkan实例信息
        VkInstanceCreateInfo instanceInfo = {
            .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
            .pNext = bValidate ? &debugReportCallbackInfoExt : nullptr, // 错误回调
            .flags = 0,
            .pApplicationInfo = &applicationInfo,
            .enabledLayerCount = enableLayerCount,
            .ppEnabledLayerNames = enableLayerCount > 0 ? enableLayers : nullptr,
            .enabledExtensionCount = enableExtensionCount,
            .ppEnabledExtensionNames = enableExtensionCount > 0 ? enableExtensions : nullptr,
        };

        CALL_VK(vkCreateInstance(&instanceInfo, nullptr, &mInstance));
        LOG_TRACE("{0} : instance : {1}", __FUNCTION__, (void*)mInstance);
    }

    // 创建表面
    void WannaVulkanContext::CreateSurface(WannaWindow *win) {

        if (!win) {
            LOG_ERROR("Window isn't exists");
            exit(EXIT_FAILURE);
        }

        auto *glfwWin = dynamic_cast<WannaGlfwWindow *>(win);
        if (!glfwWin) {
            // FIXME
            LOG_ERROR("This window isn't a glfw window");
            exit(EXIT_FAILURE);
        }

        CALL_VK(glfwCreateWindowSurface(mInstance, glfwWin->GetWindowHandle(), nullptr, &mSurface));
        LOG_TRACE("{0} : surface : {1}", __FUNCTION__, (void*)mSurface);
    }

    // 选择物理设备
    void WannaVulkanContext::SelectPhysicalDevice() {
        uint32_t physicDeviceCount;
        CALL_VK(vkEnumeratePhysicalDevices(mInstance, &physicDeviceCount, nullptr));
        VkPhysicalDevice physicalDevices[physicDeviceCount];
        CALL_VK(vkEnumeratePhysicalDevices(mInstance, &physicDeviceCount, physicalDevices));

        // 输出物理设备信息
        uint32_t maxScore = 0;
        int32_t maxScorePhysicalDeviceIndex = -1;

        LOG_DEBUG("--------------------------");
        LOG_DEBUG("All Pyhsical Devices");
        LOG_DEBUG("--------------------------");
        for (int i = 0; i < physicDeviceCount; i++) {
            VkPhysicalDeviceProperties props;
            vkGetPhysicalDeviceProperties(physicalDevices[i], &props);
            printPhysicalDeviceInfo(&props);

            // 计分(选取分数最高的物理设备)
            uint32_t score = countPhysicalDeviceScore(&props);
            uint32_t formatCount;
            CALL_VK(vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevices[i], mSurface, &formatCount, nullptr));
            VkSurfaceFormatKHR formats[formatCount];
            CALL_VK(vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevices[i], mSurface, &formatCount, formats));
            for (int j = 0; j < formatCount; j++) {
                // 检查设备是否支持RGB颜色格式和SRGB颜色空间
                if (formats[j].format == VK_FORMAT_B8G8R8A8_UNORM && formats[j].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                    score += 10;
                }
            }

            if (score < maxScore) {
                continue;
            }

            // 查询物理设备队列族
            uint32_t queueFamilyCount;
            vkGetPhysicalDeviceQueueFamilyProperties(physicalDevices[i], &queueFamilyCount, nullptr);
            VkQueueFamilyProperties queueFamilyProperties[queueFamilyCount];
            vkGetPhysicalDeviceQueueFamilyProperties(physicalDevices[i], &queueFamilyCount, queueFamilyProperties);
            for (int k = 0; k < queueFamilyCount; k++) {
                // 跳过不包含队列的族
                if (queueFamilyProperties[k].queueCount == 0) {
                    continue;
                }
                
                // 1. 图形族(graphic family)
                if (queueFamilyProperties[k].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                    mGraphicQueueFamily.queueFamilyIndex = k;
                    mGraphicQueueFamily.queueCount = queueFamilyProperties[k].queueCount;
                }

                // 找到则跳出循环
                if (mGraphicQueueFamily.queueFamilyIndex >= 0 && mPresentQueueFamily.queueFamilyIndex >= 0
                    && mGraphicQueueFamily.queueFamilyIndex != mPresentQueueFamily.queueFamilyIndex) {
                    break;
                }

                // 2. 显示族(present family)
                VkBool32 bSupportSurface;
                vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevices[i], k, mSurface, &bSupportSurface);
                if (bSupportSurface) {
                    mPresentQueueFamily.queueFamilyIndex = k;
                    mPresentQueueFamily.queueCount = queueFamilyProperties[k].queueCount;
                }

                // 更新最大分数和最大分数索引
                if (mGraphicQueueFamily.queueFamilyIndex >= 0 && mPresentQueueFamily.queueFamilyIndex >= 0) {
                    maxScore = score;
                    maxScorePhysicalDeviceIndex = i;
                }
            }
        }
        // 未找到合适的硬件设备
        if (maxScorePhysicalDeviceIndex < 0) {
            LOG_WARNING("Can't find suitable phsical device, choose Device 0.");
            maxScorePhysicalDeviceIndex = 0;
        }

        mPhysicalDevice = physicalDevices[maxScorePhysicalDeviceIndex];
        // 获取设备内存大小
        vkGetPhysicalDeviceMemoryProperties(mPhysicalDevice, &mPhysicalDeviceMemoryProperties);

        LOG_INFO("{0}: physical device:{1}, score:{2}, graphic queue: {3} : {4}, present queue: {5} : {6}", 
        __FUNCTION__, maxScorePhysicalDeviceIndex, maxScore,
        mGraphicQueueFamily.queueFamilyIndex, mGraphicQueueFamily.queueCount,
        mPresentQueueFamily.queueFamilyIndex, mPresentQueueFamily.queueCount);
    }

    // 打印物理设备信息
    void WannaVulkanContext::printPhysicalDeviceInfo(VkPhysicalDeviceProperties *properties) {

        const char* typeName = properties->deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU ? "integrated gpu" :
                               properties->deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU   ? "descrete gpu"   :
                               properties->deviceType == VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU    ? "virtual gpu"    :
                               properties->deviceType == VK_PHYSICAL_DEVICE_TYPE_CPU            ? "cpu"            :"other";
        uint32_t deviceVersionMajor = VK_VERSION_MAJOR(properties->driverVersion);
        uint32_t deviceVersionMinor = VK_VERSION_MINOR(properties->apiVersion);
        uint32_t deviceVersionPatch = VK_VERSION_PATCH(properties->apiVersion);

        uint32_t apiVersionMajor = VK_VERSION_MAJOR(properties->apiVersion);
        uint32_t apiVersionMinor = VK_VERSION_MINOR(properties->apiVersion);
        uint32_t apiVersionPatch = VK_VERSION_PATCH(properties->apiVersion);


        LOG_DEBUG("Device name:     {0}", properties->deviceName);
        LOG_DEBUG("Type name:       {0}", typeName);
        LOG_DEBUG("Device ID:       {0}", properties->deviceID);
        LOG_DEBUG("Vendor ID:       {0}", properties->vendorID);
        LOG_DEBUG("Device version:  {0}.{1}.{2}", deviceVersionMajor, deviceVersionMinor, deviceVersionPatch);
        LOG_DEBUG("Api version:     {0}.{1}.{2}", apiVersionMajor, apiVersionMinor, apiVersionPatch);

        LOG_DEBUG("--------------------------");
    }

    // 计算设备得分
    uint32_t WannaVulkanContext::countPhysicalDeviceScore(VkPhysicalDeviceProperties *properties) {
        VkPhysicalDeviceType physcialType = properties->deviceType;
        uint32_t score = 0;
        switch (physcialType) {
            case VK_PHYSICAL_DEVICE_TYPE_OTHER:
                break;
            case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
                score += 40;
                break;
            case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
                score += 30;
                break;
            case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
                score += 20;
                break;
            case VK_PHYSICAL_DEVICE_TYPE_CPU:
                score += 10;
                break;
            default:
                break;
        }
        return score;
    }
}