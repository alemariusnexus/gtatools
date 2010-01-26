#include "cliarg.h"
#include "ListVisitor.h"
#include "ExtractVisitor.h"
#include <IMGArchive.h>
#include <cstring>
#include <map>
#include <fstream>
#include <iostream>

using std::ifstream;
using std::cout;
using std::cerr;
using std::endl;

#define HELP_MESSAGE \
"gtaimg - List and extract files from GTA IMG archive files.\n\
\n\
Usage:\n\
    gtaimg (help|-help|--help)    - Prints this message\n\
    gtaimg COMMAND ...    - Executes the COMMAND (see below)\n\
    \n\
Commands & Parameters:\n\
    (l|ls|list) [-s] [-o] [-b] IMGFILE\n\
        Prints every file name inside the IMGFILE, one file per line. Valid\n\
        flags are:\n\
            -s  Print file size\n\
            -o  Print file offset\n\
            -b  Print file size and offset in blocks instead of bytes. A block\n\
                in IMG files is always 2048 bytes.\n\
        When both -s and -o are given, they are printed in the order they were\n\
        written on the command line.\n\
    \n\
    (x|ex|extract) [-r] IMGFILE (ENTRYNAME DESTFILE)...\n\
    (x|ex|extract) -s [-r] IMGFILE ENTRYNAME...\n\
    (x|ex|extract) -o [-r] IMGFILE ENTRYNAME...\n\
        Extracts files from the IMGFILE. When used without special flags, the\n\
        IMGFILE is followed by pairs consisting of the name of the IMG entry and\n\
        the destination file to extract it to. Using the -s flag, you only give\n\
        the names of the entries. They will then be written to files with the\n\
        same name as inside the archive, into the current directory. Using the -o\n\
        flag will write the contents of all files to stdout. Note that when\n\
        multiple files must be extracted, they are all written to stdout one\n\
        after the other, without a separator between them. Other flags are:\n\
            -r  Interpret the ENTRYNAME as regular expression (PCRE). All files\n\
                matching the pattern will be extracted from the archive. You can\n\
                access subpatterns in DESTFILE using perl-style format\n\
                specifiers. See the Boost::Regex documentation.\n\
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
    Extract all TXD files starting with 'a51_', and replace the beginning of their\n\
    names with 'a69_':\n\
        gtaimg x -r gta3.img 'a51_(.*?)\\.txd' 'a69_$1.txd'\n\
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
		return 1;
	}

	ifstream stream(srcfile, ifstream::in | ifstream::binary);

	if (stream.fail()) {
		cerr << "Failed to load file " << srcfile << "!" << endl;
		return 1;
	}

	IMGArchive archive(&stream);
	archive.visitAll(visitor);
	stream.close();


	/*FILE* stream = fopen(srcfile, "rb");

	if (!stream  ||  ferror(stream)) {
		fprintf(stderr, "Error opening input file!\n");
		exit(1);
	}

	ImgArchive* archive = ImgOpen(stream);
	ImgExecuteCommand(archive, *imgCommand);
	delete archive;
	fclose(stream);
	delete imgCommand;*/

	delete visitor;

	return 0;
}

