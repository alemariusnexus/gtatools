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

#include "cliarg.h"
#include "ListVisitor.h"
#include <cstdio>
#include <cstdlib>


const char flags[][PARAM_MAXLEN] = {
		"s", "o", "b", "\0"
};
const char switches[][PARAM_MAXLEN] = {
		"f", "\0"
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

void ListVisitor::readEntry(IMGEntry* entry, InputStream* stream, void*& udata) {}
