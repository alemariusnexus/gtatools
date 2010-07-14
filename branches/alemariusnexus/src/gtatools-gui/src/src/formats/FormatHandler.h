/*
 * FormatProvider.h
 *
 *  Created on: 10.07.2010
 *      Author: alemariusnexus
 */

#ifndef FORMATHANDLER_H_
#define FORMATHANDLER_H_

#include <gtaformats/util/File.h>
#include <qstring.h>
#include <qobject.h>
#include "../gui/GUIModule.h"



#define FORMAT_HANDLER_DESCRIBE (1 << 0)
#define FORMAT_HANDLER_GUIMODULE (1 << 1)



class FormatHandler : public QObject {
	Q_OBJECT

public:
	typedef char features;

public:
	virtual features getFileFeatures(const File& file, bool fromContent) = 0;
	virtual QString getFileFormatName(const File& file, bool fromContent = true) = 0;
	virtual GUIModule* createGUIModuleForFile(const File& file) { return NULL; }
};

#endif /* FORMATHANDLER_H_ */
