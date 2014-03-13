/*
	Copyright 2010-2014 David "Alemarius Nexus" Lerch

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

#ifndef RENDERER_H_
#define RENDERER_H_

#include <gta/config.h>
#include "RenderingEntity.h"
#include "../scene/objects/LightSource.h"
#include <list>

using std::list;



class Renderer {
public:
	enum VertexAttributeLocation
	{
		VertexAttributeLocationVertex = 0,
		VertexAttributeLocationNormal = 1,
		VertexAttributeLocationTexCoord = 2,
		VertexAttributeLocationVertexColor = 3,
		VertexAttributeLocationBoneIndex = 4,
		VertexAttributeLocationBoneWeight = 5,
		VertexAttributeLocationSubmeshIndex = 6
	};

public:
	Renderer() : wireframeRendering(false) {}
	virtual ~Renderer() {}
	virtual void enqueueForRendering(list<RenderingEntity*>::iterator beg, list<RenderingEntity*>::iterator end) = 0;
	virtual void enqueueForRendering(list<LightSource*>::iterator beg, list<LightSource*>::iterator end) = 0;
	virtual void render() = 0;

	virtual void enqueueForRendering(RenderingEntity* entity)
	{
		list<RenderingEntity*> l;
		l.push_back(entity);
		enqueueForRendering(l.begin(), l.end());
	}

	virtual void enqueueForRendering(LightSource* ls)
	{
		list<LightSource*> l;
		l.push_back(ls);
		enqueueForRendering(l.begin(), l.end());
	}

	virtual void setWireframeRendering(bool wireframe) { wireframeRendering = wireframe; }
	virtual bool isWireframeRendering() const { return wireframeRendering; }

protected:
	bool wireframeRendering;
};

#endif /* RENDERER_H_ */
