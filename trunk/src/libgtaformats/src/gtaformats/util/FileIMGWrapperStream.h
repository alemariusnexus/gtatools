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

#ifndef FILEIMGWRAPPERSTREAM_H_
#define FILEIMGWRAPPERSTREAM_H_

#include <ios>
#include "../img/IMGArchive.h"
#include <boost/smart_ptr/shared_ptr.hpp>

using boost::shared_ptr;


template <class StreamClass>
class FileIMGWrapperStream : public StreamClass {
public:
	FileIMGWrapperStream(StreamClass* backend, shared_ptr<IMGArchive> archive);
	~FileIMGWrapperStream();

private:
	StreamClass* backend;
	shared_ptr<IMGArchive> archive;
};


template <class StreamClass>
FileIMGWrapperStream<StreamClass>::FileIMGWrapperStream(StreamClass* backend, shared_ptr<IMGArchive> archive)
		: StreamClass(backend->rdbuf()), backend(backend), archive(archive)
{
}


template <class StreamClass>
FileIMGWrapperStream<StreamClass>::~FileIMGWrapperStream()
{
	delete backend;
}

#endif /* FILEIMGWRAPPERSTREAM_H_ */
