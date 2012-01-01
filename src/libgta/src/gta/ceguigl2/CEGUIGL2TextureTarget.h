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

#ifndef CEGUIGL2TEXTURETARGET_H_
#define CEGUIGL2TEXTURETARGET_H_

#include "CEGUIGL2RenderTarget.h"
#include "CEGUIGL2Texture.h"
#include <CEGUI/CEGUI.h>

using CEGUI::Size;


class CEGUIGL2Renderer;


class CEGUIGL2TextureTarget : public CEGUIGL2RenderTarget, public CEGUI::TextureTarget {
public:
	CEGUIGL2TextureTarget(CEGUIGL2Renderer* renderer);
	virtual CEGUI::Texture& getTexture() const { return *tex; }
	virtual bool isImageryCache() const { return true; }
	virtual bool isRenderingInverted() const { return true; }
	virtual void saveToMemory(void* data) = 0;

protected:
	CEGUIGL2Texture* tex;
};

#endif /* CEGUIGL2TEXTURETARGET_H_ */
