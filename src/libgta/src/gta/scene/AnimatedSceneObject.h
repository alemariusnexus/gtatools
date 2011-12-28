/*
 * AnimatedSceneObject.h
 *
 *  Created on: 04.09.2011
 *      Author: alemariusnexus
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
