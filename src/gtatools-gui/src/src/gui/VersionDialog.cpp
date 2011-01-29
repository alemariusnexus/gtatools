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

#include "VersionDialog.h"
#include <GL/gl.h>
#include <gtaformats/config.h>
#include <cstdio>
#include <QGLContext>
#include <QGLFormat>



VersionDialog::VersionDialog(QWidget* parent)
		: QDialog(parent)
{
	ui.setupUi(this);

	// glGetString() needs an active rendering context, so we have to create a temporary QGLWidget to
	// receive OpenGL version information.
	QGLWidget glw;
	glw.makeCurrent();

	printf("%s\n", GTATOOLS_VERSION);
	ui.gtatoolsVersionLabel->setText(GTATOOLS_VERSION);
	ui.qtVersionLabel->setText(qVersion());
	ui.qtLinkVersionLabel->setText(QT_VERSION_STR);
	ui.glVendorLabel->setText((const char*) glGetString(GL_VENDOR));
	ui.glRendererLabel->setText((const char*) glGetString(GL_RENDERER));
	ui.glVersionLabel->setText((const char*) glGetString(GL_VERSION));

	connect(ui.buttonBox, SIGNAL(clicked(QAbstractButton*)), this, SLOT(okClicked(QAbstractButton*)));
}


void VersionDialog::okClicked(QAbstractButton* button)
{
	accept();
}
