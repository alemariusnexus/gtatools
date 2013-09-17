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

#include "IPLReader.h"
#include <string>
#include <cstdlib>
#include "IPLInstance.h"
#include "IPLCar.h"
#include <cstdio>
#include "../util/stream/StreamReader.h"
#include "../util/stream/EndianSwappingStreamReader.h"
#include "../util/stream/StreamWriter.h"
#include "../util/stream/EndianSwappingStreamWriter.h"


using std::string;



IPLReader::IPLReader(istream* stream, bool deleteStream)
		: GTASectionFileReader(stream, deleteStream), currentSection(NONE)
{
	init();
}


IPLReader::~IPLReader()
{
}


IPLReader::IPLReader(const File& file)
		: GTASectionFileReader(file), currentSection(NONE)
{
	init();
}


void IPLReader::init()
{
	char skipBuf[7*4];
	char firstBytes[4];
	stream->read(firstBytes, 4);

	if (!stream->eof()  &&  strncmp(firstBytes, "bnry", 4) == 0) {
#ifdef GTAFORMATS_LITTLE_ENDIAN
		StreamReader reader(stream);
#else
		EndianSwappingStreamReader reader(stream);
#endif

		reader.read32(&binaryInstanceCount);
		stream->ignore(3*4);
		reader.read32(&binaryCarCount);
		stream->ignore(4);
		reader.read32(&binaryInstanceOffset);
		stream->ignore(7*4);
		reader.read32(&binaryCarOffset);
		stream->ignore(3*4);
		binaryReadCount = 0;

		stream->ignore(binaryInstanceOffset - 0x4C);
	} else {
		binaryReadCount = -1;
		stream->clear();
		stream->seekg(-stream->gcount(), istream::cur);
	}
}


IPLStatement* IPLReader::readStatement()
{
	if (binaryReadCount == -1) {
		// Plain ASCII IPL file

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
			} else if (strcmp(line, "cars") == 0) {
				currentSection = CARS;
				continue;
			}

			int numParams = 1;

			for (int i = 0 ; line[i] != '\0' ; i++) {
				if (line[i] == ',') {
					numParams++;
				}
			}

			bool ok = true;

			if (currentSection == INST) {
				int32_t id;
				char* modelName;
				float x, y, z;
				float rotX, rotY, rotZ, rotW;
				float scaleX = 1.0f, scaleY = 1.0f, scaleZ =  1.0f;
				int32_t interior = 0;
				int32_t lod = -1;

				id = nextInt(&ok, line);
				modelName = nextString(&ok);

				if (numParams == 11) {
					// SA format
					interior = nextInt(&ok);
					x = nextFloat(&ok);
					y = nextFloat(&ok);
					z = nextFloat(&ok);
					rotX = nextFloat(&ok);
					rotY = nextFloat(&ok);
					rotZ = nextFloat(&ok);
					rotW = nextFloat(&ok);
					lod = nextInt(&ok);
				} else if (numParams == 12) {
					// III format
					x = nextFloat(&ok);
					y = nextFloat(&ok);
					z = nextFloat(&ok);
					scaleX = nextFloat(&ok);
					scaleY = nextFloat(&ok);
					scaleZ = nextFloat(&ok);
					rotX = nextFloat(&ok);
					rotY = nextFloat(&ok);
					rotZ = nextFloat(&ok);
					rotW = nextFloat(&ok);
				} else if (numParams == 13) {
					// VC format
					interior = nextInt(&ok);
					x = nextFloat(&ok);
					y = nextFloat(&ok);
					z = nextFloat(&ok);
					scaleX = nextFloat(&ok);
					scaleY = nextFloat(&ok);
					scaleZ = nextFloat(&ok);
					rotX = nextFloat(&ok);
					rotY = nextFloat(&ok);
					rotZ = nextFloat(&ok);
					rotW = nextFloat(&ok);
				}

				if (!ok) {
					continue;
				}

				return new IPLInstance (
						id, CString(modelName),
						x, y, z,
						rotX, rotY, rotZ, rotW,
						scaleX, scaleY, scaleZ,
						interior,
						lod
				);
			} else if (currentSection == CARS) {
				float x, y, z;
				float angle;
				int32_t carId;
				int32_t primaryColor, secondaryColor;
				bool forceSpawn;
				int32_t alarmProb, doorLockProb;
				int32_t unknown1, unknown2;

				x = nextFloat(&ok, line);
				y = nextFloat(&ok);
				z = nextFloat(&ok);
				angle = nextFloat(&ok);
				carId = nextInt(&ok);
				primaryColor = nextInt(&ok);
				secondaryColor = nextInt(&ok);
				forceSpawn = (nextInt(&ok) == 1);
				alarmProb = nextInt(&ok);
				doorLockProb = nextInt(&ok);
				unknown1 = nextInt(&ok);
				unknown2 = nextInt(&ok);

				if (!ok) {
					continue;
				}

				return new IPLCar (
						x, y, z, angle, carId, primaryColor, secondaryColor, forceSpawn, alarmProb,
						doorLockProb, unknown1, unknown2
				);
			} else {
				continue;
			}
		}
	} else {
		// Binary IPL file

#ifdef GTAFORMATS_LITTLE_ENDIAN
		StreamReader reader(stream);
#else
		EndianSwappingStreamReader reader(stream);
#endif

		IPLStatement* stmt;

		if (binaryReadCount >= binaryInstanceCount+binaryCarCount) {
			return NULL;
		}

		if (binaryReadCount == binaryInstanceCount) {
			unsigned int skipCount = (unsigned int) (binaryCarOffset-stream->tellg());
			stream->ignore(skipCount);
		}

		if (binaryReadCount < binaryInstanceCount) {
			float pos[3];
			float rot[4];
			int32_t id, interior, lod;

			reader.readArrayFloat(pos, 3);
			reader.readArrayFloat(rot, 4);
			reader.read32(&id);
			reader.read32(&interior);
			reader.read32(&lod);

			stmt = new IPLInstance (
					id, CString(),
					pos[0], pos[1], pos[2],
					rot[0], rot[1], rot[2], rot[3],
					1.0f, 1.0f, 1.0f,
					interior,
					lod
			);
		} else {
			float pos[3];
			float angle;
			int32_t carId;
			int32_t primaryColor, secondaryColor;
			int32_t forceSpawn;
			int32_t alarmProb, doorLockProb;
			int32_t unknown1;
			int32_t unknown2;

			reader.readArrayFloat(pos, 3);
			reader.readFloat(&angle);
			reader.read32(&carId);
			reader.read32(&primaryColor);
			reader.read32(&secondaryColor);
			reader.read32(&forceSpawn);
			reader.read32(&alarmProb);
			reader.read32(&doorLockProb);
			reader.read32(&unknown1);
			reader.read32(&unknown2);

			stmt = new IPLCar (
					pos[0], pos[1], pos[2], angle, carId, primaryColor, secondaryColor, (forceSpawn == 1),
					alarmProb, doorLockProb, unknown1, unknown2
			);
		}

		binaryReadCount++;
		return stmt;
	}
}

