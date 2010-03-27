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
#include <fstream>
#include <wx/filename.h>

using std::ifstream;


DataSource::DataSource(istream* stream, const wxString& name, bool deleteStream)
		: stream(stream), name(name), deleteStream(deleteStream)
{
}


DataSource::DataSource(const wxString& filename)
		: stream(new ifstream(filename.mb_str(), ifstream::in | ifstream::binary)),
		  name(wxFileName(filename).GetFullName()), deleteStream(true)
{
	stream->exceptions(ifstream::failbit | ifstream::badbit | ifstream::eofbit);
}


DataSource::~DataSource()
{
	if (deleteStream) {
		delete stream;
	}
}
