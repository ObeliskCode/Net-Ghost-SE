//#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup") // remove console

#include "Main.h"

const double PI = 3.1415926535897932384626433832795028841971693993751058209;

/* Global Variables */

int screenWidth = 1600, screenHeight = 900; // Width and height of window in pixels.

float sensitivity = 1.0f; // mouse sens
bool cursorLocked = false; // whether to lock cursor for first person mode
double deltaAngle = 0.0005; // base angle we move in

float order = 10.0f;
float moveSpeed = 1.0f; // position of camera move speed

// ground at 5.0f, camera at 11.0f so 11.0f - 5.0f = 6.0f = height of player.
Camera camera(screenWidth, screenHeight, glm::vec3(0.0f, 11.0f, 30.0f)); // create cam at pos

Camera handCam(screenWidth, screenHeight, glm::vec3(0.0f, 0.0f, 2.0f));

struct {
	bool operator()(Particle a, Particle b) const { return glm::length(camera.Position-a.getTranslation()) > glm::length(camera.Position - b.getTranslation()); }
} Less;

/* Functions */

int main() {
	GLFWwindow* window = initApp();

	Shader rigProgram = Shader("rigVert.glsl", "rigFrag.glsl");
	Shader wireProgram = Shader("wireVert.glsl", "wireFrag.glsl");
	Shader animProgram = Shader("animVert.glsl", "animFrag.glsl");

	Model* panel = new Model("floor/floor.dae");
	panel->setTranslation(glm::vec3(0.0f, 5.0f, 20.0f));
	panel->setScale(glm::vec3(10.0f, 10.0f, 10.0f));

	ECS::get().addEntity(new Entity(panel, &rigProgram, &wireProgram, &camera));

	Model* lamp = new Model("lamp/lamp.dae");
	lamp->setTranslation(glm::vec3(-10.0f, 15.0f-0.1f, 28.0f));
	lamp->setScale(glm::vec3(0.05f,0.05f,0.05f));
	lamp->setRotation(glm::quat(0.0f, 0.0f, 1.0f, 0.0f));

	ECS::get().addEntity(new Entity(lamp, &rigProgram, &wireProgram, &camera));

	Model* bench = new Model("bench/bench.dae");
	bench->setScale(glm::vec3(4.5f,4.5f,4.5f));
	bench->setTranslation(glm::vec3(5.0f, 5.0f, 28.f));
	float CosHalfPi = sqrt(2.f) / 2.f;
	bench->setRotation(glm::quat(CosHalfPi,0.f,-CosHalfPi,0.f));

	ECS::get().addEntity(new Entity(bench, &rigProgram, &wireProgram, &camera));

	Model* handBat = new Model("bat/bat.dae");
	handBat->setRotation(glm::quat(0.0f, 0.0f, 1.0f, 0.0f));
	handBat->setTranslation(glm::vec3(2.0f,-2.0f,0.0f));

	ECS::get().addEntity(new Entity(handBat, &rigProgram, &wireProgram, &handCam));

	Model* cig = new Model("cig/cig.dae");
	cig->setTranslation(glm::vec3(0.0f, -2.0f, -1.0f));
	cig->setRotation(glm::quat(0.0f,0.0f,1.0f,0.0f));

	ECS::get().addEntity(new Entity(cig, &rigProgram, &wireProgram, &handCam));

	Model* dumpster = new Model("dumpster/dumpster.dae");
	dumpster->setTranslation(glm::vec3(25.0f, 5.0f, 35.0f));
	dumpster->setRotation(glm::quat(CosHalfPi, 0.f, -CosHalfPi, 0.f));
	dumpster->setScale(glm::vec3(6.5f));

	ECS::get().addEntity(new Entity(dumpster, &rigProgram, &wireProgram, &camera));

	Model* cart = new Model("cart/ShoppingCart.dae");
	cart->setTranslation(glm::vec3(20.0f, 5.0f, 20.0f));
	cart->setScale(glm::vec3(7.f));

	ECS::get().addEntity(new Entity(cart, &rigProgram, &wireProgram, &camera));

	Model* tent = new Model("tent/tent.dae");
	tent->setTranslation(glm::vec3(-15.0f, 5.0f, 10.0f));
	tent->setScale(glm::vec3(7.f));

	ECS::get().addEntity(new Entity(tent, &rigProgram, &wireProgram, &camera));

	SkeletalModel* wolf = new SkeletalModel("wolf/Wolf_One_dae.dae");
	wolf->setScale(glm::vec3(10.0f, 10.0f, 10.0f));
	wolf->setTranslation(glm::vec3(10.0f, 5.0f, 10.0f));
	Animation* wolfAnimation = new Animation("wolf/Wolf_One_dae.dae", wolf);
	Animator* animator = new Animator(wolfAnimation);

	ECS::get().addEntity(new Entity(wolf, wolfAnimation, animator, &animProgram, &wireProgram, &camera));

	glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	glm::vec3 lightPos = glm::vec3(-10.0f, 13.2f, 28.0f);

	Model* light = new Model("bulb/scene.gltf");
	light->setScale(glm::vec3(5.0f, 5.0f, 5.0f));
	light->setTranslation(lightPos);

	ECS::get().addEntity(new Entity(light, &rigProgram, &wireProgram, &camera));

	rigProgram.Activate();
	glUniform4f(glGetUniformLocation(rigProgram.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
	glUniform3f(glGetUniformLocation(rigProgram.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);

	animProgram.Activate();
	glUniform4f(glGetUniformLocation(rigProgram.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
	glUniform3f(glGetUniformLocation(rigProgram.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);

	//QUAD
	Entity* e = new Entity(&wireProgram, &camera);
	e->addWire(new Wire(glm::vec3(50.0f, 50.0f, -50.0f), glm::vec3(-50.0f, 50.0f, -50.0f)));
	e->addWire(new Wire(glm::vec3(-50.0f, 50.0f, -50.0f), glm::vec3(-50.0f, 50.0f, 50.0f)));
	e->addWire(new Wire(glm::vec3(-50.0f, 50.0f, 50.0f), glm::vec3(50.0f, 50.0f, 50.0f)));
	e->addWire(new Wire(glm::vec3(50.0f, 50.0f, 50.0f), glm::vec3(50.0f, 50.0f, -50.0f)));
	ECS::get().addEntity(e);
	e->addBody(Physics::get().addShape1(e->getID()));
	
	//STAGING AXIS
	e = new Entity(&wireProgram, &camera);
	e->addWire(new Wire(glm::vec3(-5.0f, 0.0f, 0.0f), glm::vec3(5.0f, 0.0f, 0.0f)));
	e->addWire(new Wire(glm::vec3(0.0f, -5.0f, 0.0f), glm::vec3(0.0f, 5.0f, 0.0f)));
	e->addWire(new Wire(glm::vec3(0.0f, 0.0f, -5.0f), glm::vec3(0.0f, 0.0f, 5.0f)));
	ECS::get().addEntity(e);

	//RIGID BODY 1
	e = new Entity(&wireProgram, &camera);
	e->addWire(new Wire(glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f)));
	e->addWire(new Wire(glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)));
	e->addWire(new Wire(glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
	ECS::get().addEntity(e);
	e->addBody(Physics::get().addShape2(e->getID()));
	e->setType("pickup");

	//RIGID BODY 2
	e = new Entity(&wireProgram, &camera);
	e->addWire(new Wire(glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f)));
	e->addWire(new Wire(glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)));
	e->addWire(new Wire(glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
	ECS::get().addEntity(e);
	e->addBody(Physics::get().addShape3(e->getID()));
	e->setType("pickup");
	

	//CONTROLLABLE BODY
	Entity* character = new Entity(&wireProgram, &camera);
	character->addWire(new Wire(glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f)));
	character->addWire(new Wire(glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)));
	character->addWire(new Wire(glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
	ECS::get().addEntity(character);
	character->addBody(Physics::get().addShape4(character->getID()));

	// CAPSULE
	e = new Entity(&wireProgram, &camera);
	e->addWire(new Wire(glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f)));
	e->addWire(new Wire(glm::vec3(0.0f, -2.0f, 0.0f), glm::vec3(0.0f, 2.0f, 0.0f)));
	e->addWire(new Wire(glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
	ECS::get().addEntity(e);
	e->addBody(Physics::get().addShape5(e->getID()));
	e->setType("pickup");

	//TEST CIGS
	for (int i = 0; i < 5; i++) {
		e = new Entity(new Model("cig/cig.dae"), &rigProgram, &wireProgram, &camera);
		ECS::get().addEntity(e);
		e->addWire(new Wire(glm::vec3(-0.06f, 0.0f, 0.0f), glm::vec3(0.06f, 0.0f, 0.0f)));
		e->addWire(new Wire(glm::vec3(0.0f, -0.06f, 0.0f), glm::vec3(0.0f, 0.06f, 0.0f)));
		e->addWire(new Wire(glm::vec3(0.0f, 0.0f, -0.685f), glm::vec3(0.0f, 0.0f, 0.685f)));
		e->addBody(Physics::get().addShape6(e->getID()));
		e->setType("pickup");
	}

	//CROSSHAIR HACK (class required)
	e = new Entity(&wireProgram, &handCam);
	e->addWire(new Wire(glm::vec3(-0.04f, 0.0f, 0.0f), glm::vec3(0.04f, 0.0f, 0.0f)));
	e->addWire(new Wire(glm::vec3(0.0f, -0.04f, 0.0f), glm::vec3(0.0f, 0.04f, 0.0f)));
	ECS::get().addEntity(e);
	
	std::vector<Particle> particles;

	Shader partProgram = Shader("partVert.glsl", "partFrag.glsl");

	std::vector<std::string> faces =
	{
			"skybox/right.jpg",
			"skybox/left.jpg",
			"skybox/top.jpg",
			"skybox/bottom.jpg",
			"skybox/front.jpg",
			"skybox/back.jpg"
	};
	Skybox sky = Skybox::Skybox(faces);

	Shader skyProgram = Shader("skyVert.glsl", "skyFrag.glsl");

	// loop vars
	double prevTime = 0.0;
	double crntTime = 0.0;
	double timeDiff;
	unsigned int counter = 0;

	double lastTick = 0.0;
	double thisTick = 0.0;
	double delta;

	float deltaTime = 0.0f;
	float lastFrame = 0.0f;

	float batRot = 0.0f;

	bool cig_anim = false;

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

		glfwPollEvents(); // get inputs

		/* TICK BASED EVENTS */
		thisTick = glfwGetTime();
		delta = thisTick - lastTick;
		if (delta >= 1.0 / 64.0) {
			{ // set character body velocity b4 sim update
				btRigidBody* body = character->getBody();

				float zVel = 0;
				float xVel = 0;
				float accel = delta * 20.0f;
				if (Input::get().getValue(GLFW_KEY_I)) {
					zVel += -accel;
				}
				if (Input::get().getValue(GLFW_KEY_J)) {
					xVel += -accel;
				}
				if (Input::get().getValue(GLFW_KEY_K)) {
					zVel += accel;
				}
				if (Input::get().getValue(GLFW_KEY_L)) {
					xVel += accel;
				}
				btVector3 linVel = body->getLinearVelocity();
				linVel = btVector3(linVel.getX() + xVel, linVel.getY(), linVel.getZ() + zVel);
				if (linVel.length() > 10.0f) {
					linVel = linVel * (10.0f / linVel.length());
				}
				body->setLinearVelocity(linVel);

			}
			gameTick(delta); // move cam pos
			Physics::get().updateSim(delta); // move sim forward
			ECS::get().updatePhysics(); // update entities with physics state
			{ // is this part of character if so b4 or after gameTick
				if (Input::get().getValue(GLFW_KEY_E)) {
					glm::vec3 ppp = camera.Position + (camera.Orientation * 7.0f);

					btVector3 from = btVector3(camera.Position.x, camera.Position.y, camera.Position.z);
					btVector3 to = btVector3(ppp.x, ppp.y, ppp.z);
					btCollisionWorld::ClosestRayResultCallback rrc = btCollisionWorld::ClosestRayResultCallback(from, to);
					Physics::get().getDynamicsWorld()->rayTest(from, to, rrc);

					if (rrc.hasHit()) {
						btRigidBody* sel = btRigidBody::upcast(const_cast <btCollisionObject*>(rrc.m_collisionObject));
						unsigned int entID = Physics::get().m_EntityMap[sel];
						if (entID != 0) {
							if (ECS::get().getEntity(entID)->getType() == "pickup") {
								// only deletes wires, rigid body & not model / skel model
								ECS::get().deleteEntity(entID);

							}
						}
					}
				}
			}
			{ // needs to be after camera update (gameTick)
				// UPDATE so that it shows cig first then blows smoke after it dissapears
				if (Input::get().getValue(GLFW_KEY_P)) {
					Particle p = Particle();
					p.setTranslation(camera.Position - glm::vec3(0.f, 0.25f, 0.f));
					p.setScale(0.1f);
					p.vel = glm::normalize(camera.Orientation);
					particles.push_back(p);
					
					Particle p2 = Particle();
					p2.setTranslation(camera.Position - glm::vec3(0.f, 0.25f, 0.f));
					p2.setScale(0.1f);
					p2.vel = glm::normalize(camera.Orientation);
					particles.push_back(p2);
				}

				// how slow is this? (update/cleanup particles)
				for (int i = 0; i < particles.size(); i++) {
					particles[i].update(delta);
					if (particles[i].life >= particles[i].expire) {
						particles.erase(particles.begin() + i);
						i--;
					}
				}
				
			}
			lastTick = thisTick;
		}
		/* TICK BASED EVENTS */

		/* ANIMATION UPDATES */
		// calc time since last frame for animation
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		{ // pls do not make this a function
			batRot += deltaTime;
			if (batRot > 2 * PI) {
				batRot = batRot - (2 * PI);
			}
			float q1, q3;
			q1 = cos(batRot / 2);
			q3 = sin(batRot / 2);
			handBat->setRotation(glm::quat(q1, 0.0f, q3, 0.0f));
		}
		/* ANIMATION UPDATES */

		/* RENDERING */
		renderScene();

		ECS::get().DrawEntities(deltaTime);

		sky.Draw(skyProgram, camera);

		// emitter->drawparticles()
		glEnable(GL_BLEND);
		std::sort(particles.begin(),particles.end(),Less);
		for (int i = 0; i < particles.size(); i++) {
			particles[i].Draw(partProgram, camera);
		}
		glDisable(GL_BLEND);

		glfwSwapBuffers(window);
		/* RENDERING */

	}

	// careful with these! not well written!
	ECS::destruct();
	Physics::destruct();

	/* Application Close */

	glfwTerminate();
	return 0;
	
}

GLFWwindow* initApp() {

	glfwInit();

	GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "Obelisk Engine", NULL, NULL);
	if (window == NULL) {
		printf("Failed to create GLFW window!\n");
		return nullptr;
	}

	glfwMakeContextCurrent(window);

	if (GLEW_OK != glewInit()) {
		printf("Failed to initialize GLEW!.\n");
		return nullptr;
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

	// enable transparency function
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_BLEND);

	return window;
}

void gameTick(double delta) {
	glm::vec3 proj = glm::rotate(camera.Orientation, glm::radians(90.0f), camera.Up);
	proj.y = 0.0f;
	proj = glm::normalize(proj);

	glm::vec3 velocity = glm::vec3(0.0f, 0.0f, 0.0f);

	if (Input::get().getValue(GLFW_KEY_W)) {
		velocity += camera.Orientation;
	}
	if (Input::get().getValue(GLFW_KEY_A)) {
		velocity += proj;
	}
	if (Input::get().getValue(GLFW_KEY_S)) {
		velocity -= camera.Orientation;
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
		Input::get().setValue(key, false);
		return;
	}

	Input::get().setValue(key, true);
	switch (key) {
		case GLFW_KEY_ESCAPE:
			glfwSetWindowShouldClose(window, true);
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

	handCam.width = screenWidth;
	handCam.height = screenHeight;
}

void setup_callbacks(GLFWwindow* window) {
	glfwSetFramebufferSizeCallback(window, window_size_callback);

	glfwSetKeyCallback(window, key_callback);

	glfwSetCursorPosCallback(window, cursor_pos_callback);

	// glfwSetMouseButtonCallback(window, mouse_button_callback);
}