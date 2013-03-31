/*
	Copyright 2010-2012 David "Alemarius Nexus" Lerch

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

#include "DefaultStaticRenderingMesh.h"
#include "AnimatedRenderingMesh.h"

#include "../scene/objects/AnimatedMapSceneObject.h"
#include "../scene/objects/MapSceneObject.h"

#include "../Animator.h"



//void RenderingEntityGenerator::generate(VisualSceneObject* sobj, list<RenderingEntity*>& outList)
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
	}
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

	TextureSource* texSrc = def->getTextureSource();

	Matrix4 modelMat = lodInst->getModelMatrix();

	MeshClump::MeshIterator mit;
	for (mit = meshClump->getMeshBegin() ; mit != meshClump->getMeshEnd() ; mit++) {
		Mesh* mesh = *mit;

		MeshFrame* frame = mesh->getFrame();

		Matrix4 fModelMat = modelMat;

		if (frame) {

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

		Mesh::SubmeshIterator sit;
		for (sit = mesh->getSubmeshBegin() ; sit != mesh->getSubmeshEnd() ; sit++) {
			Submesh* submesh = *sit;

			Material* mat = submesh->getMaterial();

			GLuint oglTex = 0;

			uint8_t r = 255;
			uint8_t g = 255;
			uint8_t b = 255;
			uint8_t a = 255;

			if (mat) {
				if (mat->isTextured()) {
					Texture* tex = texSrc->getTexture(mat->getTexture(), true);

					if (tex)
						oglTex = tex->getGLTexture();
				}

				mat->getColor(r, g, b, a);
			}

			DefaultStaticRenderingMesh* rm = new DefaultStaticRenderingMesh (
					rpf,
					mesh->getVertexCount(), submesh->getIndexCount(),
					mesh->getDataBuffer(), submesh->getIndexBuffer(),
					mesh->getNormalOffset(), mesh->getTexCoordOffset(), mesh->getVertexColorOffset(),
					oglTex
					);

			rm->setModelMatrix(fModelMat);
			rm->setMaterialColor(r, g, b, a);

			rm->setHasTransparency(lodInst->hasAlphaTransparency());

			rm->meshPtr = mptr->clone();
			rm->meshPtr->lock();
			rm->texSrc = texSrc->clone();
			rm->texSrc->lock();

			mptr->release();
			texSrc->release();

			outList.push_back(rm);
		}
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

		Mesh::SubmeshIterator sit;
		for (sit = mesh->getSubmeshBegin() ; sit != mesh->getSubmeshEnd() ; sit++) {
			Submesh* submesh = *sit;

			Material* mat = submesh->getMaterial();

			GLuint oglTex = 0;

			uint8_t r = 255;
			uint8_t g = 255;
			uint8_t b = 255;
			uint8_t a = 255;

			if (mat) {
				if (mat->isTextured()) {
					Texture* tex = texSrc->getTexture(mat->getTexture(), true);

					if (tex)
						oglTex = tex->getGLTexture();
				}

				mat->getColor(r, g, b, a);
			}

			Matrix4* boneMatsCpy = new Matrix4[boneCount];
			memcpy(boneMatsCpy, boneMats, boneCount*sizeof(Matrix4));

			AnimatedRenderingMesh* rm = new AnimatedRenderingMesh (
					rpf,
					mesh->getVertexCount(), submesh->getIndexCount(), boneCount,
					boneMatsCpy,
					boneIdx,
					mesh->getDataBuffer(), submesh->getIndexBuffer(),
					mesh->getNormalOffset(), mesh->getTexCoordOffset(), mesh->getVertexColorOffset(),
					mesh->getBoneIndexOffset(), mesh->getBoneWeightOffset(),
					oglTex
					);
			rm->setModelMatrix(fModelMat);
			rm->setMaterialColor(r, g, b, a);

			rm->setHasTransparency(lodInst->hasAlphaTransparency());

			/*rm->meshPtr = mptr->clone();
			rm->meshPtr->lock();
			rm->texSrc = texSrc->clone();
			rm->texSrc->lock();

			mptr->release();
			texSrc->release();*/

			outList.push_back(rm);
		}
	}
}
