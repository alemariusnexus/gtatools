/*
 * GXTLoader.h
 *
 *  Created on: 18.09.2010
 *      Author: alemariusnexus
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
