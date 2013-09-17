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

#ifndef EXCEPTION_H_
#define EXCEPTION_H_

#include <gtaformats/config.h>
#include <exception>
#include <cstdlib>

#ifdef linux
#include <execinfo.h>
#endif


using std::exception;


class Exception : public exception {
public:
	Exception(const char* message, const char* srcFile = NULL, int srcLine = -1,
			const Exception* nestedException = NULL, const char* exceptionName = "Exception");
	Exception(const Exception& ex);
	virtual ~Exception() throw();
	virtual const char* what() const throw() { return fullMessage; };
	virtual Exception* copy() const throw() { return new Exception(*this); }
	const char* getMessage() const throw() { return message; }
	const char* getBacktrace() const throw();
	const char* getName() const throw() { return exceptionName; }

protected:
	void setMessage(const char* message);

private:
	char* buildFullMessage() const throw();

private:
#ifdef linux
	char* backTrace;
#endif

	const char* exceptionName;
	char* message;
	char* fullMessage;
	const char* srcFile;
	int srcLine;
	Exception* nestedException;
};

#endif /* EXCEPTION_H_ */


