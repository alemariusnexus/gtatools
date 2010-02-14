/*
 * TXDException.h
 *
 *  Created on: 23.01.2010
 *      Author: alemariusnexus
 */

#ifndef TXDEXCEPTION_H_
#define TXDEXCEPTION_H_

#include <exception>
#include "../config.h"


class TXDException : public std::exception {
public:
	enum ErrorCode {
		SyntaxError,
		Unsupported
	};

public:
	TXDException(ErrorCode errCode, const char* msg, long long offset = -1) throw();
	virtual const char* what() const throw() { return message; }
	ErrorCode getErrorCode() const throw() { return errCode; }

private:
	ErrorCode errCode;
	long long offset;
	char* message;
};


#endif /* TXDEXCEPTION_H_ */
