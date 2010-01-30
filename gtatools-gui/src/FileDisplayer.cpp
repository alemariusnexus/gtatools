/*
 * FileDisplayer.cpp
 *
 *  Created on: 30.01.2010
 *      Author: alemariusnexus
 */

#include "FileDisplayer.h"
#include <fstream>

using std::ifstream;


FileDisplayer::FileDisplayer(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size,
		long style, const wxString& name)
		: wxPanel(parent, id, pos, size, style, name), stream(NULL), open(false)
{
}


bool FileDisplayer::displayFile(const wxString& filename)
{
	close();

	stream = new ifstream(filename, ifstream::in | ifstream::binary);

	if (stream->fail()) {
		return false;
	}

	return display(stream);
}


bool FileDisplayer::display(istream* stream)
{
	close();
	open = true;
	return doDisplay(stream);
}


void FileDisplayer::close()
{
	if (open) {
		doClose();
		open = false;

		if (stream != NULL) {
			((ifstream*) stream)->close();
			delete stream;
			stream = NULL;
		}
	}
}
