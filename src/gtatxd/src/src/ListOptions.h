/*
 * ListOptions.h
 *
 *  Created on: 03.01.2011
 *      Author: alemariusnexus
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
	ListOptions(int sections = 0) : sections(sections) {}
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
