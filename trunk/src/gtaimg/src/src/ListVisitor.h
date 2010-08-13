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

#ifndef LISTCOMMAND_H_
#define LISTCOMMAND_H_

#include <gtaformats/img/IMGVisitor.h>

class ListVisitor: public IMGVisitor {
public:
	ListVisitor(int argc, char** argv);
	bool readHeader(IMGEntry* header, void*& udata);
	void readEntry(IMGEntry* entry, InputStream* stream, void*& udata);
};

#endif /* LISTCOMMAND_H_ */
