/*
	Copyright 2010-2014 David "Alemarius Nexus" Lerch

	This file is part of gtatools-gui.

	gtatools-gui is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	gtatools-gui is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with gtatools-gui.  If not, see <http://www.gnu.org/licenses/>.

	Additional permissions are granted, which are listed in the file
	GPLADDITIONS.
 */

#include "RWSectionHeader.h"




bool RWSectionHeader::readSectionHeader(RWSectionHeader* outHeader, istream* stream, const RWSectionHeader* lastRead)
{
	// Check if there is data to read
	stream->peek();
	if (stream->eof()) {
		return false;
	}

	stream->read((char*) outHeader, 12);

#ifndef GTAFORMATS_LITTLE_ENDIAN
	outHeader->id = SwapEndianness32(outHeader->id);
	outHeader->size = SwapEndianness32(outHeader->size);
	outHeader->version = SwapEndianness32(outHeader->version);
#endif

	if (lastRead) {
		// Do some heuristic checks to see if the data we read is actually a section. For files in IMG
		// archives, there might be some junk after the actual section data (to fill the IMG section). If it
		// seems to be an invalid section, we assume this is the end of the RWBS file.
		if (		lastRead->version != outHeader->version
				||	(outHeader->id == 0  &&  outHeader->size == 0  &&  outHeader->version == 0)
		) {
			return false;
		}
	}

	return true;
}
