#include "ManagedPhysicsPointer.h"
#include "PhysicsCacheEntry.h"




btCollisionShape* ManagedPhysicsPointer::get(bool lock)
{
	PhysicsCacheEntry* entry = (PhysicsCacheEntry*) ptr.getEntry(lock);

	if (!entry)
		return NULL;

	return **entry;
}
