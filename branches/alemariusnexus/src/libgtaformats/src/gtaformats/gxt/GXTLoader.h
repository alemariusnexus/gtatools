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

#ifndef GXTLOADER_H_
#define GXTLOADER_H_

#include "GXTTable.h"
#include "../util/stream/InputStream.h"
#include "../util/File.h"
#include "../util/encoding.h"


struct GXTTableHeader
{
	char name[8];
	int32_t offset;
};


class GXTLoader {
public:
	enum Version
	{
		SAIV,
		VC3
	};

public:
	GXTLoader(InputStream* stream, Encoding encoding = None, bool autoClose = false);
	GXTLoader(const File& file, Encoding encoding = None);
	void setEncoding(Encoding encoding) { this->encoding = encoding; }
	Encoding getEncoding() { return encoding; }
	int32_t getTableCount() const { return numTables; }
	bool nextTableHeader(GXTTableHeader& header);
	void readTableHeaders(GXTTableHeader* headers);
	GXTTable* readTableData(const GXTTableHeader& header);
	Version getVersion() { return version; }

private:
	void init();
	InputStream::streampos getTableOffset(const GXTTableHeader& header) const;

private:
	InputStream* stream;
	bool autoClose;
	Version version;
	int32_t numTables;
	int32_t currentTable;
	InputStream::streampos cpos;
	Encoding encoding;
};

#endif /* GXTLOADER_H_ */
