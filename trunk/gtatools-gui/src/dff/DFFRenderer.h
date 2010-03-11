/*
 * DFFRenderer.h
 *
 *  Created on: 10.03.2010
 *      Author: alemariusnexus
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
