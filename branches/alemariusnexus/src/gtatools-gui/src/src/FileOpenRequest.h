/*
 * FileOpenRequest.h
 *
 *  Created on: 25.09.2010
 *      Author: alemariusnexus
 */

#ifndef FILEOPENREQUEST_H_
#define FILEOPENREQUEST_H_

#include <gtaformats/util/File.h>
#include <QtCore/qhash.h>
#include <QtCore/qstring.h>
#include <QtCore/qvariant.h>



class FileOpenRequest {
private:
	typedef QHash<QString, QVariant> MetaDataMap;

public:
	FileOpenRequest(const File& file);
	~FileOpenRequest();
	void setAttribute(const QString& key, const QVariant& value) { metadata[key] = value; }
	QVariant getAttribute(const QString& key) const;
	File* getFile() const { return file; }

private:
	File* file;
	MetaDataMap metadata;
};

#endif /* FILEOPENREQUEST_H_ */
