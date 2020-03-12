// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <vector>

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
//#include <glm/gtc/constants.hpp>
//#include <glm/gtc/constants.inl>

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
    window = glfwCreateWindow( 1024, 768, "Tutorial 02 - Red triangle", NULL, NULL);
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

    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    // Create and compile our GLSL program from the shaders
    GLuint firstProgramID = LoadShaders( "vertShader.vert", "firstFragShader.frag" );
    GLuint secondProgramID = LoadShaders( "vertShader.vert", "secondFragShader.frag" );

    GLuint firstMatrixID = glGetUniformLocation(firstProgramID, "MVP");
    GLuint secondMatrixID = glGetUniformLocation(secondProgramID, "MVP");

    const GLuint programs[] = { firstProgramID, secondProgramID };
    const GLuint matrices[] = { firstMatrixID, secondMatrixID };

    static const GLfloat g_vertex_buffer_data[] = {
            0.0f, 0.4f, 0.0f,
            0.5f, -0.6f, 0.0f,
            -0.5f, -0.6f, 0.0f,

            -0.5f, 0.6f, 0.01f,
            0.5f, 0.6f, 0.01f,
            0.0f, -0.4f, 0.01f,
    };

    GLuint vertexbuffer;
    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

    const int N = 100;
    const float pi = 3.14159;
    std::vector<float> angle_values;
    for (int i = 0; i < N; ++i) {
        angle_values.push_back( i * 2 * pi / N );
    }

    const int first_coord[] = {0, 3};
    int i = 0;
    const float radius = 2.5f;
    do{
        // Clear the screen
        glClear( GL_COLOR_BUFFER_BIT );

        glm::mat4 Projection = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);
        const float phi = angle_values[i];
        glm::mat4 View = glm::lookAt(
                glm::vec3(radius * cos(phi),0.0f,radius * sin(phi)), // Camera is at (4,3,3), in World Space
                glm::vec3(0,0,0), // and looks at the origin
                glm::vec3(0,1,0)  // Head is up (set to 0,-1,0 to look upside-down)
        );
        glm::mat4 Model = glm::mat4(1.0f);
        glm::mat4 MVP = Projection * View * Model;

        for (int i = 0; i < 2; ++i) {
            glUseProgram(programs[i]);

            // Send our transformation to the currently bound shader,
            // in the "MVP" uniform
            glUniformMatrix4fv(matrices[i], 1, GL_FALSE, &MVP[0][0]);

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

            // Draw the triangle !
            glDrawArrays(GL_TRIANGLES, first_coord[i], 3); // 3 indices starting at 0 -> 1 triangle

            glDisableVertexAttribArray(0);
        }

        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
        i = (i + 1) % N;
    } // Check if the ESC key was pressed or the window was closed
    while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
           glfwWindowShouldClose(window) == 0 );

    // Cleanup VBO
    glDeleteBuffers(1, &vertexbuffer);
    glDeleteVertexArrays(1, &VertexArrayID);
    glDeleteProgram(firstProgramID);
    glDeleteProgram(secondProgramID);
    // Close OpenGL window and terminate GLFW
    glfwTerminate();

    return 0;
}

