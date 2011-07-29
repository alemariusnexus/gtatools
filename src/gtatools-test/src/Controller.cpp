/*
	Copyright 2010-2011 David "Alemarius Nexus" Lerch

	This file is part of gtatools-test.

	gtatools-test is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	gtatools-test is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with gtatools-test.  If not, see <http://www.gnu.org/licenses/>.

	Additional permissions are granted, which are listed in the file
	GPLADDITIONS.
 */

#include "Controller.h"
#include <gtatools-test/config.h>
#include <gta/gl.h>
#include <gta/Engine.h>
#include <gta/Camera.h>
#include <gta/GLException.h>
#include <gta/Shader.h>
#include <gta/ShaderProgram.h>
#include <gta/resource/ResourceCache.h>
#include <gta/scene/Scene.h>
#include <gtaformats/util/util.h>
#include <gtaformats/util/strutil.h>
#include <cstdio>
#include <btBulletDynamicsCommon.h>
#include <gtaformats/gtadff.h>
#include <gta/resource/texture/ManagedTextureSource.h>
#include <gta/resource/mesh/StaticMeshPointer.h>
#include <gta/ItemManager.h>
#include <gta/DefaultRenderer.h>
#include <gta/resource/texture/TextureIndexer.h>
#include <gta/resource/texture/TextureArchive.h>









#define STATISTICS_UPDATE_INTERVAL 500





Controller::Controller()
		: lastFrameStart(0), lastMeasuredFrameStart(0), moveFactor(1.0f), moveForwardFactor(0.0f),
		  moveSidewardFactor(0.0f), moveUpFactor(0.0f), firstFrame(true), framesSinceLastMeasure(0),
		  lastMouseX(-1), lastMouseY(-1), printCacheStatistics(false), programRunning(true),
		  forceStatisticsUpdate(false)
{
}


void Controller::init()
{
	Engine* engine = Engine::getInstance();
	engine->setVersionMode(Engine::GTAVC);

	printf("Initializing Bullet...\n");

	btDefaultCollisionConfiguration* config = new btDefaultCollisionConfiguration();
	btCollisionDispatcher* dispatcher = new btCollisionDispatcher(config);
	btBroadphaseInterface* broadphase = new btDbvtBroadphase();
	btSequentialImpulseConstraintSolver* solver = new btSequentialImpulseConstraintSolver();
	btDiscreteDynamicsWorld* world = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, config);

	world->setGravity(btVector3(0.0f, 0.0f, -9.81f));
	//world->setGravity(btVector3(0.0f, 0.0f, -20.0f));

	engine->setPhysicsWorld(world);

#ifndef GT_USE_OPENGL_ES
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	glEnable(GL_LINE_SMOOTH);
#endif
	glLineWidth(2.0f);


	printf("Initializing scene...\n");

	gtaglInit();

	const char* exts = gtaglGetSupportedExtensions();
	char* extCpy = new char[strlen(exts)+1];
	strcpy(extCpy, exts);

	printf("***** SUPPORTED EXTENSIONS *****\n");

	char* ext = strtok(extCpy, " ");
	while (ext) {
		printf("%s\n", ext);
		ext = strtok(NULL, " ");
	}

	printf("***** END SUPPORTED EXTENSIONS *****\n");

	Scene* scene = new Scene;

	Camera* cam = new Camera;
	engine->setCamera(cam);

	engine->setScene(scene);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);

	glDepthFunc(GL_LEQUAL);

	GLException::checkError();

	// TODO Seems to be unnecessary
	//glEnable(GL_TEXTURE_2D);

	GLException::checkError();

	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ZERO, GL_ONE);

	GLException::checkError();

	engine->getMeshCache()->resize(40 * 1000000); // 40MB
	engine->getTextureCache()->resize(150 * 1000000); // 150MB
	engine->getCollisionMeshCache()->resize(30 * 1000000);

	printf("Loading resources...\n");
	addResource(File(GTASA_PATH "/models/gta3.img"));
	//addResource(File(GTASA_PATH "/models/gta_int.img"));
	//engine->addResource(File(GTASA_PATH ""));
	//engine->addResource(File(GTASA_PATH "/models/generic/vehicle.txd"));

	GLException::checkError();

	//debugDrawer = new BulletGLDebugDraw;
	//world->setDebugDrawer(debugDrawer);
	//debugDrawer->setDebugMode(BulletGLDebugDraw::DBG_DrawWireframe);


	printf("Loading DAT files...\n");
	engine->loadDAT(File(GTASA_PATH "/data/default.dat"), File(GTASA_PATH));
	//engine->loadDAT(File(GTASA_PATH "/data/gta.dat"), File(GTASA_PATH));
	engine->loadDAT(File(GTASA_PATH "/data/gta_vc.dat"), File(GTASA_PATH));
	//engine->loadDAT(File(GTASA_PATH "/data/gta3.dat"), File(GTASA_PATH));
	//engine->loadDAT(File(GTASA_PATH "/data/test.dat"), File(GTASA_PATH));

	for (int i = 0 ; i < 20 ; i++) {
		btScalar mass = 100.0f;
		btCollisionShape* shape;

		if (RandomBool()) {
			shape = new btSphereShape(RandomFloat(1.0f, 15.0f));
		} else {
			shape = new btBoxShape(btVector3(RandomFloat(1.0f, 10.0f), RandomFloat(1.0f, 10.0f), RandomFloat(1.0f, 10.0f)));
		}

		btVector3 inertia;
		shape->calculateLocalInertia(mass, inertia);
		btTransform trf(btQuaternion(0.0f, 0.0f, 0.0f, 1.0f),
				btVector3(RandomFloat(-50.0f, 50.0f), RandomFloat(-50.0f, 50.0f), RandomFloat(100.0f, 200.0f)));
		btDefaultMotionState* motionState = new btDefaultMotionState(trf);
		btRigidBody::btRigidBodyConstructionInfo info(mass, motionState, shape, inertia);
		btRigidBody* rigidBody = new btRigidBody(info);
		rigidBody->setRestitution(RandomFloat(0.0f, 1.0f));
		//rigidBody->applyCentralForce(btVector3(RandomFloat(0.1f, 10.0f), RandomFloat(0.1f, 10.0f), RandomFloat(0.1f, 10.0f)));
		world->addRigidBody(rigidBody);
		//rigidBody->setLinearVelocity(btVector3(RandomFloat(-20.0, 20.0f), RandomFloat(-20.0f, 20.0f), RandomFloat(-20.0f, 20.0f)));
	}

	cam->setPosition(-1013.983215f, -869.982971f, 14.407437f);
	//cam->setPosition(-97.538010f, -442.834442f, 0.799672f);
	//cam->setPosition(0.0f, 0.0f, 0.0f);
}


void Controller::reshape(int w, int h)
{
	Engine* engine = Engine::getInstance();

	engine->setViewportSize(w, h);

	float aspect = (float) w / (float) h;
	glViewport(0, 0, w, h);

	if (!engine->getRenderer()) {
		DefaultRenderer* renderer = new DefaultRenderer;

		renderer->setTransparencyMode(DefaultRenderer::DepthPeeling);

		engine->setRenderer(renderer);
	}

	float l = aspect*-0.7;
	float r = aspect*0.7;
	float b = -0.7;
	float t = 0.7;
	float n = 1.0;
	float f = 3000.0;

	// glFrustum(l, r, b, t, n, f):
	engine->setProjectionMatrix(Matrix4 (
		2*n/(r-l),		0,				0,					0,
		0,				2*n/(t-b),		0, 					0,
		(r+l)/(r-l),	(t+b)/(t-b),	(-(f+n))/(f-n),		-1,
		0,				0,				(-2*f*n)/(f-n),		0
	));
}


bool Controller::paint()
{
	uint64_t time = GetTickcount();

	Engine* engine = Engine::getInstance();

	Camera* cam = engine->getCamera();

	float timeFactor = 0.0f;

	if (lastFrameStart != 0) {
		timeFactor = (time - lastFrameStart)*0.025f;
	}

	cam->move(timeFactor*moveForwardFactor*moveFactor);
	cam->moveSideways(timeFactor*moveSidewardFactor*moveFactor);
	cam->moveUp(timeFactor*moveUpFactor*moveFactor);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	engine->render();

	lastFrameStart = time;

	time = GetTickcount();

	if (firstFrame) {
		lastMeasuredFrameStart = time;
		firstFrame = false;
	} else {
		framesSinceLastMeasure++;
	}

	uint64_t passed = time - lastMeasuredFrameStart;

	ResourceCache* mcache = engine->getMeshCache();
	ResourceCache* tcache = engine->getTextureCache();

	if (forceStatisticsUpdate  ||  passed > STATISTICS_UPDATE_INTERVAL) {
		float fps = framesSinceLastMeasure / (passed/1000.0f);

		int8_t gameH, gameM;
		engine->getGameTime(gameH, gameM);

		float mc = (float) mcache->getOccupiedSize() / mcache->getCapacity();
		float tc = (float) tcache->getOccupiedSize() / tcache->getCapacity();

		DefaultRenderer* renderer = (DefaultRenderer*) engine->getRenderer();

		char transMode[64];

		switch (renderer->getTransparencyMode()) {
		case DefaultRenderer::WeightedAverage:
			strcpy(transMode, "Weighted Average");
			break;
		case DefaultRenderer::DepthPeeling:
			sprintf(transMode, "Depth Peeling (%d passes/frame)", renderer->getTransparencyPassCount());
			break;
		}

		char title[128];
		sprintf(title, WINDOW_BASE_TITLE " - %.2f FPS - MC: %.2f%% - TC: %.2f%% - %.2d:%.2d - Transparency Mode: %s",
				fps, mc*100.0f, tc*100.0f, gameH, gameM, transMode);
		SDL_WM_SetCaption(title, NULL);

		lastMeasuredFrameStart = time;
		framesSinceLastMeasure = 0;

		forceStatisticsUpdate = false;
	}

	return programRunning;
}


void Controller::addResource(const File& file)
{
	if (file.isArchiveFile()  ||  file.isDirectory()) {
		if (file.guessContentType() != CONTENT_TYPE_DIR) {
			FileIterator* it = file.getIterator();
			File* child;

			while ((child = it->next())  !=  NULL) {
				addResource(*child);
				delete child;
			}

			delete it;
		}
	} else {
		try {
			Engine::getInstance()->addResource(file);
		} catch (Exception& ex) {
			fprintf(stderr, "ERROR loading resource file %s: %s\n", file.getPath()->toString(), ex.what());
		}
	}
}


void Controller::keyPressed(SDL_keysym key)
{
	Engine* engine = Engine::getInstance();

	SDLKey k = key.sym;

	if (k == SDLK_PLUS  ||  k == SDLK_KP_PLUS) {
		moveFactor *= 2.0f;
	} else if (k == SDLK_MINUS  ||  k == SDLK_KP_MINUS) {
		moveFactor /= 2.0f;
	} else if (k == SDLK_w) {
		moveForwardFactor = 1.0f;
	} else if (k == SDLK_s) {
		moveForwardFactor = -1.0f;
	} else if (k == SDLK_a) {
		moveSidewardFactor = 1.0f;
	} else if (k == SDLK_d) {
		moveSidewardFactor = -1.0f;
	} else if (k == SDLK_q) {
		moveUpFactor = 1.0f;
	} else if (k == SDLK_y) {
		moveUpFactor = -1.0f;
	} else if (k == SDLK_c) {
		engine->switchDrawing();
	} else if (k == SDLK_p) {
		printf("Camera position: (%f, %f, %f)\n", engine->getCamera()->getPosition().getX(),
				engine->getCamera()->getPosition().getY(), engine->getCamera()->getPosition().getZ());
	} else if (k == SDLK_x) {
		programRunning = false;
	} else if (k == SDLK_l) {
		engine->advanceGameTime(0, 30);
	} else if (k == SDLK_t) {
		DefaultRenderer* renderer = (DefaultRenderer*) engine->getRenderer();

		switch (renderer->getTransparencyMode()) {
		case DefaultRenderer::WeightedAverage:
			renderer->setTransparencyMode(DefaultRenderer::DepthPeeling);
			break;
		case DefaultRenderer::DepthPeeling:
			renderer->setTransparencyMode(DefaultRenderer::WeightedAverage);
			break;
		}

		forceStatisticsUpdate = true;
	} else if (k == SDLK_KP4) {
		DefaultRenderer* renderer = (DefaultRenderer*) engine->getRenderer();

		if (renderer->getTransparencyMode() == DefaultRenderer::DepthPeeling) {
			int passCount = renderer->getTransparencyPassCount();

			if (passCount != 0) {
				renderer->setTransparencyPassCount(passCount-1);
				forceStatisticsUpdate = true;
			}
		}
	} else if (k == SDLK_KP6) {
		DefaultRenderer* renderer = (DefaultRenderer*) engine->getRenderer();

		if (renderer->getTransparencyMode() == DefaultRenderer::DepthPeeling) {
			int passCount = renderer->getTransparencyPassCount();
			renderer->setTransparencyPassCount(passCount+1);
			forceStatisticsUpdate = true;
		}
	}
}


void Controller::keyReleased(SDL_keysym key)
{
	switch (key.sym) {
	case SDLK_w:
	case SDLK_s:
		moveForwardFactor = 0.0f;
		break;
		break;
	case SDLK_a:
	case SDLK_d:
		moveSidewardFactor = 0.0f;
		break;
	case SDLK_q:
	case SDLK_y:
		moveUpFactor = 0.0f;
		break;
	default:
		break;
	}
}


void Controller::mouseButtonPressed(Uint8 button, Uint16 x, Uint16 y)
{
	if (button == SDL_BUTTON_LEFT) {
		lastMouseX = x;
		lastMouseY = y;
	}
}


void Controller::mouseButtonReleased(Uint8 button, Uint16 x, Uint16 y)
{
	if (button == SDL_BUTTON_LEFT) {
		lastMouseX = -1;
		lastMouseY = -1;
	}
}


void Controller::mouseMotion(Uint16 x, Uint16 y)
{
	Engine* engine = Engine::getInstance();
	Camera* cam = engine->getCamera();

	if (lastMouseX != -1) {
		int xOffs = x - lastMouseX;
		int yOffs = y - lastMouseY;

		cam->rotateHorizontal(xOffs * -0.005f);
		cam->rotateVertical(yOffs * 0.005f);

		lastMouseX = x;
		lastMouseY = y;
	}
}


