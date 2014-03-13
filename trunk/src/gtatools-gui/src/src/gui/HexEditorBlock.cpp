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

#include "HexEditorBlock.h"



HexEditorBlock::HexEditorBlock(int offset, int length)
		: offset(offset), length(length), bgBrush(QBrush(QColor(QColor()))), borderType(NoBorder),
		  borderColor(QColor(Qt::black))
{
}


HexEditorBlock::HexEditorBlock(const HexEditorBlock& other)
		: offset(other.offset), length(other.length), bgBrush(other.bgBrush), borderType(other.borderType),
		  borderColor(other.borderColor)
{
}


bool HexEditorBlock::overlaps(int offset, int len) const
{
	if (offset < this->offset)
		return offset+len > this->offset;
	else
		return offset < this->offset+this->length;
}


void HexEditorBlock::getSubdivisionRange(int subdiv, int& offset, int& len)
{
	offset = subdiv == 0 ? 0 : subdivs[subdiv-1];
	len = subdiv >= subdivs.size() ? len-offset : subdivs[subdiv]-offset;
}
