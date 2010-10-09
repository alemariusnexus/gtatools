/*
	Copyright 2010 David "Alemarius Nexus" Lerch

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

#include "ExtractVisitor.h"
#include "cliarg.h"
#include <gtaformats/util/util.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <iostream>
#include <png.h>

using std::cerr;
using std::endl;
using std::cout;


void _PngErrorCallback(png_struct* png, const char* err)
{
	//throw TXDException(TXDException::PNGError, err);
	cerr << "ERROR (libpng): " << err << endl;
}

void _PngWarningCallback(png_struct* png, const char* err)
{
}



const char flagOptions[][PARAM_MAXLEN] = {
		"s", "r", "\0"
};
const char switchOptions[][PARAM_MAXLEN] = {
		"f", "m", "\0"
};



ExtractVisitor::ExtractVisitor(int argc, char** argv) {
	SetFlagOptions(flagOptions);
	SetSwitchOptions(switchOptions);

	if (!ParseCommandLineArguments(argc, argv, 2)) {
		exit(1);
	}

	paramset::iterator it;

	numPatterns = GetStandaloneParamCount()-1;

	if (numPatterns < 0) {
		cerr << "Error: No archive file given!" << endl;
		exit(1);
	}

	bool sourcesOnly = IsFlagSet("s");

	if (!sourcesOnly) {
		if (numPatterns%2 != 0) {
			cerr << "Error: Last entry has no destination file!" << endl;
			exit(1);
		}

		numPatterns /= 2;
	}

	useRegex = IsFlagSet("r");

	regexes = new const char*[numPatterns];
	destfiles = new const char*[numPatterns];
	mipmapIndices = new int8_t[numPatterns];

	/*const char* format = GetSwitch("f");

	if (format) {
		outputFormat = GetFileFormat(format);
	} else {
		outputFormat = IL_TYPE_UNKNOWN;
	}*/


	int i = 0;
	for (it = GetStandaloneParamBegin()+1 ; it != GetStandaloneParamEnd() ; it++) {
		const char* srcfile = *it;
		const char* destfile = NULL;

		const char* mipmapBegin = strrchr(srcfile, '@');

		if (mipmapBegin != NULL) {
			char* realSrcfile = new char[mipmapBegin-srcfile+1];
			strncpy(realSrcfile, srcfile, mipmapBegin-srcfile);
			realSrcfile[mipmapBegin-srcfile] = '\0';
			srcfile = realSrcfile;
			temporaryStrings.push_back(realSrcfile);
			mipmapIndices[i] = atoi(mipmapBegin+1);
		} else {
			mipmapIndices[i] = 0;
		}

		if (!sourcesOnly) {
			destfile = *++it;
		}

		regexes[i] = srcfile;
		destfiles[i] = destfile;
	}
}

ExtractVisitor::~ExtractVisitor() {
	vector<const char*>::iterator it;

	for (it = temporaryStrings.begin() ; it != temporaryStrings.end() ; it++) {
		temporaryStrings.erase(it);
		delete[] *it;
	}
}


void ExtractVisitor::handleTexture(TXDArchive* archive, TXDTexture* header)
{
	for (int i = 0 ; i < numPatterns ; i++) {
		bool match = useRegex ? WildcardMatch(regexes[i], header->getDiffuseName())
				: strcmp(regexes[i], header->getDiffuseName());

		if (match) {
			const char* destfile = destfiles[i];

			TXDTexture* texture = header;

			if (mipmapIndices[i] > header->getMipmapCount()-1) {
				cerr << "Error: Texture " << header->getDiffuseName() << " does not have a mipmap at "
						<< "index " << (int) mipmapIndices[i] << endl;
				break;
			}

			for (int8_t j = 0 ; j < header->getMipmapCount() ; j++) {
				uint8_t* rawData = archive->readTextureData(texture);

				if (j != mipmapIndices[i]) {
					delete[] rawData;
					texture = texture->generateMipmap();
					continue;
				}

				if (destfile) {
					doExtract(texture, rawData, destfile);
				} else {
					char* autogenDestFile = new char[strlen(texture->getDiffuseName()) + strlen("png") + 2];
					sprintf(autogenDestFile, "%s.%s", texture->getDiffuseName(), "png");
					doExtract(texture, rawData, autogenDestFile);
					delete[] autogenDestFile;
				}

				delete[] rawData;

				break;
			}

			break;
		}
	}
}


void ExtractVisitor::doExtract(TXDTexture* tex, uint8_t* rawData, const char* dest)
{
	char err[256];
	png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, err,
			_PngErrorCallback, _PngWarningCallback);
	if (!png) {
		return;
	}

	png_infop info = png_create_info_struct(png);
	if (!info) {
		png_destroy_write_struct(&png, NULL);
	}

	if (setjmp(png_jmpbuf(png))) {
		png_destroy_write_struct(&png, &info);
	}

	FILE* file = fopen(dest, "wb");

	png_init_io(png, file);

	int bitDepth = 8;
	int colorType;
	int bpp;

	if (tex->hasAlphaChannel()) {
		colorType = PNG_COLOR_TYPE_RGBA;
		bpp = 4;
	} else {
		colorType = PNG_COLOR_TYPE_RGB;
		bpp = 3;
	}

	int32_t width = tex->getWidth();
	int32_t height = tex->getHeight();

	png_set_IHDR(png, info, width, height, bitDepth, colorType, PNG_INTERLACE_NONE,
			PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

	uint8_t* data = new uint8_t[width * height * bpp];
	tex->convert(data, rawData, MIRROR_NONE, bpp, 0, 1, 2, tex->hasAlphaChannel() ? 3 : -1);

	uint8_t** rowData = new uint8_t*[height];

	for (int16_t i = 0 ; i < height ; i++) {
		rowData[i] = data + (i * width * bpp);
	}

	png_set_rows(png, info, rowData);

	png_write_png(png, info, PNG_TRANSFORM_IDENTITY, NULL);

	delete[] data;
	delete[] rowData;

	png_destroy_write_struct(&png, &info);

	fclose(file);
}
