#version 330 core

layout (location = 0) in vec2 a_vPos;

void main()
{   
    gl_Position = vec4(a_vPos, 1.0f, 1.0f);
}