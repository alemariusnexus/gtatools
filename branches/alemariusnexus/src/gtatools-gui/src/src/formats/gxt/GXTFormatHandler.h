/*
 * GXTFormatHandler.h
 *
 *  Created on: 26.09.2010
 *      Author: alemariusnexus
 */

#ifndef GXTFORMATHANDLER_H_
#define GXTFORMATHANDLER_H_

#include "../FormatHandler.h"



class GXTFormatHandler : public FormatHandler {
	Q_OBJECT

public:
	virtual QString getFormatName(const File* file = NULL) const { return tr("GXT Text File"); }
	virtual QLinkedList<QString> getFileFormatExtensions() const { return QLinkedList<QString>() << "gxt"; }
	virtual bool hasFileFormat(const File& file) const;
};

#endif /* GXTFORMATHANDLER_H_ */
