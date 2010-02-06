/*
 * IMGException.h
 *
 *  Created on: 29.01.2010
 *      Author: alemariusnexus
 */

#ifndef IMGEXCEPTION_H_
#define IMGEXCEPTION_H_

#include <exception>
#include "config.h"

class IMGException : std::exception {
public:
	enum ErrorCode {
		UnknownFileType,
		MayNotSeek
	};

public:
	IMGException(ErrorCode code, const char* message) throw();
	virtual ~IMGException() throw();
	virtual const char* what() const throw() { return message; };
	ErrorCode getErrorCode() const throw() { return errCode; }

private:
	char* message;
	ErrorCode errCode;
};

#endif /* IMGEXCEPTION_H_ */
