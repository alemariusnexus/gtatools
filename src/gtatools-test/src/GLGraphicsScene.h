/*
 * GLGraphicsScene.h
 *
 *  Created on: 25.12.2011
 *      Author: alemariusnexus
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
