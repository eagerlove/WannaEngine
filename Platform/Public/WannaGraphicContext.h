#ifndef WANNA_GRAPHIC_CONTEXT_H
#define WANNA_GRAPHIC_CONTEXT_H

#include "WannaEngine.h"

namespace WannaEngine {
    class WannaWindow;
    class WannaGraphicContext {
        public:
            WannaGraphicContext(const WannaGraphicContext&) = delete;
            WannaGraphicContext &operator=(const WannaGraphicContext&) = delete;
            virtual ~WannaGraphicContext() = default;

            static std::unique_ptr<WannaGraphicContext> Create(WannaWindow *window);

        protected:
            WannaGraphicContext() = default;
    };
}

#endif