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
{
	frameMatrix = Matrix4::translation(frame->getTranslation() * SCALE) * Matrix4(frame->getRotation());

	if (frame->getBone()) {
		bool found = false;

		for (IFPAnimation::ObjectIterator it = anim->getObjectBegin() ; it != anim->getObjectEnd() ; it++) {
			IFPObject* obj = *it;

			if (
					(obj->getBoneID() != -1  &&  frame->getBone()->getIndex() == obj->getBoneID())
					||	obj->getName()  &&  frame->getName()  &&  (LowerHash(obj->getName()) == LowerHash(frame->getName()))
			) {
				frameCount = obj->getFrameCount();
				frames = new IFPFrame*[frameCount];
				rootFrames = (obj->getFrameType() == IFPObject::RootFrame);

				int32_t i = 0;
				for (IFPObject::FrameIterator fit = obj->getFrameBegin() ; fit != obj->getFrameEnd() ; fit++) {
					frames[i] = *fit;
					i++;
				}

				found = true;
			}
		}

		if (found) {
			MeshGenerator gen;

			int vcount, icount;
			float* vertices;
			uint32_t* indices;

			//gen.createSphere(vertices, vcount, indices, icount, 1.0f, 4, 3);
			gen.createBox(vertices, vcount, indices, icount,
					Vector3(-1.0f, -1.0f, -0.5f), Vector3(1.0f, 1.0f, 0.5f));

			Mesh* mesh = new Mesh(vcount, VertexFormatTriangles, 0, vertices);

			uint8_t r, g, b;

			hash_t fnhash = LowerHash(frame->getName());

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

			StaticMeshPointer* meshPtr = new StaticMeshPointer(mesh);
			NullTextureSource* texSrc = new NullTextureSource;

			StaticMapItemDefinition* def = new StaticMapItemDefinition(meshPtr, texSrc, NULL, 1000.0f);

			sobj = new StaticSceneObject(def, Matrix4(), NULL);
			Engine::getInstance()->getScene()->addSceneObject(sobj);

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


void AnimatedBone::render(float relTime, const Matrix4& parentMat, const Matrix4& parentFrameMat)
{
	Matrix4 frameMat = parentMat * frameMatrix;
	Matrix4 unanimatedFrameMat = parentFrameMat * frameMatrix;

	float time = relTime;

	for (int32_t i = 0 ; i < frameCount ; i++) {
		IFPFrame* frame = frames[i];

		if (time - frame->getTime() < 0  ||  i == frameCount-1) {
			IFPFrame* nextFrame = NULL;

			if (i+1 == frameCount) {
				nextFrame = frames[0];
			} else {
				nextFrame = frames[i+1];
			}

			Vector3 t1, t2;
			Quaternion r1 = frame->getRotation();
			Quaternion r2 = nextFrame->getRotation();

			//r1.normalize();
			//r2.normalize();

			if (rootFrames) {
				t1 = ((IFPRootFrame*) frame)->getTranslation() * SCALE;
				t2 = ((IFPRootFrame*) nextFrame)->getTranslation() * SCALE;
			}

			float t = time / frame->getTime();

			if (t > 1.0f)
				t = 1.0f;

			/*printf("%f around %f %f %f [%d:%f]   and   %f around %f %f %f [%d:%f]\n",
					rotAngle1 * (180.0f/M_PI), rotAxis1.getX(), rotAxis1.getY(), rotAxis1.getZ(), i, frame->getTime(),
					rotAngle2 * (180.0f/M_PI), rotAxis2.getX(), rotAxis2.getY(), rotAxis2.getZ(), i+1, nextFrame->getTime());*/

			Quaternion ir;

			if (fabs(r1.angle(r2) - M_PI) <= 0.0000001f) {
				Quaternion relRot = lastRotation.conjugate() * r1;
				Quaternion intermediateRot = r1 * relRot;
				float intermediateAngle = r1.angle(intermediateRot);
				float splitFactor = intermediateAngle / M_PI;

				if (t <= splitFactor) {
					ir = r1.slerp(intermediateRot, (1.0f/splitFactor) * t);
				} else {
					ir = intermediateRot.slerp(r2, (1.0f / (1.0f-splitFactor)) * (t-splitFactor));
				}
			} else {
				ir = r1.slerp(r2, t);
				lastRotation = r1;
			}

			//Vector3 it = t2*t + t1*(1.0f-t);
			Vector3 it = t1 + (t2-t1) * t;

			Vector3 irAxis;
			float irAngle;
			ir.toAxisAngle(irAxis, irAngle);

			/*printf("Frame %i/%i [%f] has %f %f %f  :  %f\n", i, frameCount, t,
					irAxis.getX(), irAxis.getY(), irAxis.getZ(), irAngle * (180.0f/M_PI));*/

			Quaternion relRot = r1.conjugate() * r2;

			/*printf("From (%f %f %f) : %f  to  (%f %f %f) : %f  is  (%f %f %f) : %f\n",
					r1axis.getX(), r1axis.getY(), r1axis.getZ(), r1angle * (180.0f/M_PI),
					r2axis.getX(), r2axis.getY(), r2axis.getZ(), r2angle * (180.0f/M_PI),
					lastRotationAxis.getX(), lastRotationAxis.getY(), lastRotationAxis.getZ(), tmp * (180.0f/M_PI));*/

			/*printf("Rotation axis: %f %f %f  :  %f\n", lastRotationAxis.getX(), lastRotationAxis.getY(), lastRotationAxis.getZ(),
					tmp);*/

			Matrix4 mat = Matrix4::translation(it) * Matrix4(ir.toMatrix());
			//Matrix4 mat = Matrix4(ir.toMatrix()) * Matrix4::translation(it);

			/*printf("Frame %d/%d [%f] slerps between (%f %f %f %f) and (%f %f %f %f) / %f   =>   (%f %f %f %f)\n",
					i, frameCount, frame->getTime(),
					r1.getW(), r1.getX(), r1.getY(), r1.getZ(),
					r2.getW(), r2.getX(), r2.getY(), r2.getZ(),
					t,
					ir.getW(), ir.getX(), ir.getY(), ir.getZ());*/
			//printf("Frame %d/%d [%f]   %f %f %f %f\n", i, frameCount, t, ir.getW(), ir.getX(), ir.getY(), ir.getZ());
			//printf("Frame %d/%d [%f]   %f %f %f\n", i, frameCount, t, it.getX(), it.getY(), it.getZ());

			frameMat = parentMat * frameMatrix *  mat;

			break;
		}

		time -= frame->getTime();
	}

	if (sobj)
		sobj->setModelMatrix(frameMat);

	for (ChildIterator it = children.begin() ; it != children.end() ; it++) {
		AnimatedBone* child = *it;
		child->render(relTime, frameMat, unanimatedFrameMat);
	}
}
