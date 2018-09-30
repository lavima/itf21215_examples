#version 330

// Incoming interpolated (between vertices) texture coordinate
in vec3 interpolatedColor;

// Outgoing final color.
out vec4 outputColor;

// Texture sampler
uniform sampler2D textureSampler;

void main()
{
    // Retrieve the final color from the texture
    outputColor = texture(textureSampler, UV).rgba;
}
