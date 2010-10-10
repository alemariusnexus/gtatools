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

#include <QtGui/qapplication.h>
#include <QtGui/qwidget.h>
#include <QtGui/qtreeview.h>
#include <QtCore/qdir.h>
#include <QtGui/qfilesystemmodel.h>
#include "gui/FileItemModel.h"
#include "Profile.h"
#include "ProfileManager.h"
#include <gtaformats/util/File.h>
#include "gui/MainWindow.h"
#include <qsettings.h>
#include <config.h>
#include "gui/FirstStartWizard.h"
#include <ui_FirstStartWizard.h>
#include "Profile.h"
#include <qmetatype.h>
#include "gui/ConfigWidget.h"
#include <qtranslator.h>
#include <qlibraryinfo.h>
#include "System.h"
#include <gtaformats/util/Exception.h>
#include <qmessagebox.h>
#include <qtimer.h>
#include <qmessagebox.h>



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
		QApplication app(argc, argv);

		QTranslator qtTrans;
		qtTrans.load("qt_" + QLocale::system().name(), QLibraryInfo::location(QLibraryInfo::TranslationsPath));
		app.installTranslator(&qtTrans);

		QTranslator trans;
		trans.load(":/gtatools-gui_" + QLocale::system().name());
		app.installTranslator(&trans);

		MainWindow win;
		win.show();

		if (!File(CONFIG_FILE).exists()) {
			if (!File(CONFIG_DIR).exists()) {
				if (!File(CONFIG_DIR).mkdir()) {
					QMessageBox::critical(NULL, win.tr("Configuration Error"),
							win.tr("Configuration directory %1 could not be created. Program will terminate.").arg(CONFIG_DIR));
					return 1;
				}
			}

			QSettings settings(CONFIG_FILE, QSettings::IniFormat);
			settings.sync();
		}

		ProfileManager::getInstance()->loadProfiles();

		win.initialize();

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



