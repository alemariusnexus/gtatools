/*
 * ExtractCommand.cpp
 *
 *  Created on: 06.01.2010
 *      Author: alemariusnexus
 */

#include "ExtractVisitor.h"
#include "cliarg.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <iostream>

using std::cerr;
using std::endl;



ILenum GetFileFormat(const char* name)
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
}


const char flagOptions[][PARAM_MAXLEN] = {
		"s", "r", "\0"
};
const char switchOptions[][PARAM_MAXLEN] = {
		"f", "m", "\0"
};



ExtractVisitor::ExtractVisitor(int argc, char** argv) {
	ilInit();

	SetFlagOptions(flagOptions);
	SetSwitchOptions(switchOptions);

	if (!ParseCommandLineArguments(argc, argv, 2)) {
		exit(1);
	}

	paramset::iterator it;

	numPatterns = GetStandaloneParamCount()-1;

	if (numPatterns < 0) {
		fprintf(stderr, "Error: No archive file given!\n");
		exit(1);
	}

	bool sourcesOnly = IsFlagSet("s");

	if (!sourcesOnly) {
		if (numPatterns%2 != 0) {
			fprintf(stderr, "Error: Last entry has no destination file!\n");
			exit(1);
		}

		numPatterns /= 2;
	}

	useRegex = IsFlagSet("r");

	regexes = new regex*[numPatterns];
	destfiles = new const char*[numPatterns];
	mipmapIndices = new int8_t[numPatterns];

	const char* format = GetSwitch("f");

	if (format) {
		outputFormat = GetFileFormat(format);
	} else {
		outputFormat = IL_TYPE_UNKNOWN;
	}


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
			fprintf(stderr, "Error compiling source pattern %d at offset %d: %s\n",
					i+1, err.position(), err.what());
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

			ILenum format = outputFormat;

			if (format == IL_TYPE_UNKNOWN) {
				format = IL_PNG;
			}

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

				int16_t width = texture->getWidth();
				int16_t height = texture->getHeight();

				uint8_t* data = new uint8_t[width * height * 4];
				texture->convert(data, rawData);
				delete[] rawData;

				ilBindImage(1);
				ilTexImage(width, height, 0, 4, IL_RGBA, IL_UNSIGNED_BYTE, data);

				if (destfile) {
					std::string destStr = match->format(std::string(destfile), boost::format_perl);
					const char* dest = destStr.c_str();
					ilSave(outputFormat, dest);
				} else {
					char ext[8];
					GetFileFormatExtension(ext, format);

					char* autogenDestFile = new char[strlen(texture->getDiffuseName()) + strlen(ext) + 2];
					sprintf(autogenDestFile, "%s.%s", texture->getDiffuseName(), ext);
					ilSave(outputFormat, autogenDestFile);
					delete[] autogenDestFile;
				}

				ilDeleteImage(1);

				delete[] data;

				break;
			}

			break;
		}
	}
}
