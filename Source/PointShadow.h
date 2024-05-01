#ifndef PARTICLESYSTEM_H
#define PARTICLESYSTEM_H

#define GLEW_STATIC
#include <GL/glew.h> 
#include <GLFW/glfw3.h>

#include <vector>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "Globals.h"

class PointShadow {
    public:
        PointShadow(glm::vec3 lightPos);
        ~PointShadow();
    private:
        unsigned int pointShadowMapFBO;
        unsigned int depthCubemap;
};

#endif