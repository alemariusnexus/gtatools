/*
 * ExtractOptions.h
 *
 *  Created on: 03.01.2011
 *      Author: alemariusnexus
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
