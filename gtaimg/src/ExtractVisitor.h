/*
 * ExtractCommand.h
 *
 *  Created on: 06.01.2010
 *      Author: alemariusnexus
 */

#ifndef EXTRACTCOMMAND_H_
#define EXTRACTCOMMAND_H_

#include "cliarg.h"
//#include <ImgVisitor.h>
#include <img/IMGVisitor.h>
#include <vector>
#include <boost/regex.hpp>

using std::vector;
using boost::regex;

class ExtractVisitor : public IMGVisitor {
public:
	ExtractVisitor(int argc, char** argv);
	virtual ~ExtractVisitor();
	bool readHeader(IMGEntry* header, void*& udata);
	void readEntry(IMGEntry* entry, istream* stream, void*& udata);

private:
	int numPatterns;
	vector<char*> temporaryStrings;
	regex** regexes;
	const char** destfiles;
	bool useRegex;
	bool sourcesOnly;
	bool writeToStdout;
};

#endif /* EXTRACTCOMMAND_H_ */
