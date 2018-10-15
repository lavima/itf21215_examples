#version 450

// Incoming vertex position, Model Space.
layout (location = 0) in vec3 position;

// Incoming normal
layout (location = 1) in vec3 normal;

// Incoming vertex color.
layout (location = 2) in vec2 uv;

// Projection and view matrices.
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

// Output
layout (location = 0) out Block
{
    vec2 UV;
    vec3 N;
    vec3 worldVertex;
};

void main() {

    // Normally gl_Position is in Clip Space and we calculate it by multiplying together all the matrices
    gl_Position = proj * (view * (model * vec4(position, 1)));

    // Set the world vertex for calculating the light direction in the fragment shader
    worldVertex = vec3(model * vec4(position, 1));

    // Set the transformed normal
    N = mat3(model) * normal;

    UV = uv;
}
