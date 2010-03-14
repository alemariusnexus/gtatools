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

#ifndef IMGEXCEPTION_H_
#define IMGEXCEPTION_H_

#include <exception>
#include "../gf_config.h"


/**	\brief Objects of this class are thrown by IMGArchive when an error occurs.
 */
class IMGException : std::exception {
public:
	/**	\brief The type of error that happened
	 */
	enum ErrorCode {
		UnknownFileType,//!< UnknownFileType
		UnableToOpen,   //!< UnableToOpen
		MayNotSeek,     //!< MayNotSeek
		SyntaxError     //!< SyntaxError
	};

public:
	/**	\brief Creates a new IMGException with the given error code and message.
	 *
	 *	@param code The error code.
	 *	@param message The error message.
	 */
	IMGException(ErrorCode code, const char* message) throw();

	/**	\brief Destructor.
	 */
	virtual ~IMGException() throw();

	/**	\brief Returns the error message.
	 *
	 *	@return The error message.
	 */
	virtual const char* what() const throw() { return message; };

	/**	\brief Returns the error code.
	 *
	 *	@return The error code.
	 */
	ErrorCode getErrorCode() const throw() { return errCode; }

private:
	char* message;
	ErrorCode errCode;
};

#endif /* IMGEXCEPTION_H_ */
