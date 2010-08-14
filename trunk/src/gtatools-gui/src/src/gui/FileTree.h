/*
 * FileTree.h
 *
 *  Created on: 14.08.2010
 *      Author: alemariusnexus
 */

#ifndef FILETREE_H_
#define FILETREE_H_

#include <qtreeview.h>
#include <qwidget.h>
#include <QModelIndex>
#include <QPoint>
#include "FileItemModel.h"
#include <QHash>
#include <QString>
#include <QVariant>



class FileTree : public QTreeView {
	Q_OBJECT

public:
	FileTree(QWidget* parent = NULL);

private:
	void showProfile(Profile* profile);
	void reload();

private slots:
	void fileSelected(const QModelIndex& index, const QModelIndex& prev);
	void contextMenuRequested(const QPoint& pos);
	void currentProfileChanged(Profile* oldProfile, Profile* newProfile);
	void currentProfileContentChanged();
	void fileOpened(const File& file, const QHash<QString, QVariant>& data);

private:
	FileItemModel* model;
};

#endif /* FILETREE_H_ */
