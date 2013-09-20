/*
	Copyright 2010-2013 David "Alemarius Nexus" Lerch

	This file is part of gtatools.

	gtatools is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	gtatools is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with gtatools.  If not, see <http://www.gnu.org/licenses/>.

	Additional permissions are granted, which are listed in the file
	GPLADDITIONS.
 */

#include <fstream>
#include <cctype>
#include <cstring>
#include <cstdio>


using std::ifstream;
using std::ofstream;

int main(int argc, char** argv)
{
	char* upperAlias = new char[strlen(argv[3])+1];
	
	for (unsigned int i = 0 ; i < strlen(argv[3]) ; i++) {
		upperAlias[i] = toupper(argv[3][i]);
	}
	
	upperAlias[strlen(argv[3])] = '\0';

	ifstream in(argv[1], ifstream::in | ifstream::binary);

	if (in.fail()) {
		fprintf(stderr, "ERROR: Input file %s does not exist!", argv[1]);
		return 1;
	}

	ofstream out(argv[2]);

	out << "// Automatically compiled from resource file " << argv[1] << std::endl;
	out << "// DO NOT EDIT THIS FILE! CHANGES WILL BE LOST UPON RECOMPILATION!" << std::endl << std::endl;
	
	out << "#ifndef RES_" << upperAlias << "_H_" << std::endl;
	out << "#define RES_" << upperAlias << "_H_" << std::endl;
	
	out << "static const unsigned char res_" << argv[3] << "_data[] = {";
	out << std::hex << std::showbase;
	
	unsigned char buffer[2048];
	
	int j = 0;
	
	while (!in.eof()) {
		in.read((char*) buffer, sizeof(buffer));
		
		for (int i = 0 ; i < in.gcount() ; i++) {
			if (j%20 == 0) {
				out << std::endl << "\t";
			}
			
			out << (int) buffer[i] << ", ";
			j++;
		}
		
		out.flush();
	}
	
	out << std::endl << "};" << std::endl;
	
	out << "#endif" << std::endl;
	
	out.flush();
	
	out.close();
	in.close();
	
	delete[] upperAlias;
	
	printf("Success");
	
	return 0;
}