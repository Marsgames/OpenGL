#pragma once

//#include "Library/GLM/vec3.hpp"
#include "../GLM/vec3.hpp"
#include "../GLM/glm.hpp"
#include <vector>

using namespace glm;

struct Triangle
{
	glm::vec3 p0, p1, p2;
//    vec3 color;
    
public:
    vec3 GetNormale() const
    {
        vec3 edge1 = this->p1 - this->p0;
        vec3 edge2 = this->p2 - this->p0;
        
        return glm::normalize(glm::cross(edge1, edge2));
    };
};

std::vector<Triangle> ReadStl(const char * filename);
void Print(glm::vec3 vec);
