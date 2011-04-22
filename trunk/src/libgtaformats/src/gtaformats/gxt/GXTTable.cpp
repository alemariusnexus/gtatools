/*
	Copyright 2010-2011 David "Alemarius Nexus" Lerch

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

	Additional permissions are granted, which are listed in the file
	GPLADDITIONS.
 */

#include "GXTTable.h"
#include "../gta.h"
#include <cstring>



GXTTable::GXTTable(const char* name, Encoding internalEncoding, bool keepKeyNames)
		: internalEncoding(internalEncoding), keyNames(keepKeyNames ? new KeyNameMap : NULL)
{
	strncpy(this->name, name, 8);
}


GXTTable::~GXTTable()
{
	if (keyNames) {
		KeyNameMap::iterator it;

		for (it = keyNames->begin() ; it != keyNames->end() ; it++) {
			delete[] it->second;
		}
	}

	EntryIterator it;

	for (it = entries.begin() ; it != entries.end() ; it++) {
		delete[] it->second;
	}
}


char* GXTTable::getValue(int32_t keyHash)
{
	EntryIterator it = entries.find(keyHash);
	return it == entries.end() ? NULL : it->second;
}


const char* GXTTable::getValue(int32_t keyHash) const
{
	ConstEntryIterator it = entries.find(keyHash);
	return it == entries.end() ? NULL : it->second;
}


char* GXTTable::getValueUTF8(const char* key) const
{
	return getValueUTF8(Crc32(key));
}


char* GXTTable::getValueUTF16(const char* key) const
{
	return getValueUTF16(Crc32(key));
}


char* GXTTable::getValueUTF8(crc32_t keyHash) const
{
	const char* val = getValue(keyHash);
	int srcBytes;
	int destBytes;

	if (internalEncoding == UTF16) {
		srcBytes = (strlenUTF16(val)+1)*2;
	} else {
		srcBytes = strlen(val)+1;
	}

	char* valCpy = new char[srcBytes];
	memcpy(valCpy, val, srcBytes);

	destBytes = GetSufficientTranscodeBufferSize(srcBytes, internalEncoding, UTF8);

	char* dest = new char[destBytes];

	if (Transcode(valCpy, srcBytes, dest, destBytes, internalEncoding, UTF8) < 0) {
		delete[] dest;
		delete[] valCpy;
		return NULL;
	}

	delete[] valCpy;
	return dest;
}


char* GXTTable::getValueUTF16(crc32_t keyHash) const
{
	const char* val = getValue(keyHash);
	int srcBytes;
	int destBytes;

	if (internalEncoding == UTF16) {
		// If the internal format is UTF16 already, we'll just make a copy.
		srcBytes = (strlenUTF16(val)+1)*2;
	} else {
		srcBytes = strlen(val)+1;
	}

	char* valCpy = new char[srcBytes];
	memcpy(valCpy, val, srcBytes);

	destBytes = GetSufficientTranscodeBufferSize(srcBytes, internalEncoding, UTF16);

	char* dest = new char[destBytes];

	if (Transcode(valCpy, srcBytes, dest, destBytes, internalEncoding, UTF16) < 0) {
		delete[] dest;
		delete[] valCpy;
		return NULL;
	}

	delete[] valCpy;
	return dest;
}


void GXTTable::setValue(crc32_t keyHash, char* value)
{
	entries[keyHash] = value;
}


void GXTTable::setValue(const char* key, char* value)
{
	crc32_t keyHash = Crc32(key);
	setValue(keyHash, value);

	if (keyNames) {
		char* keyCpy = new char[strlen(key)];
		strcpy(keyCpy, key);
		(*keyNames)[keyHash] = keyCpy;
	}
}


const char* GXTTable::getKeyName(crc32_t keyHash) const
{
	if (keyNames) {
		return (*keyNames)[keyHash];
	}

	return NULL;
}


void GXTTable::setKeyName(crc32_t keyHash, const char* name)
{
	if (keyNames) {
		char* nameCpy = new char[strlen(name)];
		strcpy(nameCpy, name);
		(*keyNames)[keyHash] = nameCpy;
	}
}
