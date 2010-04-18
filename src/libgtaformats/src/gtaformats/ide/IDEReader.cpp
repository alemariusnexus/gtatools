/*
	Copyright 2010 David Lerch

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

#include "IDEReader.h"
#include "IDEStaticObject.h"
#include "IDETimedObject.h"
#include "IDEAnimation.h"
#include <string>
#include <fstream>
#include <cstdlib>

using std::string;
using std::ifstream;
using std::ofstream;



IDEReader::IDEReader(istream* stream, bool deleteStream)
		: GTASectionFileReader(stream, deleteStream), currentSection(NONE)
{
}


IDEReader::~IDEReader()
{
}


IDEReader::IDEReader(const char* filename)
		: GTASectionFileReader(filename), currentSection(NONE)
{
}


IDEStatement* IDEReader::readStatement()
{
	while (true) {
		char line[4096];

		if (!readNextLine(line, sizeof(line))) {
			return NULL;
		}

		if (strcmp(line, "end") == 0) {
			currentSection = NONE;
			continue;
		} else if (strcmp(line, "objs") == 0) {
			currentSection = OBJS;
			continue;
		} else if (strcmp(line, "tobj") == 0) {
			currentSection = TOBJ;
			continue;
		} else if (strcmp(line, "anim") == 0) {
			currentSection = ANIM;
			continue;
		}

		if (currentSection == OBJS) {
			int32_t id = nextInt(line);
			char* modelName = nextString();
			char* texName = nextString();
			int32_t numSubobjects;
			float* drawDists;
			int32_t flags;

			char* param4 = nextString();
			char* param5 = nextString();
			char* param6 = nextString();

			if (param6 == NULL) {
				numSubobjects = 1;
				drawDists = new float[1];
				drawDists[0] = atof(param4);
				flags = atoi(param5);
			} else {
				numSubobjects = atoi(param4);
				drawDists = new float[numSubobjects];
				int ddIdx = 0;
				drawDists[ddIdx++] = atof(param5);

				if (numSubobjects > 1) {
					drawDists[ddIdx++] = atof(param6);

					for (; ddIdx < numSubobjects ; ddIdx++) {
						drawDists[ddIdx] = nextFloat();
					}

					flags = nextInt();
				} else {
					flags = atoi(param6);
				}
			}

			return new IDEStaticObject(id, modelName, texName, numSubobjects, drawDists, flags);
		} else if (currentSection == TOBJ) {
			int numParams = 1;

			for (int i = 0 ; i < strlen(line) ; i++) {
				if (line[i] == ',') {
					numParams++;
				}
			}

			int32_t id = nextInt(line);
			char* modelName = nextString();
			char* texName = nextString();
			int32_t numSubobjects;
			float* drawDists;
			int32_t flags;
			int32_t timeOn;
			int32_t timeOff;

			if (numParams == 7) {
				// Object count not given, default 1
				numSubobjects = 1;
			} else {
				numSubobjects = nextInt();
			}

			drawDists = new float[numSubobjects];

			for (int i = 0 ; i < numSubobjects ; i++) {
				//drawDists[i] = nextFloat();
			}

			flags = nextInt();
			timeOn = nextInt();
			timeOff = nextInt();

			return new IDETimedObject(id, modelName, texName, numSubobjects, drawDists,
					flags, timeOn, timeOff);
		} else if (currentSection == ANIM) {
			int32_t id = nextInt(line);
			char* modelName = nextString();
			char* texName = nextString();
			char* animName = nextString();
			float drawDist = nextFloat();
			int32_t flags = nextInt();

			return new IDEAnimation(id, modelName, texName, animName, drawDist, flags);
		} else {
			continue;
		}
	}

	/*while (true) {
		char line[4096];
		stream->getline(line, sizeof(line));

		if (stream->eof()) {
			return NULL;
		}

		if (line[strlen(line)-1] == '\r') {
			line[strlen(line)-1] = '\0';
		}

		char* cmtStart = strchr(line, '#');

		if (cmtStart) {
			cmtStart[0] = '\0';
		}

		if (strcmp(line, "end") == 0) {
			currentSection = NONE;
			continue;
		} else if (strcmp(line, "objs") == 0) {
			currentSection = OBJS;
			continue;
		} else if (strcmp(line, "tobj") == 0) {
			currentSection = TOBJ;
			continue;
		} else if (strcmp(line, "anim") == 0) {
			currentSection = ANIM;
			continue;
		} else if (strlen(line) == 0) {
			continue;
		}

		if (currentSection == OBJS) {
			int32_t id = nextInt(line);
			char* modelName = nextString();
			char* texName = nextString();
			int32_t numSubobjects;
			float* drawDists;
			int32_t flags;

			char* param4 = nextString();
			char* param5 = nextString();
			char* param6 = nextString();

			if (param6 == NULL) {
				numSubobjects = 1;
				drawDists = new float[1];
				drawDists[0] = atof(param4);
				flags = atoi(param5);
			} else {
				numSubobjects = atoi(param4);
				drawDists = new float[numSubobjects];
				int ddIdx = 0;
				drawDists[ddIdx++] = atof(param5);

				if (numSubobjects > 1) {
					drawDists[ddIdx++] = atof(param6);

					for (; ddIdx < numSubobjects ; ddIdx++) {
						drawDists[ddIdx] = nextFloat();
					}

					flags = nextInt();
				} else {
					flags = atoi(param6);
				}
			}

			return new IDEStaticObject(id, modelName, texName, numSubobjects, drawDists, flags);
		} else if (currentSection == TOBJ) {
			int32_t id = nextInt(line);
			char* modelName = nextString();
			char* texName = nextString();
			int32_t numSubobjects;
			float* drawDists;
			int32_t flags;
			int32_t timeOn;
			int32_t timeOff;

			int numParams = 1;

			for (int i = 0 ; i < strlen(line) ; i++) {
				if (line[i] == ',') {
					numParams++;
				}
			}

			if (numParams == 7) {
				// Object count not given, default 1
				numSubobjects = 1;
			} else {
				numSubobjects = nextInt();
			}

			for (int i = 0 ; i < numSubobjects ; i++) {
				drawDists[i] = nextFloat();
			}

			flags = nextInt();
			timeOn = nextInt();
			timeOff = nextInt();

			return new IDETimedObject(id, modelName, texName, numSubobjects, drawDists,
					flags, timeOn, timeOff);
		} else if (currentSection == ANIM) {
			int32_t id = nextInt(line);
			char* modelName = nextString();
			char* texName = nextString();
			char* animName = nextString();
			float drawDist = nextFloat();
			int32_t flags = nextInt();

			return new IDEAnimation(id, modelName, texName, animName, drawDist, flags);
		} else {
			continue;
		}
	}*/
}

