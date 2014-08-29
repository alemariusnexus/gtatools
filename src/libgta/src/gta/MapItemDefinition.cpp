#include "MapItemDefinition.h"
#include "resource/mesh/ManagedMeshPointer.h"
#include "resource/texture/ManagedTextureSource.h"
#include "resource/collision/ManagedCollisionShapePointer.h"
#include "resource/smesh/ManagedShadowMeshPointer.h"
#include "resource/physics/ManagedPhysicsPointer.h"
#include "resource/animation/ManagedAnimationPackagePointer.h"
#include <gtaformats/ide/IDETimedObject.h>



MapItemDefinition::MapItemDefinition(const IDEStaticObject* sobj)
		: animPtr(NULL), flags(0), timeOn(0), timeOff(0)
{
	drawDist = sobj->getDrawDistances()[sobj->getNumSubObjects() - 1];
	CString lMeshName(sobj->getModelName());
	lMeshName.lower();
	meshPtr = new ManagedMeshPointer(lMeshName);
	texSrc = new ManagedTextureSource(CString(sobj->getTXDArchiveName()).lower());
	colPtr = new ManagedCollisionShapePointer(lMeshName);
	smeshPtr = new ManagedShadowMeshPointer(lMeshName);
	physicsPtr = new ManagedPhysicsPointer(lMeshName);

	if (	(sobj->getFlags() & (IDEStaticObject::AlphaTransparency1 | IDEStaticObject::AlphaTransparency2))
			!= 0
	) {
		flags |= AlphaTransparency;
	}

	const IDETimedObject* tobj = dynamic_cast<const IDETimedObject*>(sobj);

	if (tobj) {
		timeOn = tobj->getTimeOn();
		timeOff = tobj->getTimeOn();
	}
}


MapItemDefinition::MapItemDefinition(const IDEAnimation* aobj)
		: flags(0), timeOn(0), timeOff(0)
{
	drawDist = aobj->getDrawDist();
	CString lMeshName(aobj->getModelName());
	lMeshName.lower();
	meshPtr = new ManagedMeshPointer(lMeshName);
	texSrc = new ManagedTextureSource(CString(aobj->getTXDArchiveName()).lower());
	colPtr = new ManagedCollisionShapePointer(lMeshName);
	smeshPtr = new ManagedShadowMeshPointer(lMeshName);
	physicsPtr = new ManagedPhysicsPointer(lMeshName);
	animPtr = new ManagedAnimationPackagePointer(CString(aobj->getAnimationName()).lower());
	defaultAnim = lMeshName;

	if (	(aobj->getFlags() & (IDEStaticObject::AlphaTransparency1 | IDEStaticObject::AlphaTransparency2))
			!= 0
	) {
		flags |= AlphaTransparency;
	}
}

