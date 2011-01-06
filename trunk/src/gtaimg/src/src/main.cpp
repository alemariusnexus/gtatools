/*
	Copyright 2010 David "Alemarius Nexus" Lerch

	This file is part of gtaimg.

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

#include <gtaformats/util/CLIParser.h>
#include <cstdio>
#include <vector>
#include <gtaformats/util/strutil.h>
#include <gtaformats/util/File.h>
#include <gtaformats/util/FilePath.h>
#include <gtaformats/util/stream/STLInputStream.h>
#include <gtaformats/img/IMGArchive.h>
#include <gtaformats/img/IMGException.h>
#include "WildcardFilter.h"
#include "NullEntryFilter.h"
#include <ostream>
#include <fstream>
#include <iostream>
#include <cstring>

using std::vector;
using std::ostream;
using std::cout;
using std::cin;
using std::ofstream;



const int CommandList = 0;
const int CommandExtract = 1;
const int CommandShowHeader = 2;


struct ExtractPattern
{
	EntryFilter* filter;
	File* destination;
};


const int TEXT_WRAP_LENGTH = 60;



int main(int argc, char** argv)
{
	char* progName = argv[0];
	argv++;
	argc--;

	CLIParser cli;

	int helpOpt = cli.addOption(0, "help", "Show usage help and exit.");
	int versionOpt = cli.addOption(0, "version", "Show version info and exit.");
	int listOpt = cli.addOption('l', "list", "List entries of an IMG archive.");
	int extractOpt = cli.addOption('x', "extract", "Extract files from an IMG archive.");
	int headerOpt = cli.addOption('h', "header", "Show data of the IMG header.");
	int fileOpt = cli.addOption('f', "file", "Take %a as the IMG archive.", true);
	int verboseOpt = cli.addOption('v', "verbose", "Make output verbose.");

	// List options
	int lsHeaderOpt = cli.addOption('H', "list-header", "Show header.");
	int offsetOpt = cli.addOption('p', "offset", "Show entry offsets.");
	int sizeOpt = cli.addOption('s', "size", "Show entry sizes.");
	int bytesOpt = cli.addOption('b', "bytes", "Show offsets and sizes in bytes instead of IMG blocks.");
	int commaOpt = cli.addOption('c', "comma-separation", "Separate fields by commas instead of spaces.");

	// Extract options
	int outOpt = cli.addOption('o', "out", "Output the following FILEs to %a.\n"
			"If %a is a directory, the files will be saved in it, having the same names as in the IMG "
			"archive.\n"
			"If %a is a regular file or does not exist, the contents of all extracted FILEs will be written "
			"(concatenated) into the file.\n"
			"If %a is '-' (i.e. just a hyphen), all contents will be written to stdout.\n"
			"You may use this option multiple times to change the output location of all following FILEs.",
			true);

	char* arg;
	int opt;

	int command = -1;
	vector<File*> inputFiles;
	vector<ExtractPattern*> exPatterns;
	File* currentOutput = new File(".");
	bool verbose = false;
	bool showSize = false;
	bool showOffset = false;
	bool bytesUnit = false;
	bool showHeader = false;
	bool commaSep = false;

	while ((opt = cli.parse(argc, argv, arg))  >=  0) {
		if (opt == helpOpt) {
			cli.setPrintIndentation("    ");
			cli.setTextWrapLength(TEXT_WRAP_LENGTH);

			printf("gtaimg - Lists and extracts files from GTA IMG archives.\n\n");

			printf("USAGE:\n");
			printf("  gtaimg [OPTION...] FILE...\n\n");

			printf("DESCRIPTION:\n");
			printf("  You can use gtaimg to display information of and extract files from\n");
			printf("  GTA IMG archives. These are simple general-purpose archives, without\n");
			printf("  extended features like directories, file attributes etc.\n\n");
			printf("  You can either list (-l) or extract (-x) files, or show the IMG\n");
			printf("  header (-h). By default (if no -f option is found) we read the IMG\n");
			printf("  archive from stdin.\n\n");

			printf("  LIST:\n");
			printf("    In list mode (-l), the FILEs are interpreted as patterns.\n");
			printf("    The IMG archives are searched for files matching any of them. Each\n");
			printf("    matching entry name is printed, possibly along with other data\n");
			printf("    (see list flags). The output has a table format, with a header\n");
			printf("    describing the columns.\n\n");

			printf("  EXTRACT:\n");
			printf("    In extraction mode (-x), the FILEs are interpreted as patterns.\n");
			printf("    The IMG archives are searched for files matching any of them. Each\n");
			printf("    matching file is extracted. By default, the output goes to a file\n");
			printf("    of the same name in the current working directory.\n\n");

			printf("  SHOW HEADER:\n");
			printf("    If -h is given, the IMG header information will be printed (which\n");
			printf("    is just the version and number of files).\n\n");

			printf("FLAGS:\n");

			printf("  Command flags:\n");
			cli.printOption(helpOpt).printOption(versionOpt).printOption(listOpt).printOption(extractOpt)
					.printOption(headerOpt);
			printf("\n");

			printf("  General flags:\n");
			cli.printOption(fileOpt).printOption(verboseOpt);
			printf("\n");

			printf("  List flags:\n");
			cli.printOption(offsetOpt).printOption(sizeOpt).printOption(bytesOpt);

			printf("  Extract flags:\n");
			cli.printOption(outOpt);

			return 0;
		} else if (opt == versionOpt) {
			printf("gtaimg - Part of gtatools (version %s)\n\n", GTATOOLS_VERSION);
			printf("Copyright 2010 David \"Alemarius Nexus\" Lerch\n\n");

			printf("gtaimg is free software: you can redistribute it and/or modify\n");
			printf("it under the terms of the GNU General Public License as published by\n");
			printf("the Free Software Foundation, either version 3 of the License, or\n");
			printf("(at your option) any later version.\n\n");
			printf("gtaimg is distributed in the hope that it will be useful,\n");
			printf("but WITHOUT ANY WARRANTY; without even the implied warranty of\n");
			printf("MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n");
			printf("GNU General Public License for more details.\n\n");
			printf("You should have received a copy of the GNU General Public License\n");
			printf("along with gtaimg.  If not, see <http://www.gnu.org/licenses/>.\n");

			return 0;
		} else if (opt == listOpt) {
			if (command >= 0  &&  command != CommandList) {
				fprintf(stderr, "ERROR: Only one option of -lxh may be given.\n");
				return 1;
			}

			command = CommandList;
		} else if (opt == extractOpt) {
			if (command >= 0  &&  command != CommandExtract) {
				fprintf(stderr, "ERROR: Only one option of -lxh may be given.\n");
				return 1;
			}

			command = CommandExtract;
		} else if (opt == headerOpt) {
			if (command >= 0  &&  command != CommandExtract) {
				fprintf(stderr, "ERROR: Only one option of -lxh may be given.\n");
				return 1;
			}

			command = CommandShowHeader;
		} else if (opt == fileOpt) {
			if (strcmp(arg, "-") == 0) {
				inputFiles.push_back(NULL);
			} else {
				File* file = new File(arg);

				if (!file->exists()) {
					fprintf(stderr, "ERROR: Input file %s does not exist!\n", file->getPath()->toString());
					return 2;
				}

				inputFiles.push_back(file);
			}
		} else if (opt == outOpt) {
			if (currentOutput) {
				delete currentOutput;
			}

			if (strcmp(arg, "-") == 0) {
				currentOutput = NULL;
			} else {
				currentOutput = new File(arg);
			}
		} else if (opt == verboseOpt) {
			verbose = true;
		} else if (opt == sizeOpt) {
			showSize = true;
		} else if (opt == offsetOpt) {
			showOffset = true;
		} else if (opt == bytesOpt) {
			bytesUnit = true;
		} else if (opt == lsHeaderOpt) {
			showHeader = true;
		} else if (opt == commaOpt) {
			commaSep = true;
		} else if (opt == 0) {
			ExtractPattern* pattern = new ExtractPattern;
			pattern->filter = new WildcardFilter(arg);

			if (currentOutput) {
				pattern->destination = new File(*currentOutput);
			} else {
				pattern->destination = NULL;
			}

			exPatterns.push_back(pattern);
		}
	}

	if (inputFiles.size() == 0) {
		//fprintf(stderr, "ERROR: No input files!\n");
		//return 3;
		inputFiles.push_back(NULL);
	}

	int numPatterns = exPatterns.size();
	ExtractPattern** exPatternArr;

	if (numPatterns > 0) {
		exPatternArr = new ExtractPattern*[numPatterns];
		int i = 0;
		vector<ExtractPattern*>::iterator pit;
		for (pit = exPatterns.begin() ; pit != exPatterns.end() ; pit++) {
			exPatternArr[i++] = *pit;
		}
	} else {
		ExtractPattern* nullPattern = new ExtractPattern;
		nullPattern->filter = new NullEntryFilter;

		if (currentOutput) {
			nullPattern->destination = new File(*currentOutput);
		} else {
			nullPattern->destination = NULL;
		}

		exPatternArr = new ExtractPattern*[1];
		exPatternArr[0] = nullPattern;
		numPatterns = 1;
	}

	if (command == CommandList  &&  showHeader) {
		if (commaSep)
			printf("%s", "FILE NAME");
		else
			printf("%-24s", "FILE NAME");

		if (showOffset) {
			if (commaSep)
				printf(",%s", "OFFSET");
			else
				printf(" %-10s", "OFFSET");
		}
		if (showSize) {
			if (commaSep)
				printf(",%s", "SIZE");
			else
				printf(" %-10s", "SIZE");
		}

		printf("\n\n");
	}

	vector<File*>::iterator it;
	for (it = inputFiles.begin() ; it != inputFiles.end() ; it++) {
		File* file = *it;

		IMGArchive* img;

		try {
			if (file) {
				img = new IMGArchive(*file);
			} else {
				STLInputStream* in = new STLInputStream(&cin, false, false);
				img = new IMGArchive(in, true);
			}
		} catch (IMGException ex) {
			fprintf(stderr, "ERROR opening IMG file %s: %s\n", file ? file->getPath()->toString() : "(stdin)",
					ex.what());
			return 5;
		}

		const IMGEntry* entries = img->getEntries();
		int32_t entryCount = img->getEntryCount();

		if (command == CommandShowHeader) {
			if (file) {
				printf("%s\n", file->getPath()->toString());
			} else {
				printf("stdin:\n");
			}

			printf("  Version ");
			switch (img->getVersion()) {
			case IMGArchive::VER1:
				printf("1 (GTA3, GTA VC)\n");
				break;
			case IMGArchive::VER2:
				printf("2 (GTA SA)\n");
				break;
			}

			printf("  %d entries\n", img->getEntryCount());
		} else {
			for (int i = 0 ; i < entryCount ; i++) {
				const IMGEntry& entry = entries[i];

				if (command == CommandExtract) {
					for (int j = 0 ; j < numPatterns ; j++) {
						ExtractPattern* pattern = exPatternArr[j];

						if (pattern->filter->process(entry)) {
							if (verbose)
								printf("Extracting file %s ", entry.name);

							InputStream* stream = img->gotoEntry(&entry);

							ostream* out = NULL;

							if (pattern->destination) {
								if (	!pattern->destination->exists()
										||  !pattern->destination->isDirectory()
								) {
									if (verbose)
										printf("to %s...\n", pattern->destination->getPath()->toString());

									out = new ofstream(pattern->destination->getPath()->toString(),
											ofstream::out | ofstream::binary | ofstream::app);
								} else {
									FilePath outPath(*pattern->destination->getPath(), entry.name);

									if (verbose)
										printf("to %s...\n", outPath.toString());

									out = new ofstream(outPath.toString(), ofstream::out | ofstream::binary);
								}
							} else {
								// Write to stdout
								if (verbose)
									printf("to stdout...\n");
								out = &cout;
							}

							char buffer[IMG_BLOCK_SIZE];
							for (int j = 0 ; j < entry.size ; j++) {
								stream->read(buffer, IMG_BLOCK_SIZE);
								out->write(buffer, IMG_BLOCK_SIZE);
							}

							delete stream;

							// If out is a pointer to cout, we shouldn't close it
							if (pattern->destination) {
								delete out;
							}

							if (verbose)
								printf("  %d bytes written!\n", entry.size*IMG_BLOCK_SIZE);
						}
					}
				} else if (command == CommandList) {
					for (int j = 0 ; j < numPatterns ; j++) {
						ExtractPattern* pattern = exPatternArr[j];

						if (pattern->filter->process(entry)) {
							if (commaSep)
								printf("%s", entry.name);
							else
								printf("%-24s", entry.name);

							int offset = bytesUnit ? entry.offset*IMG_BLOCK_SIZE : entry.offset;
							int size = bytesUnit ? entry.size*IMG_BLOCK_SIZE : entry.size;

							if (showOffset) {
								if (commaSep)
									printf(",%d", offset);
								else
									printf(" %-10d", offset);
							}
							if (showSize) {
								if (commaSep)
									printf(",%d", size);
								else
									printf(" %-10d", size);
							}

							printf("\n");
						}
					}
				}
			}
		}

		delete img;
	}
}









