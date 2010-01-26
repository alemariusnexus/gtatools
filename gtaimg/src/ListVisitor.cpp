/*
 * ListCommand.cpp
 *
 *  Created on: 06.01.2010
 *      Author: alemariusnexus
 */

#include "cliarg.h"
#include "ListVisitor.h"
#include <cstdio>
#include <cstdlib>


const char flags[][PARAM_MAXLEN] = {
		"s", "o", "b", "\0"
};
const char switches[][PARAM_MAXLEN] = {
		"\0"
};

ListVisitor::ListVisitor(int argc, char** argv) {
	SetFlagOptions(flags);
	SetSwitchOptions(switches);

	if (!ParseCommandLineArguments(argc, argv, 2)) {
		exit(1);
	}

	if (GetStandaloneParamCount() == 0) {
		fprintf(stderr, "Error: No archive file given!\n");
		exit(1);
	}
}

bool ListVisitor::readHeader(IMGEntry* entry, void*& udata) {
	printf("%-24s", entry->name);

	flagset::iterator flag;

	for (flag = GetFlagBegin() ; flag != GetFlagEnd() ; flag++) {
		if (strcmp(*flag, "o") == 0) {
			if (IsFlagSet("b")) {
				printf("%-13d", entry->offset);
			} else {
				printf("%-13d", entry->offset * IMG_BLOCK_SIZE);
			}
		} else if (strcmp(*flag, "s") == 0) {
			if (IsFlagSet("b")) {
				printf("%-13d", entry->size);
			} else {
				printf("%-13d", entry->size * IMG_BLOCK_SIZE);
			}
		}
	}

	printf("\n");

	return false;
}

void ListVisitor::readEntry(IMGEntry* entry, istream* stream, void*& udata) {}
