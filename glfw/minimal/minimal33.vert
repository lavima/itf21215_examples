#version 330

// Incoming vertex position, Model Space.
layout (location = 0) in vec3 position;

// Incoming vertex color.
layout (location = 1) in vec3 color;

// Projection, view and model matrices.
uniform mat4 proj;
uniform mat4 view;
uniform mat4 model;

// Outgoing color.
out vec3 interpolatedColor;

void main() {

    // Normally gl_Position is in Clip Space and we calculate it by multiplying together all the matrices
    gl_Position = proj * (view * (model * vec4(position, 1)));

    // We assign the color to the outgoing variable.
    interpolatedColor = color;

}
