/*
	Copyright 2010-2013 David "Alemarius Nexus" Lerch

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

	Additional permissions are granted, which are listed in the file
	GPLADDITIONS.
 */

#include "IMGGUIModule.h"
#include "../../System.h"
#include <QtGui/QFileDialog>
#include <QtGui/QMessageBox>



IMGGUIModule::IMGGUIModule()
		: menuExists(false)
{
	System* sys = System::getInstance();

	extractAction = new QAction(tr("Extract File(s)"), NULL);
	connect(extractAction, SIGNAL(triggered(bool)), this, SLOT(onExtract(bool)));

	contextExtractAction = new QAction(tr("Extract File(s)"), NULL);
	connect(contextExtractAction, SIGNAL(triggered(bool)), this, SLOT(onContextExtract(bool)));

	connect(sys, SIGNAL(currentEntityChanged(DisplayedEntity*, DisplayedEntity*)), this,
			SLOT(currentEntityChanged(DisplayedEntity*, DisplayedEntity*)));
}


IMGGUIModule::~IMGGUIModule()
{
	delete extractAction;
	delete contextExtractAction;
	deleteCurrentContextFiles();
}


void IMGGUIModule::doInstall()
{
	//QMenu* fileMenu = mainWindow->getFileMenu();

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
	QLinkedList<File> toBeExtracted;
	bool allExtractable = true;

	for (it = files.begin() ; it != files.end() ; it++) {
		File* file = *it;

		if (file->isArchiveFile()) {
			FileIterator* it = file->getIterator();
			File* child;

			while ((child = it->next())  !=  NULL) {
				toBeExtracted << *child;
				delete child;
			}

			delete it;
		} else if (file->getPath()->isIMGPath()) {
			toBeExtracted << *file;
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


void IMGGUIModule::currentEntityChanged(DisplayedEntity* dent, DisplayedEntity* prev)
{
	QMenu* fileMenu = mainWindow->getFileMenu();

	DisplayedFile* dfile = dynamic_cast<DisplayedFile*>(dent);

	if (dfile  &&  (dfile->getFile().getPath()->isIMGPath()  ||  dfile->getFile().isArchiveFile())) {
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
	currentContextFiles.clear();
}


void IMGGUIModule::extract(const QLinkedList<File>& files)
{
	if (files.size() > 0) {
		if (files.size() == 1) {
			File file = files.first();
			QString fname = QFileDialog::getSaveFileName(mainWindow, tr("Choose a destination file"),
					file.getPath()->getFileName().get());

			if (!fname.isEmpty()) {
				QFile dfile(fname);

				if (dfile.open(QFile::WriteOnly | QFile::Truncate)) {
					istream* stream = file.openInputStream(istream::binary);

					char buf[2048];

					while (!stream->eof()) {
						stream->read(buf, sizeof(buf));
						dfile.write(buf, stream->gcount());
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
				QLinkedList<File>::const_iterator it;

				System* sys = System::getInstance();
				Task* task = sys->createTask();
				task->start(0, files.size(), tr("Extracting Files..."));
				int i = 0;

				for (it = files.begin() ; it != files.end() ; it++) {
					File file = *it;
					QString fname = dname;
					fname.append("/").append(file.getPath()->getFileName().get());
					QFile dfile(fname);

					if (dfile.open(QFile::WriteOnly | QFile::Truncate)) {
						istream* stream = file.openInputStream(istream::binary);

						char buf[2048];

						while (!stream->eof()) {
							stream->read(buf, sizeof(buf));
							dfile.write(buf, stream->gcount());
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

	DisplayedFile* dfile = dynamic_cast<DisplayedFile*>(sys->getCurrentEntity());

	if (dfile) {
		File file = dfile->getFile();
		QLinkedList<File> files;

		if (file.isArchiveFile()) {
			FileIterator* it = file.getIterator();
			File* child;

			while ((child = it->next())  !=  NULL) {
				files << *child;
				delete child;
			}

			delete it;
		} else {
			files << file;
		}

		extract(files);
	}
}


void IMGGUIModule::onContextExtract(bool)
{
	extract(currentContextFiles);
	deleteCurrentContextFiles();
}
