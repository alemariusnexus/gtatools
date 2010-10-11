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

#ifndef GTASECTIONFILEREADER_H_
#define GTASECTIONFILEREADER_H_

#include "gf_config.h"
#include <cstring>
#include <cstdlib>
#include "util/stream/InputStream.h"
#include "util/File.h"
#include "util/ErrorLog.h"



class GTASectionFileReader {
public:
	enum ErrorBehavior {
		Continue,
		Exception
	};

public:
	void setErrorBehavior(ErrorBehavior eb) { errorBehavior = eb; }
	ErrorBehavior getErrorBehavior() const { return errorBehavior; }
	ErrorLog* getErrorLog() { return &log; }

protected:
	GTASectionFileReader(InputStream* stream, bool deleteStream = true);
	GTASectionFileReader(const File& file);
	virtual ~GTASectionFileReader();

	bool readNextLine(char* buf, int len);

	char* nextString(bool* ok, char* str = NULL);
	int32_t nextInt(bool* ok, char* str = NULL);
	int32_t nextHexInt(bool* ok, char* str = NULL);
	float nextFloat(bool* ok, char* str = NULL);
	char* trim(char* str);

protected:
	InputStream* stream;
	ErrorLog log;
	ErrorBehavior errorBehavior;

private:
	bool deleteStream;
	int lastReadLine;
	int paramNo;
};

#endif /* GTASECTIONFILEREADER_H_ */
