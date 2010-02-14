/*
 * DataSource.cpp
 *
 *  Created on: 04.02.2010
 *      Author: alemariusnexus
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
