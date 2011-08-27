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
#include <gta/DefaultRenderer.h>
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
#include <CEGUI/CEGUI.h>
#include <gta/ceguigl2/CEGUIGL2Renderer.h>

using CEGUI::UDim;
using CEGUI::UVector2;



struct SDLCEGUIKeyMapping
{
	SDLKey sk;
	uint ck;
};

struct SDLCEGUIMouseButtonMapping
{
	Uint8 sb;
	CEGUI::MouseButton cb;
};


const SDLCEGUIKeyMapping SDLCEGUIKeyMap[] = {
		{SDLK_LSHIFT, CEGUI::Key::LeftShift},
		{SDLK_RSHIFT, CEGUI::Key::RightShift},
		{SDLK_LCTRL, CEGUI::Key::LeftControl},
		{SDLK_RCTRL, CEGUI::Key::RightControl},
		{SDLK_LALT, CEGUI::Key::LeftAlt},
		{SDLK_RALT, CEGUI::Key::RightAlt},
		{SDLK_CAPSLOCK, CEGUI::Key::Capital},
		//{SDLK_MODE, CEGUI::Key::},		???
		{SDLK_MENU, CEGUI::Key::AppMenu},
		{SDLK_RETURN, CEGUI::Key::Return},
		{SDLK_BACKSPACE, CEGUI::Key::Backspace},
		{SDLK_DELETE, CEGUI::Key::Delete},
		{SDLK_INSERT, CEGUI::Key::Insert},
		{SDLK_HOME, CEGUI::Key::Home},
		{SDLK_END, CEGUI::Key::End},
		{SDLK_PAGEUP, CEGUI::Key::PageUp},
		{SDLK_PAGEDOWN, CEGUI::Key::PageDown},
		{SDLK_LEFT, CEGUI::Key::ArrowLeft},
		{SDLK_RIGHT, CEGUI::Key::ArrowRight},
		{SDLK_UP, CEGUI::Key::ArrowUp},
		{SDLK_DOWN, CEGUI::Key::ArrowDown},
		{SDLK_TAB, CEGUI::Key::Tab},
		{SDLK_SPACE, CEGUI::Key::Space}
};

const SDLCEGUIMouseButtonMapping SDLCEGUIMouseButtonMap[] = {
		{SDL_BUTTON_LEFT, CEGUI::LeftButton},
		{SDL_BUTTON_RIGHT, CEGUI::RightButton},
		{SDL_BUTTON_MIDDLE, CEGUI::MiddleButton}
};







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

	StaticMeshPointer* meshPtr = new StaticMeshPointer(mesh);
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

			scene->addSceneObject(obj);
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

	gtaglInit();


	printf("Initializing CEGUI...\n");

	//CEGUI::OpenGLRenderer& ceguiRenderer = CEGUI::OpenGLRenderer::bootstrapSystem();
	CEGUIGL2Renderer& ceguiRenderer = CEGUIGL2Renderer::bootstrapSystem();

	printf("CEGUI Renderer: %s\n", ceguiRenderer.getIdentifierString().c_str());

	CEGUI::DefaultResourceProvider* resProv = (CEGUI::DefaultResourceProvider*)
			CEGUI::System::getSingleton().getResourceProvider();

	resProv->setResourceGroupDirectory("schemes", "./CEGUI/schemes");
	resProv->setResourceGroupDirectory("imagesets", "./CEGUI/imagesets");
	resProv->setResourceGroupDirectory("fonts", "./CEGUI/fonts");
	resProv->setResourceGroupDirectory("layouts", "./CEGUI/layouts");
	resProv->setResourceGroupDirectory("looknfeels", "./CEGUI/looknfeel");
	resProv->setResourceGroupDirectory("lua_scripts", "./CEGUI/lua_scripts");

	CEGUI::Scheme::setDefaultResourceGroup("schemes");
	CEGUI::Imageset::setDefaultResourceGroup("imagesets");
	CEGUI::Font::setDefaultResourceGroup("fonts");
	CEGUI::WindowManager::setDefaultResourceGroup("layouts");
	CEGUI::WidgetLookManager::setDefaultResourceGroup("looknfeels");
	CEGUI::ScriptModule::setDefaultResourceGroup("lua_scripts");

	CEGUI::SchemeManager::getSingleton().create("TaharezLook.scheme");

	CEGUI::System::getSingleton().setDefaultMouseCursor("TaharezLook", "MouseArrow");

	lastCEGUITime = GetTickcount();


	CEGUI::WindowManager& wmgr = CEGUI::WindowManager::getSingleton();

	CEGUI::Window* rootWin = wmgr.loadWindowLayout("Demo8.layout");
	rootWin->setMousePassThroughEnabled(true);
	CEGUI::System::getSingleton().setGUISheet(rootWin);

	CEGUI::Window* win = wmgr.getWindow("Demo8");
	win->setMousePassThroughEnabled(true);

	/*CEGUI::Window* rootWin = wmgr.createWindow("DefaultWindow", "root");
	rootWin->setMousePassThroughEnabled(true);
	CEGUI::System::getSingleton().setGUISheet(rootWin);


	CEGUI::FrameWindow* navWindow = (CEGUI::FrameWindow*)
			wmgr.createWindow("TaharezLook/FrameWindow", "navWindow");
	navWindow->setText("Hallo Welt!");
	navWindow->setPosition(UVector2(UDim(0.59f, 0), UDim(0.01f, 0)));
	navWindow->setSize(UVector2(UDim(0.4f, 0), UDim(0.3f, 0)));
	rootWin->addChildWindow(navWindow);*/

	/*CEGUI::Window* moveText = wmgr.createWindow("TaharezLook/StaticText");
	moveText->setText("Move Factor");
	moveText->setPosition(UVector2(UDim(0.0f, 0), UDim(0.0f, 0)));
	moveText->setSize(UVector2(UDim(0.5f, 0), UDim(0, 50)));
	moveText->setProperty("FrameEnabled", "False");
	moveText->setProperty("BackgroundEnabled", "False");
	navWindow->addChildWindow(moveText);*/

	//CEGUI::Spinner* moveSpinner = (CEGUI::Spinner*) wmgr.createWindow("TaharezLook/Spinner");
	//navWindowLayout->addChildWindow(moveSpinner);



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


	printf("Loading resources...\n");
	//addResource(File(GTASA_PATH "/models/gta3.img"));
	//addResource(File(GTASA_PATH "/models/gta_int.img"));
	//engine->addResource(File(GTASA_PATH ""));
	//engine->addResource(File(GTASA_PATH "/models/generic/vehicle.txd"));

	GLException::checkError();

	//debugDrawer = new BulletGLDebugDraw;
	//world->setDebugDrawer(debugDrawer);
	//debugDrawer->setDebugMode(BulletGLDebugDraw::DBG_DrawWireframe);


	/*DFFFrame* rootFrame = new DFFFrame(new Vector3(), new Matrix3);

	DFFFrame* childFrame = new DFFFrame(new Vector3(0.0f, 0.0f, 0.25f), new Matrix3);
	rootFrame->addChild(childFrame);

	DFFBone* bone = new DFFBone;
	bone->setIndex(0);

	DFFBone* cbone = new DFFBone;
	cbone->setIndex(1);

	rootFrame->setBone(bone);
	childFrame->setBone(cbone);

	IFPAnimation* anim = new IFPAnimation;

	IFPObject* obj = new IFPObject;
	obj->setFrameType(IFPObject::RootFrame);
	obj->setBoneID(0);

	IFPRootFrame* aframe1 = new IFPRootFrame(Vector3(0.0f, 0.0f, 0.0f), Quaternion(1.0f, 0.0f, 0.0f, 0.0f), 5.0f);
	IFPRootFrame* aframe2 = new IFPRootFrame(Vector3(1.0f, 0.0f, 0.0f),
			Quaternion::fromAxisAngle(Vector3(0.0f, -1.0f, 0.0f), M_PI_2), 5.0f);

	obj->addFrame(aframe1);
	obj->addFrame(aframe2);

	anim->addObject(obj);

	IFPObject* cobj = new IFPObject;
	cobj->setFrameType(IFPObject::RootFrame);
	cobj->setBoneID(1);

	IFPRootFrame* caframe1 = new IFPRootFrame(Vector3(0.0f, 0.0f, 0.0f), Quaternion(1.0f, 0.0f, 0.0f, 0.0f), 5.0f);
	IFPRootFrame* caframe2 = new IFPRootFrame(Vector3(0.0f, 0.0f, 1.0f), Quaternion(1.0f, 0.0f, 0.0f, 0.0f), 5.0f);

	cobj->addFrame(caframe1);
	cobj->addFrame(caframe2);

	anim->addObject(cobj);

	animBone = new AnimatedBone(rootFrame, anim);

	cam->setPosition(0.0f, 15.0f, 2.5f);*/


	/*DFFLoader dff;
	//DFFMesh* mesh = dff.loadMesh(File("/home/alemariusnexus/anim/des_ufosign.dff"));
	DFFMesh* mesh = dff.loadMesh(File("/home/alemariusnexus/anim/nt_noddonkbase.dff"));
	//DFFMesh* mesh = dff.loadMesh(File("/home/alemariusnexus/anim/vrockpole.dff"));

	debugFrame(mesh->getRootFrame(), 0);

	hash_t meshHash = Hash("player");

	//frameRecurse(mesh->getRootFrame(), Matrix4::rotationX(M_PI_2));

	//IFPLoader ifp(File("/home/alemariusnexus/anim/countn2.ifp"));
	IFPLoader ifp(File("/home/alemariusnexus/anim/counxref.ifp"));
	//IFPLoader ifp(File("/home/alemariusnexus/anim/vegasn.ifp"));

	IFPAnimation* anim;

	while ((anim = ifp.readAnimation())  !=  NULL) {
		//if (LowerHash(anim->getName()) == LowerHash("des_ufosign")) {
		if (LowerHash(anim->getName()) == LowerHash("nt_noddonkbase")) {
		//if (LowerHash(anim->getName()) == LowerHash("vrockpole")) {
			//IFPAnimation::ObjectIterator it;

			int32_t i = 0;
			for (IFPAnimation::ObjectIterator it = anim->getObjectBegin() ; it != anim->getObjectEnd() ; it++) {
				IFPObject* obj = *it;

				float ad = 0.0f;

				for (IFPObject::FrameIterator fit = obj->getFrameBegin() ; fit != obj->getFrameEnd() ; fit++) {
					IFPFrame* frame = *fit;
					ad += frame->getTime();
				}

				animDuration = std::max(animDuration, ad);
			}

			animBone = new AnimatedBone(mesh->getRootFrame(), anim);
			break;
		}

		delete anim;
	}*/

	//exit(0);


	printf("Loading DAT files...\n");
	//engine->loadDAT(File(GTASA_PATH "/data/default.dat"), File(GTASA_PATH));
	//engine->loadDAT(File(GTASA_PATH "/data/gta.dat"), File(GTASA_PATH));
	//engine->loadDAT(File(GTASA_PATH "/data/gta_vc.dat"), File(GTASA_PATH));
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

	if (!engine->getRenderer()) {
		//DefaultRenderer* renderer = new DefaultRenderer;

		//renderer->setTransparencyMode(DefaultRenderer::DepthPeeling);

		//engine->setRenderer(renderer);
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

	int64_t relTimeInt = animTime-animStart;
	float relTime = (float) (relTimeInt % (int64_t) ((float) animDuration * 1000.0f)) / 1000.0f;*/

	//printf("%f/%f\n", relTime, animDuration);

	//animBone->render(0.0f, Matrix4::rotationX(M_PI_2));
	//animBone->render((float) (animTime-animStart) / 1000.0f, Matrix4::rotationX(M_PI_2));
	//animBone->render(relTime);

	//engine->render();

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glUseProgram(0);

	uint64_t ceguiTime = GetTickcount();
	CEGUI::System::getSingleton().injectTimePulse((ceguiTime-lastCEGUITime) / 1000.0f);
	lastCEGUITime = ceguiTime;

	//renderer->drawDebug();

	//glFrameTerminatorGREMEDY();

	CEGUI::System::getSingleton().renderGUI();

	//glFrameTerminatorGREMEDY();


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

		/*switch (renderer->getTransparencyMode()) {
		case DefaultRenderer::WeightedAverage:
			strcpy(transMode, "Weighted Average");
			break;
		case DefaultRenderer::DepthPeeling:
			sprintf(transMode, "Depth Peeling (%d passes/frame)", renderer->getTransparencyPassCount());
			break;
		}*/
		strcpy(transMode, "[Disabled]");

		char title[128];
		sprintf(title, WINDOW_BASE_TITLE " - %.2f FPS - MC: %.2f%% - TC: %.2f%% - %.2d:%.2d - Transparency Mode: %s",
				fps, mc*100.0f, tc*100.0f, gameH, gameM, transMode);
		SDL_WM_SetCaption(title, NULL);

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


void Controller::keyPressed(SDL_keysym key)
{
	CEGUI::System& ceguiSys = CEGUI::System::getSingleton();

	if (key.unicode != 0) {
		if (ceguiSys.injectChar(key.unicode)) {
			return;
		}
	}

	Engine* engine = Engine::getInstance();

	SDLKey k = key.sym;

	bool consumed = false;

	unsigned int keyMapSize = sizeof(SDLCEGUIKeyMap) / sizeof(SDLCEGUIKeyMap[0]);
	for (unsigned int i = 0 ; i < keyMapSize ; i++) {
		if (SDLCEGUIKeyMap[i].sk == k) {
			consumed = ceguiSys.injectKeyDown(SDLCEGUIKeyMap[i].ck);
			break;
		}
	}

	if (consumed)
		return;

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
	CEGUI::System& ceguiSys = CEGUI::System::getSingleton();
	SDLKey k = key.sym;

	bool consumed = false;

	unsigned int keyMapSize = sizeof(SDLCEGUIKeyMap) / sizeof(SDLCEGUIKeyMap[0]);
	for (unsigned int i = 0 ; i < keyMapSize ; i++) {
		if (SDLCEGUIKeyMap[i].sk == k) {
			consumed = ceguiSys.injectKeyUp(SDLCEGUIKeyMap[i].ck);
			break;
		}
	}

	if (consumed)
		return;

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


void Controller::mouseButtonPressed(Uint8 button, Uint16 x, Uint16 y)
{
	CEGUI::System& ceguiSys = CEGUI::System::getSingleton();

	bool consumed = false;

	unsigned int buttonMapSize = sizeof(SDLCEGUIMouseButtonMap) / sizeof(SDLCEGUIMouseButtonMap[0]);
	for (unsigned int i = 0 ; i < buttonMapSize ; i++) {
		if (SDLCEGUIMouseButtonMap[i].sb == button) {
			consumed = ceguiSys.injectMouseButtonDown(SDLCEGUIMouseButtonMap[i].cb);
			break;
		}
	}

	if (consumed)
		return;

	CEGUI::WindowManager::getSingleton().getWindow("root")->deactivate();

	if (button == SDL_BUTTON_LEFT) {
		lastMouseX = x;
		lastMouseY = y;
	}
}


void Controller::mouseButtonReleased(Uint8 button, Uint16 x, Uint16 y)
{
	CEGUI::System& ceguiSys = CEGUI::System::getSingleton();

	bool consumed = false;

	unsigned int buttonMapSize = sizeof(SDLCEGUIMouseButtonMap) / sizeof(SDLCEGUIMouseButtonMap[0]);
	for (unsigned int i = 0 ; i < buttonMapSize ; i++) {
		if (SDLCEGUIMouseButtonMap[i].sb == button) {
			consumed = ceguiSys.injectMouseButtonUp(SDLCEGUIMouseButtonMap[i].cb);
			break;
		}
	}

	if (consumed)
		return;

	if (button == SDL_BUTTON_LEFT) {
		lastMouseX = -1;
		lastMouseY = -1;
	}
}


void Controller::mouseMotion(Uint16 x, Uint16 y)
{
	CEGUI::System& ceguiSys = CEGUI::System::getSingleton();

	ceguiSys.injectMousePosition(x, y);

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


