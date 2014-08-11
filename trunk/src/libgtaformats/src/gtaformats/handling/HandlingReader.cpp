/*
	Copyright 2010-2014 David "Alemarius Nexus" Lerch

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

#include "HandlingReader.h"

using std::streamsize;




HandlingReader::HandlingReader(istream* stream, bool deleteStream)
{
	init(stream, deleteStream);
}


HandlingReader::HandlingReader(const File& file)
{
	init(file.openInputStream(), true);
}


HandlingReader::~HandlingReader()
{
	if (deleteStream) {
		delete stream;
	}
}


void HandlingReader::init(istream* stream, bool deleteStream)
{
	this->stream = stream;
	this->deleteStream = deleteStream;
}


HandlingEntry* HandlingReader::readEntry()
{
	char buf[4096];

	while (true) {
		stream->getline(buf, sizeof(buf));
		streamsize numChars = stream->gcount();

		char* commentStart = strchr(buf, ';');

		if (commentStart) {
			*commentStart = '\0';
			numChars = commentStart-buf;
		}

		// TODO: Finish this code!
	}
}
