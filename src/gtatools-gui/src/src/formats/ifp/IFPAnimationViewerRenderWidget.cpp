/*
	Copyright 2010-2014 David "Alemarius Nexus" Lerch

	This file is part of gtatools-gui.

	gtatools-gui is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	gtatools-gui is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with gtatools-gui.  If not, see <http://www.gnu.org/licenses/>.

	Additional permissions are granted, which are listed in the file
	GPLADDITIONS.
 */

#include "IFPAnimationViewerRenderWidget.h"
#include <gta/Engine.h>
#include <gta/render/DefaultRenderer.h>
#include <gta/scene/visibility/PVSDatabase.h>
#include <gta/resource/mesh/StaticMeshPointer.h>
#include <gta/MeshGenerator.h>



void IFPAnimationViewerRenderWidget::buildReducedMeshRecurse(MeshClump* origClump, MeshClump* newClump,
		MeshFrame* origFrame, MeshFrame* newParentFrame, int depth = 0)
{
        MeshFrame* newFrame = new MeshFrame;
        newFrame->setModelMatrix(origFrame->getModelMatrix());
        newFrame->setName(origFrame->getName());
        newFrame->setBoneID(origFrame->getBoneID());
        newFrame->setBoneNumber(origFrame->getBoneNumber());

        if (newParentFrame) {
                newParentFrame->addChild(newFrame);
        } else {
                newClump->setRootFrame(newFrame);
        }

        MeshGenerator gen;
        //Mesh* mesh = gen.createSphere(0.1f, 10, 10);
        float len = boneLength;
        Mesh* mesh = gen.createBox(Vector3(-len, -2*len, -len), Vector3(len, 2*len, len));
        mesh->setFrame(newFrame);
        newClump->addMesh(mesh);

        for (Mesh::SubmeshIterator it = mesh->getSubmeshBegin() ; it != mesh->getSubmeshEnd() ; it++) {
                Submesh* submesh = *it;

                Material* mat = new Material;

                if (newFrame->getName() == curBone) {
                	mat->setColor(255, 255, 255, 255);
                } else {
                	mat->setColor(255 - depth*25, 0, 0, 255);
                }

                submesh->setMaterial(mat);
        }

        MeshFrame::ChildIterator it;
        for (it = origFrame->getChildBegin() ; it != origFrame->getChildEnd() ; it++) {
                MeshFrame* child = *it;
                buildReducedMeshRecurse(origClump, newClump, child, newFrame, depth+1);
        }
}


MeshClump* IFPAnimationViewerRenderWidget::buildReducedMesh(MeshClump* clump)
{
        MeshClump* newClump = new MeshClump;
        newClump->setBoneCount(clump->getBoneCount());
        buildReducedMeshRecurse(clump, newClump, clump->getRootFrame(), NULL);
        return newClump;
}







IFPAnimationViewerRenderWidget::IFPAnimationViewerRenderWidget(QWidget* parent)
		: GLBaseWidget(parent), renderer(NULL), def(NULL), obj(NULL), curTime(0.0f), boneLength(0.025f),
		  boneRendering(false), scene(NULL), boneMesh(NULL), meshPtr(NULL), boneMeshPtr(NULL), texSrc(NULL),
		  anpkPtr(NULL)
{

}


IFPAnimationViewerRenderWidget::~IFPAnimationViewerRenderWidget()
{
	delete obj;
	delete def;
	delete scene->getRenderer();
	delete scene;
	delete meshPtr;
	delete texSrc;
	delete anpkPtr;
}


void IFPAnimationViewerRenderWidget::setBoneRendering(bool br)
{
	boneRendering = br;
}


void IFPAnimationViewerRenderWidget::setBoneLength(float len)
{
	boneLength = len;

	if (meshPtr) {
		if (boneMesh) {
			delete boneMesh;
			delete boneMeshPtr;
		}

		MeshClump* clump = **meshPtr;

		boneMesh = buildReducedMesh(clump);
		boneMeshPtr = new StaticMeshPointer(boneMesh);
	}
}


void IFPAnimationViewerRenderWidget::setMeshPointer(MeshPointer* meshPtr)
{
	this->meshPtr = meshPtr;

	if (boneMesh) {
		delete boneMesh;
		delete boneMeshPtr;
	}

	rebuildBoneMesh();
}


void IFPAnimationViewerRenderWidget::rebuildBoneMesh()
{
	MeshClump* clump = **meshPtr;

	boneMesh = buildReducedMesh(clump);
	boneMeshPtr = new StaticMeshPointer(boneMesh);
}


void IFPAnimationViewerRenderWidget::setTextureSource(TextureSource* texSrc)
{
	this->texSrc = texSrc;
}


void IFPAnimationViewerRenderWidget::setAnimationPackagePointer(AnimationPackagePointer* anpkPtr)
{
	this->anpkPtr = anpkPtr;
}


void IFPAnimationViewerRenderWidget::setCurrentAnimation(const CString& curAnim)
{
	this->curAnim = curAnim;

	if (obj) {
		obj->setCurrentAnimation(curAnim);
		updateGL();
	}
}


void IFPAnimationViewerRenderWidget::setAnimationTime(float time)
{
	curTime = time;

	if (obj) {
		obj->setAnimationTime(time);
		updateGL();
	}
}


void IFPAnimationViewerRenderWidget::updateDisplay()
{
	rebuildBoneMesh();
	rebuildDisplayedObject();

	updateGL();
}


void IFPAnimationViewerRenderWidget::rebuildDisplayedObject()
{
	scene->clear();

	if (meshPtr) {
		MeshPointer* mptr;

		if (boneRendering)
			mptr = boneMeshPtr;
		else
			mptr = meshPtr;

		def = new MapItemDefinition(mptr, texSrc, NULL, NULL, NULL, 500.0f, 0);
		def->setAnimationPackagePointer(anpkPtr);

		obj = new MapSceneObject;
		MapSceneObjectLODInstance* inst = new MapSceneObjectLODInstance(def);
		obj->addLODInstance(inst);
		obj->setAutoAnimationEnabled(false);

		obj->setCurrentAnimation(curAnim);
		obj->setAnimationTime(curTime);

		scene->addSceneObject(obj);
	}
}


void IFPAnimationViewerRenderWidget::initializeGL()
{
	GLBaseWidget::initializeGL();

	cam.setPosition(0.0f, 4.0f, 0.0f);

	scene = new Scene;
	scene->addStreamingViewpoint(&cam);
	scene->getStreamingManager()->getPVSDatabase()->setEnabled(false);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}


void IFPAnimationViewerRenderWidget::resizeGL(int w, int h)
{
	GLBaseWidget::resizeGL(w, h);

	Engine* engine = Engine::getInstance();
	engine->setViewportSize(getViewportWidth(), getViewportHeight());

	if (renderer) {
		delete renderer;
	}

	renderer = new DefaultRenderer;

	scene->setRenderer(renderer);
}


void IFPAnimationViewerRenderWidget::paintGL()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	GLBaseWidget::paintGL();

	Engine* engine = Engine::getInstance();

	engine->setScene(scene);

	engine->advanceFrame(0);
	engine->renderFrame();

	printf("VisObjs: %u\n", scene->getLastVisibleObjectCount());
}
