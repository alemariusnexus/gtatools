/*
	Copyright 2010 David "Alemarius Nexus" Lerch
    
    This file is part ofrt of his file is part of gtaimg.

	gtaimg is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	gtaimg is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with gtaimg.  If not, see <http://www.gnu.org/licenses/>.
 */


#include "cliarg.h"
#include "ListVisitor.h"
#include "ExtractVisitor.h"
#include <gtaformats/img/IMGException.h>
#include <gtaformats/img/IMGArchive.h>
#include <gtaformats/util/stream/IOException.h>
#include <gtaformats/util/File.h>
#include <cstring>
#include <map>
#include <iostream>
#include <string>

using std::cout;
using std::cerr;
using std::endl;
using std::string;

#define HELP_MESSAGE \
"gtaimg - List and extract files from GTA IMG archive files.\n\
\n\
Usage:\n\
    gtaimg (help|-help|--help)    - Prints this message\n\
    gtaimg COMMAND ...    - Executes the COMMAND (see below)\n\
    \n\
Commands & Parameters:\n\
    (l|ls|list) [-s] [-o] [-b] [-f DIRFILE] IMGFILE\n\
        Prints every file name inside the IMGFILE, one file per line.\n\
        IMGFILE may either be a GTA SA IMG file, or a GTA VC/III IMG or DIR\n\
        file. In the latter case, the other file will automatically be guessed\n\
        by replacing the extension .img with .dir and vice-versa, unless -f is\n\
        used. Valid flags are:\n\
            -s  Print file size\n\
            -o  Print file offset\n\
            -b  Print file size and offset in blocks instead of bytes. A block\n\
                in IMG files is always 2048 bytes.\n\
            -f  Assuming that IMGFILE is a GTA VC/III file, you can use this\n\
                option to explicitly set the .dir file to use with the IMGFILE.\n\
        When both -s and -o are given, they are printed in the order they were\n\
        written on the command line.\n\
    \n\
    (x|ex|extract) [-r] [-f DIRFILE] IMGFILE (ENTRYNAME DESTFILE)...\n\
    (x|ex|extract) -s [-r] [-f DIRFILE] IMGFILE ENTRYNAME...\n\
    (x|ex|extract) -o [-r] [-f DIRFILE] IMGFILE ENTRYNAME...\n\
        Extracts files from the IMGFILE. When used without special flags, the\n\
        IMGFILE is followed by pairs consisting of the name of the IMG entry\n\
        and the destination file to extract it to. Using the -s flag, you only\n\
        give the names of the entries. They will then be written to files with\n\
        the same name as inside the archive, into the current directory. Using\n\
        the -o flag will write the contents of all files to stdout. Note that\n\
        when multiple files must be extracted, they are all written to stdout\n\
        one after the other, without a separator between them.\n\
        IMGFILE may either be a GTA SA IMG file, or a GTA VC/III IMG or DIR\n\
        file. In the latter case, the other file will automatically be guessed\n\
        by replacing the extension .img with .dir and vice-versa, unless -f is\n\
        used. Other flags are:\n\
            -r  Interpret the ENTRYNAME as regular expression. All files\n\
                matching the pattern will be extracted from the archive.\n\
            -f  Assuming that IMGFILE is a GTA VC/III file, you can use this\n\
                option to explicitly set the .dir file to use with the IMGFILE.\n\
\n\
Examples:\n\
    List all files in gta3.img:\n\
        gtaimg ls gta3.img\n\
    \n\
    List all files in gta3.img with size in bytes:\n\
        gtaimg ls -s gta3.img\n\
    \n\
    Extract the file 'bistro.txd' from gta3.img and save it to ./bistro.txd:\n\
        gtaimg x -s gta3.img bistro.txd\n\
    \n\
    Extract all TXD files starting with 'a51_':\n\
        gtaimg x -r -s gta3.img 'a51_(.*?)\\.txd'\n\
\n"

int main(int argc, char** argv) {
	if (argc < 2) {
		printf("%s", HELP_MESSAGE);
		return 1;
	}

	if (	strcmp(argv[1], "help") == 0
			||  strcmp(argv[1], "-help") == 0
			||	strcmp(argv[1], "--help") == 0	)
	{
		printf("%s", HELP_MESSAGE);
		return 0;
	}

	char* command = argv[1];

	const char* srcfile;
	IMGVisitor* visitor;

	paramset::iterator param;

	if (	strcmp(command, "l") == 0
			||  strcmp(command, "ls") == 0
			||  strcmp(command, "list") == 0	)
	{
		visitor = new ListVisitor(argc, argv);
		param = GetStandaloneParamBegin();
		srcfile = *param;
	} else if (	strcmp(command, "x") == 0
				||  strcmp(command, "ex") == 0
				||  strcmp(command, "extract") == 0	)
	{
		visitor = new ExtractVisitor(argc, argv);
		param = GetStandaloneParamBegin();
		srcfile = *param;
	} else {
		fprintf(stderr, "Error: Invalid command '%s'!\n", command);
		return 2;
	}

	if (!File(srcfile).exists()) {
		fprintf(stderr, "Error: Source file does not exist!\n");
		return 3;
	}

	IMGArchive* archive = NULL;

	try {
		if (GetSwitch("f")) {
			const char* dirFile = GetSwitch("f");
			archive = new IMGArchive(dirFile, srcfile);
		} else {
			archive = new IMGArchive(srcfile);
		}

		archive->visitAll(visitor);

		delete archive;
	} catch (IOException ex) {
		cerr << "I/O error: " << ex.what() << endl;
		if (archive) {
			delete archive;
		}
	} catch (IMGException ex) {
		cerr << "IMG error: " << ex.what() << endl;
		if (archive) {
			delete archive;
		}
	}

	delete visitor;

	return 0;
}

