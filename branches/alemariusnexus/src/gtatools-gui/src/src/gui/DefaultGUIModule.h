/*
 * DefaultGUIModule.h
 *
 *  Created on: 16.07.2010
 *      Author: alemariusnexus
 */

#ifndef DEFAULTGUIMODULE_H_
#define DEFAULTGUIMODULE_H_

#include "GUIModule.h"
#include "MainWindow.h"
#include <qobject.h>
#include <qaction.h>



class DefaultGUIModule : public GUIModule {
	Q_OBJECT

public:
	DefaultGUIModule(MainWindow* mw);
	virtual ~DefaultGUIModule();
	virtual void buildFileTreeMenu(const File& file, QMenu& menu);

private slots:
	void onOpenSystemProgram(bool checked);

private:
	QAction* systemOpenAction;
	File* contextFile;
};

#endif /* DEFAULTGUIMODULE_H_ */
