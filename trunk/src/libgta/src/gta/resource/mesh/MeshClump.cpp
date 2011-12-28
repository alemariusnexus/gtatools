/*
 * MeshClump.cpp
 *
 *  Created on: 04.09.2011
 *      Author: alemariusnexus
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
		: rootFrame(NULL), size(0)
{
}


MeshClump::MeshClump(const DFFMesh* mesh)
		: size(0)
{
	rootFrame = new MeshFrame(mesh->getRootFrame());

	map<const DFFFrame*, MeshFrame*> frameConv;
	_BuildFrameConversionMap(frameConv, mesh->getRootFrame(), rootFrame);

	for (DFFMesh::ConstGeometryIterator it = mesh->getGeometryBegin() ; it != mesh->getGeometryEnd() ; it++) {
		const DFFGeometry* geom = *it;
		Mesh* mesh = new Mesh(*geom);
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


void MeshClump::addMesh(Mesh* mesh)
{
	meshes.push_back(mesh);
	size += mesh->guessSize();
}


bool MeshClump::removeMesh(Mesh* mesh)
{
	MeshIterator it = find(meshes.begin(), meshes.end(), mesh);

	if (it != meshes.end()) {
		return false;
	}

	meshes.erase(it);
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
