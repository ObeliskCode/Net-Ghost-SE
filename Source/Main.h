#ifndef MAIN_H
#define MAIN_H

// #define BUILD_WINDOWS

#ifdef BUILD_WINDOWS
// #pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup") // remove console on windows??
#endif

#ifdef BUILD_WINDOWS
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")
#pragma comment(lib, "glfw3.lib")
#pragma comment(lib, "glew32s.lib")
#pragma comment(lib, "glew32.lib")
#endif

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Scene.h"

GLFWwindow *initApp();

#endif
