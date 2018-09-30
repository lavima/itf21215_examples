#version 330

// Input attributes
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 uv;

// Projection, view and model matrices.
uniform mat4 proj;
uniform mat4 view;
uniform mat4 model;

// Outgoing color.
out vec2 UV;

void main() {

    // Normally gl_Position is in Clip Space and we calculate it by multiplying together all the matrices
    gl_Position = proj * (view * (model * vec4(position, 1)));

    // Set the output UV coordinates
    UV = uv;

}
