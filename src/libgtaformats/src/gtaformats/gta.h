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

#ifndef _GTA_H
#define	_GTA_H

#include <gtaformats/config.h>
#include <istream>
#include "rwbs/RWSection.h"

using std::istream;


typedef int32_t crc32_t;


#define RW_VERSION_GTA3_1 0x0800FFFF
#define RW_VERSION_GTA3_2 0x00000310
#define RW_VERSION_GTAVC_1 0x0C02FFFF
#define RW_VERSION_GTAVC_2 0x1003FFFF
#define RW_VERSION_GTASA 0x1803FFFF

#define RW_VERSION_NR_GTA3_1 "3.?.?.?"
#define RW_VERSION_NR_GTA3_2 "3.1.0.0"
#define RW_VERSION_NR_GTAVC_1 "3.3.0.2"
#define RW_VERSION_NR_GTAVC_2 "3.4.0.3"
#define RW_VERSION_NR_GTASA "3.6.0.3"


struct RwSectionHeader {
	uint32_t id;
	int32_t size;
	int32_t version;
};

//#define RwReadSectionHeader(s,h) (s)->read(&(h), sizeof(RwSectionHeader))

inline int RwReadSectionHeader(istream* stream, RwSectionHeader& header) {
	stream->read((char*) &header, sizeof(RwSectionHeader));
	return sizeof(RwSectionHeader);
}

inline int RwSkipSectionBody(istream* stream, RwSectionHeader& header) {
	//stream->ignore(header.size);
	char* skipBuf = new char[header.size];
	stream->read(skipBuf, header.size);
	delete[] skipBuf;
	return header.size;
}


void RwGetVersionName(uint32_t version, char* dest);


/**	\brief This computes a CRC32 hash of a string the same way as GTA would do.
 *
 * 	@param value The string to hash.
 * 	@return The CRC32 hash.
 */
crc32_t Crc32(const char* value, size_t len);

inline crc32_t Crc32(const char* value) { return Crc32(value, strlen(value)); }


#endif	/* _GTA_H */

