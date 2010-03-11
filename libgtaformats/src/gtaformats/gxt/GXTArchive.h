/*
 * GXTArchive.h
 *
 *  Created on: 07.02.2010
 *      Author: alemariusnexus
 */

#ifndef GXTARCHIVE_H_
#define GXTARCHIVE_H_

#include "../gf_config.h"
#include <istream>
#include "GXTTable.h"
#include "GXTPlainKeyTable.h"
#include <map>

using std::istream;
using std::map;


struct GXTTableHeader {
	char name[8];
	int32_t offset;
};



class GXTArchive
{
public:
	enum GXTVersion {
		VER1, // GTA III/VC
		VER2 // GTA SA/IV
	};

private:
	typedef map<uint8_t, wchar_t> EncodingMap;

public:
	static GXTVersion guessVersion(const char* filename);
	static GXTVersion guessVersion(istream* stream, bool returnToStart = true);

public:
	GXTArchive(istream* stream, bool deleteStream = true);
	GXTArchive(const char* filename);
	~GXTArchive();
	GXTTableHeader** getTableHeaders() { return headers; };
	GXTTable* readTable(GXTTableHeader* header);

private:
	void init();
	wchar_t* decode8(const uint8_t* src, int32_t len);

private:
	istream* stream;
	GXTVersion version;
	bool deleteStream;
	int32_t numTables;
	GXTTableHeader** headers;
	istream::streamoff position;
	EncodingMap encodingMap;
};

#endif /* GXTARCHIVE_H_ */
