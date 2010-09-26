/*
 * IMGGUIModule.cpp
 *
 *  Created on: 13.08.2010
 *      Author: alemariusnexus
 */

#include "IMGGUIModule.h"
#include "../../System.h"
#include <qfiledialog.h>



IMGGUIModule::IMGGUIModule()
		: currentContextFile(NULL), menuExists(false)
{
	System* sys = System::getInstance();

	extractAction = new QAction(tr("Extract File"), NULL);
	connect(extractAction, SIGNAL(triggered(bool)), this, SLOT(onExtract(bool)));

	contextExtractAction = new QAction(tr("Extract File"), NULL);
	connect(contextExtractAction, SIGNAL(triggered(bool)), this, SLOT(onContextExtract(bool)));

	connect(sys, SIGNAL(currentFileChanged(File*, File*)), this, SLOT(currentFileChanged(File*, File*)));
}


IMGGUIModule::~IMGGUIModule()
{
	delete extractAction;
	delete contextExtractAction;

	if (currentContextFile) {
		delete currentContextFile;
	}
}


void IMGGUIModule::doInstall()
{
	QMenu* fileMenu = mainWindow->getFileMenu();

	extractAction->setParent(mainWindow);
}


void IMGGUIModule::doUninstall()
{
	QMenu* fileMenu = mainWindow->getFileMenu();

	if (menuExists) {
		fileMenu->removeAction(extractAction);
		menuExists = false;
	}

	extractAction->setParent(NULL);
}


void IMGGUIModule::buildFileTreeMenu(const File& file, QMenu& menu)
{
	if (file.getPath()->isIMGPath()) {
		if (currentContextFile) {
			delete currentContextFile;
		}

		menu.addAction(contextExtractAction);
		currentContextFile = new File(file);
	}
}


void IMGGUIModule::currentFileChanged(File* file, File* prev)
{
	QMenu* fileMenu = mainWindow->getFileMenu();

	if (file->getPath()->isIMGPath()) {
		if (!menuExists) {
			fileMenu->addAction(extractAction);
			menuExists = true;
		}
	} else {
		if (menuExists) {
			fileMenu->removeAction(extractAction);
			menuExists = false;
		}
	}
}


void IMGGUIModule::extract(const File& file)
{
	if (file.getPath()->isIMGPath()) {
		QString fname = QFileDialog::getSaveFileName(mainWindow, tr("Choose a destination file"));

		if (!fname.isEmpty()) {
			QFile dfile(fname);

			if (dfile.open(QFile::WriteOnly | QFile::Truncate)) {
				InputStream* stream = file.openStream(STREAM_BINARY);

				char buf[2048];

				while (!stream->hasReachedEnd()) {
					stream->read(buf, sizeof(buf));
					dfile.write(buf, stream->getLastReadCount());
				}

				dfile.flush();
				dfile.close();
				delete stream;
			}
		}
	}
}


void IMGGUIModule::onExtract(bool)
{
	System* sys = System::getInstance();

	// TODO Reactivate
	/*File* file = sys->getOpenFile();

	if (file) {
		extract(*file);
	}*/
}


void IMGGUIModule::onContextExtract(bool)
{
	extract(*currentContextFile);
}
