/*
	Copyright 2010-2014 David "Alemarius Nexus" Lerch

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

#ifndef REGEXFILEFINDER_H_
#define REGEXFILEFINDER_H_

#include <nxcommon/file/FileFinder.h>
#include <QtCore/QRegExp>



class RegexFileFinder : public FileFinder {
public:
	RegexFileFinder(const QRegExp& regex, bool exactMatch = true);
	const QRegExp& getRegex() const { return regex; }
	bool isExactMatch() const { return exactMatch; }
	void setRegex(const QRegExp& regex) { this->regex = regex; }
	void setExactMatch(bool em) { exactMatch = em; }
	virtual bool matches(const File& file);

private:
	QRegExp regex;
	bool exactMatch;
};

#endif /* REGEXFILEFINDER_H_ */
