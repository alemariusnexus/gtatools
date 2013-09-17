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

#ifndef CLIPARSER_H_
#define CLIPARSER_H_

#include <map>
#include <vector>

using std::map;
using std::vector;



/**	\brief A simple command line argument parser with a syntax similar to the POSIX standard.
 *
 * 	You first have to add possible options by calling addOption(). You can then parse() the command line
 * 	arguments.
 *
 * 	The description of the arguments may contain placeholders, which will be replaced when printOptions() is
 * 	called. It has the format %%x, where x may be one of the following:
 * 		- <b>a</b>: The name of the argument as listed in the output of printOptions(). Usually something
 * 			like ARG.
 * 		- <b>s</b>: The short name of the option or an empty string if there's no short name.
 * 		- <b>l</b>: The long name of the option or an empty string if there's no long name.
 * 		- <b>n</b>: The name of the option. If the option has a short name, that one is used, preceded by a
 * 			hyphen. If it only has a long name, that one is used, preceded by two hyphens.
 * 		- <b>%</b>: Output a literal percent character %.
 */
class CLIParser {
public:
	static const int OptionEnd = -1;
	static const int OptionInvalid = -2;
	static const int OptionMissingArgument = -3;

private:
	struct Option
	{
		char name;
		const char* longName;
		const char* description;
		bool takesArgument;
	};

public:
	CLIParser() : indentation(""), textWrapLength(60) {}

	/**	\brief Add a possible option.
	 *
	 * 	@param name The short option name (as in -f). Set to 0 if no short option shall be available.
	 * 	@param longName The long option name (as in --file). Set to NULL if no long option shall be available.
	 * 	@param description A description which is shown by printOptions(). May be NULL. See class definition
	 * 		for a description of valid placeholders.
	 * 	@param takesArgument If true, the option needs an argument (as in '-f /example/path')
	 * 	@return The option identifier. It is always > 0.
	 */
	int addOption(char name, const char* longName, const char* description, bool takesArgument = false);

	/**	\brief Parse the next command line argument(s).
	 *
	 * 	If there's an argument left in argv, the first one is read. If it's a valid option, the option
	 * 	identifier (> 0) is returned. If the option found takes a parameter, it is read and placed in
	 * 	'argument'.
	 *
	 * 	If the option is invalid (not added via addOption()), this method returns OptionInvalid and leaves
	 * 	the invalid option string at argv[0].
	 *
	 * 	If an option requiring an argument is found, but no argument was provided, OptionMissingArgument is
	 * 	returned and the flag with missing argument is left at argv[0].
	 *
	 * 	If the argument at argv[0] is not an option (i.e. not '-X' or '--XYZ'), 0 is returned and the text
	 * 	of argv[0] is placed in 'argument'.
	 *
	 * 	After the function returns, argv is changed to point to the next command line argument, and argc is
	 * 	decreased by the number of parameters evaluated by this method.
	 *
	 * 	@param argc The number of arguments left in argv.
	 * 	@param argv The command line argument array.
	 * 	@param argument Where values of parameterized options or values of non-option arguments are placed.
	 * 	@return The option identifier (a number > 0) if a valid option was parsed, the number 0 if a
	 * 		non-option was found, or an error identifier < 0 if an error occured or there were no more
	 * 		arguments to parse (OptionEnd).
	 */
	int parse(int& argc, char**& argv, char*& argument);

	void setPrintIndentation(const char* ind) { indentation = ind; }

	void setTextWrapLength(int twl) { textWrapLength = twl; }

	CLIParser& printOption(int option);

	/**	\brief Prints a string listing the valid options and their descriptions.
	 */
	void printOptions();

private:
	vector<Option> options;
	const char* indentation;
	int textWrapLength;
};

#endif /* CLIPARSER_H_ */
