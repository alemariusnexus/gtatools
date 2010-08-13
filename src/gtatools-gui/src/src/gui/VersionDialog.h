/*
 * VersionDialog.h
 *
 *  Created on: 13.08.2010
 *      Author: alemariusnexus
 */

#ifndef VERSIONDIALOG_H_
#define VERSIONDIALOG_H_

#include <ui_VersionDialog.h>
#include <qdialog.h>
#include <qwidget.h>


class VersionDialog : public QDialog {
	Q_OBJECT

public:
	VersionDialog(QWidget* parent = NULL);

private:
	Ui_VersionDialog ui;
};

#endif /* VERSIONDIALOG_H_ */
