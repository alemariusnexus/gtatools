#ifndef PHYSICSPOINTER_H_
#define PHYSICSPOINTER_H_

#include <btBulletCollisionCommon.h>


class PhysicsPointer {
public:
	virtual PhysicsPointer* clone() const = 0;
	virtual btCollisionShape* get(bool lock = false) = 0;
	virtual void release() {}

	virtual btCollisionShape* operator*() { return get(); }
};

#endif /* PHYSICSPOINTER_H_ */
