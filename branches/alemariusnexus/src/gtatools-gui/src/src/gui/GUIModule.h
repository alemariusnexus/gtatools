/*
 * GUIModule.h
 *
 *  Created on: 14.07.2010
 *      Author: alemariusnexus
 */

#ifndef GUIMODULE_H_
#define GUIMODULE_H_

class MainWindow;

#include <qobject.h>
#include <qmenu.h>
#include <gtaformats/util/File.h>



class GUIModule : public QObject {
	Q_OBJECT

public:
	GUIModule(MainWindow* mw) : mainWindow(mw) {};
	virtual ~GUIModule() {};
	virtual void buildFileTreeMenu(const File& file, QMenu& menu) {}

protected:
	MainWindow* mainWindow;
};

#endif /* GUIMODULE_H_ */
