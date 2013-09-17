/*
	Copyright 2010-2013 David "Alemarius Nexus" Lerch

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

	Additional permissions are granted, which are listed in the file
	GPLADDITIONS.
 */

#ifndef ENCODING_H_
#define ENCODING_H_

#include <gtaformats/config.h>
#include <cstddef>


#if		defined(_HAVE_ICONV) \
	||	defined(_POSIX_VERSION)  &&  _POSIX_VERSION >= 200112 \
	||	defined(__GLIBC__) && __GLIBC__ >= 2 && __GLIBC_MINOR__ >= 1
#define _HAVE_ICONV
#endif


#define ERR_INVALID_PARAMETER -1
#define ERR_INSUFFICIENT_BUFFER -2
#define ERR_INVALID_SEQUENCE -3


/**	\brief The encodings supported by the Transcode() function
 */
enum Encoding
{
	None,			//!< Special value which is invalid in Transcode()
	ASCII,			//!< ASCII encoding
	UTF8,			//!< UTF-8 encoding
	UTF16,			//!< UTF-16 encoding (little endian)
	WINDOWS1252,	//!< Windows-1252 code page
	GXT8,			//!< Encoding used in SA GXT files
	GXT16,			//!< Encoding used in VC/III GXT files
	ISO8859_1		//!< ISO-8859-1 encoding
};


/**	\brief Performs string encoding.
 *
 * 	This function transcodes a string from any given encoding to another. It transcodes srcBytes bytes from
 * 	src to dest, transcoding from srcEnc to destEnc. A maximum of destBytes bytes are written to dest. This
 * 	function returns the number of bytes written to dest, or a value lower than 0 in case of an error.
 *
 * 	@param src The buffer to read from.
 * 	@param srcBytes The number of bytes to read from src.
 * 	@param dest The buffer to transcode to.
 * 	@param destBytes The maximum number of bytes to be written to dest.
 * 	@param srcEnc The encoding of the string in src.
 * 	@param destEnc The encoding to be used in dest.
 * 	@return The number of bytes transcoded to dest or a value lower than 0 in case of an error. Possible
 * 		errors are ERR_INVALID_PARAMETERS, ERR_INVALID_SEQUENCE or ERR_INSUFFICIENT_BUFFER. The latter means
 * 		that dest isn't large enough for the transcoded string. If this is the case, dest will be filled
 * 		with as much bytes as fit in (CAUTION: This can lead to truncated unicode sequences!)
 * 	@see Encoding
 */
int Transcode(char* src, int srcBytes, char* dest, int destBytes, Encoding srcEnc, Encoding destEnc);


/**	\brief Returns a buffer size sufficient for transcoding.
 *
 * 	This function returns a buffer size which is guaranteed to be sufficient for transcoding length bytes
 * 	from srcEnc to destEnc. Note that especially for unicode conversion this size might be far greater than
 * 	actually needed, but is the least guaranteed to be sufficient for whatever content to be transcoded.
 *
 *	@param length The length in bytes of the source buffer.
 *	@param srcEnc The encoding used in the source buffer.
 *	@param destEnc The encoding to use for transcoding.
 *	@return The size of a buffer, in bytes, which is guaranteed to be sufficient for transcoding length bytes
 *		from srcEnc to destEnc.
 */
int GetSufficientTranscodeBufferSize(int length, Encoding srcEnc, Encoding destEnc);


/**	\brief Returns the number of characters (not bytes) in the null-terminated UTF-16 string.
 *
 *	@param str A null-terminated UTF-16 encoded string.
 *	@return The number of characters (not bytes) in the UTF-16 string, excluding the null-termination.
 */
size_t strlenUTF16(const char* str);

#endif /* ENCODING_H_ */
