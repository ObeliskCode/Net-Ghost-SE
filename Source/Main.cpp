#pragma comment(lib,"opengl32.lib")
#pragma comment(lib,"glu32.lib")
#pragma comment(lib,"glfw3.lib")
#pragma comment(lib,"glew32s.lib")
#pragma comment(lib,"glew32.lib")
//#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup") // remove console
#define GLEW_STATIC
#include <GL/glew.h> 
#include <GLFW/glfw3.h>
#include "Main.h"


/* Global Variables */

int screenWidth = 1024, screenHeight = 576;     // Width and height of window in pixels.

float sensitivity = 1.0f; // mouse sens
bool cursorLocked = false; // whether to lock cursor for first person mode
double deltaAngle = 0.0005; // base angle we move in

float order = 10.0f;
float moveSpeed = 1.0f; // position of camera move speed

glm::vec3 camPos = glm::vec3(0.0f, 10.0f, 30.0f);

Camera camera(screenWidth, screenHeight, camPos); // create cam at pos

glm::vec3 handCamPos = glm::vec3(0.0f, 0.0f, 2.0f);

Camera handCam(screenWidth, screenHeight, handCamPos);

bool W, A, S, D = false;

/* Functions */

int main() {

	/* GLFW & GLEW Set Up */

	glfwInit();

	GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "Game Engine", NULL, NULL);
	if (window == NULL) {
		printf("Failed to create GLFW window!\n");
		return -1;
	}

	glfwMakeContextCurrent(window);

	if (GLEW_OK != glewInit()) {
		printf("Failed to initialize GLEW!.\n");
		return -1;
	}

	setup_callbacks(window);

	// enable depth buffer
	glEnable(GL_DEPTH_TEST);

	// enable 8x MSAA
	glfwWindowHint(GLFW_SAMPLES, 8);
	glEnable(GL_MULTISAMPLE);

	// enable smooth shading vs flat
	glShadeModel(GL_SMOOTH);

	// Enable Culling
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	glFrontFace(GL_CW);

	//disable vsync if enabled
	glfwSwapInterval(0);

	/* GLFW & GLEW Set Up */

	/* Model Set Up */

	Shader rigProgram = Shader("rigVert.glsl", "rigFrag.glsl"); // NEW

	Model lamp("lamp/lamp.dae");
	Model bat("bat/bat.dae");
	Model panel("floor/floor.dae");
	Model wolf("wolf/Wolf_One_dae.dae");
	Model handBat("bat/bat.dae");

	wolf.setScale(glm::vec3(10.0f,10.0f,10.0f));
	wolf.setTranslation(glm::vec3(10.0f, 8.5f, 10.0f));

	panel.setTranslation(glm::vec3(0.0f, 5.01f, 28.0f));
	panel.setScale(glm::vec3(10.0f, 10.0f, 10.0f));

	lamp.setTranslation(glm::vec3(-10.0f, 15.0f, 28.0f));
	lamp.setScale(glm::vec3(0.05f,0.05f,0.05f));
	lamp.setRotation(glm::quat(0.0f, 0.0f, 1.0f, 0.0f));

	bat.setTranslation(glm::vec3(0.0f, 10.0f, 22.0f));
	bat.setRotation(glm::quat(0.0f, 0.0f, 1.0f, 0.0f));

	handBat.setRotation(glm::quat(0.0f, 0.0f, 1.0f, 0.0f));
	handBat.setTranslation(glm::vec3(2.0f,-2.0f,0.0f));

	Model light("bulb/scene.gltf");

	light.setScale(glm::vec3(5.0f, 5.0f, 5.0f));

	glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	glm::vec3 lightPos = glm::vec3(0.0f, 10.0f, 32.0f);

	light.setTranslation(lightPos);

	rigProgram.Activate();
	glUniform4f(glGetUniformLocation(rigProgram.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
	glUniform3f(glGetUniformLocation(rigProgram.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);

	/* Model Set Up */

	Wire xaxis(glm::vec3(-5.0f,0.0f,0.0f), glm::vec3(5.0f,0.0f,0.0f));
	Wire yaxis(glm::vec3(0.0f,-5.0f,0.0f), glm::vec3(0.0f,5.0f,0.0f));
	Wire zaxis(glm::vec3(0.0f,0.0f,-5.0f), glm::vec3(0.0f,0.0f,5.0f));

	Shader wireProgram = Shader("wireVert.glsl", "wireFrag.glsl");

	/* FPS counter Set Up */
	double prevTime = 0.0;
	double crntTime = 0.0;
	double timeDiff;
	unsigned int counter = 0;
	/* FPS counter Set Up */

	double lastTick = 0.0;
	double thisTick = 0.0;
	double delta;

	/* Main Game Loop */
	while (!glfwWindowShouldClose(window)) {

		/* FPS counter */
		crntTime = glfwGetTime();
		timeDiff = crntTime - prevTime;
		counter++;
		if (timeDiff >= 1.0 / 30.0) {
			std::string FPS = std::to_string((1.0 / timeDiff) * counter);
			std::string ms = std::to_string((timeDiff / counter) * 1000);
			std::string newTitle = "Obelisk Engine - " + FPS + "FPS / " + ms + "ms";
			glfwSetWindowTitle(window, newTitle.c_str());
			prevTime = crntTime;
			counter = 0;
		}
		/* FPS counter */

		glfwPollEvents();

		thisTick = glfwGetTime();
		delta = thisTick - lastTick;
		if (delta >= 1.0 / 60.0) {
			gameTick(delta);
			lastTick = thisTick;
		}

		renderScene();

		lamp.Draw(rigProgram, camera);
		bat.Draw(rigProgram, camera);
		panel.Draw(rigProgram, camera);
		light.Draw(rigProgram, camera);
		wolf.Draw(rigProgram, camera);
		handBat.Draw(rigProgram, handCam);

		xaxis.Draw(wireProgram, camera);
		yaxis.Draw(wireProgram, camera);
		zaxis.Draw(wireProgram, camera);

		glfwSwapBuffers(window);

	}

	/* Application Closed */

	glfwTerminate();
	return 0;
}

void gameTick(double delta) {
	glm::vec3 proj = glm::rotate(camera.Orientation, glm::radians(90.0f), camera.Up);
	proj.y = 0.0f;
	proj = glm::normalize(proj);

	glm::vec3 velocity = glm::vec3(0.0f, 0.0f, 0.0f);

	if (W) {
		velocity += camera.Orientation;
	}
	if (A) {
		velocity += proj;
	}
	if (S) {
		velocity -= camera.Orientation;
	}
	if (D) {
		velocity -= proj;
	}

	if (glm::length(velocity) > 0) {
		velocity = glm::normalize(velocity);

		velocity.x *= delta * order * moveSpeed;
		velocity.y *= delta * order * moveSpeed;
		velocity.z *= delta * order * moveSpeed;
	}

	camera.Position += velocity;

}

void renderScene() {

	glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

}

void cursor_pos_callback(GLFWwindow* window, double xpos, double ypos) {

	if (cursorLocked == true) {

		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

		double xMiddle = (double)screenWidth / 2.0;
		double yMiddle = (double)screenHeight / 2.0;

		double xOffset = xMiddle - xpos;
		double yOffset = yMiddle - ypos;

		double rotX = xOffset * sensitivity * deltaAngle;
		double rotY = yOffset * sensitivity * deltaAngle;

		camera.Orientation = glm::rotate(camera.Orientation, (float)rotX, camera.Up);

		glm::vec3 perpendicular = glm::normalize(glm::cross(camera.Orientation, camera.Up));
		// Clamps rotY so it doesn't glitch when looking directly up or down
		if (!((rotY > 0 && camera.Orientation.y > 0.99f) || (rotY < 0 && camera.Orientation.y < -0.99f)))
			camera.Orientation = glm::rotate(camera.Orientation, (float)rotY, perpendicular);

		glfwSetCursorPos(window, xMiddle, yMiddle);
	}
	else {
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}

}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (action == GLFW_RELEASE) {
		switch (key) {
		case GLFW_KEY_ESCAPE:
			glfwSetWindowShouldClose(window, true);
			break;
		case GLFW_KEY_W:
			W = false;
			break;
		case GLFW_KEY_A:
			A = false;
			break;
		case GLFW_KEY_S:
			S = false;
			break;
		case GLFW_KEY_D:
			D = false;
			break;
		}
		return;
	}

	switch (key) {
	case GLFW_KEY_ESCAPE:
		glfwSetWindowShouldClose(window, true);
		break;
	case GLFW_KEY_W:
		W = true;
		break;
	case GLFW_KEY_A:
		A = true;
		break;
	case GLFW_KEY_S:
		S = true;
		break;
	case GLFW_KEY_D:
		D = true;
		break;
	case GLFW_KEY_X:
		cursorLocked = !cursorLocked;
		break;
	}
}

void window_size_callback(GLFWwindow* window, int width, int height) {
	// Define the portion of the window used for OpenGL rendering.
	glViewport(0, 0, width, height);
	screenWidth = width == 0 ? 1 : width;
	screenHeight = height == 0 ? 1 : height;

	camera.width = screenWidth;
	camera.height = screenHeight;
}

void setup_callbacks(GLFWwindow* window) {
	glfwSetFramebufferSizeCallback(window, window_size_callback);

	glfwSetKeyCallback(window, key_callback);

	glfwSetCursorPosCallback(window, cursor_pos_callback);

	// glfwSetMouseButtonCallback(window, mouse_button_callback);
}