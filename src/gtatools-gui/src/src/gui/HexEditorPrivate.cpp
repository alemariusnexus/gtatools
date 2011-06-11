/*
	Copyright 2010-2011 David "Alemarius Nexus" Lerch

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

#include "HexEditorPrivate.h"
#include <QtGui/QPainter>
#include <QtGui/QPaintEvent>
#include <QtGui/QApplication>
#include <QtGui/QClipboard>
#include <QtCore/QByteArray>
#include <QtCore/QFile>
#include <cstdio>
#include <cmath>
#include <algorithm>

using std::min;
using std::max;



HexEditorPrivate::HexEditorPrivate(QWidget* parent)
		: QWidget(parent), enabledParts(DefaultParts), data(QByteArray()), addrHexGapSize(20),
		  hexAsciiGapSize(20), numAddressChars(8), cursorPos(0), cursorAnchor(0), overwriteMode(true),
		  temporaryOverwriteMode(false), cursorBlink(false), cursorAscii(false), numLines(0), bytesPerLine(0),
		  lineDisplayOffset(0), currentByteEdited(false)
{
	setFont(QFont("Mono", 12));
	setAddressColor(QColor(Qt::lightGray).lighter(100));
	setEvenColumnColor(palette().color(QPalette::Text));
	setOddColumnColor(QColor(Qt::blue).lighter(125));

	connect(&cursorBlinkTimer, SIGNAL(timeout()), this, SLOT(updateCursorBlink()));
	cursorBlinkTimer.start(500);
}


void HexEditorPrivate::loadData(const File& file)
{
	QFile qfile(file.getPath()->toString());
	qfile.open(QFile::ReadOnly);
	data = qfile.readAll();
	qfile.close();
	recomputeGeometry();
	update();
	setCursorPosition(0);
	emit dataChanged(data);
}


void HexEditorPrivate::updateContentLineCount(int num)
{
	numLines = num;
	emit contentLineCountChanged(num);
}


void HexEditorPrivate::setFont(const QFont& font)
{
	QWidget::setFont(font);

	QFontMetrics fm(font);

	charW = fm.width('F');
	charH = fm.height();
	charAsc = fm.ascent();
	charDesc = fm.descent();

	update();
}


void HexEditorPrivate::updateCursorBlink()
{
	if (bytesPerLine != 0) {
		cursorBlink = !cursorBlink;

		int cursorLine = cursorPos / bytesPerLine;

		int startLine = lineDisplayOffset;
		int endLine = startLine + ceil(height() / charH);

		if (cursorLine >= startLine  &&  cursorLine <= endLine) {
			int yPos = (cursorLine - startLine) * charH;
			update(0, yPos, width(), charH*2);
		}
	}
}


void HexEditorPrivate::setCursor(int cpos, int anchor)
{
	if (cpos < 0) {
		cpos = 0;
	} else if (cpos > data.size()) {
		cpos = data.size();
	}

	if (anchor < 0) {
		anchor = 0;
	} else if (anchor > data.size()) {
		anchor = data.size();
	}

	// Erase the old cursor
	cursorBlink = true;
	updateCursorBlink();

	int oldPos = cursorPos;
	int oldAnchor = cursorAnchor;

	cursorPos = cpos;
	cursorAnchor = anchor;

	int oldStart = min(oldPos, oldAnchor);
	int oldEnd = max(oldPos, oldAnchor);

	updateRange(oldStart, oldEnd-oldStart);

	if (cursorPos != cursorAnchor) {
		int start = min(cursorPos, cursorAnchor);
		int end = max(cursorPos, cursorAnchor);
		updateRange(start, end-start);
	}

	updateCursorBlink();

	currentByteEdited = false;

	if (temporaryOverwriteMode)
		setOverwriteMode(false);

	emit cursorChanged(cursorPos, cursorAnchor);
}


int HexEditorPrivate::calculateOffset(int x, int y) const
{
	int line = (y / charH) + lineDisplayOffset;
	int col;

	if (isASCIIAreaEnabled()  &&  x > asciiStartX) {
		// A click in the ASCII area
		col = (x - asciiStartX) / charW;
	} else if (isASCIIAreaEnabled()  &&  x > asciiStartX - hexAsciiGapSize/2) {
		// A click on the hex-ASCII gap on the ASCII side. Assume the first byte in the line.
		col = 0;
	} else if (isHexAreaEnabled()  &&  x > hexStartX) {
		// A click in the hex area
		col = ((x - hexStartX) / charW + 1) / 3;
	} else {
		// A click in the address area. Assume the first byte in the line.
		col = 0;
	}

	int lastLineBytes = data.size() % bytesPerLine;

	// Check that the line and column are valid.
	if (	(line < numLines  ||  (line == numLines  &&  col == 0))
			&&  col <= bytesPerLine
			&&  (line != numLines-1  ||  lastLineBytes == 0  ||  col <= lastLineBytes)
	) {
		return line*bytesPerLine + col;
	}

	return -1;
}


int HexEditorPrivate::calculateArea(int x, int y)
{
	if (isASCIIAreaEnabled()  &&  x > asciiStartX) {
		return ASCIIArea;
	} else if (isASCIIAreaEnabled()  &&  x > asciiStartX - hexAsciiGapSize/2) {
		return ASCIIArea;
	} else if (isHexAreaEnabled()  &&  x > hexStartX) {
		return HexArea;
	} else {
		return AddressArea;
	}
}


QPoint HexEditorPrivate::calculateHexPosition(int offset)
{
	if (!isHexAreaEnabled())
		return QPoint(-1, -1);

	int line = offset / bytesPerLine;

	int yPos = (line - lineDisplayOffset) * charH;
	int xPos = hexStartX + (offset % bytesPerLine) * 3 * charW;

	return QPoint(xPos, yPos);
}


QPoint HexEditorPrivate::calculateASCIIPosition(int offset)
{
	if (!isASCIIAreaEnabled())
		return QPoint(-1, -1);

	int line = offset / bytesPerLine;

	int yPos = (line - lineDisplayOffset) * charH;
	int xPos = asciiStartX + (offset % bytesPerLine) * charW;

	return QPoint(xPos, yPos);
}


int HexEditorPrivate::calculateLineStartOffset(int line)
{
	return line * bytesPerLine;
}


int HexEditorPrivate::calculateLineEndOffset(int line)
{
	int offset = line*bytesPerLine;

	if (line >= numLines-1) {
		if (line == numLines-1) {
			offset += bytesPerLine-1;
		} else {
			offset += (data.size() % bytesPerLine);
		}
	} else {
		offset += bytesPerLine-1;
	}

	return offset;
}


void HexEditorPrivate::updateLines(int first, int last)
{
	if (first < lineDisplayOffset)
		first = lineDisplayOffset;

	int lastVisibleLine = lineDisplayOffset + ceil((float) height() / charH) + 1;

	if (last > lastVisibleLine) {
		last = lastVisibleLine;
	}

	update(0, (first - lineDisplayOffset) * charH, width(), (last - first + 2) * charH);
}


void HexEditorPrivate::updateRange(int offset, int len)
{
	updateLines(offset / bytesPerLine, (offset+len) / bytesPerLine);
}


void HexEditorPrivate::insert(int offset, const QByteArray& data)
{
	this->data.insert(offset, data);
	recomputeVerticalGeometry();
	update();
	emit dataChanged(data);
}


void HexEditorPrivate::insert(const QByteArray& data)
{
	int start = min(cursorPos, cursorAnchor);
	int end = max(cursorPos, cursorAnchor);
	replace(start, end-start, data);
	setCursorPosition(start + data.size());
}


void HexEditorPrivate::remove(int offset, int len)
{
	data.remove(offset, len);
	recomputeVerticalGeometry();
	update();
	emit dataChanged(data);
}


void HexEditorPrivate::replace(int offset, int len, const QByteArray& data)
{
	this->data.replace(offset, len, data);

	if (len == data.size()) {
		// No characters were added or removed, so we may just update the lines which were changed.
		if (bytesPerLine != 0) {
			int startLine = offset / bytesPerLine;
			int endLine = (offset+len) / bytesPerLine;

			updateLines(startLine, endLine);
		}
	} else {
		// Characters were added or removed, so we have to do a full update.
		recomputeVerticalGeometry();
		update();
	}

	emit dataChanged(data);
}


QByteArray HexEditorPrivate::getSelectedData() const
{
	int start = min(cursorPos, cursorAnchor);
	int end = max(cursorPos, cursorAnchor);
	return data.mid(start, end-start);
}


void HexEditorPrivate::mousePressEvent(QMouseEvent* evt)
{
	int offs = calculateOffset(evt->pos());

	if (offs != -1) {
		setCursorPosition(offs);

		if (calculateArea(evt->pos()) == ASCIIArea) {
			cursorAscii = true;
		} else {
			cursorAscii = false;
		}
	}
}


void HexEditorPrivate::mouseMoveEvent(QMouseEvent* evt)
{
	int x = min(evt->x(), width());
	int y = min(evt->y(), height());
	int offs = calculateOffset(x, y);
	setCursor(offs, cursorAnchor);
}


void HexEditorPrivate::keyPressEvent(QKeyEvent* evt)
{
	char c = evt->text()[0].toAscii();

	if (cursorAscii  &&  c >= 0x20  &&  c <= 0x7E) {
		// Insert or replace an ASCII character.

		if (cursorPos != cursorAnchor) {
			int start = min(cursorPos, cursorAnchor);
			int end = max(cursorPos, cursorAnchor);

			if (overwriteMode)
				replace(start, end-start, QByteArray(1, 0x00));
			else
				remove(start, end-start);

			setCursorPosition(start);
		}

		if (overwriteMode) {
			replace(cursorPos, c);
		} else {
			insert(cursorPos, c);
		}

		setCursorPosition(cursorPos+1);

		return;
	} else if (!cursorAscii  &&  ((c >= '0'  &&  c <= '9')  ||  (c >= 'a'  &&  c <= 'f'))) {
		// Insert or replace a hex-character

		if (cursorPos != cursorAnchor) {
			int start = min(cursorPos, cursorAnchor);
			int end = max(cursorPos, cursorAnchor);

			if (overwriteMode)
				replace(start, end-start, QByteArray(1, 0x00));
			else
				remove(start, end-start);

			setCursorPosition(start);
		}

		if (overwriteMode) {
			QByteArray hex = data.mid(cursorPos, 1).toHex();

			// If a byte shall be overwritten, the first key pressed will change the lower 4 bits. If then
			// another hex-key is hit, the previously entered 4 bits will become the upper half, and the new
			// key becomes the lower 4 bits. When the first key is hit, we enter a state (currentByteEdited
			// = true) which will be left when the cursor changes or the second key is pressed.
			if (cursorPos == data.size()) {
				append(QByteArray::fromHex(QByteArray("0").append(c)));
				currentByteEdited = true;
			} else {
				if (currentByteEdited) {
					char old = hex[1];
					hex[1] = c;
					hex[0] = old;
					currentByteEdited = false;

					if (temporaryOverwriteMode) {
						setOverwriteMode(false);
						temporaryOverwriteMode = false;
					}
				} else {
					// Overwrite the lower 4 bits.
					hex[1] = c;
					currentByteEdited = true;
				}

				replace(cursorPos, QByteArray::fromHex(hex));
			}

			if (!currentByteEdited)
				setCursorPosition(cursorPos+1);
		} else {
			QByteArray hex;
			hex.append('0').append(c);
			insert(cursorPos, QByteArray::fromHex(hex));
			temporaryOverwriteMode = true;
			currentByteEdited = true;
			setOverwriteMode(true);
		}

		return;
	}

	if (evt->matches(QKeySequence::Delete)) {
		if (cursorPos == cursorAnchor) {
			remove(cursorPos, 1);
		} else {
			int start = min(cursorPos, cursorAnchor);
			int end = max(cursorPos, cursorAnchor);
			remove(start, end-start);
			setCursorPosition(min(cursorPos, cursorAnchor));
		}
	} else if (evt->key() == Qt::Key_Backspace) {
		if (cursorPos == cursorAnchor) {
			remove(cursorPos-1, 1);
		} else {
			int start = min(cursorPos, cursorAnchor);
			int end = max(cursorPos, cursorAnchor);
			remove(start, end-start);
		}
		setCursorPosition(min(cursorPos, cursorAnchor) - 1);
	} else if (evt->key() == Qt::Key_Insert) {
		setOverwriteMode(!overwriteMode);
	} else if (evt->matches(QKeySequence::MoveToEndOfDocument)) {
		setCursorPosition(data.size());
	} else if (evt->matches(QKeySequence::MoveToEndOfLine)) {
		setCursorPosition(calculateLineEndOffset(cursorPos / bytesPerLine));
	} else if (evt->matches(QKeySequence::MoveToNextChar)) {
		setCursorPosition(cursorPos+1);
	} else if (evt->matches(QKeySequence::MoveToNextLine)) {
		setCursorPosition(calculateLineStartOffset(cursorPos / bytesPerLine + 1) + (cursorPos % bytesPerLine));
	} else if (evt->matches(QKeySequence::MoveToNextPage)) {
		setCursorPosition(cursorPos + getVisibleLineCount()*bytesPerLine);
	} else if (evt->matches(QKeySequence::MoveToPreviousChar)) {
		setCursorPosition(cursorPos-1);
	} else if (evt->matches(QKeySequence::MoveToPreviousLine)) {
		setCursorPosition(calculateLineStartOffset(cursorPos / bytesPerLine - 1) + (cursorPos % bytesPerLine));
	} else if (evt->matches(QKeySequence::MoveToPreviousPage)) {
		setCursorPosition(cursorPos - getVisibleLineCount()*bytesPerLine);
	} else if (evt->matches(QKeySequence::MoveToStartOfDocument)) {
		setCursorPosition(0);
	} else if (evt->matches(QKeySequence::MoveToStartOfLine)) {
		setCursorPosition(calculateLineStartOffset(cursorPos / bytesPerLine));
	} else if (evt->matches(QKeySequence::SelectAll)) {
		setCursor(0, data.size());
	} else if (evt->matches(QKeySequence::SelectEndOfDocument)) {
		setCursor(data.size(), cursorAnchor);
	} else if (evt->matches(QKeySequence::SelectEndOfLine)) {
		setCursor(calculateLineEndOffset(cursorPos / bytesPerLine)+1, cursorAnchor);
	} else if (evt->matches(QKeySequence::SelectNextChar)) {
		setCursor(cursorPos+1, cursorAnchor);
	} else if (evt->matches(QKeySequence::SelectNextLine)) {
		setCursor(calculateLineStartOffset(cursorPos / bytesPerLine + 1) + (cursorPos % bytesPerLine),
				cursorAnchor);
	} else if (evt->matches(QKeySequence::SelectNextPage)) {
		setCursor(cursorPos + getVisibleLineCount()*bytesPerLine, cursorAnchor);
	} else if (evt->matches(QKeySequence::SelectPreviousChar)) {
		setCursor(cursorPos-1, cursorAnchor);
	} else if (evt->matches(QKeySequence::SelectPreviousLine)) {
		setCursor(calculateLineStartOffset(cursorPos / bytesPerLine - 1) + (cursorPos % bytesPerLine),
				cursorAnchor);
	} else if (evt->matches(QKeySequence::SelectPreviousPage)) {
		setCursor(cursorPos - getVisibleLineCount()*bytesPerLine, cursorAnchor);
	} else if (evt->matches(QKeySequence::SelectStartOfDocument)) {
		setCursor(0, cursorAnchor);
	} else if (evt->matches(QKeySequence::SelectStartOfLine)) {
		setCursor(calculateLineStartOffset(cursorPos / bytesPerLine), cursorAnchor);
	} else if (evt->matches(QKeySequence::Copy)  ||  evt->matches(QKeySequence::Cut)) {
		QByteArray sel = getSelectedData();
		QString selText;

		for (int i = 0 ; i < sel.size() ; i++) {
			if (i != 0) {
				selText.append(' ');
			}

			selText.append(QString(sel.mid(i, 1).toHex().toUpper()).toAscii().constData());
		}

		QClipboard* cb = QApplication::clipboard();
		cb->setText(selText);

		if (evt->matches(QKeySequence::Cut)) {
			int start = min(cursorPos, cursorAnchor);
			int end = max(cursorPos, cursorAnchor);
			remove(start, end-start);
			setCursorPosition(start);
		}
	} else if (evt->matches(QKeySequence::Paste)) {
		QClipboard* cb = QApplication::clipboard();
		QString text = cb->text();
		QByteArray data = QByteArray::fromHex(text.toAscii());
		insert(data);
	}
}


void HexEditorPrivate::recomputeGeometry()
{
	int w = width();
	int h = height();

	int hexAsciiGapSize = (isHexAreaEnabled()  &&  isASCIIAreaEnabled()) ? this->hexAsciiGapSize : 0;
	int addrHexGapSize = (isAddressAreaEnabled()  &&  (isHexAreaEnabled()   ||  isASCIIAreaEnabled()))
			?  this->addrHexGapSize : 0;
	int numAddressChars = isAddressAreaEnabled() ? this->numAddressChars : 0;

	// Calculate widths of the areas
	int lineW = (w - hexAsciiGapSize - addrHexGapSize - numAddressChars*charW) / charW; // Width in characters

	if (isHexAreaEnabled()  &&  isASCIIAreaEnabled()) {
		bytesPerLine = max((lineW+1) / 4, 1); // In bytes. We need at least 1 byte per line
	} else if (isHexAreaEnabled()) {
		bytesPerLine = max((lineW+1) / 3, 1); // In bytes. We need at least 1 byte per line
	} else {
		bytesPerLine = lineW;
	}

	hexWidth = isHexAreaEnabled() ? bytesPerLine*3 - 1 : 0; // Hex area width in characters
	asciiWidth = isASCIIAreaEnabled() ? bytesPerLine : 0; // ASCII area width in characters

	recomputeVerticalGeometry();

	// Start positions of the areas. This time it's in pixels!
	addrStartX = 0;
	hexStartX = addrStartX + numAddressChars*charW + addrHexGapSize;
	asciiStartX = hexStartX + hexWidth*charW + hexAsciiGapSize;
}


void HexEditorPrivate::recomputeVerticalGeometry()
{
	// Calculate the number of lines. If the content is empty, we still assume to have one line.
	// Yes, we really need double precision here!
	numLines = max((int) ceil((double) data.size() / bytesPerLine), 1);

	updateContentLineCount(numLines);
}


void HexEditorPrivate::resizeEvent(QResizeEvent* evt)
{
	// Resize
	recomputeGeometry();
}


void HexEditorPrivate::paintEvent(QPaintEvent* evt)
{
	const QRect& rect = evt->rect();

	QPainter painter(this);
	int w = width();
	int h = height();

	// Fill the background. The address area background is overwritten later.
	painter.fillRect(rect, palette().color(QPalette::Base));

	// Calculate the start and end lines to be redrawn. These are really just the lines to be redrawn, and
	// NOT the entire visible line range!
	// Theses values are counted from zero!
	// Note: When the very last line is to be redrawn and if it is completely full, we will append an empty
	// line, so that the cursor can be placed there to add new values (?: operator in second min() argument).
	int startLine = rect.y() / charH + lineDisplayOffset;
	int endLine = min((rect.y() + rect.height()) / charH + 1 + lineDisplayOffset,
			(data.size() % bytesPerLine) == 0 ? numLines : numLines-1);

	if (data.size() == 0)
		endLine = startLine;

	// Fill the background of the address area
	if (isAddressAreaEnabled()) {
		painter.fillRect(addrStartX, rect.y(),
				hexStartX - addrHexGapSize/2, rect.height(),
				addrColor);

		// Paint the addresses
		for (int line = startLine ; line <= endLine ; line++) {
			int yPos = (line-lineDisplayOffset) * charH + charAsc;
			QString addr = QString("%1").arg(line*bytesPerLine, numAddressChars, 16, QChar('0'));
			painter.drawText(QPoint(addrStartX, yPos), addr.toUpper());
		}
	}

	if (isAddressAreaEnabled()  &&  (isHexAreaEnabled()  ||  isASCIIAreaEnabled())) {
		// Paint the separator between addresses and hex area
		painter.drawLine(QLine(numAddressChars*charW + addrHexGapSize/2, rect.y(),
				numAddressChars*charW + addrHexGapSize/2, rect.y() + rect.height()));
	}

	QBrush selBrush(QColor(Qt::blue).lighter(180));
	QBrush defBrush = painter.background();

	QPen evenColPen(evenColColor);
	QPen oddColPen(oddColColor);

	// This algorithm was originally written to work with a 'start-length' range of the selection.
	int selStart = min(cursorPos, cursorAnchor);
	int selLen = max(cursorPos, cursorAnchor) - selStart;

	if (selLen == 0) {
		selStart = -1;
	}

	QPen origPen = painter.pen();

	if (isHexAreaEnabled()) {
		// Paint the hex area
		for (int line = startLine ; line <= endLine ; line++) {
			int yPos = (line-lineDisplayOffset) * charH + charAsc;
			int nextSize = 2*charW;

			// Paint a single hex line
			for (int col = 0 ; col < bytesPerLine  &&  line*bytesPerLine + col < data.size() ; col++) {
				int xPos = hexStartX + col*3*charW - charW;
				int pos = line*bytesPerLine + col;
				bool spaceDrawn = false;

				if (col%2 == 0) {
					painter.setPen(evenColPen);
				} else {
					painter.setPen(oddColPen);
				}

				if (selStart != -1  &&  pos >= selStart  &&  pos < selStart+selLen) {
					// This character is inside the current selection.
					if (pos == selStart  &&  col != 0) {
						// If this is the first byte of the selection AND if it is not the first byte in the line,
						// we draw the preceding space without highlighting background, because the selection
						// shall not include a space on the front.
						painter.drawText(QPoint(xPos, yPos), " ");
						xPos += charW;
						spaceDrawn = true;
					}

					painter.setBackground(selBrush);
					painter.setBackgroundMode(Qt::OpaqueMode);
				} else {
					// This character is outside the current selection.
					painter.setBackground(defBrush);
					painter.setBackgroundMode(Qt::TransparentMode);
				}

				if (col == 0) {
					QByteArray hex = data.mid(line*bytesPerLine + col, 1).toHex().toUpper();
					painter.drawText(QPoint(xPos + charW, yPos), hex);
					nextSize += charW;
				} else {
					// Every byte but the first one in the line gets a preceding space.
					QByteArray hex = data.mid(line*bytesPerLine + col, 1).toHex().toUpper();

					if (!spaceDrawn)
						hex.prepend(' ');

					painter.drawText(QPoint(xPos, yPos), hex);

					if (!spaceDrawn)
						xPos += charW;
				}
			}
		}
	}

	painter.setPen(origPen);

	painter.setBackground(defBrush);
	painter.setBackgroundMode(Qt::TransparentMode);

	if (isHexAreaEnabled()  &&  isASCIIAreaEnabled()) {
		// Paint the separator between hex and ASCII area
		painter.drawLine(QLine(asciiStartX - hexAsciiGapSize/2, 0, asciiStartX - hexAsciiGapSize/2, height()));
	}

	if (isASCIIAreaEnabled()) {
		// Paint the ASCII area
		for (int line = startLine ; line <= endLine ; line++) {
			QByteArray ascii = data.mid(line*bytesPerLine, bytesPerLine);

			int offsStart = line*bytesPerLine;
			int offsEnd = calculateLineEndOffset(line);

			// Transform unprintable characters to '.'
			for (int i = 0 ; i < ascii.size() ; i++) {
				if (((char) ascii[i]) < 0x20  ||  ((char) ascii[i]) > 0x7E) {
					ascii[i] = '.';
				}
			}

			int hlStart = -1, hlLen = 0;

			if (selStart != -1) {
				if (offsStart <= selStart) {
					// Selection starts in or after current line
					if (offsEnd >= selStart) {
						// Selection starts in current line
						hlStart = selStart;
						hlLen = min(selLen, (offsEnd-hlStart+1));
					} else {
						// Selection starts after current line
						hlStart = -1;
						hlLen = 0;
					}
				} else {
					// Selection starts before current line
					if (offsStart >= selStart+selLen) {
						// Selection advances into or through current line
						hlStart = -1;
						hlLen = 0;
					} else {
						// Selection is completely before current line.
						hlStart = offsStart;
						hlLen = min(selStart+selLen - offsStart, offsEnd-offsStart+1);
					}
				}
			}

			int yPos = (line-lineDisplayOffset) * charH + charAsc;

			if (hlStart == -1) {
				painter.drawText(QPoint(asciiStartX, yPos), ascii);
			} else {
				// If this line is (partially or completely) highlighted, we have to split the text to be able
				// to draw only part of the line as selected.
				hlStart -= offsStart;

				// Draw text before selection
				painter.drawText(QPoint(asciiStartX, yPos), ascii.left(hlStart));

				painter.setBackground(selBrush);
				painter.setBackgroundMode(Qt::OpaqueMode);

				// Draw the selection text
				painter.drawText(QPoint(asciiStartX+hlStart*charW, yPos),
						ascii.mid(hlStart, hlLen));

				painter.setBackground(defBrush);
				painter.setBackgroundMode(Qt::TransparentMode);

				// Draw text after selection
				painter.drawText(QPoint(asciiStartX+(hlStart+hlLen)*charW, yPos),
						ascii.mid(hlStart+hlLen, (offsEnd-offsStart+1) - (hlStart+hlLen)));
			}
		}
	}

	// Paint cursor
	if (cursorBlink) {
		int cursorLine = (float) cursorPos / bytesPerLine;

		if (cursorLine >= startLine  &&  cursorLine <= endLine) {
			// First we paint the cursor in the hex area
			int xPos = hexStartX + ((cursorPos % bytesPerLine) * 3) * charW;
			int yPos = (cursorLine - lineDisplayOffset) * charH;

			if (isHexAreaEnabled()) {
				if (!cursorAscii) {
					if (overwriteMode)
						painter.fillRect(xPos, yPos, charW*2, charH, palette().color(QPalette::WindowText));
					else
						painter.fillRect(xPos-2, yPos, 2, charH, palette().color(QPalette::WindowText));
				} else {
					if (overwriteMode)
						painter.drawRect(xPos, yPos, charW*2, charH);
					else
						painter.fillRect(xPos-1, yPos, 1, charH, palette().color(QPalette::WindowText));
				}
			}

			// And now the ASCII area cursor
			xPos = asciiStartX + (cursorPos % bytesPerLine) * charW;

			if (isASCIIAreaEnabled()) {
				if (cursorAscii) {
					if (overwriteMode)
						painter.fillRect(xPos, yPos, charW, charH, palette().color(QPalette::WindowText));
					else
						painter.fillRect(xPos-2, yPos, 2, charH, palette().color(QPalette::WindowText));
				} else {
					if (overwriteMode)
						painter.drawRect(xPos, yPos, charW, charH);
					else
						painter.fillRect(xPos-1, yPos, 1, charH, palette().color(QPalette::WindowText));
				}
			}
		}
	}
}
