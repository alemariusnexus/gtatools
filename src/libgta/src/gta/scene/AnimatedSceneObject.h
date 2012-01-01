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

#ifndef ANIMATEDSCENEOBJECT_H_
#define ANIMATEDSCENEOBJECT_H_

#include "VisualSceneObject.h"
#include "../AnimatedMapItemDefinition.h"
#include <gtaformats/util/math/Matrix4.h>
#include <gtaformats/util/CString.h>


class AnimatedSceneObject : public VisualSceneObject {
public:
	AnimatedSceneObject(AnimatedMapItemDefinition* def) : def(def), time(0.0f), currentAnimLength(1.0f) {}
	AnimatedMapItemDefinition* getDefinition() { return def; }
	void setAnimationTime(float t) { time = t; }
	float getAnimationTime() const { return time; }
	void increaseAnimationTime(float t);
	void setCurrentAnimation(const CString& name);
	CString getCurrentAnimation() const { return currentAnim; }
	virtual int getType() const { return SceneObjectAnimated; }
	virtual bool isVisible() const { return true; }
	virtual Vector3 getPosition() const
			{ const float* m = modelMatrix.toArray(); return Vector3(m[12], m[13], m[14]); }
	virtual SceneObject* getLODParent() { return NULL; }
	virtual float getDrawDistance() const { return def->getDrawDistance(); }
	void setModelMatrix(const Matrix4& mat) { modelMatrix = mat; }
	Matrix4 getModelMatrix() const { return modelMatrix; }
	virtual bool hasAlphaTransparency() const { return false; }

private:
	AnimatedMapItemDefinition* def;
	float time;
	Matrix4 modelMatrix;
	CString currentAnim;
	float currentAnimLength;
};

#endif /* ANIMATEDSCENEOBJECT_H_ */
