/*
 * GLException.cpp
 *
 *  Created on: 23.09.2010
 *      Author: alemariusnexus
 */

#include "GLException.h"
#include <GL/glew.h>
#include <cstring>



void GLException::checkError(const char* msg)
{
	GLenum error = glGetError();

	if (error != GL_NO_ERROR) {
		const GLchar* errstr = (const GLchar*) gluErrorString(error);

		char* errmsg;

		if (msg) {
			errmsg = new char[strlen(errstr) + strlen(msg) + 64];
			strcpy(errmsg, msg);
			strcat(errmsg, ": ");
		} else {
			errmsg = new char[strlen(errstr) + 64];
			errmsg[0] = '\0';
		}

		strcat(errmsg, "OpenGL error: ");
		strcat(errmsg, errstr);

		GLException ex(errmsg, __FILE__, __LINE__);
		delete[] errmsg;
		throw ex;
	}
}
