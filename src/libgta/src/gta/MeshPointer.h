/*
 * MeshPointer.h
 *
 *  Created on: 06.11.2010
 *      Author: alemariusnexus
 */

#ifndef MESHPOINTER_H_
#define MESHPOINTER_H_

#include "config.h"
#include "Mesh.h"


class MeshPointer {
public:
	virtual Mesh* operator*() = 0;
};

#endif /* MESHPOINTER_H_ */
