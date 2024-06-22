#ifndef WANNA_GRAPHIC_CONTEXT_H
#define WANNA_GRAPHIC_CONTEXT_H

#include "WannaEngine.h"

#define GLM_FORCE_RADIANS // 角度强制使用弧度制
#define GLM_FORCE_DEPTH_ZERO_TO_ONE // 深度值 0～1
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/glm.hpp"
#include "glm/gtc/random.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/quaternion.hpp"

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