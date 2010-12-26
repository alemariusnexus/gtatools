/*
	Copyright 2010 David "Alemarius Nexus" Lerch

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
 */

#ifndef COLLOADER_H_
#define COLLOADER_H_

#include "../config.h"
#include "COLModel.h"
#include "../util/stream/InputStream.h"
#include "../util/File.h"



class COLLoader {
public:
	COLModel* loadModel(InputStream* stream);
	COLModel* loadModel(const File& file);
	bool loadModelName(InputStream* stream, char* name);
	bool loadModelName(const File& file, char* name);
	COLVersion getVersion(InputStream* stream);
	COLVersion getVersion(const File& file);

private:

};

#endif /* COLLOADER_H_ */
