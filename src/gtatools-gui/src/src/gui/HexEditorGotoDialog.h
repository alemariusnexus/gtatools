/*
 * HexEditorGotoDialog.h
 *
 *  Created on: 21.06.2011
 *      Author: alemariusnexus
 */

#ifndef HEXEDITORGOTODIALOG_H_
#define HEXEDITORGOTODIALOG_H_

#include <ui_HexEditorGotoDialog.h>
#include "HexEditor.h"



class HexEditorGotoDialog : public QDialog {
	Q_OBJECT

public:
	HexEditorGotoDialog(QWidget* parent = NULL);
	void exec(HexEditor* editor);

private:
	Ui_HexEditorGotoDialog ui;
};

#endif /* HEXEDITORGOTODIALOG_H_ */
