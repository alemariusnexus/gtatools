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

#ifndef STLINPUTSTREAM_H_
#define STLINPUTSTREAM_H_

#include "../../config.h"
#include "InputStream.h"
#include <istream>

using std::istream;



/**	\brief InpuStream reading from std::istreams.
 *
 */
class STLInputStream : public InputStream {
public:
	/**	\brief Construct a new STLInputStream.
	 *
	 *	@param backend The std::istream to read from.
	 *	@param autoClose If true, the backend stream will automatically deleted inside this stream's destructor.
	 *	@param randomAccess Whether random access to this stream is allowed. Use false here for backends which don't support
	 *		seeking (like cin). Seeking will then only be allowed with STREAM_SEEK_CURRENT and will be emulated by simply
	 *		reading and discarding data.
	 */
	STLInputStream(istream* backend, bool autoClose = true, bool randomAccess = true);

	/**	\brief Destructor.
	 */
	virtual ~STLInputStream();

	/**	\brief Tells the stream position.
	 *
	 * 	Redirects to backend->tellg().
	 *
	 * 	@return The stream position or -1 in case of error.
	 */
	virtual streampos tell();

	/**	\brief Repositions this stream.
	 *
	 * 	Redirects to backend->seekg(). Note that in streams without randomAccess seeking is only allowed from STREAM_SEEK_CURRENT and
	 * 	will be relatively slow.
	 *
	 * 	@param pos The position to go to (relative to startPos)
	 * 	@param startPos The reference point.
	 */
	virtual void seek(streampos pos, SeekPosition startPos = STREAM_SEEK_CURRENT);

	/**	\brief Reads from this stream.
	 *
	 * 	Redirects to backend->read().
	 *
	 * 	@param dest The buffer to write the data to.
	 * 	@param len The maximum number of bytes to be read. The actual amount read may differ if the end of stream is reached. Use
	 * 		getLastReadCount() to get the actual amount of data read.
	 */
	virtual void read(char* dest, streamsize len);

	/**	\brief Returns whether this stream has reached it's end.
	 *
	 * 	Redirects to backend->eof().
	 *
	 * 	@return true if this stream has reached it's end.
	 */
	virtual bool hasReachedEnd();

	/**	\brief Returns the number of characters read by the last read() call.
	 *
	 * 	Redirects to backend->gcount().
	 *
	 * 	@return The number of characters read by the last read() call.
	 */
	virtual streamsize getLastReadCount();

protected:
	/**	\brief Construct an STLInputStream without a backend.
	 *
	 * 	This should only be used internally and the backend must be initialized before any calls are made to this stream.
	 *
	 *	@param autoClose If true, the backend stream will automatically deleted inside this stream's destructor.
	 *	@param randomAccess Whether random access to this stream is allowed. Use false here for backends which don't support
	 *		seeking (like cin). Seeking will then only be allowed with STREAM_SEEK_CURRENT and will be emulated by simply
	 *		reading and discarding data.
	 */
	STLInputStream(bool autoClose = true, bool randomAccess = true);

	/**	\brief Sets the backend.
	 *
	 * 	Internal function.
	 *
	 * 	@param backend The std::istream to read from.
	 */
	void setBackend(istream* backend) { this->backend = backend; };

private:
	istream* backend;
	bool autoClose;
	bool randomAccess;
};

#endif /* STLINPUTSTREAM_H_ */
