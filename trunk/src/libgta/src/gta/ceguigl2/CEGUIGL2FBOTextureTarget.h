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

#ifndef CEGUIGL2FBOTEXTURETARGET_H_
#define CEGUIGL2FBOTEXTURETARGET_H_

#include "CEGUIGL2TextureTarget.h"
#include "../gl.h"

using CEGUI::Size;


class CEGUIGL2Renderer;



class CEGUIGL2FBOTextureTarget : public CEGUIGL2TextureTarget {
public:
	CEGUIGL2FBOTextureTarget(CEGUIGL2Renderer* renderer);
	virtual void activate();
	virtual void deactivate();
	virtual void clear();
	virtual void declareRenderSize(const Size& size);
	virtual const char* getName() const { return "FBO Target"; }
	virtual void saveToMemory(void* data);

private:
	void generateFBO();

private:
	GLuint fbo;
};

#endif /* CEGUIGL2FBOTEXTURETARGET_H_ */
