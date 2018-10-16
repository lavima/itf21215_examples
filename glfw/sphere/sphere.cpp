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

// Vertex Buffer Identifiers
#define GLOBAL_MATRICES 0
#define MODEL_MATRIX 1
#define LIGHT_PROPERTIES 2
#define MATERIAL_PROPERTIES 3
#define CAMERA_PROPERTIES 4
#define VERTICES 5
#define INDICES 6

// Vertex Array attributes
#define POSITION 0
#define NORMAL 1
#define UV 2

// Vertex Array binding points
#define STREAM0 0

// GLSL Uniform indices
#define TRANSFORM0 0
#define TRANSFORM1 1
#define LIGHT 2
#define MATERIAL 3
#define CAMERA 4

// Light properties (4 valued vectors due to std140 see OpenGL 4.5 reference)
GLfloat lightProperties[] {
    // Position
    0.0f, 0.0f, 10.0f, 0.0f,
    // Ambient Color
    0.4f, 0.4f, 0.4f, 0.0f,
    // Diffuse Color
    0.7f, 0.5f, 0.5f, 0.0f,
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
    // Position 
    0.0f, 0.0f, 10.0f
};

// Pointers for updating GPU data
GLfloat *projectionMatrixPtr;
GLfloat *viewMatrixPtr;
GLfloat *modelMatrixPtr;

// Names
GLuint programName;
GLuint vertexBufferNames[7];
GLuint vertexArrayName;
GLuint textureName;

// Global variable to store the number of indices in the generated sphere
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

    // Create and initialize buffer names
    glCreateBuffers(7, vertexBufferNames);

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
    GLuint vertexName = glCreateShader(GL_VERTEX_SHADER);
    int vertexLength = 0;
    char *vertexSource = readSourceFile("default.vert", &vertexLength);
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
    char *fragmentSource = readSourceFile("default.frag", &fragmentLength);
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

int createSphere(float radius, int numDiv) {

    printf("Creating Sphere %f %d\n", radius, numDiv);

    // Variables needed for the calculations
    float t1, t2;
    float pi = glm::pi<float>();
    float pi2 = pi * 2.0f;
    float d = pi2 / numDiv;

    // Allocate the data needed to store the necessary positions, normals and texture coordinates
    int numVertices = (numDiv*(numDiv-1)+2);
    int numFloats = (3+3+2);
    int numTotal = numVertices * numFloats;
    printf("TotalFloats %d\n", numTotal);
    GLfloat vertexData[numTotal];

    // Position
    vertexData[0] = 0.0f; vertexData[1] = radius; vertexData[2] = 0.0f;
    // Normal
    vertexData[3] = 0.0f; vertexData[4] = 1.0f; vertexData[5] = 0.0f;
    // UV
    vertexData[6] = 0.5f; vertexData[7] = 1.0f;
    for (int j=0; j<numDiv-1; j++) {
        for (int i=0; i<numDiv; i++) {
            // Position
            vertexData[(j*numDiv+i+1)*numFloats] = radius * glm::sin(i*d) * glm::sin((j+1)*d);
            vertexData[(j*numDiv+i+1)*numFloats+1] = radius * glm::cos((j+1) * d);
            vertexData[(j*numDiv+i+1)*numFloats+2] = radius * glm::cos(i*d) * glm::sin((j+1)*d);
            // Normal
            vertexData[(j*numDiv+i+1)*numFloats+3] = glm::sin(i*d) * glm::sin((j+1)*d);
            vertexData[(j*numDiv+i+1)*numFloats+4] = glm::cos((j+1)*d);
            vertexData[(j*numDiv+i+1)*numFloats+5] = glm::cos(i*d)*glm::cos((j+1)*d);
            // UV
            vertexData[(j*numDiv+i+1)*numFloats+6] = glm::asin(vertexData[(j*numDiv+i+1)*numFloats+3]) / pi + 0.5f;
            vertexData[(j*numDiv+i+1)*numFloats+7] = glm::asin(vertexData[(j*numDiv+i+1)*numFloats+4]) / pi + 0.5f;
        }
    }
    // Position
    vertexData[(numVertices-1)*numFloats] = 0.0f; vertexData[(numVertices-1)*numFloats+1] = -radius; vertexData[(numVertices-1)*numFloats+2] = 0.0f;
    // Normal
    vertexData[(numVertices-1)*numFloats+3] = 0.0f; vertexData[(numVertices-1)*numFloats+4] = -1.0f; vertexData[(numVertices-1)*numFloats+5] = 0.0f;
    // UV
    vertexData[(numVertices-1)*numFloats+6] = 0.5f; vertexData[(numVertices-1)*numFloats+7] = 0.0f;

    // Allocate the data needed to store the indices
    int numTriangles = (numDiv*(numDiv-1)*2);
    numIndices = numTriangles * 3;
    printf("NumIndices %d\n", numIndices);

    GLushort indexData[numIndices];

    for (int i=0; i<numDiv; i++) {
        indexData[i*3] = 0; indexData[i*3+1] = (GLushort)(i+1); indexData[i*3+2] = (GLushort)((i+1)%numDiv+1);
    }
    for (int j=0; j<numDiv-2; j++) {
        for (int i=0; i<numDiv; i++) {
            indexData[((j*numDiv+i)*2+numDiv)*3] = (GLushort)(j*numDiv+i+1);
            indexData[((j*numDiv+i)*2+numDiv)*3+1] = (GLushort)((j+1)*numDiv+i+1);
            indexData[((j*numDiv+i)*2+numDiv)*3+2] = (GLushort)((j+1)*numDiv+(i+1)%numDiv+1);

            indexData[((j*numDiv+i)*2+numDiv)*3+3] = (GLushort)((j+1)*numDiv+(i+1)%numDiv+1);
            indexData[((j*numDiv+i)*2+numDiv)*3+4] = (GLushort)(j*numDiv+(i+1)%numDiv+1);
            indexData[((j*numDiv+i)*2+numDiv)*3+5] = (GLushort)(j*numDiv+i+1);
        }
    }
    int trianglIndex = (numTriangles-numDiv);
    int vertIndex = (numDiv-2)*numDiv+1;
    for (short i=0; i<numDiv; i++) {
        indexData[(trianglIndex+i)*3] = (GLushort)(vertIndex+i);
        indexData[(trianglIndex+i)*3+1] = (GLushort)((numDiv*(numDiv-1)+1));
        indexData[(trianglIndex+i)*3+2] = (GLushort)(vertIndex+(i+1)%numDiv);
    }


    // Create a vertex buffer with the vertex data
    glCreateBuffers(2, &vertexBufferNames[VERTICES]);
    glNamedBufferStorage(vertexBufferNames[VERTICES], numTotal * sizeof(GLfloat), vertexData, GL_STATIC_DRAW);
    glNamedBufferStorage(vertexBufferNames[INDICES], numIndices * sizeof(GLushort), indexData, GL_STATIC_DRAW);

    // Create and initialize a vertex array object
    glCreateVertexArrays(1, &vertexArrayName);

    // Associate vertex attributes with the binding point (POSITION NORMAL UV)
    glVertexArrayAttribBinding(vertexArrayName, POSITION, STREAM0);
    glVertexArrayAttribBinding(vertexArrayName, NORMAL, STREAM0);
    glVertexArrayAttribBinding(vertexArrayName, UV, STREAM0);
    // Enable the attributes
    glEnableVertexArrayAttrib(vertexArrayName, POSITION);
    glEnableVertexArrayAttrib(vertexArrayName, NORMAL);
    glEnableVertexArrayAttrib(vertexArrayName, UV);

    // Specify the format of the attributes
    glVertexArrayAttribFormat(vertexArrayName, POSITION, 3, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayAttribFormat(vertexArrayName, NORMAL, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT));
    glVertexArrayAttribFormat(vertexArrayName, UV, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(GL_FLOAT));

    // Bind the vertex data buffer to the vertex array
    glVertexArrayVertexBuffer(vertexArrayName, STREAM0, vertexBufferNames[VERTICES], 0, 8 * sizeof(GLfloat));

    // Bind the indices to the vertex array
    glVertexArrayElementBuffer(vertexArrayName, vertexBufferNames[INDICES]);

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
    view = glm::translate(view, -glm::vec3(cameraProperties[0], cameraProperties[1], cameraProperties[2]));
    memcpy(viewMatrixPtr, &view[0][0], 16 * sizeof(GLfloat));

    // Set the model matrix
    glm::mat4 model = glm::mat4(1.0);
    model = glm::rotate(model, (float)glfwGetTime() * 0.3f, glm::vec3(0.0f, 1.0f,  0.0f));
    memcpy(modelMatrixPtr, &model[0][0], 16 * sizeof(GLfloat));

    // Activate the program
    glUseProgram(programName);

    // Bind buffers to GLSL uniform indices
    glBindBufferBase(GL_UNIFORM_BUFFER, TRANSFORM0, vertexBufferNames[GLOBAL_MATRICES]);
    glBindBufferBase(GL_UNIFORM_BUFFER, TRANSFORM1, vertexBufferNames[MODEL_MATRIX]);
    glBindBufferBase(GL_UNIFORM_BUFFER, LIGHT, vertexBufferNames[LIGHT_PROPERTIES]);
    glBindBufferBase(GL_UNIFORM_BUFFER, MATERIAL, vertexBufferNames[MATERIAL_PROPERTIES]);
    glBindBufferBase(GL_UNIFORM_BUFFER, CAMERA, vertexBufferNames[CAMERA_PROPERTIES]);

    // Bind the vertex array and texture of the sphere
    glBindVertexArray(vertexArrayName);
    glBindTexture(GL_TEXTURE_2D, textureName);

    // Draw the vertex array
    glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_SHORT, 0);

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
int main(int nargs, const char **argv) {

    // Ensure that there is one argument (besides the program name)
    if (nargs != 3) {
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

    // Load the OBJ-file
    if (!createSphere(atof(argv[1]), atoi(argv[2]))) {
        printf("Failed to load %s.\n", argv[1]);  
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
