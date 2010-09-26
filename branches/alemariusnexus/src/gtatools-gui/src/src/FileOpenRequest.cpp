/*
 * FileOpenRequest.cpp
 *
 *  Created on: 25.09.2010
 *      Author: alemariusnexus
 */

#include "FileOpenRequest.h"


FileOpenRequest::FileOpenRequest(const File& file)
		: file(new File(file))
{
}


FileOpenRequest::~FileOpenRequest()
{
	delete file;
}


QVariant FileOpenRequest::getAttribute(const QString& key) const
{
	MetaDataMap::const_iterator it = metadata.find(key);

	if (it == metadata.end()) {
		return QVariant();
	}

	return it.value();
}
