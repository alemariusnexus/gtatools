/*
 * DFFFrameItemModel.h
 *
 *  Created on: 06.08.2010
 *      Author: alemariusnexus
 */

#ifndef DFFFRAMEITEMMODEL_H_
#define DFFFRAMEITEMMODEL_H_

#include <gtaformats/dff/DFFFrame.h>
#include <gtaformats/dff/DFFMesh.h>
#include <qabstractitemmodel.h>
#include <qvariant.h>


class DFFFrameItemModel : public QAbstractItemModel
{
	Q_OBJECT

public:
	DFFFrameItemModel(DFFMesh* mesh) : mesh(mesh) {}
	Qt::ItemFlags flags(const QModelIndex& index) const;
	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
	int rowCount(const QModelIndex& index = QModelIndex()) const;
	int columnCount(const QModelIndex& index) const;
	QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const;
	QModelIndex parent(const QModelIndex& index) const;

private:
	DFFMesh* mesh;
};

#endif /* DFFFRAMEITEMMODEL_H_ */
