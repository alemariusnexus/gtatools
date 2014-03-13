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

#ifndef GLDEBUG_H_
#define GLDEBUG_H_

#include <gta/config.h>
#include "gl.h"
#include <nxcommon/file/File.h>
#include <ostream>

using std::ostream;

enum PixelSaveFormat
{
	PixelSaveFormatRGBA,
	PixelSaveFormatRGB
};

void GLDebugInit();

void GLDebugSavePixels(ostream* out, int x, int y, int w, int h, PixelSaveFormat format = PixelSaveFormatRGB);
void GLDebugSavePixels(ostream* out, PixelSaveFormat format = PixelSaveFormatRGB);
void GLDebugSavePixels(const File& file, int x, int y, int w, int h, PixelSaveFormat format = PixelSaveFormatRGB);
void GLDebugSavePixels(const File& file, PixelSaveFormat format = PixelSaveFormatRGB);

void GLDebugReadDepthTexture(float* buf, GLenum target = GL_TEXTURE_2D);
float* GLDebugReadDepthTexture(GLenum target = GL_TEXTURE_2D);
void GLDebugSaveDepthTexture(ostream* out, GLenum target = GL_TEXTURE_2D);
void GLDebugSaveDepthTexture(const File& file, GLenum target = GL_TEXTURE_2D);

#endif /* GLDEBUG_H_ */
