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

#include "IPLReader.h"
#include <string>
#include <cstdlib>
#include "IPLInstance.h"

using std::string;



IPLReader::IPLReader(InputStream* stream, bool deleteStream)
		: GTASectionFileReader(stream, deleteStream), currentSection(NONE)
{
}


IPLReader::~IPLReader()
{
}


IPLReader::IPLReader(const File& file)
		: GTASectionFileReader(file), currentSection(NONE)
{
}


IPLStatement* IPLReader::readStatement()
{
	while (true) {
		char line[4096];

		if (!readNextLine(line, sizeof(line))) {
			return NULL;
		}

		if (strcmp(line, "end") == 0) {
			currentSection = NONE;
			continue;
		} else if (strcmp(line, "inst") == 0) {
			currentSection = INST;
			continue;
		}

		if (currentSection == INST) {
			int32_t id;
			char* modelName;
			float x, y, z;
			float rotX, rotY, rotZ, rotW;
			float scaleX = 1.0f, scaleY = 1.0f, scaleZ =  1.0f;
			int32_t interior = 0;
			int32_t lod = -1;

			int numParams = 1;

			for (int i = 0 ; line[i] != '\0' ; i++) {
				if (line[i] == ',') {
					numParams++;
				}
			}

			id = nextInt(line);
			modelName = nextString();

			if (numParams == 11) {
				// SA format
				interior = nextInt();
				x = nextFloat();
				y = nextFloat();
				z = nextFloat();
				rotX = nextFloat();
				rotY = nextFloat();
				rotZ = nextFloat();
				rotW = nextFloat();
				lod = nextInt();
			} else if (numParams == 12) {
				// III format
				x = nextFloat();
				y = nextFloat();
				z = nextFloat();
				scaleX = nextFloat();
				scaleY = nextFloat();
				scaleZ = nextFloat();
				rotX = nextFloat();
				rotY = nextFloat();
				rotZ = nextFloat();
				rotW = nextFloat();
			} else if (numParams == 13) {
				// VC format
				interior = nextInt();
				x = nextFloat();
				y = nextFloat();
				z = nextFloat();
				scaleX = nextFloat();
				scaleY = nextFloat();
				scaleZ = nextFloat();
				rotX = nextFloat();
				rotY = nextFloat();
				rotZ = nextFloat();
				rotW = nextFloat();
			}

			return new IPLInstance (
					id, modelName,
					x, y, z,
					rotX, rotY, rotZ, rotW,
					scaleX, scaleY, scaleZ,
					interior,
					lod
			);
		} else {
			continue;
		}
	}
}

