#include "Main.h"

int main(int argc, char **argv) {
    if (argc > 1){
        std::cout << argv[1] << std::endl;
    }
	GLFWwindow* window = initApp();

	TestRoom tr;
	tr.loadResources(window);

	/* loop vars */
	double crntTime = 0.0;

	const double timeStart = glfwGetTime();

	double prevTime = timeStart;
	double timeDiff;
	unsigned int counter = 0;

	boost::accumulators::accumulator_set<double, boost::accumulators::stats<boost::accumulators::tag::sum>> accum;

	double lastFrame = timeStart;
	double delta = 1.0 / 300.0;
	double frameTime = 0.0f;

	/* Main Game Loop */
	while (!glfwWindowShouldClose(window)) {
		crntTime = glfwGetTime();

		/* FPS counter */
		timeDiff = crntTime - prevTime;
		counter++;
		if (timeDiff >= 1.0) {
			std::string FPS = std::to_string((1.0 / timeDiff) * counter);
			std::string ms = std::to_string((timeDiff / counter) * 1000);
			std::string newTitle = "Obelisk Engine - " + FPS + "FPS / " + ms + "ms";
			glfwSetWindowTitle(window, newTitle.c_str());
			prevTime = crntTime;
			counter = 0;
		}

		glfwPollEvents(); // get inputs

		// Note: floating point should be renamed to floating transform point because subratction isn't guarenteed
		// a real floating point is impossible.

		frameTime = crntTime - lastFrame;
		lastFrame = crntTime;

		accum(frameTime);

		while (boost::accumulators::sum(accum) >= delta) {
			accum(-delta);
            tr.tick(window);
		}

        tr.drawFrame(window, frameTime);
	}

	tr.cleanup();

	glfwTerminate();
	return 0;

}

void gameTick(double delta) {
	{ // update cam pos
		if (!Globals::get().camLock) {
			glm::vec3 proj = glm::rotate(Globals::get().camera->getOrientation(), glm::radians(90.0f), Globals::get().camera->getUp());
			proj.y = 0.0f;
			proj = glm::normalize(proj);

			float moveSpeed = 1.0f; // position of camera move speed
			float order = 10.0f;

			glm::vec3 velocity = glm::vec3(0.0f, 0.0f, 0.0f);

			if (Input::get().getValue(GLFW_KEY_W)) {
				velocity += Globals::get().camera->getOrientation();
			}
			if (Input::get().getValue(GLFW_KEY_A)) {
				velocity += proj;
			}
			if (Input::get().getValue(GLFW_KEY_S)) {
				velocity -= Globals::get().camera->getOrientation();
			}
			if (Input::get().getValue(GLFW_KEY_D)) {
				velocity -= proj;
			}

			if (glm::length(velocity) > 0) {
				velocity = glm::normalize(velocity);

				velocity.x *= delta * order * moveSpeed;
				velocity.y *= delta * order * moveSpeed;
				velocity.z *= delta * order * moveSpeed;
			}

			Globals::get().camera->setPosition(Globals::get().camera->getPosition() + velocity);
		}
	}
}


GLFWwindow* initApp() {

	glfwInit();

	GLFWwindow* window = glfwCreateWindow(Globals::get().screenWidth, Globals::get().screenHeight, "Obelisk Engine", NULL, NULL); // windowed
	//GLFWwindow* window = glfwCreateWindow(Globals::get().screenWidth, Globals::get().screenHeight, "Obelisk Engine", glfwGetPrimaryMonitor(), NULL); // fullscreen
	if (window == NULL) {
		printf("Failed to create GLFW window!\n");
		return nullptr;
	}

	glfwMakeContextCurrent(window);

	if (GLEW_OK != glewInit()) {
		printf("Failed to initialize GLEW!.\n");
		return nullptr;
	}

	//refresh rate
	//glfwSetWindowMonitor(window, glfwGetPrimaryMonitor(), 0,0, Globals::get().screenWidth, Globals::get().screenHeight, 120);

	setup_callbacks(window);

	// enable depth buffer
	glEnable(GL_DEPTH_TEST);

	// emable stencil test
	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

	glStencilFunc(GL_ALWAYS, 0, 0xFF);
	glStencilMask(0xFF);

	// enable 8x MSAA
	glfwWindowHint(GLFW_SAMPLES, 8);
	glEnable(GL_MULTISAMPLE);

	// default gamma correction
	//glEnable(GL_FRAMEBUFFER_SRGB);

	// enable smooth shading vs flat
	glShadeModel(GL_SMOOTH);

	// Enable Culling
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	glFrontFace(GL_CW);

	glClearColor(0.07f, 0.13f, 0.17f, 1.0f);

	//disable vsync if enabled
	glfwSwapInterval(0);

	// enable transparency function
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_BLEND);

	return window;
}

void cursor_pos_callback(GLFWwindow* window, double xpos, double ypos) {

	if (Globals::get().cursorLocked == true) {

		float sensitivity = 1.0f; // mouse sens

		double deltaAngle = 0.0005; // base angle we move in

		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

		double xMiddle = (double)Globals::get().screenWidth / 2.0;
		double yMiddle = (double)Globals::get().screenHeight / 2.0;

		double xOffset = xMiddle - xpos;
		double yOffset = yMiddle - ypos;

		double rotX = xOffset * sensitivity * deltaAngle;
		double rotY = yOffset * sensitivity * deltaAngle;

		Globals::get().camera->setOrientation(glm::rotate(Globals::get().camera->getOrientation(), (float)rotX, Globals::get().camera->getUp()));

		glm::vec3 perpendicular = glm::normalize(glm::cross(Globals::get().camera->getOrientation(), Globals::get().camera->getUp()));
		// Clamps rotY so it doesn't glitch when looking directly up or down
		if (!((rotY > 0 && Globals::get().camera->getOrientation().y > 0.99f) || (rotY < 0 && Globals::get().camera->getOrientation().y < -0.99f)))
			Globals::get().camera->setOrientation(glm::rotate(Globals::get().camera->getOrientation(), (float)rotY, perpendicular));

		glfwSetCursorPos(window, xMiddle, yMiddle);
	}
	else {
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}

}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (action == GLFW_RELEASE) {
		Input::get().setValue(key, false);
		return;
	}

	Input::get().setValue(key, true);
	switch (key) {
	case GLFW_KEY_ESCAPE:
		glfwSetWindowShouldClose(window, true);
		break;
	case GLFW_KEY_X:
		Globals::get().cursorLocked = !Globals::get().cursorLocked;
		break;
	case GLFW_KEY_Z:
		Globals::get().camLock = !Globals::get().camLock;
		break;
	case GLFW_KEY_O:
		Globals::get().drawWires = !Globals::get().drawWires;
		break;
	case GLFW_KEY_F10:
		glfwSetWindowMonitor(window, glfwGetPrimaryMonitor(), 0, 0, Globals::get().screenWidth, Globals::get().screenHeight, GLFW_DONT_CARE);
		break;
	case GLFW_KEY_F9:
		glfwSetWindowMonitor(window, NULL, 100, 100, Globals::get().screenWidth, Globals::get().screenHeight, GLFW_DONT_CARE);
		break;
	}
}

void window_size_callback(GLFWwindow* window, int width, int height) {
	// Define the portion of the window used for OpenGL rendering.
	glViewport(0, 0, width, height);
	Globals::get().screenWidth = width == 0 ? 1 : width;
	Globals::get().screenHeight = height == 0 ? 1 : height;

	Globals::get().camera->setDims(Globals::get().screenWidth, Globals::get().screenHeight);

	Globals::get().handCam->setDims(Globals::get().screenWidth, Globals::get().screenHeight);
}

void setup_callbacks(GLFWwindow* window) {
	glfwSetFramebufferSizeCallback(window, window_size_callback);

	glfwSetKeyCallback(window, key_callback);

	glfwSetCursorPosCallback(window, cursor_pos_callback);

	// glfwSetMouseButtonCallback(window, mouse_button_callback);
}
