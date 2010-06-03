/*
 * STLInputStream.h
 *
 *  Created on: 15.05.2010
 *      Author: alemariusnexus
 */

#ifndef STLINPUTSTREAM_H_
#define STLINPUTSTREAM_H_

#include "InputStream.h"
#include <istream>

using std::istream;



class STLInputStream : public InputStream {
public:
	STLInputStream(istream* backend, bool autoClose = true, bool randomAccess = true);
	virtual ~STLInputStream();

	virtual void seek(int pos, SeekPosition startPos = STREAM_SEEK_CURRENT);
	virtual void read(char* dest, int len);
	virtual bool hasReachedEnd();
	virtual int getLastReadCount();

protected:
	STLInputStream(bool autoClose = true, bool randomAccess = true);
	void setBackend(istream* backend) { this->backend = backend; };

private:
	istream* backend;
	bool autoClose;
	bool randomAccess;
};

#endif /* STLINPUTSTREAM_H_ */
