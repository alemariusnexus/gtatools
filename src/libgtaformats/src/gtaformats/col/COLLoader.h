/*
	Copyright 2010-2014 David "Alemarius Nexus" Lerch

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

#ifndef COLLOADER_H_
#define COLLOADER_H_

#include <gtaformats/config.h>
#include "COLModel.h"
#include "COLException.h"
#include <nxcommon/file/File.h>
#include <istream>

using std::istream;
using std::streamoff;



class COLLoader {
public:
	COLModel* loadModel(istream* stream);
	COLModel* loadModel(const File& file);
	CString loadModelName(istream* stream);
	CString loadModelName(const File& file);
	COLVersion getVersion(istream* stream);
	COLVersion getVersion(const File& file);
	void skip(istream* stream, size_t numEntries);

private:
	void throwPrematureEndException(istream* stream, COLModel* model)
	{
		CString errmsg("Premature end of COL model: Model ");

		if (model  &&  !model->getName().isNull()) {
			errmsg << model->getName() << " ";
		}

		errmsg << "ended at local offset " << (stream->tellg() - colStart) << " (absolute: " << stream->tellg() << ").";

		throw COLException(errmsg, __FILE__, __LINE__);
	}

private:
	streamoff colStart;
};

#endif /* COLLOADER_H_ */
