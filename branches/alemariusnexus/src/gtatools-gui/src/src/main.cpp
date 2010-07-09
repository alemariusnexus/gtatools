#include <QtGui/qapplication.h>
#include <QtGui/qwidget.h>
#include <QtGui/qtreeview.h>
#include <QtCore/qdir.h>
#include <QtGui/qfilesystemmodel.h>
#include "gui/FileItemModel.h"
#include "gui/TreeModel.h"
#include "Profile.h"
#include "ProfileManager.h"
#include <gtaformats/util/File.h>



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

	Profile* profile = new Profile("Testprofil");
	profile->addResource(File("/home/alemariusnexus/Windows-Programme"));

	QWidget win;
	win.setWindowTitle("GTATools GUI Viewer");
	win.resize(800, 800);

	FileItemModel* model = new FileItemModel(profile);

	QTreeView tv(&win);
	tv.resize(800, 800);
	tv.setModel(model);

	win.show();

	return app.exec();
}
