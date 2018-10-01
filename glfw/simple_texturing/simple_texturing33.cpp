#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define DEFAULT_WIDTH 1024
#define DEFAULT_HEIGHT 768

// Vertex Buffer Identifiers
#define VERTICES 0

// Vertex Array attributes
#define POSITION 0
#define UV 1

// GLSL Uniform locations
#define PROJECTION_MATRIX 0
#define VIEW_MATRIX 1
#define MODEL_MATRIX 2

// Vertices
GLfloat vertices[] = {
    // Front
    -1.0f, 1.0f, 1.0f, 0.0f, 2.0f,
    -1.0f, -1.0f, 1.0f, 0.0f, 0.0f,
    1.0f, -1.0f, 1.0f, 2.0f, 0.0f,
    1.0f, 1.0f, 1.0f, 2.0f, 2.0f,
    // Back
    1.0f, 1.0f, -1.0f, 0.0f, 1.0f,
    1.0f, -1.0f, -1.0f, 0.0f, 0.0f,
    -1.0f, -1.0f, -1.0f, 1.0f, 0.0f,
    -1.0f, 1.0f, -1.0f, 1.0f, 1.0f,
    // Left
    -1.0f, 1.0f, -1.0f, 0.0f, 1.0f,
    -1.0f, -1.0f, -1.0f, 0.0f, 0.0f,
    -1.0f, -1.0f, 1.0f, 1.0f, 0.0f,
    -1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
    // Right
    1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
    1.0f, -1.0f, 1.0f, 0.0f, 0.0f,
    1.0f, -1.0f, -1.0f, 1.0f, 0.0f,
    1.0f, 1.0f, -1.0f, 1.0f, 1.0f,
    // Top
    -1.0f, 1.0f, -1.0f, 0.0f, 1.0f,
    -1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
    1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
    1.0f, 1.0f, -1.0f, 1.0f, 1.0f,
    // Bottom
    -1.0f, -1.0f, 1.0f, 0.0f, 1.0f,
    -1.0f, -1.0f, -1.0f, 0.0f, 0.0f,
    1.0f, -1.0f, -1.0f, 1.0f, 0.0f,
    1.0f, -1.0f, 1.0f,  1.0f, 1.0f
};

// Indices
GLshort indices[] {
    // Front
    0, 1, 2, 2, 3, 0,
    // Back
    4, 5, 6, 6, 7, 4,
    // Left
    8, 9, 10, 10, 11, 8,
    // Right
    12, 13, 14, 14, 15, 12,
    // Top
    16, 17, 18, 18, 19, 16,
    // Bottom
    20, 21, 22, 22, 23, 20
};
    
// Uniform locations
GLint projectionMatrixPos;
GLint viewMatrixPos;
GLint modelMatrixPos;

// Names
GLuint programName;
GLuint vertexArrayName;
GLuint vertexBufferNames[1];
GLuint textureName;

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

    // Create 4 buffer names
    glGenBuffers(2, vertexBufferNames); // 2.0

    // Allocate storage for the vertex array buffers
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferNames[POSITION]); // 2.0
    glBufferData(GL_ARRAY_BUFFER, 6 * 4 * 5 * sizeof(GLfloat), vertices, GL_STATIC_DRAW); // 2.0
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Create and initialize a vertex array object
    glGenVertexArrays(1, &vertexArrayName); // 3.0
    glBindVertexArray(vertexArrayName); // 3.0

    // Specify attribute format
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferNames[POSITION]);
    glVertexAttribPointer(POSITION, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GL_FLOAT), 0); // 3.0
    glVertexAttribPointer(UV, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GL_FLOAT), (void *)(3 * sizeof(GL_FLOAT))); // 3.0

    // Enable the attributes
    glEnableVertexAttribArray(POSITION); // 2.0
    glEnableVertexAttribArray(UV);

    // Disable the vertex array
    glBindVertexArray(0);

    // Load image from file
    GLint width, height, numChannels;
    GLubyte *imageData = stbi_load("texture.png", &width, &height, &numChannels, 3); 

    // Generate texture name
    glGenTextures(1, &textureName);

    // Bind the texture
    glBindTexture(GL_TEXTURE_2D, textureName);
    
    // Specify the format of the texture
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, imageData);

    // Set the sampler parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Generate mip maps
    glGenerateMipmap(GL_TEXTURE_2D);

    // Deactivate texture
    glBindTexture(GL_TEXTURE_2D, 0);


    // Load and compile vertex shader
    GLuint vertexName = glCreateShader(GL_VERTEX_SHADER); // 2.0
    int vertexLength = 0;
    char *vertexSource = readSourceFile("simple_texturing33.vert", &vertexLength);
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
    char *fragmentSource = readSourceFile("simple_texturing33.frag", &fragmentLength);
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

    projectionMatrixPos = glGetUniformLocation(programName, "proj");
    viewMatrixPos = glGetUniformLocation(programName, "view");
    modelMatrixPos = glGetUniformLocation(programName, "model");

    // Enable Opengl depth buffer testing
    glEnable(GL_DEPTH_TEST);

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

    // Change the view matrix
    glm::mat4 view = glm::mat4(1.0f);
    view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
    glUniformMatrix4fv(viewMatrixPos, 1, GL_FALSE, &view[0][0]); // 2.0

    // Change the model matrix
    glm::mat4 model = glm::mat4(1.0);
    model = glm::rotate(model, (float)glfwGetTime() * 0.3f, glm::vec3(0.0f, 1.0f,  0.0f));
    glUniformMatrix4fv(modelMatrixPos, 1, GL_FALSE, &model[0][0]);

    // Activate the vertex array and texture
    glBindVertexArray(vertexArrayName); // 3.0
    glBindTexture(GL_TEXTURE_2D, textureName);

    // Draw the vertex array
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, indices);

    // Disable
    glUseProgram(0);
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);

}

void resizeGL(int width, int height) {

    // Prevent division by zero
    if (height == 0)
        height = 1;										
  
    // Change the projection matrix
    glm::mat4 proj = glm::perspective(3.14f/2.0f, (float)width/height, 0.1f, 100.0f);
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
int main(void) {

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
