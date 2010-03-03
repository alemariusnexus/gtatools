/*
 * main.cpp
 *
 *  Created on: 04.01.2010
 *      Author: alemariusnexus
 */

#include <cstring>
#include <fstream>
#include <iostream>
#include <txd/TXDArchive.h>
#include "cliarg.h"
#include "ListVisitor.h"
#include "ExtractVisitor.h"
#include <boost/regex.hpp>
#include <iostream>

using std::ifstream;
using std::istream;
using std::cerr;
using std::endl;

#define HELP_MESSAGE \
"gtatxd - List and extract textures from GTA TXD files.\n\
\n\
Usage:\n\
    gtatxd (help|-help|--help)    - Prints this help message\n\
    gtatxd COMMAND ...    - Executes a command, see below\n\
\n\
Commands & Parameters:\n\
    (l|ls|list) [-c] [-a] [-b] [-w] [-h] [-f] [-m] [-p] TXDFILE\n\
        Prints a listing of all texture diffuse names in the TXDFILE. Use the\n\
        flags for printing additional information abut each texture. The data\n\
        will then be displayed table-like, separated with spaces. Information\n\
        will be written in the order the flags were specified. Flags are:\n\
            -c  Show texture compression type. This is one of 'DXT1', 'DXT3'\n\
                or 'None'\n\
            -a  Show alpha texture name\n\
            -b  Show number of bytes per pixel\n\
            -w  Show image width (in pixels)\n\
            -h  Show image height (in pixels)\n\
            -f  Show image raster format. Currently, supported formats are:\n\
                A1R5G5B5, R5G6B5, R4G4B4A4, B8G8R8A8, B8G8R8, R5G5B5, LUM8\n\
            -m  Show the number of mipmaps included\n\
            -p  Show the type of palette used. This is '4' for PAL4, '8' for\n\
                PAL8 or '-' if no palette is used\n\
            -s  Use a comma instead of whitespaces to seperate values. This\n\
                should be used when information needs to be processed by the\n\
                machine.\n\
    \n\
    (x|ex|extract) [-r] [-f] TXDFILE (TEXNAME DESTFILE)...\n\
    (x|ex|extract) -s [-r] [-f] TXDFILE TEXNAME...\n\
        Extracts textures from the TXDFILE. When used without the -s flag,\n\
        the TXDFILE is followed by pairs of the texture name to extract and\n\
        the file to write it to. When using the -s flag, only the texture\n\
        names are given. The textures will be written to files in the current\n\
        directory, with names of the form TXDNAME.XXX. Other flags are:\n\
            -r  Interpret texture names as regular expressions. When this flag\n\
                is given, all matching files will be written to the current\n\
                working directory with names of the form TXDNAME.png. The\n\
                destination files will _always_ be ignored when -r is set.\n\
            -f  The format into which textures will be extracted. If you don't\n\
                give this option, the default is PNG. Note that not always all\n\
                formats will work for a texture (e.g. JPEG does not work for\n\
                textures with alpha channel). To be sure that extraction will \n\
                work, you can use PNG, which should work for all textures.\n\
                Supported formats are: bmp, png, jpg, pnm, psd, tga, sgi, tif.\n\
\n\
Notes:\n\
    Wherever you are asked to provide the TXDFILE you can also pass the string\n\
    '-', which will cause the program to read the TXDFILE from stdin.\n\
    \n\
    Warning: There ARE textures with whitespaces in their names, so if you want\n\
    the output of 'list' to be processed by other tools, you can't rely on using\n\
    whitespaces as field-separators. You should use the -s option here.\n\
    \n\
    Official support is only given for TXD files of the PC versions of GTA3,\n\
    Vice City and San Andreas. PS2 files are not supported.\n\
\n\
Examples:\n\
    List all textures inside test.txd with their width and height:\n\
        gtatxd ls -w -h test.txd\n\
\n\
    Extract the texture 'vent_64' from bistro.txd:\n\
        gtatxd x -s bistro.txd vent_64\n\
\n\
    Extract all textures beginning with the letter C from bistro.txd:\n\
        gtatxd x -s -r bistro.txd 'C.*'\n\
\n\
    Extract all textures beginning with the letter C and the textures named\n\
    'vent_64' and 'Tablecloth' from bistro.txd:\n\
        gtatxd x -s -r bistro.txd 'C.*' vent_64 Tablecloth\n\
\n\
"


int main(int argc, char** argv) {
	if (argc < 2) {
		printf("%s", HELP_MESSAGE);
		return 1;
	}

	char* command = argv[1];

	const char* srcfile;
	TXDVisitor* visitor;
	TXDArchive* archive = NULL;

	if (	strcmp(command, "l") == 0
			||  strcmp(command, "ls") == 0
			||  strcmp(command, "list") == 0	)
	{
		ListVisitor* lvisitor = new ListVisitor(argc, argv);
		visitor = lvisitor;
		srcfile = *GetStandaloneParamBegin();
	} else if (	strcmp(command, "x") == 0
				||  strcmp(command, "ex") == 0
				||  strcmp(command, "extract") == 0	)
	{
		visitor = new ExtractVisitor(argc, argv);
		srcfile = *GetStandaloneParamBegin();
	} else if (	strcmp(command, "help") == 0
				||  strcmp(command, "-help") == 0
				||  strcmp(command, "--help") == 0	)
	{
		printf("%s", HELP_MESSAGE);
		return 0;
	} else {
		printf("Error: Unknown command '%s'!\n", command);
		return 1;
	}

	istream* stream;

	if (strcmp(srcfile, "-") == 0) {
		stream = &std::cin;
	} else {
		ifstream* fileStream = new ifstream(srcfile, ifstream::in | ifstream::binary);

		if (fileStream->fail()) {
			cerr << "Error: Failed to open file!" << endl;
			return 1;
		}

		stream = fileStream;
	}

	try {
		archive = new TXDArchive(stream);
		archive->visitAll(visitor);
		delete archive;
	} catch (TXDException ex) {
		cerr << "Error reading file: " << ex.what() << endl;
		return 1;
	}

	if (stream != &std::cin) {
		dynamic_cast<ifstream*>(stream)->close();
		delete stream;
	}

	delete visitor;

	return 0;
}
