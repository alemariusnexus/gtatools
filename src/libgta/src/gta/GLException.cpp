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

#include "GLException.h"
#include "gl.h"
#include <cstring>
#include <cstdio>
#include "Engine.h"



void GLException::checkError(const CString& msg)
{
	return;
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

		printf("Buffer memory before exception: %u bytes\n", Engine::getInstance()->getTestMem());

		char* errmsg = new char[128+strlen(msg)];
		sprintf(errmsg, "OpenGL error %s (%u) [%s]", errname, error, msg.get());
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

		if (!msg.isNull()) {
			errmsg = new char[strlen(errname) + strlen(msg.get()) + 64];
			strcpy(errmsg, msg.get());
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


void GLException::checkFramebufferStatus(GLenum target, const CString& msg)
{
	GLenum status;

#ifndef GTA_USE_OPENGL_ES
	if (gtaglIsVersionSupported(3, 0)) {
		status = glCheckFramebufferStatus(target);
	} else {
		status = glCheckFramebufferStatusEXT(target);
	}
#else
	status = glCheckFramebufferStatus(target);
#endif

	if (status != GL_FRAMEBUFFER_COMPLETE) {
		char* errmsg = new char[256 + strlen(msg.get())];
		strcpy(errmsg, "Error: OpenGL Framebuffer is incomplete. glCheckFramebufferStatus returned ");

		switch (status) {
		case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
			strcat(errmsg, "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT");
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
			strcat(errmsg, "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT");
			break;
		case GL_FRAMEBUFFER_UNSUPPORTED:
			strcat(errmsg, "GL_FRAMEBUFFER_UNSUPPORTED");
			break;
#ifndef GTA_USE_OPENGL_ES
		case GL_FRAMEBUFFER_UNDEFINED:
			strcat(errmsg, "GL_FRAMEBUFFER_UNDEFINED");
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
			strcat(errmsg, "GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER");
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
			strcat(errmsg, "GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER");
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
			strcat(errmsg, "GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE");
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
			strcat(errmsg, "GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS");
			break;
#else
		case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS:
			strcat(errmsg, "GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS");
			break;
#endif
		default:
			char submsg[64];
			sprintf(submsg, "[UNKNOWN: 0x%X]", status);
			strcat(errmsg, submsg);
		}

		char* submsg = new char[8 + strlen(msg.get())];
		sprintf(submsg, " [%s]!", msg.get());
		strcat(errmsg, submsg);
		delete[] submsg;

		GLException ex(errmsg, __FILE__, __LINE__);
		delete[] errmsg;

		throw ex;
	}
}
