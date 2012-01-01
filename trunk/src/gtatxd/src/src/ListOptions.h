/*
	Copyright 2010-2012 David "Alemarius Nexus" Lerch

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

	Additional permissions are granted, which are listed in the file
	GPLADDITIONS.
 */

#ifndef LISTOPTIONS_H_
#define LISTOPTIONS_H_


enum ListSections
{
	RasterFormat = (1 << 0),
	Compression = (1 << 1),
	Width = (1 << 2),
	Height = (1 << 3),
	BitsPerPixel = (1 << 4),
	HasAlpha = (1 << 5),
	Mipmaps = (1 << 6),
	UWrapFlags = (1 << 7),
	VWrapFlags = (1 << 8),
	FilterFlags = (1 << 9)
};


class ListOptions {
public:
	ListOptions(int sections = 0) : sections(sections), showHeader(false), commaSeparation(false) {}
	void setListSections(int sections) { this->sections = sections; }
	int getListSections() const { return sections; }
	bool isShowHeader() const { return showHeader; }
	bool isCommaSeparation() const { return commaSeparation; }
	void setShowHeader(bool sh) { showHeader = sh; }
	void setCommaSeparation(bool cs) { commaSeparation = cs; }

private:
	int sections;
	bool showHeader;
	bool commaSeparation;
};

#endif /* LISTOPTIONS_H_ */
