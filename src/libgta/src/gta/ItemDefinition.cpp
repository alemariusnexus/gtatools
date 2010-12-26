/*
	Copyright 2010 David "Alemarius Nexus" Lerch

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
 */

#include "ItemDefinition.h"
#include <GL/glew.h>
#include "ShaderProgram.h"
#include "ResourceManager.h"
#include "Submesh.h"
#include <cstring>
#include <gtaformats/util/strutil.h>
#include "GLException.h"
#include "ManagedMeshPointer.h"
#include "ManagedTextureSource.h"



ItemDefinition::ItemDefinition(MeshPointer* meshPtr, TextureSource* texSrc, float drawDist)
		: meshPtr(meshPtr), texSrc(texSrc), drawDistance(drawDist)
{
}


ItemDefinition::ItemDefinition(const IDEStaticObject& object)
		: drawDistance(object.getDrawDistances()[0])
{
	char* lMeshName = new char[strlen(object.getModelName())+1];
	char* lTexName = new char[strlen(object.getTextureName())+1];
	strtolower(lMeshName, object.getModelName());
	strtolower(lTexName, object.getTextureName());
	meshPtr = new ManagedMeshPointer(Hash(lMeshName));
	texSrc = new ManagedTextureSource(Hash(lTexName));
	//meshHash = Hash(lMeshName);
	//txdHash = Hash(lTexName);
	delete[] lMeshName;
	delete[] lTexName;
}


ItemDefinition::~ItemDefinition()
{
	delete meshPtr;
	delete texSrc;
}


void ItemDefinition::render()
{
	Engine* engine = Engine::getInstance();
	ShaderProgram* program = engine->getCurrentShaderProgram();
	ResourceManager* resMgr = engine->getResourceManager();

	//Mesh* mesh = getMesh();
	Mesh* mesh = **meshPtr;

	//printf("Rendering\n");

	if (mesh) {
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

			if (mat) {
				uint8_t r, g, b, a;
				mat->getColor(r, g, b, a);
				glUniform4f(materialColorUniform, (1.0f/255)*r, (1.0f/255)*g, (1.0f/255)*b, (1.0f/255)*a);

				if (textureUniform != -1  &&  mat->isTextured()) {
					glActiveTexture(GL_TEXTURE0);
					//TextureIndex index(txdHash, mat->getTextureHash());
					//resMgr->bindTexture(index);
					glBindTexture(GL_TEXTURE_2D, texSrc->getTexture(mat->getTextureHash()));
					glUniform1i(textureUniform, 0);
					glUniform1i(texturedUniform, 1);
				} else if (texturedUniform) {
					glUniform1i(texturedUniform, 0);
				}
			} else {
				if (texturedUniform) {
					glUniform1i(texturedUniform, 0);
				}

				glUniform4f(materialColorUniform, 1.0f, 1.0f, 1.0f, 0.0f);
			}

			/*if (textureUniform != -1  &&  mat  &&  mat->isTextured()) {
				glActiveTexture(GL_TEXTURE0);
				//TextureIndex index(txdHash, mat->getTextureHash());
				//resMgr->bindTexture(index);
				glBindTexture(GL_TEXTURE_2D, texSrc->getTexture(mat->getTextureHash()));
				glUniform1i(textureUniform, 0);
				glUniform1i(texturedUniform, 1);
			} else if (mat) {
				uint8_t r, g, b, a;
				mat->getColor(r, g, b, a);
				glUniform1i(texturedUniform, 0);
				glUniform4f(materialColorUniform, (1.0f/255)*r, (1.0f/255)*g, (1.0f/255)*b, (1.0f/255)*a);
			} else {
				glUniform1i(texturedUniform, 0);
				glUniform4f(materialColorUniform, 1.0f, 1.0f, 1.0f, 1.0f);
			}*/

			submesh->bindIndexBuffer();

			GLException::checkError();

			glDrawElements((mesh->getFlags() & MeshTriangleStrips) != 0 ? GL_TRIANGLE_STRIP : GL_TRIANGLES,
					submesh->getIndexCount(), GL_UNSIGNED_INT, (char*) 0);
		}
	}
}
