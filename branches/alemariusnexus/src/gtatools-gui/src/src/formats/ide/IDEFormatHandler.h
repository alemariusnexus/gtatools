/*
 * TXDFormatHandler.h
 *
 *  Created on: 11.07.2010
 *      Author: alemariusnexus
 */

#ifndef IDEFORMATHANDLER_H_
#define IDEFORMATHANDLER_H_

#include "../FormatHandler.h"

class IDEFormatHandler: public FormatHandler {
	Q_OBJECT

public:
	virtual QString getFormatName(const File* file = NULL) const { return tr("Item Definition File (IDE)"); }
	virtual QLinkedList<QString> getFileFormatExtensions() const { return QLinkedList<QString>() << "ide"; }
	virtual bool hasFileFormat(const File& file) const { return file.guessContentType() == CONTENT_TYPE_IDE; }
};

#endif /* IDEFORMATHANDLER_H_ */
