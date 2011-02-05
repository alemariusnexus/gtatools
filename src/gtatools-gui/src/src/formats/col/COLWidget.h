#ifndef COLWIDGET_H
#define COLWIDGET_H

#include <QtGui/QWidget>
#include "ui_COLWidget.h"
#include <gtaformats/gtacol.h>
#include <QtCore/QList>
#include "COLSphereBoxRenderWidget.h"
#include "COLMeshRenderWidget.h"

class COLWidget : public QWidget
{
    Q_OBJECT

public:
    COLWidget(const File& file, QWidget *parent = 0);
    ~COLWidget();

private:
    void updateVertexMesh();

private slots:
    void currentModelChanged(int index);
    void faceGroupsToggled(bool status);
    void faceGroupSelectionChanged();
    void currentMeshChanged(int index);
    void wireframePropertyChanged(bool wireframe);
	void shadowMeshFaceSelectionChanged(int prevIdx, int idx);
	void vertexMeshFaceSelectionChanged(int prevIdx, int idx);

private:
    Ui::COLWidgetClass ui;
    QList<COLModel*> models;
    COLSphereBoxRenderWidget* sphereBoxRenderer;
    COLMeshRenderWidget* meshRenderer;
    COLMeshRenderWidget* shadowMeshRenderer;
    QList<int> vmeshFaceIndexMap;
};

#endif // COLWIDGET_H
