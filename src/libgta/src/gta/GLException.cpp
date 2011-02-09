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
