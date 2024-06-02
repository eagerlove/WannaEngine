#include "WannaGraphicContext.h"
#include "WannaVulkanContext.h"

namespace WannaEngine {
    std::unique_ptr<WannaGraphicContext> WannaGraphicContext::Create(WannaWindow *window) {

#ifdef WANNA_ENGINE_GRAPHIC_API_VULKAN
    return std::make_unique<WannaVulkanContext>(window);
#else

#endif
        return nullptr;
    }
}