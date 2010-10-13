/*
	Copyright 2010 David "Alemarius Nexus" Lerch

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
 */

#ifndef DFFFORMATHANDLER_H_
#define DFFFORMATHANDLER_H_

#include "../FormatHandler.h"
#include <QtOpenGL/qgl.h>
#include <gtaformats/dff/DFFMesh.h>
#include <gtaformats/dff/DFFFrame.h>
#include <QtCore/QTextStream>
#include <QtGui/QWidget>


class DFFFormatHandler: public FormatHandler {
	Q_OBJECT

public:
	static DFFFormatHandler* getInstance();

public:
	virtual QString getFormatName(const File* file = NULL) const { return tr("DFF Mesh"); }
	virtual QLinkedList<QString> getFileFormatExtensions() const { return QLinkedList<QString>() << "dff"; }
	virtual bool hasFileFormat(const File& file) const { return file.guessContentType() == CONTENT_TYPE_DFF; }
	virtual QWidget* createWidgetForFile(const FileOpenRequest& request, QWidget* parent);

	void xmlDumpDialog(const DFFMesh& mesh, QWidget* parent);

private:
	DFFFormatHandler();
	void xmlDumpFrame(const DFFFrame* frame, QTextStream& xml, int indLevel);

private:
	QGLWidget* shareWidget;
};

#endif /* DFFFORMATHANDLER_H_ */
