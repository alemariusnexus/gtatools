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

#ifndef BUFFEREDINPUTSTREAM_H_
#define BUFFEREDINPUTSTREAM_H_

#include "../../config.h"
#include "FilterInputStream.h"


class BufferedInputStream : public FilterInputStream {
public:
	BufferedInputStream(InputStream* backend, int bufferSize = 4096, bool deleteBackend = true);
	virtual ~BufferedInputStream();
	virtual streampos tell();
	virtual void seek(streampos pos, SeekPosition startPos = STREAM_SEEK_CURRENT);
	virtual void read(char* dest, streamsize len);
	virtual bool hasReachedEnd();
	virtual streamsize getLastReadCount();

private:
	void fillBuffer();

private:
	int bufferSize;
	char* buffer;
	char* bufferPos;
	streamsize lastReadCount;
	int lastBufferReadCount;
};

#endif /* BUFFEREDINPUTSTREAM_H_ */
