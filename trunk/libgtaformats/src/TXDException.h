/*
 * TXDException.h
 *
 *  Created on: 23.01.2010
 *      Author: alemariusnexus
 */

#ifndef TXDEXCEPTION_H_
#define TXDEXCEPTION_H_

#include <exception>


class TXDException : public std::exception {
public:
	enum ErrorCode {
		SyntaxError,
		Unsupported
	};

public:
	TXDException(ErrorCode errCode, const char* msg) throw();
	virtual const char* what() const throw() { return message; }
	ErrorCode getErrorCode() const throw() { return errCode; }

private:
	ErrorCode errCode;
	char* message;
};


#endif /* TXDEXCEPTION_H_ */
