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
	virtual features getFileFeatures(const File& file, bool fromContent);
	virtual QString getFileFormatName(const File& file, bool fromContent = true);
	virtual QWidget* createWidgetForFile(const File& file, QWidget* parent);
};

#endif /* TXDFORMATHANDLER_H_ */
