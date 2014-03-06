#ifndef TESTWINDOW_H_
#define TESTWINDOW_H_

#include <QtGui/QMainWindow>
#include <QtGui/QTextEdit>
#include <QtCore/QList>


class TestWindow : public QMainWindow
{
	Q_OBJECT

public:
	TestWindow();

private:
	QTextEdit* textEdit;
};

#endif /* TESTWINDOW_H_ */
