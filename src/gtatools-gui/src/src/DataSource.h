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

#ifndef DATASOURCE_H_
#define DATASOURCE_H_

#include <wx/wx.h>
#include <gtaformats/util/stream/InputStream.h>



/**	\brief Represents a source from which data can be read.
 *
 * 	This class is used by parts of the program that load data to display it. It is used to have a common
 * 	interface for both direct file and embedded (like entries in IMG files) access.
 *
 * 	\see FileDataSource
 */
class DataSource {
public:
	/**	\brief Creates a new DataSource with the given stream.
	 *
	 *	If deleteStream is true, the stream will be deleted when this object's destructor is called.
	 *	The name can be used to identify the source (e.g. a file name) in messages to the user.
	 *
	 *	@param stream The stream to read data from.
	 *	@param name A name that can be used to describe this DataSource to the user.
	 *	@param deleteStream Whether to automatically delete the associated stream on destruction.
	 */
	DataSource(InputStream* stream, const wxString& name = wxT("(Unknown)"), bool deleteStream = true);

	/**	\brief Deletes this DataSource.
	 *
	 * 	This will close the associated stream if deleteStream was true in the constructor.
	 */
	~DataSource();

	/**	\brief Returns the associated stream.
	 *
	 *	@return The associated stream.
	 */
	InputStream* getStream() const { return stream; }

	/**	\brief Sets the associated stream.
	 *
	 *	The old one will not be deleted!
	 *
	 *	@param stream The new stream.
	 */
	void setStream(InputStream* stream) { this->stream = stream; }

	/**	\brief Returns the name of this DataSource that can be used as a description to the user.
	 *
	 * 	This is the string which was passed to the constructor.
	 *
	 *	@return The name of this DataSource. May be something like "(Unknown)", too.
	 */
	wxString getName() const { return name; }

protected:
	/**	\brief Constructor for FileDataSource.
	 *
	 *	name will be equal to filename and a new ifstream will be opened that is automatically deleted
	 *	on destruction.
	 *
	 *	@param filename
	 *	@return
	 */
	DataSource(const wxString& filename);

private:
	InputStream* stream;
	wxString name;
	bool deleteStream;
};

#endif /* DATASOURCE_H_ */
