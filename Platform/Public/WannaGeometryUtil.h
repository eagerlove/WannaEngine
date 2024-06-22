#ifndef WANNA_GEOMETRY_UTIL_H
#define WANNA_GEOMETRY_UTIL_H

#include "WannaGraphicContext.h"

namespace WannaEngine {
    // 顶点数据
    struct WannaVertex{
        glm::vec3 position; // 顶点坐标
        glm::vec2 texcoord0; // 纹理坐标
        glm::vec3 normal;   // 法线坐标
    };

    class WannaGeometryUtil{
        public:
        // 创建立方体
        static void CreateCube(float leftPlane, float rightPlane, float bottomPlane, float topPlane, float nearPlane, float farPlane,
                                    std::vector<WannaVertex> &vertices, std::vector<uint32_t> &indices, const bool bUseTextcoords = true,
                                    const bool bUseNormals = true, const glm::mat4 &relativeMat = glm::mat4(1.0f));

    };
}

#endif