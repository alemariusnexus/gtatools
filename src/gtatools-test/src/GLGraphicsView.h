/*
 * GLGraphicsView.h
 *
 *  Created on: 25.12.2011
 *      Author: alemariusnexus
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
