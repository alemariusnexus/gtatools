/*
	Copyright 2010 David "Alemarius Nexus" Lerch

	This file is part of gtaformats.

	gtaformats is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	gtaformats is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with gtaformats.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef DFFEXCEPTION_H_
#define DFFEXCEPTION_H_

#include "../util/Exception.h"


class DFFException : public Exception {
public:
	DFFException(const char* msg, const char* srcFile = NULL, int srcLine = -1, Exception* nestedException = NULL) throw()
			: Exception(msg, srcFile, srcLine, nestedException, "DFFException") {}
	DFFException(const DFFException& ex) throw() : Exception(ex) {}
};

#endif /* DFFEXCEPTION_H_ */