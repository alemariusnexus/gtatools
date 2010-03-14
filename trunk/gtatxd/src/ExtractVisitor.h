/*
	Copyright 2010 David Lerch

	This file is part of gtatxd.

	gtatxd is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	gtatxd is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with gtatxd.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef EXTRACTCOMMAND_H_
#define EXTRACTCOMMAND_H_

#include <gtaformats/txd/TXDVisitor.h>
#include "cliarg.h"
#include <boost/regex.hpp>
#include <vector>
//#include <IL/il.h>

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
	//ILenum outputFormat;
};

#endif /* EXTRACTCOMMAND_H_ */
