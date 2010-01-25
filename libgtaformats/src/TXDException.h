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
	TXDException(const char* msg) throw() : message(msg) {}
	virtual const char* what() const throw() { return message; }

private:
	const char* message;
};


#endif /* TXDEXCEPTION_H_ */
