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
#include <stdlib.h>
#include <string.h>
#include <stdio.h>


switchmap switches;
paramset standaloneParams;
flagset flags;

const char (*flagOptions)[PARAM_MAXLEN];
const char (*switchOptions)[PARAM_MAXLEN];


void SetFlagOptions(const char (*fopts)[PARAM_MAXLEN]) {
	flagOptions = fopts;

}

void SetSwitchOptions(const char (*sopts)[PARAM_MAXLEN]) {
	switchOptions = sopts;
}

bool ParseCommandLineArguments(int argc, char** argv, int begin) {
	switches.clear();

	for (int i = begin ; i < argc ; i++) {
		if (argv[i][0] == '-'  &&  argv[i][1] != '\0') {
			bool isFlag = false;

			for (int j = 0 ; flagOptions[j][0] != '\0' ; j++) {
				if (strcmp(argv[i]+1, flagOptions[j]) == 0) {
					flags.push_back(flagOptions[j]);
					isFlag = true;
					break;
				}
			}

			if (!isFlag) {
				bool valid = false;

				for (int j = 0 ; switchOptions[j][0] != '\0' ; j++) {
					if (strcmp(argv[i]+1, switchOptions[j]) == 0) {
						if (i+1 >= argc) {
							printf("Error: No value given for switch '%s'!\n", argv[i]);
							return false;
						}

						switches.insert(optswitch(argv[i]+1, argv[i+1]));
						valid = true;
					}
				}

				if (!valid) {
					printf("Error: Unknown switch '%s'!\n", argv[i]);
					return false;
				}
			}
		} else {
			for (int j = i ; j < argc ; j++) {
				standaloneParams.push_back(argv[j]);
			}

			break;
		}
	}

	return true;
}

const char* GetSwitch(const char* key) {
	switchmap::iterator it = switches.find(key);
	if (it != switches.end()) {
		return it->second;
	}

	return NULL;
}

flagset::iterator GetFlagBegin() {
	return flags.begin();
}

flagset::iterator GetFlagEnd() {
	return flags.end();
}

paramset::iterator GetStandaloneParamBegin() {
	return standaloneParams.begin();
}

paramset::iterator GetStandaloneParamEnd() {
	return standaloneParams.end();
}

bool IsFlagSet(const char* flag) {
	flagset::iterator it;
	for (it = flags.begin() ; it != flags.end() ; it++) {
		if (strcmp(*it, flag) == 0) {
			return true;
		}
	}

	return false;
}

int GetStandaloneParamCount() {
	return standaloneParams.size();
}
