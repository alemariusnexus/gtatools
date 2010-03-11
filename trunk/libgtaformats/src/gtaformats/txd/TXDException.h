/*
 * TXDException.h
 *
 *  Created on: 23.01.2010
 *      Author: alemariusnexus
 */

#ifndef TXDEXCEPTION_H_
#define TXDEXCEPTION_H_

#include <exception>
#include "../gf_config.h"


/**	\brief This exception can be thrown by TXDArchive.
 */
class TXDException : public std::exception {
public:
	/**	\brief The type of error.
	 */
	enum ErrorCode {
		SyntaxError,//!< SyntaxError
		Unsupported //!< Unsupported
	};

public:
	/**	\brief Creates a new exception.
	 *
	 *	@param errCode The error code.
	 *	@param msg The error message.
	 *	@param offset The offset at which the error happened (optional).
	 */
	TXDException(ErrorCode errCode, const char* msg, long long offset = -1) throw();

	/**	\brief Returns the error message.
	 *
	 *	@return The error message.
	 */
	virtual const char* what() const throw() { return message; }

	/**	\brief Returns the error code.
	 *
	 *	@return The error code.
	 */
	ErrorCode getErrorCode() const throw() { return errCode; }

private:
	ErrorCode errCode;
	long long offset;
	char* message;
};


#endif /* TXDEXCEPTION_H_ */
