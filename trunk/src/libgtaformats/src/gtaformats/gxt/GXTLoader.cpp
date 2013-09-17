/*
	Copyright 2010-2013 David "Alemarius Nexus" Lerch

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

#include "GXTLoader.h"
#include "GXTException.h"
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include "../util/stream/StreamReader.h"
#include "../util/stream/EndianSwappingStreamReader.h"
#include "../util/stream/StreamWriter.h"
#include "../util/stream/EndianSwappingStreamWriter.h"

using std::streamoff;


struct GXTSAIVEntry
{
	int32_t offset;
	crc32_t keyHash;
};


struct GXTVC3Entry
{
	int32_t offset;
	char key[8];
};


int EntrySortComparator(const void* e1, const void* e2)
{
	return ((GXTSAIVEntry*) e1)->offset - ((GXTSAIVEntry*) e2)->offset;
}




GXTLoader::GXTLoader(istream* stream, Encoding encoding, bool autoClose)
		: stream(stream), autoClose(autoClose), encoding(encoding)
{
	init();
}


GXTLoader::GXTLoader(const File& file, Encoding encoding)
		: stream(file.openInputStream(ifstream::binary)), autoClose(true), encoding(encoding)
{
	init();
}


void GXTLoader::init()
{
#ifdef GTAFORMATS_LITTLE_ENDIAN
	StreamReader reader(stream);
#else
	EndianSwappingStreamReader reader(stream);
#endif

	char skipBuf[4];
	char tov[4];
	stream->read(tov, 4);
	cpos = 4;

	if (strncmp(tov, "TABL", 4) == 0) {
		version = VC3;
	} else {
		version = SAIV;
		stream->read(skipBuf, 4); // TABL
		cpos += 4;
	}

	reader.read32(&numTables);

	if (numTables < 0) {
		char* errmsg = new char[64];
		sprintf(errmsg, "Invalid table header size: %d", numTables);
		GXTException ex(errmsg, __FILE__, __LINE__);
		delete[] errmsg;
		throw ex;
	}
	if (numTables%12 != 0) {
		char* errmsg = new char[128];
		sprintf(errmsg, "Invalid table header size (must be multiple of 12): %d", numTables);
		GXTException ex(errmsg, __FILE__, __LINE__);
		delete[] errmsg;
		throw ex;
	}

	numTables /= 12;
	cpos += 4;

	currentTable = 0;
}


bool GXTLoader::nextTableHeader(GXTTableHeader& header)
{
	if (currentTable >= numTables) {
		return false;
	}

	int32_t headerOffset = currentTable*12 + 8; // +8 = TABL and block size

	if (version == SAIV) {
		headerOffset += 4; // GXT version
	}

	if (cpos != headerOffset) {
		stream->seekg(headerOffset-cpos, istream::cur);
	}

	stream->read((char*) &header, 12);

#ifndef GTAFORMATS_LITTLE_ENDIAN
	header.offset = FromLittleEndian32(header.offset);
#endif

	cpos += 12;
	currentTable++;

	if (header.offset < 0) {
		char* errmsg = new char[64];
		sprintf(errmsg, "Invalid table offset: %d", header.offset);
		GXTException ex(errmsg, __FILE__, __LINE__);
		delete[] errmsg;
		throw ex;
	}

	return true;
}


void GXTLoader::readTableHeaders(GXTTableHeader* headers)
{
	int32_t readTableCount = numTables-currentTable;
	for (int32_t i = 0 ; i < readTableCount ; i++) {
		nextTableHeader(headers[i]);
	}
}


streamoff GXTLoader::getTableOffset(const GXTTableHeader& header) const
{
	streamoff offset = header.offset;

	if (strncmp(header.name, "MAIN\0", 5) != 0) {
		offset += 8; // The table name again
	}

	return offset;
}


GXTTable* GXTLoader::readTableData(const GXTTableHeader& header)
{
#ifdef GTAFORMATS_LITTLE_ENDIAN
	StreamReader reader(stream);
#else
	EndianSwappingStreamReader reader(stream);
#endif

	char skipBuf[8];
	streamoff offset = getTableOffset(header);
	stream->seekg(offset-cpos, istream::cur);
	cpos = offset;

	stream->read(skipBuf, 4); // TKEY
	cpos += 4;

	int32_t tkeySize = reader.read32();
	cpos += 4;

	if (version == SAIV) {
		int32_t numEntries = tkeySize/8;
		GXTTable* table = new GXTTable(header.name, encoding == None ? GXT8 : encoding, keepKeyNames);
		GXTSAIVEntry* entries = new GXTSAIVEntry[numEntries];

		reader.readArray32((int32_t*) entries, 2*numEntries);

		/*for (int32_t i = 0 ; i < numEntries ; i++) {
			stream->read((char*) &entries[i], 8);
		}*/

		cpos += 8*numEntries;

		stream->read(skipBuf, 8); // "TDAT" + TDAT size
		cpos += 8;

		qsort(entries, numEntries, 8, EntrySortComparator);
		streamoff tdatRead = 0;

		for (int32_t i = 0 ; i < numEntries ; i++) {
			unsigned int skip = (unsigned int) (entries[i].offset - tdatRead);
			char* tmpSkipBuf = new char[skip];
			stream->read(tmpSkipBuf, skip);
			delete[] tmpSkipBuf;
			tdatRead += skip;

			int step = 64;
			char* text = NULL;
			bool finished = false;
			int strLen = 0;

			for (int j = 0 ; !finished ; j++) {
				char* tmp = new char[(j+1)*step];

				if (text) {
					memcpy(tmp, text, j*step);
					delete[] text;
				}

				text = tmp;

				for (int k = 0 ; k < step ; k++) {
					int idx = j*step + k;
					stream->read(text+idx, 1);

					strLen++;
					if (text[idx] == '\0') {
						finished = true;
						break;
					}
				}
			}

			if (encoding != None  &&  encoding != GXT8) {
				char* old = text;
				int textSize = GetSufficientTranscodeBufferSize(strLen, GXT8, encoding);
				text = new char[textSize];
				int res = Transcode(old, strLen, text, textSize, GXT8, encoding);

				if (res < 0) {
					char* errmsg = new char[128];
					sprintf(errmsg, "Error transcoding GXT string %d: %d\n", entries[i].keyHash, res);
					GXTException ex(errmsg, __FILE__, __LINE__);
					delete[] errmsg;
					throw ex;
				}

				delete[] old;
			}

			table->setValue(entries[i].keyHash, text);
			tdatRead += strLen;
		}

		cpos += tdatRead;

		delete[] entries;

		return table;
	} else {
		int32_t numEntries = tkeySize/12;
		GXTTable* table = new GXTTable(header.name, encoding == None ? GXT16 : encoding, keepKeyNames);
		GXTVC3Entry* entries = new GXTVC3Entry[numEntries];

		stream->read((char*) entries, 12*numEntries);

#ifndef GTAFORMATS_LITTLE_ENDIAN
		for (int32_t i = 0 ; i < numEntries ; i++) {
			entries[i].offset = SwapEndianness32(entries[i].offset);
		}
#endif
		/*for (int32_t i = 0 ; i < numEntries ; i++) {
			stream->read((char*) &entries[i], 12);
		}*/

		cpos += 12*numEntries;

		qsort(entries, numEntries, 12, EntrySortComparator);

		stream->read(skipBuf, 4); // TDAT
		cpos += 4;

		int32_t tdatSize = reader.read32();
		cpos += 4;

		streamoff tdatRead = 0;

		for (int32_t i = 0 ; i < numEntries ; i++) {
			unsigned int skip = (unsigned int) (entries[i].offset - tdatRead);
			char* tmpSkipBuf = new char[skip];
			stream->read(tmpSkipBuf, skip);
			delete[] tmpSkipBuf;
			tdatRead += skip;

			unsigned int maxLen;

			if (i < numEntries-1) {
				maxLen = entries[i+1].offset - entries[i].offset;
			} else {
				maxLen = (unsigned int) (tdatSize - tdatRead);
			}

			char* text = NULL;

			text = new char[maxLen];
			stream->read(text, maxLen);
			tdatRead += maxLen;

			if (encoding != None  &&  encoding != GXT16) {
				char* old = text;
				int textSize = GetSufficientTranscodeBufferSize(maxLen, GXT16, encoding);
				text = new char[textSize];
				Transcode(old, maxLen, text, textSize, GXT16, encoding);
				delete[] old;
			}

			table->setValue(entries[i].key, text);
		}

		cpos += tdatRead;

		delete[] entries;

		return table;
	}

	return NULL;
}
