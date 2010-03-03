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
	DataSource(istream* stream, const wxString& name = wxT("(Unknown)"), bool deleteStream = true);

	/**	\brief Deletes this DataSource.
	 *
	 * 	This will close the associated stream if deleteStream was true in the constructor.
	 */
	~DataSource();

	/**	\brief Returns the associated stream.
	 *
	 *	@return The associated stream.
	 */
	istream* getStream() const { return stream; }

	/**	\brief Sets the associated stream.
	 *
	 *	The old one will not be deleted!
	 *
	 *	@param stream The new stream.
	 */
	void setStream(istream* stream) { this->stream = stream; }

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
	istream* stream;
	wxString name;
	bool deleteStream;
};

#endif /* DATASOURCE_H_ */
