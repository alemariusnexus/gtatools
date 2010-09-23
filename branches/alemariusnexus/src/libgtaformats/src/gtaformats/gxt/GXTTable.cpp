/*
 * GXTTable.cpp
 *
 *  Created on: 17.09.2010
 *      Author: alemariusnexus
 */

#include "GXTTable.h"
#include "../gta.h"
#include <cstring>



GXTTable::GXTTable(Encoding internalEncoding)
		: internalEncoding(internalEncoding)
{
}


char* GXTTable::getValue(const char* key)
{
	return getValue(Crc32(key));
}


char* GXTTable::getValueUTF8(const char* key)
{
	return getValueUTF8(Crc32(key));
}


char* GXTTable::getValueUTF16(const char* key)
{
	return getValueUTF16(Crc32(key));
}


char* GXTTable::getValueUTF8(int32_t keyHash)
{
	char* val = getValue(keyHash);
	int srcBytes;
	int destBytes;

	if (internalEncoding == UTF16) {
		srcBytes = (strlenUTF16(val)+1)*2;
	} else {
		srcBytes = strlen(val)+1;
	}

	destBytes = GetSufficientTranscodeBufferSize(srcBytes, internalEncoding, UTF8);

	char* dest = new char[destBytes];

	if (Transcode(val, srcBytes, dest, destBytes, internalEncoding, UTF8) < 0) {
		delete[] dest;
		return NULL;
	}

	return dest;
}


char* GXTTable::getValueUTF16(int32_t keyHash)
{
	char* val = getValue(keyHash);
	int srcBytes;
	int destBytes;

	if (internalEncoding == UTF16) {
		// If the internal format is UTF16 already, we'll just make a copy.
		srcBytes = (strlenUTF16(val)+1)*2;
	} else {
		srcBytes = strlen(val)+1;
	}

	destBytes = GetSufficientTranscodeBufferSize(srcBytes, internalEncoding, UTF16);

	char* dest = new char[destBytes];

	if (Transcode(val, srcBytes, dest, destBytes, internalEncoding, UTF16) < 0) {
		delete[] dest;
		return NULL;
	}

	return dest;
}
