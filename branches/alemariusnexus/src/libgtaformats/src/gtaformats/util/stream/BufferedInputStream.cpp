/*
 * BufferedInputStream.cpp
 *
 *  Created on: 22.08.2010
 *      Author: alemariusnexus
 */

#include "BufferedInputStream.h"
#include <cstring>



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
		int bufferLeft = lastBufferReadCount-(bufferPos-buffer);
		int skipFromBuffer = pos > bufferLeft ? bufferLeft : pos;
		bufferPos += skipFromBuffer;

		if (skipFromBuffer != pos) {
			backend->seek(pos-skipFromBuffer, STREAM_SEEK_CURRENT);
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

