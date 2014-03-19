/*
	Copyright 2010-2014 David "Alemarius Nexus" Lerch

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

#include <nxcommon/CLIParser.h>
#include <gtaformats/gtatxd.h>
#include <nxcommon/file/File.h>
#include <nxcommon/strutil.h>
#include <cstdio>
#include <vector>
#include <iostream>
#include <cstring>
#include <fstream>
#include "TextureFilter.h"
#include "NullTextureFilter.h"
#include "WildcardTextureFilter.h"
#include "MipmapRange.h"
#include "ExtractOptions.h"
#include "ListOptions.h"
#include <png.h>
#include <cmath>
#include <istream>

using std::vector;
using std::cin;
using std::cout;
using std::ostream;
using std::ofstream;
using std::istream;


const int CommandList = 0;
const int CommandExtract = 1;



void pngWriteCallback(png_structp png, png_bytep buffer, png_size_t size)
{
	ostream* out = (ostream*) png_get_io_ptr(png);
	out->write((char*) buffer, size);
}


void pngFlushCallback(png_structp png)
{
	ostream* out = (ostream*) png_get_io_ptr(png);
	out->flush();
}



void extractTexture(const ExtractOptions& opts, TXDArchive* txd, TXDTextureHeader* tex);
void extractMipmapPNG(ostream* out, TXDTextureHeader* tex, int16_t w, int16_t h, uint8_t* data, int mipmap);
uint8_t* buildPackedMipmap(const ExtractOptions& opts, TXDTextureHeader* tex, uint8_t* data,
		int& packedW, int& packedH);
void listTexture(const ListOptions& opts, TXDArchive* txd, TXDTextureHeader* tex);



int main(int argc, char** argv)
{
	CLIParser cli;

	argc--;
	argv++;

	int helpOpt = cli.addOption(0, "help", "Shows help and exits.");
	int listOpt = cli.addOption('l', "list", "Lists all textures.");
	int extractOpt = cli.addOption('x', "extract", "Extracts textures from the archives.");
	int fileOpt = cli.addOption('f', "file", "Add a new input file.", true);

	// List options
	int rasterFormatOpt = cli.addOption('R', "raster-format", "Show texture raster formats. It is a "
			"string of comma-seperated properties, starting with the base raster format. Possible values for "
			"the base raster format are: A1R5G5B5, R5G6B5, R4G4B4A4, LUM8, B8G8R8A8, B8G8R8, R5G5B5. The "
			"other properties are the possible raster format extensions, which may be: M (mipmaps included), "
			"AM (automatically create mipmaps), P4 (paletted with 2^4 colors), P8 (paletted with 2^8 "
			"colors).");
	int comprOpt = cli.addOption('C', "compression", "Show compression type. Possible values are "
			"DXT1, DXT3 or None.");
	int widthOpt = cli.addOption('W', "width", "Show texture width in pixels.");
	int heightOpt = cli.addOption('H', "height", "Show texture height in pixels.");
	int bppOpt = cli.addOption('B', "bpp", "Shows the number of bits per pixel. Usually 8, 16 or 32");
	int alphaOpt = cli.addOption('A', "alpha", "Shows 'contains alpha' state (Yes/No).");
	int mipmapOpt = cli.addOption('M', "mipmap", "Shows number of mipmaps.");
	int uWrapOpt = cli.addOption('U', "wrap-u", "Shows U wrapping flags. Possible values are CLAMP, MIRROR, "
			"NONE, WRAP. If an unknown value was found, a string of format UXXXXX is printed, where XXXXX is "
			"the hexadecimal value of the U wrap option");
	int vWrapOpt = cli.addOption('V', "wrap-v", "Shows V wrapping flags. For possible values, see -U.");
	int filterOpt = cli.addOption('F', "filter", "Shows filter flags. Possible values are: "
			"N (Nearest), L (Linear), NMIPN (Nearest Mipmap Nearest), NMIPL (Nearest Mipmap Linear), "
			"LMIPN (Linear Mipmap Nearest), LMIPL (Linear Mipmap Linear), NONE. If an unknown value was "
			"found, a string of format UXXXX is printed, where XXXX is the hexadecimal value of the filter "
			"flags.");
	int commaOpt = cli.addOption('c', "comma-separation", "Separate fields by commas instead of spaces.");
	int headerOpt = cli.addOption('h', "list-header", "Show header.");

	// Extract options
	int outOpt = cli.addOption('o', "opt", "Specify the output file.", true);
	int exMipmapOpt = cli.addOption('m', "extract-mipmaps", "Selects the mipmaps to extract. It is a "
			"comma-separated list of mipmaps (0 is the base mipmap). Each of these mipmap values may also "
			"be a range of the form MIN-MAX to select a range of mipmaps. Values which are out of range "
			"will be ignored.",
			true);
	int packOpt = cli.addOption('p', "pack-mipmaps", "Packs all mipmaps together in one image. This builds "
			"a typical mipmap image, where the base image is on the left, and all other mipmaps are placed "
			"to the right of it, in decreasing size from top to bottom.");
	int formatOpt = cli.addOption('t', "format", "Specify the output format for the images. This will "
			"override the default behavior as determined from the file name of the output file. It can be "
			"either raw or png. In raw mode, the plain raster data will be printed.",
			true);


	int opt;
	char* arg;
	int command = -1;
	vector<File*> inputFiles;
	vector<ExtractOptions*> exOpts;
	File* currentOutputFile = new File(".");
	int currentOutputFormat = -1;
	vector<MipmapRange> currentMipmaps;
	currentMipmaps.push_back(MipmapRange(0));
	bool packMipmaps = false;

	ListOptions lsOpts;

	while ((opt = cli.parse(argc, argv, arg))  >=  0) {
		if (opt == helpOpt) {
			cli.setPrintIndentation("    ");
			cli.setTextWrapLength(75);

			printf("gtatxd - Lists and extracts textures from GTA TXD files.\n\n");

			printf("USAGE:\n");
			printf("  gtatxd [OPTION...] FILE...\n\n");

			printf("DESCRIPTION:\n");
			printf("  You can use gtatxd to extract and display information of textures from GTA\n");
			printf("  Texture Dictionaries (TXDs). You can list the textures (-l), optionally\n");
			printf("  showing header information, or extract them (-x) to image files.\n\n");

			printf("  LIST:\n");
			printf("    In list mode, gtatxd will display information about all textures in the\n");
			printf("    input files. They will be displayed in a table-like structure. By default,\n");
			printf("    the diffuse and alpha names are printed. You can include other data by\n");
			printf("    using the list flags. The FILEs will be interpreted as patterns of the\n");
			printf("    textures to be listed.\n\n");

			printf("  EXTRACT:\n");
			printf("    Extracts the image data of selected textures either as raw raster data or\n");
			printf("    as PNG. The format of the file is chosen by looking at the name of the\n");
			printf("    output file, defaulting to raw if it can't be determined. In extraction\n");
			printf("    mode, the FILEs will be interpreted as patterns of the texture names to\n");
			printf("    extract.\n\n");

			printf("FLAGS:\n");

			printf("  Command flags:\n");
			cli.printOption(helpOpt).printOption(listOpt).printOption(extractOpt);
			printf("\n");

			printf("  General flags:\n");
			cli.printOption(fileOpt);
			printf("\n");

			printf("  List flags:\n");
			cli.printOption(commaOpt).printOption(headerOpt);
			cli.printOption(rasterFormatOpt).printOption(comprOpt).printOption(widthOpt)
					.printOption(heightOpt).printOption(bppOpt).printOption(alphaOpt).printOption(mipmapOpt)
					.printOption(uWrapOpt).printOption(vWrapOpt).printOption(filterOpt);
			printf("\n");

			printf("  Extract flags:\n");
			cli.printOption(outOpt).printOption(exMipmapOpt).printOption(packOpt).printOption(formatOpt);

			return 0;
		} else if (opt == listOpt) {
			command = CommandList;
		} else if (opt == extractOpt) {
			command = CommandExtract;
		} else if (opt == fileOpt) {
			if (strcmp(arg, "-") == 0) {
				inputFiles.push_back(NULL);
			} else {
				File* file = new File(arg);

				if (!file->exists()) {
					fprintf(stderr, "ERROR: Input file %s does not exist!\n",
							file->getPath().toString().get());
					return 2;
				}

				inputFiles.push_back(file);
			}
		} else if (opt == rasterFormatOpt) {
			lsOpts.setListSections(lsOpts.getListSections() | RasterFormat);
		} else if (opt == comprOpt) {
			lsOpts.setListSections(lsOpts.getListSections() | Compression);
		} else if (opt == widthOpt) {
			lsOpts.setListSections(lsOpts.getListSections() | Width);
		} else if (opt == heightOpt) {
			lsOpts.setListSections(lsOpts.getListSections() | Height);
		} else if (opt == bppOpt) {
			lsOpts.setListSections(lsOpts.getListSections() | BitsPerPixel);
		} else if (opt == alphaOpt) {
			lsOpts.setListSections(lsOpts.getListSections() | HasAlpha);
		} else if (opt == mipmapOpt) {
			lsOpts.setListSections(lsOpts.getListSections() | Mipmaps);
		} else if (opt == uWrapOpt) {
			lsOpts.setListSections(lsOpts.getListSections() | UWrapFlags);
		} else if (opt == vWrapOpt) {
			lsOpts.setListSections(lsOpts.getListSections() | VWrapFlags);
		} else if (opt == filterOpt) {
			lsOpts.setListSections(lsOpts.getListSections() | FilterFlags);
		} else if (opt == outOpt) {
			if (strcmp(arg, "-") == 0) {
				currentOutputFile = NULL;
			} else {
				if (currentOutputFile)
					delete currentOutputFile;

				currentOutputFile = new File(arg);

				if (	!currentOutputFile->exists()
						||  !currentOutputFile->isDirectory()
				) {
					CString ext = currentOutputFile->getPath().getExtension();
					char* lExt = new char[ext.length() + 1];
					strtolower(lExt, ext.get());

					if (currentOutputFormat == -1) {
						if (strcmp(lExt, "png") == 0) {
							currentOutputFormat = OutputFormatPNG;
						} else {
							currentOutputFormat = OutputFormatRaw;
						}
					}

					delete[] lExt;
				}
			}
		} else if (opt == exMipmapOpt) {
			currentMipmaps.clear();

			char* range = strtok(arg, ",");
			while (range != NULL) {
				char* rangeDelim;
				int from = strtol(range, &rangeDelim, 10);

				if (rangeDelim  &&  *rangeDelim == '-') {
					int to = strtol(rangeDelim+1, NULL, 10);

					currentMipmaps.push_back(MipmapRange(from, to));

					/*for (int i = from ; i <= to ; i++) {
						currentMipmaps.push_back(i);
					}*/
				} else {
					//currentMipmaps.push_back(from);
					currentMipmaps.push_back(MipmapRange(from));
				}

				range = strtok(NULL, ",");
			}
		} else if (opt == packOpt) {
			packMipmaps = true;
		} else if (opt == formatOpt) {
			if (strcmp(arg, "png") == 0) {
				currentOutputFormat = OutputFormatPNG;
			} else if (strcmp(arg, "raw") == 0) {
				currentOutputFormat = OutputFormatRaw;
			}
		} else if (opt == commaOpt) {
			lsOpts.setCommaSeparation(true);
		} else if (opt == headerOpt) {
			lsOpts.setShowHeader(true);
		} else if (opt == 0) {
			ExtractOptions* opts = new ExtractOptions;

			if (currentOutputFile) {
				opts->setDestination(new File(*currentOutputFile));
			} else {
				opts->setDestination(NULL);
			}

			opts->setFilter(new WildcardTextureFilter(arg));

			if (currentOutputFormat == -1) {
				currentOutputFormat = OutputFormatRaw;
			}

			opts->setOutputFormat(currentOutputFormat);
			opts->setMipmapRanges(currentMipmaps);
			opts->setPackMipmaps(packMipmaps);
			exOpts.push_back(opts);
			currentOutputFormat = -1;
			packMipmaps = false;
		}
	}

	if (currentOutputFormat == -1) {
		currentOutputFormat = OutputFormatRaw;
	}

	if (exOpts.size() == 0) {
		ExtractOptions* opts = new ExtractOptions;

		if (currentOutputFile) {
			opts->setDestination(new File(*currentOutputFile));
		} else {
			opts->setDestination(NULL);
		}

		opts->setFilter(new NullTextureFilter);
		opts->setOutputFormat(currentOutputFormat);
		exOpts.push_back(opts);
	}

	int numExOpts = exOpts.size();
	ExtractOptions** opts = new ExtractOptions*[exOpts.size()];
	vector<ExtractOptions*>::iterator pit;
	int i = 0;
	for (pit = exOpts.begin() ; pit != exOpts.end() ; pit++) {
		opts[i++] = *pit;
	}

	if (inputFiles.size() == 0) {
		inputFiles.push_back(NULL);
	}

	if (command == CommandList  &&  lsOpts.isShowHeader()) {
		bool cs = lsOpts.isCommaSeparation();
		char sep = cs ? ',' : ' ';

		printf("%-*s%c%-*s", cs ? 0 : 32, "DIFFUSE NAME", sep, cs ? 0 : 32, "ALPHA NAME");

		if ((lsOpts.getListSections() & RasterFormat) != 0) {
			printf("%c%-*s", sep, cs ? 0 : 19, "RASTER FORMAT");
		}
		if ((lsOpts.getListSections() & Compression) != 0) {
			printf("%c%-*s", sep, cs ? 0 : 6, "COMPR");
		}
		if ((lsOpts.getListSections() & Width) != 0) {
			printf("%c%-*s", sep, cs ? 0 : 5, "WIDTH");
		}
		if ((lsOpts.getListSections() & Height) != 0) {
			printf("%c%-*s", sep, cs ? 0 : 5, "HGHT");
		}
		if ((lsOpts.getListSections() & BitsPerPixel) != 0) {
			printf("%c%-*s", sep, cs ? 0 : 3, "BPP");
		}
		if ((lsOpts.getListSections() & HasAlpha) != 0) {
			printf("%c%-*s", sep, cs ? 0 : 5, "ALPHA");
		}
		if ((lsOpts.getListSections() & Mipmaps) != 0) {
			printf("%c%-*s", sep, cs ? 0 : 3, "MIP");
		}
		if ((lsOpts.getListSections() & UWrapFlags) != 0) {
			printf("%c%-*s", sep, cs ? 0 : 6, "U WRAP");
		}
		if ((lsOpts.getListSections() & VWrapFlags) != 0) {
			printf("%c%-*s", sep, cs ? 0 : 6, "V WRAP");
		}
		if ((lsOpts.getListSections() & FilterFlags) != 0) {
			printf("%c%-*s", sep, cs ? 0 : 5, "FLTR");
		}

		printf("\n\n");
	}

	vector<File*>::iterator fit;
	for (fit = inputFiles.begin() ; fit != inputFiles.end() ; fit++) {
		File* file = *fit;

		TXDArchive* txd;

		if (file) {
			txd = new TXDArchive(*file);
		} else {
			txd = new TXDArchive(&cin);
		}

		//for (int16_t i = 0 ; i < txd->getTextureCount() ; i++) {
		for (TXDArchive::TextureIterator it = txd->getHeaderBegin() ; it != txd->getHeaderEnd() ; it++) {
			TXDTextureHeader* tex = *it;

			for (int j = 0 ; j < numExOpts ; j++) {
				ExtractOptions* opt = opts[j];

				if (opt->getFilter()->process(*tex)) {

					if (command == CommandList) {
						listTexture(lsOpts, txd, tex);
					} else if (command == CommandExtract) {
						extractTexture(*opt, txd, tex);
					}

					break;
				}
			}
		}

		delete txd;
	}

	return 0;
}




void listTexture(const ListOptions& opts, TXDArchive* txd, TXDTextureHeader* tex)
{
	bool cs = opts.isCommaSeparation();
	char sep = cs ? ',' : ' ';

	printf("%-*s%c%-*s", cs ? 0 : 32, tex->getDiffuseName().get() ? tex->getDiffuseName().get() : "-", sep,
			cs ? 0 : 32, !tex->getAlphaName().isNull() ? tex->getAlphaName().get() : "-");

	if ((opts.getListSections() & RasterFormat)  !=  0) {
		char rasterFormat[19];
		TxdGetRasterFormatName(rasterFormat, tex->getRasterFormat());

		int32_t ext = tex->getRasterFormatExtension();

		if (( ext & RasterFormatEXTAutoMipmap)  !=  0) {
			strcat(rasterFormat, ",AM");
		}
		if ((ext & RasterFormatEXTMipmap)  !=  0) {
			strcat(rasterFormat, ",M");
		}
		if ((ext & RasterFormatEXTPAL4)  !=  0) {
			strcat(rasterFormat, ",P4");
		}
		if ((ext & RasterFormatEXTPAL8)  !=  0) {
			strcat(rasterFormat, ",P8");
		}

		printf("%c%-*s", sep, cs ? 0 : 19, rasterFormat);
	}

	if ((opts.getListSections() & Compression)  !=  0) {
		switch (tex->getCompression()) {
		case NONE:
			printf("%c%-*s", sep, cs ? 0 : 6, "None");
			break;

		case DXT1:
			printf("%c%-*s", sep, cs ? 0 : 6, "DXT1");
			break;

		case DXT3:
			printf("%c%-*s", sep, cs ? 0 : 6, "DXT3");
			break;

		case PVRTC2:
			printf("%c%-*s", sep, cs ? 0 : 6, "PVRTC2");
			break;

		case PVRTC4:
			printf("%c%-*s", sep, cs ? 0 : 6, "PVRTC4");
			break;
		}
	}

	if ((opts.getListSections() & Width)  !=  0)
		printf("%c%-*d", sep, cs ? 0 : 5, tex->getWidth());
	if ((opts.getListSections() & Height)  !=  0)
		printf("%c%-*d", sep, cs ? 0 : 5, tex->getHeight());
	if ((opts.getListSections() & BitsPerPixel)  !=  0)
		printf("%c%-*d", sep, cs ? 0 : 3, tex->getBytesPerPixel()*8);
	if ((opts.getListSections() & HasAlpha)  !=  0)
		printf("%c%-*s", sep, cs ? 0 : 5, tex->isAlphaChannelUsed() ? "Yes" : "No");
	if ((opts.getListSections() & Mipmaps)  !=  0)
		printf("%c%-*d", sep, cs ? 0 : 3, tex->getMipmapCount());

	if ((opts.getListSections() & UWrapFlags)  !=  0) {
		switch (tex->getUWrapFlags()) {
		case WrapClamp:
			printf("%c%-*s", sep, cs ? 0 : 6, "CLAMP");
			break;
		case WrapMirror:
			printf("%c%-*s", sep, cs ? 0 : 6, "MIRROR");
			break;
		case WrapNone:
			printf("%c%-*s", sep, cs ? 0 : 6, "NONE");
			break;
		case WrapWrap:
			printf("%c%-*s", sep, cs ? 0 : 6, "WRAP");
			break;
		default:
			printf("%cU%05X", sep, tex->getUWrapFlags());
			break;
		}
	}

	if ((opts.getListSections() & VWrapFlags)  !=  0) {
		switch (tex->getVWrapFlags()) {
		case WrapClamp:
			printf("%c%-*s", sep, cs ? 0 : 6, "CLAMP");
			break;
		case WrapMirror:
			printf("%c%-*s", sep, cs ? 0 : 6, "MIRROR");
			break;
		case WrapNone:
			printf("%c%-*s", sep, cs ? 0 : 6, "NONE");
			break;
		case WrapWrap:
			printf("%c%-*s", sep, cs ? 0 : 6, "WRAP");
			break;
		default:
			printf("%cU%05X", sep, tex->getVWrapFlags());
			break;
		}
	}

	if ((opts.getListSections() & FilterFlags)  !=  0) {
		switch (tex->getFilterFlags()) {
		case FilterLinear:
			printf("%c%-*s", sep, cs ? 0 : 5, "L");
			break;
		case FilterNearest:
			printf("%c%-*s", sep, cs ? 0 : 5, "N");
			break;
		case FilterLinearMipLinear:
			printf("%c%-*s", sep, cs ? 0 : 5, "LMIPL");
			break;
		case FilterLinearMipNearest:
			printf("%c%-*s", sep, cs ? 0 : 5, "LMIPN");
			break;
		case FilterMipLinear:
			printf("%c%-*s", sep, cs ? 0 : 5, "NMIPL");
			break;
		case FilterMipNearest:
			printf("%c%-*s", sep, cs ? 0 : 5, "NMIPN");
			break;
		case FilterNone:
			printf("%c%-*s", sep, cs ? 0 : 5, "NONE");
			break;
		default:
			printf("%cU%04X", sep, tex->getFilterFlags());
		}
	}

	printf("\n");
}


void extractTexture(const ExtractOptions& opts, TXDArchive* txd, TXDTextureHeader* tex)
{
	uint8_t* data = txd->getTextureData(tex);
	uint8_t* dataStart = data;
	const File* dest = opts.getDestination();

	if (opts.isPackMipmaps()) {
		int packedW, packedH;
		uint8_t* packed = buildPackedMipmap(opts, tex, data, packedW, packedH);

		ostream* out = new ofstream(dest->getPath().toString().get(), ofstream::out | ofstream::binary);

		png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL,
				NULL);
		if (!png) {
			exit(10);
		}

		png_infop info = png_create_info_struct(png);
		if (!info) {
			png_destroy_write_struct(&png, (png_infopp) NULL);
			exit(11);
		}

		if (setjmp(png_jmpbuf(png))) {
			png_destroy_write_struct(&png, (png_infopp) info);
			exit(12);
		}

		png_set_write_fn(png, (void*) out, pngWriteCallback, pngFlushCallback);

		png_set_IHDR(png, info, packedW, packedH, 8, PNG_COLOR_TYPE_RGB_ALPHA, PNG_INTERLACE_NONE,
				PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

		const char* softwareKey = "Software";
		const char* softwareText = "gtatxd using gtatools " GTATOOLS_VERSION;

		const char* commentKey = "Comment";
		const char* commentText = "Automatically converted from a GTA TXD texture.";

		char* softwareKeyCpy = new char[strlen(softwareKey)];
		char* softwareTextCpy = new char[strlen(softwareText)];
		strcpy(softwareKeyCpy, softwareKey);
		strcpy(softwareTextCpy, softwareText);
		png_text software;
		software.key = softwareKeyCpy;
		software.text = softwareTextCpy;
		software.text_length = strlen(softwareTextCpy);
		software.compression = PNG_TEXT_COMPRESSION_NONE;

		char* commentKeyCpy = new char[strlen(commentKey)];
		char* commentTextCpy = new char[strlen(commentText)];
		strcpy(commentKeyCpy, commentKey);
		strcpy(commentTextCpy, commentText);
		png_text comment;
		comment.key = commentKeyCpy;
		comment.text = commentTextCpy;
		comment.text_length = strlen(commentTextCpy);
		comment.compression = PNG_TEXT_COMPRESSION_NONE;

		png_text texts[] = {software, comment};

		png_set_text(png, info, texts, sizeof(texts)/sizeof(texts[0]));

		uint8_t** rowData = new uint8_t*[packedH];

		for (int k = 0 ; k < packedH ; k++) {
			rowData[k] = packed + k*packedW*4;
		}

		png_set_rows(png, info, rowData);
		png_write_png(png, info, PNG_TRANSFORM_IDENTITY, NULL);

		png_destroy_write_struct(&png, &info);

		delete[] rowData;
		delete[] packed;

		delete[] softwareKeyCpy;
		delete[] softwareTextCpy;
		delete[] commentKeyCpy;
		delete[] commentTextCpy;
	} else {
		int16_t w = tex->getWidth();
		int16_t h = tex->getHeight();
		const vector<MipmapRange>& ranges = opts.getMipmapRanges();

		for (int i = 0 ; i < tex->getMipmapCount() ; i++) {
			vector<MipmapRange>::const_iterator it;

			int mipmapSize = tex->computeMipmapDataSize(i);

			for (it = ranges.begin() ; it != ranges.end() ; it++) {
				const MipmapRange& range = *it;

				if (range.contains(i)) {
					ostream* out;

					if (dest) {
						if (	!dest->exists()
								||  !dest->isDirectory()
						) {
							// Plain file
							out = new ofstream(dest->getPath().toString().get(),
									ofstream::out | ofstream::binary | ofstream::app);
						} else {
							char fname[50];
							strcpy(fname, tex->getDiffuseName().get());

							sprintf(fname+strlen(fname), "@%d", i);

							switch (opts.getOutputFormat()) {
							case OutputFormatRaw:
								strcat(fname, ".bin");
								break;
							case OutputFormatPNG:
								strcat(fname, ".png");
								break;
							}

							FilePath path(dest->getPath(), fname);

							out = new ofstream(path.toString().get(), ofstream::out | ofstream::binary);
						}
					} else {
						out = &cout;
					}

					switch (opts.getOutputFormat()) {
					case OutputFormatRaw:
						out->write((char*) data, mipmapSize);
						out->flush();
						break;

					case OutputFormatPNG:
						extractMipmapPNG(out, tex, w, h, data, i);
						break;
					}

					if (dest) {
						delete out;
					}
				}
			}

			data += mipmapSize;

			w /= 2;
			h /= 2;
		}
	}

	delete[] dataStart;
}


void extractMipmapPNG(ostream* out, TXDTextureHeader* tex, int16_t w, int16_t h, uint8_t* data, int mipmap)
{
	png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL,
			NULL);
	if (!png) {
		exit(10);
	}

	png_infop info = png_create_info_struct(png);
	if (!info) {
		png_destroy_write_struct(&png, (png_infopp) NULL);
		exit(11);
	}

	if (setjmp(png_jmpbuf(png))) {
		png_destroy_write_struct(&png, (png_infopp) info);
		exit(12);
	}

	png_set_write_fn(png, (void*) out, pngWriteCallback, pngFlushCallback);

	int8_t bpp = 4;

	uint8_t* pixels = new uint8_t[w*h*bpp];

	TXDConverter conv;

	TXDTextureHeader toTex(*tex);
	toTex.setRasterFormat(RasterFormatR8G8B8A8);

	conv.convert(*tex, toTex, data, pixels, mipmap, mipmap);

	/*int haa = 0;

	for (int32_t i = 0 ; i < w*h ; i++) {
		uint32_t& pixel = *(((uint32_t*) pixels) + i);

		//fprintf(stderr, "At %d/%d\n", i, w*h);
		if (pixel == 0) {
			haa++;
		}
	}

	printf("%d blank pixels\n", haa);

	exit(0);*/

	png_set_IHDR(png, info, w, h, 8,
			PNG_COLOR_TYPE_RGB_ALPHA, PNG_INTERLACE_NONE,
			PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

	const char* softwareKey = "Software";
	const char* softwareText = "gtatxd using gtatools " GTATOOLS_VERSION;

	const char* commentKey = "Comment";
	const char* commentText = "Automatically converted from a GTA TXD texture.";

	char* softwareKeyCpy = new char[strlen(softwareKey)+1];
	char* softwareTextCpy = new char[strlen(softwareText)+1];
	strcpy(softwareKeyCpy, softwareKey);
	strcpy(softwareTextCpy, softwareText);
	png_text software;
	software.key = softwareKeyCpy;
	software.text = softwareTextCpy;
	software.text_length = strlen(softwareTextCpy);
	software.compression = PNG_TEXT_COMPRESSION_NONE;

	char* commentKeyCpy = new char[strlen(commentKey)+1];
	char* commentTextCpy = new char[strlen(commentText)+1];
	strcpy(commentKeyCpy, commentKey);
	strcpy(commentTextCpy, commentText);
	png_text comment;
	comment.key = commentKeyCpy;
	comment.text = commentTextCpy;
	comment.text_length = strlen(commentTextCpy);
	comment.compression = PNG_TEXT_COMPRESSION_NONE;

	png_text texts[] = {software, comment};

	png_set_text(png, info, texts, sizeof(texts)/sizeof(texts[0]));

	uint8_t** rowData = new uint8_t*[h];

	for (int k = 0 ; k < h ; k++) {
		rowData[k] = pixels + k*w*bpp;
	}

	png_set_rows(png, info, rowData);
	png_write_png(png, info, PNG_TRANSFORM_IDENTITY, NULL);

	png_destroy_write_struct(&png, &info);

	delete[] rowData;
	delete[] pixels;

	delete[] softwareKeyCpy;
	delete[] softwareTextCpy;
	delete[] commentKeyCpy;
	delete[] commentTextCpy;
}


uint8_t* buildPackedMipmap(const ExtractOptions& opts, TXDTextureHeader* tex, uint8_t* data,
		int& packedW, int& packedH)
{
	const vector<MipmapRange>& ranges = opts.getMipmapRanges();
	int16_t baseW = tex->getWidth();
	int16_t baseH = tex->getHeight();

	// First we convert all mipmaps to R8G8B8A8

	// Calculate the size of the new rasters
	int normalizedRastersSize = 0;

	for (int i = 0 ; i < tex->getMipmapCount() ; i++) {
		vector<MipmapRange>::const_iterator it;

		for (it = ranges.begin() ; it != ranges.end() ; it++) {
			const MipmapRange& range = *it;

			if (range.contains(i)) {
                // TODO: Correct this
				normalizedRastersSize += (baseW / (int) pow((float) 2, (float) i)) * (baseH / (int) pow((float) 2, (float)i)) * 4;
				break;
			}
		}
	}

	// And then we convert them
	uint8_t* normalizedRasters = new uint8_t[normalizedRastersSize];
	uint8_t* normalizedRaster = normalizedRasters;

	TXDTextureHeader toTex(*tex);
	toTex.setRasterFormat(RasterFormatR8G8B8A8);
	TXDConverter conv;

	for (int i = 0 ; i < tex->getMipmapCount() ; i++) {
		vector<MipmapRange>::const_iterator it;

		for (it = ranges.begin() ; it != ranges.end() ; it++) {
			const MipmapRange& range = *it;

			if (range.contains(i)) {
				//tex->convert(normalizedRaster, data, i, MIRROR_NONE, 4, 0, 1, 2, 3);
				conv.convert(*tex, toTex, data, normalizedRaster, i, i);

				data += tex->computeMipmapDataSize(i);
                // TODO: Correct this
				normalizedRaster += (baseW / (int) pow((float) 2, (float) i)) * (baseH / (int) pow((float) 2, (float) i)) * 4;
				break;
			}
		}
	}


	// Now get the base mipmap level and the next one. We need them to compute the dimensions of the packed
	// output buffer.
	int baseLevel = -1;
	int secondLevel = -1; // The one below baseLevel which was specified on command line.

	for (int i = 0 ; i < tex->getMipmapCount()  &&  secondLevel == -1 ; i++) {
		vector<MipmapRange>::const_iterator it;

		for (it = ranges.begin() ; it != ranges.end() ; it++) {
			const MipmapRange& range = *it;

			if (range.contains(i)) {
				if (baseLevel == -1) {
					baseLevel = i;
				} else if (secondLevel == -1) {
					secondLevel = i;
				}

				break;
			}
		}
	}

    // TODO: Correct this
	if (baseLevel != -1) {
		packedW = baseW / (int) pow((float) 2, (float) baseLevel);
		packedH = baseH / (int) pow((float) 2, (float) baseLevel);
	}
	if (secondLevel != -1) {
		packedW += baseW / (int) pow((float) 2, (float) secondLevel);
	}

	uint8_t* packed = new uint8_t[packedW*packedH*4];
	uint8_t* packedRowStart = packed;

	// The pointer to the current destination in the packed texture buffer
	uint8_t* packedPointer = packed;

	// Finally, we pack the mipmaps together row by row
	for (int row = 0 ; row < packedH ; row++) {
		int16_t mipW = baseW;
		int16_t mipH = baseH;

		bool mipmapMatch = false;
		int i;
		uint8_t* normalizedMipmapStart = normalizedRasters;

		// First we write the row of the base mipmap
		for (i = 0 ; i < tex->getMipmapCount()  &&  !mipmapMatch ; i++) {
			vector<MipmapRange>::const_iterator it;

			for (it = ranges.begin() ; it != ranges.end() ; it++) {
				const MipmapRange& range = *it;

				if (range.contains(i)) {
					memcpy(packedPointer, normalizedRasters + mipW*4*row, mipW*4);
					packedPointer += mipW*4;
					mipmapMatch = true;
					break;
				}
			}

			normalizedMipmapStart += mipW * mipH * 4;

			mipW /= 2;
			mipH /= 2;
		}

		int rowOffset = 0;
		mipmapMatch = false;

		// And then we figure out which mipmap will go to the right of the row
		for (; i < tex->getMipmapCount()  &&  !mipmapMatch ; i++) {
			vector<MipmapRange>::const_iterator it;

			for (it = ranges.begin() ; it != ranges.end() ; it++) {
				const MipmapRange& range = *it;

				if (range.contains(i)) {
					if (mipH+rowOffset > row) {
						memcpy(packedPointer, normalizedMipmapStart + mipW*4*(row-rowOffset), mipW*4);
						packedPointer += mipW*4;
						mipmapMatch = true;
						break;
					} else {
						rowOffset += mipH;
					}
				}
			}

			normalizedMipmapStart += mipW * mipH * 4;

			mipW /= 2;
			mipH /= 2;
		}

		// Fill in the rest of the row with pure alpha
		int bytesLeftInRow = (packedW*4) / (packedPointer-packedRowStart);
		memset(packedPointer, 255, bytesLeftInRow);

		packedRowStart += packedW*4;
		packedPointer = packedRowStart;
	}

	delete[] normalizedRasters;

	return packed;
}

