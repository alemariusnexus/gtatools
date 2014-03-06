#ifndef STATICPHYSICSPOINTER_H_
#define STATICPHYSICSPOINTER_H_

#include "PhysicsPointer.h"


class StaticPhysicsPointer : public PhysicsPointer
{
public:
	StaticPhysicsPointer(btCollisionShape* shape) : shape(shape) {}
	StaticPhysicsPointer(const StaticPhysicsPointer& other) : shape(other.shape) {}
	virtual PhysicsPointer* clone() const { return new StaticPhysicsPointer(*this); }
	virtual btCollisionShape* get(bool lock = false) { return shape; }

private:
	btCollisionShape* shape;
};

#endif /* STATICPHYSICSPOINTER_H_ */
