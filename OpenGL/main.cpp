//
//  main.cpp
//  OpenGL
//
//  Created by Raphaël Daumas on 06/12/2018.
//

#include <glad/glad.h>
#include <glfw3.h>
#include <GLM/vec3.hpp>

#include <vector>
#include <iostream>
#include <random>
#include <sstream>
#include <fstream>
#include <string>

using namespace std;

static void error_callback(int /*error*/, const char* description)
{
    cerr << "Error: " << description << endl;
}

static void key_callback(GLFWwindow* window, int key, int /*scancode*/, int action, int /*mods*/)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

/* PARTICULES */
struct Particule {
    glm::vec3 position;
    glm::vec3 color;
    glm::vec3 speed;
};

vector<Particule> MakeParticules(const int n)
{
    default_random_engine generator;
    uniform_real_distribution<float> distribution01(0, 1);
    uniform_real_distribution<float> distributionWorld(-1, 1);
    
    vector<Particule> p;
    p.reserve(n);
    
    for(int i = 0; i < n; i++)
    {
        p.push_back(Particule{
            {
                distributionWorld(generator),
                distributionWorld(generator),
                distributionWorld(generator)
            },
            {
                distribution01(generator),
                distribution01(generator),
                distribution01(generator)
            },
            {0.f, 0.f, 0.f}
        });
    }
    
    return p;
}

GLuint MakeShader(GLuint t, string path)
{
//    cout << "the path is : " << path << endl;
    ifstream file(path.c_str(), ios::in);
    ostringstream contents;
    contents << file.rdbuf();
    file.close();
    
    const auto content = contents.str();
//    cout << content << endl;
    
    const auto s = glCreateShader(t);
    
    GLint sizes[] = {(GLint) content.size()};
    const auto data = content.data();
    
    glShaderSource(s, 1, &data, sizes);
    glCompileShader(s);
    
    GLint success;
    glGetShaderiv(s, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        GLchar infoLog[512];
        GLsizei l;
        glGetShaderInfoLog(s, 512, &l, infoLog);
        
        cout << infoLog << endl;
    }
    
    return s;
}

GLuint AttachAndLink(vector<GLuint> shaders)
{
    const auto prg = glCreateProgram();
    for(const auto s : shaders)
    {
        glAttachShader(prg, s);
    }
    
    glLinkProgram(prg);
    
    GLint success;
    glGetProgramiv(prg, GL_LINK_STATUS, &success);
    if(!success)
    {
        GLchar infoLog[512];
        GLsizei l;
        glGetProgramInfoLog(prg, 512, &l, infoLog);
        
        cout << infoLog << endl;
    }
    
    return prg;
}

int main(void)
{
    GLFWwindow* window;
    glfwSetErrorCallback(error_callback);
    
    if (!glfwInit())
        exit(EXIT_FAILURE);
    
//    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
//    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    window = glfwCreateWindow(640, 480, "Simple example", 0, 0);
    
    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    
    glfwSetKeyCallback(window, key_callback);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    // NOTE: OpenGL error checks have been omitted for brevity
    
    if(!gladLoadGL()) {
        cerr << "Something went wrong!" << endl;
        exit(-1);
    }
    
//    size_t nParticules = 10000;
    const size_t nParticules = 500;
    auto particules = MakeParticules(nParticules);
    
    // Shader
    const auto vertex = MakeShader(GL_VERTEX_SHADER, "/Users/Raph/Documents/Gamagora/Synthese/OpenGL/OpenGL/shader.vert");
    const auto fragment = MakeShader(GL_FRAGMENT_SHADER, "/Users/Raph/Documents/Gamagora/Synthese/OpenGL/OpenGL/shader.frag");
    
    const auto program = AttachAndLink({vertex, fragment});
    
    glUseProgram(program);
    
    
    // Buffers
    GLuint vbo, vao;
    glGenBuffers(1, &vbo);
    glGenVertexArrays(1, &vao);
    
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, nParticules * sizeof(Particule), particules.data(), GL_STATIC_DRAW);
    
    // Bindings
    const auto index = glGetAttribLocation(program, "position");
//    const auto indexColor = glGetAttribLocation(program, "color");
    
    glVertexAttribPointer(index, 3, GL_FLOAT, GL_FALSE, sizeof(Particule), nullptr);
    glEnableVertexAttribArray(index);
    
//    glVertexAttribPointer(indexColor, 3, GL_FLOAT, GL_FALSE, sizeof(Particule), nullptr);
//    glEnableVertexAttribArray(indexColor);
    
    glPointSize(5.f);
    
//    while (!glfwWindowShouldClose(window))
    while (nParticules > 0 && !glfwWindowShouldClose(window))
    {
//        nParticules--;
//        particules = MakeParticules(nParticules);
//        float f = 0;
        for (Particule& p : particules)
        {
            p.position = p.position - glm::vec3(.0, .0001, .0);
//            p.color = glm::vec3(f, f, f);
//            f += 0.01;
//            if (f > 1)
//            {
//                f = 0;
//            }
        }
        
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        
        glViewport(0, 0, width, height);
        
        glClear(GL_COLOR_BUFFER_BIT);
//         glClearColor(1.f, 0.0f, 1.f, 1.0f);
        
        glDrawArrays(GL_POINTS, 0, nParticules);
        
        glfwSwapBuffers(window);
        glfwPollEvents();
        
        glBufferSubData(GL_ARRAY_BUFFER, 0, nParticules * sizeof(Particule), particules.data());

    }
    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}

void MovePixel()
{
    
}
