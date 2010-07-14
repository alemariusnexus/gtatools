#ifndef FILEITEMMODEL_H_
#define FILEITEMMODEL_H_

#include <QtCore/qabstractitemmodel.h>
#include <QtCore/qmap.h>
#include <QtCore/qlist.h>
#include "../Profile.h"
#include <gtaformats/util/File.h>
#include "StaticFile.h"


class FileItemModel : public QAbstractItemModel
{
	Q_OBJECT

public:
	FileItemModel(Profile* profile, QObject* parent = 0);
	virtual ~FileItemModel() {};

	int rowCount(const QModelIndex& parent = QModelIndex()) const;
	int columnCount(const QModelIndex& parent = QModelIndex()) const { return 2; }
	Qt::ItemFlags flags(const QModelIndex& index) const;
	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
	QVariant headerData(int section, Qt::Orientation orient, int role = Qt::DisplayRole) const;
	QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const;
	QModelIndex parent(const QModelIndex& parent) const;

private:
	Profile* profile;
	StaticFile* rootFile;
};

#endif /* FILEITEMMODEL_H_ */