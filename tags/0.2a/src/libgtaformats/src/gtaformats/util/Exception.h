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

#ifndef EXCEPTION_H_
#define EXCEPTION_H_

#include <exception>
#include <cstdlib>

#ifdef linux
#include <execinfo.h>
#endif


using std::exception;


class Exception : public exception {
public:
	Exception(const char* message, const char* srcFile = NULL, int srcLine = -1,
			Exception* nestedException = NULL, const char* exceptionName = "Exception");
	Exception(const Exception& ex);
	virtual ~Exception() throw();
	virtual const char* what() const throw() { return fullMessage; };
	const char* getMessage() const throw() { return message; }
	const char* getBacktrace() const throw();

protected:
	void setMessage(const char* message);

private:
	char* buildFullMessage() const throw();

private:
#ifdef linux
	char* backTrace;
#endif

	Exception* nestedException;
	const char* exceptionName;
	char* message;
	char* fullMessage;
	const char* srcFile;
	int srcLine;
};

#endif /* EXCEPTION_H_ */


