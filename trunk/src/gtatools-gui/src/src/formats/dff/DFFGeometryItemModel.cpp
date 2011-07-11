/*
	Copyright 2010-2011 David "Alemarius Nexus" Lerch

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

#include "DFFGeometryItemModel.h"
#include <gtaformats/dff/DFFMesh.h>



DFFGeometryItemModel::DFFGeometryItemModel(DFFMesh* mesh)
{
	for (DFFMesh::GeometryIterator it = mesh->getGeometryBegin() ; it != mesh->getGeometryEnd() ; it++) {
		DFFGeometry* geom = *it;
		geoms << new InternalDFFGeometry(geom);
	}
}


QModelIndex DFFGeometryItemModel::getGeometryIndex(DFFGeometry* geom) const
{
	int i = 0;
	for (QList<InternalDFFGeometry*>::const_iterator it = geoms.begin() ; it != geoms.end() ; it++, i++) {
		InternalDFFGeometry* igeom = *it;

		if (igeom->getGeometry() == geom) {
			return index(i, 0, QModelIndex());
		}
	}

	return QModelIndex();
}


QModelIndex DFFGeometryItemModel::getGeometryPartIndex(DFFGeometryPart* part)
{
	QModelIndex geomIdx = getGeometryIndex(part->getGeometry());
	InternalDFFGeometry* igeom = (InternalDFFGeometry*) geomIdx.internalPointer();
	int32_t idx = igeom->getGeometry()->indexOf(part);
	return index(idx, 0, geomIdx);
}


void DFFGeometryItemModel::getDisplayedGeometryParts(DFFGeometry* geom,
		QLinkedList<const DFFGeometryPart*>& parts)
{
	for (QList<InternalDFFGeometry*>::const_iterator it = geoms.begin() ; it != geoms.end() ; it++) {
		InternalDFFGeometry* igeom = *it;

		if (igeom->getGeometry() == geom) {
			InternalDFFGeometry::PartIterator pit;

			for (pit = igeom->getPartBegin() ; pit != igeom->getPartEnd() ; pit++) {
				InternalDFFGeometryPart* part = *pit;

				if (part->isDisplayed())
					parts.push_back(part->getPart());
			}

			break;
		}
	}
}


Qt::ItemFlags DFFGeometryItemModel::flags(const QModelIndex& index) const
{
	if (!index.isValid())
		return 0;

	return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable;
}


QVariant DFFGeometryItemModel::data(const QModelIndex& index, int role) const
{
	if (!index.isValid())
		return QVariant();

	if (index.parent().isValid()) {
		// index is a geometry part
		InternalDFFGeometryPart* ipart = (InternalDFFGeometryPart*) index.internalPointer();
		DFFGeometryPart* part = ipart->getPart();

		if (role == Qt::DisplayRole) {
			return tr("Part %1").arg(part->getGeometry()->indexOf(part)+1);
		} else if (role == Qt::CheckStateRole) {
			return ipart->isDisplayed() ? Qt::Checked : Qt::Unchecked;
		}
	} else {
		// index is a geometry
		InternalDFFGeometry* igeom = (InternalDFFGeometry*) index.internalPointer();
		DFFGeometry* geom = igeom->getGeometry();

		if (role == Qt::DisplayRole) {
			QString text = tr("Geometry %1").arg(geoms.indexOf(igeom)+1);

			DFFFrame* frame = geom->getAssociatedFrame();

			if (frame) {
				text += QString(" [%1]").arg(frame->getName());
			}

			return text;
		} else if (role == Qt::CheckStateRole) {
			return igeom->isDisplayed() ? Qt::Checked : Qt::Unchecked;
		}
	}

	return QVariant();
}


QVariant DFFGeometryItemModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	return QVariant();
}


int DFFGeometryItemModel::rowCount(const QModelIndex& index) const
{
	if (!index.isValid())
		return geoms.size();

	if (index.parent().isValid()) {
		// index is a geometry part
		return 0;
	} else {
		InternalDFFGeometry* igeom = (InternalDFFGeometry*) index.internalPointer();
		return igeom->getPartCount();
	}
}


QModelIndex DFFGeometryItemModel::index(int row, int column, const QModelIndex& parent) const
{
	if (!hasIndex(row, column, parent))
		return QModelIndex();

	if (parent.isValid()) {
		InternalDFFGeometry* igeom = (InternalDFFGeometry*) parent.internalPointer();
		InternalDFFGeometryPart* ipart = igeom->getPart(row);
		return createIndex(row, column, ipart);
	} else {
		InternalDFFGeometry* igeom = geoms[row];
		return createIndex(row, column, igeom);
	}
}


QModelIndex DFFGeometryItemModel::parent(const QModelIndex& index) const
{
	if (!index.isValid())
		return QModelIndex();

	Item* item = (Item*) index.internalPointer();

	if (item->isPart()) {
		InternalDFFGeometryPart* ipart = (InternalDFFGeometryPart*) item;
		return createIndex(geoms.indexOf(ipart->getGeometry()), 0, ipart->getGeometry());
	} else {
		return QModelIndex();
	}
}


bool DFFGeometryItemModel::setData(const QModelIndex& index, const QVariant& data, int role)
{
	if (role == Qt::CheckStateRole) {
		Item* item = (Item*) index.internalPointer();

		item->setDisplayed(data.toInt() == Qt::Checked);

		if (item->isPart()) {
			InternalDFFGeometryPart* ipart = (InternalDFFGeometryPart*) item;
			emit geometryPartDisplayStateChanged(ipart->getPart(), item->isDisplayed());
		} else {
			InternalDFFGeometry* igeom = (InternalDFFGeometry*) item;
			emit geometryDisplayStateChanged(igeom->getGeometry(), item->isDisplayed());
		}

		return true;
	}

	return false;
}
