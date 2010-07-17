/*
 * TXDFormatHandler.h
 *
 *  Created on: 11.07.2010
 *      Author: alemariusnexus
 */

#ifndef IPLFORMATHANDLER_H_
#define IPLFORMATHANDLER_H_

#include "../FormatHandler.h"

class IPLFormatHandler: public FormatHandler {
	Q_OBJECT

public:
	virtual QString getFormatName(const File* file = NULL) const { return tr("Item Placement File (IPL)"); }
	virtual QLinkedList<QString> getFileFormatExtensions() const { return QLinkedList<QString>() << "ipl"; }
	virtual bool hasFileFormat(const File& file) const { return file.guessContentType() == CONTENT_TYPE_IPL; }
};

#endif /* IPLFORMATHANDLER_H_ */
