/*
 * DFFFormatProvider.h
 *
 *  Created on: 31.01.2010
 *      Author: alemariusnexus
 */

#ifndef DFFFORMATPROVIDER_H_
#define DFFFORMATPROVIDER_H_

#include "FormatProvider.h"

class DFFFormatProvider : public FormatProvider {
public:
	virtual bool canDisplay(const wxString& filename) const;
	virtual FileDisplayer* openDisplayer(wxWindow* parent, DataSource* stream);
	//virtual FileDisplayer* openDisplayer(wxWindow* parent, const wxString& filename);
	virtual wxString getFileWildcard() const;
	virtual wxString getDescription(const wxString& filename) const;
};

#endif /* DFFFORMATPROVIDER_H_ */
