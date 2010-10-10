/*
	Copyright 2010 David "Alemarius Nexus" Lerch

	This file is part of gtatools-gui.

	gtatools-gui is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	gtatools-gui is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with gtatools-gui.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "IMGGUIModule.h"
#include "../../System.h"
#include <qfiledialog.h>
#include <QtGui/QMessageBox>



IMGGUIModule::IMGGUIModule()
		: menuExists(false)
{
	System* sys = System::getInstance();

	extractAction = new QAction(tr("Extract File(s)"), NULL);
	connect(extractAction, SIGNAL(triggered(bool)), this, SLOT(onExtract(bool)));

	contextExtractAction = new QAction(tr("Extract File(s)"), NULL);
	connect(contextExtractAction, SIGNAL(triggered(bool)), this, SLOT(onContextExtract(bool)));

	connect(sys, SIGNAL(currentFileChanged(File*, File*)), this, SLOT(currentFileChanged(File*, File*)));
}


IMGGUIModule::~IMGGUIModule()
{
	delete extractAction;
	delete contextExtractAction;
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


void IMGGUIModule::buildFileTreeMenu(const QLinkedList<File*>& files, QMenu& menu)
{
	QLinkedList<File*>::const_iterator it;

	for (it = files.begin() ; it != files.end() ; it++) {
		File* file = *it;

		if (!file->getPath()->isIMGPath()) {
			return;
		}
	}

	menu.addAction(contextExtractAction);
	currentContextFiles = files;
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


void IMGGUIModule::extract(const QLinkedList<File*>& files)
{
	if (files.size() > 0) {
		if (files.size() == 1) {
			File* file = files.first();
			QString fname = QFileDialog::getSaveFileName(mainWindow, tr("Choose a destination file"),
					file->getPath()->getFileName());

			if (!fname.isEmpty()) {
				QFile dfile(fname);

				if (dfile.open(QFile::WriteOnly | QFile::Truncate)) {
					InputStream* stream = file->openStream(STREAM_BINARY);

					char buf[2048];

					while (!stream->hasReachedEnd()) {
						stream->read(buf, sizeof(buf));
						dfile.write(buf, stream->getLastReadCount());
					}

					dfile.flush();
					dfile.close();
					delete stream;
				} else {
					QMessageBox::critical(mainWindow, tr("Unable to open file"),
							tr("Unable to open file %1 for writing!").arg(fname));
				}
			}
		} else {
			QString dname = QFileDialog::getExistingDirectory(mainWindow, tr("Choose a destination directory"));

			if (!dname.isEmpty()) {
				QLinkedList<File*>::const_iterator it;

				for (it = files.begin() ; it != files.end() ; it++) {
					File* file = *it;
					QString fname = dname;
					fname.append("/").append(file->getPath()->getFileName());
					QFile dfile(fname);

					if (dfile.open(QFile::WriteOnly | QFile::Truncate)) {
						InputStream* stream = file->openStream(STREAM_BINARY);

						char buf[2048];

						while (!stream->hasReachedEnd()) {
							stream->read(buf, sizeof(buf));
							dfile.write(buf, stream->getLastReadCount());
						}

						dfile.flush();
						dfile.close();
						delete stream;
					} else {
						QMessageBox::critical(mainWindow, tr("Unable to open file"),
								tr("Unable to open file %1 for writing!").arg(fname));
					}
				}
			}
		}
	}
}


void IMGGUIModule::onExtract(bool)
{
	System* sys = System::getInstance();

	File* file = sys->getCurrentFile();

	if (file) {
		QLinkedList<File*> files;
		files << file;
		extract(files);
	}
}


void IMGGUIModule::onContextExtract(bool)
{
	extract(currentContextFiles);
}
