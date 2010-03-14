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
		SyntaxError,	//!< SyntaxError
		Unsupported,	//!< Unsupported
		PNGError
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
