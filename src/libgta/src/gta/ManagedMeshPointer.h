/*
 * ManagedMeshPointer.h
 *
 *  Created on: 06.11.2010
 *      Author: alemariusnexus
 */

#ifndef MANAGEDMESHPOINTER_H_
#define MANAGEDMESHPOINTER_H_

#include "MeshPointer.h"
#include <gtaformats/gf_config.h>
#include "Engine.h"


class ManagedMeshPointer : public MeshPointer {
public:
	ManagedMeshPointer(hash_t meshHash) : meshHash(meshHash) {}
	hash_t getMeshHash() const { return meshHash; }
	void setMeshHash(hash_t hash) { meshHash = hash; }
	virtual Mesh* operator*();

private:
	hash_t meshHash;
};

#endif /* MANAGEDMESHPOINTER_H_ */
