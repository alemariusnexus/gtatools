/*
 * CollisionShapePointer.h
 *
 *  Created on: 13.02.2011
 *      Author: alemariusnexus
 */

#ifndef COLLISIONSHAPEPOINTER_H_
#define COLLISIONSHAPEPOINTER_H_

#include <btBulletDynamicsCommon.h>

class CollisionShapePointer {
public:
	virtual CollisionShapePointer* clone() const = 0;
	virtual btCollisionShape* operator*() = 0;
};

#endif /* COLLISIONSHAPEPOINTER_H_ */
