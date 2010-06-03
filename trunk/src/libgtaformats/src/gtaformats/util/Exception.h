/*
 * Exception.h
 *
 *  Created on: 15.05.2010
 *      Author: alemariusnexus
 */

#ifndef EXCEPTION_H_
#define EXCEPTION_H_

#include <exception>
#include <cstdlib>

using std::exception;


class Exception : public exception {
public:
	Exception(const char* message, const char* srcFile = NULL, int srcLine = -1);
	Exception(const Exception& ex);
	virtual ~Exception() throw();
	virtual const char* what() const throw() { return message; };
	const char* getMessage() const throw() { return message; }
	const char* getFullMessage() const throw();

private:
	char* message;
	const char* srcFile;
	int srcLine;
};

#endif /* EXCEPTION_H_ */


