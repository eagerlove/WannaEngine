#ifndef WANNA_ENGINE_GLFW_WINDOW
#define WANNA_ENGINE_GLFW_WINDOW

#include "WannaWindow.h"
#include "GLFW/glfw3.h"

namespace WannaEngine{
    class WannaGlfwWindow : public WannaWindow{
        public:
            WannaGlfwWindow() = delete;
            WannaGlfwWindow(uint32_t width, uint32_t height, const char* title);
            ~WannaGlfwWindow() override;

            bool CLOSE() override;
            void PollEvent() override;
            void SwapBuffer() override;
            GLFWwindow* GetWindowHandle() const { return myGlfwWindow; }
        private:
            GLFWwindow* myGlfwWindow;
    };
}
#endif