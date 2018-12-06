//
//  main.cpp
//  OpenGL
//
//  Created by Raphaël Daumas on 06/12/2018.
//

#include <glad/glad.h>
#include <glfw3.h>
#include <GLM/vec3.hpp>
#include <STL/stl.h>

#include <vector>
#include <iostream>
#include <random>
#include <sstream>
#include <fstream>
#include <string>

using namespace std;
using namespace glm;

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
            {0.f, 0.f, 0.f},
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

void DrawSTL()
{
    ReadStl("/Users/Raph/Documents/Gamagora/Synthese/OpenGL/OpenGL/logo.stl");
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
    const size_t nParticules = 10000;
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
    const auto indexColor = glGetAttribLocation(program, "color");
    
    glVertexAttribPointer(index, 3, GL_FLOAT, GL_FALSE, sizeof(Particule), nullptr);
    glEnableVertexAttribArray(index);
    
    glVertexAttribPointer(indexColor, 3, GL_FLOAT, GL_FALSE, sizeof(Particule), (void*)sizeof(vec3));
    glEnableVertexAttribArray(indexColor);
    
    glPointSize(5.f);
    
    DrawSTL();
    
    //    while (!glfwWindowShouldClose(window))
    default_random_engine generator;
    while (nParticules > 0 && !glfwWindowShouldClose(window))
    {
        //        nParticules--;
        //        particules = MakeParticules(nParticules);
        
        for (Particule& p : particules)
        {
            uniform_real_distribution<float> distribution01(p.color[0] - .05, p.color[0] + .05);
            uniform_real_distribution<float> distribution012(p.color[1] - .05, p.color[1] + .05);
            uniform_real_distribution<float> distribution013(p.color[2] - .05, p.color[2] + .05);
            
            p.color[0] = distribution01(generator);
            p.color[1] = distribution012(generator);
            p.color[2] = distribution013(generator);
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
