/*
	Copyright 2010 David "Alemarius Nexus" Lerch

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

#ifndef INPUTSTREAM_H_
#define INPUTSTREAM_H_

#include "../../config.h"



/**	\brief Abstract class representing any kind of input streams
 *
 * 	This class defines an interface for any kind of input streams. Subclasses read from files, stdin etc.
 */
class InputStream {
public:
	typedef int64_t streampos;
	typedef int64_t streamsize;

public:
	/**	\brief Positions used as reference points in the seek() method.
	 */
	enum SeekPosition {
		STREAM_SEEK_BEGIN,		//!< Start from the beginning of the stream
		STREAM_SEEK_CURRENT,	//!< Start from the current stream position
		STREAM_SEEK_END			//!< Start from the end of the stream
	};

public:
	/**	\brief Destructor
	 */
	virtual ~InputStream() {}

	/**	\brief Tells the current position of the steam.
	 *
	 *	@return The current stream position or -1 if it can't be told.
	 */
	virtual streampos tell() = 0;

	/**	\brief Reposition the stream.
	 *
	 * 	This sets the position of the stream to pos, relative to the given startPos. Implementations may throw exceptions if
	 * 	anything goes wrong.
	 *
	 * 	@param pos The position to go to (relative to startPos)
	 * 	@param startPos The reference point.
	 */
	virtual void seek(streampos pos, SeekPosition startPos = STREAM_SEEK_CURRENT) = 0;

	/**	\brief Read a given amount of data from the stream.
	 *
	 * 	If the end of the stream is reached while reading, only the data read until then is put to dest.
	 *
	 * 	@param dest The buffer to write the data to.
	 * 	@param len The maximum number of bytes to be read. The actual amount read may differ if the end of stream is reached. Use
	 * 		getLastReadCount() to get the actual amount of data read.
	 */
	virtual void read(char* dest, streamsize len) = 0;

	/**	\brief Return whether the stream has reached it's end.
	 *
	 * 	@return true if the stream has reached it's end.
	 */
	virtual bool hasReachedEnd() = 0;

	/**	\brief Return the number of bytes actually read by the last read() call.
	 *
	 * 	@return The number of bytes actually read by the last read() call.
	 */
	virtual streamsize getLastReadCount() = 0;

	/**	\brief Read up to the next newline character.
	 *
	 * 	The newline character(s) won't be included.
	 *
	 *	@param dest The buffer to read the data to.
	 *	@param len The maximum amount of bytes to be read.
	 *	@return The number of bytes read (don't use getLastReadCount() here, because it will always result in the value 1).
	 */
	int readLine(char* dest, streamsize len);

	/**	\brief Skip data.
	 *
	 * 	This works by calling seek().
	 *
	 * 	@param len The number of bytes to be skipped.
	 */
	void skip(streamsize len);

protected:
	/**	\brief Constructs an InputStream.
	 *
	 * 	Should never be called from outside the API.
	 */
	InputStream() {}
};

#endif /* INPUTSTREAM_H_ */
