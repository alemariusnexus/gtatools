/*
 * GUI.h
 *
 *  Created on: 13.10.2010
 *      Author: alemariusnexus
 */

#ifndef GUI_H_
#define GUI_H_

#include <QtCore/QObject>
#include <QtCore/QLinkedList>
#include <QtGui/QWidget>
#include <gtaformats/util/FileFinder.h>
#include <gtaformats/util/File.h>




class GUI : public QObject {
	Q_OBJECT

public:
	static GUI* getInstance();

public:
	File* findFile(const QLinkedList<File*>& rootFiles, FileFinder* finder, QWidget* parent = NULL);
	File* findFile(FileFinder* finder, QWidget* parent = NULL);
	bool findAndOpenFile(const QLinkedList<File*>& rootFiles, FileFinder* finder, QWidget* parent = NULL);
	bool findAndOpenFile(FileFinder* finder, QWidget* parent = NULL);

private:
	GUI() {}
};

#endif /* GUI_H_ */
