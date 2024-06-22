#version 450
#extension GL_ARB_separate_shader_objects : enable

// 帧缓冲区0
layout(location=0) in vec4 vertexColor;
layout(location=0) out vec4 fragColor;

void main(){
    fragColor = vertexColor;
}
