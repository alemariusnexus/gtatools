/*
	Copyright 2010-2012 David "Alemarius Nexus" Lerch

	This file is part of gtatools-test.

	gtatools-test is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	gtatools-test is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with gtatools-test.  If not, see <http://www.gnu.org/licenses/>.

	Additional permissions are granted, which are listed in the file
	GPLADDITIONS.
 */

#ifndef GLGRAPHICSVIEW_H_
#define GLGRAPHICSVIEW_H_

#include <QtGui/QGraphicsView>
#include <QtGui/QResizeEvent>


class GLGraphicsView : public QGraphicsView
{
public:


protected:
	virtual void resizeEvent(QResizeEvent* evt);
};

#endif /* GLGRAPHICSVIEW_H_ */
