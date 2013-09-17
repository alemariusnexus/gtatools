/*
	Copyright 2010-2013 David "Alemarius Nexus" Lerch

	This file is part of gtaformats.

	gtaformats is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	gtaformats is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with gtaformats.  If not, see <http://www.gnu.org/licenses/>.

	Additional permissions are granted, which are listed in the file
	GPLADDITIONS.
 */

#include "CLIParser.h"
#include <cstring>
#include <cstdio>
#include "strutil.h"
#include <string>

using std::string;



int CLIParser::addOption(char name, const char* longName, const char* description, bool takesArgument)
{
	Option opt;
	opt.name = name;
	opt.longName = longName;
	opt.description = description;
	opt.takesArgument = takesArgument;
	options.push_back(opt);
	return options.size();
}


int CLIParser::parse(int& argc, char**& argv, char*& argument)
{
	if (argc <= 0) {
		return OptionEnd;
	}

	char*& arg = argv[0];
	int len = strlen(arg);

	if (len < 2) {
		// Too few characters for an option.
		argc--;
		argv++;
		argument = arg;
		return 0;
	} else {
		if (arg[0] == '-') {
			if (arg[1] == '-') {
				if (len == 2) {
					// Only '--' is not an option.
					argc--;
					argv++;
					argument = arg;
					return 0;
				} else {
					// Long option

					int numOpt = 1;
					vector<Option>::const_iterator it;
					for (it = options.begin() ; it != options.end() ; it++) {
						const Option& opt = *it;

						if (opt.longName) {
							if (strcmp(arg+2, opt.longName) == 0) {
								if (opt.takesArgument) {
									if (argc == 1) {
										// Error: Argument missing for option
										return OptionMissingArgument;
									} else {
										argument = argv[1];
										argc -= 2;
										argv += 2;
									}
								} else {
									argc--;
									argv++;
									argument = NULL;
								}

								return numOpt;
							}
						}

						numOpt++;
					}

					// Invalid argument
					return OptionInvalid;
				}
			} else {
				int numOpt = 1;
				vector<Option>::const_iterator it;
				for (it = options.begin() ; it != options.end() ; it++) {
					const Option& opt = *it;

					if (opt.name != 0) {
						if (opt.name == arg[1]) {
							if (opt.takesArgument) {
								if (len > 2) {
									// The argument is directly appended
									argument = arg+2;
									argc--;
									argv++;
								} else if (argc == 1) {
									// Error: Argument missing for option
									return OptionMissingArgument;
								} else {
									argument = argv[1];
									argc -= 2;
									argv += 2;
								}
							} else {
								if (len > 2) {
									// A group of short options without arguments. To support them, we remove
									// the first option character in the group and replace it by '-', then we
									// increment arg so that arg starts with just another short option.

									arg[0] = '\0';
									arg[1] = '-';
									arg++;
								} else {
									argc--;
									argv++;
								}

								argument = NULL;
							}

							return numOpt;
						}
					}

					numOpt++;
				}

				// Invalid option
				return OptionInvalid;
			}
		} else {
			argc--;
			argv++;
			argument = arg;
			return 0;
		}
	}
}


CLIParser& CLIParser::printOption(int option)
{
	const Option& opt = options[option-1];

	printf("%s", indentation);
	if (opt.name != 0) {
		printf("-%c", opt.name);

		if (opt.takesArgument) {
			printf(" ARG");
		}

		if (opt.longName) {
			printf(", --%s", opt.longName);

			if (opt.takesArgument) {
				printf(" ARG");
			}
		}
	} else {
		printf("--%s", opt.longName);;

		if (opt.takesArgument) {
			printf(" ARG");
		}
	}
	printf("\n");

	if (opt.description) {
		string description;
		description.reserve(strlen(opt.description));

		const char* srcDescr = opt.description;

		while (*srcDescr != '\0') {
			if (*srcDescr == '%') {
				if (srcDescr[1] != '\0') {
					switch (srcDescr[1]) {
					case 'a':
						description += "ARG";
						break;
					case 's':
						description += opt.name;
						break;
					case 'l':
						description += opt.longName;
						break;
					case 'n':
						if (opt.name != 0) {
							description += '-';
							description += opt.name;
						} else {
							description += "--";
							description += opt.longName;
						}
						break;
					case '%':
						description += '%';
						break;
					default:
						srcDescr--;
					}
					srcDescr++;
				}
			} else {
				description += *srcDescr;
			}

			srcDescr++;
		}

		char* subIndentation = new char[strlen(indentation)+5];
		strcpy(subIndentation, indentation);
		strcat(subIndentation, "    ");

		char* wrapped = wrapText(description.c_str(), textWrapLength-strlen(subIndentation));
		char* indented = indent(wrapped, subIndentation);
		delete[] wrapped;
		printf("%s%s\n", subIndentation, indented);
		delete[] indented;

		delete[] subIndentation;
	}

	return *this;
}


void CLIParser::printOptions()
{
	for (unsigned int i = 0 ; i < options.size() ; i++) {
		printOption(i+1);
	}
}



