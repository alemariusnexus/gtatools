/*
 * TXDFormatHandler.h
 *
 *  Created on: 11.07.2010
 *      Author: alemariusnexus
 */

#ifndef TXDFORMATHANDLER_H_
#define TXDFORMATHANDLER_H_

#include "../FormatHandler.h"



class TXDFormatHandler: public FormatHandler {
	Q_OBJECT

public:
	virtual QString getFormatName(const File* file = NULL) const { return tr("Texture Dictionary (TXD)"); };
	virtual QLinkedList<QString> getFileFormatExtensions() const { return QLinkedList<QString>() << "txd"; }
	virtual bool hasFileFormat(const File& file) const { return file.guessContentType() == CONTENT_TYPE_TXD; }
	virtual QWidget* createWidgetForFile(const File& file, QWidget* parent);
};

#endif /* TXDFORMATHANDLER_H_ */
