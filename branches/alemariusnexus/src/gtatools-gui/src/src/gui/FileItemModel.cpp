#include "FileItemModel.h"
#include "../formats/FormatManager.h"
#include <cstring>



FileItemModel::FileItemModel(Profile* profile, QObject* parent)
		: QAbstractItemModel(parent), profile(profile)
{
	rootFile = new StaticFile;
	Profile::ResourceIterator it;

	for (it = profile->getResourceBegin() ; it != profile->getResourceEnd() ; it++) {
		rootFile->addRootChild(new StaticFile(**it));
	}

	/*rootFile = new StaticFile("null");

	StaticFile* root = new StaticFile("root", rootFile);
		StaticFile* root1 = new StaticFile("root.1", root);
			StaticFile* root11 = new StaticFile("root.1.1", root1);
				StaticFile* root111 = new StaticFile("root.1.1.1", root11);
			StaticFile* root12 = new StaticFile("root.1.2", root1);
				StaticFile* root121 = new StaticFile("root.1.2.1", root12);
			StaticFile* root13 = new StaticFile("root.1.3", root1);
				StaticFile* root131 = new StaticFile("root.1.3.1", root13);
		StaticFile* root2 = new StaticFile("root.2", root);
			StaticFile* root21 = new StaticFile("root.2.1", root2);
			StaticFile* root22 = new StaticFile("root.2.2", root2);*/
}


QModelIndex FileItemModel::indexOf(const File& file, const QModelIndex& start)
{
	int rc = rowCount(start);

	for (int i = 0 ; i < rc ; i++) {
		//QModelIndex child = start.child(i, 0);
		QModelIndex child = index(i, 0, start);

		StaticFile* sf = static_cast<StaticFile*>(child.internalPointer());

		if (*sf->getFile() == file) {
			return child;
		} else if (file.isChildOf(*sf->getFile())) {
			return indexOf(file, child);
		}
	}

	return QModelIndex();
}


File* FileItemModel::getFileForIndex(const QModelIndex& index)
{
	return static_cast<StaticFile*>(index.internalPointer())->getFile();
}


int FileItemModel::rowCount(const QModelIndex& parent) const
{
	if (parent.column() > 0) {
		return 0;
	}

	StaticFile* parentFile;

	if (parent.isValid()) {
		parentFile = static_cast<StaticFile*>(parent.internalPointer());
	} else {
		parentFile = rootFile;
	}

	return parentFile->getChildCount();
}


Qt::ItemFlags FileItemModel::flags(const QModelIndex& index) const
{
	if (!index.isValid()) {
		return 0;
	}

	return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}


QVariant FileItemModel::headerData(int section, Qt::Orientation orient, int role) const
{
	if (orient != Qt::Horizontal) {
		return QVariant();
	}

	if (role == Qt::DisplayRole) {
		switch (section) {
		case 0:
			return tr("Filename");
			break;
		case 1:
			return tr("File Type");
			break;
		}
	}

	return QVariant();
}


QVariant FileItemModel::data(const QModelIndex& index, int role) const
{
	if (!index.isValid()) {
		return QVariant();
	}
	if (role != Qt::DisplayRole) {
		return QVariant();
	}

	StaticFile* file = static_cast<StaticFile*>(index.internalPointer());

	switch (index.column()) {
	case 0:
		return QVariant(file->toString());
		break;
	case 1:
		File* realFile = file->getFile();

		if (realFile->isDirectory()) {
			return tr("Directory");
		} else {
			FormatHandler* handler = FormatManager::getInstance()->getHandler(*realFile, FORMAT_HANDLER_DESCRIBE, true);

			if (handler) {
				return handler->getFileFormatName(*realFile, true);
			} else {
				return tr("Unrecognized File");
			}
		}
	}
}


QModelIndex FileItemModel::index(int row, int column, const QModelIndex& parent) const
{
	if (!hasIndex(row, column, parent)) {
		return QModelIndex();
	}

	StaticFile* parentFile;

	if (parent.isValid()) {
		parentFile = static_cast<StaticFile*>(parent.internalPointer());
	} else {
		parentFile = rootFile;
	}

	return createIndex(row, column, parentFile->getChild(row));
}


QModelIndex FileItemModel::parent(const QModelIndex& index) const
{
	if (!index.isValid()) {
		return QModelIndex();
	}

	StaticFile* file = static_cast<StaticFile*>(index.internalPointer());
	StaticFile* parent = file->getParent();

	if (parent == rootFile) {
		return QModelIndex();
	}

	return createIndex(parent->getParent()->indexOf(parent), 0, parent);
}

