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

#ifndef STREAMINGMANAGER_H_
#define STREAMINGMANAGER_H_

#include "StreamingViewpoint.h"
#include "parts/SceneObject.h"
#include "visibility/PVSDatabase.h"
#include <map>
#include <list>

using std::map;
using std::list;



class StreamingManager
{
public:
	enum BucketType
	{
		VisibleBucket = 1 << 0,
		PhysicsBucket = 1 << 1
	};

	class StreamingListener
	{
	public:
		virtual void streamed(SceneObject* obj, uint32_t inBuckets, uint32_t outBuckets) = 0;
	};

private:
	typedef list<StreamingViewpoint*> ViewpointList;
	typedef list<SceneObject*> ObjectList;
	typedef list<StreamingListener*> ListenerList;

public:
	void addViewpoint(StreamingViewpoint* vp);
	void addSceneObject(SceneObject* obj);
	void addStreamingListener(StreamingListener* l);
	void update();
	PVSDatabase* getPVSDatabase() { return &pvs; }

private:
	template <class ItType>
	void processObjects(StreamingViewpoint* svp, ItType beg, ItType end);

	void streamed(SceneObject* obj, uint32_t inBuckets, uint32_t outBuckets);

private:
	PVSDatabase pvs;
	ViewpointList vps;
	ObjectList objects;
	ObjectList dynamicObjects;
	ObjectList changedObjects;
	ListenerList listeners;
};

#endif /* STREAMINGMANAGER_H_ */
