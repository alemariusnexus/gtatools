/*
 * BufferedInputStream.h
 *
 *  Created on: 22.08.2010
 *      Author: alemariusnexus
 */

#ifndef BUFFEREDINPUTSTREAM_H_
#define BUFFEREDINPUTSTREAM_H_

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
