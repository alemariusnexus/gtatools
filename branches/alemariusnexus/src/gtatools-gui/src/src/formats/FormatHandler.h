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
#include <qwidget.h>
#include <qlinkedlist.h>



class FormatHandler : public QObject {
	Q_OBJECT

public:
	typedef char features;

public:
	FormatHandler() {}
	virtual ~FormatHandler() {}
	virtual QString getFormatName(const File* file = NULL) const = 0;
	virtual QLinkedList<QString> getFileFormatExtensions() const = 0;
	virtual bool hasFileFormat(const File& file) const = 0;
	virtual GUIModule* createGUIModuleForFile(const File& file, MainWindow* mw) { return NULL; }
	virtual QWidget* createWidgetForFile(const File& file, QWidget* parent) { return NULL; }
	virtual void buildFileTreeMenu(const File& file, QMenu& menu) {}
};

#endif /* FORMATHANDLER_H_ */
