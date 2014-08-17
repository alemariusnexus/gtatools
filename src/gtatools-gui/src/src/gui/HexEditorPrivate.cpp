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

#include "HexEditorPrivate.h"
#include "HexEditorContentTraverser.h"
#include <nxcommon/Color4.h>
#include <nxcommon/util.h>
#include <QtGui/QPainter>
#include <QtGui/QPaintEvent>
#include <QtGui/QApplication>
#include <QtGui/QClipboard>
#include <QtGui/QToolTip>
#include <QtCore/QByteArray>
#include <QtCore/QFile>
#include <cstdio>
#include <cmath>
#include <algorithm>

using std::min;
using std::max;



HexEditorPrivate::HexEditorPrivate(QWidget* parent)
		: QWidget(parent), enabledParts(DefaultParts), doc(NULL), docCreatedHere(false), addrHexGapSize(20),
		  hexAsciiGapSize(20), numAddressChars(8), cursorPos(0), cursorAnchor(0), overwriteMode(true),
		  temporaryOverwriteMode(false), cursorBlink(false), cursorAscii(false), numLines(0), bytesPerLine(0),
		  lineDisplayOffset(0), currentByteEdited(false), editable(true)
{
	QFont font("", 10);
	font.setFixedPitch(true);
	font.setKerning(false);
	setFont(font);

	setData(QByteArray());

	setAddressColor(QColor(Qt::lightGray).lighter(100));
	setEvenColumnColor(palette().color(QPalette::Text));
	setOddColumnColor(QColor(Qt::blue).lighter(125));

	connect(&cursorBlinkTimer, SIGNAL(timeout()), this, SLOT(updateCursorBlink()));
	cursorBlinkTimer.start(500);

	//setMouseTracking(true);

	connect(this, SIGNAL(dataReplaced(int, int, const QByteArray&, const QByteArray&)), this,
			SLOT(dataReplacedSlot(int, int, const QByteArray&, const QByteArray&)));
}


void HexEditorPrivate::setData(const QByteArray& data)
{
	HexEditorDocument* doc = new HexEditorDocument(data);
	setDocument(doc);
	docCreatedHere = true;
}


void HexEditorPrivate::loadData(const File& file)
{
	QFile qfile(file.getPath().toString().get());
	qfile.open(QFile::ReadOnly);
	QByteArray data = qfile.readAll();
	qfile.close();
	setData(data);
}


void HexEditorPrivate::updateContentLineCount(int num)
{
	numLines = num;
	emit contentLineCountChanged(num);
}


void HexEditorPrivate::setFont(const QFont& font)
{
	printf("#### setFont()\n");

	QWidget::setFont(font);

	QFontMetricsF fm(font);

	charW = fm.width('F');
	charH = fm.height();
	charAsc = fm.ascent();
	charDesc = fm.descent();

	update();
}


void HexEditorPrivate::setDocument(HexEditorDocument* doc)
{
	if (this->doc) {
		disconnect(this->doc, NULL, this, NULL);

		if (docCreatedHere)
			delete this->doc;
	}

	this->doc = doc;

	connect(doc, SIGNAL(dataChanged(const QByteArray&)), this, SIGNAL(dataChanged(const QByteArray&)));
	connect(doc, SIGNAL(dataReplaced(int, int, const QByteArray&, const QByteArray&)), this,
			SIGNAL(dataReplaced(int, int, const QByteArray&, const QByteArray&)));

	recomputeGeometry();
	update();
	setCursorPosition(0);
	emit dataChanged(doc->getData());
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
	} else if (cpos > doc->size()) {
		cpos = doc->size();
	}

	if (anchor < 0) {
		anchor = 0;
	} else if (anchor > doc->size()) {
		anchor = doc->size();
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

	int lastLineBytes = doc->size() % bytesPerLine;

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
			offset += (doc->getData().size() % bytesPerLine);
		}
	} else {
		offset += bytesPerLine-1;
	}

	return offset;
}


void HexEditorPrivate::updateLines(int first, int last)
{
	if (first != 0)
		first--;
	if (last != getNumberOfLines()-1)
		last++;

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
	replace(offset, 0, data);
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
	replace(offset, len, QByteArray());
}


void HexEditorPrivate::replace(int offset, int len, const QByteArray& data)
{
	doc->replace(offset, len, data);
}


QByteArray HexEditorPrivate::getSelectedData() const
{
	int start = min(cursorPos, cursorAnchor);
	int end = max(cursorPos, cursorAnchor);
	return doc->getData().mid(start, end-start);
}


void HexEditorPrivate::dataReplacedSlot(int offset, int len, const QByteArray& oldData, const QByteArray& newData)
{
	if (len == newData.size()) {
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

	/*int x = min(evt->x(), width());
	int y = min(evt->y(), height());
	int offs = calculateOffset(x, y);

	if ((evt->buttons() & Qt::LeftButton)  !=  0) {
		setCursor(offs, cursorAnchor);
	}

	HexEditorBlock* block;
	int subdiv;

	calculateBlockAndSubdivision(x, y, block, subdiv);

	updateCurrentBlockAndSubdivision(block, subdiv);*/
}


void HexEditorPrivate::keyPressEvent(QKeyEvent* evt)
{
	char c = evt->text()[0].toAscii();

	if (cursorAscii  &&  c >= 0x20  &&  c <= 0x7E) {
		// Insert or replace an ASCII character.

		if (!editable)
			return;

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

		if (!editable)
			return;

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
			QByteArray hex = doc->getData().mid(cursorPos, 1).toHex();

			// If a byte shall be overwritten, the first key pressed will change the lower 4 bits. If then
			// another hex-key is hit, the previously entered 4 bits will become the upper half, and the new
			// key becomes the lower 4 bits. When the first key is hit, we enter a state (currentByteEdited
			// = true) which will be left when the cursor changes or the second key is pressed.
			if (cursorPos == doc->getData().size()) {
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
		if (!editable)
			return;

		if (cursorPos == cursorAnchor) {
			remove(cursorPos, 1);
		} else {
			int start = min(cursorPos, cursorAnchor);
			int end = max(cursorPos, cursorAnchor);
			remove(start, end-start);
			setCursorPosition(min(cursorPos, cursorAnchor));
		}
	} else if (evt->key() == Qt::Key_Backspace) {
		if (!editable)
			return;

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
		setCursorPosition(doc->getData().size());
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
		setCursor(0, doc->getData().size());
	} else if (evt->matches(QKeySequence::SelectEndOfDocument)) {
		setCursor(doc->getData().size(), cursorAnchor);
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

		if (evt->matches(QKeySequence::Cut)  &&  editable) {
			int start = min(cursorPos, cursorAnchor);
			int end = max(cursorPos, cursorAnchor);
			remove(start, end-start);
			setCursorPosition(start);
		}
	} else if (evt->matches(QKeySequence::Paste)) {
		if (!editable)
			return;

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
	numLines = max((int) ceil((double) doc->getData().size() / bytesPerLine), 1);

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

	//painter.setClipRect(rect);

	// Calculate the start and end lines to be redrawn. These are really just the lines to be redrawn, and
	// NOT the entire visible line range!
	// Theses values are counted from zero!
	// Note: When the very last line is to be redrawn and if it is completely full, we will append an empty
	// line, so that the cursor can be placed there to add new values (?: operator in second min() argument).
	int startLine = rect.y() / charH + lineDisplayOffset;
	int endLine = min((int) ((rect.y() + rect.height()) / charH + 1 + lineDisplayOffset),
			(doc->getData().size() % bytesPerLine) == 0 ? numLines : numLines-1);

	if (doc->getData().size() == 0)
		endLine = startLine;

	QPen origPen = painter.pen();

	// This must have the thickness of the thickest border that is to be supported (see the decrement/increment of
	// startLine/endLine below for more details).
	// Keep these additional parentheses. I have no idea why, but Clang complains when they are missing...
	QPen rectPen((QColor(Qt::black)));
	rectPen.setWidth(3);

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

	QColor selColor = QColor(Qt::blue);
	selColor.setAlpha(75);
	rstate.selBrush = QBrush(selColor);
	rstate.defBrush = painter.background();

	rstate.evenColPen = QPen(evenColColor);
	rstate.oddColPen = QPen(oddColColor);

	QRect clipRect(rect);

	// We will actually paint a small part of the line before and the line after the requested range, because if we have
	// HexEditorBlocks with borders enabled, these borders can extend to at most one additional line, and by overpainting
	// the requested line range with a white rectangle in the beginning of paintEvent(), we might have overwritten these
	// borders, so we'll just paint this small part where borders could have been again.
	if (startLine != 0) {
		startLine--;
		clipRect.setTop(clipRect.top() - floor(rectPen.width()*0.5f));
	}
	if (endLine == getNumberOfLines()-1) {
		endLine++;
		clipRect.setBottom(clipRect.bottom() + floor(rectPen.width()*0.5f));
	}

	// Don't overpaint too much. Still a hack, but... meh.
	painter.setClipRect(clipRect);

	// This algorithm was originally written to work with a 'start-length' range of the selection.
	int selStart = min(cursorPos, cursorAnchor);
	int selLen = max(cursorPos, cursorAnchor) - selStart;

	if (selLen == 0) {
		selStart = -1;
	}

	painter.setBackgroundMode(Qt::TransparentMode);

	// Draw the selection block.
	if (selLen > 0) {
		drawBlock(&painter, rstate.selBrush, startLine, endLine, selStart, selLen, QList<int>(), -1, false);
	}


	// Now, draw all HexEditorBlocks that lie inside the requested line range, including borders.
	QList<HexEditorBlock*> blocks = doc->getEffectiveBlocks(startLine*bytesPerLine, (endLine-startLine)*bytesPerLine);

	for (QList<HexEditorBlock*>::iterator it = blocks.begin() ; it != blocks.end() ; it++) {
		HexEditorBlock* block = *it;

		QBrush brush = block->getBackgroundBrush();

		QPen pen(block->getBorderColor());

		bool border = true;

		switch (block->getBorderType()) {
		case HexEditorBlock::NoBorder:
			border = false;
			break;

		case HexEditorBlock::ThinBorder:
			pen.setWidth(1);
			painter.setPen(pen);
			break;

		case HexEditorBlock::MediumBorder:
			pen.setWidth(2);
			painter.setPen(pen);
			break;

		case HexEditorBlock::ThickBorder:
			pen.setWidth(3);
			painter.setPen(pen);
			break;
		}

		drawBlock(&painter, brush, startLine, endLine, block->getOffset(), block->getLength(), block->getSubdivisions(), -1, border);

		/*if (block == currentBlock) {
			QList<int> subdivs = block->getSubdivisions();

			int csdEnd = currentBlockSubdivision >= subdivs.size() ? block->getLength() : subdivs[currentBlockSubdivision];
			int csdStart = currentBlockSubdivision == 0 ? 0 : subdivs[currentBlockSubdivision-1];

			QBrush csdBrush = brush;
			csdBrush.setColor(csdBrush.color().darker(150));
			drawBlock(&painter, csdBrush, startLine, endLine, csdStart+block->getOffset(), csdEnd-csdStart, QList<int>(), -1, false);
		}*/
	}

	if (isHexAreaEnabled()) {
		painter.setBackground(rstate.defBrush);

		// Paint the hex area
		for (int line = startLine ; line <= endLine ; line++) {
			int yPos = (line-lineDisplayOffset) * charH + charAsc;

			// Paint a single hex line
			for (int col = 0 ; col < bytesPerLine  &&  line*bytesPerLine + col < doc->getData().size() ; col++) {
				int xPos = hexStartX + col*3*charW - charW;
				int pos = line*bytesPerLine + col;


				if (col%2 == 0)
					painter.setPen(rstate.evenColPen);
				else
					painter.setPen(rstate.oddColPen);

				if (col != 0) {
					// Every byte but the first one in the line gets a preceding space.

					painter.drawText(QPoint(xPos, yPos), " ");
				}

				QByteArray hex = doc->getData().mid(line*bytesPerLine + col, 1).toHex().toUpper();
				painter.drawText(QPoint(xPos + charW, yPos), hex);
			}
		}
	}

	painter.setPen(origPen);

	painter.setBackground(rstate.defBrush);
	painter.setBackgroundMode(Qt::TransparentMode);

	if (isHexAreaEnabled()  &&  isASCIIAreaEnabled()) {
		// Paint the separator between hex and ASCII area
		painter.drawLine(QLine(asciiStartX - hexAsciiGapSize/2, 0, asciiStartX - hexAsciiGapSize/2, height()));
	}

	if (isASCIIAreaEnabled()) {
		// Paint the ASCII area
		for (int line = startLine ; line <= endLine ; line++) {
			QByteArray ascii = doc->getData().mid(line*bytesPerLine, bytesPerLine);

			int offsStart = line*bytesPerLine;
			int offsEnd = calculateLineEndOffset(line);

			// Transform unprintable characters to '.'
			for (int i = 0 ; i < ascii.size() ; i++) {
				if (((char) ascii[i]) < 0x20  ||  ((char) ascii[i]) > 0x7E) {
					ascii[i] = '.';
				}
			}

			int yPos = (line-lineDisplayOffset) * charH + charAsc;

			painter.drawText(QPoint(asciiStartX, yPos), ascii);
		}
	}

	// Paint cursor
	if (cursorBlink  &&  isEnabled()) {
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


bool HexEditorPrivate::event(QEvent* evt)
{
	/*if (evt->type() == QEvent::ToolTip) {
		QHelpEvent* hevt = (QHelpEvent*) evt;
		QPoint pos = hevt->pos();

		int offset = calculateOffset(pos);

		if (offset == -1) {
			evt->ignore();
		} else {
			QList<HexEditorBlock*> blocks = doc->getEffectiveBlocks(offset, 1);

			QToolTip::showText(hevt->globalPos(), QString("Number of Blocks: %1").arg(blocks.size()), this);
		}

		return true;
	}*/

	return QWidget::event(evt);
}


void HexEditorPrivate::drawBlock(QPainter* painter, const QBrush& brush, int startLine, int endLine, int offset, int len,
		const QList<int>& subdivs, int activeSubdiv, bool border)
{
	int bStartLine = min(max(offset / bytesPerLine, startLine), getNumberOfLines()-1);
	int bEndLine = min(min((offset+len-1) / bytesPerLine, endLine), getNumberOfLines()-1);

	int firstLineCharOffset = (bStartLine == offset / bytesPerLine) ? offset % bytesPerLine : 0;
	int lastLineCharOffset = (bEndLine == (offset+len) / bytesPerLine) ? (offset+len) % bytesPerLine : bytesPerLine;

	if (firstLineCharOffset >= doc->size())
		return;

	lastLineCharOffset = min(lastLineCharOffset, (int) (doc->size() - bEndLine*bytesPerLine));

	QList<int>::const_iterator sit = qLowerBound(subdivs.begin(), subdivs.end(), bStartLine*bytesPerLine - offset);
	int subdivIdx = sit - subdivs.begin();

	for (int line = bStartLine ; line <= bEndLine ; line++) {
		int yPos = (line-lineDisplayOffset) * charH;
		int xPosHexS, xPosHexE;
		int xPosAsciiS, xPosAsciiE;

		if (line == bStartLine) {
			xPosHexS = hexStartX + firstLineCharOffset*3*charW;
			xPosAsciiS = asciiStartX + firstLineCharOffset*charW;
		} else {
			xPosHexS = hexStartX;
			xPosAsciiS = asciiStartX;
		}

		if (line == bEndLine) {
			xPosHexE = hexStartX + lastLineCharOffset*3*charW - charW;
			xPosAsciiE = asciiStartX + lastLineCharOffset*charW;
		} else {
			xPosHexE = hexStartX + bytesPerLine*3*charW - charW;
			xPosAsciiE = asciiStartX + bytesPerLine*charW;
		}

		painter->fillRect(xPosHexS, yPos, xPosHexE-xPosHexS, charH, brush);
		painter->fillRect(xPosAsciiS, yPos, xPosAsciiE-xPosAsciiS, charH, brush);

		if (border) {
			float pw = painter->pen().widthF();
			int pwOffs = floor(pw*0.5f);

			painter->drawRect(xPosHexS, yPos, xPosHexE-xPosHexS, charH);

			/*// Vertical start line
			painter->drawLine(xPosHexS - pwOffs, yPos, xPosHexS - pwOffs, yPos+charH);
			painter->drawLine(xPosAsciiS - pwOffs, yPos, xPosAsciiS - pwOffs, yPos+charH);

			// Vertical end line
			painter->drawLine(xPosHexE+pwOffs, yPos, xPosHexE+pwOffs, yPos+charH);
			painter->drawLine(xPosAsciiE+pwOffs, yPos, xPosAsciiE+pwOffs, yPos+charH);

			if (line == bStartLine) {
				// Horizontal top line
				painter->drawLine(xPosHexS-pwOffs, yPos, xPosHexE+pwOffs, yPos);
				painter->drawLine(xPosAsciiS-pwOffs, yPos, xPosAsciiE+pwOffs, yPos);
			} else if (line == bStartLine+1) {
				// Horizontal second line top line
				int firstLineXPosHexS = min((int) (hexStartX + firstLineCharOffset*3*charW), xPosHexE);
				int firstLineXPosAsciiS = min((int) (asciiStartX + firstLineCharOffset*charW), xPosAsciiE);
				painter->drawLine(xPosHexS-pwOffs, yPos, firstLineXPosHexS-pwOffs, yPos);
				painter->drawLine(xPosAsciiS-pwOffs, yPos, firstLineXPosAsciiS-pwOffs, yPos);
			}

			if (line == bEndLine) {
				// Horizontal bottom line
				painter->drawLine(xPosHexS-pwOffs, yPos+charH, xPosHexE+pwOffs, yPos+charH);
				painter->drawLine(xPosAsciiS-pwOffs, yPos+charH, xPosAsciiE+pwOffs, yPos+charH);
			} else if (line == bEndLine-1) {
				// Horizontal second-to-last line bottom line
				int lastLineXPosHexE = max((int) (hexStartX + lastLineCharOffset*3*charW - charW), xPosHexS);
				int lastLineXPosAsciiE = max((int) (asciiStartX + lastLineCharOffset*charW), xPosAsciiS);
				painter->drawLine(lastLineXPosHexE+pwOffs, yPos+charH, xPosHexE+pwOffs, yPos+charH);
				painter->drawLine(lastLineXPosAsciiE+pwOffs, yPos+charH, xPosAsciiE+pwOffs, yPos+charH);
			}*/
		}

		while (sit != subdivs.end()  &&  *sit+offset < (line+1)*bytesPerLine) {
			int sdOffs = *sit + offset;

			int xPosHexSd = hexStartX + (sdOffs % bytesPerLine)*3*charW - charW/2;

			painter->drawLine(xPosHexSd, yPos, xPosHexSd, yPos+charH);

			sit++;
			subdivIdx++;
		}
	}
}


/*void HexEditorPrivate::updateCurrentBlockAndSubdivision(HexEditorBlock* block, int subdiv)
{
	if (block == currentBlock  &&  subdiv == currentBlockSubdivision)
		return;

	HexEditorBlock* oldBlock = currentBlock;
	int oldSubdiv = currentBlockSubdivision;

	currentBlock = block;
	currentBlockSubdivision = subdiv;

	if (oldBlock) {
		int offs, len;
		oldBlock->getSubdivisionRange(oldSubdiv, offs, len);

		updateRange(offs, len);
	}

	if (block) {
		int offs, len;
		block->getSubdivisionRange(subdiv, offs, len);

		updateRange(offs, len);
	}
}


bool HexEditorPrivate::calculateBlockAndSubdivision(const QPoint& pos, HexEditorBlock*& block, int& subdiv)
{

	int offs = calculateOffset(pos - QPoint(charW/2, 0));

	QList<HexEditorBlock*> blocks = doc->getEffectiveBlocks(offs, 0);

	if (!blocks.isEmpty()) {
		block = blocks[0];
		QList<int> subdivs = block->getSubdivisions();
		QList<int>::const_iterator it = qUpperBound(subdivs.begin(), subdivs.end(), offs - block->getOffset());

		subdiv = it - subdivs.begin();
		return true;
	} else {
		block = NULL;
		subdiv = -1;
		return false;
	}
}*/

