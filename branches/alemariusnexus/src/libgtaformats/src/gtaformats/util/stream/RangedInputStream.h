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

#ifndef RANGEDINPUTSTREAM_H_
#define RANGEDINPUTSTREAM_H_

#include "InputStream.h"


/**	\brief A filtering InputStream that only allows access to parts of another InputStream.
 *
 * 	RangedInputStreams have a start and end point. Reading and seeking are only allowed within the bounds of this range. The end of
 * 	the range is considered the end of this stream and seeking operations going beyond the range will throw an exception.
 */
class RangedInputStream : public InputStream {
public:
	/**	\brief Creates a new RangedInputStream.
	 *
	 * 	The RangedInputStream will read it's data from the backend. The range start will be the current position of the backend
	 * 	stream (or 0 if InputStream::tell() on the backend returns -1), and the end will be that start point plus maxRead.
	 *
	 * 	@param backend The stream to read from.
	 * 	@param maxRead The maximum number of bytes to be readable from the current stream position. This defines the end point.
	 * 	@param autoDeleteBackend If true, the backend stream will be deleted by this stream's destructor.
	 */
	RangedInputStream(InputStream* backend, int maxRead, bool autoDeleteBackend = true);

	/**	\brief Destructor.
	 */
	~RangedInputStream();

	/**	\brief Repositions this stream.
	 *
	 * 	STREAM_SEEK_BEGIN will be relative to the range start and STREAM_SEEK_END will be relative to the range end. Attempts to
	 * 	seek beyond the bounds of this range result in an IOException to be thrown.
	 *
	 * 	@param pos The position to go to (relative to startPos)
	 * 	@param startPos The reference point.
	 */
	virtual void seek(int pos, SeekPosition startPos = STREAM_SEEK_CURRENT);

	/**	\brief Tells the position of this stream.
	 *
	 * 	@return The position of this stream relative to the range start.
	 */
	virtual int tell();

	/**	\brief Reads data from the stream.
	 *
	 * 	As soon as the end of the range is reached, the reading will stop.
	 *
	 * 	@param dest The buffer to read to.
	 * 	@param len The maximum number of bytes to be read.
	 */
	virtual void read(char* dest, int len);

	/**	\brief Returns whether the stream has reached the range end.
	 *
	 * 	@return true if the stream has reached the range end point.
	 */
	virtual bool hasReachedEnd();

	/**	\brief Returns the number of bytes actually read by the last read() call.
	 *
	 *	@return The number of bytes actually read by the last read() call.
	 */
	virtual int getLastReadCount();

private:
	/**	\brief Returns the number of bytes which can be read until the range end is reached.
	 *
	 *	@return The number of bytes which can be read until the range end is reached.
	 */
	int getMaxReadUntilEnd();

private:
	InputStream* backend;
	bool autoDeleteBackend;
	int readStart;
	int maxRead;
	int readCount;
};

#endif /* RANGEDINPUTSTREAM_H_ */
