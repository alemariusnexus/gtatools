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

#ifndef EXTRACTCOMMAND_H_
#define EXTRACTCOMMAND_H_

#include "cliarg.h"
//#include <ImgVisitor.h>
#include <gtaformats/img/IMGVisitor.h>
#include <vector>
#include <boost/regex.hpp>

using std::vector;
using boost::regex;

class ExtractVisitor : public IMGVisitor {
public:
	ExtractVisitor(int argc, char** argv);
	virtual ~ExtractVisitor();
	bool readHeader(IMGEntry* header, void*& udata);
	void readEntry(IMGEntry* entry, InputStream* stream, void*& udata);

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
