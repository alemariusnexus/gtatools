/*
 * StaticCollisionShapePointer.h
 *
 *  Created on: 09.06.2012
 *      Author: alemariusnexus
 */

#ifndef STATICCOLLISIONSHAPEPOINTER_H_
#define STATICCOLLISIONSHAPEPOINTER_H_

#include "CollisionShapePointer.h"



class StaticCollisionShapePointer : public CollisionShapePointer
{
public:
	StaticCollisionShapePointer(btCollisionShape* shape) : shape(shape) {}
	StaticCollisionShapePointer(const StaticCollisionShapePointer& other) : shape(other.shape) {}
	virtual CollisionShapePointer* clone() const { return new StaticCollisionShapePointer(*this); }
	virtual btCollisionShape* get(bool lock = false) { return shape; }

private:
	btCollisionShape* shape;
};

#endif /* STATICCOLLISIONSHAPEPOINTER_H_ */
