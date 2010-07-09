/*
 * InputStream.h
 *
 *  Created on: 15.05.2010
 *      Author: alemariusnexus
 */

#ifndef INPUTSTREAM_H_
#define INPUTSTREAM_H_



class InputStream {
public:
	enum SeekPosition {
		STREAM_SEEK_BEGIN,
		STREAM_SEEK_CURRENT,
		STREAM_SEEK_END
	};

public:
	virtual ~InputStream() {}

	virtual void seek(int pos, SeekPosition startPos = STREAM_SEEK_CURRENT) = 0;
	virtual void read(char* dest, int len) = 0;
	virtual bool hasReachedEnd() = 0;
	virtual int getLastReadCount() = 0;

	int readLine(char* dest, int len);
	void skip(int len);

protected:
	InputStream() {}
};

#endif /* INPUTSTREAM_H_ */
