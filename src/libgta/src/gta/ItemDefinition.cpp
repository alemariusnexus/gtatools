/*
 * ItemDefinition.cpp
 *
 *  Created on: 21.10.2010
 *      Author: alemariusnexus
 */

#include "ItemDefinition.h"
#include <GL/glew.h>
#include "ShaderProgram.h"
#include "ResourceManager.h"
#include "Submesh.h"
#include <cstring>
#include <gtaformats/util/util.h>
#include "GLException.h"



ItemDefinition::ItemDefinition(hash_t meshHash, hash_t txdHash, float drawDist)
		: meshHash(meshHash), txdHash(txdHash), drawDistance(drawDist)
{
}


ItemDefinition::ItemDefinition(const IDEStaticObject& object)
		: drawDistance(object.getDrawDistances()[0])
{
	char* lMeshName = new char[strlen(object.getModelName())+1];
	char* lTexName = new char[strlen(object.getTextureName())+1];
	strtolower(lMeshName, object.getModelName());
	strtolower(lTexName, object.getTextureName());
	meshHash = Hash(lMeshName);
	txdHash = Hash(lTexName);
	delete[] lMeshName;
	delete[] lTexName;
}


Mesh* ItemDefinition::getMesh()
{
	Engine* engine = Engine::getInstance();
	ResourceManager* resMgr = engine->getResourceManager();
	Mesh* mesh;
	resMgr->getMesh(meshHash, mesh);
	return mesh;
}


void ItemDefinition::render()
{
	Engine* engine = Engine::getInstance();
	ShaderProgram* program = engine->getCurrentShaderProgram();
	ResourceManager* resMgr = engine->getResourceManager();

	Mesh* mesh = getMesh();

	GLint vertexAttrib = program->getAttributeLocation("Vertex");
	GLint normalAttrib = program->getAttributeLocation("Normal");
	GLint texCoordAttrib = program->getAttributeLocation("TexCoord");
	GLint colorAttrib = program->getAttributeLocation("Color");
	GLint textureUniform = program->getUniformLocation("Texture");
	GLint texturedUniform = program->getUniformLocation("Textured");
	GLint materialColorUniform = program->getUniformLocation("MaterialColor");
	GLint vertexColorsUniform = program->getUniformLocation("VertexColors");

	glUniform1i(vertexColorsUniform, (mesh->getFlags() & MeshVertexColors) != 0 ? 1 : 0);

	mesh->bindDataBuffer();

	if (vertexAttrib != -1) {
		int offset = mesh->getVertexOffset();
		glEnableVertexAttribArray(vertexAttrib);
		glVertexAttribPointer(vertexAttrib, 3, GL_FLOAT, GL_FALSE, 0, (char*) offset);
	}
	if (normalAttrib != -1) {
		int offset = mesh->getNormalOffset();

		if (offset != -1) {
			glEnableVertexAttribArray(normalAttrib);
			glVertexAttribPointer(normalAttrib, 3, GL_FLOAT, GL_FALSE, 0, (char*) offset);
		}
	}
	if (texCoordAttrib != -1) {
		int offset = mesh->getTexCoordOffset();

		if (offset != -1) {
			glEnableVertexAttribArray(texCoordAttrib);
			glVertexAttribPointer(texCoordAttrib, 2, GL_FLOAT, GL_FALSE, 0, (char*) offset);
		}
	}
	if (colorAttrib != -1) {
		int offset = mesh->getVertexColorOffset();

		if (offset != -1) {
			glEnableVertexAttribArray(colorAttrib);
			glVertexAttribPointer(colorAttrib, 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, (char*) offset);
		}
	}

	Mesh::SubmeshIterator it;

	for (it = mesh->getSubmeshBegin() ; it != mesh->getSubmeshEnd() ; it++) {
		Submesh* submesh = *it;
		Material* mat = submesh->getMaterial();

		if (textureUniform != -1  &&  mat  &&  mat->isTextured()) {
			glActiveTexture(GL_TEXTURE0);
			TextureIndex index(txdHash, mat->getTextureHash());
			resMgr->bindTexture(index);
			glUniform1i(textureUniform, 0);
			glUniform1i(texturedUniform, 1);
		} else {
			uint8_t r, g, b, a;
			mat->getColor(r, g, b, a);
			glUniform1i(texturedUniform, 0);
			glUniform4f(materialColorUniform, (1.0f/255)*r, (1.0f/255)*g, (1.0f/255)*b, (1.0f/255)*a);
		}

		submesh->bindIndexBuffer();

		GLException::checkError();

		glDrawElements((mesh->getFlags() & MeshTriangleStrips) != 0 ? GL_TRIANGLE_STRIP : GL_TRIANGLES,
				submesh->getIndexCount(), GL_UNSIGNED_INT, (char*) 0);
	}
}
