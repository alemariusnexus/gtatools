/*
 * IFPAnimationViewerRenderWidget.cpp
 *
 *  Created on: 28.05.2012
 *      Author: alemariusnexus
 */

#include "IFPAnimationViewerRenderWidget.h"
#include <gta/scene/DefaultRenderer.h>
#include <gta/scene/BasicTransparencyAlgorithm.h>
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
		: GLBaseWidget(parent), def(NULL), obj(NULL), curTime(0.0f), boneLength(0.025f), boneRendering(false),
		  scene(NULL), boneMesh(NULL), meshPtr(NULL), boneMeshPtr(NULL), texSrc(NULL), anpkPtr(NULL)
{

}


IFPAnimationViewerRenderWidget::~IFPAnimationViewerRenderWidget()
{
	delete obj;
	delete def;
	delete ((DefaultRenderer*) scene->getRenderer())->getTransparencyAlgorithm();
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

		def = new AnimatedMapItemDefinition(mptr, texSrc, NULL, anpkPtr, 500.0f, 0);
		obj = new AnimatedMapSceneObject(def);
		obj->setAutoAnimationEnabled(false);

		obj->setCurrentAnimation(curAnim);
		obj->setAnimationTime(curTime);

		scene->addSceneObject(obj);
	}
}


void IFPAnimationViewerRenderWidget::initializeGL()
{
	GLBaseWidget::initializeGL();

	DefaultRenderer* renderer = new DefaultRenderer;

	BasicTransparencyAlgorithm* basicAlgo = new BasicTransparencyAlgorithm;
	renderer->setTransparencyAlgorithm(basicAlgo);

	scene = new Scene;
	scene->setRenderer(renderer);

	PVSDatabase* pvs = new PVSDatabase;
	scene->setPVSDatabase(pvs);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}


void IFPAnimationViewerRenderWidget::resizeGL(int w, int h)
{
	GLBaseWidget::resizeGL(w, h);
}


void IFPAnimationViewerRenderWidget::paintGL()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	GLBaseWidget::paintGL();

	Engine* engine = Engine::getInstance();

	engine->setScene(scene);

	engine->render();
}
