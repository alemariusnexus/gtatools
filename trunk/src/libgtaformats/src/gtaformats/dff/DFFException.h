/*
	Copyright 2010 David Lerch

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

#include <exception>


class DFFException : std::exception {
public:
	enum ErrorCode {
		UnknownFileType,
		SyntaxError
	};

public:
	DFFException(ErrorCode code, const char* message) throw();
	virtual ~DFFException() throw();
	virtual const char* what() const throw() { return message; };
	ErrorCode getErrorCode() const throw() { return errCode; }

private:
	char* message;
	ErrorCode errCode;
};

#endif /* DFFEXCEPTION_H_ */
