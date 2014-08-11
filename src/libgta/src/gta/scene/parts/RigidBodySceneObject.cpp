/*
	Copyright 2010-2014 David "Alemarius Nexus" Lerch

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

#include "RigidBodySceneObject.h"
#include "../objects/SimpleDynamicSceneObject.h"




void RigidBodySceneObject::getWorldTransform(btTransform& trans) const
{
	const SimpleDynamicSceneObject* sdObj = dynamic_cast<const SimpleDynamicSceneObject*>(this);

	const Matrix4& mm = getModelMatrix();

	trans.setFromOpenGLMatrix((const float*) &mm);

	/*if (getStreamingDistance() == 503.456f) {
		btVector3 p = trans.getOrigin();
		printf("Fetching transform %p: %f, %f, %f\n", this, p.x(), p.y(), p.z());
	}*/
}


void RigidBodySceneObject::setWorldTransform(const btTransform& trans)
{
	Matrix4 mat = Matrix4::Identity;
	trans.getOpenGLMatrix((float*) &mat);
	setModelMatrix(mat);

	/*if (getStreamingDistance() == 503.456f) {
		btVector3 p = trans.getOrigin();
		const float* a = mat.toArray();
		printf("Setting to %p: %f, %f, %f (that's %f %f %f)\n", this, p.x(), p.y(), p.z(), a[12], a[13], a[14]);
	}*/
}
