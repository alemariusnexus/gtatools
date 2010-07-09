/*
	Copyright 2010 David Lerch

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
 */

#include "ListVisitor.h"
#include "cliarg.h"
#include <cstdlib>
#include <cstdio>


const char flagOptions[][PARAM_MAXLEN] = {
		"s", "c", "a", "b", "w", "h", "f", "m", "p", "\0"
};
const char switchOptions[][PARAM_MAXLEN] = {
		"\0"
};

ListVisitor::ListVisitor(int argc, char** argv) {
	SetFlagOptions(flagOptions);
	SetSwitchOptions(switchOptions);

	if (!ParseCommandLineArguments(argc, argv, 2)) {
		exit(1);
	}

	if (GetStandaloneParamCount() <= 0) {
		fprintf(stderr, "Error: No archive file given!\n");
		exit(1);
	}

	useCommaAsSeparator = IsFlagSet("s");
}

void ListVisitor::handleTexture(TXDArchive* archive, TXDTexture* header) {
	if (!useCommaAsSeparator) {
		printf("%-35s", header->getDiffuseName());
	} else {
		printf("%s", header->getDiffuseName());
	}

	flagset::iterator flag;

	for (flag = GetFlagBegin() ; flag != GetFlagEnd() ; flag++) {
		if (strcmp(*flag, "c") == 0) {
			if (useCommaAsSeparator) {
				printf(",");
			}

			switch (header->getCompression()) {
			case DXT1:
				printf("DXT1");
				break;
			case DXT3:
				printf("DXT3");
				break;
			case NONE:
				printf("None");
				break;
			}

			if (!useCommaAsSeparator) {
				printf("   ");
			}
		} else if (strcmp(*flag, "a") == 0) {
			if (!useCommaAsSeparator) {
				printf("%-35s", header->getAlphaName());
			} else {
				printf(",%s", header->getAlphaName());
			}
		} else if (strcmp(*flag, "b") == 0) {
			if (!useCommaAsSeparator) {
				printf("%-6d", header->getBytesPerPixel());
			} else {
				printf(",%d", header->getBytesPerPixel());
			}
		} else if (strcmp(*flag, "w") == 0) {
			if (!useCommaAsSeparator) {
				printf("%-7d", header->getWidth());
			} else {
				printf(",%d", header->getWidth());
			}
		} else if (strcmp(*flag, "h") == 0) {
			if (!useCommaAsSeparator) {
				printf("%-7d", header->getHeight());
			} else {
				printf(",%d", header->getHeight());
			}
		} else if (strcmp(*flag, "f") == 0) {
			char format[10];
			TxdGetRasterFormatName(format, header->getRasterFormat() & 0xF00);
			if (!useCommaAsSeparator) {
				printf("%-11s", format);
			} else {
				printf(",%s", format);
			}
		} else if (strcmp(*flag, "m") == 0) {
			if (!useCommaAsSeparator) {
				printf("%-6d", header->getMipmapCount());
			} else {
				printf(",%d", header->getMipmapCount());
			}
		} else if (strcmp(*flag, "p") == 0) {
			int32_t ext = header->getRasterFormatExtension();

			if (!useCommaAsSeparator) {
				if ((ext & TXD_FORMAT_EXT_PAL4) != 0) {
					printf("%-6d", 4);
				} else if ((ext & TXD_FORMAT_EXT_PAL8) != 0) {
					printf("%-6d", 8);
				} else {
					printf("%-6c", '-');
				}
			} else {
				if ((ext & TXD_FORMAT_EXT_PAL4) != 0) {
					printf(",%d", 4);
				} else if ((ext & TXD_FORMAT_EXT_PAL8) != 0) {
					printf(",%d", 8);
				} else {
					printf(",-");
				}
			}
		}
	}

	printf("\n");
}
