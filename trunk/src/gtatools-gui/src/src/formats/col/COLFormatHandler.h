/*
 * COLFormatHandler.h
 *
 *  Created on: 30.01.2011
 *      Author: alemariusnexus
 */

#ifndef COLFORMATHANDLER_H_
#define COLFORMATHANDLER_H_

#include "../FormatHandler.h"

class COLFormatHandler: public FormatHandler
{
public:
	virtual QString getFormatName(const File* file = NULL) const { return tr("Collision File (COL)"); }
	virtual QLinkedList<QString> getFileFormatExtensions() const { return QLinkedList<QString>() << "col"; }
	virtual bool hasFileFormat(const File& file) const { return file.guessContentType() == CONTENT_TYPE_COL; }
	virtual QWidget* createWidgetForFile(const FileOpenRequest& request, QWidget* parent);
};

#endif /* COLFORMATHANDLER_H_ */
