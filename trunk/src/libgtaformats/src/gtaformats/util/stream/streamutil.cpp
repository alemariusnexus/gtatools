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

#include "streamutil.h"
#include <algorithm>
#include <cstdio>

using std::min;



void StreamMove(iostream* stream, streamsize len, streampos toPos)
{
	streamoff fromStart = stream->tellg();
	streamoff toStart = toPos;

	if (fromStart == toStart)
		return;

	if (toStart > fromStart  &&  fromStart+len > toStart) {
		// The regions overlap and content is moved towards the end.

		char buf[32768];

		streamsize numMoved = 0;

		while (numMoved < len) {
			streamsize numToRead = min(len-numMoved, (streamsize) sizeof(buf));

			stream->seekg(fromStart+len-numMoved-numToRead, iostream::beg);
			stream->read(buf, numToRead);

			stream->seekp(toStart+len-numMoved-numToRead, iostream::beg);
			stream->write(buf, numToRead);

			numMoved += numToRead;
		}
	} else {
		// Either the regions don't overlap, or they are moved towards the beginning

		char buf[32768];

		streamsize numMoved = 0;

		while (numMoved < len) {
			streamsize numToRead = min(len-numMoved, (streamsize) sizeof(buf));
			stream->read(buf, numToRead);
			stream->seekp(toStart + numMoved);
			stream->write(buf, numToRead);
			numMoved += numToRead;

			stream->seekg(fromStart + numMoved);
		}
	}
}
