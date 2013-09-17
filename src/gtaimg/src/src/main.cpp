/*
	Copyright 2010-2013 David "Alemarius Nexus" Lerch

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

	Additional permissions are granted, which are listed in the file
	GPLADDITIONS.
 */

#include <gtaformats/util/CLIParser.h>
#include <cstdio>
#include <vector>
#include <gtaformats/util/strutil.h>
#include <gtaformats/util/File.h>
#include <gtaformats/util/FilePath.h>
#include <gtaformats/img/IMGArchive.h>
#include <gtaformats/img/IMGException.h>
#include "WildcardFilter.h"
#include "NullEntryFilter.h"
#include <ostream>
#include <fstream>
#include <iostream>
#include <cstring>
#include <istream>

using std::vector;
using std::ostream;
using std::cout;
using std::cin;
using std::ofstream;
using std::istream;



const int CommandList = 0;
const int CommandExtract = 1;
const int CommandShowHeader = 2;
const int CommandAppend = 3;
const int CommandRemove = 4;
const int CommandPack = 5;
const int CommandCreate = 6;


struct ExtractPattern
{
	EntryFilter* filter;
	File* destination;
};

struct InputFile
{
	File* file;
	IMGArchive::IMGVersion ver;
	File* dirFile;
	bool dirFileIsStdout;
};


const int TEXT_WRAP_LENGTH = 60;



int main(int argc, char** argv)
{
	argv++;
	argc--;

	CLIParser cli;

	// Command options
	int helpOpt = cli.addOption(0, "help", "Show usage help and exit.");
	int versionOpt = cli.addOption(0, "version", "Show version info and exit.");
	int listOpt = cli.addOption('l', "list", "List entries of an IMG archive.");
	int extractOpt = cli.addOption('x', "extract", "Extract files from an IMG archive.");
	int headerOpt = cli.addOption('h', "header", "Show data of the IMG header.");
	int createOpt = cli.addOption('c', "create", "Create a new, empty IMG archive.");
	int appendOpt = cli.addOption('a', "append", "Append files to the IMG archive.");
	int removeOpt = cli.addOption('r', "remove", "Remove files from the IMG archive.");
	int packOpt = cli.addOption('p', "pack", "Pack the IMG archive to take up as few space as possible.");

	// General options
	int fileOpt = cli.addOption('f', "file", "Take %a as the IMG archive.", true);
	int verboseOpt = cli.addOption('v', "verbose", "Make output verbose.");
	int dirFileOpt = cli.addOption('d', "dir-file", "Select the DIR file. This option is only needed for "
			"VER1 archives where the DIR file doesn't have the same base name (without extension) as the "
			"IMG file, or if the IMG file is read from stdin. This option only affects the next listed IMG "
			"archive, so you have to specify it again for the next archive for which the DIR file can't "
			"be automatically guessed.", true);

	// List options
	int lsHeaderOpt = cli.addOption('H', "list-header", "Show header.");
	int offsetOpt = cli.addOption('O', "offset", "Show entry offsets.");
	int sizeOpt = cli.addOption('S', "size", "Show entry sizes.");
	int bytesOpt = cli.addOption('B', "bytes", "Show offsets and sizes in bytes instead of IMG blocks.");
	int commaOpt = cli.addOption('C', "comma-separation", "Separate fields by commas instead of spaces.");

	// Extract options
	int outOpt = cli.addOption('o', "out", "Output the following FILEs to %a.\n"
			"If %a is a directory, the files will be saved in it, having the same names as in the IMG "
			"archive.\n"
			"If %a is a regular file or does not exist, the contents of all extracted FILEs will be written "
			"(concatenated) into the file.\n"
			"If %a is '-' (i.e. just a hyphen), all contents will be written to stdout.\n"
			"You may use this option multiple times to change the output location of all following FILEs.",
			true);

	// Create options
	int imgVersionOpt = cli.addOption('V', "img-version", "Select the file version for the IMG archive to be "
			"created. %a can be either ver1 or ver2. This version is used for all archives following this "
			"option, until the next %n.", true);

	// Append options
	int replaceOpt = cli.addOption('R', "replace", "Replace existing files. Normally, if a file is appended "
			"that already exists in the archive, a warning is printed and it is left unchanged. This option "
			"forces to overwrite the file.");

	char* arg;
	int opt;

	int command = -1;
	vector<InputFile> inputFiles;
	vector<ExtractPattern*> exPatterns;
	File* currentOutput = new File(".");
	File* currentDirFile = NULL;
	bool currentDirFileIsStdout = false;
	IMGArchive::IMGVersion currentVersion = IMGArchive::VER2;
	bool verbose = false;
	bool showSize = false;
	bool showOffset = false;
	bool bytesUnit = false;
	bool showHeader = false;
	bool commaSep = false;
	bool replace = false;

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
			printf("  You can list (-l), extract (-l), append (-a) or remove (-r) entries\n");
			printf("  to or from the archive. Also, you can create new (-c) or pack\n");
			printf("  existing (-p) archives. By default (if no -f option is found) we\n");
			printf("  read the IMG archive from stdin.\n\n");

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

			printf("  APPEND:\n");
			printf("    In append mode (-a), the FILEs are interpreted as actual files. All\n");
			printf("    listed FILEs are appended to the given archives (if multiple\n");
			printf("    archives are given, they are added to every one of them) if they\n");
			printf("    do not exist in them yet. If an entry with the same name already\n");
			printf("    exists in an archive, a warning is printed and the entry inside the\n");
			printf("    archive is left unchanged. You can force overwriting of existing\n");
			printf("    entries using the -r flag.\n\n");

			printf("  REMOVE:\n");
			printf("    In remove mode (-r), the FILEs are interpreted as patterns.\n");
			printf("    The IMG archives are searched for files matching any of them. Each\n");
			printf("    matching entry is removed from the archive. Note that this only\n");
			printf("    marks the space it occupied as unused and removes it from the\n");
			printf("    archive header, but does not delete it's content, nor does it move\n");
			printf("    any of the entries, so the actual size of the archive is not reduced\n");
			printf("    by this operation. You can pack (-p) the archive afterwards to save\n");
			printf("    some disk space.\n\n");

			printf("  CREATE\n");
			printf("    If -c is given, the FILEs are completely ignored. Instead, new IMG\n");
			printf("    archives are created for each listed input archive (-f). The newly\n");
			printf("    created archives are completely empty. See the flags for this\n");
			printf("    operation for more information on how to choose the archive version\n");
			printf("    and DIR path.\n\n");

			printf("  PACK\n");
			printf("    In pack mode (-p), the FILEs are completely ignored. Only the archive\n");
			printf("    input files (-f) are processed. Packing an archive moves all it's\n");
			printf("    entries as tightly together as possible, filling holes which might\n");
			printf("    be created when files somewhere in the middle of the archive are\n");
			printf("    removed or resized. This operation might change the order in which\n");
			printf("    the files are stored in the archive. Note that packing might involve\n");
			printf("    moving a lot of entries and their contents, so it's a rather\n");
			printf("    time-consuming operation.\n\n");

			printf("FLAGS:\n");

			printf("  Command flags:\n");
			cli.printOption(helpOpt).printOption(versionOpt).printOption(listOpt).printOption(extractOpt)
					.printOption(headerOpt).printOption(appendOpt).printOption(removeOpt)
					.printOption(createOpt).printOption(packOpt);
			printf("\n");

			printf("  General flags:\n");
			cli.printOption(fileOpt).printOption(dirFileOpt).printOption(verboseOpt);
			printf("\n");

			printf("  List flags:\n");
			cli.printOption(offsetOpt).printOption(sizeOpt).printOption(bytesOpt);
			printf("\n");

			printf("  Extract flags:\n");
			cli.printOption(outOpt);
			printf("\n");

			printf("  Append flags:\n");
			cli.printOption(replaceOpt);
			printf("\n");

			printf("  Create flags:\n");
			cli.printOption(imgVersionOpt);

			return 0;
		} else if (opt == versionOpt) {
			printf("gtaimg - Part of gtatools (version %s)\n\n", GTATOOLS_VERSION);
			printf("Copyright 2010-2012 David \"Alemarius Nexus\" Lerch\n\n");

			printf("gtaimg is free software: you can redistribute it and/or modify\n");
			printf("it under the terms of the GNU General Public License as published by\n");
			printf("the Free Software Foundation, either version 3 of the License, or\n");
			printf("(at your option) any later version.\n\n");
			printf("gtaimg is distributed in the hope that it will be useful,\n");
			printf("but WITHOUT ANY WARRANTY; without even the implied warranty of\n");
			printf("MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n");
			printf("GNU General Public License for more details.\n\n");
			printf("You should have received a copy of the GNU General Public License\n");
			printf("along with gtaimg.  If not, see <http://www.gnu.org/licenses/>.\n\n");
			printf("Additional permissions are granted, which are listed in the file\n");
			printf("GPLADDITIONS.\n");

			return 0;
		} else if (opt == listOpt) {
			if (command >= 0  &&  command != CommandList) {
				fprintf(stderr, "ERROR: Only one option of -lxharpc may be given.\n");
				return 1;
			}

			command = CommandList;
		} else if (opt == extractOpt) {
			if (command >= 0  &&  command != CommandExtract) {
				fprintf(stderr, "ERROR: Only one option of -lxharpc may be given.\n");
				return 1;
			}

			command = CommandExtract;
		} else if (opt == headerOpt) {
			if (command >= 0  &&  command != CommandShowHeader) {
				fprintf(stderr, "ERROR: Only one option of -lxharpc may be given.\n");
				return 1;
			}

			command = CommandShowHeader;
		} else if (opt == appendOpt) {
			if (command >= 0  &&  command != CommandAppend) {
				fprintf(stderr, "ERROR: Only one option of -lxharpc may be given.\n");
				return 1;
			}

			command = CommandAppend;
		} else if (opt == removeOpt) {
			if (command >= 0  &&  command != CommandRemove) {
				fprintf(stderr, "ERROR: Only one option of -lxharpc may be given.\n");
				return 1;
			}

			command = CommandRemove;
		} else if (opt == packOpt) {
			if (command >= 0  &&  command != CommandPack) {
				fprintf(stderr, "ERROR: Only one option of -lxharpc may be given.\n");
				return 1;
			}

			command = CommandPack;
		} else if (opt == createOpt) {
			if (command >= 0  &&  command != CommandCreate) {
				fprintf(stderr, "ERROR: Only one option of -lxharpc may be given.\n");
				return 1;
			}

			command = CommandCreate;
		} else if (opt == fileOpt) {
			if (strcmp(arg, "-") == 0) {
				InputFile ifile;
				ifile.file = NULL;
				ifile.ver = currentVersion;
				ifile.dirFile = currentDirFile ? new File(*currentDirFile) : NULL;
				ifile.dirFileIsStdout = currentDirFileIsStdout;
				inputFiles.push_back(ifile);
			} else {
				File* file = new File(arg);

				InputFile ifile;
				ifile.file = file;
				ifile.ver = currentVersion;
				ifile.dirFile = currentDirFile ? new File(*currentDirFile) : NULL;
				ifile.dirFileIsStdout = currentDirFileIsStdout;

				inputFiles.push_back(ifile);
			}

			if (currentDirFile) {
				delete currentDirFile;
				currentDirFile = NULL;
			}

			currentDirFileIsStdout = false;
		} else if (opt == dirFileOpt) {
			if (currentDirFile)
				delete currentDirFile;

			if (strcmp(arg, "-") == 0) {
				currentDirFile = NULL;
				currentDirFileIsStdout = true;
			} else {
				currentDirFile = new File(arg);
				currentDirFileIsStdout = false;
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
		} else if (opt == imgVersionOpt) {
			if (strcmp(arg, "ver1") == 0) {
				currentVersion = IMGArchive::VER1;
			} else if (strcmp(arg, "ver2") == 0) {
				currentVersion = IMGArchive::VER2;
			} else {
				fprintf(stderr, "ERROR: Invalid IMG version: %s! Terminating program.\n", arg);
				return 6;
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
		} else if (opt == replaceOpt) {
			replace = true;
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

	if (command == -1) {
		fprintf(stderr, "ERROR: You have to specify a command option! Use 'gtaimg --help' to see the list of "
				"available options.\n");
		return 9;
	}

	if (inputFiles.size() == 0) {
		InputFile ifile;
		ifile.file = NULL;
		ifile.ver = currentVersion;
		ifile.dirFile = currentDirFile ? new File(*currentDirFile) : NULL;
		ifile.dirFileIsStdout = currentDirFileIsStdout;
		inputFiles.push_back(ifile);
	} else {
		if (command != CommandCreate) {
			for (vector<InputFile>::iterator it = inputFiles.begin() ; it != inputFiles.end() ; it++) {
				InputFile ifile = *it;
				File* file = ifile.file;

				if (file  &&  !file->exists()) {
					fprintf(stderr, "ERROR: Input file %s does not exist!\n",
							file->getPath()->toString().get());
					return 2;
				}
			}
		}
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
		if (command == CommandAppend) {
			// There's nothing to do.
			return 0;
		}

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
				printf(" %-20s", "OFFSET");
		}
		if (showSize) {
			if (commaSep)
				printf(",%s", "SIZE");
			else
				printf(" %-20s", "SIZE");
		}

		printf("\n\n");
	}

	vector<InputFile>::iterator it;
	for (it = inputFiles.begin() ; it != inputFiles.end() ; it++) {
		InputFile ifile = *it;
		File* file = ifile.file;

		IMGArchive* img;

		if (command == CommandCreate) {
			if (ifile.ver == IMGArchive::VER1) {
				if (verbose)
					printf("Creating a VER1 archive (");

				iostream* dirStream;
				iostream* imgStream;

				if (ifile.dirFile) {
					dirStream = ifile.dirFile->openInputOutputStream(
							ostream::binary | ostream::out | ostream::in);

					if (verbose)
						printf("DIR file: %s", ifile.dirFile->getPath()->toString().get());
				} else {
					if (ifile.dirFileIsStdout) {
						// TODO Implement
						//dirStream = &cout;

						if (verbose)
							printf("DIR file: [stdout]");
					} else {
						if (!file) {
							fprintf(stderr, "ERROR: No DIR file path specified for stdout VER1 IMG archive! "
									"For VER1 IMG archives written to stdout, you MUST specify a DIR path, "
									"because there's no IMG file name to guess it from. Terminating "
									"program.");
							return 7;
						}

						const char* imgPath = file->getPath()->toString().get();
						char* dirPath = new char[strlen(imgPath)+1];
						int baseLen = strlen(imgPath) - file->getPath()->getExtension().length();
						strncpy(dirPath, imgPath, baseLen);
						strcpy(dirPath+baseLen, "dir");
						File dirFile(dirPath);
						delete[] dirPath;

						dirStream = dirFile.openInputOutputStream(
								ostream::binary | ostream::out | ostream::in);

						if (verbose)
							printf("DIR file: %s", dirFile.getPath()->toString().get());
					}
				}

				if (file) {
					imgStream = file->openInputOutputStream(ostream::binary | ostream::out | ostream::in);

					if (verbose)
						printf(", IMG file: %s)", file->getPath()->toString().get());
				} else {
					// TODO Implement
					//imgStream = &cout;

					if (verbose)
						printf(", IMG file: [stdout])");
				}

				if (verbose)
					printf("\n");

				img = IMGArchive::createArchive(dirStream, imgStream);

				delete img;
				delete imgStream;
				delete dirStream;

				continue;
			} else {
				iostream* imgStream;

				if (verbose)
					printf("Creating a VER2 archive: ");

				if (file) {
					imgStream = file->openInputOutputStream(ostream::binary | ostream::out | ostream::in);

					if (verbose)
						printf("%s\n", file->getPath()->toString().get());
				} else {
					// TODO Implement
					//imgStream = &cout;

					if (verbose)
						printf("[stdout]\n");
				}

				img = IMGArchive::createArchive(imgStream);

				delete img;
				delete imgStream;

				continue;
			}
		}

		int mode;

		if (command == CommandList  ||  command == CommandExtract  ||  command == CommandShowHeader) {
			mode = IMGArchive::ReadOnly;
		} else {
			mode = IMGArchive::ReadWrite;
		}

		try {
			if (file) {
				// The IMG file is an existing file.
				IMGArchive::IMGVersion ver = IMGArchive::guessIMGVersion(*file);

				if (ver == IMGArchive::VER1) {
					// The IMG file is VER1, so we need a DIR file for it.
					if (ifile.dirFile) {
						// The DIR file is given as an existing file.
						img = new IMGArchive(*ifile.dirFile, *file, mode);
					} else {
						if (ifile.dirFileIsStdout) {
							// The DIR file is read from stdin.
							if ((mode & IMGArchive::ReadWrite)  !=  0) {
								fprintf(stderr, "ERROR: Cannot write to an IMG file read from stdin! "
										"Terminating program.\n");
								return 8;
							}

							istream* imgStream = file->openInputStream(istream::binary);
							img = new IMGArchive(&cin, imgStream, mode);
						} else {
							// The DIR file name is automatically guessed from the IMG file name.
							img = new IMGArchive(*file, mode);
						}
					}
				} else {
					// The IMG file is VER2, so we don't need any DIR file.
					img = new IMGArchive(*file, mode);
				}
			} else {
				// The IMG archive is read from stdin. In this case, we can't determine the archive version
				// that easy, because backward seeking is not possible in stdin. Also, guessing the DIR file
				// name automatically from the IMG file name doesn't work here, as we don't have an IMG file
				// name. We therefore assume that is the DIR file is not explicitly specified, we have a VER2
				// archive.

				if ((mode & IMGArchive::ReadWrite)  !=  0) {
					fprintf(stderr, "ERROR: Cannot write to an IMG file read from stdin! "
							"Terminating program.\n");
					return 8;
				}

				if (ifile.dirFile) {
					istream* dirStream = ifile.dirFile->openInputStream(istream::binary);
					img = new IMGArchive(dirStream, &cin, mode);
				} else {
					img = new IMGArchive(&cin, mode);
				}
			}
		} catch (IMGException ex) {
			fprintf(stderr, "ERROR opening IMG file %s: %s\n", file ? file->getPath()->toString().get()
					: "(stdin)", ex.what());
			return 5;
		}

		if (command == CommandShowHeader) {
			if (file) {
				printf("%s\n", file->getPath()->toString().get());
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
			printf("  %d blocks (= %d bytes)\n", img->getSize(), img->getSize() * IMG_BLOCK_SIZE);
		} else if (command == CommandList  ||  command == CommandExtract  ||  command == CommandRemove) {
			vector<IMGArchive::EntryIterator> entriesToRemove;

			//for (int i = 0 ; i < entryCount ; i++) {
			for (IMGArchive::EntryIterator it = img->getEntryBegin() ; it != img->getEntryEnd() ; it++) {
				const IMGEntry& entry = **it;

				if (command == CommandExtract) {
					for (int j = 0 ; j < numPatterns ; j++) {
						ExtractPattern* pattern = exPatternArr[j];

						if (pattern->filter->process(entry)) {
							if (verbose)
								printf("Extracting file %s ", entry.name);

							istream* stream = img->gotoEntry(it);

							ostream* out = NULL;

							if (pattern->destination) {
								if (	!pattern->destination->exists()
										||  !pattern->destination->isDirectory()
								) {
									if (verbose)
										printf("to %s...\n",
												pattern->destination->getPath()->toString().get());

									out = new ofstream(pattern->destination->getPath()->toString().get(),
											ofstream::out | ofstream::binary | ofstream::app);
								} else {
									FilePath outPath(*pattern->destination->getPath(), entry.name);

									if (verbose)
										printf("to %s...\n", outPath.toString().get());

									out = new ofstream(outPath.toString().get(),
											ofstream::out | ofstream::binary);
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
								printf("  %llu bytes written!\n",
										(long long unsigned) IMG_BLOCKS2BYTES(entry.size));
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

							long long unsigned offset = bytesUnit ? IMG_BLOCKS2BYTES(entry.offset) : entry.offset;
							long long unsigned size = bytesUnit ? IMG_BLOCKS2BYTES(entry.size) : entry.size;

							if (showOffset) {
								if (commaSep)
									printf(",%llu", offset);
								else
									printf(" %-20llu", offset);
							}
							if (showSize) {
								if (commaSep)
									printf(",%llu", size);
								else
									printf(" %-20llu", size);
							}

							printf("\n");
						}
					}
				} else if (command == CommandRemove) {
					for (int j = 0 ; j < numPatterns ; j++) {
						ExtractPattern* pattern = exPatternArr[j];

						if (pattern->filter->process(entry)) {
							entriesToRemove.push_back(it);
						}
					}
				}
			}

			vector<IMGArchive::EntryIterator>::iterator it;
			for (it = entriesToRemove.begin() ; it != entriesToRemove.end() ; it++) {
				IMGArchive::EntryIterator eit = *it;

				if (verbose)
					printf("Removing %s...\n", (*eit)->name);

				img->removeEntry(eit);
			}
		} else if (command == CommandAppend) {
			// In append mode, exPatterns is actually the list of files to append.

			img->reserveHeaderSpace(img->getEntryCount() + numPatterns);

			for (int i = 0 ; i < numPatterns ; i++) {
				ExtractPattern* pattern = exPatternArr[i];

				const char* fpath = ((WildcardFilter*) pattern->filter)->getPattern();

				File file(fpath);

				CString fname = file.getPath()->getFileName();

				IMGArchive::EntryIterator it = img->getEntryByName(fname.get());

				if (it != img->getEntryEnd()  &&  !replace) {
					fprintf(stderr, "WARNING: Entry %s already exists in the archive and will not be "
							"touched. Use -R to replace it's contents.\n", fname.get());
					continue;
				}

				IMGEntry* entry = *it;
				File::filesize fsize = file.getSize();
				int32_t newSize = IMG_BYTES2BLOCKS(fsize);

				if (it == img->getEntryEnd()) {
					// Add a new entry

					if (verbose)
						printf("Adding entry %s (%d blocks)...\n", fname.get(), newSize);

					it = img->addEntry(fname.get(), newSize);
				} else {
					// Replace an existing entry. Here, we need to be sure that the entry has enough space to
					// hold the new contents.

					if (verbose)
						printf("Replacing entry %s (%d blocks)...\n", fname.get(), newSize);

					if (newSize > entry->size) {
						img->resizeEntry(it, newSize);
					}
				}

				istream* inStream = file.openInputStream(istream::binary | istream::in);
				ostream* outStream = (ostream*) img->gotoEntry(it);

				char buf[IMG_BLOCK_SIZE];

				while (!inStream->eof()) {
					inStream->read(buf, sizeof(buf));
					outStream->write(buf, inStream->gcount());
				}

				delete inStream;
				delete outStream;
			}
		} else if (command == CommandPack) {
			int32_t oldSize = img->getSize();
			int32_t newSize = img->pack();

			if (file) {
				file->resize(newSize * IMG_BLOCK_SIZE);
			}

			if (verbose)
				printf("Packing reduced the archive size from %d to %d blocks.\n", oldSize, newSize);
		}

		delete img;
	}

	if (verbose)
		printf("All done!\n");

	return 0;
}









