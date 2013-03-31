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
#include <gta/resource/collision/StaticCollisionShapePointer.h>
#include <gta/StaticMapItemDefinition.h>
#include <gta/ItemManager.h>
#include <gta/render/DefaultRenderer.h>
#include <gta/resource/texture/TextureIndexer.h>
#include <gta/resource/texture/TextureArchive.h>
#include <gtaformats/util/math/Quaternion.h>
#include <gta/MeshGenerator.h>
#include <gtaformats/ifp/IFPAnimation.h>
#include <gtaformats/ifp/IFPRotFrame.h>
#include <gtaformats/ifp/IFPLoader.h>
#include <gtaformats/ifp/IFPObject.h>
#include <gtaformats/ifp/IFPRotTransFrame.h>
#include <gtaformats/ifp/IFPRotTransScaleFrame.h>
#include <gtaformats/util/Exception.h>
#include <gtaformats/util/CRC32.h>
#include <gta/resource/animation/ManagedAnimationPackagePointer.h>
#include <gta/gldebug.h>
#include <gta/scene/visibility/PVSDatabase.h>
#include <gta/scene/SceneObjectDefinitionInfo.h>
#include <gta/DefaultIPLStreamingFileProvider.h>
#include <gta/scene/objects/DirectionalLightSource.h>
#include <gta/scene/objects/PointLightSource.h>
#include <gta/scene/objects/SpotLightSource.h>
#include <gta/scene/objects/AmbientLightSource.h>
#include <gta/scene/objects/AnimatedMapSceneObject.h>
#include <gta/scene/objects/MapSceneObject.h>
#include <gta/AnimatedMapItemDefinition.h>
#include <gta/resource/animation/AnimationCacheEntry.h>
#include <gta/resource/smesh/ShadowMesh.h>
#include <gta/resource/smesh/ShadowVolumeGenerator.h>
#include <gta/resource/smesh/ManagedShadowMeshPointer.h>





#define STATISTICS_UPDATE_INTERVAL 500




Controller::Controller()
		: lastFrameStart(0), lastMeasuredFrameStart(0), moveFactor(1.0f),
		  moveForwardFactor(0.0f), moveSidewardFactor(0.0f), moveUpFactor(0.0f), firstFrame(true),
		  framesSinceLastMeasure(0), lastMouseX(-1), lastMouseY(-1), printCacheStatistics(false),
		  programRunning(true), forceStatisticsUpdate(false)
{
}



class TestViewpoint : public StreamingViewpoint
{
public:
	TestViewpoint(Vector3 pos, float sdMul) : pos(pos), sdMul(sdMul) {}
	virtual Vector3 getStreamingViewpointPosition() const { return pos; }
	virtual int getStreamingFlags() const { return GraphicsStreaming; }
	virtual float getStreamingDistanceMultiplier() const { return sdMul; }

private:
	Vector3 pos;
	float sdMul;
};



ShadowMesh* shadowMesh;
ShaderProgram* testProgram;
ShaderProgram* envProgram;

float* testVertices;
uint32_t* testIndices;
uint32_t numTestVertices;
uint32_t numTestIndices;


#define FLOOR_RESOLUTION 200
#define FLOOR_HALF_LENGTH 100.0f
#define FLOOR_Z_VARIANCE 50.0f

float floorVerts[(FLOOR_RESOLUTION+1) * (FLOOR_RESOLUTION+1) * 3];
uint32_t floorInds[FLOOR_RESOLUTION*FLOOR_RESOLUTION*6];


SpotLightSource* sl;
Vector3 slBasePos(-1974.994873f, 254.986267f, 200.536682f);






void Controller::init()
{
	Engine* engine = Engine::getInstance();

	GameInfo::VersionMode ver;

	File rootDir(GTASA_PATH);

	if (File(rootDir, "gta_sa.exe").exists()) {
		ver = GameInfo::GTASA;
	} else if (File(rootDir, "gta-vc.exe").exists()) {
		ver = GameInfo::GTAVC;
	} else if (File(rootDir, "gta3.exe").exists()) {
		ver = GameInfo::GTAIII;
	} else {
		fprintf(stderr, "ERROR: Unable to guess GTA version!\n");
		exit(1);
	}

	GameInfo* gameInfo = new GameInfo(ver, File(GTASA_PATH));
	engine->setDefaultGameInfo(gameInfo);

	printf("Initializing Bullet...\n");

	btDefaultCollisionConfiguration* config = new btDefaultCollisionConfiguration();
	btCollisionDispatcher* dispatcher = new btCollisionDispatcher(config);
	btBroadphaseInterface* broadphase = new btDbvtBroadphase();
	btSequentialImpulseConstraintSolver* solver = new btSequentialImpulseConstraintSolver();
	btDiscreteDynamicsWorld* world = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, config);

	world->setGravity(btVector3(0.0f, 0.0f, -9.81f));

#ifndef GT_USE_OPENGL_ES
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	glEnable(GL_LINE_SMOOTH);
#endif
	glLineWidth(1.0f);

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




	printf("Initializing scene...\n");

	Scene* scene = new Scene;

	scene->setPhysicsWorld(world);

	scene->setPVSEnabled(true);
	PVSDatabase* pvs = scene->getPVSDatabase();

	Camera* cam = new Camera;
	engine->setCamera(cam);

	scene->addStreamingViewpoint(cam);

	//TestViewpoint* vp1 = new TestViewpoint(Vector3(500.0f, 0.0f, 0.0f), 1.0f);
	//scene->addStreamingViewpoint(vp1);

	engine->setScene(scene);

	if (gameInfo->getVersionMode() == GameInfo::GTASA) {
		DefaultIPLStreamingFileProvider* sfProv = new DefaultIPLStreamingFileProvider;
		File gta3imgFile(GTASA_PATH "/models/gta3.img");
		gta3imgFile.preloadIMG();
		sfProv->addSearchDirectory(gta3imgFile);

		File gtaintimgFile(GTASA_PATH "/models/gta_int.img");
		gtaintimgFile.preloadIMG();
		sfProv->addSearchDirectory(gtaintimgFile);

		engine->getIPLLoader()->setStreamingFileProvider(sfProv);
	}

	glDisable(GL_SCISSOR_TEST);
	glDisable(GL_DITHER);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);

	glDepthFunc(GL_LEQUAL);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//engine->getMeshCache()->resize(40 * 1000000); // 40MB
	engine->getMeshCache()->resize(25 * 1000000); // 40MB
	engine->getTextureCache()->resize(150 * 1000000); // 150MB
	engine->getCollisionMeshCache()->resize(30 * 1000000);
	engine->getAnimationCache()->resize(100 * 1000000);


	printf("Loading resources...\n");

	if (gameInfo->getVersionMode() == GameInfo::GTASA) {
		addResource(File(GTASA_PATH "/models/gta_int.img"));
	}

	//addResource(File(GTASA_PATH "/anim/anim.img"));
	addResource(File(GTASA_PATH "/models/gta3.img"));

	addResource(File(GTASA_PATH "/anim/ped.ifp"));
	//engine->addResource(File(GTASA_PATH ""));
	//engine->addResource(File(GTASA_PATH "/models/generic/vehicle.txd"));

	printf("Loading DAT files...\n");
	engine->loadDAT(File(GTASA_PATH "/data/default.dat"), File(GTASA_PATH));

	switch (gameInfo->getVersionMode()) {
	case GameInfo::GTASA:
		engine->loadDAT(File(GTASA_PATH "/data/gta.dat"), File(GTASA_PATH));
		break;

	case GameInfo::GTAVC:
		engine->loadDAT(File(GTASA_PATH "/data/gta_vc.dat"), File(GTASA_PATH));;
		break;

	case GameInfo::GTAIII:
		engine->loadDAT(File(GTASA_PATH "/data/gta3.dat"), File(GTASA_PATH));
		break;
	}

	//engine->loadDAT(File(GTASA_PATH "/data/test.dat"), File(GTASA_PATH));



	/*PointLightSource* pl = new PointLightSource;
	//pl->setDiffuseColor(Vector4(1.0f, 1.0f, 1.0f, 0.0f));
	pl->setAmbientColor(Vector3(0.5f, 0.0f, 0.0f));
	pl->setPosition(Vector3(-1974.994873f, 258.986267f, 42.536682f));
	pl->setLinearAttenuation(0.01f);
	//pl->setSpecularColor(Vector4(5.0f, 5.0f, 5.0f, 0.0f));
	//pl->setShininess(96.0f);
	scene->addSceneObject(pl);*/

	sl = new SpotLightSource;
	sl->setShadowCastingEnabled(false);
	//sl->setDiffuseColor(Vector4(5.0f, 0.0f, 0.0f, 0.0f));
	sl->setAmbientColor(Vector3(0.2f, 0.2f, 0.2f));
	sl->setPosition(slBasePos);
	sl->setDirection(Vector3::UnitZ);
	sl->setCutoffAngleCosine(cos((30.0f / 180.0f) * M_PI));
	scene->addSceneObject(sl);

	AmbientLightSource* al = new AmbientLightSource;
	al->setAmbientColor(Vector3(0.2f, 0.2f, 0.2f));
	scene->addSceneObject(al);

	/*DirectionalLightSource* dl = new DirectionalLightSource;
	dl->setDiffuseColor(Vector4(0.0f, 0.0f, 0.5f, 0.0f));
	dl->setDirection(Vector3::UnitZ);
	scene->addSceneObject(dl);*/

	/*DirectionalLightSource* dl = new DirectionalLightSource;
	dl->setShadowCastingEnabled(false);
	dl->setAmbientColor(Vector3(0.5f, 0.5f, 0.0f));
	Vector3 dir(1.0f, 0.0f, 1.0f);
	dir.normalize();
	dl->setDirection(dir);
	scene->addSceneObject(dl);*/

	/*DirectionalLightSource* dl2 = new DirectionalLightSource;
	dl2->setAmbientColor(Vector3(0.1f, 0.1f, 0.1f));
	Vector3 dir2(1.0f, 0.5f, 1.0f);
	dir2.normalize();
	dl2->setDirection(dir2);
	scene->addSceneObject(dl2);*/

	// -1974.994873f, 258.986267f, 42.536682f


	CString testMeshName;

	switch (gameInfo->getVersionMode()) {
	case GameInfo::GTASA:
		testMeshName = CString("jizzy");
		break;
	case GameInfo::GTAVC:
		testMeshName = CString("hmori");
		break;
	case GameInfo::GTAIII:
		testMeshName = CString("medic");
		break;
	}


	AnimationCacheEntry* pedEntry = (AnimationCacheEntry*) engine->getAnimationCache()->getEntryPointer(CString("ped")).getEntry();

	AnimationPackage* anpk = pedEntry->getPackage();

	CString* AnimationData = new CString[anpk->getAnimationCount() * 4];

	AnimationPackage::AnimIterator it;
	int idx = 0;
	for (it = anpk->getAnimationBegin() ; it != anpk->getAnimationEnd() ; it++, idx++) {
		Animation* anim = it->second;
		CString name = it->first;

		AnimationData[idx*4] = CString(testMeshName);
		AnimationData[idx*4 + 1] = CString(testMeshName);
		AnimationData[idx*4 + 2] = CString("ped");
		AnimationData[idx*4 + 3] = CString(name);
	}

	//size_t numObjs = anpk->getAnimationCount();
	size_t numObjs = 0;

	/*CString AnimationData[] = {
			"jizzy",					"jizzy",					"ped",					"arrestgun"
	};

	size_t numObjs = sizeof(AnimationData) / (sizeof(CString) * 4);*/




	for (size_t j = 0 ; j < 1 ; j++) {
		for (size_t i = 0 ; i < numObjs ; i++) {
			ManagedMeshPointer* meshPtr = new ManagedMeshPointer(AnimationData[i*4]);
			ManagedTextureSource* texSrc = new ManagedTextureSource(AnimationData[i*4 + 1]);
			ManagedAnimationPackagePointer* animPtr = new ManagedAnimationPackagePointer(AnimationData[i*4 + 2]);

			AnimatedMapItemDefinition* def = new AnimatedMapItemDefinition(meshPtr, texSrc, NULL, NULL, animPtr, 100.0f, 0);

			AnimatedMapSceneObject* obj = new AnimatedMapSceneObject;

			MapSceneObjectLODInstance* lodInst = new MapSceneObjectLODInstance(def);
			obj->addLODInstance(lodInst);

			obj->setCurrentAnimation(AnimationData[i*4 + 3]);
			obj->setModelMatrix(Matrix4::translation(2.0f * i, -2.0f * (j+1), 10.0f));

			scene->addSceneObject(obj);
		}
	}




	File pvsFile(GTASA_PATH "/visibility.pvs");

	bool pvsBuilt = false;

	if (pvsFile.exists()) {
		printf("Loading PVS data from file '%s'...\n", pvsFile.getPath()->toString().get());

		PVSDatabase::LoadingResult res = pvs->load(pvsFile, engine->getDefaultGameInfo()->getRootDirectory());

		if (res == PVSDatabase::ResultOK) {
			size_t numUncalculated = pvs->getUncalculatedObjectCount();
			if (numUncalculated != 0) {
				printf("Building PVS data from scratch for %u (of %u) missing or potentially modified "
						"objects...\n", numUncalculated, scene->getSceneObjectCount());
				//pvs->calculatePVS();
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
		pvs->calculateSections(250.0f, 250.0f, 2000.0f);
		pvs->calculatePVS(8);
		printf("Writing PVS data to file '%s'\n", pvsFile.getPath()->toString().get());
		pvs->save(pvsFile);
		printf("PVS data successfully built!\n");

		pvsBuilt = true;
	}




	printf("\n\n\n\n");

	for (int i = 0 ; i < 0 ; i++) {
		//Vector3 pos(RandomFloat(-50.0f, 50.0f), RandomFloat(-50.0f, 50.0f), RandomFloat(100.0f, 200.0f));
		Vector3 pos(sl->getPosition());

		MeshGenerator mg;

		Mesh* mesh = mg.createSphere(0.1f, 10, 10);
		mesh->setDynamicLightingEnabled(true);

		MeshClump* clump = new MeshClump;
		MeshFrame* frame = new MeshFrame;
		frame->setModelMatrix(Matrix4::translation(0.0f, 0.0f, 0.0f));
		clump->setRootFrame(frame);
		mesh->setFrame(frame);
		clump->addMesh(mesh);

		for (Mesh::SubmeshIterator it = mesh->getSubmeshBegin() ; it != mesh->getSubmeshEnd() ; it++) {
			Submesh* submesh = *it;

			Material* mat = new Material;
			mat->setColor(0, 0, 0, 255);

			submesh->setMaterial(mat);
		}

		btSphereShape* shape = new btSphereShape(2.5f);
		StaticCollisionShapePointer* colPtr = new StaticCollisionShapePointer(shape);

		StaticMeshPointer* meshPtr = new StaticMeshPointer(clump);
		StaticMapItemDefinition* def = new StaticMapItemDefinition(meshPtr, new NullTextureSource(), colPtr, NULL, 503.456f);

		MapSceneObject* obj = new MapSceneObject;

		MapSceneObjectLODInstance* lodInst = new MapSceneObjectLODInstance(def);
		obj->addLODInstance(lodInst);

		obj->setModelMatrix(Matrix4::translation(pos));

		btRigidBody* rb = obj->getRigidBody();

		btVector3 inertia;
		shape->calculateLocalInertia(1.0f, inertia);

		obj->setMass(1.0f);
		rb->setRestitution(RandomFloat(0.0f, 1.0f));

		scene->addSceneObject(obj);

		/*btScalar mass = 100.0f;
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
		//rigidBody->setLinearVelocity(btVector3(RandomFloat(-20.0, 20.0f), RandomFloat(-20.0f, 20.0f), RandomFloat(-20.0f, 20.0f)));*/
	}

	// <------ X+
	// V Y+
	cam->setPosition(0.0f, 0.0f, 0.0f);
	//cam->setPosition(999.427185f, -1385.898071f, 64.900925f);

	//cam->setPosition(162.309998f, -201.011093f, 35.131886f);
	//cam->lookAt(Vector3(0.757422f, 0.506264f, -0.412321f), Vector3(0.342802f, 0.229130f, 0.911053f));


	GLException::checkError("After init");
}


Vector4 lightPos = Vector4(0.0f, 0.0f, 75.0f, 1.0f);


void Controller::reshape(int w, int h)
{
	Engine* engine = Engine::getInstance();

	engine->setViewportSize(w, h);

	float aspect = (float) w / (float) h;
	glViewport(0, 0, w, h);

	DefaultRenderer* renderer = new DefaultRenderer;
	engine->getScene()->setRenderer(renderer);


	float l = aspect*-0.7;
	float r = aspect*0.7;
	float b = -0.7;
	float t = 0.7;
	float n = 1.0;
	float f = 3000.0;

	engine->getCamera()->getFrustum().setDistances(l, r, t, b, n ,f);

	/*// glFrustum(l, r, b, t, n, f):
	engine->setProjectionMatrix(Matrix4 (
		2*n/(r-l),		0,				0,					0,
		0,				2*n/(t-b),		0, 					0,
		(r+l)/(r-l),	(t+b)/(t-b),	(-(f+n))/(f-n),		-1,
		0,				0,				(-2*f*n)/(f-n),		0
	));*/

	GLException::checkError("After reshape");
}


unsigned int fib(int n)
{
    if (n == 0)
        return 0;
    else if (n == 1)
        return 1;
    else
        return fib(n-1) + fib(n-2);
}


bool Controller::paint()
{
	uint64_t absS, absE;
	absS = GetTickcount();

	uint64_t time = GetTickcount();

	Engine* engine = Engine::getInstance();

	Camera* cam = engine->getCamera();

	uint64_t timePassed = 0;
	float timeFactor = 0.0f;

	if (lastFrameStart != 0) {
		timeFactor = (time - lastFrameStart)*0.025f;
		timePassed = time - lastFrameStart;
	}

	cam->move(timeFactor*moveForwardFactor*moveFactor);
	cam->moveSideways(timeFactor*moveSidewardFactor*moveFactor);
	cam->moveUp(timeFactor*moveUpFactor*moveFactor);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	uint64_t slInterval = 5000;
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
		float fps = framesSinceLastMeasure / (passed/1000.0f);

		int8_t gameH, gameM;
		engine->getGameTime(gameH, gameM);

		float mc = (float) mcache->getOccupiedSize() / mcache->getCapacity();
		float tc = (float) tcache->getOccupiedSize() / tcache->getCapacity();

		char transMode[64];

		DefaultRenderer* renderer = (DefaultRenderer*) engine->getScene()->getRenderer();

		sprintf(transMode, "Depth Peeling (%d passes/frame)", renderer->dpNumPasses);
		//sprintf(transMode, "Depth Peeling (%d passes/frame)", 0);

		Scene* scene = engine->getScene();

		float renderingPercentage = (scene->getSceneObjectCount() == 0) ? 0.0f
				: ((float) scene->getLastVisibleObjectCount() / scene->getSceneObjectCount());

		char title[512];
		sprintf(title, WINDOW_BASE_TITLE " - %.2f FPS - MC: %.2f%% - TC: %.2f%% - %.2d:%.2d - Transparency "
				"Mode: %s, PVS: %s, Frustum Culling: %s, Backface Culling: %s, Rendering %d/%d objects (%.3f%%)",
				fps, mc*100.0f, tc*100.0f, gameH, gameM, transMode,
				scene->isPVSEnabled() ? "on" : "off", scene->isFrustumCullingEnabled() ? "on" : "off",
				glIsEnabled(GL_CULL_FACE) == GL_TRUE ? "on" : "off",
				scene->getLastVisibleObjectCount(), scene->getSceneObjectCount(),
				renderingPercentage * 100.0f);
		SDL_WM_SetCaption(title, NULL);

		lastMeasuredFrameStart = time;
		framesSinceLastMeasure = 0;

		forceStatisticsUpdate = false;
	}

	engine->advanceFrame(timePassed);

	absE = GetTickcount();

	GLException::checkError("After paint");

	firstFrame = false;

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
			fprintf(stderr, "ERROR loading resource file %s: %s\n", file.getPath()->toString().get(),
					ex.what());
		}
	}
}


bool wireframe = false;


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
	} else if (k == SDLK_p) {
		Camera* cam = engine->getCamera();
		Vector3 p = cam->getPosition();
		Vector3 t = cam->getTarget();
		Vector3 up = cam->getUp();
		printf("Camera is at (%f, %f, %f), looking at (%f, %f, %f), with up vector (%f, %f, %f)\n", p.getX(),
				p.getY(), p.getZ(), t.getX(), t.getY(), t.getZ(), up.getX(), up.getY(), up.getZ());
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
	} else if (k == SDLK_v) {
		engine->getScene()->setPVSEnabled(!engine->getScene()->isPVSEnabled());
	} else if (k == SDLK_c) {
		if (glIsEnabled(GL_CULL_FACE) == GL_TRUE) {
			glDisable(GL_CULL_FACE);
		} else {
			glEnable(GL_CULL_FACE);
		}
	} else if (k == SDLK_b) {
		/*if (wireframe)
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);*/

		wireframe = !wireframe;
	} else if (k == SDLK_u) {
		Camera* cam = Engine::getInstance()->getCamera();
		lightPos = Vector4(cam->getPosition(), 0.0f);
	} else if (k == SDLK_z) {
		Camera* cam = Engine::getInstance()->getCamera();
		lightPos = Vector4(cam->getPosition(), 1.0f);
	} else if (k == SDLK_i) {
		sl->setEnabled(!sl->isEnabled());
	} else if (k == SDLK_o) {
		sl->setShadowCastingEnabled(!sl->isShadowCastingEnabled());
	} else if (k == SDLK_g) {
		engine->setFreezeVisibility(!engine->isVisibilityFrozen());
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



