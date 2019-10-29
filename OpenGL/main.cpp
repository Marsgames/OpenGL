//
//  main.cpp
//  OpenGL
//
//  Created by Raphaël Daumas on 06/12/2018.
//

//#include <Library/glad/glad.h>
#include "Library/glad/glad.h"
#include <glfw3.h>
//#include <Library/GLM/vec3.hpp>
#include "Library/GLM/vec3.hpp"
#include "Library/STL/stl.h"
#include "main.hpp"

#include <vector>
#include <iostream>
#include <random>
#include <sstream>
#include <fstream>
#include <string>

using std::endl;
using std::vector;
using std::default_random_engine;
using std::uniform_real_distribution;
using std::string;
using std::cout;


int main(void)
{
    srand(0);
    
    GLFWwindow* window;
//    GLFWwindow* window2;
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
//    window2 = glfwCreateWindow(640, 480, "Simple example2", 0, 0);
    
    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
//    if (!window2)
//    {
//        glfwTerminate();
//        exit(EXIT_FAILURE);
//    }
    
    glfwSetKeyCallback(window, key_callback);
    glfwMakeContextCurrent(window);
    
//    glfwSetKeyCallback(window2, key_callback);
//    glfwMakeContextCurrent(window2);
    
    glfwSwapInterval(1);
    // NOTE: OpenGL error checks have been omitted for brevity
    
    if(!gladLoadGL()) {
        std::cerr << "Something went wrong!" << endl;
        exit(-1);
    }
    
    
    /////////////////////////////////////////////////////////////////////////
    ///
    ///
    ///
    /////////////////////////////////////////////////////////////////////////
    
        cout << "0 : DrawParticle(clear)" << endl << "1 : DrawSTL(logo)" << endl << "2 : DrawParticle(don't clear)" << endl;
        int action = 0;
    std::cin >> action;
    
    GLenum type = GL_POINTS;
    GLint startIndex = 0;
    GLsizei count;
//    GLsizei count2;
    
        switch (action) {
            case 0:
                count = DrawParticle(window, true);
                type = GL_POINTS;
                break;
            case 1:
    count = DrawSTL("logo.stl", window);
    type = GL_TRIANGLES;
                break;
                
            case 2:
                count = DrawParticle(window, false);
                type = GL_POINTS;
                break;
    
            default:
                count = DrawParticle(window, true);
                break;
        }
    
    while (!glfwWindowShouldClose(window))// && !glfwWindowShouldClose(window2))
    {
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
//        glfwGetFramebufferSize(window2, &width, &height);
        
        glViewport(0, 0, width, height);
        
        glClear(GL_COLOR_BUFFER_BIT);
        // glClearColor(1.f, 0.0f, 1.f, 1.0f);
        
        
        glDrawArrays(type, startIndex, count);
        
        glfwSwapBuffers(window);
//        glfwSwapBuffers(window2);
        glfwPollEvents();
        
        // glBufferSubData(GL_ARRAY_BUFFER, 0, logoArray.size() * sizeof(Triangle), logoArray.data());
        
    }
    glfwDestroyWindow(window);
//    glfwDestroyWindow(window2);
    glfwTerminate();
    exit(EXIT_SUCCESS);
    
}

static void error_callback(int /*error*/, const char* description)
{
    std::cerr << "Error: " << description << endl;
}

static void key_callback(GLFWwindow* window, int key, int /*scancode*/, int action, int /*mods*/)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

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
    std::ifstream file(path.c_str(), std::ios::in);
    std::ostringstream contents;
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

GLsizei DrawParticle(GLFWwindow* window, const bool clear)
{
    const size_t nParticules = 10000;
    //    const size_t nParticules = 5000;

    auto particules = MakeParticules(nParticules);

    // Shader
    const auto vertex = MakeShader(GL_VERTEX_SHADER, ShaderString + "shader.vert");
    const auto fragment = MakeShader(GL_FRAGMENT_SHADER, ShaderString + "shader.frag");

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

    //    glPointSize(5.f);
    glPointSize(1.f);


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
            //            uniform_real_distribution<float> distribution01(0.25, 0.75);
            //            uniform_real_distribution<float> distribution012(0.25, .75);
            //            uniform_real_distribution<float> distribution013(0.25, .75);
            uniform_real_distribution<float> distribution014(-0.0005, 0.0005);


            p.color[0] = distribution01(generator);
            p.color[1] = distribution012(generator);
            p.color[2] = distribution013(generator);

            p.position = p.position - vec3(distribution014(generator), .0001, 0);
            if (p.position[1] < -1)
            {
                p.position[1] = 1;
            }
        }

        int width, height;
        glfwGetFramebufferSize(window, &width, &height);

        glViewport(0, 0, width, height);

        if (clear)
        {
            glClear(GL_COLOR_BUFFER_BIT);
        }
//                         glClearColor(1.f, 0.0f, 1.f, 1.0f);

        glDrawArrays(GL_POINTS, 0, nParticules);
        //        return nParticules;

        glfwSwapBuffers(window);
        glfwPollEvents();

        glBufferSubData(GL_ARRAY_BUFFER, 0, nParticules * sizeof(Particule), particules.data());

    }
    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}

GLint DrawSTL(const string path, GLFWwindow* window)
{
    vector<Triangle> triangleArray = ReadStl((Path + path).c_str());

    const auto vertex = MakeShader(GL_VERTEX_SHADER, ShaderString + "shaderSTL.vert");
    const auto fragment = MakeShader(GL_FRAGMENT_SHADER, ShaderString + "shaderSTL.frag");

    const auto programSTL = AttachAndLink({vertex, fragment});

    glUseProgram(programSTL);

    // Buffers
    GLuint vbo, vao;
    glGenBuffers(1, &vbo);
    glGenVertexArrays(1, &vao);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, triangleArray.size() * sizeof(Triangle), triangleArray.data(), GL_STATIC_DRAW);

    // Bindings
    const auto index = glGetAttribLocation(programSTL, "position");
    //    const auto indexColor = glGetAttribLocation(programSTL, "color");

    glVertexAttribPointer(index, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), nullptr);
    glEnableVertexAttribArray(index);

    //    int vec3Size = sizeof(vec3) * 3;
    //    glVertexAttribPointer(indexColor, 3, GL_FLOAT, GL_FALSE, sizeof(Triangle), (void*)vec3Size);
    //    glEnableVertexAttribArray(indexColor);
    //
    //    default_random_engine generator;
    //    uniform_real_distribution<float> distribution01(0.5, 1);
    //    for (Triangle& t : triangleArray)
    //    {
    //        t.color = vec3(1, 0, 1);
    ////        t.color = vec3(distribution01(generator), distribution01(generator), distribution01(generator));
    //    }
    vec3 initSize = triangleArray[0].p0;
    bool reverse = false;
    while (!glfwWindowShouldClose(window))
    {
        for (Triangle& t : triangleArray)
        {
            t.p0 = reverse ? t.p0 / vec1(1.002) : t.p0 * vec1(1.002);
            t.p1 = reverse ? t.p1 / vec1(1.002) : t.p1 * vec1(1.002);
            t.p2 = reverse ? t.p2 / vec1(1.002) : t.p2 * vec1(1.002);

            float teta = .01;
            t.p0 = vec3(cos(teta) * t.p0[0] + sin(teta) * t.p0[2], t.p0[1], -sin(teta) * t.p0[0] + cos(teta) * t.p0[2]);
            t.p1 = vec3(cos(teta) * t.p1[0] + sin(teta) * t.p1[2], t.p1[1], -sin(teta) * t.p1[0] + cos(teta) * t.p1[2]);
            t.p2 = vec3(cos(teta) * t.p2[0] + sin(teta) * t.p2[2], t.p2[1], -sin(teta) * t.p2[0] + cos(teta) * t.p2[2]);

        }

        reverse = triangleArray[0].p0[0] > (initSize * vec1(3))[0] ? true : triangleArray[0].p0[0] < initSize[0] * vec1(0.5)[0] ? false : reverse;

        int width, height;
        glfwGetFramebufferSize(window, &width, &height);

        glViewport(0, 0, width, height);

        glClear(GL_COLOR_BUFFER_BIT);
        //                 glClearColor(1.f, 0.0f, 1.f, 1.0f);

        glDrawArrays(GL_TRIANGLES, 0, (GLint)(triangleArray.size() * 3));
        //        return nParticules;

        glfwSwapBuffers(window);
        glfwPollEvents();

        glBufferSubData(GL_ARRAY_BUFFER, 0, triangleArray.size() * sizeof(Triangle), triangleArray.data());

    }
    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);

    return (GLint)(triangleArray.size() * 3);
}
