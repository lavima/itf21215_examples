#version 450

layout (location = 0) in vec3 pos;

layout (binding = 0, std140) uniform Transform0
{
    mat4 proj;
    mat4 view;
};

// model matrix
layout (binding = 1, std140) uniform Transform1
{
    mat4 model;
};

void main()
{
    //gl_Position = proj * (view * (model * vec4(pos,1)));
    gl_Position = vec4(pos,1);
}
