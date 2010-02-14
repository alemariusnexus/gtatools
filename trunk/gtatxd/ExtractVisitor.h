/*
 * ExtractCommand.h
 *
 *  Created on: 06.01.2010
 *      Author: alemariusnexus
 */

#ifndef EXTRACTCOMMAND_H_
#define EXTRACTCOMMAND_H_

#include <txd/TXDVisitor.h>
#include "cliarg.h"
#include <boost/regex.hpp>
#include <vector>
#include <IL/il.h>

using std::vector;
using boost::regex;

class ExtractVisitor : public TXDVisitor {
public:
	ExtractVisitor(int argc, char** argv);
	virtual ~ExtractVisitor();
	void handleTexture(TXDArchive* archive, TXDTexture* header);

private:
	vector<const char*> temporaryStrings;
	regex** regexes;
	const char** destfiles;
	int8_t* mipmapIndices;
	int numPatterns;
	int extractionCounter;
	bool useRegex;
	ILenum outputFormat;
};

#endif /* EXTRACTCOMMAND_H_ */
