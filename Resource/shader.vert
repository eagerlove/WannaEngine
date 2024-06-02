#version 450
#extension GL_ARB_separate_shader_objects : enable

// 顶点着色器
out gl_PerVertex {
    vec4 gl_Position;
};

// 帧缓冲区0
layout(location = 0) out vec3 fragColor;

// 顶点坐标
vec2 positions[3] = vec2[](
    vec2(0.0, -0.5),
    vec2(0.5, 0.5),
    vec2(-0.5, 0.5)
);

// 顶点颜色
// 顶点之外的颜色被自动插值处理
vec3 colors[3] = vec3[](
    vec3(1.0, 0.0, 0.0),
    vec3(0.0, 1.0, 0.0),
    vec3(0.0, 0.0, 1.0)
);

void main() {
    gl_Position = vec4(positions[gl_VertexIndex], 0.0, 1.0);
    fragColor = colors[gl_VertexIndex];
}
