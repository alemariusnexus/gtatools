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

#include "gta.h"
#include "util/CRC32.h"
#include <cstring>
#include <cstdlib>
#include <cstdio>



crc32_t Crc32(const char* value, size_t len)
{
	CRC32 crc;
	crc.append(value, len);
	return crc.getChecksum();
}


void RwGetVersionName(uint32_t version, char* dest) {
	switch (version) {
	case RW_VERSION_GTA3_1:
		sprintf(dest, "%s (RW_VERSION_GTA3_1)", RW_VERSION_NR_GTA3_1);
		break;
	case RW_VERSION_GTA3_2:
		sprintf(dest, "%s (RW_VERSION_GTA3_2)", RW_VERSION_NR_GTA3_2);
		break;
	case RW_VERSION_GTAVC_1:
		sprintf(dest, "%s (RW_VERSION_GTAVC_1)", RW_VERSION_NR_GTAVC_1);
		break;
	case RW_VERSION_GTAVC_2:
		sprintf(dest, "%s (RW_VERSION_GTAVC_2)", RW_VERSION_NR_GTAVC_2);
		break;
	case RW_VERSION_GTASA:
		sprintf(dest, "%s (RW_VERSION_GTASA)", RW_VERSION_NR_GTASA);
		break;
	default:
		sprintf(dest, "{unknown: 0x%X}", version);
		break;
	}
}

/*int RwReadSectionHeaderWithID(istream* stream, RwSectionHeader& header, uint32_t id) {
	int size = RwReadSectionHeader(stream, header);
	if (header.id != id) {
		char expected[64];
		char found[64];
		RwGetSectionName(id, expected);
		RwGetSectionName(header.id, found);
		fprintf(stderr, "Error: Found section with type %s where %s was expected!\n", found, expected);
		exit(1);
	}
	return size;
}

int RwSkipSectionHeaderWithID(istream* stream, uint32_t id) {
	RwSectionHeader header;
	return RwReadSectionHeaderWithID(stream, header, id);
}

int RwSkipSectionWithID(istream* stream, uint32_t id) {
	RwSectionHeader header;
	int size = RwReadSectionHeaderWithID(stream, header, id);
	size += RwSkipSectionBody(stream, header);
	return size;
}*/
