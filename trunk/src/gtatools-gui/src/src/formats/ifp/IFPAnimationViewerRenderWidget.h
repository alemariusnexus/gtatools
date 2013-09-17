/*
 * IFPAnimationViewerRenderWidget.h
 *
 *  Created on: 28.05.2012
 *      Author: alemariusnexus
 */

#ifndef IFPANIMATIONVIEWERRENDERWIDGET_H_
#define IFPANIMATIONVIEWERRENDERWIDGET_H_

#include "../../gui/GLBaseWidget.h"
#include <gta/scene/Scene.h>
#include <gta/scene/objects/AnimatedMapSceneObject.h>
#include <gta/scene/objects/MapSceneObject.h>


class IFPAnimationViewerRenderWidget : public GLBaseWidget
{
	Q_OBJECT

public:
	IFPAnimationViewerRenderWidget(QWidget* parent = NULL);
	~IFPAnimationViewerRenderWidget();
	void setMeshPointer(MeshPointer* meshPtr);
	void setTextureSource(TextureSource* texSrc);
	void setAnimationPackagePointer(AnimationPackagePointer* anpkPtr);
	void setCurrentAnimation(const CString& curAnim);
	void setAnimationTime(float time);
	void updateDisplay();
	void setBoneRendering(bool br);
	bool isBoneRenderingEnabled() const { return boneRendering; }
	void setBoneLength(float len);
	float getBoneLength() const { return boneLength; }
	void setCurrentBone(const CString& bname) { curBone = bname; }

private:
	void rebuildDisplayedObject();
	void rebuildBoneMesh();
	void buildReducedMeshRecurse(MeshClump* origClump, MeshClump* newClump,MeshFrame* origFrame,
			MeshFrame* newParentFrame, int depth);
	MeshClump* buildReducedMesh(MeshClump* clump);

protected:
	virtual void initializeGL();
	virtual void resizeGL(int w, int h);
	virtual void paintGL();

private:
	AnimatedMapItemDefinition* def;
	AnimatedMapSceneObject* obj;
	CString curAnim;
	float curTime;
	float boneLength;
	bool boneRendering;
	Scene* scene;
	MeshClump* boneMesh;
	MeshPointer* meshPtr;
	MeshPointer* boneMeshPtr;
	TextureSource* texSrc;
	AnimationPackagePointer* anpkPtr;
	CString curBone;
};

#endif /* IFPANIMATIONVIEWERRENDERWIDGET_H_ */
