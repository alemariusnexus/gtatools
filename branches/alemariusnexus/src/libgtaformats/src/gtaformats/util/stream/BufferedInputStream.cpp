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

#include "BufferedInputStream.h"
#include <cstring>
#include <cstdio>



BufferedInputStream::BufferedInputStream(InputStream* backend, int bufferSize, bool deleteBackend)
		: FilterInputStream(backend, deleteBackend), bufferSize(bufferSize), buffer(new char[bufferSize]),
		  bufferPos(buffer), lastReadCount(0)
{
	fillBuffer();
}


BufferedInputStream::~BufferedInputStream()
{
	delete[] buffer;
}


BufferedInputStream::streampos BufferedInputStream::tell()
{
	streampos basePos = backend->tell();

	if (basePos == -1) {
		return -1;
	}

	return basePos - (lastBufferReadCount-(bufferPos-buffer));
}


void BufferedInputStream::seek(streampos pos, SeekPosition startPos)
{
	if (startPos == STREAM_SEEK_BEGIN) {
		backend->seek(pos, STREAM_SEEK_BEGIN);
		fillBuffer();
	} else if (startPos == STREAM_SEEK_END) {
		backend->seek(pos, STREAM_SEEK_END);
		fillBuffer();
	} else {
		if (pos >= 0) {
			int bufferLeft = lastBufferReadCount-(bufferPos-buffer);
			int skipFromBuffer = pos > bufferLeft ? bufferLeft : pos;

			bufferPos += skipFromBuffer;

			if (skipFromBuffer != pos) {
				backend->seek(pos-skipFromBuffer, STREAM_SEEK_CURRENT);
			}
		} else {
			// Seek backwards
			int bufferRead = bufferPos-buffer;
			int skipFromBuffer = -pos > bufferRead ? -bufferRead : pos;
			bufferPos += skipFromBuffer;

			if (skipFromBuffer != pos) {
				backend->seek(pos-skipFromBuffer-lastBufferReadCount, STREAM_SEEK_CURRENT);
				fillBuffer();
			}
		}
	}
}


void BufferedInputStream::read(char* dest, streamsize len)
{
	int bufferLeft = lastBufferReadCount-(bufferPos-buffer);

	if (len > bufferLeft) {
		memcpy(dest, bufferPos, bufferLeft);

		if (lastBufferReadCount == bufferSize) {
			backend->read(dest+bufferLeft, len-bufferLeft);
			lastReadCount = bufferLeft + backend->getLastReadCount();
			fillBuffer();
		} else {
			lastReadCount = bufferLeft;
			bufferPos += bufferLeft;
		}
	} else {
		memcpy(dest, bufferPos, len);
		bufferPos += len;
		lastReadCount = len;
	}
}


bool BufferedInputStream::hasReachedEnd()
{
	return backend->hasReachedEnd()  &&  (bufferPos-buffer) == lastBufferReadCount;
}


BufferedInputStream::streamsize BufferedInputStream::getLastReadCount()
{
	return lastReadCount;
}


void BufferedInputStream::fillBuffer()
{
	backend->read(buffer, bufferSize);
	lastBufferReadCount = backend->getLastReadCount();
	bufferPos = buffer;
}

