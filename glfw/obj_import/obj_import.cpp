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
#define VERTICES 0
#define INDICES 1
#define GLOBAL_MATRICES 2
#define MODEL_MATRIX 3
#define LIGHT_PROPERTIES 4
#define MATERIAL_PROPERTIES 5
#define CAMERA_PROPERTIES 6

// Vertex Array attributes
#define POSITION 0
#define COLOR 1
#define NORMAL 2

// Vertex Array binding points
#define STREAM0 0

// GLSL Uniform indices
#define TRANSFORM0 0
#define TRANSFORM1 1
#define LIGHT 2
#define MATERIAL 3
#define CAMERA 4

// Vertices
GLfloat vertices[] = {
    // Front
    -1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
    -1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
    1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
    1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
    // Back
    1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f,
    1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f,
    -1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f,
    -1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f,
    // Left
    -1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f,
    -1.0f, -1.0f, -1.0f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f,
    -1.0f, -1.0f, 1.0f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f,
    -1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f,
    // Right
    1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
    1.0f, -1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
    1.0f, -1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
    1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
    // Top
    -1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
    -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
    1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
    1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
    // Bottom
    -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f,
    -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f,
    1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f,
    1.0f, -1.0f, 1.0f,  0.0f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f
};

GLushort indices[] {
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

// Light properties (4 valued vectors due to std140 see OpenGL 4.5 reference)
GLfloat lightProperties[] {
    // Position
    0.0f, 0.0f, 4.0f, 0.0f,
    // Ambient Color
    0.0f, 0.0f, 0.2f, 0.0f,
    // Diffuse Color
    0.5f, 0.5f, 0.5f, 0.0f,
    // Specular Color
    0.6f, 0.6f, 0.6f, 0.0f
};

GLfloat materialProperties[] = {
    // Shininess color
    1.0f, 1.0f, 1.0f, 1.0f,
    // Shininess
    32.0f
};

// Camera properties 
GLfloat cameraProperties[] {
    0.0f, 0.0f, 4.0f
};

// Pointers for updating GPU data
GLfloat *projectionMatrixPtr;
GLfloat *viewMatrixPtr;
GLfloat *modelMatrixPtr;

// Names
GLuint programName;
GLuint vertexArrayName;
GLuint vertexBufferNames[7];

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
 * Callback function for OpenGL debug messages 
 */
void glDebugCallback(GLenum sources, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *msg, const void *userParam) {
    printf("DEBUG: %s\n", msg);
}

/*
 * Initialize OpenGL
 */
int initGL() {

    // Register the debug callback function
    glDebugMessageCallback(glDebugCallback, NULL);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_TRUE);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

    // Create and initialize 4 buffer names
    glCreateBuffers(7, vertexBufferNames);

    // Allocate storage for the vertex array buffers
    glNamedBufferStorage(vertexBufferNames[VERTICES], 6 * 4 * 9 * sizeof(GLfloat), vertices, 0);

    // Allocate storage for the triangle indices
    glNamedBufferStorage(vertexBufferNames[INDICES], 3 * 2 * 6 * sizeof(GLshort), indices, 0);

    // Allocate storage for the transformation matrices and retrieve their addresses
    glNamedBufferStorage(vertexBufferNames[GLOBAL_MATRICES], 16 * sizeof(GLfloat) * 2, NULL, GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);
    glNamedBufferStorage(vertexBufferNames[MODEL_MATRIX], 16 * sizeof(GLfloat), NULL, GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);

    // Allocate storage for the buffers used for lighting calculations
    glNamedBufferStorage(vertexBufferNames[LIGHT_PROPERTIES], 16 * sizeof(GLfloat), lightProperties, 0);
    glNamedBufferStorage(vertexBufferNames[MATERIAL_PROPERTIES], 5 * sizeof(GLfloat), materialProperties, 0);
    glNamedBufferStorage(vertexBufferNames[CAMERA_PROPERTIES], 3 * sizeof(GLfloat), cameraProperties, 0);

    // Get a pointer to the global matrices data
    GLfloat *globalMatricesPtr = (GLfloat *)glMapNamedBufferRange(vertexBufferNames[GLOBAL_MATRICES], 0, 16 * sizeof(GLfloat) * 2, 
            GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
    projectionMatrixPtr = globalMatricesPtr;
    viewMatrixPtr = globalMatricesPtr + 16;

    // Get a pointer to the model matrix data
    modelMatrixPtr = (GLfloat *)glMapNamedBufferRange(vertexBufferNames[MODEL_MATRIX], 0, 16 * sizeof(GLfloat), 
            GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);

    // Create and initialize a vertex array object
    glCreateVertexArrays(1, &vertexArrayName);

    // Associate attributes with binding points
    glVertexArrayAttribBinding(vertexArrayName, POSITION, STREAM0);
    glVertexArrayAttribBinding(vertexArrayName, COLOR, STREAM0);
    glVertexArrayAttribBinding(vertexArrayName, NORMAL, STREAM0);

    // Specify attribute format
    glVertexArrayAttribFormat(vertexArrayName, POSITION, 3, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayAttribFormat(vertexArrayName, COLOR, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT));
    glVertexArrayAttribFormat(vertexArrayName, NORMAL, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GL_FLOAT));

    // Enable the attributes
    glEnableVertexArrayAttrib(vertexArrayName, POSITION);
    glEnableVertexArrayAttrib(vertexArrayName, COLOR);
    glEnableVertexArrayAttrib(vertexArrayName, NORMAL);

    // Bind the indices to the vertex array
    glVertexArrayElementBuffer(vertexArrayName, vertexBufferNames[INDICES]);

    // Bind the vertex buffer to the vertex array
    glVertexArrayVertexBuffer(vertexArrayName, STREAM0, vertexBufferNames[VERTICES], 0, 9 * sizeof(GLfloat));

    // Load and compile vertex shader
    GLuint vertexName = glCreateShader(GL_VERTEX_SHADER);
    int vertexLength = 0;
    char *vertexSource = readSourceFile("simple_lighting.vert", &vertexLength);
    glShaderSource(vertexName, 1, (const char * const *)&vertexSource, &vertexLength);
    GLint compileStatus;
    glCompileShader(vertexName);
    glGetShaderiv(vertexName, GL_COMPILE_STATUS, &compileStatus);
    if (!compileStatus) {
        GLint logSize = 0;
        glGetShaderiv(vertexName, GL_INFO_LOG_LENGTH, &logSize);
        char *errorLog = (char *)malloc(sizeof(char) * logSize);
        glGetShaderInfoLog(vertexName, logSize, &logSize, errorLog);
        glDeleteShader(vertexName);
        printf("VERTEX ERROR %s\n", errorLog);
        return 0;
    }
    free(vertexSource);

    // Load and compile fragment shader
    GLuint fragmentName = glCreateShader(GL_FRAGMENT_SHADER);
    int fragmentLength = 0;
    char *fragmentSource = readSourceFile("simple_lighting.frag", &fragmentLength);
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
    programName = glCreateProgram();
    glAttachShader(programName, vertexName);
    glAttachShader(programName, fragmentName);
    glLinkProgram(programName);
    GLint linkStatus;
    glGetProgramiv(programName, GL_LINK_STATUS, &linkStatus);
    if (!linkStatus) {
        GLint logSize = 0;
        glGetProgramiv(programName, GL_INFO_LOG_LENGTH, &logSize);
        char *errorLog = (char *)malloc(sizeof(char) * logSize);
        glGetProgramInfoLog(programName, logSize, &logSize, errorLog);

        printf("LINK ERROR %s\n", errorLog);
        return 0;
    }

    // Enable depth buffer testing
    glEnable(GL_DEPTH_TEST);

    return 1;

}


/*
 * Draw OpenGL screne
 */
void drawGLScene() {

    // Clear color and depth buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Set the view matrix
    glm::mat4 view = glm::mat4(1.0f);
    view = glm::translate(view, glm::vec3(-cameraProperties[0], -cameraProperties[1], -cameraProperties[2]));
    memcpy(viewMatrixPtr, &view[0][0], 16 * sizeof(GLfloat));

    // Set the model matrix
    glm::mat4 model = glm::mat4(1.0);
    model = glm::rotate(model, (float)glfwGetTime() * 0.3f, glm::vec3(0.0f, 1.0f,  0.0f));
    memcpy(modelMatrixPtr, &model[0][0], 16 * sizeof(GLfloat));

    // Activate the program
    glUseProgram(programName);

    // Activate the vertex array
    glBindVertexArray(vertexArrayName);

    // Bind buffers to GLSL uniform indices
    glBindBufferBase(GL_UNIFORM_BUFFER, TRANSFORM0, vertexBufferNames[GLOBAL_MATRICES]);
    glBindBufferBase(GL_UNIFORM_BUFFER, TRANSFORM1, vertexBufferNames[MODEL_MATRIX]);
    glBindBufferBase(GL_UNIFORM_BUFFER, LIGHT, vertexBufferNames[LIGHT_PROPERTIES]);
    glBindBufferBase(GL_UNIFORM_BUFFER, MATERIAL, vertexBufferNames[MATERIAL_PROPERTIES]);
    glBindBufferBase(GL_UNIFORM_BUFFER, CAMERA, vertexBufferNames[CAMERA_PROPERTIES]);

    // Draw the vertex array
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, 0);

    // Disable
    glUseProgram(0);
    glBindVertexArray(0);

}

void resizeGL(int width, int height) {

    // Prevent division by zero
    if (height == 0)
        height = 1;										

    // Change the projection matrix
    glm::mat4 proj = glm::perspective(3.14f/2.0f, (float)width/height, 0.1f, 100.0f);
    memcpy(projectionMatrixPtr, &proj[0][0], 16 * sizeof(GLfloat));

    // Set the OpenGL viewport
    glViewport(0, 0, width, height);

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
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);

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
