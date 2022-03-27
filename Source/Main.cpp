#pragma comment(lib,"opengl32.lib")
#pragma comment(lib,"glu32.lib")
#pragma comment(lib,"glfw3.lib")
#pragma comment(lib,"glew32s.lib")
#pragma comment(lib,"glew32.lib")
#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup") // remove console
#define GLEW_STATIC
#include <GL/glew.h> 
#include <GLFW/glfw3.h>


#include "Main.h"


/* Global Variables */

int screenWidth = 1024, screenHeight = 576;     // Width and height of window in pixels.

float sensitivity = 1.0f; // mouse sens
bool cursorLocked = false; // whether to lock cursor for first person mode
double deltaAngle = 0.0005; // base angle we move in
float moveSpeed = 1.0f; // position of camera move speed

glm::vec3 pos = glm::vec3(0.0f, 10.0f, 30.0f);

Camera camera(screenWidth, screenHeight, pos); // create cam at pos

const unsigned int skyWidth = 800; // cubeMap vars
const unsigned int skyHeight = 800;

/* Functions */

void gameTick() {



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
        return;			// Ignore key up (key release) events
    }

	// for A and D movement, Orientation projected onto XZ-plane
	glm::vec3 proj = glm::rotate(camera.Orientation, glm::radians(90.0f), camera.Up);
	proj.y = 0.0f;
	proj = glm::normalize(proj);

	switch (key) {
	case GLFW_KEY_ESCAPE:
		glfwSetWindowShouldClose(window, true);
		break;
	case GLFW_KEY_W:
		camera.Position = camera.Position + (camera.Orientation * moveSpeed);
		break;
	case GLFW_KEY_A:
		camera.Position = camera.Position + (proj * moveSpeed);
		break;
	case GLFW_KEY_S:
		camera.Position = camera.Position - (camera.Orientation * moveSpeed);
		break;
	case GLFW_KEY_D:
		camera.Position = camera.Position - (proj * moveSpeed);
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

int main() {

	/* GLFW & GLEW Set Up*/

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

	/* Set Up */
	Shader shaderProgram = Shader("vertexShader.glsl", "fragmentShader.glsl"); // OLD

	Shader rigProgram = Shader("rigVert.glsl", "rigFrag.glsl"); // NEW

	Model sword("lamp/lamp.dae");
	Model assim("bat/bat.dae");
	Model floor("floor/floor.dae");

	floor.setTranslation(glm::vec3(0.0f, 5.0f, 28.0f));
	floor.setScale(glm::vec3(10.0f, 10.0f, 10.0f));

	sword.setTranslation(glm::vec3(-10.0f, 15.0f, 28.0f));
	sword.setScale(glm::vec3(0.05f,0.05f,0.05f));

	assim.setTranslation(glm::vec3(0.0f, 10.0f, 22.0f));
	
	/* light set up */

	Shader lightShader = Shader("lightVert.glsl", "lightFrag.glsl"); // OLD

	Model light("bulb/scene.gltf");

	light.setScale(glm::vec3(5.0f, 5.0f, 5.0f));

	glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	glm::vec3 lightPos = glm::vec3(0.0f, 10.0f, 28.0f);
	glm::mat4 lightModel = glm::mat4(1.0f);
	lightModel = glm::translate(lightModel, lightPos);

	lightShader.Activate();
	glUniformMatrix4fv(glGetUniformLocation(lightShader.ID, "lightModel"), 1, GL_FALSE, glm::value_ptr(lightModel)); // model uniform already taken by Model Class
	glUniform4f(glGetUniformLocation(lightShader.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);

	rigProgram.Activate();
	glUniform4f(glGetUniformLocation(rigProgram.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
	glUniform3f(glGetUniformLocation(rigProgram.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);

	/* Cube Map Set up */

	Shader skyShader = Shader("skyVert.glsl", "skyFrag.glsl");

	SkyBox sky = SkyBox(skyShader, skyWidth, skyHeight);

	/* FPS counter set up */

	double prevTime = 0.0;
	double crntTime = 0.0;
	double timeDiff;
	unsigned int counter = 0;

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


		gameTick();

		renderScene();

		sword.Draw(rigProgram, camera);
		assim.Draw(rigProgram, camera);
		floor.Draw(rigProgram, camera);

		light.Draw(lightShader, camera);

		sky.Draw(skyShader, camera);

		glfwSwapBuffers(window);

		glfwPollEvents();	

	}

	/* Application Closed */

	glfwTerminate();
	return 0;
}