#ifndef WANNA_WINDOW_H
#define WANNA_WINDOW_H

#include "WannaEngine.h"


namespace WannaEngine {
    class WannaWindow{
        public:
            WannaWindow(const WannaWindow&) = delete;
            WannaWindow &operator=(const WannaWindow&) = delete;
            virtual ~WannaWindow() = default;

            static std::unique_ptr<WannaWindow> Create(uint32_t width, uint32_t height, const char* title);
            virtual void PollEvent() = 0; // 拖动事件
            virtual void SwapBuffer() = 0; // 渲染交换缓冲区
            virtual bool CLOSE() = 0; // 窗口关闭事件

        protected:
            WannaWindow() = default;

    };
}

#endif