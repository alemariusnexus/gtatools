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
