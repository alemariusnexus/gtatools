/*
 * VersionDialog.cpp
 *
 *  Created on: 13.08.2010
 *      Author: alemariusnexus
 */

#include "VersionDialog.h"
#include <GL/gl.h>
#include <gf_config.h>
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
}

