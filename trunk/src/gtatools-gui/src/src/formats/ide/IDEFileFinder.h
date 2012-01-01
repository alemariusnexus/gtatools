/*
	Copyright 2010-2012 David "Alemarius Nexus" Lerch

	This file is part of gtatools-gui.

	gtatools-gui is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	gtatools-gui is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with gtatools-gui.  If not, see <http://www.gnu.org/licenses/>.

	Additional permissions are granted, which are listed in the file
	GPLADDITIONS.
 */

#ifndef IDEFILEFINDER_H_
#define IDEFILEFINDER_H_

#include <gtaformats/ide/IDEStatement.h>
#include <gtaformats/util/FileFinder.h>
#include <QtCore/QMap>



class IDEFileFinder : public FileFinder {
public:
	IDEFileFinder(int32_t id, idetype_t types, FileFinder* meshFinder = NULL, FileFinder* txdFinder = NULL);
	int32_t getID() const { return id; }
	void setID(int32_t id) { this->id = id; }
	virtual bool matches(const File& file);
	int getMatchedLine(const File& file) const { return fileLines[file]; }

private:
	int32_t id;
	idetype_t types;
	FileFinder* meshFinder;
	FileFinder* txdFinder;
	QMap<File, int> fileLines;
};

#endif /* IDEFILEFINDER_H_ */
