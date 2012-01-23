/*
	Copyright 2010-2012 David "Alemarius Nexus" Lerch

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

#include "StaticSceneObject.h"
#include "../gl.h"
#include "../resource/ResourceCache.h"
#include "SceneObjectDefinitionInfo.h"



// TODO When do we delete defInfo?


StaticSceneObject::StaticSceneObject(MapItemDefinition* def, const Matrix4& modelMatrix,
			StaticSceneObject* lodParent)
		: def(def), defInfo(NULL)
{
	this->modelMatrix = modelMatrix;
	this->lodParent = lodParent;

	setHasAlphaTransparency(def->hasAlphaTransparency());
}


StaticSceneObject::StaticSceneObject(const StaticSceneObject& other)
		: def(other.def), defInfo(other.defInfo ? new SceneObjectDefinitionInfo(*other.defInfo) : NULL)
{
	this->modelMatrix = other.modelMatrix;
	this->lodParent = other.lodParent;

	setHasAlphaTransparency(other.hasAlphaTransparency());
}


/*void StaticSceneObject::render(Matrix4& vpMatrix, GLuint mvpMatrixUniform)
{


	if (Engine::getInstance()->isDrawingEnabled()) {
		def->render();
	}

	mat = vpMatrix;
	matData = mat.toArray();
	glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, matData);

	if (def->getCollisionShapePointer()) {
		btCollisionShape* shape = **def->getCollisionShapePointer();
		rigidBody->setCollisionShape(shape);
	}
}*/
