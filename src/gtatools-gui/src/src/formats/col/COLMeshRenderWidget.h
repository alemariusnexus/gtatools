/*
 * COLMeshRenderWidget.h
 *
 *  Created on: 04.02.2011
 *      Author: alemariusnexus
 */

#ifndef COLMESHRENDERWIDGET_H_
#define COLMESHRENDERWIDGET_H_

#include "../../gui/GLBaseWidget.h"
#include <gtaformats/gtacol.h>
#include <gta/ItemDefinition.h>


class COLMeshRenderWidget : public GLBaseWidget {
	Q_OBJECT

public:
	COLMeshRenderWidget(QWidget* parent);
	void render(const float* vertices, int32_t vertexCount, const COLFace* faces, int32_t faceCount);
	void setSelectedFace(int faceIndex);

protected:
	virtual void initializeGL();
	virtual void resizeGL(int w, int h);
	virtual void paintGL();
	void mouseDoubleClickEvent(QMouseEvent* evt);

signals:
	void faceSelectionChanged(int prevIdx, int idx);

private:
	ItemDefinition* item;
	ItemDefinition* pickItem;
	int pickedFace;
	uint8_t pickedFaceRealColor[4];
};

#endif /* COLMESHRENDERWIDGET_H_ */
