#ifndef MAIN_H
#define MAIN_H

//#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup") // remove console

#pragma comment(lib,"opengl32.lib")
#pragma comment(lib,"glu32.lib")
#pragma comment(lib,"glfw3.lib")
#pragma comment(lib,"glew32s.lib")
#pragma comment(lib,"glew32.lib")
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "glm/glm.hpp"
#include <glm/gtx/string_cast.hpp>
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <FileSystem>
#include <math.h>
#include <cmath>

#include "stb_image.h"

#include "bullet/btBulletDynamicsCommon.h"

#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/stats.hpp>
#include <boost/accumulators/statistics/sum.hpp>

#include "Globals.h"
#include "Shader.h"
#include "ECS.h"
#include "Entity.h"
#include "VAO.h"
#include "VBO.h"
#include "EBO.h"
#include "Camera.h"
#include "Mesh.h"
#include "Model.h"
#include "Wire.h"
#include "Particle.h"
#include "Physics.h"
#include "Skybox.h"
#include "SkeletalModel.h"
#include "Skeleton.h"
#include "Light.h"
#include "Animator.h"
#include "Input.h"
#include "ParticleEmitter.h"
#include "LightSystem.h"
#include "ParticleSystem.h"

GLFWwindow* initApp();
void gameTick(double delta);
void renderScene();

/*Event Callback Functions*/
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void window_size_callback(GLFWwindow* window, int width, int height);
void cursor_pos_callback(GLFWwindow* window, double xpos, double ypos);
void setup_callbacks(GLFWwindow* window);

#endif