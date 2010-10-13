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
	deleteCurrentContextFiles();
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
	QLinkedList<File*> toBeExtracted;
	bool allExtractable = true;

	for (it = files.begin() ; it != files.end() ; it++) {
		File* file = *it;

		if (file->isArchiveFile()) {
			FileIterator* it = file->getIterator();
			File* child;

			while ((child = it->next())  !=  NULL) {
				toBeExtracted << child;
			}

			delete it;
		} else if (file->getPath()->isIMGPath()) {
			toBeExtracted << new File(*file);
		} else {
			allExtractable = false;
			break;
		}
	}

	currentContextFiles = toBeExtracted;

	if (allExtractable) {
		menu.addAction(contextExtractAction);
	} else {
		deleteCurrentContextFiles();
	}
}


void IMGGUIModule::currentFileChanged(File* file, File* prev)
{
	QMenu* fileMenu = mainWindow->getFileMenu();

	if (file->getPath()->isIMGPath()  ||  file->isArchiveFile()) {
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


void IMGGUIModule::deleteCurrentContextFiles()
{
	QLinkedList<File*>::iterator it;

	for (it = currentContextFiles.begin() ; it != currentContextFiles.end() ; it++) {
		delete *it;
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

				System* sys = System::getInstance();
				Task* task = sys->createTask();
				task->start(0, files.size(), tr("Extracting Files..."));
				int i = 0;

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

					i++;
					task->update(i);
					qApp->processEvents();
				}

				delete task;
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

		if (file->isArchiveFile()) {
			FileIterator* it = file->getIterator();
			File* child;

			while ((child = it->next())  !=  NULL) {
				files << child;
			}

			delete it;
		} else {
			files << file;
		}

		extract(files);

		if (file->isArchiveFile()) {
			QLinkedList<File*>::iterator it;

			for (it = files.begin() ; it != files.end() ; it++) {
				delete *it;
			}
		}
	}
}


void IMGGUIModule::onContextExtract(bool)
{
	extract(currentContextFiles);
	deleteCurrentContextFiles();
}
