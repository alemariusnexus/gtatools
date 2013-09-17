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

#include "IDEReader.h"
#include "IDEStaticObject.h"
#include "IDETimedObject.h"
#include "IDEAnimation.h"
#include "IDEPedestrian.h"
#include "IDEWeapon.h"
#include <string>
#include <cstdlib>
#include "IDEException.h"

using std::string;



IDEReader::IDEReader(istream* stream, bool deleteStream)
		: GTASectionFileReader(stream, deleteStream), currentSection(NONE)
{
}


IDEReader::~IDEReader()
{
}


IDEReader::IDEReader(const File& file)
		: GTASectionFileReader(file), currentSection(NONE)
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
		} else if (strcmp(line, "peds") == 0) {
			currentSection = PEDS;
			continue;
		} else if (strcmp(line, "weap") == 0) {
			currentSection = WEAP;
			continue;
		}

		unsigned int numParams = 1;

		for (unsigned int i = 0 ; i < strlen(line) ; i++) {
			if (line[i] == ',') {
				numParams++;
			}
		}

		bool ok = true;

		if (currentSection == OBJS) {
			int32_t id = nextInt(&ok, line);
			char* modelName = nextString(&ok);
			char* texName = nextString(&ok);
			int32_t numSubobjects;
			float* drawDists;
			int32_t flags;

			if (numParams == 5) {
				numSubobjects = 1;
			} else {
				numSubobjects = nextInt(&ok);
			}

			drawDists = new float[numSubobjects];

			for (int32_t i = 0 ; i < numSubobjects ; i++) {
				drawDists[i] = nextFloat(&ok);
			}

			flags = nextInt(&ok);

			if (!ok) {
				continue;
			}

			/*char* param4 = nextString(&ok);
			char* param5 = nextString(&ok);
			char* param6 = nextString(&ok);

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
			}*/

			return new IDEStaticObject(id, modelName, texName, numSubobjects, drawDists, flags);
		} else if (currentSection == TOBJ) {
			int32_t id = nextInt(&ok, line);
			char* modelName = nextString(&ok);
			char* texName = nextString(&ok);
			int32_t numSubobjects;
			float* drawDists;
			int32_t flags;
			int32_t timeOn;
			int32_t timeOff;

			if (numParams == 7) {
				// Object count not given, default 1
				numSubobjects = 1;
			} else {
				numSubobjects = nextInt(&ok);
			}

			drawDists = new float[numSubobjects];

			for (int i = 0 ; i < numSubobjects ; i++) {
				drawDists[i] = nextFloat(&ok);
			}

			flags = nextInt(&ok);
			timeOn = nextInt(&ok);
			timeOff = nextInt(&ok);

			if (!ok) {
				continue;
			}

			return new IDETimedObject(id, modelName, texName, numSubobjects, drawDists,
					flags, timeOn, timeOff);
		} else if (currentSection == ANIM) {
			int32_t id = nextInt(&ok, line);
			char* modelName = nextString(&ok);
			char* texName = nextString(&ok);
			char* animName = nextString(&ok);
			float drawDist = nextFloat(&ok);
			int32_t flags = nextInt(&ok);

			if (!ok) {
				continue;
			}

			return new IDEAnimation(id, modelName, texName, animName, drawDist, flags);
		} else if (currentSection == PEDS) {
			int32_t id = nextInt(&ok, line);
			char* modelName = nextString(&ok);
			char* txdName = nextString(&ok);
			char* defPedType = nextString(&ok);
			char* behavior = nextString(&ok);
			char* animGroup = nextString(&ok);
			int32_t carMask = nextHexInt(&ok);
			int32_t flags = 0;
			char* animFile = NULL;
			int32_t radio1 = -1;
			int32_t radio2 = -1;
			char* voiceFile = NULL;
			char* voice1 = NULL;
			char* voice2 = NULL;

			if (numParams > 7) {
				if (numParams > 10) {
					// SA
					flags = nextInt(&ok);
					animFile = nextString(&ok);
					radio1 = nextInt(&ok);
					radio2 = nextInt(&ok);
					voiceFile = nextString(&ok);
					voice1 = nextString(&ok);
					voice2 = nextString(&ok);
				} else {
					// VC
					animFile = nextString(&ok);
					radio1 = nextInt(&ok);
					radio2 = nextInt(&ok);
				}
			}

			if (!ok) {
				continue;
			}

			return new IDEPedestrian(id, modelName, txdName, defPedType, behavior, animGroup, carMask,
					flags, animFile, radio1, radio2, voiceFile, voice1, voice2);
		} else if (currentSection == WEAP) {
			int32_t id = nextInt(&ok, line);
			char* modelName = nextString(&ok);
			char* txdName = nextString(&ok);
			char* animName = nextString(&ok);
			int32_t objc = nextInt(&ok);
			float* ddists = new float[objc];

			for (int32_t i = 0 ; i < objc ; i++) {
				ddists[i] = nextFloat(&ok);
			}

			int32_t flags = nextInt(&ok);

			if (!ok) {
				continue;
			}

			return new IDEWeapon(id, modelName, txdName, animName, objc, ddists, flags);
		} else {
			continue;
		}
	}
}

