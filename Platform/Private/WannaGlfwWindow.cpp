#include "WannaGlfwWindow.h"
#include "WannaLog.h"


namespace WannaEngine{
    WannaGlfwWindow::WannaGlfwWindow(uint32_t width, uint32_t height, const char* title)
    {
        if (!glfwInit()) {
            LOG_ERROR("GLFW not init");
            exit(EXIT_FAILURE);
        }

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // 禁用Opengl上下文创建
        glfwWindowHint(GLFW_VISIBLE, GL_FALSE); // 等待窗口初始化完成 暂时设为不可见

        mGlfwWindow = glfwCreateWindow(width, height, title, nullptr, nullptr); 
        if (!mGlfwWindow) {
            LOG_ERROR("Failed to create Window");
            exit(EXIT_FAILURE);
        }

        // 设置窗口居中
        GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
        if (primaryMonitor) {
            int xPos = 0, yPos = 0, workWidth = 0, workHeight = 0;
            glfwGetMonitorWorkarea(primaryMonitor, &xPos, &yPos, &workWidth, &workHeight);
            glfwSetWindowPos(mGlfwWindow, workWidth/2 - width/2, workHeight/2 - height/2);
        }

        glfwMakeContextCurrent(mGlfwWindow);

        // 展示窗口
        glfwShowWindow(mGlfwWindow);
    }

    // 销毁窗口
    WannaGlfwWindow::~WannaGlfwWindow()
    {
        glfwDestroyWindow(mGlfwWindow);
        glfwTerminate();
        LOG_INFO("The application is end!");
    }

    bool WannaGlfwWindow::CLOSE() {
        return glfwWindowShouldClose(mGlfwWindow);
    }

    void WannaGlfwWindow::PollEvent() {
        glfwPollEvents();
    }

    void WannaGlfwWindow::SwapBuffer() {
        glfwSwapBuffers(mGlfwWindow);
    }
}