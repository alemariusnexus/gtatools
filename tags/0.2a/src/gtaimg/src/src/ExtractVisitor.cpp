/*
	Copyright 2010 David "Alemarius Nexus" Lerch

	This file is part of gtaimg.

	gtaimg is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	gtaimg is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with gtaimg.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "ExtractVisitor.h"
#include <gtaformats/util/util.h>
//#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <fstream>

using std::ostream;
using std::ofstream;
using std::cout;
using std::endl;


struct MatchInfo {
	int matchedPatternIndex;
};

const char flags[][PARAM_MAXLEN] = {
		"r", "s", "o", "\0"
};
const char switches[][PARAM_MAXLEN] = {
		"f", "\0"
};


ExtractVisitor::ExtractVisitor(int argc, char** argv) {
	SetFlagOptions(flags);
	SetSwitchOptions(switches);

	if (!ParseCommandLineArguments(argc, argv, 2)) {
		exit(1);
	}

	if (GetStandaloneParamCount() == 0) {
		fprintf(stderr, "Error: No archive file given!\n");
		exit(1);
	}

	paramset::iterator it;

	useRegex = IsFlagSet("r");
	sourcesOnly = IsFlagSet("s");

	if (IsFlagSet("o")) {
		writeToStdout = true;
		sourcesOnly = true;
	} else {
		writeToStdout = false;
	}

	numPatterns = (GetStandaloneParamCount()-1);

	if (!sourcesOnly) {
		if (numPatterns%2 != 0) {
			fprintf(stderr, "Error: No destination file given for last entry!\n");
			exit(1);
		}

		numPatterns /= 2;
	}

	destfiles = new const char*[numPatterns];
	regexes = new const char*[numPatterns];

	int i = 0;
	for (it = GetStandaloneParamBegin()+1 ; it != GetStandaloneParamEnd() ; it++) {
		const char* pattern = *it;
		const char* destfile = NULL;

		if (!sourcesOnly) {
			destfile = *++it;
		}

		regexes[i] = pattern;
		destfiles[i] = destfile;
		i++;
	}
}

ExtractVisitor::~ExtractVisitor() {
	vector<char*>::iterator it;

	for (it = temporaryStrings.begin() ; it != temporaryStrings.end() ; it++) {
		temporaryStrings.erase(it);
		delete *it;
	}
}

bool ExtractVisitor::readHeader(IMGEntry* header, void*& udata) {
	for (int i = 0 ; i < numPatterns ; i++) {
		bool match = useRegex ? WildcardMatch(regexes[i], header->name)
				: strcmp(regexes[i], header->name) == 0;

		if (match) {
			MatchInfo* info = new MatchInfo;
			info->matchedPatternIndex = i;
			udata = info;
			return true;
		}
	}

	return false;
}

void ExtractVisitor::readEntry(IMGEntry* header, InputStream* stream, void*& udata) {
	MatchInfo* info = (MatchInfo*) udata;

	const char* rawDestfile = destfiles[info->matchedPatternIndex];
	const char* destfile = NULL;

	ostream* out = NULL;

	if (!writeToStdout) {
		if (rawDestfile) {
			destfile = rawDestfile;
		} else {
			destfile = header->name;
		}

		cout << "Writing entry " << header->name << " to file " << destfile << endl;

		out = new ofstream(destfile, ofstream::out | ofstream::binary);
	} else {
		out = &std::cout;
	}

	char buffer[IMG_BLOCK_SIZE];

	for (int i = 0 ; i < header->size ; i++) {
		stream->read(buffer, sizeof(buffer));

		out->write(buffer, sizeof(buffer));
	}

	if (!writeToStdout) {
		dynamic_cast<ofstream*>(out)->close();
		delete out;
	}

	delete info;
}
