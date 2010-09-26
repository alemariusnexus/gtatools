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
#include "../FileOpenRequest.h"
#include <QSortFilterProxyModel>



class FileTree : public QTreeView {
	Q_OBJECT

public:
	FileTree(QWidget* parent = NULL);
	~FileTree();

private:
	void showProfile(Profile* profile);
	void reload();

private slots:
	void fileSelected(const QModelIndex& index);
	void contextMenuRequested(const QPoint& pos);
	void currentProfileChanged(Profile* oldProfile, Profile* newProfile);
	void currentProfileContentChanged();
	void fileOpened(const FileOpenRequest& request);

private:
	QSortFilterProxyModel* proxyModel;
	FileItemModel* model;
};

#endif /* FILETREE_H_ */
