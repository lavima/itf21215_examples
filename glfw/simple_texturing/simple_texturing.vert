#version 450

// Input attributes
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 uv;

layout (binding = 0) uniform Transform0
{
    mat4 proj;
    mat4 view;
};

layout (binding = 1) uniform Transform1
{
    mat4 model;
};

layout (location = 0) out Block
{
    vec2 UV;
};

void main() {

    // Normally gl_Position is in Clip Space and we calculate it by multiplying together all the matrices
    gl_Position = proj * (view * (model * vec4(position, 1)));

    // Set the output UV coordinates
    UV = uv;

}
