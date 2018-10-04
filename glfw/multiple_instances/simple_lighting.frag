#version 450

// Incoming interpolated (between vertices) color.
layout (location = 0) in Block
{
    vec3 interpolatedColor;
    vec3 N;
    vec3 worldVertex;
};

layout (std140, binding = 2) uniform Light
{
    vec3 lightPos;
    vec3 lightAmbient;
    vec3 lightDiffuse;
    vec3 lightSpecular;
};

layout (std140, binding = 3) uniform Material
{
    vec4 shininessColor;
    float shininess;
};

layout (std140, binding = 4) uniform Camera
{
    vec3 cameraPos;
};

// Outgoing final color.
layout (location = 0) out vec4 outputColor;

// Vectors
vec3 L;
vec3 NN;
vec3 V;
vec3 R;

// Colors
vec4 color;
vec4 ambient;
vec4 diffuse;
vec4 specular;


void main()
{
    color = vec4(interpolatedColor, 1);

    // Normalize the interpolated normal to ensure unit length
    NN = normalize(N);
    
    // Find the unit length normal giving the direction from the vertex to the light
    L = normalize(lightPos - worldVertex);

    // Find the unit length normal giving the direction from the vertex to the camera
    V = normalize(cameraPos - worldVertex);

    // Find the unit length reflection normal
    R = normalize(reflect(-L, NN));
    
    // Calculate the ambient component
    ambient = vec4(lightAmbient, 1) * color;

    // Calculate the diffuse component
    diffuse = vec4(max(dot(L, NN), 0.0) * lightDiffuse, 1) * color;

    // Calculate the specular component
    specular = vec4(pow(max(dot(R, V), 0.0), shininess) * lightSpecular, 1) * shininessColor;

    // Put it all together
    outputColor = ambient + diffuse + specular;

}
