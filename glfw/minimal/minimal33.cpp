#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#define DEFAULT_WIDTH 1024
#define DEFAULT_HEIGHT 768

// Vertex Buffer Identifiers
#define POSITION 0
#define COLOR 1

// GLSL Uniform locations
#define PROJECTION_MATRIX 0
#define VIEW_MATRIX 1
#define MODEL_MATRIX 2

// Vertices
GLfloat vertices[] = {
    -1.0f, -1.0f, 0.0f,
    1.0f, -1.0f, 0.0f,
    0.0f, 2.0f, 0.0f };
// Colors
GLfloat colors[] = { 
    1.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 1.0f };

// Uniform locations
GLint projectionMatrixPos;
GLint viewMatrixPos;
GLint modelMatrixPos;

// Names
GLuint programName;
GLuint vertexArrayName;
GLuint vertexBufferNames[2];

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
    glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(GLfloat), vertices, GL_STATIC_DRAW); // 2.0
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferNames[COLOR]);
    glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(GLfloat), colors, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Create and initialize a vertex array object
    glGenVertexArrays(1, &vertexArrayName); // 3.0
    glBindVertexArray(vertexArrayName); // 3.0

    // Specify attribute format
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferNames[POSITION]);
    glVertexAttribPointer(POSITION, 3, GL_FLOAT, GL_FALSE, 0, 0); // 3.0
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferNames[COLOR]);
    glVertexAttribPointer(COLOR, 3, GL_FLOAT, GL_FALSE, 0, 0);

    // Enable the attributes
    glEnableVertexAttribArray(POSITION); // 2.0
    glEnableVertexAttribArray(COLOR);

    // Disable the vertex array
    glBindVertexArray(0);

    // Load and compile vertex shader
    GLuint vertexName = glCreateShader(GL_VERTEX_SHADER); // 2.0
    int vertexLength = 0;
    char *vertexSource = readSourceFile("minimal33.vert", &vertexLength);
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
    char *fragmentSource = readSourceFile("minimal33.frag", &fragmentLength);
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

    return 1;

}

/*
 * Draw OpenGL screne
 */
void drawGLScene() {

    // Clear color and depth buffers
    glClear(GL_COLOR_BUFFER_BIT);

    // Activate the program
    glUseProgram(programName); // 2.0

    // Change the view matrix
    GLfloat view[16] = {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1 };
    glUniformMatrix4fv(viewMatrixPos, 1, GL_FALSE, view); // 2.0

    // Change the model matrix
    GLfloat scale[16] = {
        0.5, 0, 0, 0,
        0, 0.5, 0, 0,
        0, 0, 0.5, 0,
        0, 0, 0, 1 };
    glUniformMatrix4fv(modelMatrixPos, 1, GL_FALSE, scale);

    // Activate the vertex array
    glBindVertexArray(vertexArrayName); // 3.0

    // Draw the vertex array
    glDrawArrays(GL_TRIANGLES, 0, 3); // 2.0

    // Disable
    glUseProgram(0);
    glBindVertexArray(0);

}

void resizeGL(int width, int height) {

    // Prevent division by zero
    if (height == 0)
        height = 1;										
  
    // Change the projection matrix
    GLfloat projection[16] = {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1 };
    glm::mat4 proj= glm::ortho(-1, 1, -1, 1, -1, 1);
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
