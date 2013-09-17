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

#ifndef IFPLOADER_H_
#define IFPLOADER_H_

#include "IFPAnimation.h"
#include "../util/File.h"
#include <istream>

using std::istream;



class IFPLoader {
public:
	enum IFPVersion {
		ANPK = 1,
		ANP3 = 3
	};

public:
	IFPLoader(istream* stream);
	IFPLoader(const File& file);
	~IFPLoader();
	IFPAnimation* readAnimation();
	CString getName() const { return ifpName; }
	int32_t getAnimationCount() const { return numAnims; }

private:
	void init();
	CString readVER1String()
	{
		CString str("");
		char buf[64];
		char* offBuf = buf;

		while (true) {
			stream->read(offBuf, 4);

			if (offBuf[0] == '\0') {
				str.append(CString(buf, offBuf-buf));
				break;
			} else if (offBuf[1] == '\0') {
				str.append(CString(buf, offBuf-buf + 1));
				break;
			} else if (offBuf[2] == '\0') {
				str.append(CString(buf, offBuf-buf + 2));
				break;
			} else if (offBuf[3] == '\0') {
				str.append(CString(buf, offBuf-buf + 3));
				break;
			}

			offBuf += 4;

			if (offBuf-buf == sizeof(buf)) {
				str.append(CString(buf, sizeof(buf)));
				offBuf = buf;
			}
		}

		return str;
	}
	void skipVER1String()
	{
		char buf[4];
		do {
			stream->read(buf, 4);
		} while (buf[0] != '\0'  &&  buf[1] != '\0'  &&  buf[2] != '\0'  &&  buf[3] != '\0');
	}

private:
	istream* stream;
	CString ifpName;
	int32_t endOffs;
	int32_t numAnims;
	IFPVersion ver;
};

#endif /* IFPLOADER_H_ */
