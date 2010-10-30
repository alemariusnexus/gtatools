/*
 * Mesh.h
 *
 *  Created on: 21.10.2010
 *      Author: alemariusnexus
 */

#ifndef MESH_H_
#define MESH_H_

#include <GL/glew.h>
#include <vector>
#include <gtaformats/dff/DFFGeometry.h>
#include "Engine.h"
#include "Material.h"

using std::vector;



class Submesh;


enum MeshFlags
{
	MeshNormals = 1<<0,
	MeshTexCoords = 1<<1,
	MeshVertexColors = 1<<2,
	MeshTriangleStrips = 1<<3
};


class Mesh {
public:
	typedef vector<Submesh*>::iterator SubmeshIterator;
	typedef vector<Material*>::iterator MaterialIterator;

public:
	Mesh(int vertexCount, int flags, const float* vertices, const float* normals = NULL,
			const float* texCoords = NULL, const uint8_t* vertexColors = NULL);
	Mesh(const DFFGeometry& geometry);
	~Mesh();

	int getFlags() const { return flags; }

	int getVertexCount() const { return vertexCount; }

	int getVertexOffset() const { return 0; }

	int getNormalOffset() const { return normalOffs; }

	int getTexCoordOffset() const { return texCoordOffs; }

	int getVertexColorOffset() const { return vertexColorOffs; }

	void bindDataBuffer();

	void addSubmesh(Submesh* submesh);

	SubmeshIterator getSubmeshBegin() { return submeshes.begin(); }

	SubmeshIterator getSubmeshEnd() { return submeshes.end(); }

	void addMaterial(Material* material);

	MaterialIterator getMaterialBegin() { return materials.begin(); }

	MaterialIterator getMaterialEnd() { return materials.end(); }

	Material* getMaterial(int index) { return materials[index]; }

	hash_t getTXDHash() const { return txdHash; }

	void setTXDHash(hash_t txdHash) { this->txdHash = txdHash; }

	float* getBounds() { return bounds; }

	void setBounds(float x, float y, float z, float r) { bounds[0]=x; bounds[1]=y; bounds[2]=z; bounds[3]=r; }

	int guessSize() const;

private:
	void init(int flags, const float* vertices, const float* normals, const float* texCoords,
			const uint8_t* vertexColors);

private:
	int flags;
	int vertexCount;
	GLuint dataBuffer;
	hash_t txdHash;
	vector<Submesh*> submeshes;
	vector<Material*> materials;
	float bounds[4];
	int normalOffs;
	int texCoordOffs;
	int vertexColorOffs;
};

#endif /* MESH_H_ */
