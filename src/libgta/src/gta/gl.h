/*
 * gl.h
 *
 *  Created on: 07.03.2011
 *      Author: alemariusnexus
 */

#ifndef GL_H_
#define GL_H_

#include "config.h"

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

#endif /* GL_H_ */
