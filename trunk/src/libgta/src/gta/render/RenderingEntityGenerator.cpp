/*
	Copyright 2010-2013 David "Alemarius Nexus" Lerch

	This file is part of libgta.

	libgta is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	libgta is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with libgta.  If not, see <http://www.gnu.org/licenses/>.

	Additional permissions are granted, which are listed in the file
	GPLADDITIONS.
 */

#include "RenderingEntityGenerator.h"

#include "DefaultRenderingMesh.h"

#include "../scene/objects/AnimatedMapSceneObject.h"
#include "../scene/objects/MapSceneObject.h"
#include "../scene/objects/SimpleDynamicSceneObject.h"

#include "../Animator.h"




RenderingEntityGenerator::RenderingEntityGenerator()
{
}


void RenderingEntityGenerator::generate(list<VisualSceneObject*>::iterator beg, list<VisualSceneObject*>::iterator end, list<RenderingEntity*>& outList)
{
	for (list<VisualSceneObject*>::iterator it = beg ; it != end ; it++) {
		VisualSceneObject* sobj = *it;

		SceneObject::typeflags_t tf = sobj->getTypeFlags();

		AnimatedMapSceneObject* aobj = dynamic_cast<AnimatedMapSceneObject*>(sobj);

		if (aobj) {
			generateFromAnimatedMapSceneObject(aobj, outList);
			continue;
		}

		MapSceneObject* mobj = dynamic_cast<MapSceneObject*>(sobj);

		if (mobj) {
			generateFromMapSceneObject(mobj, outList);
			continue;
		}

		SimpleDynamicSceneObject* sdobj = dynamic_cast<SimpleDynamicSceneObject*>(sobj);

		if (sdobj) {
			generateFromSimpleDynamicSceneObject(sdobj, outList);
			continue;
		}

		Vehicle* veh = dynamic_cast<Vehicle*>(sobj);

		if (veh) {
			generateFromVehicle(veh, outList);
			continue;
		}
	}
}


RenderingSubmesh* RenderingEntityGenerator::generateFromSubmesh(Submesh* submesh, TextureSource* texSrc, RenderingMesh* rmesh, bool debug)
{
	Material* mat = submesh->getMaterial();

	GLuint oglTex = 0;

	uint8_t r = 255;
	uint8_t g = 255;
	uint8_t b = 255;
	uint8_t a = 255;

	bool texLocked = false;

	if (mat) {
		if (mat->isTextured()) {
			Texture* tex = texSrc->getTexture(mat->getTexture(), true);
			texLocked = true;
			if (tex)
				oglTex = tex->getGLTexture();
		}

		mat->getColor(r, g, b, a);
	}

	RenderingSubmesh* sm = new RenderingSubmesh(rmesh, submesh->getIndexCount(), submesh->getIndexOffset(), oglTex);

	sm->setMaterialColor(r, g, b, a);

	if (texSrc) {
		sm->texSrc = texSrc->clone();
		sm->texSrc->lock();
	}

	if (texLocked)
		texSrc->release();

	return sm;
}


RenderingMesh* RenderingEntityGenerator::generateFromMesh(Mesh* mesh, MeshPointer* mptr, TextureSource* texSrc,
		const Matrix4& modelMat, bool transparent, MeshFrame* frame)
{
	Matrix4 fModelMat = modelMat;

	bool isChassis = false;

	if (frame) {
		fModelMat = fModelMat * frame->getAbsoluteModelMatrix();

		if (frame->getName().lower() == "chassis") {
			isChassis = true;
		}
	}

	RenderingPrimitiveFormat rpf;

	switch (mesh->getVertexFormat()) {
	case VertexFormatPoints:
		rpf = RenderingPrimitivePoints;
		break;
	case VertexFormatLines:
		rpf = RenderingPrimitiveLines;
		break;
	case VertexFormatTriangles:
		rpf = RenderingPrimitiveTriangles;
		break;
	case VertexFormatTriangleStrips:
		rpf = RenderingPrimitiveTriangleStrip;
		break;
	}

	uint32_t flags = RenderingMesh::EnableShaderPluginUniformBuffers;

	if (transparent)
		flags |= RenderingMesh::HasTransparency;

	DefaultRenderingMesh* rm = new DefaultRenderingMesh (
			rpf,
			flags,
			mesh->getVertexCount(), 0, NULL, 0,
			mesh->getDataBuffer(), mesh->getIndexBuffer(),
			mesh->getVertexOffset(), mesh->getVertexStride(),
			mesh->getSubmeshIDOffset(), mesh->getSubmeshIDStride(),
			mesh->getNormalOffset(), mesh->getNormalStride(),
			mesh->getTexCoordOffset(), mesh->getTexCoordStride(),
			mesh->getVertexColorOffset(), mesh->getVertexColorStride(),
			-1, 0,
			-1, 0
			);

	rm->setModelMatrix(fModelMat);

	rm->meshPtr = mptr->clone();
	rm->meshPtr->lock();

	mptr->release();

	uint32_t numSubmeshes = 0;

	Mesh::SubmeshIterator sit;
	for (sit = mesh->getSubmeshBegin() ; sit != mesh->getSubmeshEnd() ; sit++) {
		Submesh* submesh = *sit;

		bool debug = false;

		if (isChassis  &&  numSubmeshes == 0)
			debug = true;

		generateFromSubmesh(submesh, texSrc, rm, debug);

		numSubmeshes++;
	}

	return rm;
}


void RenderingEntityGenerator::generateFromMapSceneObject(MapSceneObject* mobj, list<RenderingEntity*>& outList)
{
	MapSceneObject::LODInstanceMapIterator beg, end;

	mobj->getRenderingLODInstances(beg, end);

	for (MapSceneObject::LODInstanceMapIterator it = beg ; it != end ; it++) {
		MapSceneObjectLODInstance* lodInst = *it;

		generateFromStaticMapSceneObjectLODInstance(mobj, lodInst, outList);
	}
}


void RenderingEntityGenerator::generateFromAnimatedMapSceneObject(AnimatedMapSceneObject* aobj, list<RenderingEntity*>& outList)
{
	MapSceneObject::LODInstanceMapIterator beg, end;

	aobj->getRenderingLODInstances(beg, end);

	for (MapSceneObject::LODInstanceMapIterator it = beg ; it != end ; it++) {
		MapSceneObjectLODInstance* lodInst = *it;

		if (lodInst->getDefinition()->getType() == ItemTypeAnimatedMapItem) {
			generateFromAnimatedMapSceneObjectLODInstance(aobj, lodInst, outList);
		} else {
			generateFromStaticMapSceneObjectLODInstance(aobj, lodInst, outList);
		}
	}
}


void RenderingEntityGenerator::generateFromStaticMapSceneObjectLODInstance(MapSceneObject* mobj,
		MapSceneObjectLODInstance* lodInst, list<RenderingEntity*>& outList)
{
	MapItemDefinition* def = lodInst->getDefinition();

	MeshPointer* mptr = def->getMeshPointer();
	MeshClump* meshClump = mptr->get(true);

	if (!meshClump)
		return;

	TextureSource* texSrc = def->getTextureSource();

	Matrix4 modelMat = lodInst->getModelMatrix();

	MeshClump::MeshIterator mit;
	for (mit = meshClump->getMeshBegin() ; mit != meshClump->getMeshEnd() ; mit++) {
		Mesh* mesh = *mit;

		// TODO: We only render the last mesh of a mesh clump for static objects. Don't know
		// if this is how it's supposed to be, but at least some mesh clumps in GTA3 have
		// different meshes (actually: DFFGeometries) that represent different levels of
		// physical damage for the object, where the physically intact one seems to be the
		// last in all cases (actually, it's more likely a naming system with suffixes.
		// Should investigate more...)
		MeshClump::MeshIterator nit = mit;
		nit++;

		if (nit != meshClump->getMeshEnd())
			continue;

		RenderingMesh* rm = generateFromMesh(mesh, mptr, texSrc, modelMat, lodInst->hasAlphaTransparency(), mesh->getFrame());
		rm->setPluginRegistry(mobj->getShaderPluginRegistry());

		outList.push_back(rm);
	}
}


void RenderingEntityGenerator::generateFromAnimatedMapSceneObjectLODInstance(AnimatedMapSceneObject* aobj,
		MapSceneObjectLODInstance* lodInst, list<RenderingEntity*>& outList)
{
	AnimatedMapItemDefinition* def = (AnimatedMapItemDefinition*) lodInst->getDefinition();

	CString canimName = aobj->getCurrentAnimation();

	AnimationPackage* apkg = NULL;
	AnimationPackagePointer* apkgPtr = def->getAnimationPackagePointer();

	if (apkgPtr) {
		apkg = apkgPtr->get(true);
	}

	Animation* anim = NULL;

	if (canimName.get()  &&  apkg) {
		anim = apkg->find(canimName);
	}

	if (!anim) {
		// If the mesh has no valid current animation, we can simply render it as a static mesh.
		generateFromStaticMapSceneObjectLODInstance(aobj, lodInst, outList);
		return;
	}


	MeshPointer* mptr = def->getMeshPointer();
	MeshClump* meshClump = mptr->get(true);

	if (!meshClump)
		return;

	TextureSource* texSrc = def->getTextureSource();

	Matrix4 modelMat = lodInst->getModelMatrix();

	Animator animator(meshClump, anim);

	int32_t boneCount = animator.getBoneCount();

	Matrix4* boneMats = new Matrix4[boneCount];

	animator.setTime(aobj->getAnimationTime());

	memcpy(boneMats, animator.getBoneMatrices(), boneCount*sizeof(Matrix4));


	MeshClump::MeshIterator mit;
	for (mit = meshClump->getMeshBegin() ; mit != meshClump->getMeshEnd() ; mit++) {
		Mesh* mesh = *mit;

		MeshFrame* frame = mesh->getFrame();

		Matrix4 fModelMat = modelMat;

		if (frame) {

		}

		int16_t boneIdx = -1;

		if (mesh->getBoneIndexOffset() == -1) {
			if (!animator.hasPseudoBoneNumbers())
				boneIdx = frame->getBoneNumber();
			else
				boneIdx = animator.getPseudoBoneNumber(frame);
		}

		RenderingPrimitiveFormat rpf;

		switch (mesh->getVertexFormat()) {
		case VertexFormatPoints:
			rpf = RenderingPrimitivePoints;
			break;
		case VertexFormatLines:
			rpf = RenderingPrimitiveLines;
			break;
		case VertexFormatTriangles:
			rpf = RenderingPrimitiveTriangles;
			break;
		case VertexFormatTriangleStrips:
			rpf = RenderingPrimitiveTriangleStrip;
			break;
		}

		Matrix4* boneMatsCpy = new Matrix4[boneCount];
		memcpy(boneMatsCpy, boneMats, boneCount*sizeof(Matrix4));

		uint32_t flags = RenderingMesh::IsAnimated | RenderingMesh::EnableShaderPluginUniformBuffers;

		if (lodInst->hasAlphaTransparency()  ||  aobj->selected)
			flags |= RenderingMesh::HasTransparency;

		DefaultRenderingMesh* rm = new DefaultRenderingMesh (
				rpf,
				flags,
				mesh->getVertexCount(), boneCount,
				boneMatsCpy,
				boneIdx,
				mesh->getDataBuffer(), mesh->getIndexBuffer(),
				mesh->getVertexOffset(), mesh->getVertexStride(),
				mesh->getSubmeshIDOffset(), mesh->getSubmeshIDStride(),
				mesh->getNormalOffset(), mesh->getNormalStride(),
				mesh->getTexCoordOffset(), mesh->getTexCoordStride(),
				mesh->getVertexColorOffset(), mesh->getVertexColorStride(),
				mesh->getBoneIndexOffset(), mesh->getBoneIndexStride(),
				mesh->getBoneWeightOffset(), mesh->getBoneWeightStride()
				);
		rm->setPluginRegistry(aobj->getShaderPluginRegistry());

		rm->setModelMatrix(fModelMat);

		rm->meshPtr = mptr->clone();
		rm->meshPtr->lock();

		mptr->release();

		Mesh::SubmeshIterator sit;
		for (sit = mesh->getSubmeshBegin() ; sit != mesh->getSubmeshEnd() ; sit++) {
			Submesh* submesh = *sit;

			generateFromSubmesh(submesh, texSrc, rm);
		}

		outList.push_back(rm);
	}
}


void RenderingEntityGenerator::generateFromSimpleDynamicSceneObject(SimpleDynamicSceneObject* obj, list<RenderingEntity*>& outList)
{
	MeshPointer* mptr = obj->getMeshPointer();
	MeshClump* meshClump = mptr->get(true);

	if (!meshClump)
		return;

	TextureSource* texSrc = obj->getTextureSource();

	Matrix4 modelMat = obj->getModelMatrix();

	MeshClump::MeshIterator mit;
	for (mit = meshClump->getMeshBegin() ; mit != meshClump->getMeshEnd() ; mit++) {
		Mesh* mesh = *mit;

		// TODO: We only render the last mesh of a mesh clump for static objects. Don't know
		// if this is how it's supposed to be, but at least some mesh clumps in GTA3 have
		// different meshes (actually: DFFGeometries) that represent different levels of
		// physical damage for the object, where the physically intact one seems to be the
		// last in all cases (actually, it's more likely a naming system with suffixes.
		// Should investigate more...)
		MeshClump::MeshIterator nit = mit;
		nit++;

		if (nit != meshClump->getMeshEnd())
			continue;

		RenderingMesh* rm = generateFromMesh(mesh, mptr, texSrc, modelMat, obj->hasAlphaTransparency(), mesh->getFrame());
		rm->setPluginRegistry(obj->getShaderPluginRegistry());

		outList.push_back(rm);
	}
}








void RenderingEntityGenerator::generateFromVehicle(Vehicle* veh, list<RenderingEntity*>& outList)
{
	MeshPointer* mptr = veh->getMeshPointer();
	MeshClump* clump = **mptr;

	TextureSource* texSrc = veh->getTextureSource();

	Matrix4 modelMat = veh->getModelMatrix();

	Mesh* chassisMesh = clump->getMeshByName("chassis");
	RenderingMesh* chassis = generateFromMesh(chassisMesh, mptr, texSrc, modelMat, veh->hasAlphaTransparency(),
			chassisMesh->getFrame());
	chassis->setPluginRegistry(veh->getShaderPluginRegistry());

	for (RenderingMesh::SubmeshIterator it = chassis->getSubmeshBegin() ; it != chassis->getSubmeshEnd() ; it++) {
		RenderingSubmesh* sm = *it;

		if (sm->getTexture() == 0)
			sm->setMaterialColor(255, 0, 0, 255);
	}

	outList.push_back(chassis);


	Mesh* wheelMesh = clump->getMeshByName("wheel");

	RenderingMesh* lfWheel = generateFromMesh(wheelMesh, mptr, texSrc, modelMat, veh->hasAlphaTransparency(),
			clump->getRootFrame()->getChildByName("wheel_lf_dummy", true));
	lfWheel->setPluginRegistry(veh->getShaderPluginRegistry());
	outList.push_back(lfWheel);

	RenderingMesh* rfWheel = generateFromMesh(wheelMesh, mptr, texSrc, modelMat, veh->hasAlphaTransparency(),
			clump->getRootFrame()->getChildByName("wheel_rf_dummy", true));
	rfWheel->setPluginRegistry(veh->getShaderPluginRegistry());
	outList.push_back(rfWheel);

	RenderingMesh* lbWheel = generateFromMesh(wheelMesh, mptr, texSrc, modelMat, veh->hasAlphaTransparency(),
			clump->getRootFrame()->getChildByName("wheel_lb_dummy", true));
	lbWheel->setPluginRegistry(veh->getShaderPluginRegistry());
	outList.push_back(lbWheel);

	RenderingMesh* rbWheel = generateFromMesh(wheelMesh, mptr, texSrc, modelMat, veh->hasAlphaTransparency(),
			clump->getRootFrame()->getChildByName("wheel_rb_dummy", true));
	rbWheel->setPluginRegistry(veh->getShaderPluginRegistry());
	outList.push_back(rbWheel);


	Mesh* bonnetMesh = clump->getMeshByName("bonnet_ok");

	RenderingMesh* bonnet = generateFromMesh(bonnetMesh, mptr, texSrc, modelMat, veh->hasAlphaTransparency(), bonnetMesh->getFrame());
	bonnet->setPluginRegistry(veh->getShaderPluginRegistry());
	outList.push_back(bonnet);


	Mesh* lfDoorMesh = clump->getMeshByName("door_lf_ok");

	RenderingMesh* lfDoor = generateFromMesh(lfDoorMesh, mptr, texSrc, modelMat, veh->hasAlphaTransparency(), lfDoorMesh->getFrame());
	lfDoor->setPluginRegistry(veh->getShaderPluginRegistry());
	outList.push_back(lfDoor);


	Mesh* rfDoorMesh = clump->getMeshByName("door_rf_ok");

	RenderingMesh* rfDoor = generateFromMesh(rfDoorMesh, mptr, texSrc, modelMat, veh->hasAlphaTransparency(), rfDoorMesh->getFrame());
	rfDoor->setPluginRegistry(veh->getShaderPluginRegistry());
	outList.push_back(rfDoor);
}
