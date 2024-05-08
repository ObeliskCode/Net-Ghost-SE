#ifndef POINTSHADOW_H
#define POINTSHADOW_H

#ifdef BUILD_WINDOWS
#define GLEW_STATIC
#endif
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

        void linkShadowShaders();

        unsigned int getFBO(){ return pointShadowMapFBO; }
        unsigned int getMap(){ return depthCubemap; }
    private:
        unsigned int pointShadowMapFBO;
        unsigned int depthCubemap;

    	std::vector<glm::mat4> shadowTransforms;
        glm::vec3 lightPos;
        float far_plane;

};

#endif
