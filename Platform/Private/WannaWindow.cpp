#include "WannaWindow.h"
#include "WannaGlfwWindow.h"


namespace WannaEngine{
    std::unique_ptr<WannaWindow> WannaWindow::Create(uint32_t width, uint32_t height, const char* title) {

// Windows
#ifdef WANNA_ENGINE_PLATFORM_WIN32
    return std::make_unique<WannaGlfwWindow>(width, height, title);

// Macos
#elif WANNA_ENGINE_PLATFORM_MACOS
    return std::make_unique<WannaGlfwWindow>(width, height, title);

// Linux
#elif WANNA_ENGINE_PLATFORM_LINUX
    return std::make_unique<WannaGlfwWindow>(width, height, title);

#endif
        return nullptr;
    }
}