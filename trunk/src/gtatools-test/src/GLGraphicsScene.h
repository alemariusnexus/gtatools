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

#ifndef GLGRAPHICSSCENE_H_
#define GLGRAPHICSSCENE_H_

#include <QtGui/QGraphicsScene>
#include <QtGui/QGraphicsSceneMouseEvent>
#include <QtGui/QKeyEvent>
#include "Controller.h"



class GLGraphicsScene : public QGraphicsScene
{
	Q_OBJECT

public:
	GLGraphicsScene(Controller* ctrl);

protected:
	virtual void drawBackground(QPainter* painter, const QRectF& rect);
	virtual void mousePressEvent(QGraphicsSceneMouseEvent* evt);
	virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* evt);
	virtual void mouseMoveEvent(QGraphicsSceneMouseEvent* evt);
	virtual void keyPressEvent(QKeyEvent* evt);
	virtual void keyReleaseEvent(QKeyEvent* evt);

private slots:
	void sceneRectChangedSlot(const QRectF& rect);

private:
	Controller* controller;
};

#endif /* GLGRAPHICSSCENE_H_ */
