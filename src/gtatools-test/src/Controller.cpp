/*
	Copyright 2010-2014 David "Alemarius Nexus" Lerch

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
#include <nxcommon/ResourceCache.h>
#include <gta/scene/Scene.h>
#include <gta/resource/collision/COLMeshConverter.h>
#include <nxcommon/util.h>
#include <nxcommon/strutil.h>
#include <cstdio>
#include <algorithm>
#include <btBulletDynamicsCommon.h>
#include <gtaformats/gtadff.h>
#include <gta/resource/mesh/ManagedMeshPointer.h>
#include <gta/resource/texture/ManagedTextureSource.h>
#include <gta/resource/texture/NullTextureSource.h>
#include <gta/resource/mesh/StaticMeshPointer.h>
#include <gta/resource/collision/StaticCollisionShapePointer.h>
#include <gta/resource/physics/StaticPhysicsPointer.h>
#include <gta/MapItemDefinition.h>
#include <gta/ItemManager.h>
#include <gta/render/DefaultRenderer.h>
#include <gta/resource/texture/TextureIndexer.h>
#include <gta/resource/texture/TextureArchive.h>
#include <nxcommon/math/Quaternion.h>
#include <nxcommon/file/NullFileFinder.h>
#include <gta/MeshGenerator.h>
#include <gtaformats/ifp/IFPAnimation.h>
#include <gtaformats/ifp/IFPRotFrame.h>
#include <gtaformats/ifp/IFPLoader.h>
#include <gtaformats/ifp/IFPObject.h>
#include <gtaformats/ifp/IFPRotTransFrame.h>
#include <gtaformats/ifp/IFPRotTransScaleFrame.h>
#include <gtaformats/col/COLLoader.h>
#include <gtaformats/gtagxt.h>
#include <nxcommon/exception/Exception.h>
#include <nxcommon/CRC32.h>
#include <nxcommon/file/DefaultFileFinder.h>
#include <nxcommon/math/project.h>
#include <nxcommon/log.h>
#include <gta/resource/animation/ManagedAnimationPackagePointer.h>
#include <gta/gldebug.h>
#include <gta/scene/visibility/PVSDatabase.h>
#include <gta/scene/SceneObjectDefinitionInfo.h>
#include <gta/DefaultIPLStreamingFileProvider.h>
#include <gta/scene/objects/DirectionalLightSource.h>
#include <gta/scene/objects/PointLightSource.h>
#include <gta/scene/objects/SpotLightSource.h>
#include <gta/scene/objects/AmbientLightSource.h>
#include <gta/scene/objects/MapSceneObject.h>
#include <gta/scene/objects/SimpleDynamicSceneObject.h>
#include <gta/scene/objects/Vehicle.h>
#include <gta/resource/animation/AnimationCacheEntry.h>
#include <gta/resource/smesh/ShadowMesh.h>
#include <gta/resource/smesh/ShadowVolumeGenerator.h>
#include <gta/resource/smesh/ManagedShadowMeshPointer.h>
#include <gta/scene/raycasting/RayCaster.h>
#include <gta/scene/raycasting/CollisionRayCastingHandler.h>





#define STATISTICS_UPDATE_INTERVAL 500




// Highly experimental and unusable. Keep it disabled.
//#define ENABLE_VEHICLE_TEST





Controller::Controller()
		: lastFrameStart(0), lastMeasuredFrameStart(0), moveFactor(1.0f),
		  moveForwardFactor(0.0f), moveSidewardFactor(0.0f), moveUpFactor(0.0f), firstFrame(true),
		  framesSinceLastMeasure(0), lastMouseX(-1), lastMouseY(-1), printCacheStatistics(false),
		  programRunning(true), forceStatisticsUpdate(false), freeRunning(true), increaseTime(false),
		  increaseTimeHold(false)
{
}





// Position of the moving test spotlight
SpotLightSource* sl;
Vector3 slBasePos(-1974.994873f, 254.986267f, 200.536682f);




void Controller::init()
{
	// Get game root directory from config.xml
	gtaRootPath = File(configDoc.RootElement()->FirstChildElement("game")->FirstChildElement("rootpath")->GetText());

	if (!gtaRootPath.isDirectory()) {
		LogError("Game root path '%s' is not a directory!", gtaRootPath.toString().get());
		exit(1);
	}


	Engine* engine = Engine::getInstance();

	GameInfo::VersionMode ver;

	// Determine the game version
	if (File(gtaRootPath, "gta_sa.exe").exists()) {
		ver = GameInfo::GTASA;
		LogInfo("Found GTA San Andreas installation!");
	} else if (File(gtaRootPath, "gta-vc.exe").exists()) {
		ver = GameInfo::GTAVC;
		LogInfo("Found GTA Vice City installation!");
	} else if (File(gtaRootPath, "gta3.exe").exists()) {
		ver = GameInfo::GTAIII;
		LogInfo("Found GTA III installation!");
	} else {
		LogError("Unable to guess GTA version!");
		exit(1);
	}

	GameInfo gameInfo(ver, gtaRootPath);
	engine->setDefaultGameInfo(gameInfo);

	// Turn on line smoothing.
#ifndef GT_USE_OPENGL_ES
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	glEnable(GL_LINE_SMOOTH);
#endif
	glLineWidth(0.5f);


	// Also calls glewInit()
	gtaglInit();

	engine->setupDebug();


	// Show OpenGL information for debugging purposes.
	LogInfo("OpenGL Version: %s", glGetString(GL_VERSION));
	LogInfo("OpenGL Vendor: %s", glGetString(GL_VENDOR));
	LogInfo("OpenGL Renderer: %s", glGetString(GL_RENDERER));

	LogInfo("---------- SUPPORTED EXTENSIONS ----------");
	const char* extStr = gtaglGetSupportedExtensions();
	char* extStrCpy = new char[strlen(extStr)+1];
	strcpy(extStrCpy, extStr);

	char* ext;
	ext = strtok(extStrCpy, " ");
	while (ext) {
		LogInfo("%s", ext);
		ext = strtok(NULL, " ");
	}
	LogInfo("---------- END SUPPORTED EXTENSIONS ----------");

	delete[] extStrCpy;




	LogInfo("Initializing scene...");

	Scene* scene = new Scene;

	Camera* cam = new Camera;
	engine->setCamera(cam);

	// To make objects stream in around the camera
	scene->addStreamingViewpoint(cam);

	engine->setScene(scene);

	if (gameInfo.getVersionMode() == GameInfo::GTASA) {
		// GTA SA has binary IPL streaming files (e.g. LAn_stream1.ipl) inside the IMG archives. Tell the engine
		// to use them.

		DefaultIPLStreamingFileProvider* sfProv = new DefaultIPLStreamingFileProvider;
		File gta3imgFile(gtaRootPath, "/models/gta3.img");
		sfProv->addSearchDirectory(gta3imgFile);

		File gtaintimgFile(gtaRootPath, "/models/gta_int.img");
		sfProv->addSearchDirectory(gtaintimgFile);

		engine->getIPLLoader()->setStreamingFileProvider(sfProv);
	}

	glDisable(GL_SCISSOR_TEST);
	glDisable(GL_DITHER);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);

	glDepthFunc(GL_LEQUAL);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Just some random sizes for the various caches.
	engine->getMeshCache()->resize(100 * 1000000); // 100MB
	engine->getTextureCache()->resize(150 * 1000000); // 150MB
	engine->getCollisionMeshCache()->resize(30 * 1000000); // 30MB
	engine->getAnimationCache()->resize(100 * 1000000); // 100MB
	engine->getPhysicsCache()->resize(100 * 1000000); // 100MB


	// ********** RESOURCE LOADING **********

	// This will create indices of various files, to be able to find models, textures etc. quickly by name.

	LogInfo("Loading resources...");

	if (gameInfo.getVersionMode() == GameInfo::GTASA) {
		addResource(File(gtaRootPath, "/models/gta_int.img"));
	}

	//addResource(File(GTASA_PATH "/anim/anim.img"));
	addResource(File(gtaRootPath, "/models/gta3.img"));

	addResource(File(gtaRootPath, "/anim/ped.ifp"));
	engine->addResource(File(gtaRootPath, "/models/generic/vehicle.txd"));


	// ********** DAT LOADING **********

	// This will mainly load the item definitions from IDE files and MapSceneObjects from IPL files.

	LogInfo("Loading DAT files...");
	engine->loadDAT(File(gtaRootPath, "/data/default.dat"), gtaRootPath);

	switch (gameInfo.getVersionMode()) {
	case GameInfo::GTASA:
		engine->loadDAT(File(gtaRootPath, "/data/gta.dat"), gtaRootPath);
		//engine->loadDAT(File(GTASA_PATH "/data/test.dat"), File(GTASA_PATH));
		break;

	case GameInfo::GTAVC:
		engine->loadDAT(File(gtaRootPath, "/data/gta_vc.dat"), gtaRootPath);
		break;

	case GameInfo::GTAIII:
		engine->loadDAT(File(gtaRootPath, "/data/gta3.dat"), gtaRootPath);
		//engine->loadDAT(File(GTASA_PATH "/data/test.dat"), File(GTASA_PATH));
		break;
	}


	// Some dynamic light testing.

	sl = new SpotLightSource;
	sl->setShadowCastingEnabled(false);
	sl->setDiffuseColor(Vector4(5.0f, 0.0f, 0.0f, 0.0f));
	sl->setAmbientColor(Vector3(0.2f, 0.2f, 0.2f));
	sl->setPosition(slBasePos);
	sl->setDirection(Vector3::UnitZ);
	sl->setCutoffAngleCosine(cos((30.0f / 180.0f) * M_PI));
	scene->addSceneObject(sl);

	AmbientLightSource* al = new AmbientLightSource;
	al->setAmbientColor(Vector3(0.2f, 0.2f, 0.2f));
	scene->addSceneObject(al);



	// Setting a TXD hierarchy. When a texture is not found in the base archive (infernus.txd), it will be searched
	// in the parent archive (vehicle.txd).

	TextureIndexer* txdIdx = Engine::getInstance()->getTextureIndexer();

	TextureArchive* infernusArchive = txdIdx->findArchive("infernus");
	TextureArchive* vehicleArchive = txdIdx->findArchive("vehicle");

	infernusArchive->setParent(vehicleArchive);


	{
		// Show some animated peds.

		struct AnimTest
		{
			const char* meshName;
			const char* txdName;
			const char* ifpName;
			const char* animName;
		};

		AnimTest animTestsSA[] = {
				{"jizzy",		"jizzy",		"ped",		"run_stop"},
				{"swmotr4",		"swmotr4",		"ped",		"bike_pickupr"},
				{"hmost",		"hmost",		"ped",		"walk_civi"},
				{"lapd1",		"lapd1",		"ped",		"arrestgun"},
				{"army",		"army",			"ped",		"gunmove_fwd"},
				{"wmybmx",		"wmybmx",		"ped",		"handsup"},
				{"male01",		"male01",		"ped",		"run_player"},

				{NULL,			NULL,			NULL,		NULL}
		};

		AnimTest animTestsVC[] = {
				{"wfogo", 		"wfogo", 		"ped", 		"woman_walkshop"},
				{"swat", 		"swat", 		"ped", 		"seat_up"},
				{"wmybu", 		"wmybu", 		"ped", 		"ev_dive"},
				{"medic", 		"medic", 		"ped", 		"xpressscratch"},

				{NULL,			NULL,			NULL,		NULL}
		};

		AnimTest animTestsIII[] = {
				{"taxi_d",		"taxi_d",		"ped",		"weapon_throwu"},
				{"fireman",		"fireman",		"ped",		"punchr"},
				{"prostitute",	"prostitute",	"ped",		"fucku"},
				{"cas_man",		"cas_man",		"ped",		"fightsh_f"},

				{NULL,			NULL,			NULL,		NULL}
		};

		AnimTest* animTests;

		if (gameInfo.getVersionMode() == GameInfo::GTASA) {
			animTests = animTestsSA;
		} else if (gameInfo.getVersionMode() == GameInfo::GTAVC) {
			animTests = animTestsVC;
		} else if (gameInfo.getVersionMode() == GameInfo::GTAIII) {
			animTests = animTestsIII;
		}

		for (size_t i = 0 ; animTests[i].meshName != NULL ; i++) {
			const AnimTest& test = animTests[i];

			MapItemDefinition* def = new MapItemDefinition (
					new ManagedMeshPointer(test.meshName), new ManagedTextureSource(test.txdName), NULL, NULL, NULL,
					500.0f, 0);
			def->setAnimationPackagePointer(new ManagedAnimationPackagePointer(test.ifpName));

			MapSceneObject* mobj = new MapSceneObject;

			MapSceneObjectLODInstance* lodInst = new MapSceneObjectLODInstance(def);
			mobj->addLODInstance(lodInst);

			mobj->setModelMatrix(Matrix4::translation(i * 2.0f, -5.0f, 50.0f));
			mobj->setCurrentAnimation(test.animName);

			scene->addSceneObject(mobj);
		}
	}


#ifdef ENABLE_VEHICLE_TEST
	veh = new VehicleController;

	veh->init();
#endif


	PVSDatabase* pvs = scene->getStreamingManager()->getPVSDatabase();

	File pvsFile(gtaRootPath, "/visibility.pvs");

	bool pvsBuilt = false;

#if 0
	// Load PVS data from a file. This is still a bit experimental and may fail, so it's disabled by default.

	if (pvsFile.exists()) {
		LogInfo("Loading PVS data from file '%s'...", pvsFile.getPath().toString().get());

		PVSDatabase::LoadingResult res = pvs->load(pvsFile, engine->getDefaultGameInfo().getRootDirectory());

		if (res == PVSDatabase::ResultOK) {
			size_t numUncalculated = pvs->getUncalculatedObjectCount();
			if (numUncalculated != 0) {
				LogInfo("Building PVS data from scratch for %u (of %u) missing or potentially modified "
						"objects...", numUncalculated, scene->getSceneObjectCount());
				//pvs->calculatePVS();
			}

			pvsBuilt = true;
			LogInfo("PVS data successfully loaded!");
		} else {
			const char* errstr = "";

			switch (res) {
			case PVSDatabase::ResultInvalidFormat:
				errstr = "Invalid format in file";
				break;
			case PVSDatabase::ResultWrongVersion:
				errstr = "Unsupported file version";
				break;
			}

			LogWarning("PVS data was NOT successfully loaded: %s", errstr);
		}
	}
#endif

	if (!pvsBuilt) {
		LogInfo("Building PVS data from scratch. This may take some time...");
		pvs->calculateSections(1000.0f, 1000.0f, 2000.0f);
		pvs->calculatePVS(8);
		LogInfo("Writing PVS data to file '%s'", pvsFile.getPath().toString().get());

#if 0
		pvs->save(pvsFile);
#endif

		LogInfo("PVS data successfully built!");

		pvsBuilt = true;
	}

	pvs->setEnabled(false);


	cam->setPosition(0.0f, 0.0f, 50.0f);

	LogInfo("");
}



void Controller::reshape(int w, int h)
{
	Engine* engine = Engine::getInstance();

	engine->setViewportSize(w, h);

	float aspect = (float) w / (float) h;
	glViewport(0, 0, w, h);

	// We have to recreate the renderer each time the viewport is resized.
	DefaultRenderer* renderer = new DefaultRenderer;
	engine->getScene()->setRenderer(renderer);

	float l = aspect*-0.7;
	float r = aspect*0.7;
	float b = -0.7;
	float t = 0.7;
	float n = 1.0;
	float f = 3000.0;

	engine->getCamera()->getFrustum().setDistances(l, r, t, b, n ,f);
}


bool Controller::paint()
{
	uint64_t time = GetTickcount();

	Engine* engine = Engine::getInstance();

	Camera* cam = engine->getCamera();

	uint64_t timePassed = 0;
	float timeFactor = 0.0f;

	// To make things like movement using the WASD keys or rotating using the mouse independent of the frame rate, we will scale
	// them by a factor that is dependent on the time passed since the last frame.
	if (lastFrameStart != 0) {
		timeFactor = (time - lastFrameStart)*0.025f;
		timePassed = time - lastFrameStart;
	}

#ifdef ENABLE_VEHICLE_TEST
	timePassed = (1.0f/60.0f) * 1000;
#endif

#if 1
#ifndef ENABLE_VEHICLE_TEST
	// Move the camera depending on which keys are currently pressed. See the key*() methods.
	cam->move(timeFactor*moveForwardFactor*moveFactor);
	cam->moveSideways(timeFactor*moveSidewardFactor*moveFactor);
	cam->moveUp(timeFactor*moveUpFactor*moveFactor);
#endif
#else
	// For the vehicle test, the camera is controlled by an external, spherical camera controller

	Vector3 cpos(0.0f, -5.0f, 2.5f);
	cpos = veh->getModelMatrix() * cpos;

	cam->setPosition(cpos);

	Vector3 target(0.0f, 1.0f, -0.5f);
	Vector3 up(0.0f, 0.0f, 1.0f);

	cam->lookAt(target, up);
#endif

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// The time that our test spotlight takes to move full circle.
	uint64_t slInterval = 5000;

	// The current angle of our spotlight in its circle.
	float slAngle = 2.0f * M_PI * ((GetTickcount() % slInterval) / (float) slInterval);

	Vector3 slPos(slBasePos);
	slPos += Vector3(sin(slAngle), cos(slAngle), 0.0f) * 50.0f;
	sl->setPosition(slPos);

	if (!firstFrame) {
		engine->renderFrame();
	}

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glUseProgram(0);

	lastFrameStart = time;

	time = GetTickcount();

	if (firstFrame) {
		lastMeasuredFrameStart = time;
	} else {
		framesSinceLastMeasure++;
	}

	uint64_t passed = time - lastMeasuredFrameStart;

	Engine::StringResourceCache* mcache = engine->getMeshCache();
	Engine::StringResourceCache* tcache = engine->getTextureCache();

	if (forceStatisticsUpdate  ||  passed > STATISTICS_UPDATE_INTERVAL) {
		// Show some statistics like FPS or cache sizes in the window title.

		float fps = framesSinceLastMeasure / (passed/1000.0f);

		int8_t gameH, gameM;
		engine->getGameTime(gameH, gameM);

		float mc = (float) mcache->getOccupiedSize() / mcache->getCapacity();
		float tc = (float) tcache->getOccupiedSize() / tcache->getCapacity();

		char transMode[64];

		DefaultRenderer* renderer = (DefaultRenderer*) engine->getScene()->getRenderer();

		sprintf(transMode, "Depth Peeling (%d passes/frame)", renderer->dpNumPasses);

		Scene* scene = engine->getScene();

		float renderingPercentage = (scene->getSceneObjectCount() == 0) ? 0.0f
				: ((float) scene->getLastVisibleObjectCount() / scene->getSceneObjectCount());

		char title[512];
		sprintf(title, WINDOW_BASE_TITLE " - %.2f FPS - MC: %.2f%% - TC: %.2f%% - %.2d:%.2d - Transparency "
				"Mode: %s, Frustum Culling: %s, Backface Culling: %s, Rendering %d/%d objects (%.3f%%)",
				fps, mc*100.0f, tc*100.0f, gameH, gameM, transMode,
				scene->isFrustumCullingEnabled() ? "on" : "off",
				glIsEnabled(GL_CULL_FACE) == GL_TRUE ? "on" : "off",
				scene->getLastVisibleObjectCount(), scene->getSceneObjectCount(),
				renderingPercentage * 100.0f);
		SDL_WM_SetCaption(title, NULL);

		lastMeasuredFrameStart = time;
		framesSinceLastMeasure = 0;

		forceStatisticsUpdate = false;
	}

	if (freeRunning  ||  increaseTime) {
		// Increase the engine time. Needed for things like animations or the physics engine.

		engine->advanceFrame(timePassed);

#ifdef ENABLE_VEHICLE_TEST
		veh->update(timePassed);
#endif

		if (!increaseTimeHold)
			increaseTime = false;
	} else {
		engine->advanceFrame(0);

#ifdef ENABLE_VEHICLE_TEST
		veh->update(0);
#endif
	}

	GLException::checkError("After paint");

	firstFrame = false;

	return programRunning;
}


void Controller::shutdown()
{
	Engine* engine = Engine::getInstance();

	delete engine->getScene();
	delete engine->getCamera();

	Engine::destroy();
}


void Controller::addResource(const File& file)
{
	Engine::getInstance()->addResource(file);
	return;
}



void Controller::keyPressed(SDL_keysym evt)
{
	Engine* engine = Engine::getInstance();

#ifdef ENABLE_VEHICLE_TEST
	veh->keyPressed(evt);
#endif

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
	} else if (k == SDLK_y  ||  k == SDLK_z) {
		// y and z for English and German keyboard layout.
		moveUpFactor = -1.0f;
	} else if (k == SDLK_p) {
		Camera* cam = engine->getCamera();
		Vector3 p = cam->getPosition();
		Vector3 t = cam->getTarget();
		Vector3 up = cam->getUp();

		LogInfo("cam->setPosition(%ff, %ff, %ff);", p.getX(), p.getY(), p.getZ());
		LogInfo("cam->lookAt(Vector3(%ff, %ff, %ff), Vector3(%ff, %ff, %ff));",
				t.getX(), t.getY(), t.getZ(), up.getX(), up.getY(), up.getZ());
	} else if (k == SDLK_x) {
		programRunning = false;
	} else if (k == SDLK_l) {
		engine->advanceGameTime(0, 30);
	} else if (k == SDLK_m) {
		DefaultRenderer* r = (DefaultRenderer*) engine->getScene()->getRenderer();
		r->dpNumPasses++;
	} else if (k == SDLK_n) {
		DefaultRenderer* r = (DefaultRenderer*) engine->getScene()->getRenderer();
		if (r->dpNumPasses != 0)
			r->dpNumPasses--;
	} else if (k == SDLK_f) {
		engine->getScene()->setFrustumCullingEnabled(!engine->getScene()->isFrustumCullingEnabled());
	} else if (k == SDLK_c) {
		if (glIsEnabled(GL_CULL_FACE) == GL_TRUE) {
			glDisable(GL_CULL_FACE);
		} else {
			glEnable(GL_CULL_FACE);
		}
	} else if (k == SDLK_b) {
		Renderer* renderer = engine->getScene()->getRenderer();
		renderer->setWireframeRendering(!renderer->isWireframeRendering());
	} else if (k == SDLK_i) {
		sl->setEnabled(!sl->isEnabled());
	} else if (k == SDLK_g) {
		engine->setFreezeVisibility(!engine->isVisibilityFrozen());
	} else if (k == SDLK_t) {
		Camera* cam = engine->getCamera();
		cam->setPosition(Vector3(0.0f, 0.0f, 0.0f));
		cam->lookAt(Vector3::NegativeUnitY, Vector3::UnitZ);
	} else if (k == SDLK_h) {
		freeRunning = !freeRunning;
	} else if (k == SDLK_0) {
		increaseTime = true;
		increaseTimeHold = true;
	} else if (k == SDLK_9) {
		increaseTime = true;
		increaseTimeHold = false;
	}
}


void Controller::keyReleased(SDL_keysym evt)
{
#ifdef ENABLE_VEHICLE_TEST
	veh->keyReleased(evt);
#endif

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
	case SDLK_z:
		moveUpFactor = 0.0f;
		break;
	case SDLK_0:
		increaseTimeHold = false;
		break;
	default:
		break;
	}
}


void Controller::mouseButtonPressed(Uint8 button, int x, int y)
{
#ifdef ENABLE_VEHICLE_TEST
	veh->mouseButtonPressed(button, x, y);
#endif

	if (button == SDL_BUTTON_LEFT) {
		lastMouseX = x;
		lastMouseY = y;
	}
}


void Controller::mouseButtonDoubleClicked(int x, int y)
{
#ifdef ENABLE_VEHICLE_TEST
	veh->mouseButtonDoubleClicked(x, y);
#endif
}


void Controller::mouseButtonReleased(Uint8 button, int x, int y)
{
#ifdef ENABLE_VEHICLE_TEST
	veh->mouseButtonReleased(button, x, y);
#endif

	if (button == SDL_BUTTON_LEFT) {
		lastMouseX = -1;
		lastMouseY = -1;
	}
}


void Controller::mouseMotion(int x, int y)
{
#ifdef ENABLE_VEHICLE_TEST
	veh->mouseMotion(x, y);
#endif

	Engine* engine = Engine::getInstance();
	Camera* cam = engine->getCamera();

	if (lastMouseX != -1) {
		int xOffs = x - lastMouseX;
		int yOffs = y - lastMouseY;

#ifndef ENABLE_VEHICLE_TEST
		cam->rotateHorizontal(xOffs * -0.005f);
		cam->rotateVertical(yOffs * 0.005f);
#endif

		lastMouseX = x;
		lastMouseY = y;
	}
}



