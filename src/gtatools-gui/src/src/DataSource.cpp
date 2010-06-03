/*
	Copyright 2010 David Lerch

	This file is part of gtatools-gui.

	gtatools-gui is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	gtatools-gui is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with gtatools-gui.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "DataSource.h"
#include <wx/filename.h>
#include <gtaformats/util/stream/FileInputStream.h>


DataSource::DataSource(InputStream* stream, const wxString& name, bool deleteStream)
		: stream(stream), name(name), deleteStream(deleteStream)
{
}


DataSource::DataSource(const wxString& filename)
		: stream(new FileInputStream(filename.mb_str(), STREAM_BINARY)),
		  name(wxFileName(filename).GetFullName()), deleteStream(true)
{
}


DataSource::~DataSource()
{
	if (deleteStream) {
		delete stream;
	}
}
