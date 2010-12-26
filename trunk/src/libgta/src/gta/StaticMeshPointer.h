/*
 * StaticMeshPointer.h
 *
 *  Created on: 06.11.2010
 *      Author: alemariusnexus
 */

#ifndef STATICMESHPOINTER_H_
#define STATICMESHPOINTER_H_

#include "config.h"
#include "MeshPointer.h"
#include "Mesh.h"


class StaticMeshPointer : public MeshPointer {
public:
	StaticMeshPointer(Mesh* mesh) : mesh(mesh) {}
	virtual Mesh* operator*() { return mesh; }

private:
	Mesh* mesh;
};

#endif /* STATICMESHPOINTER_H_ */
