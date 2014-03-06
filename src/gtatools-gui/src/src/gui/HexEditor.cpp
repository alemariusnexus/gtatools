/*
	Copyright 2010-2013 David "Alemarius Nexus" Lerch

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

#include "HexEditor.h"
#include "HexEditorUndoCommand.h"
#include <QtGui/QScrollBar>
#include <QtGui/QPaintEvent>
#include <algorithm>

using std::min;
using std::max;



HexEditor::HexEditor(QWidget* parent)
		: QAbstractScrollArea(parent), editor(new HexEditorPrivate(this)), scrollFactor(0), undoStack(NULL),
		  undoDecorator(NULL)
{
	setUndoDecorator(NULL);

	setViewport(editor);
	connect(editor, SIGNAL(dataReplaced(int, int, const QByteArray&, const QByteArray&)), this,
			SIGNAL(dataReplaced(int, int, const QByteArray&, const QByteArray&)));
	connect(editor, SIGNAL(dataChanged(const QByteArray&)), this, SIGNAL(dataChanged(const QByteArray&)));
	connect(editor, SIGNAL(cursorChanged(int, int)), this, SIGNAL(cursorChanged(int, int)));
	connect(editor, SIGNAL(contentLineCountChanged(int)), this, SLOT(contentLineCountChanged(int)));
	connect(editor, SIGNAL(cursorChanged(int, int)), this, SLOT(cursorChangedSlot(int, int)));
	connect(verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(verticalScrollBarChanged(int)));
	connect(&scrollTimer, SIGNAL(timeout()), this, SLOT(scroll()));

	connect(this, SIGNAL(dataReplaced(int, int, const QByteArray&, const QByteArray&)), this,
			SLOT(dataReplacedSlot(int, int, const QByteArray&, const QByteArray&)));

	scrollTimer.start(50);
}


void HexEditor::ensureLineVisible(int line)
{
	int firstLine = getLineDisplayOffset();
	int visCount = getFullyVisibleLineCount();

	if (line >= firstLine+visCount) {
		// line is after the visible range
		verticalScrollBar()->setValue(min(line-visCount+1, getNumberOfLines()-1));
	} else if (line < firstLine) {
		// Line is before the visible range
		verticalScrollBar()->setValue(line);
	}
}


void HexEditor::contentLineCountChanged(int lines)
{
	QScrollBar* vbar = verticalScrollBar();
	vbar->setMaximum(lines-1);
}


void HexEditor::verticalScrollBarChanged(int value)
{
	editor->setLineDisplayOffset(value);
}


void HexEditor::cursorChangedSlot(int pos, int anchor)
{
	if (scrollFactor == 0)
		ensureLineVisible(pos / getBytesPerLine());
}


void HexEditor::scroll()
{
	if (scrollFactor != 0) {
		QScrollBar* vbar = verticalScrollBar();
		int value = max(min(vbar->value()+scrollFactor, vbar->maximum()), 0);
		vbar->setValue(value);
	}
}


void HexEditor::paintEvent(QPaintEvent* evt)
{
	editor->paintEvent(evt);
}


void HexEditor::mousePressEvent(QMouseEvent* evt)
{

	editor->mousePressEvent(evt);
}


void HexEditor::mouseReleaseEvent(QMouseEvent* evt)
{
	scrollFactor = 0;
}


void HexEditor::mouseMoveEvent(QMouseEvent* evt)
{
	editor->mouseMoveEvent(evt);

	if (evt->y() >= editor->height()) {
		scrollFactor = 1 + min((evt->y() - editor->height()) / 10, 5);
	} else if (evt->y() < 0) {
		scrollFactor = -1 + max(evt->y() / 10, -5);
	} else {
		scrollFactor = 0;
	}
}


void HexEditor::keyPressEvent(QKeyEvent* evt)
{
	editor->keyPressEvent(evt);
}


void HexEditor::keyReleaseEvent(QKeyEvent* evt)
{
}


void HexEditor::resizeEvent(QResizeEvent* evt)
{
	editor->resizeEvent(evt);
}


void HexEditor::dataReplacedSlot(int offset, int len, const QByteArray& oldData, const QByteArray& newData)
{
	HexEditorUndoCommand* cmd = new HexEditorUndoCommand(this, offset, len, oldData, newData, true);
	undoDecorator->push(cmd);
}


void HexEditor::setUndoDecorator(UndoDecorator* decorator)
{
	if (!decorator)
		decorator = &nullUndoDecorator;

	if (undoDecorator)
		disconnect(undoDecorator, SIGNAL(committed(QUndoCommand*)), this, SLOT(undoDecoratorCommitted(QUndoCommand*)));

	undoDecorator = decorator;

	connect(decorator, SIGNAL(committed(QUndoCommand*)), this, SLOT(undoDecoratorCommitted(QUndoCommand*)));
}


void HexEditor::undoDecoratorCommitted(QUndoCommand* cmd)
{
	if (undoStack) {
		undoStack->push(cmd);
	} else {
		cmd->redo();
		delete cmd;
	}
}
