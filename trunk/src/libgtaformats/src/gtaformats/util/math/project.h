/*
	Copyright 2010-2013 David "Alemarius Nexus" Lerch

	This file is part of gtaformats.

	gtaformats is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	gtaformats is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with gtaformats.  If not, see <http://www.gnu.org/licenses/>.

	Additional permissions are granted, which are listed in the file
	GPLADDITIONS.
 */

#ifndef PROJECT_H_
#define PROJECT_H_

#include "Matrix4.h"


bool Project(const Vector3& objPos, const Matrix4& modelMatrix, const Matrix4& projMatrix,
		int viewX, int viewY, int viewW, int viewH, Vector3& winPos);
bool UnProject(const Vector3& winPos, const Matrix4& modelMatrix, const Matrix4& projMatrix,
		int viewX, int viewY, int viewW, int viewH, Vector3& objPos);

#endif /* PROJECT_H_ */
