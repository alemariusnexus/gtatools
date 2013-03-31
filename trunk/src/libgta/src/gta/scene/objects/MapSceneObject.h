/*
	Copyright 2010-2012 David "Alemarius Nexus" Lerch

	This file is part of libgta.

	libgta is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	libgta is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with libgta.  If not, see <http://www.gnu.org/licenses/>.

	Additional permissions are granted, which are listed in the file
	GPLADDITIONS.
 */

#ifndef MAPSCENEOBJECT_H_
#define MAPSCENEOBJECT_H_

#include "../parts/VisualSceneObject.h"
#include "../parts/PVSSceneObject.h"
#include "../parts/RigidBodySceneObject.h"
#include "../../MapItemDefinition.h"
#include "MapSceneObjectLODInstance.h"
#include <set>
#include <algorithm>

using std::multiset;



class MapSceneObject : public virtual VisualSceneObject, public virtual PVSSceneObject,
		public virtual RigidBodySceneObject
{
private:
	class LODInstanceComparator
	{
	public:
		bool operator()(MapSceneObjectLODInstance* i1, MapSceneObjectLODInstance* i2)
				{ return i1->getStreamingDistance() < i2->getStreamingDistance(); }
	};

public:
	typedef multiset<MapSceneObjectLODInstance*, LODInstanceComparator> LODInstanceMap;
	typedef LODInstanceMap::iterator LODInstanceMapIterator;
	typedef LODInstanceMap::const_iterator ConstLODInstanceMapIterator;

public:
	MapSceneObject();
	MapSceneObject(const MapSceneObject& other);

	virtual SceneObject* clone() const { return new MapSceneObject(*this); }
	virtual bool isEnabled() const
			{ return enabled; }
	virtual float getStreamingDistance() const
			{ return maxStreamingDist; }

	virtual bool hasAlphaTransparency() const;

	virtual Matrix4& getModelMatrix() { return mm; }
	virtual const Matrix4& getModelMatrix() const { return mm; }
	virtual void setModelMatrix(const Matrix4& matrix);

	virtual SceneObjectDefinitionInfo* getDefinitionInfo() { return defInfo; }

	virtual typeflags_t getTypeFlags() const
			{ return TypeFlagVisual | TypeFlagPVS | TypeFlagRigidBody; }

	virtual void setEnabled(bool enabled) { this->enabled = enabled; }
	void setDefinitionInfo(SceneObjectDefinitionInfo* info) { defInfo = info; }

	virtual const btRigidBody* getRigidBody() const { return rb; }
	virtual btRigidBody* getRigidBody() { return rb; }
	virtual CollisionShapePointer* getCollisionShapePointer()
			{ return getLODInstance()->getDefinition()->getCollisionShapePointer(); }

	void addLODInstance(MapSceneObjectLODInstance* inst);

	LODInstanceMapIterator getLODInstanceBegin() { return lodInsts.begin(); }
	LODInstanceMapIterator getLODInstanceEnd() { return lodInsts.end(); }
	ConstLODInstanceMapIterator getLODInstanceBegin() const { return lodInsts.begin(); }
	ConstLODInstanceMapIterator getLODInstanceEnd() const { return lodInsts.end(); }
	size_t getLODInstanceCount() const { return lodInsts.size(); }

	bool getLODInstances(LODInstanceMapIterator& beg, LODInstanceMapIterator& end, size_t depth = 0);
	const bool getLODInstances(LODInstanceMapIterator& beg, LODInstanceMapIterator& end, size_t depth = 0) const
			{ return const_cast<MapSceneObject*>(this)->getLODInstances(beg, end, depth); }

	MapSceneObjectLODInstance* getLODInstance(size_t depth = 0)
			{ LODInstanceMapIterator beg, end; return getLODInstances(beg, end ,depth) ? *beg : NULL; }
	const MapSceneObjectLODInstance* getLODInstance(size_t depth = 0) const
			{ LODInstanceMapIterator beg, end; return getLODInstances(beg, end, depth) ? *beg : NULL; }

	void getRenderingLODInstances(LODInstanceMapIterator& beg, LODInstanceMapIterator& end)
			{ beg = visibleInstBeg; end = visibleInstEnd; }
	void getRenderingLODInstances(ConstLODInstanceMapIterator& beg, ConstLODInstanceMapIterator& end) const
			{ beg = visibleInstBeg; end = visibleInstEnd; }


	virtual ShadowMeshPointer* getCurrentShadowMeshPointer()
			// TODO
			{ return NULL; }
			//{ return visibleInst ? visibleInst->getDefinition()->getShadowMeshPointer() : NULL; }

	void setMass(float m);
	float getMass() const { return mass; }

	virtual void getBoundingSphere(Vector3& center, float& radius);

private:
	virtual void updateRenderingDistance(float dist, float sdMultiplier);
	virtual void resetRenderingDistance();
	void calculateBounds();

private:
	bool enabled;
	SceneObjectDefinitionInfo* defInfo;
	btRigidBody* rb;
	LODInstanceMap lodInsts;
	LODInstanceMapIterator visibleInstBeg, visibleInstEnd;
	float mass;
	float maxStreamingDist;
	Matrix4 mm;

	Vector3 boundsCenter;
	float boundsRadius;
	bool boundsValid;
};

#endif /* MAPSCENEOBJECT_H_ */
