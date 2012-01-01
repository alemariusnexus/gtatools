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

#include "GLGraphicsScene.h"
#include <QtGui/QWidget>
#include <QtGui/QDialog>
#include <QtGui/QGraphicsItem>
#include <QtCore/QList>
#include <QtCore/QTimer>
#include <QtGui/QVBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QPaintEngine>
#include <QtOpenGL/QGLContext>



GLGraphicsScene::GLGraphicsScene(Controller* ctrl)
		: controller(ctrl)
{
	connect(this, SIGNAL(sceneRectChanged(const QRectF&)), this, SLOT(sceneRectChangedSlot(const QRectF&)));

	/*QDialog* dlg = new QDialog(NULL, Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::Window);
	dlg->setWindowTitle("Qt Test");
	dlg->resize(300, 500);
	dlg->move(50, 50);

	QVBoxLayout* layout = new QVBoxLayout(dlg);
	layout->addWidget(new QLabel("Hallo Welt!"));

	addWidget(dlg);

	QList<QGraphicsItem*> its = items();

	for (QList<QGraphicsItem*>::iterator it = its.begin() ; it != its.end() ; it++) {
		QGraphicsItem* item = *it;

		item->setFlags(QGraphicsItem::ItemIsMovable);
		item->setCacheMode(QGraphicsItem::DeviceCoordinateCache);
	}*/
}


void GLGraphicsScene::drawBackground(QPainter* painter, const QRectF& rect)
{
	controller->paint();

	QTimer::singleShot(0, this, SLOT(update()));
}


void GLGraphicsScene::mousePressEvent(QGraphicsSceneMouseEvent* evt)
{
	QGraphicsScene::mousePressEvent(evt);

	if (!evt->isAccepted()) {
		controller->mouseButtonPressed(evt->button(), (int) evt->scenePos().x(), (int) evt->scenePos().y());
		evt->accept();
	}
}


void GLGraphicsScene::mouseReleaseEvent(QGraphicsSceneMouseEvent* evt)
{
	QGraphicsScene::mouseReleaseEvent(evt);

	if (!evt->isAccepted()) {
		controller->mouseButtonReleased(evt->button(), (int) evt->scenePos().x(), (int) evt->scenePos().y());
		evt->accept();
	}
}


void GLGraphicsScene::mouseMoveEvent(QGraphicsSceneMouseEvent* evt)
{
	QGraphicsScene::mouseMoveEvent(evt);

	if (!evt->isAccepted()) {
		controller->mouseMotion((int) evt->scenePos().x(), (int) evt->scenePos().y());
		evt->accept();
	}
}


void GLGraphicsScene::keyPressEvent(QKeyEvent* evt)
{
	QGraphicsScene::keyPressEvent(evt);

	if (!evt->isAccepted()) {
		controller->keyPressed(evt);
		evt->accept();
	}
}


void GLGraphicsScene::keyReleaseEvent(QKeyEvent* evt)
{
	QGraphicsScene::keyReleaseEvent(evt);

	if (!evt->isAccepted()) {
		controller->keyReleased(evt);
		evt->accept();
	}
}


void GLGraphicsScene::sceneRectChangedSlot(const QRectF& rect)
{
	controller->reshape((int) rect.width(), (int) rect.height());
}
