/*
 * gl.cpp
 *
 *  Created on: 07.03.2011
 *      Author: alemariusnexus
 */

#include "gl.h"
#include "Engine.h"
#include <set>
#include <cstring>
#include <cstdio>

using std::set;



#ifdef GTA_USE_OPENGL_ES

set<hash_t> supportedExtensions;

#endif

int majorVersion, minorVersion;


void gtaglInit()
{
	const char* verStr = (const char*) glGetString(GL_VERSION);

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
	return majorVersion >= major  &&  minorVersion >= minor;
}


void gtaglGetVersion(int& major, int& minor)
{
	major = majorVersion;
	minor = minorVersion;
}
