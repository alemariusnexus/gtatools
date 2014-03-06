#include "TestWindow.h"
#include <QtCore/QFile>
#include <QtGui/QBoxLayout>
#include <QtGui/QAction>



TestWindow::TestWindow()
		: QMainWindow()
{
	QWidget* cw = new QWidget(this);

	QVBoxLayout* layout = new QVBoxLayout(cw);
	cw->setLayout(layout);

	setCentralWidget(cw);

	textEdit = new QTextEdit(this);
	textEdit->setReadOnly(true);

	QFile file("/home/alemariusnexus/test.htm");
	file.open(QFile::ReadOnly);
	QByteArray bcont = file.readAll();
	QString cont(bcont);
	textEdit->setHtml(bcont);

	printf("%u bytes!\n", bcont.size());

	layout->addWidget(textEdit);
}

