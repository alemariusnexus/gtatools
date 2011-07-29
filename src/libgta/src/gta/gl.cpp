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

#include "gl.h"
#include "Engine.h"
#include <set>
#include <cstring>
#include <cstdio>
#include "GLException.h"

using std::set;



#ifdef GTA_USE_OPENGL_ES

set<hash_t> supportedExtensions;

#endif

int majorVersion, minorVersion;
bool gtaglInitialized = false;


void gtaglInit()
{
	if (gtaglInitialized)
		return;

	const char* verStr = (const char*) glGetString(GL_VERSION);

	GLException::checkError();

#ifdef GTA_USE_OPENGL_ES
	const char* extStr = (const char*) glGetString(GL_EXTENSIONS);
	char* extStrCpy = new char[strlen(extStr)+1];
	strcpy(extStrCpy, extStr);

	char* ext = strtok(extStrCpy, " ");
	while (ext) {
		supportedExtensions.insert(Hash(ext));
		ext = strtok(NULL, " ");
	}
	
	sscanf(verStr, "OpenGL ES %d.%d", &majorVersion, &minorVersion);
#else
	glewInit();
	sscanf(verStr, "%d.%d", &majorVersion, &minorVersion);
#endif

	gtaglInitialized = true;
}


bool gtaglIsExtensionSupported(const char* extension)
{
#ifdef GTA_USE_OPENGL_ES
	return supportedExtensions.find(Hash(extension)) != supportedExtensions.end();
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
