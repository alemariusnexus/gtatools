/*
 * IDEReader.cpp
 *
 *  Created on: 09.02.2010
 *      Author: alemariusnexus
 */

#include "IDEReader.h"
#include "IDEStaticObject.h"
#include <string>
#include <fstream>

using std::string;
using std::ifstream;



IDEReader::IDEReader(istream* stream, bool deleteStream)
		: stream(stream), currentSection(NONE), deleteStream(deleteStream)
{
}


IDEReader::~IDEReader()
{
	if (deleteStream) {
		delete stream;
	}
}


IDEReader::IDEReader(const char* filename)
		: stream(new ifstream(filename, ifstream::in)), currentSection(NONE), deleteStream(true)
{
}


IDEEntity* IDEReader::readEntity()
{
	while (true) {
		char line[4096];
		stream->getline(line, sizeof(line));

		for (unsigned int i = 0 ; i < sizeof(line)  &&  line[i] != '\0' ; i++) {
			if (line[i] == '#') {
				line[i] = '\0';
				break;
			}
		}

		printf("%s\n", line);

		if (strcmp(line, "end") == 0) {
			currentSection = NONE;
			continue;
		} else if (strcmp(line, "objs") == 0) {
			currentSection = OBJS;
			continue;
		} else if (strlen(line) == 0) {
			continue;
		}

		if (currentSection == OBJS) {
			int32_t id = nextInt(line);
			char* modelName = nextString();
			char* texName = nextString();
			int32_t numSubobjects = nextInt();

			float* drawDists = new float[numSubobjects];

			for (int32_t i = 0 ; i < numSubobjects ; i++) {
				drawDists[i] = nextFloat();
			}

			int32_t flags = nextInt();

			char* modelNameCpy = new char[strlen(modelName)];
			char* texNameCpy = new char[strlen(texName)];
			strcpy(modelNameCpy, modelName);
			strcpy(texNameCpy, texName);

			return new IDEStaticObject(id, modelNameCpy, texNameCpy, numSubobjects, drawDists, flags);
		} else {
			continue;
		}
	}
}

