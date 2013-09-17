/*
	Copyright 2010-2013 David "Alemarius Nexus" Lerch

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

	Additional permissions are granted, which are listed in the file
	GPLADDITIONS.
 */

#ifndef TXDEXCEPTION_H_
#define TXDEXCEPTION_H_

#include <gtaformats/config.h>
#include "../util/Exception.h"


/**	\brief This exception can be thrown by TXDArchive.
 */
class TXDException : public Exception {
public:

public:
	/**	\brief Creates a new exception.
	 *
	 *	@param errCode The error code.
	 *	@param msg The error message.
	 *	@param offset The offset at which the error happened (optional).
	 */
	TXDException(const char* message, const char* srcFile = NULL, int srcLine = -1, Exception* nestedException = NULL) throw();
	TXDException(const TXDException& ex) : Exception(ex) {}
};


#endif /* TXDEXCEPTION_H_ */
