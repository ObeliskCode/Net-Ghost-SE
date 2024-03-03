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

const double PI = 3.1415926535897932384626433832795028841971693993751058209;

/* Global Variables */

int screenWidth = 1024, screenHeight = 576;     // Width and height of window in pixels.

float sensitivity = 1.0f; // mouse sens
bool cursorLocked = false; // whether to lock cursor for first person mode
double deltaAngle = 0.0005; // base angle we move in

float order = 10.0f;
float moveSpeed = 1.0f; // position of camera move speed

Camera camera(screenWidth, screenHeight, glm::vec3(0.0f, 10.0f, 30.0f)); // create cam at pos

Camera handCam(screenWidth, screenHeight, glm::vec3(0.0f, 0.0f, 2.0f));

bool W, A, S, D = false;

bool P = false;

struct {
	bool operator()(Particle a, Particle b) const { return glm::length(camera.Position-a.getTranslation()) > glm::length(camera.Position - b.getTranslation()); }
} Less;

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

	// enable blending for transparency
	//glEnable(GL_BLEND);

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

	// transparency
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_BLEND);

	/* GLFW & GLEW Set Up */

	/* Load Models/Shaders */

	Shader rigProgram = Shader("rigVert.glsl", "rigFrag.glsl"); // rigid bodies

	Model lamp("lamp/lamp.dae");
	Model panel("floor/floor.dae");
	Model handBat("bat/bat.dae");
	Model bench("bench/bench.dae");

	panel.setTranslation(glm::vec3(0.0f, 5.01f, 28.0f));
	panel.setScale(glm::vec3(10.0f, 10.0f, 10.0f));

	lamp.setTranslation(glm::vec3(-10.0f, 15.0f, 28.0f));
	lamp.setScale(glm::vec3(0.05f,0.05f,0.05f));
	lamp.setRotation(glm::quat(0.0f, 0.0f, 1.0f, 0.0f));

	bench.setScale(glm::vec3(5.f,5.f,5.f));
	bench.setTranslation(glm::vec3(5.0f, 5.0f, 28.f));
	float emg = sqrt(2.f) / 2.f;
	bench.setRotation(glm::quat(emg,0.f,-emg,0.f));

	handBat.setRotation(glm::quat(0.0f, 0.0f, 1.0f, 0.0f));
	handBat.setTranslation(glm::vec3(2.0f,-2.0f,0.0f));

	Shader animProgram = Shader("animVert.glsl", "animFrag.glsl"); // animated models

	SkeletalModel wolf("wolf/Wolf_One_dae.dae");
	Animation wolfAnimation("wolf/Wolf_One_dae.dae", &wolf);
	Animator animator(&wolfAnimation);

	wolf.setScale(glm::vec3(10.0f, 10.0f, 10.0f));
	wolf.setTranslation(glm::vec3(10.0f, 8.5f, 10.0f));

	Model light("bulb/scene.gltf");

	light.setScale(glm::vec3(5.0f, 5.0f, 5.0f));

	glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	glm::vec3 lightPos = glm::vec3(-10.0f, 13.2f, 28.0f);

	light.setTranslation(lightPos);

	rigProgram.Activate();
	glUniform4f(glGetUniformLocation(rigProgram.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
	glUniform3f(glGetUniformLocation(rigProgram.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);

	animProgram.Activate();
	glUniform4f(glGetUniformLocation(rigProgram.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
	glUniform3f(glGetUniformLocation(rigProgram.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);

	Wire xaxis(glm::vec3(-5.0f,0.0f,0.0f), glm::vec3(5.0f,0.0f,0.0f));
	Wire yaxis(glm::vec3(0.0f,-5.0f,0.0f), glm::vec3(0.0f,5.0f,0.0f));
	Wire zaxis(glm::vec3(0.0f,0.0f,-5.0f), glm::vec3(0.0f,0.0f,5.0f));

	Wire xsphere(glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	Wire ysphere(glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	Wire zsphere(glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 0.0f, 1.0f));

	Wire x2sphere(glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	Wire y2sphere(glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	Wire z2sphere(glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 0.0f, 1.0f));

	Shader wireProgram = Shader("wireVert.glsl", "wireFrag.glsl");

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

	/* Load Models/Shaders */

	// bullet physics
	int i;

	///collision configuration contains default setup for memory, collision setup. Advanced users can create their own configuration.
	btDefaultCollisionConfiguration* collisionConfiguration = new btDefaultCollisionConfiguration();

	///use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
	btCollisionDispatcher* dispatcher = new btCollisionDispatcher(collisionConfiguration);

	///btDbvtBroadphase is a good general purpose broadphase. You can also try out btAxis3Sweep.
	btBroadphaseInterface* overlappingPairCache = new btDbvtBroadphase();

	///the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
	btSequentialImpulseConstraintSolver* solver = new btSequentialImpulseConstraintSolver;

	btDiscreteDynamicsWorld* dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration);

	dynamicsWorld->setGravity(btVector3(0, -10, 0));

	//keep track of the shapes, we release memory at exit.
	//make sure to re-use collision shapes among rigid bodies whenever possible!
	btAlignedObjectArray<btCollisionShape*> collisionShapes;

	///create a few basic rigid bodies

	//the ground is a cube of side 100 at position y = -56.
	//the sphere will hit it at y = -6, with center at -5
	{
		btCollisionShape* groundShape = new btBoxShape(btVector3(btScalar(50.), btScalar(50.), btScalar(50.)));

		collisionShapes.push_back(groundShape);

		btTransform groundTransform;
		groundTransform.setIdentity();
		groundTransform.setOrigin(btVector3(0, -42, 0));

		btScalar mass(0.);

		//rigidbody is dynamic if and only if mass is non zero, otherwise static
		bool isDynamic = (mass != 0.f);

		btVector3 localInertia(0, 0, 0);
		if (isDynamic)
			groundShape->calculateLocalInertia(mass, localInertia);

		//using motionstate is optional, it provides interpolation capabilities, and only synchronizes 'active' objects
		btDefaultMotionState* myMotionState = new btDefaultMotionState(groundTransform);
		btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, groundShape, localInertia);
		btRigidBody* body = new btRigidBody(rbInfo);

		//add the body to the dynamics world
		dynamicsWorld->addRigidBody(body);
	}

	{
		//create a dynamic rigidbody

		//btCollisionShape* colShape = new btBoxShape(btVector3(1,1,1));
		btCollisionShape* colShape = new btSphereShape(btScalar(1.));
		collisionShapes.push_back(colShape);

		/// Create Dynamic Objects
		btTransform startTransform;
		startTransform.setIdentity();

		btScalar mass(1.f);

		//rigidbody is dynamic if and only if mass is non zero, otherwise static
		bool isDynamic = (mass != 0.f);

		btVector3 localInertia(0, 0, 0);
		if (isDynamic)
			colShape->calculateLocalInertia(mass, localInertia);

		startTransform.setOrigin(btVector3(2, 20, 0));

		//using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
		btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
		btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, colShape, localInertia);
		btRigidBody* body = new btRigidBody(rbInfo);

		dynamicsWorld->addRigidBody(body);
	}

	{
		//create a dynamic rigidbody

		//btCollisionShape* colShape = new btBoxShape(btVector3(1,1,1));
		btCollisionShape* colShape = new btSphereShape(btScalar(1.));
		collisionShapes.push_back(colShape);

		/// Create Dynamic Objects
		btTransform startTransform;
		startTransform.setIdentity();

		btScalar mass(1.f);

		//rigidbody is dynamic if and only if mass is non zero, otherwise static
		bool isDynamic = (mass != 0.f);

		btVector3 localInertia(0, 0, 0);
		if (isDynamic)
			colShape->calculateLocalInertia(mass, localInertia);

		startTransform.setOrigin(btVector3(0.0, 23, 0));

		//using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
		btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
		btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, colShape, localInertia);
		btRigidBody* body = new btRigidBody(rbInfo);

		dynamicsWorld->addRigidBody(body);
	}

	// timing
	double prevTime = 0.0;
	double crntTime = 0.0;
	double timeDiff;
	unsigned int counter = 0;

	double lastTick = 0.0;
	double thisTick = 0.0;
	double delta;

	float deltaTime = 0.0f;
	float lastFrame = 0.0f;

	float rot = 0.0f;

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

		// ~60 ticks per second
		thisTick = glfwGetTime();
		delta = thisTick - lastTick;
		if (delta >= 1.0 / 60.0) {
			gameTick(delta);
			{
				dynamicsWorld->stepSimulation(delta, 10);

				//print positions of all objects
				for (int j = dynamicsWorld->getNumCollisionObjects() - 1; j >= 0; j--)
				{
					btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[j];
					btRigidBody* body = btRigidBody::upcast(obj);
					btTransform trans;
					if (body && body->getMotionState())
					{
						body->getMotionState()->getWorldTransform(trans);
					}
					else
					{
						trans = obj->getWorldTransform();
					}
					if (j == 1) {
						xsphere.setTranslation(glm::vec3(float(trans.getOrigin().getX()), float(trans.getOrigin().getY()), float(trans.getOrigin().getZ())));
						ysphere.setTranslation(glm::vec3(float(trans.getOrigin().getX()), float(trans.getOrigin().getY()), float(trans.getOrigin().getZ())));
						zsphere.setTranslation(glm::vec3(float(trans.getOrigin().getX()), float(trans.getOrigin().getY()), float(trans.getOrigin().getZ())));
					}
					else if (j == 2) {
						x2sphere.setTranslation(glm::vec3(float(trans.getOrigin().getX()), float(trans.getOrigin().getY()), float(trans.getOrigin().getZ())));
						y2sphere.setTranslation(glm::vec3(float(trans.getOrigin().getX()), float(trans.getOrigin().getY()), float(trans.getOrigin().getZ())));
						z2sphere.setTranslation(glm::vec3(float(trans.getOrigin().getX()), float(trans.getOrigin().getY()), float(trans.getOrigin().getZ())));
					}
				}
			}
			{
				rot += delta;
				if (rot > 2*PI) {
					rot = rot - 2*PI;
				}
				float q1, q3;
				q1 = cos(rot / 2);
				q3 = sin(rot / 2);
				handBat.setRotation(glm::quat(q1,0.0f,q3,0.0f));
			}
			{
				if (P) {
					Particle p = Particle();
					p.setTranslation(camera.Position - glm::vec3(0.f, 0.25f, 0.f));
					p.setScale(glm::vec3(0.1f, 0.1f, 0.1f));
					p.vel = glm::normalize(camera.Orientation);
					particles.push_back(p);
				}

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

		// calc time since last frame for animation
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		animProgram.Activate();

		animator.UpdateAnimation(deltaTime);

		// why no transforms size so that leftover transforms don't get checked
		auto transforms = animator.GetFinalBoneMatrices();
		for (int i = 0; i < transforms.size(); ++i)
			glUniformMatrix4fv(glGetUniformLocation(animProgram.ID, ("finalBonesMatrices[" + std::to_string(i) + "]").c_str()), 1, GL_FALSE, &transforms[i][0][0]);

		renderScene();

		lamp.Draw(rigProgram, camera);
		panel.Draw(rigProgram, camera);
		light.Draw(rigProgram, camera);
		bench.Draw(rigProgram, camera);
		
		handBat.Draw(rigProgram, handCam);

		wolf.Draw(animProgram, camera);
		
		xaxis.Draw(wireProgram, camera);
		yaxis.Draw(wireProgram, camera);
		zaxis.Draw(wireProgram, camera);

		xsphere.Draw(wireProgram, camera);
		ysphere.Draw(wireProgram, camera);
		zsphere.Draw(wireProgram, camera);

		x2sphere.Draw(wireProgram, camera);
		y2sphere.Draw(wireProgram, camera);
		z2sphere.Draw(wireProgram, camera);

		sky.Draw(skyProgram, camera);

		glEnable(GL_BLEND);
		std::sort(particles.begin(),particles.end(),Less);
		for (int i = 0; i < particles.size(); i++) {
			particles[i].Draw(partProgram, camera);
		}
		glDisable(GL_BLEND);

		glfwSwapBuffers(window);

	}

	// bullet physics cleanup

	//remove the rigidbodies from the dynamics world and delete them
	for (i = dynamicsWorld->getNumCollisionObjects() - 1; i >= 0; i--)
	{
		btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[i];
		btRigidBody* body = btRigidBody::upcast(obj);
		if (body && body->getMotionState())
		{
			delete body->getMotionState();
		}
		dynamicsWorld->removeCollisionObject(obj);
		delete obj;
	}

	//delete collision shapes
	for (int j = 0; j < collisionShapes.size(); j++)
	{
		btCollisionShape* shape = collisionShapes[j];
		collisionShapes[j] = 0;
		delete shape;
	}

	//delete dynamics world
	delete dynamicsWorld;

	//delete solver
	delete solver;

	//delete broadphase
	delete overlappingPairCache;

	//delete dispatcher
	delete dispatcher;

	delete collisionConfiguration;

	//next line is optional: it will be cleared by the destructor when the array goes out of scope
	collisionShapes.clear();

	/* Application Close */

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
		case GLFW_KEY_P:
			P = false;
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
	case GLFW_KEY_P:
		P = true;
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