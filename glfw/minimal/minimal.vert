#version 440

// Incoming vertex position, Model Space.
layout (location = 0) in vec3 position;

// Incoming vertex color.
layout (location = 1) in vec3 color;

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

// Outgoing color.
layout (location = 0) out Block
{
    vec3 interpolatedColor;
};

void main() {

    // Normally gl_Position is in Clip Space and we calculate it by multiplying together all the matrices
    gl_Position = proj * (view * (model * vec4(position, 1)));

    // We assign the color to the outgoing variable.
    interpolatedColor = color;

}
