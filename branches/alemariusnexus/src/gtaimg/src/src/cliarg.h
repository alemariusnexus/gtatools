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

#ifndef CLIARG_H_
#define CLIARG_H_

#include <map>
#include <vector>
#include <string.h>

#define PARAM_MAXLEN 2

using std::multimap;
using std::vector;
using std::pair;

class StrLess {
public:
	bool operator()(const char* s1, const char* s2) const {
		return strcmp(s1, s2) < 0;
	}
};

typedef pair<const char*, const char*> optswitch;
typedef multimap<const char*, const char*, StrLess> switchmap;
typedef vector<const char*> flagset;
typedef vector<const char*> paramset;




void SetFlagOptions(const char (*fopts)[PARAM_MAXLEN]);
void SetSwitchOptions(const char (*sopts)[PARAM_MAXLEN]);
bool ParseCommandLineArguments(int argc, char** argv, int begin);
const char* GetSwitch(const char* key);
flagset::iterator GetFlagBegin();
flagset::iterator GetFlagEnd();
paramset::iterator GetStandaloneParamBegin();
paramset::iterator GetStandaloneParamEnd();
int GetStandaloneParamCount();
bool IsFlagSet(const char* flag);


#endif /* CLIARG_H_ */
