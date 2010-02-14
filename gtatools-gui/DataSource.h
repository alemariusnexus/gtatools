/*
 * DataSource.h
 *
 *  Created on: 04.02.2010
 *      Author: alemariusnexus
 */

#ifndef DATASOURCE_H_
#define DATASOURCE_H_

#include <istream>
#include <wx/wx.h>

using std::istream;


class DataSource {
public:
	DataSource(istream* stream, const wxString& name = wxT("(Unknown)"), bool deleteStream = true);
	~DataSource();
	istream* getStream() const { return stream; }
	void setStream(istream* stream) { this->stream = stream; }
	wxString getName() const { return name; }

protected:
	DataSource(const wxString& filename);

private:
	istream* stream;
	wxString name;
	bool deleteStream;
};

#endif /* DATASOURCE_H_ */
