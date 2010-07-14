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
	QApplication app(argc, argv);

	printf("%s\n", QLibraryInfo::location(QLibraryInfo::TranslationsPath).toLocal8Bit().constData());

	QTranslator qtTrans;
	qtTrans.load("qt_" + QLocale::system().name(), QLibraryInfo::location(QLibraryInfo::TranslationsPath));
	app.installTranslator(&qtTrans);

	QTranslator trans;
	trans.load("gtatools-gui_" + QLocale::system().name());
	app.installTranslator(&trans);

	//qRegisterMetaType<Profile>("Profile");

	MainWindow win;
	win.show();

	FirstStartWizard wiz;

	if (!File(CONFIG_FILE).exists()) {
		wiz.exec();
	}

	ProfileManager::getInstance();

	//ConfigWidget cfgWidget;
	//cfgWidget.show();

	win.initialize();

	return app.exec();
}
