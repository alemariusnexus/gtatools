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

#include "InputStream.h"


int InputStream::readLine(char* dest, streamsize len)
{
	streamsize i;

	for (i = 0 ; i < len  &&  !hasReachedEnd() ; i++) {
		read(dest+i, 1);

		if (dest[i] == '\n') {
			dest[i] = '\0';
			return i+1;
		} else if (dest[i] == '\r') {
			dest[i] = '\0';
			seek(1);
			return i+2;
		}
	}

	return i;
}


void InputStream::skip(streamsize len)
{
	seek(len);
}

