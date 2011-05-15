/*
	Copyright 2010-2011 David "Alemarius Nexus" Lerch

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

#include "ItemDefinition.h"
#include "gl.h"
#include "ShaderProgram.h"
#include "Submesh.h"
#include <cstring>
#include <gtaformats/util/strutil.h>
#include "GLException.h"
#include "ManagedMeshPointer.h"
#include "StaticMeshPointer.h"
#include "ManagedTextureSource.h"
#include "ManagedCollisionShapePointer.h"
#include "resource/mesh/MeshCacheEntry.h"
#include "MeshGenerator.h"



ItemDefinition::ItemDefinition(MeshPointer* meshPtr, TextureSource* texSrc, float drawDist)
		: meshPtr(meshPtr), texSrc(texSrc), colShapePtr(NULL), drawDistance(drawDist),
		  drawDistanceSquarred(drawDist*drawDist)
{
	initShaderLocations();
}


ItemDefinition::ItemDefinition(const IDEStaticObject& object)
		: drawDistance(object.getDrawDistances()[0]), drawDistanceSquarred(drawDistance*drawDistance)
{
	initShaderLocations();
	char* lMeshName = new char[strlen(object.getModelName())+1];
	char* lTexName = new char[strlen(object.getTextureName())+1];
	strtolower(lMeshName, object.getModelName());
	strtolower(lTexName, object.getTextureName());
	meshPtr = new ManagedMeshPointer(Hash(lMeshName));
	/*MeshGenerator gen;
	Mesh* mesh = gen.createSphere(10.0f, 5, 5);
	meshPtr = new StaticMeshPointer(mesh);*/
	texSrc = new ManagedTextureSource(Hash(lTexName));
	//colShapePtr = new ManagedCollisionShapePointer(Hash(lMeshName));
	colShapePtr = new ManagedCollisionShapePointer(lMeshName);
	delete[] lMeshName;
	delete[] lTexName;
}


ItemDefinition::~ItemDefinition()
{
	if (meshPtr)
		delete meshPtr;
	if (texSrc)
		delete texSrc;
	if (colShapePtr)
		delete colShapePtr;
}


void ItemDefinition::initShaderLocations()
{
}


void ItemDefinition::render()
{
	Engine* engine = Engine::getInstance();
	ShaderProgram* program = engine->getCurrentShaderProgram();

	Mesh* mesh = **meshPtr;

	if (mesh) {
		GLException::checkError();

		vertexAttrib = program->getAttributeLocation("Vertex");
		normalAttrib = program->getAttributeLocation("Normal");
		texCoordAttrib = program->getAttributeLocation("TexCoord");
		colorAttrib = program->getAttributeLocation("Color");
		textureUniform = program->getUniformLocation("Texture");
		texturedUniform = program->getUniformLocation("Textured");
		materialColorUniform = program->getUniformLocation("MaterialColor");
		vertexColorsUniform = program->getUniformLocation("VertexColors");

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
			} else {
				glDisableVertexAttribArray(normalAttrib);
			}
		}
		if (texCoordAttrib != -1) {
			int offset = mesh->getTexCoordOffset();

			if (offset != -1) {
				glEnableVertexAttribArray(texCoordAttrib);
				glVertexAttribPointer(texCoordAttrib, 2, GL_FLOAT, GL_FALSE, 0, (char*) offset);
			} else {
				glDisableVertexAttribArray(texCoordAttrib);
			}
		}
		if (colorAttrib != -1) {
			int offset = mesh->getVertexColorOffset();

			if (offset != -1) {
				glEnableVertexAttribArray(colorAttrib);
				glVertexAttribPointer(colorAttrib, 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, (char*) offset);
			} else {
				glDisableVertexAttribArray(colorAttrib);
			}
		}

		GLException::checkError();

		Mesh::SubmeshIterator it;

		for (it = mesh->getSubmeshBegin() ; it != mesh->getSubmeshEnd() ; it++) {
			Submesh* submesh = *it;
			Material* mat = submesh->getMaterial();

			GLException::checkError();

			if (mat) {
				uint8_t r, g, b, a;
				mat->getColor(r, g, b, a);
				glUniform4f(materialColorUniform, (1.0f/255)*r, (1.0f/255)*g, (1.0f/255)*b, (1.0f/255)*a);

				if (textureUniform != -1  &&  mat->isTextured()  &&  texSrc) {
					glActiveTexture(GL_TEXTURE0);
					hash_t texHash = mat->getTextureHash();
					GLuint tex = texSrc->getTexture(texHash);
					glBindTexture(GL_TEXTURE_2D, tex);
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

			GLException::checkError();

			submesh->bindIndexBuffer();

			GLException::checkError();

			GLenum mode;

			switch (mesh->getVertexFormat()) {
			case VertexFormatPoints:
				mode = GL_POINTS;
				break;

			case VertexFormatLines:
				mode = GL_LINES;
				break;

			case VertexFormatTriangles:
				mode = GL_TRIANGLES;
				break;

			case VertexFormatTriangleStrips:
				mode = GL_TRIANGLE_STRIP;
				break;

			default:
				assert(false);
			}

			glDrawElements(mode, submesh->getIndexCount(), GL_UNSIGNED_INT, (char*) 0);
		}
	}
}
