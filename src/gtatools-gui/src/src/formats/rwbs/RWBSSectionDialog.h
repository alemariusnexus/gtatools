/*
	Copyright 2010-2012 David "Alemarius Nexus" Lerch

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

	Additional permissions are granted, which are listed in the file
	GPLADDITIONS.
 */

#ifndef RWBSSECTIONDIALOG_H_
#define RWBSSECTIONDIALOG_H_

#include <gta/config.h>
#include <ui_RWBSSectionDialog.h>


class RWBSSectionDialog : public QDialog {
	Q_OBJECT

public:
	RWBSSectionDialog(QWidget* parent = NULL);
	int32_t getID() const;
	bool isContainerSection() const { return ui.containerBox->isChecked(); }
	int32_t getVersion() const;
	void setID(int32_t id);
	void setContainerSection(bool cs) { ui.containerBox->setChecked(cs); }
	void setVersion(int32_t version);

private slots:
	void idBoxChanged(int idx);
	void versionBoxChanged(int idx);

private:
	Ui_RWBSSectionDialog ui;
};

#endif /* RWBSSECTIONDIALOG_H_ */
