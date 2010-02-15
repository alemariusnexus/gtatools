/*
 * DFFException.h
 *
 *  Created on: 10.02.2010
 *      Author: alemariusnexus
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
