#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <cmath>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 800;

const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec2 aTexCoord;\n"

"out vec2 TexCoord;\n"

"uniform mat4 transform;\n"
"uniform mat4 projection;\n"

"void main()\n"
"{\n"
"    gl_Position = projection * transform * vec4(aPos, 1.0f);\n"
"    TexCoord = vec2(aTexCoord.x, aTexCoord.y);\n"
"}\0";


const char* fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"uniform vec4 ourColor;\n"
"void main()\n"
"{\n"
"   FragColor = ourColor;\n"
"}\n\0";

int main()
{
    // GLFW initialize and configure
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // GLFW window creation
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);


    // load all opengl function pointers GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // build and compile our shader program
    // vertex shader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    // check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // link shaders
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // set up vertex data 
    // ------------------
    float vertices[] = {
         0.25f,  0.25f, 0.0f,  // top right
         0.25f, -0.25f, 0.0f,  // bottom right
        -0.25f, -0.25f, 0.0f,  // bottom left
        -0.25f,  0.25f, 0.0f   // top left 
    };

    unsigned int indices[] = {  // start from 0
        0, 1, 3,  // first Triangle
        1, 2, 3   // second Triangle
    };

    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // wireframe polygons
  
    // Render loop
    while (!glfwWindowShouldClose(window))
    {
        // Process input
        processInput(window);

        // background color
        // All 0.0 = black backgorund
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Use the shader program
        glUseProgram(shaderProgram);

        // Bind the VAO
        glBindVertexArray(VAO);

        // Create a projection matrix
        // Using orthographic projection due to not needing perspective with 2D objects
        glm::mat4 projection = glm::ortho(-2.0f, 2.0f, -2.0f, 2.0f, -1.0f, 1.0f);
        unsigned int projectionLoc = glGetUniformLocation(shaderProgram, "projection");
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));


        // Initializing matrix diagonals to 1.0
        // Initializing identity matrix
        // First Square
        glm::mat4 square = glm::mat4(1.0f);
        unsigned int transformLoc = glGetUniformLocation(shaderProgram, "transform");
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(square));

        int vertexColorLocation = glGetUniformLocation(shaderProgram, "ourColor");
        glUniform4f(vertexColorLocation, 1.0f, 0.0f, 0.0f, 1.0f);  // Red color
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);


        // Square translated
        glm::mat4 square_trans = glm::mat4(1.0f);
        square_trans = glm::translate(square_trans, glm::vec3(1.5f, 0.0, 0.0f));

        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(square_trans));
        glUniform4f(vertexColorLocation, 0.0f, 1.0f, 0.0f, 1.0f);  // Green color
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);


        // Scaled Square
        glm::mat4 square_scale = glm::mat4(1.0f);
        square_scale = glm::scale(square_scale, glm::vec3(1.5f, 1.5f, 1.0f));

        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(square_scale));
        glUniform4f(vertexColorLocation, 0.0f, 0.0f, 1.0f, 1.0f);  // Blue color
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);


        // Rotated Square
        //float angle = (float)glfwGetTime(); // For it to spin
        glm::mat4 square_angle = glm::mat4(1.0f);
        float angle = 0.785398;
        square_angle = glm::rotate(square_angle, angle, glm::vec3(0.0f, 0.0f, 1.0f));

        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(square_angle));
        glUniform4f(vertexColorLocation, 1.0f, 0.0f, 1.0f, 1.0f);  // Purple Color
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);


        // Skewed Square
        glm::mat4 square_shear = glm::mat4(1.0f);
        square_shear = glm::scale(square_shear, glm::vec3(0.5f, 0.5f, 1.0f));

        square_shear[1][0] = 0.5f; // shearing square along x axis by 0.5
        square_shear[0][1] = 0.2f; // shearing square along y axis by 0.2

        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(square_shear));
        glUniform4f(vertexColorLocation, 1.0f, 1.0f, 0.0f, 1.0f);  // Yellow color
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        // Swap buffers and poll events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }



    // deallocate all resources once finished
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shaderProgram);

    // clear all previsouly allocated glfw resources
    glfwTerminate();
    return 0;
}

// process all input. query GLFW if relevant keys are pressed and to react accordingly
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// callback function that executes when window size changes by user or OS
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}