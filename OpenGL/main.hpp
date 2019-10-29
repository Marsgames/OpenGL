//
//  main.hpp
//  OpenGL
//
//  Created by Raphaël Daumas on 06/12/2018.
//  Copyright © 2018 Headcrab Entertainment. All rights reserved.
//

#pragma once

#include <string>

using std::string;
using std::vector;

const string ShaderString = "/Users/Raph/Documents/Gamagora/Synthese/OpenGL/OpenGL/Shaders/";
const string Path = "/Users/Raph/Documents/Gamagora/Synthese/OpenGL/OpenGL/";

/* PARTICULES */
struct Particule {
    vec3 position;
    vec3 color;
    vec3 speed;
};

static void error_callback(int /*error*/, const char* description);
static void key_callback(GLFWwindow* window, int key, int /*scancode*/, int action, int /*mods*/);
vector<Particule> MakeParticules(const int n);
GLuint MakeShader(GLuint t, string path);
GLuint AttachAndLink(vector<GLuint> shaders);
GLsizei DrawParticle(GLFWwindow* window, const bool clear);
GLint DrawSTL(string path, GLFWwindow* window);
