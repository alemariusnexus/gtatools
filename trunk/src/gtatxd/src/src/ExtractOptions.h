/*
	Copyright 2010-2013 David "Alemarius Nexus" Lerch

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

#ifndef EXTRACTOPTIONS_H_
#define EXTRACTOPTIONS_H_

#include "TextureFilter.h"
#include "MipmapRange.h"
#include <vector>
#include <gtaformats/util/File.h>


using std::vector;



enum OutputFormat
{
	OutputFormatRaw,
	OutputFormatPNG
};




class ExtractOptions {
public:
	ExtractOptions(File* dest = NULL)
			: destination(dest), filter(NULL), outputFormat(OutputFormatRaw), packMipmaps(false) {}
	~ExtractOptions();
	void setDestination(File* destination) { this->destination = destination; }
	void setFilter(TextureFilter* filter) { this->filter = filter; }
	void setOutputFormat(int format) { outputFormat = format; }
	void setPackMipmaps(bool pm) { packMipmaps = pm; }
	void addMipmapRange(const MipmapRange& range) { mipmaps.push_back(range); }
	void clearMipmapRanges() { mipmaps.clear(); }
	File* getDestination() { return destination; }
	const File* getDestination() const { return destination; }
	TextureFilter* getFilter() { return filter; }
	const TextureFilter* getFilter() const { return filter; }
	int getOutputFormat() const { return outputFormat; }
	bool isPackMipmaps() const { return packMipmaps; }
	const vector<MipmapRange>& getMipmapRanges() const { return mipmaps; }
	void setMipmapRanges(const vector<MipmapRange>& ranges) { mipmaps = ranges; }

private:
	File* destination;
	TextureFilter* filter;
	int outputFormat;
	vector<MipmapRange> mipmaps;
	bool packMipmaps;
};

#endif /* EXTRACTOPTIONS_H_ */
