/*
 * DefaultGUIModule.cpp
 *
 *  Created on: 16.07.2010
 *      Author: alemariusnexus
 */

#include "DefaultGUIModule.h"
#include <qdesktopservices.h>
#include <qurl.h>



DefaultGUIModule::DefaultGUIModule(MainWindow* mw)
		: GUIModule(mw), contextFile(NULL)
{
	systemOpenAction = new QAction(tr("Execute System Program"), mw);
	connect(systemOpenAction, SIGNAL(triggered(bool)), this, SLOT(onOpenSystemProgram(bool)));
}


DefaultGUIModule::~DefaultGUIModule()
{
	delete systemOpenAction;

	if (contextFile) {
		delete contextFile;
	}
}


void DefaultGUIModule::buildFileTreeMenu(const File& file, QMenu& menu)
{
	if (contextFile) {
		delete contextFile;
		contextFile = NULL;
	}

	menu.addAction(systemOpenAction);
	contextFile = new File(file);
}


void DefaultGUIModule::onOpenSystemProgram(bool checked)
{
	QDesktopServices::openUrl(QUrl(QString("file://%1").arg(contextFile->getPath()->toString())));
}
