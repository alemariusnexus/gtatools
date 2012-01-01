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

#ifndef HEXEDITOR_H_
#define HEXEDITOR_H_

#include "HexEditorPrivate.h"
#include <QtGui/QAbstractScrollArea>
#include <QtGui/QScrollBar>
#include <QtCore/QTimer>


class HexEditor : public QAbstractScrollArea {
	Q_OBJECT

public:
	HexEditor(QWidget* parent = NULL);

	void setData(const QByteArray& data) { editor->setData(data); }
	void loadData(const File& file) { editor->loadData(file); }
	int getDataSize() const { return editor->getDataSize(); }
	QByteArray getData() const { return editor->getData(); }
	QByteArray getSelectedData() const { return editor->getSelectedData(); }

	void setAddressHexGapSize(int size) { editor->setAddressHexGapSize(size); }
	void setHexASCIIGapSize(int size) { editor->setHexASCIIGapSize(size); }
	void setAddressCharCount(int count) { editor->setAddressCharCount(count); }
	int getAddressHexGapSize() const { return editor->getAddressHexGapSize(); }
	int getHexASCIIGapSize() const { return editor->getHexASCIIGapSize(); }
	int getAddressCharCount() const { return editor->getAddressCharCount(); }

	int getNumberOfLines() const { return editor->getNumberOfLines(); }
	int getBytesPerLine() const { return editor->getBytesPerLine(); }
	int getVisibleLineCount() const { return editor->getVisibleLineCount(); }
	int getFullyVisibleLineCount() const { return editor->getFullyVisibleLineCount(); }

	virtual void setFont(const QFont& font) { QAbstractScrollArea::setFont(font); editor->setFont(font); }
	void setAddressColor(const QColor& color) { editor->setAddressColor(color); }
	QColor getAddressColor() const { return editor->getAddressColor(); }
	void setEvenColumnColor(const QColor& color) { editor->setEvenColumnColor(color); }
	QColor getEvenColumnColor() const { return editor->getEvenColumnColor(); }
	void setOddColumnColor(const QColor& color) { editor->setOddColumnColor(color); }
	QColor getOddColumnColor() const { return editor->getOddColumnColor(); }

	int getLineDisplayOffset() const { return editor->getLineDisplayOffset(); }

	void setCursor(int cpos, int anchor) { editor->setCursor(cpos, anchor); }
	void setCursorPosition(int cpos) { editor->setCursorPosition(cpos); }
	int getCursorPosition() const { return editor->getCursorPosition(); }
	int getCursorAnchor() const { return editor->getCursorAnchor(); }

	void setOverwriteMode(bool ovw) { editor->setOverwriteMode(ovw); }
	bool isOverwriteMode() const { return editor->isOverwriteMode(); }

	bool isAddressAreaEnabled() const { return editor->isAddressAreaEnabled(); }
	bool isHexAreaEnabled() const { return editor->isHexAreaEnabled(); }
	bool isASCIIAreaEnabled() const { return editor->isASCIIAreaEnabled(); }
	void setAddressAreaEnabled(bool enabled) { editor->setAddressAreaEnabled(enabled); }
	void setHexAreaEnabled(bool enabled) { editor->setHexAreaEnabled(enabled); }
	void setASCIIAreaEnabled(bool enabled) { editor->setASCIIAreaEnabled(enabled); }

	int calculateOffset(int x, int y) const { return editor->calculateOffset(x, y); }
	int calculateOffset(const QPoint& pos) const { return editor->calculateOffset(pos); }
	int calculateLineStartOffset(int line) { return editor->calculateLineStartOffset(line); }
	int calculateLineEndOffset(int line) { return editor->calculateLineEndOffset(line); }

	void insert(int offset, const QByteArray& data) { editor->insert(offset, data); }
	void insert(int offset, char c) { editor->insert(offset, c); }
	void insert(const QByteArray& data) { editor->insert(data); }
	void remove(int offset, int len) { editor->remove(offset, len); }
	void replace(int offset, int len, const QByteArray& data) { editor->replace(offset, len, data); }
	void replace(int offset, const QByteArray& data) { editor->replace(offset, data); }
	void replace(int offset, char c) { editor->replace(offset, c); }
	void prepend(const QByteArray& data) { insert(0, data); }
	void append(const QByteArray& data) { insert(data.size(), data); }

	void ensureLineVisible(int line);
	void ensureVisible(int offset) { ensureLineVisible(offset / getBytesPerLine()); }

public slots:
	void replace(const QByteArray& data) { editor->replace(data); }

signals:
	void dataChanged(const QByteArray& data);
	void cursorChanged(int pos, int anchor);

protected:
	virtual void paintEvent(QPaintEvent* evt);
	virtual void mousePressEvent(QMouseEvent* evt);
	virtual void mouseReleaseEvent(QMouseEvent* evt);
	virtual void mouseMoveEvent(QMouseEvent* evt);
	virtual void keyPressEvent(QKeyEvent* evt);
	virtual void keyReleaseEvent(QKeyEvent* evt);
	virtual void resizeEvent(QResizeEvent* evt);

private slots:
	void contentLineCountChanged(int lines);
	void verticalScrollBarChanged(int value);
	void cursorChangedSlot(int pos, int anchor);
	void scroll();

public:
	HexEditorPrivate* editor;
	QTimer scrollTimer;
	int scrollFactor;
};

#endif /* HEXEDITOR_H_ */
