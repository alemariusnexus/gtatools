/*
 * VisualSceneObject.h
 *
 *  Created on: 05.09.2011
 *      Author: alemariusnexus
 */

#ifndef VISUALSCENEOBJECT_H_
#define VISUALSCENEOBJECT_H_

#include "SceneObject.h"


class VisualSceneObject : public SceneObject {
public:
	virtual bool hasAlphaTransparency() const = 0;
};

#endif /* VISUALSCENEOBJECT_H_ */
