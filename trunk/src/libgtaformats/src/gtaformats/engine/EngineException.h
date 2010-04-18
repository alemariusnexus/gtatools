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

#ifndef ENGINEEXCEPTION_H_
#define ENGINEEXCEPTION_H_

#include <exception>

using std::exception;


class EngineException : public exception {
public:
	enum ErrorCode {
		NoSuchTexture,
		NoSuchMesh,
		InvalidArgument
	};

public:
	EngineException(ErrorCode code, const char* errmsg) throw();
	virtual ~EngineException() throw();
	virtual const char* what() const throw() { return errmsg; }
	ErrorCode getErrorCode() { return errcode; }

private:
	ErrorCode errcode;
	char* errmsg;
};

#endif /* ENGINEEXCEPTION_H_ */
