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

#ifndef RENDERING_H_
#define RENDERING_H_

#include "../gl.h"


enum RenderingPrimitiveFormat
{
	RenderingPrimitivePoints = GL_POINTS,
	RenderingPrimitiveLines = GL_LINES,
	RenderingPrimitiveTriangles = GL_TRIANGLES,
	RenderingPrimitiveTriangleStrip = GL_TRIANGLE_STRIP,
};


#endif /* RENDERING_H_ */
