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
	}
}


const char flagOptions[][PARAM_MAXLEN] = {
		"s", "r", "\0"
};
const char switchOptions[][PARAM_MAXLEN] = {
		"f", "\0"
};


struct MatchInfo {
	boost::cmatch* match;
	int patternIndex;
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

bool ExtractVisitor::handleHeader(TXDTexture* header, void*& udata) {
	for (int i = 0 ; i < numPatterns ; i++) {
		boost::regex* reg = regexes[i];
		boost::cmatch* match = new boost::cmatch;

		if (boost::regex_match(header->getDiffuseName(), *match, *reg)) {
			/*if (!TxdArchive::isSupported(header)) {
				std::cerr << "Error: unsupported TXD format!" << std::endl;
				exit(1);
			}*/

			MatchInfo* info = new MatchInfo;
			info->match = match;
			info->patternIndex = i;
			udata = info;
			return true;
		}

		delete match;
	}

	return false;
}

bool ExtractVisitor::handleTexture(TXDTexture* header, uint8_t* bmp, void*& udata) {
	MatchInfo* info = (MatchInfo*) udata;
	const char* destfile = destfiles[info->patternIndex];

	ILenum format = outputFormat;

	if (format == IL_TYPE_UNKNOWN) {
		format = IL_PNG;
	}

	uint8_t* data = new uint8_t[header->getWidth() * header->getHeight() * 4];
	header->convert(data, bmp);

	ilBindImage(1);
	ilTexImage(header->getWidth(), header->getHeight(), 0, 4, IL_RGBA, IL_UNSIGNED_BYTE, data);

	if (destfile) {
		std::string destStr = info->match->format(std::string(destfile), boost::format_perl);
		const char* dest = destStr.c_str();

		//long long s = GetTimestamp();
		ilSave(outputFormat, dest);
		//long long e = GetTimestamp();
		//printf("        -> Took %fms\n", (e-s)/1000.0f);
	} else {
		char ext[8];
		GetFileFormatExtension(ext, format);

		char* autogenDestFile = new char[strlen(header->getDiffuseName()) + strlen(ext) + 2];
		sprintf(autogenDestFile, "%s.%s", header->getDiffuseName(), ext);
		ilSave(outputFormat, autogenDestFile);
		delete[] autogenDestFile;
	}

	ilDeleteImage(1);

	delete info->match;
	delete info;

	if (!useRegex) {
		return ++extractionCounter < numPatterns;
	} else {
		return true;
	}
}
