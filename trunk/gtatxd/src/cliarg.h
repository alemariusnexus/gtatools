/*
 * cliarg.h
 *
 *  Created on: 04.01.2010
 *      Author: alemariusnexus
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
