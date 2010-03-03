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
		: wxPanel(parent, id, pos, size, style, name), source(NULL), open(false)
{
}


bool FileDisplayer::display(DataSource* source)
{
	this->source = source;
	close();
	open = true;
	return doDisplay(source);
}


void FileDisplayer::close()
{
	if (open) {
		doClose();
		open = false;

		if (source != NULL) {
			delete source;
			source = NULL;
		}
	}
}
