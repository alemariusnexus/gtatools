/*
 * DFFXMLDumpDialog.h
 *
 *  Created on: 07.08.2010
 *      Author: alemariusnexus
 */

#ifndef DFFXMLDUMPDIALOG_H_
#define DFFXMLDUMPDIALOG_H_


#include <ui_DFFXMLDumpDialog.h>
#include <qdialog.h>


class DFFXMLDumpDialog : public QDialog
{
public:
	DFFXMLDumpDialog(QWidget* parent = NULL);
	bool isFrameDumpChecked() { return ui.frameBox->isChecked(); }
	bool isGeometryDumpChecked() { return ui.geometryBox->isChecked(); }
	bool isPartDumpChecked() { return ui.partBox->isChecked(); }
	bool isVertexDataDumpChecked() { return ui.vertexDataBox->isChecked(); }
	bool isMaterialDumpChecked() { return ui.materialBox->isChecked(); }
	bool isIndexDumpChecked() { return ui.indexBox->isChecked(); }

private:
	Ui_DFFXMLDumpDialog ui;
};

#endif /* DFFXMLDUMPDIALOG_H_ */
