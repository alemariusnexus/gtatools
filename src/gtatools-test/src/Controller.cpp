/*
	Copyright 2010-2012 David "Alemarius Nexus" Lerch

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
#include <gta/resource/mesh/ManagedMeshPointer.h>
#include <gta/resource/texture/ManagedTextureSource.h>
#include <gta/resource/texture/NullTextureSource.h>
#include <gta/resource/mesh/StaticMeshPointer.h>
#include <gta/scene/StaticSceneObject.h>
#include <gta/StaticMapItemDefinition.h>
#include <gta/ItemManager.h>
#include <gta/scene/DefaultRenderer.h>
#include <gta/resource/texture/TextureIndexer.h>
#include <gta/resource/texture/TextureArchive.h>
#include <gtaformats/util/math/Quaternion.h>
#include <gta/MeshGenerator.h>
#include <gtaformats/ifp/IFPAnimation.h>
#include <gtaformats/ifp/IFPFrame.h>
#include <gtaformats/ifp/IFPLoader.h>
#include <gtaformats/ifp/IFPObject.h>
#include <gtaformats/ifp/IFPRootFrame.h>
#include <gtaformats/util/Exception.h>
#include <gtaformats/util/CRC32.h>
#include "AnimatedBone.h"
#include <gta/resource/animation/ManagedAnimationPackagePointer.h>
#include <gta/gldebug.h>
#include <gta/scene/visibility/PVSDatabase.h>
#include <gta/scene/SceneObjectDefinitionInfo.h>





#define STATISTICS_UPDATE_INTERVAL 500




Controller::Controller()
		: lastFrameStart(0), lastMeasuredFrameStart(0), moveFactor(1.0f),
		  moveForwardFactor(0.0f), moveSidewardFactor(0.0f), moveUpFactor(0.0f), firstFrame(true),
		  framesSinceLastMeasure(0), lastMouseX(-1), lastMouseY(-1), printCacheStatistics(false),
		  programRunning(true), forceStatisticsUpdate(false)
{
}





void BuildReducedMeshRecurse(MeshClump* origClump, MeshClump* newClump, MeshFrame* origFrame, MeshFrame* newParentFrame, int depth = 0)
{
	MeshFrame* newFrame = new MeshFrame;
	newFrame->setModelMatrix(origFrame->getModelMatrix());
	newFrame->setName(origFrame->getName());
	newFrame->setBoneID(origFrame->getBoneID());

	if (newParentFrame) {
		newParentFrame->addChild(newFrame);
	} else {
		newClump->setRootFrame(newFrame);
	}

	MeshGenerator gen;
	//Mesh* mesh = gen.createSphere(0.1f, 10, 10);
	float len = 0.025f;
	Mesh* mesh = gen.createBox(Vector3(-len, -2*len, -len), Vector3(len, 2*len, len));
	mesh->setFrame(newFrame);
	newClump->addMesh(mesh);

	for (Mesh::SubmeshIterator it = mesh->getSubmeshBegin() ; it != mesh->getSubmeshEnd() ; it++) {
		Submesh* submesh = *it;

		Material* mat = new Material;
		mat->setColor(255 - depth*25, 0, 0, 255);

		submesh->setMaterial(mat);
	}

	for (MeshFrame::ChildIterator it = origFrame->getChildBegin() ; it != origFrame->getChildEnd() ; it++) {
		MeshFrame* child = *it;
		BuildReducedMeshRecurse(origClump, newClump, child, newFrame, depth+1);
	}
}


MeshClump* BuildReducedMesh(MeshClump* clump)
{
	MeshClump* newClump = new MeshClump;
	newClump->setBoneCount(clump->getBoneCount());
	BuildReducedMeshRecurse(clump, newClump, clump->getRootFrame(), NULL);
	return newClump;
}




void Controller::init()
{
	Engine* engine = Engine::getInstance();

	GameInfo* gameInfo = new GameInfo(GameInfo::GTASA, File(GTASA_PATH));
	engine->setDefaultGameInfo(gameInfo);

	printf("Initializing Bullet...\n");

	btDefaultCollisionConfiguration* config = new btDefaultCollisionConfiguration();
	btCollisionDispatcher* dispatcher = new btCollisionDispatcher(config);
	btBroadphaseInterface* broadphase = new btDbvtBroadphase();
	btSequentialImpulseConstraintSolver* solver = new btSequentialImpulseConstraintSolver();
	btDiscreteDynamicsWorld* world = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, config);

	world->setGravity(btVector3(0.0f, 0.0f, -9.81f));

	engine->setPhysicsWorld(world);

#ifndef GT_USE_OPENGL_ES
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	glEnable(GL_LINE_SMOOTH);
#endif
	glLineWidth(2.0f);

	gtaglInit();

	printf("OpenGL Version: %s\n", glGetString(GL_VERSION));
	printf("OpenGL Vendor: %s\n", glGetString(GL_VENDOR));
	printf("OpenGL Renderer: %s\n", glGetString(GL_RENDERER));

	printf("---------- SUPPORTED EXTENSIONS ----------\n");
	const char* extStr = gtaglGetSupportedExtensions();
	char* extStrCpy = new char[strlen(extStr)+1];
	strcpy(extStrCpy, extStr);

	char* ext;
	ext = strtok(extStrCpy, " ");
	while (ext) {
		printf("%s\n", ext);
		ext = strtok(NULL, " ");
	}
	printf("---------- END SUPPORTED EXTENSIONS ----------\n");

	delete[] extStrCpy;

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	basicTransAlgo = new BasicTransparencyAlgorithm;
	dpAlgo = new DepthPeelingAlgorithm;
	//wavgAlgo = new WeightedAverageAlgorithm;




	printf("Initializing scene...\n");

	Scene* scene = new Scene;

	Camera* cam = new Camera;
	engine->setCamera(cam);

	engine->setScene(scene);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);

	glDepthFunc(GL_LEQUAL);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	engine->getMeshCache()->resize(40 * 1000000); // 40MB
	engine->getTextureCache()->resize(150 * 1000000); // 150MB
	engine->getCollisionMeshCache()->resize(30 * 1000000);
	engine->getAnimationCache()->resize(100 * 1000000);


	printf("Loading resources...\n");
	//addResource(File(GTASA_PATH "/anim/anim.img"));
	addResource(File(GTASA_PATH "/models/gta3.img"));
	addResource(File(GTASA_PATH "/models/gta_int.img"));
	addResource(File(GTASA_PATH "/anim/ped.ifp"));
	//engine->addResource(File(GTASA_PATH ""));
	//engine->addResource(File(GTASA_PATH "/models/generic/vehicle.txd"));


	printf("Loading DAT files...\n");
	engine->loadDAT(File(GTASA_PATH "/data/default.dat"), File(GTASA_PATH));
	engine->loadDAT(File(GTASA_PATH "/data/gta.dat"), File(GTASA_PATH));
	//engine->loadDAT(File(GTASA_PATH "/data/gta_vc.dat"), File(GTASA_PATH));
	//engine->loadDAT(File(GTASA_PATH "/data/gta3.dat"), File(GTASA_PATH));
	//engine->loadDAT(File(GTASA_PATH "/data/test.dat"), File(GTASA_PATH));




	const char* AnimationData[] = {
		/*	Mesh Name				TXD Name				IFP Name				Animation Name	*/

			"jizzy",				"jizzy",				"ped",					"atm",
			"jizzy",				"jizzy",				"ped",					"bike_fall_off",
			"jizzy",				"jizzy",				"ped",					"car_getinl_lhs",
			"jizzy",				"jizzy",				"ped",					"climb_jump",
			"jizzy",				"jizzy",				"ped",					"drown",
			"jizzy",				"jizzy",				"ped",					"arrestgun",
			"jizzy",				"jizzy",				"ped",					"bomber",
			"jizzy",				"jizzy",				"ped",					"gum_eat",
			"jizzy",				"jizzy",				"ped",					"handsup",
			"jizzy",				"jizzy",				"ped",					"hit_back",
			"jizzy",				"jizzy",				"ped",					"woman_runfatold",
			"jizzy",				"jizzy",				"ped",					"xpressscratch"
	};

	size_t numObjs = sizeof(AnimationData) / (sizeof(const char*) * 4);


	for (size_t j = 0 ; j < 1 ; j++) {
		for (size_t i = 0 ; i < numObjs ; i++) {
			ManagedMeshPointer* meshPtr = new ManagedMeshPointer(AnimationData[i*4]);
			ManagedTextureSource* texSrc = new ManagedTextureSource(AnimationData[i*4 + 1]);
			ManagedAnimationPackagePointer* animPtr = new ManagedAnimationPackagePointer(AnimationData[i*4 + 2]);

			AnimatedMapItemDefinition* def = new AnimatedMapItemDefinition(meshPtr, texSrc, NULL, animPtr, 500.0f, 0);

			AnimatedSceneObject* obj = new AnimatedSceneObject(def);
			obj->setCurrentAnimation(AnimationData[i*4 + 3]);
			obj->setModelMatrix(Matrix4::translation(2.0f * i, -2.0f * (j+1), 20.0f));

			scene->addSceneObject(obj);
		}
	}




	File pvsFile(GTASA_PATH "/visibility.pvs");

	PVSDatabase* pvs = new PVSDatabase;
	bool pvsBuilt = false;

	SceneObjectDefinitionDatabase* defDB = scene->getDefinitionDatabase();

	if (pvsFile.exists()) {
		printf("Loading PVS data from file '%s'...\n", pvsFile.getPath()->toString());

		Scene::ObjectList missingObjs;
		PVSDatabase::LoadingResult res = pvs->load(pvsFile, defDB, missingObjs,
				engine->getDefaultGameInfo()->getRootDirectory(), 0);

		if (res == PVSDatabase::ResultOK) {
			if (!missingObjs.empty()) {
				printf("Building PVS data from scratch for %u (of %u) missing or potentially modified "
						"objects...\n", missingObjs.size(), scene->getSceneObjectCount());
				pvs->calculatePVS(missingObjs.begin(), missingObjs.end());
			}

			pvsBuilt = true;
			printf("PVS data successfully loaded!\n");
		} else {
			printf("WARNING: PVS data was NOT successfully loaded: ");

			switch (res) {
			case PVSDatabase::ResultInvalidFormat:
				printf("Invalid format in file\n");
				break;
			case PVSDatabase::ResultWrongVersion:
				printf("Unsupported file version\n");
				break;
			}
		}
	}

	if (!pvsBuilt) {
		printf("Building PVS data from scratch. This may take some time...\n");
		pvs->buildSections(scene->getSceneObjectBegin(), scene->getSceneObjectEnd());
		pvs->calculatePVS(scene->getSceneObjectBegin(), scene->getSceneObjectEnd());
		printf("Writing PVS data to file '%s'\n", pvsFile.getPath()->toString());
		pvs->save(pvsFile, defDB);
		printf("PVS data successfully built!\n");

		pvsBuilt = true;
	}

	scene->setPVSDatabase(pvs);


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

	//cam->setPosition(475.285858f, 1377.026855f, 28.393959f);
	cam->setPosition(0.0f, 0.0f, 20.0f);

	GLException::checkError("After init");
}


void Controller::reshape(int w, int h)
{
	Engine* engine = Engine::getInstance();

	engine->setViewportSize(w, h);

	float aspect = (float) w / (float) h;
	glViewport(0, 0, w, h);

	DefaultRenderer* renderer = new DefaultRenderer;
	//renderer->setTransparencyAlgorithm(dpAlgo);
	//dpAlgo->setPassCount(0);
	renderer->setTransparencyAlgorithm(basicTransAlgo);
	engine->getScene()->setRenderer(renderer);

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

	GLException::checkError("After reshape");
}

uint64_t animStart = 0;


bool Controller::paint()
{
	uint64_t absS, absE;
	absS = GetTickcount();

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

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glUseProgram(0);


	lastFrameStart = time;

	time = GetTickcount();

	if (firstFrame) {
		lastMeasuredFrameStart = time;
		firstFrame = false;
	} else {
		framesSinceLastMeasure++;
	}

	uint64_t passed = time - lastMeasuredFrameStart;

	Engine::StringResourceCache* mcache = engine->getMeshCache();
	Engine::StringResourceCache* tcache = engine->getTextureCache();

	if (forceStatisticsUpdate  ||  passed > STATISTICS_UPDATE_INTERVAL) {
		float fps = framesSinceLastMeasure / (passed/1000.0f);

		int8_t gameH, gameM;
		engine->getGameTime(gameH, gameM);

		float mc = (float) mcache->getOccupiedSize() / mcache->getCapacity();
		float tc = (float) tcache->getOccupiedSize() / tcache->getCapacity();

		char transMode[64];

		DefaultRenderer* renderer = (DefaultRenderer*) engine->getScene()->getRenderer();

		TransparencyAlgorithm* algo = renderer->getTransparencyAlgorithm();

		sprintf(transMode, "Depth Peeling (%d passes/frame)", dpAlgo->getPassCount());

		char title[128];
		sprintf(title, WINDOW_BASE_TITLE " - %.2f FPS - MC: %.2f%% - TC: %.2f%% - %.2d:%.2d - Transparency Mode: %s",
				fps, mc*100.0f, tc*100.0f, gameH, gameM, transMode);
		SDL_WM_SetCaption(title, NULL);

		lastMeasuredFrameStart = time;
		framesSinceLastMeasure = 0;

		forceStatisticsUpdate = false;
	}

	absE = GetTickcount();

	GLException::checkError("After paint");

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


void Controller::keyPressed(SDL_keysym evt)
{
	Engine* engine = Engine::getInstance();

	SDLKey k = evt.sym;

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
	} else if (k == SDLK_m) {
		dpAlgo->setPassCount(dpAlgo->getPassCount()+1);
	} else if (k == SDLK_n) {
		if (dpAlgo->getPassCount() != 0) {
			dpAlgo->setPassCount(dpAlgo->getPassCount()-1);
		}
	}
}


void Controller::keyReleased(SDL_keysym evt)
{
	SDLKey k = evt.sym;

	switch (k) {
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


void Controller::mouseButtonPressed(Uint8 button, int x, int y)
{
	if (button == SDL_BUTTON_LEFT) {
		lastMouseX = x;
		lastMouseY = y;
	}
}


void Controller::mouseButtonReleased(Uint8 button, int x, int y)
{
	if (button == SDL_BUTTON_LEFT) {
		lastMouseX = -1;
		lastMouseY = -1;
	}
}


void Controller::mouseMotion(int x, int y)
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



