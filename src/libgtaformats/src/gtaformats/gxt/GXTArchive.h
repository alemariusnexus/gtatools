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

#ifndef GXTARCHIVE_H_
#define GXTARCHIVE_H_

#include <gf_config.h>\n#includelude <istream>
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
	long long position;
	EncodingMap encodingMap;
};

#endif /* GXTARCHIVE_H_ */
