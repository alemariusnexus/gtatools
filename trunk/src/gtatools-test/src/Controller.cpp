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





#define STATISTICS_UPDATE_INTERVAL 500



struct FrameMesh
{
	int vcount, icount;
	float* vertices;
	uint32_t* indices;
};


StaticSceneObject* generateFrameMesh(DFFFrame* frame, Matrix4 mm)
{
	if (!frame->getBone())
		return NULL;

	MeshGenerator gen;

	FrameMesh fmesh;
	gen.createSphere(fmesh.vertices, fmesh.vcount, fmesh.indices, fmesh.icount, 1.0f, 4, 3);

	Mesh* mesh = new Mesh(fmesh.vcount, VertexFormatTriangles, 0, fmesh.vertices);

	Submesh* submesh = new Submesh(mesh, fmesh.icount, fmesh.indices);
	Material* mat = new Material;
	mat->setColor(255, 0, 0, 255);
	mesh->addMaterial(mat);
	submesh->setMaterial(mat);
	mesh->addSubmesh(submesh);

	MeshClump* mclump = new MeshClump;
	mclump->addMesh(mesh);

	StaticMeshPointer* meshPtr = new StaticMeshPointer(mclump);
	NullTextureSource* texSrc = new NullTextureSource;

	StaticMapItemDefinition* def = new StaticMapItemDefinition(meshPtr, texSrc, NULL, 100.0f);

	StaticSceneObject* obj = new StaticSceneObject(def, mm, NULL);

	return obj;
}


struct BoneObject
{
	StaticSceneObject* obj;
	Matrix4 mm;
	int32_t frameCount;
	bool rootFrames;
	IFPFrame** frames;
};


map<int32_t, BoneObject> BoneObjs;


void frameRecurse(DFFFrame* frame, Matrix4 mm = Matrix4())
{
	Scene* scene = Engine::getInstance()->getScene();

	for (DFFFrame::ChildIterator it = frame->getChildBegin() ; it != frame->getChildEnd() ; it++) {
		DFFFrame* cframe = *it;

		Matrix4 cmm = mm * Matrix4::translation(frame->getTranslation() * 20.0f) * frame->getRotation();

		StaticSceneObject* obj = generateFrameMesh(cframe, cmm);

		if (obj) {
			if (frame->getBone()) {
				BoneObject bobj;
				bobj.obj = obj;
				bobj.mm = cmm;
				bobj.frameCount = 0;
				bobj.frames = NULL;
				bobj.rootFrames = false;
				BoneObjs[frame->getBone()->getIndex()] = bobj;
			}

			//scene->addSceneObject(obj);
		}

		frameRecurse(cframe, cmm);
	}
}


void debugFrame(DFFFrame* frame, int depth)
{
	for (int i = 0 ; i < depth ; i++)
		printf("  ");

	printf("%s", frame->getName() ? frame->getName() : "[Unnamed]");

	if (frame->getBone()) {
		DFFBone* bone = frame->getBone();
		printf(" [Bone Index %d]", bone->getIndex());
	} else {
		printf(" [Boneless]");
	}

	printf("\n");

	for (DFFFrame::ChildIterator it = frame->getChildBegin() ; it != frame->getChildEnd() ; it++) {
		DFFFrame* child = *it;
		debugFrame(child, depth+1);
	}
}



Controller::Controller(QWidget* mainWidget)
		: mainWidget(mainWidget), lastFrameStart(0), lastMeasuredFrameStart(0), moveFactor(1.0f),
		  moveForwardFactor(0.0f), moveSidewardFactor(0.0f), moveUpFactor(0.0f), firstFrame(true),
		  framesSinceLastMeasure(0), lastMouseX(-1), lastMouseY(-1), printCacheStatistics(false),
		  programRunning(true), forceStatisticsUpdate(false)
{
}



AnimatedBone* animBone;
float animDuration;
float relTime;
map<AnimatedBone*, uint> BoneRowMap;
IFPObject* selObj = NULL;
IFPFrame* prevF1 = NULL;
IFPFrame* prevF2 = NULL;
bool autoPlayEnabled = false;





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

	GLException::checkError();

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ZERO, GL_ONE);

	GLException::checkError();

	engine->getMeshCache()->resize(40 * 1000000); // 40MB
	engine->getTextureCache()->resize(150 * 1000000); // 150MB
	engine->getCollisionMeshCache()->resize(30 * 1000000);
	engine->getAnimationCache()->resize(100 * 1000000);


	printf("Loading resources...\n");
	//addResource(File(GTASA_PATH "/anim/anim.img"));
	addResource(File(GTASA_PATH "/models/gta3.img"));
	addResource(File(GTASA_PATH "/models/gta_int.img"));
	//engine->addResource(File(GTASA_PATH ""));
	//engine->addResource(File(GTASA_PATH "/models/generic/vehicle.txd"));

	GLException::checkError();


	printf("Loading DAT files...\n");
	engine->loadDAT(File(GTASA_PATH "/data/default.dat"), File(GTASA_PATH));
	engine->loadDAT(File(GTASA_PATH "/data/gta.dat"), File(GTASA_PATH));
	//engine->loadDAT(File(GTASA_PATH "/data/gta_vc.dat"), File(GTASA_PATH));
	//engine->loadDAT(File(GTASA_PATH "/data/gta3.dat"), File(GTASA_PATH));
	//engine->loadDAT(File(GTASA_PATH "/data/test.dat"), File(GTASA_PATH));


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


#define MESH_NAME "nt_noddonkbase"
#define TEX_NAME "des_xoilfield"
#define ANPK_NAME "counxref"
#define ANIM_NAME "nt_noddonkbase"

#define MESH_NAME2 "des_ufosign"
#define TEX_NAME2 "des_ufoinn"
#define ANPK_NAME2 "countn2"
#define ANIM_NAME2 "des_ufosign"


	/*ManagedMeshPointer* meshPtr = new ManagedMeshPointer(LowerHash(MESH_NAME));
	ManagedTextureSource* texSrc = new ManagedTextureSource(TEX_NAME);
	ManagedAnimationPackagePointer* animPtr = new ManagedAnimationPackagePointer(LowerHash(ANPK_NAME));

	AnimatedMapItemDefinition* def = new AnimatedMapItemDefinition(meshPtr, texSrc, NULL, animPtr, 500.0f, 0);

	AnimatedSceneObject* obj = new AnimatedSceneObject(def);
	obj->setCurrentAnimation(LowerHash(ANIM_NAME));
	obj->setModelMatrix(Matrix4::rotationZ(PI/2.0f) * Matrix4::translation(0.0f, 0.0f, 10.0f));

	scene->addSceneObject(obj);*/


	/*ManagedMeshPointer* meshPtr2 = new ManagedMeshPointer(LowerHash(MESH_NAME2));
	ManagedTextureSource* texSrc2 = new ManagedTextureSource(TEX_NAME2);
	ManagedAnimationPackagePointer* animPtr2 = new ManagedAnimationPackagePointer(LowerHash(ANPK_NAME2));

	AnimatedMapItemDefinition* def2 = new AnimatedMapItemDefinition(meshPtr2, texSrc2, NULL, animPtr2, 500.0f, 0);

	AnimatedSceneObject* obj2 = new AnimatedSceneObject(def2);
	obj2->setCurrentAnimation(LowerHash(ANIM_NAME2));
	obj2->setModelMatrix(Matrix4::translation(20.0f, 0.0f, 10.0f));

	scene->addSceneObject(obj2);*/


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

	//cam->setPosition(-1013.983215f, -869.982971f, 14.407437f);
	//cam->setPosition(-97.538010f, -442.834442f, 0.799672f);
	cam->setPosition(0.0f, 0.0f, 0.0f);
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
	//renderer->setTransparencyMode(DefaultRenderer::DepthPeeling);
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


	/*map<int32_t, BoneObject>::iterator it;

	uint64_t animTime;
	if (animStart == 0) {
		animStart = GetTickcount();
		animTime = animStart;
	} else {
		animTime = GetTickcount();
	}

	int64_t frameRelTimeInt = animTime-animStart;
	float frameRelTime = (float) (frameRelTimeInt % (int64_t) ((float) animDuration * 1000.0f)) / 1000.0f;

	if (autoPlayEnabled) {
		setAnimationTime(frameRelTime);
	}

	animBone->render(relTime);*/

	engine->render();

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glUseProgram(0);

	//renderer->drawDebug();


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

		/*if (algo == basicTransAlgo) {
			strcpy(transMode, "Basic");
		} else if (algo == wavgAlgo) {
			strcpy(transMode, "Weighted Average");
		} else {
			sprintf(transMode, "Depth Peeling (%d passes/frame)", dpAlgo->getPassCount());
		}*/

		sprintf(transMode, "Depth Peeling (%d passes/frame)", dpAlgo->getPassCount());

		/*switch (renderer->getTransparencyMode()) {
		case DefaultRenderer::WeightedAverage:
			strcpy(transMode, "Weighted Average");
			break;
		case DefaultRenderer::DepthPeeling:
			sprintf(transMode, "Depth Peeling (%d passes/frame)", renderer->getTransparencyPassCount());
			break;
		}*/
		//strcpy(transMode, "[Disabled]");

		char title[128];
		sprintf(title, WINDOW_BASE_TITLE " - %.2f FPS - MC: %.2f%% - TC: %.2f%% - %.2d:%.2d - Transparency Mode: %s",
				fps, mc*100.0f, tc*100.0f, gameH, gameM, transMode);
		mainWidget->setWindowTitle(title);

		lastMeasuredFrameStart = time;
		framesSinceLastMeasure = 0;

		forceStatisticsUpdate = false;
	}

	absE = GetTickcount();

	//printf("Absolute time: %dms\n", (int) (absE-absS));

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


void Controller::keyPressed(QKeyEvent* evt)
{
	Engine* engine = Engine::getInstance();

	int k = evt->key();

	if (k == Qt::Key_Plus) {
		moveFactor *= 2.0f;
	} else if (k == Qt::Key_Minus) {
		moveFactor /= 2.0f;
	} else if (k == Qt::Key_W) {
		moveForwardFactor = 1.0f;
	} else if (k == Qt::Key_S) {
		moveForwardFactor = -1.0f;
	} else if (k == Qt::Key_A) {
		moveSidewardFactor = 1.0f;
	} else if (k == Qt::Key_D) {
		moveSidewardFactor = -1.0f;
	} else if (k == Qt::Key_Q) {
		moveUpFactor = 1.0f;
	} else if (k == Qt::Key_Y) {
		moveUpFactor = -1.0f;
	} else if (k == Qt::Key_C) {
		engine->switchDrawing();
	} else if (k == Qt::Key_P) {
		printf("Camera position: (%f, %f, %f)\n", engine->getCamera()->getPosition().getX(),
				engine->getCamera()->getPosition().getY(), engine->getCamera()->getPosition().getZ());
	} else if (k == Qt::Key_X) {
		programRunning = false;
	} else if (k == Qt::Key_L) {
		engine->advanceGameTime(0, 30);
	}
}


void Controller::keyReleased(QKeyEvent* evt)
{
	int k = evt->key();

	switch (k) {
	case Qt::Key_W:
	case Qt::Key_S:
		moveForwardFactor = 0.0f;
		break;
		break;
	case Qt::Key_A:
	case Qt::Key_D:
		moveSidewardFactor = 0.0f;
		break;
	case Qt::Key_Q:
	case Qt::Key_Y:
		moveUpFactor = 0.0f;
		break;
	default:
		break;
	}
}


void Controller::mouseButtonPressed(Qt::MouseButton button, int x, int y)
{
	if (button == Qt::LeftButton) {
		lastMouseX = x;
		lastMouseY = y;
	}
}


void Controller::mouseButtonReleased(Qt::MouseButton button, int x, int y)
{
	if (button == Qt::LeftButton) {
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



