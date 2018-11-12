#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <string>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define DEFAULT_WIDTH 1024
#define DEFAULT_HEIGHT 768

// Vertex Array attributes
#define POSITION 0
#define NORMAL 1
#define UV 2

// GLSL Uniform indices
#define TRANSFORM0 0
#define TRANSFORM1 1
#define LIGHT 2
#define MATERIAL 3
#define CAMERA 4

// Uniforms values
GLfloat lightPosition[] { 0.0f, 0.0f, 10.0f };
GLfloat lightAmbient[] { 0.4f, 0.4f, 0.4f };
GLfloat lightDiffuse[] { 0.7f, 0.5f, 0.5f };
GLfloat lightSpecular[] { 0.6f, 0.6f, 0.6f };
GLfloat materialShininessColor[] { 1.0f, 1.0f, 1.0f,  1.0f };
GLfloat materialShininess = 32.0f;
GLfloat cameraPosition[] { 0.0f, 0.0f, 10.0f };

// Uniform locations
GLint projectionMatrixPos;
GLint viewMatrixPos;
GLint modelMatrixPos;
GLint lightPositionPos;
GLint lightAmbientPos;
GLint lightDiffusePos;
GLint lightSpecularPos;
GLint materialShininessColorPos;
GLint materialShininessPos;
GLint cameraPositionPos;

// Names
GLuint programName;
GLuint vertexBufferName;
GLuint vertexArrayName;
GLuint textureName;

// Global variables to store the index data and the number of indices in 
// the generated sphere
GLushort *indexData;
int numIndices;

/*
 * Read shader source file from disk
 */
char *readSourceFile(const char *filename, int *size) {

    // Open the file as read only
    FILE *file = fopen(filename, "r");

    // Find the end of the file to determine the file size
    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);

    // Rewind
    fseek(file, 0, SEEK_SET); 

    // Allocate memory for the source and initialize it to 0
    char *source = (char *)malloc(fileSize + 1);
    for (int i = 0; i <= fileSize; i++) source[i] = 0;

    // Read the source
    fread(source, fileSize, 1, file);

    // Close the file
    fclose(file);

    // Store the size of the file in the output variable
    *size = fileSize-1;

    // Return the shader source
    return source;

}

/*
 * Initialize OpenGL
 */
int initGL() {

    // Read the texture image
    int width, height, channels;
    GLubyte *imageData = stbi_load("sphere.png", &width, &height, &channels, STBI_default);
    if (!imageData)
        return 0;

    // Generate a new texture name and activate it
    glGenTextures(1, &textureName);
    glBindTexture(GL_TEXTURE_2D, textureName);

    // Set sampler properties
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    if (channels == 3)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, imageData);
    else if (channels == 4)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData);
    else
        return 0; 

    // Generate mip map images
    glGenerateMipmap(GL_TEXTURE_2D);

    // Deactivate the texture and free the image data
    glBindTexture(GL_TEXTURE_2D, 0);
    stbi_image_free(imageData);

    // Load and compile vertex shader
    GLuint vertexName = glCreateShader(GL_VERTEX_SHADER); // 2.0
    int vertexLength = 0;
    char *vertexSource = readSourceFile("default33.vert", &vertexLength);
    glShaderSource(vertexName, 1, (const char * const *)&vertexSource, &vertexLength); // 2.0
    GLint compileStatus;
    glCompileShader(vertexName); // 2.0
    glGetShaderiv(vertexName, GL_COMPILE_STATUS, &compileStatus); // 2.0
    if (!compileStatus) {
        GLint logSize = 0;
        glGetShaderiv(vertexName, GL_INFO_LOG_LENGTH, &logSize);
        char *errorLog = (char *)malloc(sizeof(char) * logSize);
        glGetShaderInfoLog(vertexName, logSize, &logSize, errorLog); // 2.0
        glDeleteShader(vertexName); // 2.0 
        printf("VERTEX ERROR %s\n", errorLog);
        return 0;
    }
    free(vertexSource);

    // Load and compile fragment shader
    GLuint fragmentName = glCreateShader(GL_FRAGMENT_SHADER);
    int fragmentLength = 0;
    char *fragmentSource = readSourceFile("default33.frag", &fragmentLength);
    glShaderSource(fragmentName, 1, (const char * const *)&fragmentSource, &fragmentLength);
    glCompileShader(fragmentName);
    glGetShaderiv(fragmentName, GL_COMPILE_STATUS, &compileStatus);
    if (!compileStatus) {
        GLint logSize = 0;
        glGetShaderiv(fragmentName, GL_INFO_LOG_LENGTH, &logSize);
        char *errorLog = (char *)malloc(sizeof(char) * logSize);
        glGetShaderInfoLog(fragmentName, logSize, &logSize, errorLog);
        glDeleteShader(fragmentName);

        printf("FRAGMENT ERROR %s\n", errorLog);
        return 0;
    }
    free(fragmentSource);

    // Create and link vertex program
    programName = glCreateProgram(); // 2.0
    glAttachShader(programName, vertexName); // 2.0
    glAttachShader(programName, fragmentName);
    glLinkProgram(programName); // 2.0
    GLint linkStatus;
    glGetProgramiv(programName, GL_LINK_STATUS, &linkStatus); // 2.0
    if (!linkStatus) {
        GLint logSize = 0;
        glGetProgramiv(programName, GL_INFO_LOG_LENGTH, &logSize);
        char *errorLog = (char *)malloc(sizeof(char) * logSize);
        glGetProgramInfoLog(programName, logSize, &logSize, errorLog); // 2.0

        printf("LINK ERROR %s\n", errorLog);
        return 0;
    }

    // Get uniform locations
    projectionMatrixPos = glGetUniformLocation(programName, "proj");
    viewMatrixPos = glGetUniformLocation(programName, "view");
    modelMatrixPos = glGetUniformLocation(programName, "model");
    lightPositionPos = glGetUniformLocation(programName, "lightPosition");
    lightAmbientPos = glGetUniformLocation(programName, "lightAmbient");
    lightDiffusePos = glGetUniformLocation(programName, "lightDiffuse");
    lightSpecularPos = glGetUniformLocation(programName, "lightSpecular");
    materialShininessColorPos = glGetUniformLocation(programName, "shininessColor");
    materialShininessPos = glGetUniformLocation(programName, "shininess");
    cameraPositionPos = glGetUniformLocation(programName, "cameraPosition");

    // Enable depth buffer testing
    glEnable(GL_DEPTH_TEST);

    return 1;

}

/*
 * Create a sphere with the specified radius and with the specified number of segments.
 * numV specifies the number of segments along the vertical axis
 * numH specifies the number of segments along the horizontal axis
 */
int createSphere(float radius, int numH, int numV) {

    if (numH < 4 || numV < 2)
        return 0;

    // Variables needed for the calculations
    float pi = glm::pi<float>();
    float pi2 = pi * 2.0f;
    float d1 = pi / numV;
    float d2 = pi2 / numH;

    // Allocate the data needed to store the necessary positions, normals and texture coordinates
    int numVertices = numH*(numV-1)+2;
    int numPer = (3+3+2);
    std::vector<GLfloat> vertexData(numVertices * numPer);

    // Create the top vertex
    vertexData[0] = 0.0f; vertexData[1] = radius; vertexData[2] = 0.0f;
    vertexData[3] = 0.0f; vertexData[4] = 1.0f; vertexData[5] = 0.0f;
    vertexData[6] = 0.5f; vertexData[7] = 1.0f;

    // Loop through the divisions along the vertical axis
    for (int i=0; i<numV-1; ++i) {
        // Loop through the divisions along the horizontal axis
        for (int j=0; j<numH; ++j) {
            // Calculate the variables needed for this iteration
            int base = (i * numH + j + 1) * numPer;
            float t1 = d1 * (i + 1);
            float t2 = d2 * j;
            // Position (like given in lecture)
            vertexData[base] = radius * glm::sin(t2) * glm::sin(t1);
            vertexData[base+1] = radius * glm::cos(t1);
            vertexData[base+2] = radius * glm::cos(t2) * glm::sin(t1);
            // Normal (the same as position except unit length)
            vertexData[base+3] = glm::sin(t2) * glm::sin(t1);
            vertexData[base+4] = glm::cos(t1);
            vertexData[base+5] = glm::cos(t2)*glm::sin(t1);
            // UV 
            vertexData[base+6] = glm::asin(vertexData[base+3]) / pi + 0.5f;
            vertexData[base+7] = glm::asin(vertexData[base+4]) / pi + 0.5f;
        }
    }

    // Create the bottom vertex
    vertexData[(numVertices-1)*numPer] = 0.0f; vertexData[(numVertices-1)*numPer+1] = -radius; vertexData[(numVertices-1)*numPer+2] = 0.0f;
    vertexData[(numVertices-1)*numPer+3] = 0.0f; vertexData[(numVertices-1)*numPer+4] = -1.0f; vertexData[(numVertices-1)*numPer+5] = 0.0f;
    vertexData[(numVertices-1)*numPer+6] = 0.5f; vertexData[(numVertices-1)*numPer+7] = 0.0f;

    // Allocate the data needed to store the indices
    int numTriangles = (numH*(numV-1)*2);
    numIndices = numTriangles * 3;
    indexData = (GLushort *)malloc(numIndices * sizeof(GLushort));

    // Create the triangles for the top
    for (int j=0; j<numH; j++) {
        indexData[j*3] = 0; 
        indexData[j*3+1] = (GLushort)(j+1); 
        indexData[j*3+2] = (GLushort)((j+1)%numH+1);
    }
    // Loop through the segment circles 
    for (int i=0; i<numV-2; ++i) {
        for (int j=0; j<numH; ++j) {
            indexData[((i*numH+j)*2+numH)*3] = (GLushort)(i*numH+j+1);
            indexData[((i*numH+j)*2+numH)*3+1] = (GLushort)((i+1)*numH+j+1);
            indexData[((i*numH+j)*2+numH)*3+2] = (GLushort)((i+1)*numH+(j+1)%numH+1);

            indexData[((i*numH+j)*2+numH)*3+3] = (GLushort)((i+1)*numH+(j+1)%numH+1);
            indexData[((i*numH+j)*2+numH)*3+4] = (GLushort)(i*numH+(j+1)%numH+1);
            indexData[((i*numH+j)*2+numH)*3+5] = (GLushort)(i*numH+j+1);
        }
    }
    // Create the triangles for the bottom
    int triIndex = (numTriangles-numH);
    int vertIndex = (numV-2)*numH+1;
    for (short i=0; i<numH; i++) {
        indexData[(triIndex+i)*3] = (GLushort)(vertIndex+i);
        indexData[(triIndex+i)*3+1] = (GLushort)((numH*(numV-1)+1));
        indexData[(triIndex+i)*3+2] = (GLushort)(vertIndex+(i+1)%numH);
    }


    // Create a vertex buffer for the vertex and index data
    glGenBuffers(1, &vertexBufferName);

    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferName); // 2.0
    glBufferData(GL_ARRAY_BUFFER, numVertices * numPer * sizeof(GLfloat), &vertexData[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0); // 2.0

    // Create and initialize a vertex array object
    glGenVertexArrays(1, &vertexArrayName);
    glBindVertexArray(vertexArrayName);

    // Associate vertex attributes with the binding point (POSITION NORMAL UV) and specify the format
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferName);
    glVertexAttribPointer(POSITION, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), 0); // 3.0
    glVertexAttribPointer(NORMAL, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), (void *)(3 * sizeof(GL_FLOAT))); // 3.0
    glVertexAttribPointer(UV, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), (void *)(6 * sizeof(GL_FLOAT))); // 3.0
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Enable the attributes
    glEnableVertexAttribArray(POSITION); // 2.0
    glEnableVertexAttribArray(NORMAL);
    glEnableVertexAttribArray(UV);

    glBindVertexArray(0);

    return 1;

}

/*
 * Draw OpenGL screne
 */
void drawGLScene() {

    // Clear color and depth buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Activate the program
    glUseProgram(programName); // 2.0

    // Set the view matrix
    glm::mat4 view = glm::mat4(1.0f);
    view = glm::translate(view, glm::vec3(-cameraPosition[0], -cameraPosition[1], -cameraPosition[2]));
    glUniformMatrix4fv(viewMatrixPos, 1, GL_FALSE, &view[0][0]); // 2.0

    // Set the model matrix
    glm::mat4 model = glm::mat4(1.0);
    model = glm::rotate(model, (float)glfwGetTime() * 0.3f, glm::vec3(0.0f, 1.0f,  0.0f));
    glUniformMatrix4fv(modelMatrixPos, 1, GL_FALSE, &model[0][0]);

    // Set the remaining uniforms
    glUniform3fv(lightPositionPos, 1, lightPosition);
    glUniform3f(lightAmbientPos, lightAmbient[0], lightAmbient[1], lightAmbient[2]);
    glUniform3fv(lightDiffusePos, 1, lightDiffuse);
    glUniform3fv(lightSpecularPos, 1, lightSpecular);
    glUniform4fv(materialShininessColorPos, 1, materialShininessColor);
    glUniform1f(materialShininessPos, materialShininess);
    glUniform3fv(cameraPositionPos, 1, cameraPosition);

    // Bind the vertex array and texture of the sphere
    glBindVertexArray(vertexArrayName);
    glBindTexture(GL_TEXTURE_2D, textureName);

    // Draw the vertex array
    glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_SHORT, indexData);

    // Disable vertex array and texture
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);

    // Disable
    glUseProgram(0);

}

void resizeGL(int width, int height) {

    // Prevent division by zero
    if (height == 0)
        height = 1;										

    // Change the projection matrix
    glm::mat4 proj = glm::perspective(3.14f/2.0f, (float)width/height, 0.1f, 1000.0f);
    glUseProgram(programName); 
    glUniformMatrix4fv(projectionMatrixPos, 1, GL_FALSE, &proj[0][0]);
    glUseProgram(0);

    // Set the OpenGL viewport
    glViewport(0, 0, width, height); // 2.0

}

/*
 * Error callback function for GLFW
 */
static void glfwErrorCallback(int error, const char* description) {
    fprintf(stderr, "Error: %s\n", description);
}

/*
 * Input event callback function for GLFW
 */
static void glfwKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

/*
 * Window size changed callback function for GLFW
 */
void glfwWindowSizeCallback(GLFWwindow* window, int width, int height) {

    resizeGL(width, height);

}

/*
 * Program entry function
 */
int main(int nargs, const char **argv) {

    // Ensure that there is one argument (besides the program name)
    if (nargs != 4) {
        printf("Wrong usage\n");
        exit(EXIT_FAILURE);
    }

    // Set error callback
    glfwSetErrorCallback(glfwErrorCallback);

    // Initialize GLFW
    if (!glfwInit()) {
        printf("Failed to initialize GLFW\n");  
        exit(EXIT_FAILURE);
    }

    // Specify minimum OpenGL version
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create window
    GLFWwindow* window = glfwCreateWindow(DEFAULT_WIDTH, DEFAULT_HEIGHT, "Minimal", NULL, NULL);
    if (!window) {
        printf("Failed to create GLFW window\n");  
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    // Set input key event callback
    glfwSetKeyCallback(window, glfwKeyCallback);

    // Set window resize callback
    glfwSetWindowSizeCallback(window, glfwWindowSizeCallback);

    // Make the context current
    glfwMakeContextCurrent(window);

    // Initialize GLEW
    if (glewInit() != GLEW_OK) {
        printf("Failed to initialize GLEW\n");  
        glfwDestroyWindow(window);
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    // Make GLFW swap buffers directly 
    glfwSwapInterval(0);

    // Initialize OpenGL
    if (!initGL()) {
        printf("Failed to initialize OpenGL\n");  
        glfwDestroyWindow(window);
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    // Load the OBJ-file
    if (!createSphere(atof(argv[1]), atoi(argv[2]), atoi(argv[3]))) {
        printf("Failed to create sphere.\n");  
        glfwDestroyWindow(window);
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    // Initialize OpenGL view
    resizeGL(DEFAULT_WIDTH, DEFAULT_HEIGHT);

    // Run a loop until the window is closed
    while (!glfwWindowShouldClose(window)) {

        // Draw OpenGL screne
        drawGLScene();

        // Swap buffers
        glfwSwapBuffers(window);

        // Poll fow input events
        glfwPollEvents();

    }

    // Shutdown GLFW
    glfwDestroyWindow(window);
    glfwTerminate();

    // Exit
    exit(EXIT_SUCCESS);

}
