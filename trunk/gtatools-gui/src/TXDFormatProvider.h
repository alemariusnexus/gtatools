/*
 * TXDFormatProvider.h
 *
 *  Created on: 30.01.2010
 *      Author: alemariusnexus
 */

#ifndef TXDFORMATPROVIDER_H_
#define TXDFORMATPROVIDER_H_

#include "FormatProvider.h"
#include <wx/wx.h>

class TXDFormatProvider : public FormatProvider {
public:
	virtual bool canDisplay(const wxString& filename) const;
	virtual FileDisplayer* openDisplayer(wxWindow* parent, istream* stream);
	virtual FileDisplayer* openDisplayer(wxWindow* parent, const wxString& filename);
	virtual wxString getFileWildcard() const;
	virtual wxString getDescription(const wxString& filename) const;
};

#endif /* TXDFORMATPROVIDER_H_ */
