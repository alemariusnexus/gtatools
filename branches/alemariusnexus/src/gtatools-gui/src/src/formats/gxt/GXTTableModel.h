/*
 * GXTTableModel.h
 *
 *  Created on: 26.09.2010
 *      Author: alemariusnexus
 */

#ifndef GXTTABLEMODEL_H_
#define GXTTABLEMODEL_H_

#include <QAbstractTableModel>
#include <QLinkedList>
#include <gtaformats/gxt/GXTTable.h>



class GXTTableModel : public QAbstractTableModel {
	Q_OBJECT

private:
	struct NamedGXTTable
	{
		GXTTable* table;
		QString name;
	};

public:
	GXTTableModel();
	void addGXTArchive(const QString& name, GXTTable* table);
	virtual int rowCount(const QModelIndex& parent = QModelIndex());
	virtual int columnCount(const QModelIndex& parent = QModelIndex());
	virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole);

private:
	QLinkedList<NamedGXTTable*> tables;
};

#endif /* GXTTABLEMODEL_H_ */
