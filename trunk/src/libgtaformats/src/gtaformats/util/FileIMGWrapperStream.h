/*
 * FileIMGWrapperStream.h
 *
 *  Created on: 18.04.2011
 *      Author: alemariusnexus
 */

#ifndef FILEIMGWRAPPERSTREAM_H_
#define FILEIMGWRAPPERSTREAM_H_

#include <ios>
#include "../img/IMGArchive.h"
#include <boost/smart_ptr/shared_ptr.hpp>

using boost::shared_ptr;


template <class StreamClass>
class FileIMGWrapperStream : public StreamClass {
public:
	FileIMGWrapperStream(StreamClass* backend, shared_ptr<IMGArchive> archive);
	~FileIMGWrapperStream();

private:
	StreamClass* backend;
	shared_ptr<IMGArchive> archive;
};


template <class StreamClass>
FileIMGWrapperStream<StreamClass>::FileIMGWrapperStream(StreamClass* backend, shared_ptr<IMGArchive> archive)
		: StreamClass(backend->rdbuf()), backend(backend), archive(archive)
{
}


template <class StreamClass>
FileIMGWrapperStream<StreamClass>::~FileIMGWrapperStream()
{
	delete backend;
}

#endif /* FILEIMGWRAPPERSTREAM_H_ */
