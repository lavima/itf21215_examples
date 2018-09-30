#version 440

// Incoming interpolated (between vertices) color.
layout (location = 0) in Block
{
    vec3 interpolatedColor;
};

// Outgoing final color.
layout (location = 0) out vec4 outputColor;


void main()
{
    // We simply pad the interpolatedColor
    outputColor = vec4(interpolatedColor, 1);
}
