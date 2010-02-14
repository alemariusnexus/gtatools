/*
 * FileDataSource.h
 *
 *  Created on: 04.02.2010
 *      Author: alemariusnexus
 */

#ifndef FILEDATASOURCE_H_
#define FILEDATASOURCE_H_

#include "DataSource.h"
#include <wx/wx.h>


class FileDataSource : public DataSource {
public:
	FileDataSource(const wxString& filename);
	const wxString& getFilename() const { return filename; }

private:
	wxString filename;
};

#endif /* FILEDATASOURCE_H_ */
