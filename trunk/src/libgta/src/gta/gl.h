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

#ifndef GL_H_
#define GL_H_

#include <gta/config.h>

#ifdef GTA_USE_OPENGL_ES

#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#else

#include <GL/glew.h>

#endif



void gtaglInit();
bool gtaglIsExtensionSupported(const char* extension);
bool gtaglIsVersionSupported(int major, int minor);
void gtaglGetVersion(int& major, int& minor);
const char* gtaglGetSupportedExtensions();

#endif /* GL_H_ */
