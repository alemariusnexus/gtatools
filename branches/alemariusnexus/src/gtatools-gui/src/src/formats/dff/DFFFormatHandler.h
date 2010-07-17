/*
 * TXDFormatHandler.h
 *
 *  Created on: 11.07.2010
 *      Author: alemariusnexus
 */

#ifndef DFFFORMATHANDLER_H_
#define DFFFORMATHANDLER_H_

#include "../FormatHandler.h"


class DFFFormatHandler: public FormatHandler {
	Q_OBJECT

public:
	virtual QString getFormatName(const File* file = NULL) const { return tr("DFF Mesh"); }
	virtual QLinkedList<QString> getFileFormatExtensions() const { return QLinkedList<QString>() << "dff"; }
	virtual bool hasFileFormat(const File& file) const { return file.guessContentType() == CONTENT_TYPE_DFF; }
};

#endif /* DFFFORMATHANDLER_H_ */
