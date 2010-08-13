/*
	Copyright 2010 David "Alemarius Nexus" Lerch

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

#ifndef LISTCOMMAND_H_
#define LISTCOMMAND_H_

#include <gtaformats/txd/TXDVisitor.h>

class ListVisitor : public TXDVisitor {
public:
	ListVisitor(int argc, char** argv);
	void handleTexture(TXDArchive* archive, TXDTexture* header);

private:
	bool useCommaAsSeparator;
};

#endif /* LISTCOMMAND_H_ */
