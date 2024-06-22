/* 旋转立方体 */
#version 450
#extension GL_ARB_separate_shader_objects : enable

// 帧缓冲区
layout(location = 0) in vec3 Pos;
layout(location = 1) in vec2 Texcoord;
layout(location = 2) in vec3 Normal;

// 顶点着色器输出
out gl_PerVertex {
    vec4 gl_Position;
};

// 旋转矩阵
layout(push_constant) uniform PushConstants {
    mat4 matrix;
} PC;

out layout(location=0) vec4 vertexColor;

void main() {
    gl_Position = PC.matrix * vec4(Pos.x, -Pos.y, Pos.z, 1.f);
    vertexColor = vec4(Normal.x * 0.5 + 0.5, Normal.y * 0.5 + 0.5, Normal.z * 0.5 + 0.5, 1.0);
}


/* 三角形 */
// #version 450

// out gl_PerVertex{
//     vec4 gl_Position;
// };

// vec3 vertices[] = {
//     vec3(0, 0, 0),
//     vec3(0, 0.5, 0),
//     vec3(-0.5, 0, 0)
// };

// vec4 colors[] = {
//     vec4(1, 0, 0, 1),
//     vec4(0, 1, 0, 1),
//     vec4(0, 0, 1, 1)
// };

// out layout(location=0) vec4 vertexColor;

// void main(){
//     vec3 in_pos = vertices[gl_VertexIndex];
//     gl_Position = vec4(in_pos.x, -in_pos.y, in_pos.z, 1.f);
//     vertexColor = colors[gl_VertexIndex];
// }
