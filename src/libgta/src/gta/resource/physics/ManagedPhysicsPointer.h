#ifndef MANAGEDPHYSICSPOINTER_H_
#define MANAGEDPHYSICSPOINTER_H_

#include "PhysicsPointer.h"
#include "../../Engine.h"


class ManagedPhysicsPointer : public PhysicsPointer
{
public:
	ManagedPhysicsPointer(const CString& name)
			: ptr(Engine::getInstance()->getPhysicsCache()->getEntryPointer(name)) {}
	ManagedPhysicsPointer(const ManagedPhysicsPointer& other) : ptr(other.ptr) {}
	virtual PhysicsPointer* clone() const { return new ManagedPhysicsPointer(*this); }
	virtual btCollisionShape* get(bool lock = false);

private:
	Engine::StringResourceCache::Pointer ptr;
};

#endif /* MANAGEDPHYSICSPOINTER_H_ */
