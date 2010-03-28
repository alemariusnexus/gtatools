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

#include "ExtractVisitor.h"
#include "cliarg.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <iostream>
#include <png.h>

using std::cerr;
using std::endl;
using std::cout;



/*ILenum GetFileFormat(const char* name)
{
	if (strcmp(name, "bmp") == 0) {
		return IL_BMP;
	} else if (strcmp(name, "png") == 0) {
		return IL_PNG;
	} else if (strcmp(name, "jpg") == 0) {
		return IL_JPG;
	} else if (strcmp(name, "pnm") == 0) {
		return IL_PNM;
	} else if (strcmp(name, "psd") == 0) {
		return IL_PSD;
	} else if (strcmp(name, "tga") == 0) {
		return IL_TGA;
	} else if (strcmp(name, "sgi") == 0) {
		return IL_SGI;
	} else if (strcmp(name, "tif") == 0) {
		return IL_TIF;
	}

	return IL_TYPE_UNKNOWN;
}

void GetFileFormatExtension(char* dest, ILenum format)
{
	switch (format) {
	case IL_BMP:
		strcpy(dest, "bmp");
		break;
	case IL_PNG:
		strcpy(dest, "png");
		break;
	case IL_JPG:
		strcpy(dest, "jpg");
		break;
	case IL_PNM:
		strcpy(dest, "pnm");
		break;
	case IL_PSD:
		strcpy(dest, "psd");
		break;
	case IL_TGA:
		strcpy(dest, "tga");
		break;
	case IL_SGI:
		strcpy(dest, "sgi");
		break;
	case IL_TIF:
		strcpy(dest, "tif");
		break;
	}
}*/


const char flagOptions[][PARAM_MAXLEN] = {
		"s", "r", "\0"
};
const char switchOptions[][PARAM_MAXLEN] = {
		"f", "m", "\0"
};



ExtractVisitor::ExtractVisitor(int argc, char** argv) {
	//ilInit();

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

	regexes = new regex*[numPatterns];
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

		regex* reg;

		if (!useRegex) {
			char* quotedSrcfile = new char[strlen(srcfile)+5];
			sprintf(quotedSrcfile, "\\Q%s\\E", srcfile);
			srcfile = quotedSrcfile;
			temporaryStrings.push_back(quotedSrcfile);
		}

		try {
			reg = new regex(srcfile);
		} catch (boost::regex_error& err) {
			cerr << "Error compiling source pattern " << i+1 << " at offset " << err.position()
					<< ": " << err.what() << endl;
			exit(1);
		}

		regexes[i] = reg;
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
		boost::regex* reg = regexes[i];
		boost::cmatch* match = new boost::cmatch;

		if (boost::regex_match(header->getDiffuseName(), *match, *reg)) {
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
					std::string destStr = match->format(std::string(destfile), boost::format_perl);
					const char* dest = destStr.c_str();
					texture->writePNG(dest, rawData);
				} else {
					char* autogenDestFile = new char[strlen(texture->getDiffuseName()) + strlen("png") + 2];
					sprintf(autogenDestFile, "%s.%s", texture->getDiffuseName(), "png");
					texture->writePNG(autogenDestFile, rawData);
					delete[] autogenDestFile;
				}

				delete[] rawData;

				break;
			}

			break;
		}
	}
}
