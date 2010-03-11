/*
 * IMGException.h
 *
 *  Created on: 29.01.2010
 *      Author: alemariusnexus
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
