/*
 * IPLReader.cpp
 *
 *  Created on: 18.04.2010
 *      Author: alemariusnexus
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


IPLReader::IPLReader(const char* filename)
		: GTASectionFileReader(filename), currentSection(NONE)
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

