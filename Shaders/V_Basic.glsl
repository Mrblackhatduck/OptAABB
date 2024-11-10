#version 430 core
layout (location = 0) in vec3 aPos;
uniform mat4 proj;
uniform mat4 view;
uniform mat4 transform;
void main()
{
    gl_Position = proj * view * transform * vec4(aPos.xyz, 1.0);
}