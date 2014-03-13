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

#ifndef CEGUIGL2VIEWPORTTARGET_H_
#define CEGUIGL2VIEWPORTTARGET_H_

#include "CEGUIGL2RenderTarget.h"

using CEGUI::Rect;


class CEGUIGL2Renderer;


class CEGUIGL2ViewportTarget : public CEGUIGL2RenderTarget {
public:
	CEGUIGL2ViewportTarget(CEGUIGL2Renderer* renderer);
	CEGUIGL2ViewportTarget(CEGUIGL2Renderer* renderer, const Rect& area);
	virtual bool isImageryCache() const { return false; }
	virtual const char* getName() const { return "Viewport Target"; }
};

#endif /* CEGUIGL2VIEWPORTTARGET_H_ */
