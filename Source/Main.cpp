#include "Main.h"

const double PI = 3.1415926535897932384626433832795028841971693993751058209;

// todo move to particle class?
struct {
	bool operator()(Particle a, Particle b) const { return glm::length(Globals::get().camera->Position - a.getTranslation()) > glm::length(Globals::get().camera->Position - b.getTranslation()); }
} Less;

int main() {
	GLFWwindow* window = initApp();

	Shader rigProgram = Shader("rigVert.glsl", "mdlFrag.glsl");
	Shader wireProgram = Shader("wireVert.glsl", "wireFrag.glsl");
	Shader lightProgram = Shader("rigVert.glsl", "lightFrag.glsl");
	Shader animProgram = Shader("animVert.glsl", "mdlFrag.glsl");
	Shader noTexAnimProgram = Shader("animVert.glsl", "noTexFrag.glsl");
	Shader shadowProgram = Shader("shadowVert.glsl", "shadowFrag.glsl");
	Globals::get().shadowShader = &shadowProgram;
	Shader animShadowProgram = Shader("animShadowVert.glsl", "shadowFrag.glsl");
	Globals::get().animShadowShader = &animShadowProgram;
	Shader cellProgram = Shader("rigVert.glsl", "cellFrag.glsl");
	Globals::get().cellShader = &cellProgram;
	Shader animCellProgram = Shader("animVert.glsl", "cellFrag.glsl");
	Globals::get().animCellShader = &animCellProgram;

	Entity* e;

	Model* panel = new Model("floor/floor.dae");
	e = ECS::get().linkEntity(new Entity(panel, &rigProgram, &wireProgram, Globals::get().camera));
	e->setTranslation(glm::vec3(0.0f, 5.0f, 0.0f));
	e->setScale(glm::vec3(20.0f));
	e->m_surface = true;

	Model* lamp = new Model("lamp/lamp.dae");
	lamp->setUnitConversion(0.05f);
	lamp->setOffset(glm::vec3(0.0f, 5.0f, 0.0f));
	e = ECS::get().linkEntity(new Entity(lamp, &lightProgram, &wireProgram, Globals::get().camera));
	e->addBody(Physics::get().addUnitBoxStaticBody(e->getID(), 2.0f, 5.0f, 2.0f, -20.0f, 10.0f - 0.1f, 28.0f));
	e->addWireFrame(2.0f, 5.0f, 2.0f);

	Model* bench = new Model("bench/bench.dae");
	bench->setUnitConversion(4.5f);
	bench->setOffset(glm::vec3(0.0f, -2.5f, 0.0f)); // since updatePhysics puts object bottom at middle of physics object, translate by halfHeight
	float CosHalfPi = sqrt(2.f) / 2.f;
	bench->setOrientation(glm::quat(CosHalfPi, 0.f, -CosHalfPi, 0.f));
	e = ECS::get().linkEntity(new Entity(bench, &rigProgram, &wireProgram, Globals::get().camera));
	e->addBody(Physics::get().addUnitBoxStaticBody(e->getID(), 2.0f, 2.5f, 5.0f, 5.0f, 5.0f + 2.5f, 28.f)); // whole lot of maths
	e->addWireFrame(2.0f, 2.5f, 5.0f);

	Model* handBat = new Model("bat/bat.dae");
	Entity* batEnt = new Entity(handBat, &rigProgram, &wireProgram, Globals::get().handCam);
	batEnt->setRotation(glm::quat(0.0f, 0.0f, 1.0f, 0.0f));
	batEnt->setTranslation(glm::vec3(2.0f, -2.0f, 0.0f));

	Model* cig = new Model("cig/cig.dae");
	Entity* cigEnt = new Entity(cig, &rigProgram, &wireProgram, Globals::get().handCam);
	cigEnt->setTranslation(glm::vec3(0.0f, -2.0f, -1.0f));
	cigEnt->setRotation(glm::quat(0.0f, 0.0f, 1.0f, 0.0f));

	Model* dumpster = new Model("dumpster/dumpster.dae");
	dumpster->setUnitConversion(6.5f);
	dumpster->setOffset(glm::vec3(0.0f, -4.0f, 0.0f));
	dumpster->setOrientation(glm::quat(CosHalfPi, 0.f, -CosHalfPi, 0.f));
	e = ECS::get().linkEntity(new Entity(dumpster, &rigProgram, &wireProgram, Globals::get().camera));
	e->addBody(Physics::get().addUnitBoxStaticBody(e->getID(), 3.0f, 4.0f, 6.0f, 25.0f, 5.0f + 4.0f, 35.f)); // whole lot of maths
	e->addWireFrame(3.0f, 4.0f, 6.0f);

	Model* cart = new Model("cart/ShoppingCart.dae");
	e = ECS::get().linkEntity(new Entity(cart, &rigProgram, &wireProgram, Globals::get().camera));
	e->setTranslation(glm::vec3(20.0f, 5.0f, 20.0f));
	e->setScale(glm::vec3(7.f));

	Model* tent = new Model("tent/tent.dae");
	tent->setUnitConversion(7.f);
	tent->setOffset(glm::vec3(0.0f, -4.0f, 0.0f));
	e = ECS::get().linkEntity(new Entity(tent, &rigProgram, &wireProgram, Globals::get().camera));
	e->addBody(Physics::get().addUnitBoxStaticBody(e->getID(), 6.0f, 4.0f, 6.0f, -15.0f, 5.0f + 4.0f, 10.0f)); // whole lot of maths
	e->addWireFrame(6.0f, 4.0f, 6.0f);

	SkeletalModel* wolf = new SkeletalModel("wolf/Wolf_One_dae.dae");
	Skeleton* wolfAnimation = new Skeleton("wolf/Wolf_One_dae.dae", wolf);
	Animator* animator = new Animator(wolfAnimation);
	e = ECS::get().linkEntity(new Entity(wolf, animator, &noTexAnimProgram, &wireProgram, Globals::get().camera));
	e->setScale(glm::vec3(10.0f, 10.0f, 10.0f));
	e->setTranslation(glm::vec3(10.0f, 5.0f, 10.0f));

	SkeletalModel* sit = new SkeletalModel("sit/Sitting Clap.dae");
	Skeleton* sitAnimation = new Skeleton("sit/Sitting Clap.dae", sit);
	Animator* sitMator = new Animator(sitAnimation);
	e = ECS::get().linkEntity(new Entity(sit, sitMator, &animProgram, &wireProgram, Globals::get().camera));
	e->setScale(glm::vec3(5.0f, 5.0f, 5.0f));
	e->setTranslation(glm::vec3(5.0f, 5.0f, 28.f));
	e->setRotation(glm::quat(CosHalfPi, 0.0f, -CosHalfPi, 0.0f));

	SkeletalModel* walk = new SkeletalModel("jjong/Idle.dae");
	Skeleton* walkAnimation = new Skeleton("jjong/Idle.dae", walk);
	walkAnimation->addAnimation("jjong/Walking.dae", walk);
	Animator* mator = new Animator(walkAnimation);
	mator->QueueAnimation(1);
	e = ECS::get().linkEntity(new Entity(walk, mator, &animProgram, &wireProgram, Globals::get().camera));
	e->setScale(glm::vec3(5.0f, 5.0f, 5.0f));
	e->setTranslation(glm::vec3(15.0f, 5.0f, -5.0f));

	glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	glm::vec3 lightPos = glm::vec3(-20.0f, 13.2f, 28.0f);

	Model* light = new Model("bulb/scene.gltf");
	e = ECS::get().linkEntity(new Entity(light, &lightProgram, &wireProgram, Globals::get().camera));
	e->setScale(glm::vec3(5.0f, 5.0f, 5.0f));
	e->setTranslation(lightPos);

	rigProgram.Activate();
	glUniform4f(glGetUniformLocation(rigProgram.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
	glUniform3f(glGetUniformLocation(rigProgram.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);

	animProgram.Activate();
	glUniform4f(glGetUniformLocation(animProgram.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
	glUniform3f(glGetUniformLocation(animProgram.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);

	noTexAnimProgram.Activate();
	glUniform4f(glGetUniformLocation(noTexAnimProgram.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
	glUniform3f(glGetUniformLocation(noTexAnimProgram.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);

	lightProgram.Activate();
	glUniform4f(glGetUniformLocation(lightProgram.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
	glUniform3f(glGetUniformLocation(lightProgram.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);

	//QUAD
	e = ECS::get().linkEntity(new Entity(&wireProgram, Globals::get().camera));
	e->addWire(new Wire(glm::vec3(60.0f, 50.0f, -60.0f), glm::vec3(-60.0f, 50.0f, -60.0f)));
	e->addWire(new Wire(glm::vec3(-60.0f, 50.0f, -60.0f), glm::vec3(-60.0f, 50.0f, 60.0f)));
	e->addWire(new Wire(glm::vec3(-60.0f, 50.0f, 60.0f), glm::vec3(60.0f, 50.0f, 60.0f)));
	e->addWire(new Wire(glm::vec3(60.0f, 50.0f, 60.0f), glm::vec3(60.0f, 50.0f, -60.0f)));
	e->addBody(Physics::get().addShape1(e->getID()));

	//STAGING AXIS
	e = ECS::get().linkEntity(new Entity(&wireProgram, Globals::get().camera));
	e->addWire(new Wire(glm::vec3(-5.0f, 0.0f, 0.0f), glm::vec3(5.0f, 0.0f, 0.0f)));
	e->addWire(new Wire(glm::vec3(0.0f, -5.0f, 0.0f), glm::vec3(0.0f, 5.0f, 0.0f)));
	e->addWire(new Wire(glm::vec3(0.0f, 0.0f, -5.0f), glm::vec3(0.0f, 0.0f, 5.0f)));

	//RIGID BODY 1
	e = ECS::get().linkEntity(new Entity(&wireProgram, Globals::get().camera));
	e->addWire(new Wire(glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f)));
	e->addWire(new Wire(glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)));
	e->addWire(new Wire(glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
	e->addBody(Physics::get().addShape2(e->getID()));
	e->setType("pickup");

	//RIGID BODY 2
	e = ECS::get().linkEntity(new Entity(&wireProgram, Globals::get().camera));
	e->addWire(new Wire(glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f)));
	e->addWire(new Wire(glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)));
	e->addWire(new Wire(glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
	e->addBody(Physics::get().addShape3(e->getID()));
	e->setType("pickup");

	//CONTROLLABLE BODY
	Entity* character = ECS::get().linkEntity(new Entity(&wireProgram, Globals::get().camera));
	character->addWire(new Wire(glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f)));
	character->addWire(new Wire(glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)));
	character->addWire(new Wire(glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
	character->addBody(Physics::get().addShape4(character->getID()));

	// CAPSULE
	e = ECS::get().linkEntity(new Entity(&wireProgram, Globals::get().camera));
	e->addWire(new Wire(glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f)));
	e->addWire(new Wire(glm::vec3(0.0f, -2.0f, 0.0f), glm::vec3(0.0f, 2.0f, 0.0f)));
	e->addWire(new Wire(glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
	e->addBody(Physics::get().addShape5(e->getID()));
	e->setType("pickup");

	//TEST CIGS
	for (int i = 0; i < 5; i++) {
		e = ECS::get().linkEntity(new Entity(new Model("cig/cig.dae"), &rigProgram, &wireProgram, Globals::get().camera));
		e->addWire(new Wire(glm::vec3(-0.06f, 0.0f, 0.0f), glm::vec3(0.06f, 0.0f, 0.0f)));
		e->addWire(new Wire(glm::vec3(0.0f, -0.06f, 0.0f), glm::vec3(0.0f, 0.06f, 0.0f)));
		e->addWire(new Wire(glm::vec3(0.0f, 0.0f, -0.685f), glm::vec3(0.0f, 0.0f, 0.685f)));
		e->addBody(Physics::get().addShape6(e->getID()));
		e->setType("pickup");
	}

	//CROSSHAIR HACK (class required)
	e = ECS::get().linkEntity(new Entity(&wireProgram, Globals::get().handCam));
	e->addWire(new Wire(glm::vec3(-0.04f, 0.0f, 0.0f), glm::vec3(0.04f, 0.0f, 0.0f)));
	e->addWire(new Wire(glm::vec3(0.0f, -0.04f, 0.0f), glm::vec3(0.0f, 0.04f, 0.0f)));

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
	Skybox* sky = new Skybox(faces);

	Shader skyProgram = Shader("skyVert.glsl", "skyFrag.glsl");

	unsigned int depthMapFBO;
	glGenFramebuffers(1, &depthMapFBO);

	const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;

	unsigned int depthMap;
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float clampColor[] = { 1.0f,1.0f,1.0f,1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, clampColor);

	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	Globals::get().depthMap = depthMap;

	float near_plane = 0.1f, far_plane = 200.0f;
	glm::mat4 lightProjection = glm::ortho(-60.0f, 60.0f, -60.0f, 60.0f, near_plane, far_plane);

	glm::mat4 lightView = glm::lookAt(glm::vec3(-4.0f, 10.0f, -6.0f),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f));

	glm::mat4 lightSpaceMatrix = lightProjection * lightView;
	Globals::get().lightSpaceMatrix = lightSpaceMatrix;

	shadowProgram.Activate();
	glUniformMatrix4fv(glGetUniformLocation(shadowProgram.ID, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));

	// loop vars
	double crntTime = 0.0;

	double prevTime = 0.0;
	double timeDiff;
	unsigned int counter = 0;

	double lastTick = 0.0;
	double delta;

	double deltaTime = 0.0f;
	double lastFrame = 0.0f;

	double batRot = 0.0f;

	float bf = 0.0f;

	unsigned int prevID = 0;

	bool cig_anim = false;
	bool is_smoking = false;
	float cig_anim_time = 0.0f;

	int cd = 0;

	/* Main Game Loop */
	while (!glfwWindowShouldClose(window)) {

		/* FPS counter */
		crntTime = glfwGetTime();
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
		/* FPS counter */

		glfwPollEvents(); // get inputs
		
		/* TICK BASED EVENTS */ // 1. calc physics update -> 2. game logic
		delta = crntTime - lastTick;
		if (delta >= 1.0 / 192.0) {
			{ // character, delta
				if (Globals::get().camLock) {
					btRigidBody* body = character->getBody();

					glm::vec2 camOri = glm::vec2(Globals::get().camera->Orientation.x, Globals::get().camera->Orientation.z);

					glm::vec2 proj = glm::rotate(camOri, glm::radians(-90.0f));

					glm::vec2 vel = glm::vec2(0.0f);


					if (Input::get().getValue(GLFW_KEY_W)) {
						vel += camOri;
					}
					if (Input::get().getValue(GLFW_KEY_A)) {
						vel += proj;
					}
					if (Input::get().getValue(GLFW_KEY_S)) {
						vel -= camOri;
					}
					if (Input::get().getValue(GLFW_KEY_D)) {
						vel -= proj;
					}


					if (cd > 0) cd--;
					// code character->canJump()
					if (Input::get().getValue(GLFW_KEY_SPACE)) {
						if (cd == 0) {
							cd = 192;
							body->applyCentralImpulse(btVector3(0.0f,7.0f,0.0f));
						}
					}
					

					float accel = delta * 60.0f;
					if (glm::length(vel) > 0) {
						vel = glm::normalize(vel);
						vel *= accel;
					}

					btVector3 linVel = body->getLinearVelocity();
					btVector3 horizVel = btVector3(linVel.getX() + vel.x, 0.0f, linVel.getZ() + vel.y);
					if (horizVel.length() > 12.0f) {
						horizVel = horizVel * (12.0f / horizVel.length());
					}
					linVel = btVector3(horizVel.getX(), linVel.getY(), horizVel.getZ());
					body->setLinearVelocity(linVel);
				}
			} // character.physicsProcess(delta)

			Physics::get().updateSim(delta); // move sim forward by delta
			ECS::get().updatePhysics(); // update entities with physics state
			{
				if (Globals::get().camLock) {
					btTransform trans;
					btRigidBody* body = character->getBody();
					body->getMotionState()->getWorldTransform(trans);
					glm::vec3 pos = glm::vec3(float(trans.getOrigin().getX()), float(trans.getOrigin().getY()) + 7.0f, float(trans.getOrigin().getZ()));
					Globals::get().camera->Position = pos;
				}
			}
			{
				if (Input::get().getValue(GLFW_KEY_RIGHT)) bf += 0.05f;
				if (Input::get().getValue(GLFW_KEY_LEFT)) bf -= 0.05f;
				if (bf < 0.0f) bf = 0.0f;
				if (bf > 1.0f) bf = 1.0f;
				mator->SetBlendFactor(bf);
			}
			gameTick(delta); // post-physics game logic.
			{ // do rayCast

				glm::vec3 ppp = Globals::get().camera->Position + (Globals::get().camera->Orientation * 12.5f);

				btVector3 from = btVector3(Globals::get().camera->Position.x, Globals::get().camera->Position.y, Globals::get().camera->Position.z);
				btVector3 to = btVector3(ppp.x, ppp.y, ppp.z);
				btCollisionWorld::ClosestRayResultCallback rrc = btCollisionWorld::ClosestRayResultCallback(from, to);
				Physics::get().getDynamicsWorld()->rayTest(from, to, rrc);

				if (rrc.hasHit()) {
					btRigidBody* sel = btRigidBody::upcast(const_cast <btCollisionObject*>(rrc.m_collisionObject));
					unsigned int entID = Physics::get().m_EntityMap[sel];
					if (entID != 0) {
						if (prevID != 0 && prevID != entID) {
							Entity* prevEnt = ECS::get().getEntity(prevID);
							if (prevEnt) {
								prevEnt->m_stenciled = false;
							}
							prevID = 0;
						}
						if (Input::get().getValue(GLFW_KEY_E)) {
							if (ECS::get().getEntity(entID)->getType() == "pickup") {
								// only deletes wires, rigid body & not model / skel model
								ECS::get().deleteEntity(entID);
								prevID = 0;

							}
						}
						else if (!ECS::get().getEntity(entID)->m_surface) {
							ECS::get().getEntity(entID)->m_stenciled = true;
							prevID = entID;
						}
					}
				} else if (prevID != 0) {
					Entity* prevEnt = ECS::get().getEntity(prevID);
					if (prevEnt) {
						prevEnt->m_stenciled = false;
					}
					prevID = 0;
				}

			}

			{ // particles
			// UPDATE so that it shows cig first then blows smoke after it dissapears
				if (Input::get().getValue(GLFW_KEY_P) && !is_smoking) {
					is_smoking = true;
					cig_anim = true;
					cig_anim_time = 0.0f;
				}				
				
				if (is_smoking) {
					cig_anim_time += delta;
					if (cig_anim_time >= 2.0f) cig_anim = false;
					if (cig_anim_time >= 3.0f) is_smoking = false;
				}
				
				if (cig_anim) {
					cigEnt->m_visible = true;
				} else cigEnt->m_visible = false;


				if  (cig_anim == false && is_smoking == true) {
					Particle p = Particle();
					p.setTranslation(Globals::get().camera->Position - glm::vec3(0.f, 0.25f, 0.f));
					p.setScale(0.1f);
					p.vel = glm::normalize(Globals::get().camera->Orientation);
					Globals::get().particles.push_back(p);

					Particle p2 = Particle();
					p2.setTranslation(Globals::get().camera->Position - glm::vec3(0.f, 0.25f, 0.f));
					p2.setScale(0.1f);
					p2.vel = glm::normalize(Globals::get().camera->Orientation);
					Globals::get().particles.push_back(p2);
				}

				// how slow is this? (update/cleanup particles)
				for (int i = 0; i < Globals::get().particles.size(); i++) {
					Globals::get().particles[i].update(delta);
					if (Globals::get().particles[i].life >= Globals::get().particles[i].expire) {
						Globals::get().particles.erase(Globals::get().particles.begin() + i);
						i--;
					}
				}

			}
			
			lastTick = crntTime;
		}
		/* TICK BASED EVENTS */
		
		/* ANIMATION UPDATES / RENDERING */
		// calc time since last frame for animation
		deltaTime = crntTime - lastFrame;
		lastFrame = crntTime;

		{ // pls do not make this a function
			batRot += deltaTime;
			if (batRot > 2 * PI) {
				batRot = batRot - (2 * PI);
			}
			float q1, q3;
			q1 = cos(batRot / 2);
			q3 = sin(batRot / 2);
			batEnt->setRotation(glm::quat(q1, 0.0f, q3, 0.0f));
		}

		renderScene();


		{ // shadow draw pass!
			glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
			glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
			glClear(GL_DEPTH_BUFFER_BIT);// clears this framebuffers depth bit!

			ECS::get().DrawEntityShadows(deltaTime);

			glBindFramebuffer(GL_FRAMEBUFFER, 0);

			glViewport(0, 0, Globals::get().screenWidth, Globals::get().screenHeight);
		}

		ECS::get().DrawEntities();

		sky->Draw(skyProgram, *Globals::get().camera);

		ECS::get().DrawEntityStencils();

		batEnt->Draw();
		cigEnt->Draw();

		// emitter->drawparticles()
		glEnable(GL_BLEND);
		std::sort(Globals::get().particles.begin(), Globals::get().particles.end(),Less);
		for (int i = 0; i < Globals::get().particles.size(); i++) {
			Globals::get().particles[i].Draw(partProgram, *Globals::get().camera);
		}
		glDisable(GL_BLEND);

		glfwSwapBuffers(window);
		/* ANIMATION UPDATES / RENDERING */

	}

	delete sky;

	Globals::get().particles.clear();

	// careful with these! not well written!
	ECS::destruct();
	Physics::destruct();

	/* Application Close */

	glfwTerminate();
	return 0;
	
}

void gameTick(double delta) {
	{ // update cam pos
		if (!Globals::get().camLock) {
			glm::vec3 proj = glm::rotate(Globals::get().camera->Orientation, glm::radians(90.0f), Globals::get().camera->Up);
			proj.y = 0.0f;
			proj = glm::normalize(proj);

			float moveSpeed = 1.0f; // position of camera move speed
			float order = 10.0f;

			glm::vec3 velocity = glm::vec3(0.0f, 0.0f, 0.0f);

			if (Input::get().getValue(GLFW_KEY_W)) {
				velocity += Globals::get().camera->Orientation;
			}
			if (Input::get().getValue(GLFW_KEY_A)) {
				velocity += proj;
			}
			if (Input::get().getValue(GLFW_KEY_S)) {
				velocity -= Globals::get().camera->Orientation;
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

			Globals::get().camera->Position += velocity;
		}
	}


}

void renderScene() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
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

		Globals::get().camera->Orientation = glm::rotate(Globals::get().camera->Orientation, (float)rotX, Globals::get().camera->Up);

		glm::vec3 perpendicular = glm::normalize(glm::cross(Globals::get().camera->Orientation, Globals::get().camera->Up));
		// Clamps rotY so it doesn't glitch when looking directly up or down
		if (!((rotY > 0 && Globals::get().camera->Orientation.y > 0.99f) || (rotY < 0 && Globals::get().camera->Orientation.y < -0.99f)))
			Globals::get().camera->Orientation = glm::rotate(Globals::get().camera->Orientation, (float)rotY, perpendicular);

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

	Globals::get().camera->width = Globals::get().screenWidth;
	Globals::get().camera->height = Globals::get().screenHeight;

	Globals::get().handCam->width = Globals::get().screenWidth;
	Globals::get().handCam->height = Globals::get().screenHeight;
}

void setup_callbacks(GLFWwindow* window) {
	glfwSetFramebufferSizeCallback(window, window_size_callback);

	glfwSetKeyCallback(window, key_callback);

	glfwSetCursorPosCallback(window, cursor_pos_callback);

	// glfwSetMouseButtonCallback(window, mouse_button_callback);
}