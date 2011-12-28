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

#define GL_CPP

#include "gl.h"
#include "Engine.h"
#include <set>
#include <cstring>
#include <cstdio>
#include "GLException.h"
#include <gtaformats/util/StringComparator.h>
#include <boost/shared_array.hpp>

using std::set;
using boost::shared_array;




#ifdef GTA_USE_OPENGL_ES

set<const char*, StringComparator> supportedExtensions;

#endif

int majorVersion, minorVersion;
bool gtaglInitialized = false;


#ifndef GTA_USE_OPENGL_ES

inline void gtaglGenFramebuffersCORE(GLsizei p1, GLuint* p2) { glGenFramebuffers(p1, p2); }
inline void gtaglBindFramebufferCORE(GLenum p1, GLuint p2) { glBindFramebuffer(p1, p2); }
inline void gtaglFramebufferTexture2DCORE(GLenum p1, GLenum p2, GLenum p3, GLuint p4, GLint p5)
		{ glFramebufferTexture2D(p1, p2, p3, p4, p5); }

inline void gtaglGenFramebuffersEXT(GLsizei p1, GLuint* p2) { glGenFramebuffersEXT(p1, p2); }
inline void gtaglBindFramebufferEXT(GLenum p1, GLuint p2) { glBindFramebufferEXT(p1, p2); }
inline void gtaglFramebufferTexture2DEXT(GLenum p1, GLenum p2, GLenum p3, GLuint p4, GLint p5)
		{ glFramebufferTexture2DEXT(p1, p2, p3, p4, p5); }

#endif




void gtaglInit()
{
	if (gtaglInitialized)
		return;

	const char* verStr = (const char*) glGetString(GL_VERSION);

	GLException::checkError();

#ifdef GTA_USE_OPENGL_ES
	const char* extStr = (const char*) glGetString(GL_EXTENSIONS);
	GLException::checkError();
	char* extStrCpy = new char[strlen(extStr)+1];
	strcpy(extStrCpy, extStr);

	char* ext = strtok(extStrCpy, " ");
	while (ext) {
		char* extCpy = new char[strlen(ext)+1];
		strcpy(extCpy, ext);
		supportedExtensions.insert(ext);
		ext = strtok(NULL, " ");
	}
	
	sscanf(verStr, "OpenGL ES %d.%d", &majorVersion, &minorVersion);
#else
	glewInit();
	sscanf(verStr, "%d.%d", &majorVersion, &minorVersion);
#endif

#ifndef GTA_USE_OPENGL_ES
	if (gtaglIsVersionSupported(3, 0)) {
		gtaglGenFramebuffers = &gtaglGenFramebuffersCORE;
		gtaglBindFramebuffer = &gtaglBindFramebufferCORE;
		gtaglFramebufferTexture2D = &gtaglFramebufferTexture2DCORE;

		GTAGL_FRAMEBUFFER = GL_FRAMEBUFFER;
		GTAGL_COLOR_ATTACHMENT0 = GL_COLOR_ATTACHMENT0;
		GTAGL_COLOR_ATTACHMENT1 = GL_COLOR_ATTACHMENT1;
		GTAGL_COLOR_ATTACHMENT2 = GL_COLOR_ATTACHMENT2;
		GTAGL_COLOR_ATTACHMENT3 = GL_COLOR_ATTACHMENT3;
		GTAGL_DEPTH_ATTACHMENT = GL_DEPTH_ATTACHMENT;
	} else {
		gtaglGenFramebuffers = &gtaglGenFramebuffersEXT;
		gtaglBindFramebuffer = &gtaglBindFramebufferEXT;
		gtaglFramebufferTexture2D = &gtaglFramebufferTexture2DEXT;

		GTAGL_FRAMEBUFFER = GL_FRAMEBUFFER_EXT;
		GTAGL_COLOR_ATTACHMENT0 = GL_COLOR_ATTACHMENT0_EXT;
		GTAGL_COLOR_ATTACHMENT1 = GL_COLOR_ATTACHMENT1_EXT;
		GTAGL_COLOR_ATTACHMENT2 = GL_COLOR_ATTACHMENT2_EXT;
		GTAGL_COLOR_ATTACHMENT3 = GL_COLOR_ATTACHMENT3_EXT;
		GTAGL_DEPTH_ATTACHMENT = GL_DEPTH_ATTACHMENT_EXT;
	}
#else
	gtaglGenFramebuffers = &glGenFramebuffers;
	gtaglBindFramebuffer = &glBindFramebuffer;
	gtaglFramebufferTexture2D = &glFramebufferTexture2D;

	GTAGL_FRAMEBUFFER = GL_FRAMEBUFFER;
	GTAGL_COLOR_ATTACHMENT0 = GL_COLOR_ATTACHMENT0;
	GTAGL_COLOR_ATTACHMENT1 = GL_COLOR_ATTACHMENT0;
	GTAGL_COLOR_ATTACHMENT2 = GL_COLOR_ATTACHMENT0;
	GTAGL_COLOR_ATTACHMENT3 = GL_COLOR_ATTACHMENT0;
	GTAGL_DEPTH_ATTACHMENT = GL_DEPTH_ATTACHMENT;
#endif

	gtaglInitialized = true;
}


bool gtaglIsExtensionSupported(const char* extension)
{
#ifdef GTA_USE_OPENGL_ES
	return supportedExtensions.find(extension) != supportedExtensions.end();
#else
	return glewIsSupported(extension);
#endif
}


bool gtaglIsVersionSupported(int major, int minor)
{
	return majorVersion > major  ||  (majorVersion == major  &&  minorVersion >= minor);
}


void gtaglGetVersion(int& major, int& minor)
{
	major = majorVersion;
	minor = minorVersion;
}


const char* gtaglGetSupportedExtensions()
{
	return (const char*) glGetString(GL_EXTENSIONS);
}



#undef GL_CPP
