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
#include <cstdio>

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

	if (strncmp(firstBytes, "bnry", 4) == 0) {
		stream->read((char*) &binaryInstanceCount, 4);
		stream->read(skipBuf, 3*4);
		stream->read((char*) &binaryCarCount, 4);
		stream->read(skipBuf, 4);
		stream->read((char*) &binaryInstanceOffset, 4);
		stream->read(skipBuf, 7*4);
		stream->read((char*) &binaryCarOffset, 4);
		stream->read(skipBuf, 3*4);
		binaryReadCount = 0;

		char* tmpSkipBuf = new char[binaryInstanceOffset - 0x4C];
		stream->read(tmpSkipBuf, binaryInstanceOffset - 0x4C);
		delete[] tmpSkipBuf;
	} else {
		binaryReadCount = -1;
		stream->seekg(-4, istream::cur);
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

		IPLStatement* stmt;

		if (binaryReadCount >= binaryInstanceCount+binaryCarCount) {
			return NULL;
		}

		if (binaryReadCount == binaryInstanceCount) {
			int skipCount = binaryCarOffset-stream->tellg();
			char* tmpSkipBuf = new char[skipCount];
			stream->read(tmpSkipBuf, skipCount);
			delete[] tmpSkipBuf;
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

