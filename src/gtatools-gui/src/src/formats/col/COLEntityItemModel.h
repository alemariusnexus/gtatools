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

#ifndef COLENTITYITEMMODEL_H_
#define COLENTITYITEMMODEL_H_

#include <QAbstractItemModel>
#include <QSet>
#include <gtaformats/col/COLModel.h>



class COLEntityItemModel : public QAbstractItemModel
{
	Q_OBJECT

private:
	enum ItemIndexType
	{
		Invalid,

		Spheres,
		Boxes,
		Faces,
		FaceGroups,

		Sphere,
		Box,
		FaceGroup,
		Face
	};

public:
	COLEntityItemModel(COLModel* model);
	Qt::ItemFlags flags(const QModelIndex& index) const;
	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
	int rowCount(const QModelIndex& index = QModelIndex()) const;
	int columnCount(const QModelIndex& index) const { return 1; }
	QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const;
	QModelIndex parent(const QModelIndex& index) const;
	bool setData(const QModelIndex& index, const QVariant& data, int role = Qt::EditRole);

signals:
	void sphereDisplayStateChanged(COLSphere* sphere, bool displayed);
	void boxDisplayStateChanged(COLBox* box, bool displayed);
	void faceDisplayStateChanged(COLFace* face, bool displayed);

private:
	ItemIndexType getItemIndexType(const QModelIndex& idx, uint32_t& idxInModel);

private:
	COLModel* model;

	char sphereCountDummy;
	char boxCountDummy;
	char faceGroupCountDummy;
	char faceCountDummy;

	bool spheresEnabled;
	bool boxesEnabled;
	bool faceGroupsEnabled;
	bool facesEnabled;
	QSet<uint32_t> enabledBoxes;
	QSet<uint32_t> enabledSpheres;
	QSet<uint32_t> enabledFaceGroups;
	QSet<uint32_t> enabledFaces;
};

#endif /* COLENTITYITEMMODEL_H_ */
