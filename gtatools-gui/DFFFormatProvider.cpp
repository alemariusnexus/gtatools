/*
 * DFFFormatProvider.cpp
 *
 *  Created on: 31.01.2010
 *      Author: alemariusnexus
 */

#include "DFFFormatProvider.h"

bool DFFFormatProvider::canDisplay(const wxString& filename) const
{
	return filename.Lower().EndsWith(wxT(".dff"));
}


FileDisplayer* DFFFormatProvider::openDisplayer(wxWindow* parent, DataSource* source)
{
	// TODO implement method stub
	return NULL;
}


/*FileDisplayer* DFFFormatProvider::openDisplayer(wxWindow* parent, const wxString& filename)
{
	// TODO implement method stub
	return NULL;
}*/


wxString DFFFormatProvider::getFileWildcard() const
{
	return wxT("DFF files (*.dff)|*.dff");
}


wxString DFFFormatProvider::getDescription(const wxString& filename) const
{
	return wxT("DFF Mesh file");
}
