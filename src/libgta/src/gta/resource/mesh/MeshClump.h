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

#ifndef MESHCLUMP_H_
#define MESHCLUMP_H_

#include "Mesh.h"
#include "MeshFrame.h"
#include <gtaformats/dff/DFFMesh.h>
#include <vector>
#include "../ResourceCache.h"

using std::vector;



class MeshClump {
public:
	typedef vector<Mesh*> MeshList;
	typedef MeshList::iterator MeshIterator;
	typedef MeshList::const_iterator ConstMeshIterator;

public:
	MeshClump();
	MeshClump(const DFFMesh* mesh);
	~MeshClump();
	void addMesh(Mesh* mesh);
	bool removeMesh(Mesh* mesh);
	size_t getMeshCount() const { return meshes.size(); }
	MeshIterator getMeshBegin() { return meshes.begin(); }
	MeshIterator getMeshEnd() { return meshes.end(); }
	ConstMeshIterator getMeshBegin() const { return meshes.begin(); }
	ConstMeshIterator getMeshEnd() const { return meshes.end(); }
	Mesh* getMeshByName(const CString& name);
	MeshFrame* getRootFrame() { return rootFrame; }
	void setRootFrame(MeshFrame* frame) { rootFrame = frame; }
	cachesize_t guessSize() const { return size; }

private:
	MeshList meshes;
	MeshFrame* rootFrame;
	cachesize_t size;
};

#endif /* MESHCLUMP_H_ */
