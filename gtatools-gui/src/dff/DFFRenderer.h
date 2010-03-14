/*
	Copyright 2010 David Lerch

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

#ifndef DFFRENDERER_H_
#define DFFRENDERER_H_

#include <gtaformats/gtadff.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <wx/wx.h>
#include <wx/glcanvas.h>


class DFFRenderer : public wxGLCanvas
{
public:
	DFFRenderer(wxWindow* parent, int* attribs);
	void setGeometry(DFFGeometry* geom);

	DECLARE_EVENT_TABLE()

private:
	void render(wxPaintEvent& evt);

private:
	DFFGeometry* geometry;
};

#endif /* DFFRENDERER_H_ */
