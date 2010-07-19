/*
 * GUIModule.h
 *
 *  Created on: 14.07.2010
 *      Author: alemariusnexus
 */

#ifndef GUIMODULE_H_
#define GUIMODULE_H_

#include <qobject.h>
#include <qmenu.h>
#include <gtaformats/util/File.h>


class MainWindow;



class GUIModule : public QObject {
	Q_OBJECT

public:
	GUIModule() : mainWindow(NULL) {};
	virtual ~GUIModule() {};
	virtual void buildFileTreeMenu(const File& file, QMenu& menu) {}
	void install(MainWindow* mw) { mainWindow = mw; doInstall(); }
	void uninstall() { doUninstall(); mainWindow = NULL; }

private:
	virtual void doInstall() = 0;
	virtual void doUninstall() = 0;

protected:
	MainWindow* mainWindow;
};

#endif /* GUIMODULE_H_ */
