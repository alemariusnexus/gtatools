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

#include "GLException.h"
#include "gl.h"
#include <cstring>
#include <cstdio>



void GLException::checkError(const char* msg)
{
	GLenum error = glGetError();

	if (error != GL_NO_ERROR) {
#ifdef GTA_USE_OPENGL_ES
		char errname[32];

		switch (error) {
		case GL_INVALID_ENUM:
			sprintf(errname, "GL_INVALID_ENUM");
			break;
		case GL_INVALID_VALUE:
			sprintf(errname, "GL_INVALID_VALUE");
			break;
		case GL_INVALID_OPERATION:
			sprintf(errname, "GL_INVALID_OPERATION");
			break;
		case GL_INVALID_FRAMEBUFFER_OPERATION:
			sprintf(errname, "GL_INVALID_FRAMEBUFFER_OPERATION");
			break;
		case GL_OUT_OF_MEMORY:
			sprintf(errname, "GL_OUT_OF_MEMORY");
			break;
		default:
			sprintf(errname, "[UNKNOWN: 0x%X]", error);
		}

		char* errmsg = new char[128+strlen(msg)];
		sprintf(errmsg, "OpenGL error %s (%u) [%s]", errname, error, msg);
		GLException ex(errmsg, __FILE__, __LINE__);
		delete[] errmsg;
		throw ex;
#else
		char errname[32];
   
		switch (error) {
		case GL_INVALID_ENUM:
			sprintf(errname, "invalid enumerant");
			break;
		case GL_INVALID_VALUE:
			sprintf(errname, "invalid value");
			break;
		case GL_INVALID_OPERATION:
			sprintf(errname, "invalid operation");
			break;
		case GL_STACK_OVERFLOW:
			sprintf(errname, "stack overflow");
			break;
		case GL_STACK_UNDERFLOW:
			sprintf(errname, "stack underflow");
			break;
		case GL_OUT_OF_MEMORY:
			sprintf(errname, "out of memory");
			break;
		case GL_TABLE_TOO_LARGE:
			sprintf(errname, "table too large");
			break;
		default:
			sprintf(errname, "[UNKNOWN: 0x%X]", error);
		}
		

		char* errmsg;

		if (msg) {
			errmsg = new char[strlen(errname) + strlen(msg) + 64];
			strcpy(errmsg, msg);
			strcat(errmsg, ": ");
		} else {
			errmsg = new char[strlen(errname) + 64];
			errmsg[0] = '\0';
		}

		strcat(errmsg, "OpenGL error: ");
		strcat(errmsg, errname);

		GLException ex(errmsg, __FILE__, __LINE__);
		delete[] errmsg;
		throw ex;
#endif
	}
}
