/*
 * ExtractCommand.cpp
 *
 *  Created on: 06.01.2010
 *      Author: alemariusnexus
 */

#include "ExtractVisitor.h"
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <fstream>

using std::ostream;
using std::ofstream;


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
			fprintf(stderr, "Error compiling source pattern %d at offset %d: %s\n",
					i+1, err.position(), err.what());
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

		printf("Writing entry %s to file %s\n", header->name, destfile);

		out = new ofstream(destfile, ofstream::out | ofstream::binary);
	} else {
		out = &std::cout;
	}

	char buffer[IMG_BLOCK_SIZE];

	for (int i = 0 ; i < header->size ; i++) {
		/*if (fread(buffer, 1, sizeof(buffer), stream) != sizeof(buffer)) {
			fprintf(stderr, "Error: Premature end of file for entry '%s'!\n", header->name);
		}*/
		stream->read(buffer, sizeof(buffer));

		//fwrite(buffer, 1, sizeof(buffer), out);
		out->write(buffer, sizeof(buffer));
	}

	if (!writeToStdout) {
		dynamic_cast<ofstream*>(out)->close();
		delete out;
	}

	delete info->match;
	delete info;
}
