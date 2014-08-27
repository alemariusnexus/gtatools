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

#ifndef HEXEDITORBLOCK_H_
#define HEXEDITORBLOCK_H_

#include <QColor>
#include <QBrush>
#include <QtCore/QList>



class HexEditorBlock
{
public:
	enum BorderType
	{
		NoBorder = 0,
		ThinBorder = 1,
		MediumBorder = 2,
		ThickBorder = 3
	};

public:
	//HexEditorBlock() : offset(-1), length(-1), borderType(NoBorder), borderColor(QColor(Qt::black)) {}
	HexEditorBlock(int offset, int length);
	HexEditorBlock(const HexEditorBlock& other);
	void setBackgroundBrush(const QBrush& brush) { bgBrush = brush; }
	QBrush getBackgroundBrush() const { return bgBrush; }
	void setBorderType(BorderType type) { borderType = type; }
	BorderType getBorderType() const { return borderType; }
	void setBorderColor(const QColor& color) { borderColor = color; }
	QColor getBorderColor() const { return borderColor; }
	int getOffset() const { return offset; }
	int getLength() const { return length; }
	bool isValid() const { return offset >= 0; }
	bool contains(int offset) const { return offset >= this->offset  &&  offset < this->offset+length; }
	bool overlaps(int offset, int len) const;
	void setSubdivisions(const QList<int>& subdivs) { this->subdivs = subdivs; }
	QList<int> getSubdivisions() const { return subdivs; }
	void getSubdivisionRange(int subdiv, int& offset, int& len);

private:
	int offset;
	int length;
	QList<int> subdivs;

	QBrush bgBrush;
	BorderType borderType;
	QColor borderColor;
};

#endif /* HEXEDITORBLOCK_H_ */
