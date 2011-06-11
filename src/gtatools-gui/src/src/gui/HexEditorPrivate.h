/*
 * HexEditor.h
 *
 *  Created on: 01.06.2011
 *      Author: alemariusnexus
 */

#ifndef HEXEDITORPRIVATE_H_
#define HEXEDITORPRIVATE_H_

#include <QtCore/QTimer>
#include <QtGui/QWidget>
#include <QtGui/QFontMetrics>
#include <gtaformats/util/File.h>
#include <cmath>


class HexEditorPrivate : public QWidget {
	Q_OBJECT

public:
	enum EditorParts
	{
		AddressArea = (1 << 0),
		HexArea = (1 << 1),
		ASCIIArea = (1 << 2),

		DefaultParts = AddressArea | HexArea | ASCIIArea
	};

public:
	HexEditorPrivate(QWidget* parent = NULL);

	void setData(const QByteArray& data)
			{ this->data = data; recomputeGeometry(); update(); setCursorPosition(0); emit dataChanged(data); }
	void loadData(const File& file);
	int getDataSize() const { return data.size(); }
	QByteArray getData() const { return data; }
	QByteArray getSelectedData() const;

	void setAddressHexGapSize(int size) { addrHexGapSize = size; }
	void setHexASCIIGapSize(int size) { hexAsciiGapSize = size; }
	void setAddressCharCount(int count) { numAddressChars = count; }
	int getAddressHexGapSize() const { return addrHexGapSize; }
	int getHexASCIIGapSize() const { return hexAsciiGapSize; }
	int getAddressCharCount() const { return numAddressChars; }

	int getNumberOfLines() const { return numLines; }
	int getLineHeight() const { return charH; }
	int getBytesPerLine() const { return bytesPerLine; }
	int getVisibleLineCount() const { return (int) ceil((float) height() / charH); }
	int getFullyVisibleLineCount() const { return height() / charH; }

	virtual void setFont(const QFont& font);
	void setAddressColor(const QColor& color) { addrColor = color; }
	QColor getAddressColor() const { return addrColor; }
	void setEvenColumnColor(const QColor& color) { evenColColor = color; }
	QColor getEvenColumnColor() const { return evenColColor; }
	void setOddColumnColor(const QColor& color) { oddColColor = color; }
	QColor getOddColumnColor() const { return oddColColor; }

	void setLineDisplayOffset(int offs) { lineDisplayOffset = offs; update(); }
	int getLineDisplayOffset() const { return lineDisplayOffset; }

	void setCursor(int cpos, int anchor);
	void setCursorPosition(int cpos) { setCursor(cpos, cpos); }
	int getCursorPosition() const { return cursorPos; }
	int getCursorAnchor() const { return cursorAnchor; }

	void setOverwriteMode(bool ovw) { overwriteMode = ovw; updateCursorBlink(); }
	bool isOverwriteMode() const { return overwriteMode; }

	bool isAddressAreaEnabled() const { return (enabledParts & AddressArea) != 0; }
	bool isHexAreaEnabled() const { return (enabledParts & HexArea) != 0; }
	bool isASCIIAreaEnabled() const { return (enabledParts & ASCIIArea) != 0; }
	void setAddressAreaEnabled(bool enabled)
			{ if (enabled) enabledParts |= AddressArea; else enabledParts &= ~AddressArea; }
	void setHexAreaEnabled(bool enabled)
			{ if (enabled) enabledParts |= HexArea; else enabledParts &= ~HexArea; }
	void setASCIIAreaEnabled(bool enabled)
			{ if (enabled) enabledParts |= ASCIIArea; else enabledParts &= ~ASCIIArea; }

	int calculateOffset(int x, int y) const;
	int calculateOffset(const QPoint& pos) const { return calculateOffset(pos.x(), pos.y()); }
	int calculateArea(int x, int y);
	int calculateArea(const QPoint& pos) { return calculateArea(pos.x(), pos.y()); }
	QPoint calculateHexPosition(int offset);
	QPoint calculateASCIIPosition(int offset);
	int calculateLineStartOffset(int line);
	int calculateLineEndOffset(int line);

	void updateRange(int offset, int len);
	void updateLines(int first, int last);

	void insert(int offset, const QByteArray& data);
	void insert(int offset, char c) { insert(offset, QByteArray(1, c)); }
	void insert(const QByteArray& data);
	void remove(int offset, int len);
	void replace(int offset, int len, const QByteArray& data);
	void replace(int offset, const QByteArray& data) { replace(offset, data.size(), data); }
	void replace(int offset, char c) { replace(offset, QByteArray(1, c)); }
	void prepend(const QByteArray& data) { insert(0, data); }
	void append(const QByteArray& data) { insert(this->data.size(), data); }

public slots:
	void replace(const QByteArray& data) { replace(cursorPos, data); }

protected:
	virtual void paintEvent(QPaintEvent* evt);
	virtual void mousePressEvent(QMouseEvent* evt);
	virtual void mouseMoveEvent(QMouseEvent* evt);
	virtual void keyPressEvent(QKeyEvent* evt);
	virtual void resizeEvent(QResizeEvent* evt);

signals:
	void contentLineCountChanged(int lines);
	void dataChanged(const QByteArray& data);
	void cursorChanged(int pos, int anchor);

private slots:
	void updateCursorBlink();

private:
	void updateContentLineCount(int lines);
	void recomputeGeometry();
	void recomputeVerticalGeometry();

private:
	int enabledParts;
	QTimer cursorBlinkTimer;
	float charW, charH, charAsc, charDesc;
	QByteArray data;
	int addrHexGapSize;
	int hexAsciiGapSize;
	int numAddressChars;
	int cursorPos;
	int cursorAnchor;
	bool overwriteMode;
	bool temporaryOverwriteMode;
	bool cursorBlink;
	bool cursorAscii;
	int numLines;
	int bytesPerLine;
	int lineDisplayOffset;
	QColor addrColor;
	QColor evenColColor, oddColColor;
	bool currentByteEdited;
	int mouseSelStart;

	int hexWidth, asciiWidth;
	int addrStartX, hexStartX, asciiStartX;

private:
	friend class HexEditor;
};

#endif /* HEXEDITORPRIVATE_H_ */
