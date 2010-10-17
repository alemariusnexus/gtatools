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

#include "RangedInputStream.h"
#include "IOException.h"


RangedInputStream::RangedInputStream(InputStream* backend, streamsize maxRead, bool autoDeleteBackend)
		: FilterInputStream(backend, autoDeleteBackend), maxRead(maxRead), readCount(0),
		  readStart(backend->tell())
{
	if (readStart == -1) {
		readStart = 0;
	}
}


RangedInputStream::streampos RangedInputStream::tell()
{
	return backend->tell() - readStart;
}


void RangedInputStream::seek(streampos pos, SeekPosition startPos)
{
	if (startPos == STREAM_SEEK_BEGIN) {
		if (pos > maxRead  ||  pos < 0) {
			throw IOException("Invalid seek: Would jump out of the range defined for this RangedInputStream!", __FILE__, __LINE__);
		}

		backend->seek(readStart+pos, STREAM_SEEK_BEGIN);
		readCount = pos;
	} else if (startPos == STREAM_SEEK_CURRENT) {
		if (readCount+pos > maxRead  ||  readCount+pos < 0) {
			throw IOException("Invalid seek: Would jump out of the range defined for this RangedInputStream!", __FILE__, __LINE__);
		}

		readCount += pos;
		backend->seek(pos, STREAM_SEEK_CURRENT);
	} else if (startPos == STREAM_SEEK_END) {
		int end = readStart+maxRead;
		int seekFromCur = end-readCount-pos;

		if (readCount+seekFromCur > maxRead  ||  readCount+seekFromCur < 0) {
			throw IOException("Invalid seek: Would jump out of the range defined for this RangedInputStream!", __FILE__, __LINE__);
		}

		backend->seek(seekFromCur, STREAM_SEEK_CURRENT);
		readCount = end-pos;
	}
}


void RangedInputStream::read(char* dest, streamsize len)
{
	streamsize max = getMaxReadUntilEnd();

	if (len > max) {
		len = max;
	}

	backend->read(dest, len);
	readCount += len;
}


RangedInputStream::streamsize RangedInputStream::getMaxReadUntilEnd()
{
	return maxRead-readCount;
}


bool RangedInputStream::hasReachedEnd()
{
	return readCount >= maxRead;
}


RangedInputStream::streamsize RangedInputStream::getLastReadCount()
{
	return backend->getLastReadCount();
}

