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

#ifndef IMGARCHIVEHANDLER_H_
#define IMGARCHIVEHANDLER_H_

#include <nxcommon/file/ArchiveHandler.h>
#include <nxcommon/cxx11hash.h>
#include "IMGArchive.h"
#include <unordered_map>
#include <memory>

using std::unordered_map;
using std::shared_ptr;




class IMGArchiveHandler : public ArchiveHandler
{
private:
	class IMGArchiveHandlerSharedFileData : public ArchiveHandlerSharedFileData
	{
	public:
		shared_ptr<IMGArchive> img;
	};

private:
	static IMGArchiveHandler* inst;

public:
	static IMGArchiveHandler* getInstance();

public:
	IMGArchiveHandler();
	virtual ~IMGArchiveHandler() {}

	virtual bool exists(const File& file) const;
	virtual FileType getType(const File& file) const;
	virtual bool isArchiveDirectory(const File& file) const;
	virtual bool isArchiveEntry(const File& file) const;
	virtual ArchiveChildIterator* getChildIterator(const File& file) const;
	virtual File getCanonicalFile(const File& file, const File& cdir = File()) const;
	virtual istream* openInputStream(const File& file, istream::openmode mode) const;
	virtual ostream* openOutputStream(const File& file, ostream::openmode mode) const { return NULL; }
	virtual iostream* openInputOutputStream(const File& file, iostream::openmode mode) const { return NULL; }
	virtual File::filesize getSize(const File& file, bool& supported) const;

private:
	File getArchiveBaseFile(const File& file) const;
	shared_ptr<IMGArchive> getIMGArchive(const File& file) const;
	shared_ptr<IMGArchive> getIMGArchiveFromBase(const File& base) const;
	shared_ptr<IMGArchive> getIMGArchiveByPath(const CString& path) const;

private:
	unordered_map<CString, shared_ptr<IMGArchive>, CXX11Hash<CString> > archives;
};

#endif /* IMGARCHIVEHANDLER_H_ */
