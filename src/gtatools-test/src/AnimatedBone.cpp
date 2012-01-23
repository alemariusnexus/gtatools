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

#include "AnimatedBone.h"
#include <gta/MeshGenerator.h>
#include <gta/resource/mesh/ManagedMeshPointer.h>
#include <gta/resource/texture/ManagedTextureSource.h>
#include <gta/resource/texture/NullTextureSource.h>
#include <gta/resource/mesh/StaticMeshPointer.h>
#include <gta/scene/StaticSceneObject.h>
#include <gta/StaticMapItemDefinition.h>
#include <gta/Engine.h>
#include <gta/scene/Scene.h>


//#define SCALE 20.0f
#define SCALE 1.0f


AnimatedBone::AnimatedBone(DFFFrame* frame, IFPAnimation* anim)
		: name(NULL), ifpObj(NULL)
{
	//frameMatrix = Matrix4::translation(frame->getTranslation() * SCALE) * Matrix4(frame->getRotation());
	frameMatrix = frame->getModelMatrix();

	if (frame->getBone()) {
		bool found = false;

		for (IFPAnimation::ObjectIterator it = anim->getObjectBegin() ; it != anim->getObjectEnd() ; it++) {
			IFPObject* obj = *it;

			printf("Bone ID: %d\n", obj->getBoneID());

			if (
					(obj->getBoneID() != -1  &&  frame->getBone()->getIndex() == obj->getBoneID())
					//||	obj->getName()  &&  frame->getName()  &&  (LowerHash(obj->getName()) == LowerHash(frame->getName()))
			) {
				name = new char[strlen(obj->getName().get())+1];
				strcpy(name, obj->getName().get());

				frameCount = obj->getFrameCount();
				frames = new IFPFrame*[frameCount];
				rootFrames = (obj->getFrameType() == IFPObject::RootFrame);

				int32_t i = 0;
				for (IFPObject::FrameIterator fit = obj->getFrameBegin() ; fit != obj->getFrameEnd() ; fit++) {
					frames[i] = *fit;
					i++;
				}

				found = true;

				ifpObj = obj;

				break;
			}
		}

		if (found) {
			MeshGenerator gen;

			int vcount, icount;
			float* vertices;
			uint32_t* indices;

			//gen.createSphere(vertices, vcount, indices, icount, 0.025f, 4, 3);
			gen.createBox(vertices, vcount, indices, icount,
					Vector3(-1.0f, -1.0f, -0.5f), Vector3(1.0f, 1.0f, 0.5f));

			Mesh* mesh = new Mesh(vcount, VertexFormatTriangles, 0, vertices);

			uint8_t r, g, b;

			hash_t fnhash = LowerHash(frame->getName().get());

			if (fnhash == LowerHash("Object01")) {
				r = 255;
				g = 0;
				b = 0;
			} else if (fnhash == LowerHash("Object02")) {
				r = 0;
				g = 255;
				b = 0;
			} else if (fnhash == LowerHash("Object03")) {
				r = 0;
				g = 0;
				b = 255;
			} else if (fnhash == LowerHash("Object04")) {
				r = 255;
				g = 0;
				b = 255;
			} else {
				r = 255;
				g = 255;
				b = 255;
			}

			Submesh* submesh = new Submesh(mesh, icount, indices);
			Material* mat = new Material;
			mat->setColor(r, g, b, 255);
			mesh->addMaterial(mat);
			submesh->setMaterial(mat);
			mesh->addSubmesh(submesh);

			MeshClump* mclump = new MeshClump;
			mclump->addMesh(mesh);

			StaticMeshPointer* meshPtr = new StaticMeshPointer(mclump);
			NullTextureSource* texSrc = new NullTextureSource;

			StaticMapItemDefinition* def = new StaticMapItemDefinition(meshPtr, texSrc, NULL, 1000.0f);

			sobj = new StaticSceneObject(def, Matrix4(), NULL);
			//Engine::getInstance()->getScene()->addSceneObject(sobj);

			printf("Added\n");
		} else {
			frameCount = 0;
			frames = NULL;
			rootFrames = false;
			sobj = NULL;
			printf("WARNING: Not found: %d\n", frame->getBone()->getIndex());
		}
	} else {
		frameCount = 0;
		frames = NULL;
		rootFrames = false;
		sobj = NULL;
	}

	for (DFFFrame::ChildIterator it = frame->getChildBegin() ; it != frame->getChildEnd() ; it++) {
		DFFFrame* child = *it;

		AnimatedBone* bchild = new AnimatedBone(child, anim);
		children.push_back(bchild);
	}
}


bool AnimatedBone::getFrames(float relTime, IFPFrame*& f1, IFPFrame*& f2, float& t)
{
	for (int32_t i = 0 ; i < frameCount ; i++) {
		IFPFrame* frame = frames[i];

		if (relTime <= frame->getTime()) {
			f2 = frame;

			if (i == 0) {
				f1 = frames[frameCount-1];
			} else {
				f1 = frames[i-1];
			}

			t = (relTime - f1->getTime()) / (f2->getTime() - f1->getTime());

			return true;
		}
	}

	return false;
}


void AnimatedBone::render(float relTime, const Matrix4& parentMat, const Matrix4& parentFrameMat)
{
	Matrix4 frameMat = parentMat * frameMatrix;
	Matrix4 unanimatedFrameMat = parentFrameMat * frameMatrix;

	IFPFrame* frame = NULL;
	IFPFrame* nextFrame = NULL;
	float t;

	if (getFrames(relTime, frame, nextFrame, t)) {
		Vector3 t1, t2;
		Quaternion r1 = frame->getRotation();
		Quaternion r2 = nextFrame->getRotation();

		if (rootFrames) {
			t1 = ((IFPRootFrame*) frame)->getTranslation() * SCALE;
			t2 = ((IFPRootFrame*) nextFrame)->getTranslation() * SCALE;
		}

		Quaternion ir = r1.slerp(r2, t);

		Vector3 it = t1 + (t2-t1) * t;

		Vector3 irAxis;
		float irAngle;
		ir.toAxisAngle(irAxis, irAngle);

		Matrix4 mat = Matrix4::translation(it) * Matrix4(ir.toMatrix());

		frameMat = parentMat * frameMatrix *  mat;
	}

	if (sobj)
		sobj->setModelMatrix(frameMat);

	for (ChildIterator it = children.begin() ; it != children.end() ; it++) {
		AnimatedBone* child = *it;
		child->render(relTime, frameMat, unanimatedFrameMat);
	}
}
