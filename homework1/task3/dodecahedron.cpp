// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <iostream>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>
GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include <common/shader.hpp>

static const float vertices[] = {
        0.469f, 0.469f, 0.469f,
        0.290f, 0.000f, 0.759f,
        -0.759f, -0.290f, 0.000f,
        0.759f, 0.290f, 0.000f,
        -0.469f, 0.469, -0.469f,
        0.000f, -0.759f, -0.290f,
        -0.759f, 0.290f, 0.000f,
        0.469f, -0.469f, 0.469f,
        -0.469f, 0.469f, 0.469f,
        -0.469f, -0.469f, 0.469f,
        0.469f, -0.469f, -0.469f,
        0.290f, 0.000f, -0.759f,
        -0.469f, -0.469f, -0.469f,
        0.000f, -0.759f, 0.290f,
        0.000f, 0.759f, -0.290f,
        -0.290f, 0.000f, 0.759f,
        0.759f, -0.290f, 0.000f,
        -0.290f, 0.000f, -0.759f,
        0.469f, 0.469f, -0.469f,
        0.000f, 0.759f, 0.290f
};

// One color for each vertex. They were generated randomly.
static const float colors[] = {
        0.583f,  0.771f,  0.5f,
        0.1f, 0.750f, 0.2f,
        0.327f,  0.483f,  0.844f,
        0.822f,  0.569f,  0.201f,
        0.435f,  0.602f,  0.223f,
        0.810f,  0.247f,  0.185f,
        0.597f,  0.770f,  0.761f,
        0.559f,  0.07f,  0.730f,
        0.359f,  0.383f,  0.952f,
        0.483f,  0.596f,  0.789f,
        0.278f, 0.861f, 0.429f,
        0.764f, 0.376f, 0.533f,
        0.931f, 0.715f, 0.444f,
        0.692f, 0.674f, 0.264f,
        0.863f, 0.187f, 0.140f,
        0.657f, 0.396f, 0.770f,
        0.500f, 0.946f, 0.405f,
        0.854f, 0.512f, 0.873f,
        0.199f, 0.863f, 0.518f,
        0.984f, 0.875f, 0.100f
};

static const int facets_indices[] = {
        9, 13, 7, 1, 15,
        6, 4, 14, 19, 8,
        12, 5, 13, 9, 2,
        6, 2, 12, 17, 4,
        16, 10, 11, 18, 3,
        19, 8, 15, 1, 0,
        16, 7, 1, 0, 3,
        5, 12, 17, 11, 10,
        18, 14, 4, 17, 11,
        16, 10, 5, 13, 7,
        2, 6, 8, 15, 9,
        19, 0, 3, 18, 14
};

std::vector<float> triangles;
std::vector<float> triangles_colors;

void push_point(int idx) {
    triangles.push_back(vertices[3*idx]);
    triangles.push_back(vertices[3*idx + 1]);
    triangles.push_back(vertices[3*idx + 2]);

    triangles_colors.push_back(colors[3*idx]);
    triangles_colors.push_back(colors[3*idx + 1]);
    triangles_colors.push_back(colors[3*idx + 2]);
};

void push_triangle(int idx1, int idx2, int idx3) {
    push_point(facets_indices[idx1]);
    push_point(facets_indices[idx2]);
    push_point(facets_indices[idx3]);
};

int main( void )
{
    // Initialise GLFW
    if( !glfwInit() )
    {
        fprintf( stderr, "Failed to initialize GLFW\n" );
        getchar();
        return -1;
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Open a window and create its OpenGL context
    window = glfwCreateWindow( 1024, 768, "Tutorial 04 - Colored Cube", NULL, NULL);
    if( window == NULL ){
        fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
        getchar();
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Initialize GLEW
    glewExperimental = true; // Needed for core profile
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        getchar();
        glfwTerminate();
        return -1;
    }

    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    // Dark blue background
    glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    // Accept fragment if it closer to the camera than the former one
    glDepthFunc(GL_LESS);

    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    // Create and compile our GLSL program from the shaders
    GLuint programID = LoadShaders( "vertShader.vert", "fragShader.frag" );

    // Get a handle for our "MVP" uniform
    GLuint MatrixID = glGetUniformLocation(programID, "MVP");

    // Projection matrix : 45� Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
    glm::mat4 Projection = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);
    // Camera matrix
    glm::mat4 View       = glm::lookAt(
            glm::vec3(4,0,3), // Camera is at (4,3,-3), in World Space
            glm::vec3(0,0,0), // and looks at the origin
            glm::vec3(0,1,0)  // Head is up (set to 0,-1,0 to look upside-down)
    );
    // Model matrix : an identity matrix (model will be at the origin)
    glm::mat4 Model      = glm::mat4(1.0f);
    // Our ModelViewProjection : multiplication of our 3 matrices
    glm::mat4 MVP        = Projection * View * Model; // Remember, matrix multiplication is the other way around

    const int num_vertices = 20;
    const int points_per_facet = 5;
    const int num_facets = 12;

    for (int i = 0; i < num_facets; ++i) {
        const int start_idx = 5*i;
        push_triangle( start_idx, start_idx + 1, start_idx + 4);
        push_triangle( start_idx + 1, start_idx + 2, start_idx + 3);
        push_triangle( start_idx + 1, start_idx + 3, start_idx + 4);
    }
    std::cout << triangles.size() << std::endl;
    for (int i = 0; i < triangles.size() / 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            std::cout << triangles[3 * i + j] << " ";
        }
        std::cout << std::endl;
    }

    GLuint vertexbuffer;
    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, triangles.size() * sizeof(float), triangles.data(), GL_STATIC_DRAW);

    GLuint colorbuffer;
    glGenBuffers(1, &colorbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
    glBufferData(GL_ARRAY_BUFFER, triangles_colors.size() * sizeof(float), triangles_colors.data(), GL_STATIC_DRAW);

    do{

        // Clear the screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Use our shader
        glUseProgram(programID);

        // Send our transformation to the currently bound shader,
        // in the "MVP" uniform
        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

        // 1rst attribute buffer : vertices
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glVertexAttribPointer(
                0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
                3,                  // size
                GL_FLOAT,           // type
                GL_FALSE,           // normalized?
                0,                  // stride
                (void*)0            // array buffer offset
        );

        // 2nd attribute buffer : colors
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
        glVertexAttribPointer(
                1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
                3,                                // size
                GL_FLOAT,                         // type
                GL_FALSE,                         // normalized?
                0,                                // stride
                (void*)0                          // array buffer offset
        );

        // Draw the triangle !
        glDrawArrays(GL_TRIANGLES, 0, 3*3*num_facets);

        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);

        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();

    } // Check if the ESC key was pressed or the window was closed
    while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
           glfwWindowShouldClose(window) == 0 );

    // Cleanup VBO and shader
    glDeleteBuffers(1, &vertexbuffer);
    glDeleteBuffers(1, &colorbuffer);
    glDeleteProgram(programID);
    glDeleteVertexArrays(1, &VertexArrayID);

    // Close OpenGL window and terminate GLFW
    glfwTerminate();

    return 0;
}

