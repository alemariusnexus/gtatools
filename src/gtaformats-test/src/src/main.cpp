/*
	Copyright 2010-2012 David "Alemarius Nexus" Lerch

	This file is part of gtaformats-test.

	gtaformats-test is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	gtaformats-test is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with gtaformats-test.  If not, see <http://www.gnu.org/licenses/>.

	Additional permissions are granted, which are listed in the file
	GPLADDITIONS.
 */

#define GLOBAL_H_DEF
#include "global.h"
#undef GLOBAL_H_DEF

#include <cstdio>
#include <gtaformats/util/CLIParser.h>
#include <gtaformats/util/File.h>




int main(int argc, char** argv)
{
	testing::InitGoogleTest(&argc, argv);

	CLIParser cli;

	int helpOpt = cli.addOption(0, "gtaformats-test-help", "Print help", false);
	int gta3PathOpt = cli.addOption(0, "gta3-path", "Root directory of GTA III", true);
	int gtavcPathOpt = cli.addOption(0, "gtavc-path", "Root directory of GTA VC", true);
	int gtasaPathOpt = cli.addOption(0, "gtasa-path", "Root directory of GTA SA", true);

	argv++;
	argc--;

	int opt;
	char* arg;
	bool testRootFound = false;

	while ((opt = cli.parse(argc, argv, arg))  >=  0) {
		if (opt == helpOpt) {
			cli.printOptions();
			exit(0);
		} else if (opt == gta3PathOpt) {
			gta3Enabled = true;
			gta3Root = File(arg);
		} else if (opt == gtavcPathOpt) {
			gtavcEnabled = true;
			gtavcRoot = File(arg);
		} else if (opt == gtasaPathOpt) {
			gtasaEnabled = true;
			gtasaRoot = File(arg);
		} else if (opt == 0) {
			testRootDir = File(arg);
			testRootFound = true;
		}
	}

	if (!testRootFound) {
		fprintf(stderr, "ERROR: No testing root directory given!\n");
		exit(1);
	}
	if (!testRootDir.exists()) {
		if (!testRootDir.mkdirs()  ||  !testRootDir.exists()) {
			fprintf(stderr, "ERROR: Testing root directory does not exist and could not be created!\n");
			exit(2);
		}
	}

	if (testRootDir.getChildCount(false, false) != 0) {
		fprintf(stderr, "WARNING: Testing root directory is not empty. It is recommended to use en empty "
				"one for gtaformats-test!\n");
	}


	return RUN_ALL_TESTS();
}
