/*
	Copyright 2010 David Lerch

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
	boost::cmatch* match;
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
	regexes = new regex*[numPatterns];

	int i = 0;
	for (it = GetStandaloneParamBegin()+1 ; it != GetStandaloneParamEnd() ; it++) {
		const char* pattern = *it;
		const char* destfile = NULL;

		if (!sourcesOnly) {
			destfile = *++it;
		}

		regex* reg;

		if (!useRegex) {
			char* escapedPattern = new char[strlen(pattern)+5];
			sprintf(escapedPattern, "\\Q%s\\E", pattern);
			temporaryStrings.push_back(escapedPattern);
			pattern = escapedPattern;
		}

		try {
			reg = new regex(pattern);
		} catch (boost::regex_error& err) {
			cout << "Error compiling source pattern " << i+1 << " at offset " << err.position()
					<< ": " << err.what() << endl;
			exit(1);
		}

		regexes[i] = reg;
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
		boost::cmatch* match = new boost::cmatch;
		if (boost::regex_match(header->name, *match, *regexes[i])) {
			MatchInfo* info = new MatchInfo;
			info->matchedPatternIndex = i;
			info->match = match;
			udata = info;
			return true;
		}
		delete match;
	}

	return false;
}

void ExtractVisitor::readEntry(IMGEntry* header, istream* stream, void*& udata) {
	MatchInfo* info = (MatchInfo*) udata;

	const char* rawDestfile = destfiles[info->matchedPatternIndex];
	const char* destfile = NULL;

	ostream* out = NULL;

	if (!writeToStdout) {
		if (rawDestfile) {
			if (useRegex) {
				destfile = info->match->format(std::string(rawDestfile), boost::format_perl).c_str();
			} else {
				destfile = rawDestfile;
			}
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

	delete info->match;
	delete info;
}
