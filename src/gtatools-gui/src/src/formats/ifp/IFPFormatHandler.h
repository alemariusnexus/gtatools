/*
 * IFPFormatHandler.h
 *
 *  Created on: 02.01.2012
 *      Author: alemariusnexus
 */

#ifndef IFPFORMATHANDLER_H_
#define IFPFORMATHANDLER_H_

#include "../FormatHandler.h"


class IFPFormatHandler : public FormatHandler
{
	Q_OBJECT

public:
	virtual QString getFormatName(const File* file = NULL) const { return tr("IFP Animation Package"); }
	virtual QLinkedList<QString> getFileFormatExtensions() const { return QLinkedList<QString>() << "ifp"; }
	virtual bool hasFileFormat(const File& file) const { return file.guessContentType() == CONTENT_TYPE_IFP; }
	virtual DisplayedFile* openFile(const FileOpenRequest& request);
};

#endif /* IFPFORMATHANDLER_H_ */
