/*
 * STLInputStream.cpp
 *
 *  Created on: 15.05.2010
 *      Author: alemariusnexus
 */

#include "STLInputStream.h"
#include "IOException.h"


STLInputStream::STLInputStream(istream* backend, bool autoClose, bool randomAccess)
		: backend(backend), autoClose(autoClose), randomAccess(randomAccess)
{
}


STLInputStream::STLInputStream(bool autoClose, bool randomAccess)
		: backend(NULL), autoClose(autoClose), randomAccess(randomAccess)
{
}


STLInputStream::~STLInputStream()
{
	if (autoClose) {
		delete backend;
	}
}


void STLInputStream::seek(int pos, SeekPosition startPos)
{
	if (randomAccess) {
		istream::seekdir dir;

		switch (startPos) {
		case STREAM_SEEK_BEGIN:
			dir = istream::beg;
			break;
		case STREAM_SEEK_CURRENT:
			dir = istream::cur;
			break;
		case STREAM_SEEK_END:
			dir = istream::end;
			break;
		}

		backend->seekg(pos, dir);
	} else {
		if (startPos == STREAM_SEEK_CURRENT) {
			char skipBuf[4096];

			for (int i = 0 ; i < pos/sizeof(skipBuf) ; i++) {
				read(skipBuf, sizeof(skipBuf));
			}

			read(skipBuf, pos % sizeof(skipBuf));
		} else {
			throw IOException("Seeking in non-random access streams is only allowed from CURRENT position!", __FILE__, __LINE__);
		}
	}
}


void STLInputStream::read(char* dest, int len)
{
	backend->read(dest, len);
}


bool STLInputStream::hasReachedEnd()
{
	return backend->eof();
}


int STLInputStream::getLastReadCount()
{
	return backend->gcount();
}


