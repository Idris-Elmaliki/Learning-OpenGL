#include <iostream>
#include <cassert> // for assert
#include <cmath> 

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "readFiles/readFile.hpp" 
#include <openglErrorReporting.h>
//#include <gl2d/gl2d.h>
//#include "imgui.h"
//#include "backends/imgui_impl_glfw.h"
//#include "backends/imgui_impl_opengl3.h"
//#include "imguiThemes.h"

static void error_callback(int error, const char *description)
{
	std::cout << "Error: " <<  description << "\n";
}

GLuint VAO, VBO, shader, uniformXMove;

bool direction = true; 
float triOffset = 0.0f, triMaxoffSet = 0.7f, triIncrement = 0.005f; 


static void CreateTriangle(GLfloat* vertices) // don't forget arrays downgrade to pointers!
{
    // generating and binding the VAO
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO); 

    // generating and binding the VBO
    glGenBuffers(1, &VBO); 
    glBindBuffer(GL_ARRAY_BUFFER, VBO); 
    glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(GLfloat), vertices, GL_STATIC_DRAW);
    // GL_STATIC_DRAW is essentially telling OpenGL that we won't be changing this shader around (That is what GL_DYNAMIC_DRAW is for)!

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, false, 0); 
    // stride allows us to combine the data for a fragment and vertex shader into a single vertex
    // With the data of where we start [final parameter]
    //stride would determine what is what by skipping every value of data by of course what the type is and how much each "row" has
 
    glEnableVertexAttribArray(0); 

    // This unbinds the VBO and VAO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0); 
} 

void AddShaders(
    GLuint program, 
    const std::string& shaderCode, 
    GLenum shaderType
) 
{
    GLuint shader = glCreateShader(shaderType); 

    const GLchar* shadersCode[] = { shaderCode.c_str()};
    GLint shaderCodeLen[] = { shaderCode.size() };

    glShaderSource(shader, 1, shadersCode, shaderCodeLen);
    glCompileShader(shader); 

    GLint res = 1;
    GLchar errLog[1024] = { 0 }; 

    // We call these function as we are currently making the shaders!
    glGetShaderiv(shader, GL_COMPILE_STATUS, &res);

    if (res == GL_FALSE) {
        glGetShaderInfoLog(shader, sizeof(errLog), nullptr, errLog);
        std::cout << "Shader Compiling Error for the " << shaderType << " shader: \n" << errLog << '\n';

        assert(res == GL_TRUE);
    }

    glAttachShader(program, shader);
}

void CompileShader(const ShaderSource& shaders) 
{
    shader = glCreateProgram();

    if (shader == GL_FALSE) {
        assert(shader == GL_TRUE); 
    }

    AddShaders(shader, shaders.vertexShader, GL_VERTEX_SHADER);
    AddShaders(shader, shaders.fragmentShader, GL_FRAGMENT_SHADER);
  
    GLint res = 1; 
    GLchar errLog[1024] = {0}; // We make a char* as that is what we need to pass in
    
    glLinkProgram(shader); 

    // This is how we debug in OpenGL!
    glGetProgramiv(shader, GL_LINK_STATUS, &res); // we get the program info (link status in this case) and store the result in res

    // if res is false, then we will get the log and print it to the console!

    if (res == GL_FALSE) {
        glGetProgramInfoLog(shader, sizeof(errLog), nullptr, errLog);
        std::cout << "Shader Linking Error: " << errLog << '\n'; 

        assert(res == GL_TRUE);
    }

    glValidateProgram(shader); 
    glGetProgramiv(shader, GL_VALIDATE_STATUS, &res);

    if (res == GL_FALSE) {
        glGetProgramInfoLog(shader, sizeof(errLog), nullptr, errLog);
        std::cout << "Shader Validation Error: " << errLog << '\n';

        assert(res == GL_TRUE);
    }

    // This givs us the location of where the uniform variable we are looking for is
    uniformXMove = glGetUniformLocation(shader, "xMove"); 
}

int main(void)
{
	glfwSetErrorCallback(error_callback);
	
    /* Initialize the library */
    if (!glfwInit())
        return -1;
  
    /* Create a windowed mode window and its OpenGL context */
    GLFWwindow* window = glfwCreateWindow(1920, 1080, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        glfwTerminate();
        return -1;
    }

    std::cout << "OpenGL version: " << glGetString(GL_VERSION) << '\n';
    std::cout << GLAD_GL_VERSION_4_6 << '\n';

    enableReportGlErrors();

    GLfloat vertices[] = {
        -1.0f, -1.0f, 0.0f, 
         1.0f, -1.0f, 0.0f, 
         0.0f,  1.0f, 0.0f, 
     //   x      y     z
    };

    ShaderSource shaders = getShaders(
        "src/shaders/vertex.shader", 
        "src/shaders/fragment.shader"
    ); 

    CreateTriangle(vertices); 
    CompileShader(shaders);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Poll for and process events */
        glfwPollEvents();

        if (direction)
        {
            triOffset += triIncrement;
        }
        else
        {
            triOffset -= triIncrement;
        }

        if (abs(triOffset) >= triMaxoffSet) {
            direction = !direction;
        }

        // chnages the background color
        glClearColor(0.0f, 0.5f, 0.5f, 0.25f);

        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shader);

        // This binds the uniform in the shader with our current data
        glUniform1f(uniformXMove, triOffset);

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glBindVertexArray(0);

        glUseProgram(0);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);
    }

    glfwDestroyWindow(window);
    glfwTerminate();
	
	//glfwSwapInterval(1); //vsync

	return 0;
}
