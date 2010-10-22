/*
 * Submesh.h
 *
 *  Created on: 21.10.2010
 *      Author: alemariusnexus
 */

#ifndef SUBMESH_H_
#define SUBMESH_H_

#include <GL/glew.h>
#include "Submesh.h"
#include "ResourceManager.h"
#include "TextureIndex.h"
#include "Material.h"
#include <gtaformats/dff/DFFGeometryPart.h>



class Submesh {
public:
	Submesh(Mesh* mesh, int indexCount, int32_t* indices);
	Submesh(Mesh* mesh, const DFFGeometryPart& part);
	Material* getMaterial() { return material; }
	void setMaterial(Material* mat);
	void bindIndexBuffer();
	int getIndexCount() const { return indexCount; }

private:
	void init(int32_t* indices);

private:
	Mesh* mesh;
	Material* material;
	int indexCount;
	GLuint indexBuffer;
};

#endif /* SUBMESH_H_ */
