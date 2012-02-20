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

	if (argc < 3) {
		fprintf(stderr, "ERROR: No input directories given!\n");
		exit(1);
	}

	rootDir = File(argv[1]);
	testRootDir = File(argv[2]);

	if (!rootDir.physicallyExists()) {
		fprintf(stderr, "ERROR: Input directory %s does not exist!\n", rootDir.getPath()->toString());
		exit(2);
	}

	if (!testRootDir.physicallyExists()) {
		if (!testRootDir.mkdir()) {
			fprintf(stderr, "ERROR: Test directory could not be created!\n");
			exit(3);
		}
	}


	return RUN_ALL_TESTS();
}
