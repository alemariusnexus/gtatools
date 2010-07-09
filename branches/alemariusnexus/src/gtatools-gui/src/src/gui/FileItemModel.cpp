#include "FileItemModel.h"
#include <cstring>


FileItemModel::FileItemModel(Profile* profile, QObject* parent)
		: QAbstractItemModel(parent), profile(profile)
{
}


int FileItemModel::rowCount(const QModelIndex& parent) const
{
	if (parent.isValid()) {
		File* parentFile = static_cast<File*>(parent.internalPointer());
		return parentFile->getChildCount();
	} else {
		Profile::ResourceIterator it;
		int i = 0;

		for (it = profile->getResourceBegin() ; it != profile->getResourceEnd() ; it++, i++);

		return i;
	}
}


QVariant FileItemModel::data(const QModelIndex& index, int role) const
{
	if (role != Qt::DisplayRole) {
		return QVariant();
	}

	if (index.isValid()) {
		File* file = static_cast<File*>(index.internalPointer());
		return QVariant(file->getPath()->getFileName());
	} else {
		return QVariant(profile->getName());
	}
}


QModelIndex FileItemModel::index(int row, int column, const QModelIndex& parent) const
{
	if (!hasIndex(row, column, parent)) {
		return QModelIndex();
	}

	if (column >= 1) {
		return QModelIndex();
	}

	File* parentFile = NULL;

	if (parent.isValid()) {
		parentFile = static_cast<File*>(parent.internalPointer());
		return createIndex(row, column, static_cast<void*>(parentFile->getChild(row)));
	} else {
		File* file;
		Profile::ResourceIterator it = profile->getResourceBegin();

		for (int i = 0 ; i <= row ; i++) {
			file = *it;
			it++;
		}

		return createIndex(row, column, static_cast<void*>(file));
	}
}


QModelIndex FileItemModel::parent(const QModelIndex& index) const
{
	if (!index.isValid()) {
		fprintf(stderr, "[parent end0]\n");
		return QModelIndex();
	}

	File* file = static_cast<File*>(index.internalPointer());

	if (file->getParent() == NULL) {
		return QModelIndex();
	}

	/*Profile::ResourceIterator it;

	for (it = profile->getResourceBegin() ; it != profile->getResourceEnd() ; it++) {
		File* resource = *it;

		if (strcmp(file->getPath()->toString(), resource->getPath()->toString()) == 0) {
			return QModelIndex();
		}
	}*/

	return createIndex(file->getDirectoryIndex(), 0, static_cast<void*>(file->getParent()));
}

