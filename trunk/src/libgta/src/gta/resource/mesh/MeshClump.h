/*
 * MeshClump.h
 *
 *  Created on: 04.09.2011
 *      Author: alemariusnexus
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
