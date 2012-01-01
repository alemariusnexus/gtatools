/*
	Copyright 2010-2012 David "Alemarius Nexus" Lerch

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

	Additional permissions are granted, which are listed in the file
	GPLADDITIONS.
 */

#ifndef FILEOPENREQUEST_H_
#define FILEOPENREQUEST_H_

#include <gtaformats/util/File.h>
#include <QtCore/QHash>
#include <QtCore/QString>
#include <QtCore/QVariant>



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
