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

#ifndef DFFGEOMETRYITEMMODEL_H_
#define DFFGEOMETRYITEMMODEL_H_

#include <gtaformats/dff/DFFGeometry.h>
#include <gtaformats/dff/DFFGeometryPart.h>
#include <QtCore/QAbstractItemModel>
#include <QtCore/QList>
#include <QtCore/QLinkedList>
#include <algorithm>

using std::find;


class DFFGeometryItemModel : public QAbstractItemModel {
	Q_OBJECT

public:
	class Item
	{
	public:
		Item(bool displayed) : displayed(displayed) {}
		virtual bool isPart() const = 0;
		bool isDisplayed() const { return displayed; }
		void setDisplayed(bool displayed) { this->displayed = displayed; }

	private:
		bool displayed;
	};

	class InternalDFFGeometry;

	class InternalDFFGeometryPart : public Item
	{
	public:
		InternalDFFGeometryPart(DFFGeometryPart* part, InternalDFFGeometry* geom)
				: Item(true), part(part), geom(geom) {}
		virtual bool isPart() const { return true; }
		DFFGeometryPart* getPart() { return part; }
		InternalDFFGeometry* getGeometry() { return geom; }

	private:
		DFFGeometryPart* part;
		InternalDFFGeometry* geom;
	};

	class InternalDFFGeometry : public Item
	{
	public:
		typedef QList<InternalDFFGeometryPart*>::iterator PartIterator;
		typedef QList<InternalDFFGeometryPart*>::const_iterator ConstPartIterator;

	public:
		InternalDFFGeometry(DFFGeometry* geom)
				: Item(false), geom(geom)
		{
			for (DFFGeometry::PartIterator it = geom->getPartBegin() ; it != geom->getPartEnd() ; it++) {
				parts << new InternalDFFGeometryPart(*it, this);
			}
		}
		DFFGeometry* getGeometry() { return geom; }
		virtual bool isPart() const { return false; }
		InternalDFFGeometryPart* getPart(int32_t idx) { return parts[idx]; }
		int32_t indexOf(InternalDFFGeometryPart* part) const
		{
			QList<InternalDFFGeometryPart*>::const_iterator it = find(parts.begin(), parts.end(), part);
			return it == parts.end() ? -1 : it - parts.begin();
		}
		int32_t getPartCount() const { return parts.size(); }
		PartIterator getPartBegin() { return parts.begin(); }
		PartIterator getPartEnd() { return parts.end(); }
		ConstPartIterator getPartBegin() const { return parts.begin(); }
		ConstPartIterator getPartEnd() const { return parts.end(); }

	private:
		DFFGeometry* geom;
		QList<InternalDFFGeometryPart*> parts;
	};

public:
	DFFGeometryItemModel(DFFMesh* mesh);
	QModelIndex getGeometryIndex(DFFGeometry* geom) const;
	QModelIndex getGeometryPartIndex(DFFGeometryPart* part);
	void getDisplayedGeometryParts(DFFGeometry* geom, QLinkedList<const DFFGeometryPart*>& parts);
	Qt::ItemFlags flags(const QModelIndex& index) const;
	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
	int rowCount(const QModelIndex& index = QModelIndex()) const;
	int columnCount(const QModelIndex& index) const { return 1; }
	QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const;
	QModelIndex parent(const QModelIndex& index) const;
	bool setData(const QModelIndex& index, const QVariant& data, int role = Qt::EditRole);

signals:
	void geometryPartDisplayStateChanged(DFFGeometryPart* part, bool displayed);
	void geometryDisplayStateChanged(DFFGeometry* geom, bool displayed);

private:
	QList<InternalDFFGeometry*> geoms;
};

#endif /* DFFGEOMETRYITEMMODEL_H_ */
