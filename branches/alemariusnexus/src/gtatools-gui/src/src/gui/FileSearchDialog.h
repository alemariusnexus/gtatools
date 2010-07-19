/*
 * FileSearchDialog.h
 *
 *  Created on: 17.07.2010
 *      Author: alemariusnexus
 */

#ifndef FILESEARCHDIALOG_H_
#define FILESEARCHDIALOG_H_

#include <ui_FileSearchDialog.h>
#include <qdialog.h>
#include <qwidget.h>
#include <qlinkedlist.h>
#include <qlist.h>
#include "../StringMatcher.h"
#include "../formats/FormatHandler.h"
#include <qprogressdialog.h>



class FileSearchDialog : public QDialog {
	Q_OBJECT

public:
	FileSearchDialog(QWidget* parent = NULL);

private:
	void collectResults(const File& file, StringMatcher* matcher, bool dirs, QLinkedList<FormatHandler*> handlers,
			QList<File*>& results, int filesMax, int& filesDone);

private slots:
	void onCancel(bool checked);
	void onSearch(bool checked);

private:
	Ui_FileSearchDialog ui;
};

#endif /* FILESEARCHDIALOG_H_ */
