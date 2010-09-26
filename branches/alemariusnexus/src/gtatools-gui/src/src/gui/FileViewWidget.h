/*
 * FileViewWidget.h
 *
 *  Created on: 24.09.2010
 *      Author: alemariusnexus
 */

#ifndef FILEVIEWWIDGET_H_
#define FILEVIEWWIDGET_H_

#include <ui_FileViewWidget.h>
#include <QtGui/qwidget.h>
#include "../FileOpenRequest.h"



class FileViewWidget : public QWidget {
	Q_OBJECT

public:
	FileViewWidget(const FileOpenRequest& request);

private:
	Ui_FileViewWidget ui;
	QWidget* displayWidget;
};

#endif /* FILEVIEWWIDGET_H_ */
