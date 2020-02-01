/*
	Copyright 2010-2014 David "Alemarius Nexus" Lerch

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
#include <QWidget>
#include <QTreeView>
#include <QDir>
#include <QFileSystemModel>
#include "gui/FileItemModel.h"
#include "Profile.h"
#include "ProfileManager.h"
#include <nxcommon/file/File.h>
#include "gui/MainWindow.h"
#include <QSettings>
#include "Profile.h"
#include <QMetaType>
#include "gui/ConfigWidget.h"
#include <QTranslator>
#include <QLibraryInfo>
#include "System.h"
#include <nxcommon/exception/Exception.h>
#include <QMessageBox>
#include <QTimer>
#include <gta/Engine.h>
#include <nxcommon/ResourceCache.h>
#include <signal.h>
#include <nxcommon/util.h>
#include "formats/dff/DFFRenderWidget.h"
#include "gui/GLContainerWidget.h"
#include <fstream>
#include <nxcommon/util.h>
#include <nxcommon/file/DefaultFileFinder.h>
#include "TestWindow.h"

using std::ofstream;



void listRecurse(const File& file, int ind = 0)
{
	for (File child : file.getChildren()) {
		for (int i = 0 ; i < ind ; i++) {
			printf("  ");
		}

		printf("%s\n", child.getPath().getFileName().get());

		if (child.isDirectory()) {
			listRecurse(child, ind+1);
		}
	}
}


int main(int argc, char** argv)
{
#ifdef _WIN32
	/*printf("INFO: stdout/stderr are being redirected to files stdout.log and stderr.log from now on!\n");
	fflush(stdout);

	freopen("stdout.log", "a", stdout);
	freopen("stderr.log", "a", stderr);*/
#endif

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

		/*TestWindow* w = new TestWindow;
		w->resize(1000, 700);
		w->show();*/

		System::initialize();

		try {
			int v = app.exec();

			return v;
		} catch (Exception& ex) {
			System* sys = System::getInstance();
			sys->unhandeledException(ex);
		}
	} catch (Exception& ex) {
		fprintf(stderr, "### Caught unhandeled exception ###\n%s\n", ex.what());

		if (!ex.getBacktrace().isNull()) {
			fprintf(stderr, "### Backtrace ###\n%s\n", ex.getBacktrace().get());
		}
	}
}



