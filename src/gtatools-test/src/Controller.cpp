/*
	Copyright 2010-2013 David "Alemarius Nexus" Lerch

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
#include <gta/resource/collision/COLMeshConverter.h>
#include <gtaformats/util/util.h>
#include <gtaformats/util/strutil.h>
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
#include <gtaformats/col/COLLoader.h>
#include <gtaformats/util/Exception.h>
#include <gtaformats/util/CRC32.h>
#include <gtaformats/util/DefaultFileFinder.h>
#include <gtaformats/util/math/project.h>
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
#include <gta/scene/objects/SimpleDynamicSceneObject.h>
#include <gta/scene/objects/Vehicle.h>
#include <gta/AnimatedMapItemDefinition.h>
#include <gta/resource/animation/AnimationCacheEntry.h>
#include <gta/resource/smesh/ShadowMesh.h>
#include <gta/resource/smesh/ShadowVolumeGenerator.h>
#include <gta/resource/smesh/ManagedShadowMeshPointer.h>
#include <gta/scene/raycasting/RayCaster.h>
#include <gta/scene/raycasting/CollisionRayCastingHandler.h>





#define STATISTICS_UPDATE_INTERVAL 500




Controller::Controller()
		: lastFrameStart(0), lastMeasuredFrameStart(0), moveFactor(1.0f),
		  moveForwardFactor(0.0f), moveSidewardFactor(0.0f), moveUpFactor(0.0f), firstFrame(true),
		  framesSinceLastMeasure(0), lastMouseX(-1), lastMouseY(-1), printCacheStatistics(false),
		  programRunning(true), forceStatisticsUpdate(false), freeRunning(true), increaseTime(false),
		  increaseTimeHold(false),
		  lastSelectedObj(NULL)
{
}



class TestViewpoint : public StreamingViewpoint
{
public:
	TestViewpoint(Vector3 pos, float sdMul) : pos(pos), sdMul(sdMul) {}
	virtual Vector3 getStreamingViewpointPosition() const { return pos; }
	virtual uint32_t getStreamingFlags() const { return FrustumCulling; }
	virtual uint32_t getStreamingBuckets() const { return StreamingManager::VisibleBucket; }
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

	GameInfo gameInfo(ver, File(GTASA_PATH));
	engine->setDefaultGameInfo(gameInfo);

	printf("Initializing Bullet...\n");



	//world->setGravity(btVector3(0.0f, 0.0f, -9.81f));

#ifndef GT_USE_OPENGL_ES
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	glEnable(GL_LINE_SMOOTH);
#endif
	glLineWidth(0.5f);

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

	//scene->setPhysicsWorld(world);

	Camera* cam = new Camera;
	engine->setCamera(cam);

	scene->addStreamingViewpoint(cam);

	//TestViewpoint* vp1 = new TestViewpoint(Vector3(500.0f, 0.0f, 0.0f), 1.0f);
	//scene->addStreamingViewpoint(vp1);

	engine->setScene(scene);

	if (gameInfo.getVersionMode() == GameInfo::GTASA) {
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
	engine->getMeshCache()->resize(100 * 1000000); // 40MB
	engine->getTextureCache()->resize(150 * 1000000); // 150MB
	engine->getCollisionMeshCache()->resize(30 * 1000000);
	engine->getAnimationCache()->resize(100 * 1000000);


	printf("Loading resources...\n");

	if (gameInfo.getVersionMode() == GameInfo::GTASA) {
		addResource(File(GTASA_PATH "/models/gta_int.img"));
	}

	//addResource(File(GTASA_PATH "/anim/anim.img"));
	addResource(File(GTASA_PATH "/models/gta3.img"));

	addResource(File(GTASA_PATH "/anim/ped.ifp"));
	//engine->addResource(File(GTASA_PATH ""));
	engine->addResource(File(GTASA_PATH "/models/generic/vehicle.txd"));

	printf("Loading DAT files...\n");
	engine->loadDAT(File(GTASA_PATH "/data/default.dat"), File(GTASA_PATH));

	switch (gameInfo.getVersionMode()) {
	case GameInfo::GTASA:
		engine->loadDAT(File(GTASA_PATH "/data/gta.dat"), File(GTASA_PATH));
		//engine->loadDAT(File(GTASA_PATH "/data/test.dat"), File(GTASA_PATH));
		break;

	case GameInfo::GTAVC:
		engine->loadDAT(File(GTASA_PATH "/data/gta_vc.dat"), File(GTASA_PATH));;
		break;

	case GameInfo::GTAIII:
		engine->loadDAT(File(GTASA_PATH "/data/gta3.dat"), File(GTASA_PATH));
		//engine->loadDAT(File(GTASA_PATH "/data/test.dat"), File(GTASA_PATH));
		break;
	}


	selPlugin = new TestShaderPlugin(0);

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

	switch (gameInfo.getVersionMode()) {
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

			AnimatedMapItemDefinition* def = new AnimatedMapItemDefinition(meshPtr, texSrc, NULL, NULL, NULL, animPtr, 100.0f, 0);

			AnimatedMapSceneObject* obj = new AnimatedMapSceneObject;

			MapSceneObjectLODInstance* lodInst = new MapSceneObjectLODInstance(def);
			obj->addLODInstance(lodInst);

			obj->setCurrentAnimation(AnimationData[i*4 + 3]);
			obj->setModelMatrix(Matrix4::translation(2.0f * i, -2.0f * (j+1), 10.0f));

			scene->addSceneObject(obj);
		}
	}


	TextureIndexer* txdIdx = TextureIndexer::getInstance();

	TextureArchive* infernusArchive = txdIdx->findArchive("infernus");
	TextureArchive* vehicleArchive = txdIdx->findArchive("vehicle");

	printf("VA: %p\n", vehicleArchive);

	infernusArchive->setParent(vehicleArchive);



	/*float testRadius = 5.0f;

	MeshGenerator mg;

	Mesh* mesh = mg.createSphere(testRadius, 10, 10);

	for (Mesh::SubmeshIterator it = mesh->getSubmeshBegin() ; it != mesh->getSubmeshEnd() ; it++) {
		Submesh* submesh = *it;
		Material* mat = new Material;
		mat->setColor(255, 0, 0, 50);
		mesh->addMaterial(mat);
		submesh->setMaterial(mat);
	}

	MeshClump* clump = new MeshClump;

	MeshFrame* rootFrame = new MeshFrame;
	clump->setRootFrame(rootFrame);

	clump->addMesh(mesh);

	StaticMeshPointer* meshPtr = new StaticMeshPointer(clump);

	btSphereShape* shape = new btSphereShape(testRadius);
	StaticPhysicsPointer* physPtr = new StaticPhysicsPointer(shape);

	SimpleDynamicSceneObject* sdObj = new SimpleDynamicSceneObject(meshPtr, NULL, NULL, NULL, physPtr, 250.0f, 20.0f, Matrix4::translation(100.0f, -100.0f, 100.0f));
	btRigidBody* rb = sdObj->getRigidBody();

	rb->setRestitution(1.0f);
	rb->forceActivationState(DISABLE_DEACTIVATION);
	rb->setLinearVelocity(btVector3(20.0f, 0.0f, 0.0f));
	//rb->applyForce(btVector3(1.0f, 0.0f, 0.0f), btVector3(0.0f, 0.0f, 0.0f));

	sdObj->setBoundingSphere(Vector3::Zero, testRadius);

	scene->addSceneObject(sdObj);*/

	/*veh = new Vehicle("infernus", Matrix4::translation(0.0f, 0.0f, 500.0f) * Matrix4::rotationY(0.5f));

	veh->setBoundingSphere(Vector3::Zero, 25.0f);

	scene->addSceneObject(veh);*/

	/*MeshGenerator mg;

	float halfLenX = 1.0f;
	float halfLenY = 1.0f;
	float halfLenZ = 1.0f;

	Vector3 boxMin(-halfLenX, -halfLenY, -halfLenZ);
	Vector3 boxMax(halfLenX, halfLenY, halfLenZ);

	//Mesh* mesh = mg.createBox(boxMin, boxMax);
	Mesh* mesh = mg.createSphere(5.0f, 100, 100);
	mesh->setBounds(0.0f, 0.0f, 0.0f, 5.0f);

	MeshClump* clump = new MeshClump;
	MeshFrame* frame = new MeshFrame;
	frame->setModelMatrix(Matrix4::translation(0.0f, 0.0f, 0.0f));
	clump->setRootFrame(frame);
	mesh->setFrame(frame);
	clump->addMesh(mesh);

	for (Mesh::SubmeshIterator it = mesh->getSubmeshBegin() ; it != mesh->getSubmeshEnd() ; it++) {
		Submesh* submesh = *it;

		Material* mat = new Material;
		mat->setColor(0, 0, 255, 255);

		submesh->setMaterial(mat);
	}

	StaticMeshPointer* meshPtr = new StaticMeshPointer(clump);

	CollisionModel* cmodel = new CollisionModel;
	cmodel->setBoundingSphere(Vector3::Zero, 5.0f);
	cmodel->setBoundingBox(Vector3(-5.0f, -5.0f, -5.0f), Vector3(5.0f, 5.0f, 5.0f));

	CollisionSphere* sphere = new CollisionSphere(Vector3::Zero, 5.0f);
	cmodel->addSphere(sphere);

	StaticCollisionShapePointer* colPtr = new StaticCollisionShapePointer(cmodel);

	StaticMapItemDefinition* def = new StaticMapItemDefinition(meshPtr, NULL, colPtr, NULL, 250.0f);

	MapSceneObject* obj = new MapSceneObject;

	MapSceneObjectLODInstance* lodInst = new MapSceneObjectLODInstance(def);
	obj->addLODInstance(lodInst);

	float angle = 45.0f;
	obj->setModelMatrix(Matrix4::translation(Vector3(0.0f, 0.0f, 50.0f)) * Matrix4::rotation((angle / 180.0f) * M_PI, Vector3::UnitY));

	//scene->addSceneObject(obj);




	Vector3 rayStart(0.3f, -2.0f, 30.0f);
	//Vector3 rayStart(2392.345215f, -1.829378f, 30);
	//Vector3 rayStart(2414.333252f, -1.863295f, 27.659145f);
	Vector3 rayDir(1.0f, 0.0f, 0.0f);

	float rayLen = 10000.0f;



	rayDir.normalize();


	RayCaster rc;

	Scene::ObjectList olist;
	olist.push_back(obj);

	CollisionRayCastingHandler<Scene::ObjectIterator> rch;
	rch.setSceneObjects(scene->getSceneObjectBegin(), scene->getSceneObjectEnd());
	//rch.setSceneObjects(olist.begin(), olist.end());

	rc.setHandler(&rch);

	uint64_t s, e;

	s = GetTickcount();
	rc.castRay(rayStart, rayDir, RayCaster::Sorted | RayCaster::CalculateIntersectionPosition);
	e = GetTickcount();
	uint64_t t1 = e-s;
	s = GetTickcount();
	rc.castRay(rayStart, rayDir, RayCaster::Sorted | RayCaster::CalculateIntersectionPosition);
	e = GetTickcount();
	uint64_t t2 = e-s;

	printf("Raycasting took %ums (first: %ums)\n", t2, t1);

	uint32_t numRes = 0;
	for (CollisionRayCastingHandler<Scene::ObjectIterator>::ResultIterator it = rch.getResultBegin() ; it != rch.getResultEnd() ; it++) {
		CollisionRayCastingHandler<Scene::ObjectIterator>::ObjectResult& res = *it;

		unsigned int numR;
		float r[2];

		if ((res.flags & CollisionRayCastingHandler<Scene::ObjectIterator>::ObjectResultFlagSphere)  !=  0) {
			r[0] = res.sphereRes.rayRa;
			r[1] = res.sphereRes.rayRb;
			numR = 2;
		} else if ((res.flags & CollisionRayCastingHandler<Scene::ObjectIterator>::ObjectResultFlagBox)  !=  0) {
			memcpy(r, res.boxRes.intersectionRayR, sizeof(r));
			numR = 2;
		} else {
			r[0] = res.meshRes.rayR;
			numR = 1;
		}

		for (unsigned int i = 0 ; i < numR ; i++) {
			Vector3 pos = rayStart + rayDir*r[i];

			MeshGenerator mg;

			Mesh* mesh = mg.createSphere(0.1f, 10, 10);

			MeshClump* clump = new MeshClump;
			MeshFrame* frame = new MeshFrame;
			frame->setModelMatrix(Matrix4::translation(0.0f, 0.0f, 0.0f));
			clump->setRootFrame(frame);
			mesh->setFrame(frame);
			clump->addMesh(mesh);

			for (Mesh::SubmeshIterator it = mesh->getSubmeshBegin() ; it != mesh->getSubmeshEnd() ; it++) {
				Submesh* submesh = *it;

				Material* mat = new Material;
				mat->setColor(0, 0, 255, 255);

				submesh->setMaterial(mat);
			}

			StaticMeshPointer* meshPtr = new StaticMeshPointer(clump);
			StaticMapItemDefinition* def = new StaticMapItemDefinition(meshPtr, NULL, NULL, NULL, 250.0f);

			MapSceneObject* obj = new MapSceneObject;

			MapSceneObjectLODInstance* lodInst = new MapSceneObjectLODInstance(def);
			obj->addLODInstance(lodInst);

			obj->setModelMatrix(Matrix4::translation(pos));

			scene->addSceneObject(obj);

			printf("* INTERSECTION at position %f, %f, %f (R%u: %f)\n", pos.getX(), pos.getY(), pos.getZ(), i, r[i]);
		}

		dynamic_cast<VisualSceneObject*>(res.obj)->selected = true;

		numRes++;
	}

	printf("%u Raycasting Results!\n", numRes);


	float* vertices = new float[2*3];
	uint32_t* indices = new uint32_t[2];

	vertices[0] = rayStart.getX();
	vertices[1] = rayStart.getY();
	vertices[2] = rayStart.getZ();

	vertices[3] = rayStart.getX() + rayDir.getX() * rayLen;
	vertices[4] = rayStart.getY() + rayDir.getY() * rayLen;
	vertices[5] = rayStart.getZ() + rayDir.getZ() * rayLen;

	indices[0] = 0;
	indices[1] = 1;

	mesh = new Mesh(2, VertexFormatLines, 0, vertices);
	Vector3 bsCenter = rayStart + rayDir*rayLen*0.5f;
	mesh->setBounds(bsCenter.getX(), bsCenter.getY(), bsCenter.getZ(), rayLen*0.5f);

	Submesh* submesh = new Submesh(mesh, 2, indices);

	Material* mat = new Material;
	mat->setColor(255, 0, 0, 255);

	submesh->setMaterial(mat);

	mesh->addMaterial(mat);
	mesh->addSubmesh(submesh);

	float sphereRadius = 0.25f;
	Mesh* sphereMesh = mg.createSphere(sphereRadius, 10, 10, rayStart - rayDir*sphereRadius);

	Material* sphereMat = new Material;
	sphereMat->setColor(255, 255, 0, 255);

	(*sphereMesh->getSubmeshBegin())->setMaterial(sphereMat);
	sphereMesh->addMaterial(sphereMat);

	MeshFrame* rootFrame = new MeshFrame;
	mesh->setFrame(rootFrame);
	sphereMesh->setFrame(rootFrame);

	clump = new MeshClump;
	clump->setRootFrame(rootFrame);
	clump->addMesh(mesh);
	clump->addMesh(sphereMesh);

	delete[] vertices;
	delete[] indices;


	meshPtr = new StaticMeshPointer(clump);

	def = new StaticMapItemDefinition(meshPtr, NULL, NULL, NULL, rayLen + 200.0f, 0);

	lodInst = new MapSceneObjectLODInstance(def);

	obj = new MapSceneObject;
	obj->addLODInstance(lodInst);

	//obj->setModelMatrix(Matrix4::translation(0.0f, 0.0f, 50.0f));

	scene->addSceneObject(obj);*/




	PVSDatabase* pvs = scene->getStreamingManager()->getPVSDatabase();

	File pvsFile(GTASA_PATH "/visibility.pvs");

	bool pvsBuilt = false;

	if (pvsFile.exists()) {
		printf("Loading PVS data from file '%s'...\n", pvsFile.getPath()->toString().get());

		PVSDatabase::LoadingResult res = pvs->load(pvsFile, engine->getDefaultGameInfo().getRootDirectory());

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
		pvs->calculateSections(500.0f, 500.0f, 2000.0f);
		pvs->calculatePVS(8);
		printf("Writing PVS data to file '%s'\n", pvsFile.getPath()->toString().get());
		//pvs->save(pvsFile);
		printf("PVS data successfully built!\n");

		pvsBuilt = true;
	}




	printf("\n\n\n\n");

	/*for (int i = 0 ; i < 0 ; i++) {
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

		scene->addSceneObject(obj);*/

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
	//}

	// <------ X+
	// V Y+
	cam->setPosition(0.0f, 0.0f, 20.0f);

	//cam->setPosition(-1955.232544f, -58.526737f, 49.788841f);
	//cam->lookAt(Vector3(0.595735f, 0.704997f, -0.384810f), Vector3(0.248370f, 0.293923f, 0.922996f));

	//cam->setPosition(-2067.237549f, -2.653352f, 115.438286f);
	//cam->lookAt(Vector3(0.860327f, -0.065335f, -0.505538f), Vector3(0.499779f, -0.037955f, 0.865321f));

	//cam->setPosition(-2076.432861f, -1.954256f, 120.847687f);
	//cam->lookAt(Vector3(0.860327f, -0.065335f, -0.505538f), Vector3(0.499779f, -0.037955f, 0.865321f));

	//cam->setPosition(2148.097168f, -1520.077026f, 58.264038f);
	//cam->lookAt(Vector3(0.065757f, -0.875112f, -0.479432f), Vector3(0.035923f, -0.478085f, 0.877579f));

	//cam->setPosition(-110.013428f, -270.041931f, 23.978954f);
	//cam->lookAt(Vector3(0.068262f, -0.908477f, -0.412323f), Vector3(0.031236f, -0.415701f, 0.908965f));

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


bool Controller::paint()
{
	uint64_t absS, absE;
	absS = GetTickcountMicroseconds();

	uint64_t time = GetTickcount();

	Engine* engine = Engine::getInstance();

	Camera* cam = engine->getCamera();

	uint64_t timePassed = 0;
	float timeFactor = 0.0f;

	if (lastFrameStart != 0) {
		timeFactor = (time - lastFrameStart)*0.025f;
		timePassed = time - lastFrameStart;
	}

#if 1
	cam->move(timeFactor*moveForwardFactor*moveFactor);
	cam->moveSideways(timeFactor*moveSidewardFactor*moveFactor);
	cam->moveUp(timeFactor*moveUpFactor*moveFactor);
#else
	Vector3 cpos(0.0f, -5.0f, 2.5f);
	cpos = veh->getModelMatrix() * cpos;

	cam->setPosition(cpos);

	Vector3 target(0.0f, 1.0f, -0.5f);
	Vector3 up(0.0f, 0.0f, 1.0f);

	cam->lookAt(target, up);
#endif

	//Matrix4 mm = veh->getModelMatrix();
	//const float* a = mm.toArray();
	//veh->setModelMatrix(Matrix4::translation(a[12], a[13]+0.25f, a[14]));

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	uint64_t slInterval = 5000;
	float slAngle = 2.0f * M_PI * ((GetTickcount() % slInterval) / (float) slInterval);

	Vector3 slPos(slBasePos);
	slPos += Vector3(sin(slAngle), cos(slAngle), 0.0f) * 50.0f;
	sl->setPosition(slPos);

	uint64_t s, e;
	s = GetTickcountMicroseconds();

	if (!firstFrame) {
		engine->renderFrame();
	}

	e = GetTickcountMicroseconds();
	uint64_t t1 = e-s;

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

	s = GetTickcountMicroseconds();

	if (freeRunning  ||  increaseTime) {
		engine->advanceFrame(timePassed);

		if (!increaseTimeHold)
			increaseTime = false;
	} else {
		engine->advanceFrame(0);
	}

	e = GetTickcountMicroseconds();

	absE = GetTickcountMicroseconds();

	//printf("Took %uus (%uus / %uus)\n", (unsigned int) (absE-absS), (unsigned int) t1, (unsigned int) (e-s));

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

		printf("cam->setPosition(%ff, %ff, %ff);\n", p.getX(), p.getY(), p.getZ());
		printf("cam->lookAt(Vector3(%ff, %ff, %ff), Vector3(%ff, %ff, %ff));\n",
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
	case SDLK_0:
		increaseTimeHold = false;
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


void Controller::mouseButtonDoubleClicked(int x, int y)
{
	Engine* engine = Engine::getInstance();
	Camera* cam = engine->getCamera();
	Matrix4 pMatrix = engine->getProjectionMatrix();
	Matrix4 vMatrix = Matrix4::lookAt(cam->getTarget(), cam->getUp())
			* Matrix4::translation(-cam->getPosition());
	Scene* scene = engine->getScene();

	int w = engine->getViewportWidth();
	int h = engine->getViewportHeight();

	Vector3 point1, point2;
	bool par = UnProject(Vector3(x, h-y, 0.0f), vMatrix, pMatrix, 0, 0, w, h, point1);
	bool pbr = UnProject(Vector3(x, h-y, 0.5f), vMatrix, pMatrix, 0, 0, w, h, point2);

	Vector3 rayStart = point1;
	Vector3 rayDir = point2-point1;

	rayDir.normalize();

	float rayLen = 1000.0f;

	RayCaster rc;

	CollisionRayCastingHandler<Scene::ObjectIterator> rch;
	rch.setSceneObjects(scene->getSceneObjectBegin(), scene->getSceneObjectEnd());

	rc.setHandler(&rch);
	rc.castRay(point1, rayDir, RayCaster::Sorted | RayCaster::CalculateIntersectionPosition, 1);

	if (lastSelectedObj) {
		//lastSelectedObj->selected = false;
		dynamic_cast<MapSceneObject*>(lastSelectedObj)->getShaderPluginRegistry().uninstallPlugin(selPlugin);
	}

	if (rch.getResultBegin() != rch.getResultEnd()) {
		CollisionRayCastingHandler<Scene::ObjectIterator>::ObjectResult& res = *rch.getResultBegin();
		VisualSceneObject* vobj = dynamic_cast<VisualSceneObject*>(res.obj);
		MapSceneObject* mobj = dynamic_cast<MapSceneObject*>(vobj);
		//vobj->selected = true;
		mobj->getShaderPluginRegistry().installPlugin(selPlugin);
		lastSelectedObj = vobj;
	} else {
		lastSelectedObj = NULL;
	}



	/*MeshGenerator mg;

	float* vertices = new float[2*3];
	uint32_t* indices = new uint32_t[2];

	vertices[0] = rayStart.getX();
	vertices[1] = rayStart.getY();
	vertices[2] = rayStart.getZ();

	vertices[3] = rayStart.getX() + rayDir.getX() * rayLen;
	vertices[4] = rayStart.getY() + rayDir.getY() * rayLen;
	vertices[5] = rayStart.getZ() + rayDir.getZ() * rayLen;

	indices[0] = 0;
	indices[1] = 1;

	Mesh* mesh = new Mesh(2, VertexFormatLines, 0, vertices);
	Vector3 bsCenter = rayStart + rayDir*rayLen*0.5f;
	mesh->setBounds(bsCenter.getX(), bsCenter.getY(), bsCenter.getZ(), rayLen*0.5f);

	Submesh* submesh = new Submesh(mesh, 2, indices);

	Material* mat = new Material;
	mat->setColor(255, 0, 0, 255);

	submesh->setMaterial(mat);

	mesh->addMaterial(mat);
	mesh->addSubmesh(submesh);

	float sphereRadius = 0.25f;
	Mesh* sphereMesh = mg.createSphere(sphereRadius, 10, 10, rayStart - rayDir*sphereRadius);

	Material* sphereMat = new Material;
	sphereMat->setColor(255, 255, 0, 255);

	(*sphereMesh->getSubmeshBegin())->setMaterial(sphereMat);
	sphereMesh->addMaterial(sphereMat);

	MeshFrame* rootFrame = new MeshFrame;
	mesh->setFrame(rootFrame);
	sphereMesh->setFrame(rootFrame);

	MeshClump* clump = new MeshClump;
	clump->setRootFrame(rootFrame);
	clump->addMesh(mesh);
	clump->addMesh(sphereMesh);

	delete[] vertices;
	delete[] indices;


	StaticMeshPointer* meshPtr = new StaticMeshPointer(clump);

	StaticMapItemDefinition* def = new StaticMapItemDefinition(meshPtr, NULL, NULL, NULL, rayLen + 200.0f, 0);

	MapSceneObjectLODInstance* lodInst = new MapSceneObjectLODInstance(def);

	MapSceneObject* obj = new MapSceneObject;
	obj->addLODInstance(lodInst);

	//obj->setModelMatrix(Matrix4::translation(0.0f, 0.0f, 50.0f));

	scene->addSceneObject(obj);*/
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



