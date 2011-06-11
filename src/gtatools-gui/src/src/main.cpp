/*
	Copyright 2010-2011 David "Alemarius Nexus" Lerch

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

#include <gtatools-gui/config.h>
#include "MainApplication.h"
#include <QtGui/QWidget>
#include <QtGui/QTreeView>
#include <QtCore/QDir>
#include <QtGui/QFileSystemModel>
#include "gui/FileItemModel.h"
#include "Profile.h"
#include "ProfileManager.h"
#include <gtaformats/util/File.h>
#include "gui/MainWindow.h"
#include <QtCore/QSettings>
#include "Profile.h"
#include <QtCore/QMetaType>
#include "gui/ConfigWidget.h"
#include <QtCore/QTranslator>
#include <QtCore/QLibraryInfo>
#include "System.h"
#include <gtaformats/util/Exception.h>
#include <QtGui/QMessageBox>
#include <QtCore/QTimer>
#include <gta/Engine.h>
#include <gta/resource/ResourceCache.h>
#include <signal.h>
#include <gtaformats/util/util.h>



void listRecurse(File* file, int ind = 0)
{
	FileIterator* it = file->getIterator();
	File* child;

	while ((child = it->next())  !=  NULL) {
		for (int i = 0 ; i < ind ; i++) {
			printf("  ");
		}

		printf("%s\n", child->getPath()->getFileName());

		if (child->isDirectory()) {
			listRecurse(child, ind+1);
		}
	}

	delete it;
}



int main(int argc, char** argv)
{
	try {
		MainApplication app(argc, argv);

		app.setOrganizationName("gtatools");
		app.setApplicationName("gtatools-gui");
		app.setOrganizationDomain("gtatools.googlecode.com");

		QTranslator qtTrans;
		qtTrans.load("qt_" + QLocale::system().name(), QLibraryInfo::location(QLibraryInfo::TranslationsPath));
		app.installTranslator(&qtTrans);

		QTranslator trans;
		trans.load(":/gtatools-gui_" + QLocale::system().name());
		app.installTranslator(&trans);

		Engine* engine = Engine::getInstance();

		engine->getMeshCache()->resize(10 * 1000000); // 10MB
		engine->getTextureCache()->resize(25 * 1000000); // 25MB

		MainWindow win;
		win.show();

		win.initialize();

		System::getInstance()->initializeGL();

		argc--;
		argv++;

		for (int i = 0 ; i < argc ; i++) {
			File file(argv[i]);
			FileOpenRequest req(file);
			System::getInstance()->openFile(req);
		}

		ProfileManager::getInstance()->loadProfiles();

		try {
			return app.exec();
		} catch (Exception& ex) {
			System* sys = System::getInstance();
			sys->unhandeledException(ex);
		}
	} catch (Exception& ex) {
		fprintf(stderr, "### Caught unhandeled exception ###\n%s\n", ex.what());

		if (ex.getBacktrace()) {
			fprintf(stderr, "### Backtrace ###\n%s\n", ex.getBacktrace());
		}
	}
}



