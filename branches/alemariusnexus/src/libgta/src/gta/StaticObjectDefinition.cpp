/*
 * StaticObjectDefinition.cpp
 *
 *  Created on: 29.08.2010
 *      Author: alemariusnexus
 */

#include "StaticObjectDefinition.h"
#include <gtaformats/engine/ResourceIndex.h>
#include <gtaformats/util/util.h>
#include <gtaformats/util/math/Matrix4.h>
#include "ShaderProgram.h"
#include "GLException.h"


StaticObjectDefinition::StaticObjectDefinition(DFFMesh* mesh, const char* txdName, ResourceManager* res)
		: res(res), txdName(txdName ? res->hash(txdName) : 0), txdKnown(txdName != NULL)
{
	DFFGeometry* geom = mesh->getGeometry(0);
	init(geom);
}


StaticObjectDefinition::StaticObjectDefinition(DFFGeometry* geom, const char* txdName, ResourceManager* res)
		: res(res), txdName(txdName ? res->hash(txdName) : 0), txdKnown(txdName != NULL)
{
	init(geom);
}


StaticObjectDefinition::StaticObjectDefinition(DFFGeometry* geom, DFFGeometryPart* part, const char* txdName,
			ResourceManager* res)
		: res(res), txdName(txdName ? res->hash(txdName) : 0), txdKnown(txdName != NULL)
{
	init(geom, part);
}


StaticObjectDefinition::~StaticObjectDefinition()
{
	delete[] indexCounts;
	delete[] partTextures;
	delete[] partHasTextures;
	glDeleteBuffers(1, &dataBuffer);
	glDeleteBuffers(1, &indexBuffer);
}


void StaticObjectDefinition::init(DFFGeometry* geom, DFFGeometryPart* renderPart)
{
	glGenBuffers(1, &dataBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, dataBuffer);

	DFFFrame* frame = geom->getAssociatedFrame();

	DFFBoundingSphere* bounds = geom->getBounds();
	setBounds(bounds->x, bounds->y, bounds->z, bounds->radius);

	Matrix3 rotMat = frame->getRotation();
	float* arot = rotMat.toArray();

	Vector3 trans = frame->getTranslation();

	flags = geom->getFlags();

	int32_t vc = geom->getVertexCount();
	vertexCount = vc;
	float* vertices = geom->getVertices();
	float* normals = geom->getNormals();
	float* texCoords = geom->getUVCoordSet(0);
	//uint8_t* colors = geom->getVertexColors();

	normals = NULL;
	flags &= ~GEOMETRY_FLAG_NORMALS;

	unsigned int dataLen = vc*3*4;

	if (normals)
		dataLen += vc*3*4;
	if (texCoords)
		dataLen += vc*2*4;
	//if (colors)
		//dataLen += vc*4;

	unsigned int dataOffset = 0;
	char* data = new char[dataLen];
	memcpy(data, vertices, vc*3*4);
	dataOffset += vc*3*4;

	if (normals) {
		memcpy(data+dataOffset, normals, vc*3*4);
		dataOffset += vc*3*4;
	}
	if (texCoords) {
		memcpy(data+dataOffset, texCoords, vc*2*4);
		dataOffset += vc*2*4;
	}
	//if (colors) {
		//memcpy(data+dataOffset, colors, vc*4);
		//dataOffset += vc*4;
	//}

	glBufferData(GL_ARRAY_BUFFER, dataLen, data, GL_STATIC_DRAW);
	delete[] data;

	glGenBuffers(1, &indexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);

	unsigned int indexLen = 0;
	partCount = renderPart ? 1 : geom->getPartCount();
	indexCounts = new int32_t[partCount];
	partTextures = new GLuint[partCount];
	partHasTextures = new bool[partCount];

	DFFGeometry::PartIterator it;
	int i = 0;

	for (it = geom->getPartBegin() ; it != geom->getPartEnd() ; it++) {
		DFFGeometryPart* part = *it;

		if (renderPart  &&  renderPart != part) {
			continue;
		}

		indexLen += part->getIndexCount()*4;
		indexCounts[i] = part->getIndexCount();
		i++;
	}

	char* indexData = new char[indexLen];
	unsigned int indexOffs = 0;
	i = 0;

	glActiveTexture(GL_TEXTURE0);

	for (it = geom->getPartBegin() ; it != geom->getPartEnd() ; it++) {
		DFFGeometryPart* part = *it;

		if (renderPart  &&  part != renderPart) {
			continue;
		}

		DFFMaterial* mat = part->getMaterial();

		partHasTextures[i] = false;

		if (mat) {
			if (mat->getTextureCount() > 0) {
				DFFTexture* dffTex = mat->getTexture(0);

				char* lTexName = new char[strlen(dffTex->getDiffuseName())+1];
				strtolower(lTexName, dffTex->getDiffuseName());
				ResourceManager::hash_t texHash = res->hash(lTexName);

				if (!txdKnown) {
					res->findTextureArchive(texHash, &this->txdName);
				}

				GLuint texName = res->bindTexture(texHash, this->txdName);

				if (texName != 0) {
					partTextures[i] = texName;
					partHasTextures[i] = true;
				} else {
					printf("WARNING: Texture not found: %s (in %s)\n", dffTex->getDiffuseName(), txdName);
				}
			}
		}

		memcpy(indexData+indexOffs, part->getIndices(), part->getIndexCount()*4);
		indexOffs += part->getIndexCount()*4;
		i++;
	}

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexLen, indexData, GL_STATIC_DRAW);
	delete[] indexData;
}


void StaticObjectDefinition::draw()
{
	glBindBuffer(GL_ARRAY_BUFFER, dataBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);

	//glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix.toArray());

	ShaderProgram* program = ShaderProgram::getCurrent();
	GLint vertexAttrib = program->getAttributeLocation("Vertex");
	GLint normalAttrib = program->getAttributeLocation("Normal");
	GLint texCoordAttrib = program->getAttributeLocation("TexCoord");
	GLint textureUniform = program->getUniformLocation("Texture");
	GLint texturedUniform = program->getUniformLocation("Textured");
	GLint modelMatrixUniform = program->getUniformLocation("ModelMatrix");

	if (vertexAttrib != -1) {
		glEnableVertexAttribArray(vertexAttrib);
		glVertexAttribPointer(vertexAttrib, 3, GL_FLOAT, false, 0, (char*) 0);
	}

	bool normals = (flags & GEOMETRY_FLAG_NORMALS) != 0;
	bool texCoords = (flags & GEOMETRY_FLAG_TEXCOORDS) != 0;
	//bool colors = (flags & GEOMETRY_FLAG_COLORS) != 0;
	bool tristrips = (flags & GEOMETRY_FLAG_TRISTRIP) != 0;

	unsigned int dataOffs = vertexCount*3*4;

	if (normals) {
		if (normalAttrib != -1) {
			glEnableVertexAttribArray(normalAttrib);
			glVertexAttribPointer(normalAttrib, 3, GL_FLOAT, false, 0, ((char*) 0) + dataOffs);
		}
		dataOffs += vertexCount*3*4;
	}
	if (texCoords) {
		if (texCoordAttrib != -1) {
			glEnableVertexAttribArray(texCoordAttrib);
			glVertexAttribPointer(texCoordAttrib, 2, GL_FLOAT, false, 0, (char*) dataOffs);
		}
		dataOffs += vertexCount*2*4;
	}
	/*if (colors) {
		glEnableVertexAttribArray(colorAttrib);
		glVertexAttribPointer(colorAttrib, 4, GL_UNSIGNED_BYTE, false, 0, (char*) dataOffs);
		dataOffs += vertexCount*4;
	}*/

	unsigned int indexOffs = 0;
	for (int i = 0 ; i < partCount ; i++) {
		if (textureUniform != -1  &&  partHasTextures[i]) {
			glBindTexture(GL_TEXTURE_2D, partTextures[i]);
			glUniform1i(textureUniform, 0);
			glUniform1i(texturedUniform, 1);
		} else {
			glUniform1i(texturedUniform, 0);
		}

		glDrawElements(tristrips ? GL_TRIANGLE_STRIP : GL_TRIANGLES, indexCounts[i], GL_UNSIGNED_INT,
				(char*) 0 + indexOffs);
		indexOffs += indexCounts[i]*4;
	}

	GLException::checkError("Error drawing static object");
}
