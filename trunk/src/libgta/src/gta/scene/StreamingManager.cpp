/*
	Copyright 2010-2013 David "Alemarius Nexus" Lerch

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

#include "StreamingManager.h"
#include "parts/VisualSceneObject.h"




void StreamingManager::addViewpoint(StreamingViewpoint* vp)
{
	vps.push_back(vp);
}


void StreamingManager::addSceneObject(SceneObject* obj)
{
	obj->visibleBuckets = 0;
	obj->lastVisibleBuckets = 0;

	objects.push_back(obj);

	if ((obj->getTypeFlags() & SceneObject::TypeFlagPVS)  !=  0) {
		pvs.addObject(dynamic_cast<PVSSceneObject*>(obj));
	} else {
		dynamicObjects.push_back(obj);
	}
}


void StreamingManager::addStreamingListener(StreamingListener* l)
{
	listeners.push_back(l);
}


void StreamingManager::update()
{
	pvs.calculatePVS();

	changedObjects.clear();

	for (ViewpointList::iterator it = vps.begin() ; it != vps.end() ; it++) {
		StreamingViewpoint* vp = *it;

		PVSDatabase::PVSSceneObjectIterator pvBeg, pvEnd;

		if (pvs.queryPVS(pvBeg, pvEnd, vp->getStreamingViewpointPosition(), vp->getStreamingDistanceMultiplier())) {
			processObjects(vp, pvBeg, pvEnd);
			processObjects(vp, dynamicObjects.begin(), dynamicObjects.end());
		} else {
			processObjects(vp, objects.begin(), objects.end());
		}
	}

	for (ObjectList::iterator it = changedObjects.begin() ; it != changedObjects.end() ; it++) {
		SceneObject* obj = *it;

		uint32_t changedBuckets = obj->visibleBuckets ^ obj->lastVisibleBuckets;
		uint32_t inBuckets = changedBuckets & obj->visibleBuckets;
		uint32_t outBuckets = changedBuckets & obj->lastVisibleBuckets;

		streamed(obj, inBuckets, outBuckets);

		obj->lastVisibleBuckets = obj->visibleBuckets;
		obj->visibleBuckets = 0;
	}
}


template <class ItType>
void StreamingManager::processObjects(StreamingViewpoint* svp, ItType beg, ItType end)
{
	uint32_t buckets = svp->getBuckets();

	Frustum frustum = svp->getCullingFrustum();

	float sdMul = svp->getStreamingDistanceMultiplier();
	Vector3 svpPos = svp->getStreamingViewpointPosition();
	uint32_t flags = svp->getStreamingFlags();

	bool fcEnabled = (flags & StreamingViewpoint::FrustumCulling)  !=  0;

	float sx = svpPos.getX();
	float sy = svpPos.getY();
	float sz = svpPos.getZ();

	for (ItType it = beg ; it != end ; it++) {
		SceneObject* obj = *it;
		uint32_t objBuckets = obj->getStreamingBuckets();

		if (objBuckets == obj->visibleBuckets)
			continue;

		Vector3 dv = svpPos - obj->getPosition();

		float distSq = dv.dot(dv);

		float sd = obj->getStreamingDistance() * sdMul;

		bool visible = false;

		if (sd == 0.0f  ||  sd*sd > distSq) {
			if ((obj->getTypeFlags() & SceneObject::TypeFlagVisual)  !=  0) {
				VisualSceneObject* vobj = dynamic_cast<VisualSceneObject*>(obj);

				if (fcEnabled) {
					Vector3 bCenter;
					float bRadius;
					vobj->getBoundingSphere(bCenter, bRadius);

					if (frustum.computeSphere(bCenter, bRadius) == Frustum::Outside) {
						continue;
					}
				}

				vobj->updateRenderingDistance(sqrtf(distSq), sdMul);

				visible = true;
			} else {
				visible = true;
			}
		}

		if (visible) {
			uint32_t newBuckets = obj->visibleBuckets | (buckets & objBuckets);

			if (newBuckets != obj->visibleBuckets) {
				if (obj->visibleBuckets == obj->lastVisibleBuckets) {
					changedObjects.push_back(obj);
				}

				obj->visibleBuckets = newBuckets;
			}
		}
	}
}


void StreamingManager::streamed(SceneObject* obj, uint32_t inBuckets, uint32_t outBuckets)
{
	for (ListenerList::iterator it = listeners.begin() ; it != listeners.end() ; it++) {
		StreamingListener* l = *it;
		l->streamed(obj, inBuckets, outBuckets);
	}
}
