/*
 * GLGraphicsView.cpp
 *
 *  Created on: 25.12.2011
 *      Author: alemariusnexus
 */

#include "GLGraphicsView.h"



void GLGraphicsView::resizeEvent(QResizeEvent* evt)
{
	if (scene()) {
		scene()->setSceneRect(QRectF(QPointF(0.0f, 0.0f), evt->size()));
	}

	QGraphicsView::resizeEvent(evt);
}
