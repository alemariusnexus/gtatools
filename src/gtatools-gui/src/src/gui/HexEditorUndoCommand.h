/*
	Copyright 2010-2014 David "Alemarius Nexus" Lerch

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

#ifndef HEXEDITORUNDOCOMMAND_H_
#define HEXEDITORUNDOCOMMAND_H_

#include <QtCore/QObject>
#include <QUndoCommand>
#include "HexEditor.h"


class HexEditorUndoCommand : public QObject, public QUndoCommand
{
	Q_OBJECT

public:
	HexEditorUndoCommand(HexEditor* editor, int offset, int len, const QByteArray& oldData, const QByteArray& newData,
			bool alreadyDone);

	virtual void undo();
	virtual void redo();

private:
	HexEditor* editor;
	int offset;
	int len;
	QByteArray oldData;
	QByteArray newData;
	bool alreadyDone;
};

#endif /* HEXEDITORUNDOCOMMAND_H_ */
