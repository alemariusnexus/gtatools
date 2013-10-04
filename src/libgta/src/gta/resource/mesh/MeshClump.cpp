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

#include "MeshClump.h"
#include <algorithm>
#include <map>
#include <utility>

using std::find;
using std::map;
using std::pair;


void _BuildFrameConversionMap(map<const DFFFrame*, MeshFrame*>& m, const DFFFrame* frame, MeshFrame* mframe)
{
	m.insert(pair<const DFFFrame*, MeshFrame*>(frame, mframe));

	DFFFrame::ConstChildIterator it = frame->getChildBegin();
	MeshFrame::ChildIterator mit = mframe->getChildBegin();

	for (; it != frame->getChildEnd() ; it++, mit++) {
		_BuildFrameConversionMap(m, *it, *mit);
	}
}



MeshClump::MeshClump()
		: rootFrame(NULL), boneCount(0), size(0), boundsValid(true)
{
	bounds[0] = 0.0f;
	bounds[1] = 0.0f;
	bounds[2] = 0.0f;
	bounds[3] = 0.0f;
}


MeshClump::MeshClump(const DFFMesh* mesh)
		: boneCount(mesh->getBoneCount()), size(0), boundsValid(false)
{
	rootFrame = new MeshFrame(mesh->getRootFrame());

	map<const DFFFrame*, MeshFrame*> frameConv;
	_BuildFrameConversionMap(frameConv, mesh->getRootFrame(), rootFrame);

	for (DFFMesh::ConstGeometryIterator it = mesh->getGeometryBegin() ; it != mesh->getGeometryEnd() ; it++) {
		const DFFGeometry* geom = *it;
		Mesh* mesh = new Mesh(*geom);
		mesh->link();
		MeshFrame* frame = NULL;

		if (geom->getAssociatedFrame()) {
			frame = frameConv[geom->getAssociatedFrame()];
		}

		mesh->setFrame(frame);
		addMesh(mesh);
	}
}


MeshClump::~MeshClump()
{
	if (rootFrame)
		delete rootFrame;

	for (MeshIterator it = meshes.begin() ; it != meshes.end() ; it++) {
		delete *it;
	}
}


void MeshClump::calculateBounds()
{
	if (boundsValid)
		return;

	Vector3 center;

	for (MeshIterator it = meshes.begin() ; it != meshes.end() ; it++) {
		Mesh* mesh = *it;
		float* mbounds = mesh->getBounds();
		center += *((Vector3*) mbounds);
	}

	center *= 1.0f / (meshes.size());

	float radius = 0.0f;

	for (MeshIterator it = meshes.begin() ; it != meshes.end() ; it++) {
		Mesh* mesh = *it;
		float* mbounds = mesh->getBounds();
		float d = (*((Vector3*) mbounds) - center).length() + mbounds[3];

		if (d > radius) {
			radius = d;
		}
	}

	memcpy(bounds, &center, 3*sizeof(float));
	bounds[3] = radius;

	boundsValid = true;
}


void MeshClump::addMesh(Mesh* mesh)
{
	meshes.push_back(mesh);
	size += mesh->guessSize();
	boundsValid = false;
}


bool MeshClump::removeMesh(Mesh* mesh)
{
	MeshIterator it = find(meshes.begin(), meshes.end(), mesh);

	if (it != meshes.end()) {
		return false;
	}

	meshes.erase(it);
	boundsValid = false;
	return true;
}


Mesh* MeshClump::getMeshByName(const CString& name)
{
	for (MeshIterator it = meshes.begin() ; it != meshes.end() ; it++) {
		Mesh* mesh = *it;
		MeshFrame* frame = mesh->getFrame();

		if (frame  &&  frame->getName() == name)
			return mesh;
	}

	return NULL;
}
