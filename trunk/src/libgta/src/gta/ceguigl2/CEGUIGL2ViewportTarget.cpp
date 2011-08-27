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

#include "CEGUIGL2ViewportTarget.h"
#include "CEGUIGL2Renderer.h"
#include "../gl.h"



CEGUIGL2ViewportTarget::CEGUIGL2ViewportTarget(CEGUIGL2Renderer* renderer)
		: CEGUIGL2RenderTarget(renderer)
{
	GLint vp[4];
	glGetIntegerv(GL_VIEWPORT, vp);
	setArea(Rect(vp[0], vp[1], vp[2], vp[3]));
}


CEGUIGL2ViewportTarget::CEGUIGL2ViewportTarget(CEGUIGL2Renderer* renderer, const Rect& area)
		: CEGUIGL2RenderTarget(renderer)
{
	setArea(area);
}
