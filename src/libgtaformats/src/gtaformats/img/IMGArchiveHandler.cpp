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

#include "IMGArchiveHandler.h"
#include "IMGException.h"
#include "IMGArchiveChildIterator.h"
#include <nxcommon/file/FileException.h>
#include <nxcommon/stream/RangedStream.h>




IMGArchiveHandler* IMGArchiveHandler::inst = new IMGArchiveHandler;




IMGArchiveHandler* IMGArchiveHandler::getInstance()
{
	return inst;
}


IMGArchiveHandler::IMGArchiveHandler()
{
}


bool IMGArchiveHandler::exists(const File& file) const
{
	FilePath dirPath = file.getPath().getDirectoryPath();
	FileContentType ctype = dirPath.guessContentType();

	if (ctype == CONTENT_TYPE_IMG  ||  ctype == CONTENT_TYPE_DIR) {
		shared_ptr<IMGArchive> img = getIMGArchiveFromBase(File(dirPath));

		IMGArchive::EntryIterator it = img->getEntryByName(file.getPath().getFileName().get());

		return it != img->getEntryEnd();
	} else {
		return false;
	}
}


FileType IMGArchiveHandler::getType(const File& file) const
{
	if (exists(file)) {
		return TYPE_FILE;
	}

	return TYPE_ERROR;
}


bool IMGArchiveHandler::isArchiveDirectory(const File& file) const
{
	if (!file.physicallyExists())
		return false;

	FileContentType ctype = file.guessContentType();

	return ctype == CONTENT_TYPE_IMG  ||  ctype == CONTENT_TYPE_DIR;
}


bool IMGArchiveHandler::isArchiveEntry(const File& file) const
{
	File base(file.getPath().getDirectoryPath());

	if (!isArchiveDirectory(base))
		return false;
	if (!exists(file))
		return false;

	return true;
}


ArchiveChildIterator* IMGArchiveHandler::getChildIterator(const File& file) const
{
	if (!isArchiveDirectory(file))
		return NULL;

	return new IMGArchiveChildIterator(file, getIMGArchive(file));
}


File IMGArchiveHandler::getCanonicalFile(const File& file, const File& cdir) const
{
	// We know that file does NOT physically exist. Also, we know that cdir is either null, or cdir
	// does NOT physically exist either.

	// NOTE: These files don't exist, so don't use getParent() on them, it will fail!

	if (cdir.isNull()) {
		File base = getArchiveBaseFile(file);

		if (base.isNull()  ||  !base.physicallyExists()  ||  !isArchiveDirectory(base))
			return File();

		File canParent = base.getCanonicalFile();

		return File(canParent, file.getPath().getFileName());
	} else {
		// For IMG, this only happens when cdir is an IMG entry, but an IMG entry is not a directory.

		return File();
	}
}


istream* IMGArchiveHandler::openInputStream(const File& file, istream::openmode mode) const
{
	shared_ptr<IMGArchive> img = getIMGArchive(file);

	if (img == NULL)
		return NULL;

	IMGArchive::EntryIterator it = img->getEntryByName(file.getPath().getFileName());

	if (it == img->getEntryEnd())
			return NULL;

	IMGEntry& entry = *it;
	File base = getArchiveBaseFile(file);

	istream* imgStream = base.openInputStream(istream::binary);
	imgStream->seekg(IMG_BLOCKS2BYTES(entry.offset));

	RangedStream<istream>* rstream = new RangedStream<istream>(imgStream,
					IMG_BLOCKS2BYTES(entry.size), true);

	return rstream;
}


File::filesize IMGArchiveHandler::getSize(const File& file, bool& supported) const
{
	if (!isArchiveEntry(file)) {
		supported = false;
		return 0;
	}

	shared_ptr<IMGArchive> img = getIMGArchive(file);
	IMGArchive::EntryIterator it = img->getEntryByName(file.getPath().getFileName().get());

	if (it == img->getEntryEnd()) {
		supported = false;
		return 0;
	}

	supported = true;
	return IMG_BLOCKS2BYTES(it->size);
}


File IMGArchiveHandler::getArchiveBaseFile(const File& file) const
{
	if (isArchiveDirectory(file))
		return file;
	if (isArchiveEntry(file))
		return File(file.getPath().getDirectoryPath());
	return File();
}


shared_ptr<IMGArchive> IMGArchiveHandler::getIMGArchiveByPath(const CString& path) const
{
	auto it = archives.find(path);

	if (it == archives.end()) {
		shared_ptr<IMGArchive> img = shared_ptr<IMGArchive>(new IMGArchive(File(path)));
		const_cast<IMGArchiveHandler*>(this)->archives[path] = img;
		return img;
	}

	return it->second;
}


shared_ptr<IMGArchive> IMGArchiveHandler::getIMGArchiveFromBase(const File& base) const
{
	ArchiveHandlerSharedFileData* data = base.getArchiveHandlerData(const_cast<IMGArchiveHandler*>(this));

	if (!data) {
		IMGArchiveHandlerSharedFileData* sdata = new IMGArchiveHandlerSharedFileData;

		//printf("Base: %s\n", base.getPath().toString().get());
		File cBaseFile = base.getCanonicalFile();

		if (cBaseFile.isNull()) {
			delete sdata;
			throw IMGException("IMGArchiveHandler::getIMGArchive() called on file whose canonical path can't be resolved!",
					__FILE__, __LINE__);
		}

		shared_ptr<IMGArchive> img = getIMGArchiveByPath(cBaseFile.getPath().toString());
		sdata->img = img;

		base.setArchiveHandlerData(const_cast<IMGArchiveHandler*>(this), sdata);

		return img;
	}

	return ((IMGArchiveHandlerSharedFileData*) data)->img;
}


shared_ptr<IMGArchive> IMGArchiveHandler::getIMGArchive(const File& file) const
{
	ArchiveHandlerSharedFileData* data = file.getArchiveHandlerData(const_cast<IMGArchiveHandler*>(this));

	if (!data) {
		File baseFile = getArchiveBaseFile(file);

		if (baseFile.isNull())
			return NULL;

		return getIMGArchiveFromBase(baseFile);
	}

	return ((IMGArchiveHandlerSharedFileData*) data)->img;
}
