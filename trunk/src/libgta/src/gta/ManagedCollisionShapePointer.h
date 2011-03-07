/*
 * ManagedCollisionShapePointer.h
 *
 *  Created on: 13.02.2011
 *      Author: alemariusnexus
 */

#ifndef MANAGEDCOLLISIONSHAPEPOINTER_H_
#define MANAGEDCOLLISIONSHAPEPOINTER_H_

#include "CollisionShapePointer.h"
#include "Engine.h"


class ManagedCollisionShapePointer : public CollisionShapePointer {
public:
	ManagedCollisionShapePointer(hash_t name) : name(name) {}
	ManagedCollisionShapePointer(const ManagedCollisionShapePointer& other) : name(other.name) {}
	virtual CollisionShapePointer* clone() const;
	virtual btCollisionShape* operator*();

private:
	hash_t name;
};

#endif /* MANAGEDCOLLISIONSHAPEPOINTER_H_ */
