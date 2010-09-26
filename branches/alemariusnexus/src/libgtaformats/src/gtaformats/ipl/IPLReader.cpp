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
#include "IPLCar.h"

using std::string;



IPLReader::IPLReader(InputStream* stream, bool deleteStream)
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
	char firstBytes[4];
	stream->read(firstBytes, 4);

	if (strncmp(firstBytes, "bnry", 4) == 0) {
		stream->read((char*) &binaryInstanceCount, 4);
		stream->skip(3*4);
		stream->read((char*) &binaryCarCount, 4);
		stream->skip(4);
		stream->read((char*) &binaryInstanceOffset, 4);
		stream->skip(7*4);
		stream->read((char*) &binaryCarOffset, 4);
		stream->skip(3*4);
		binaryReadCount = 0;
		stream->skip(binaryInstanceOffset - 0x4C);
	} else {
		binaryReadCount = -1;
		stream->seek(-4, InputStream::STREAM_SEEK_CURRENT);
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
			} else if (currentSection == CARS) {
				float x, y, z;
				float angle;
				int32_t carId;
				int32_t primaryColor, secondaryColor;
				bool forceSpawn;
				int32_t alarmProb, doorLockProb;
				int32_t unknown1, unknown2;

				x = nextFloat(line);
				y = nextFloat();
				z = nextFloat();
				angle = nextFloat();
				carId = nextInt();
				primaryColor = nextInt();
				secondaryColor = nextInt();
				forceSpawn = (nextInt() == 1);
				alarmProb = nextInt();
				doorLockProb = nextInt();
				unknown1 = nextInt();
				unknown2 = nextInt();

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

		IPLStatement* stmt;

		if (binaryReadCount >= binaryInstanceCount+binaryCarCount) {
			return NULL;
		}

		if (binaryReadCount == binaryInstanceCount) {
			stream->skip(binaryCarOffset-stream->tell());
		}

		if (binaryReadCount < binaryInstanceCount) {
			float pos[3];
			float rot[4];
			int32_t id, interior, lod;

			stream->read((char*) pos, 3*4);
			stream->read((char*) rot, 4*4);
			stream->read((char*) &id, 4);
			stream->read((char*) &interior, 4);
			stream->read((char*) &lod, 4);

			stmt = new IPLInstance (
					id, NULL,
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

			stream->read((char*) pos, 3*4);
			stream->read((char*) &angle, 4);
			stream->read((char*) &carId, 4);
			stream->read((char*) &primaryColor, 4);
			stream->read((char*) &secondaryColor, 4);
			stream->read((char*) &forceSpawn, 4);
			stream->read((char*) &alarmProb, 4);
			stream->read((char*) &doorLockProb, 4);
			stream->read((char*) &unknown1, 4);
			stream->read((char*) &unknown2, 4);

			stmt = new IPLCar (
					pos[0], pos[1], pos[2], angle, carId, primaryColor, secondaryColor, (forceSpawn == 1),
					alarmProb, doorLockProb, unknown1, unknown2
			);
		}

		binaryReadCount++;
		return stmt;
	}
}

