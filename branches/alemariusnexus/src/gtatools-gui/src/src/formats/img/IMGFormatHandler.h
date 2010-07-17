/*
 * IMGFormatHandler.h
 *
 *  Created on: 10.07.2010
 *      Author: alemariusnexus
 */

#ifndef IMGFORMATHANDLER_H_
#define IMGFORMATHANDLER_H_

#include "../FormatHandler.h"


class IMGFormatHandler : public FormatHandler {
	Q_OBJECT

public:
	virtual QString getFormatName(const File* file = NULL) const;
	virtual QLinkedList<QString> getFileFormatExtensions() const { return QLinkedList<QString>() << "img" << "dir"; }
	virtual bool hasFileFormat(const File& file) const { return file.guessContentType() == CONTENT_TYPE_IMG; }
};

#endif /* IMGFORMATHANDLER_H_ */
