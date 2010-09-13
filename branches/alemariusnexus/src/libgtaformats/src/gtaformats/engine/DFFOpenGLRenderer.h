/*
	Copyright 2010 David Lerch

	This file is part of gtaformats.

	gtaformats is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	gtaformats is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with gtaformats.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef DFFOPENGLRENDERER_H_
#define DFFOPENGLRENDERER_H_

#include "OpenGLResourceManager.h"
#include "../gtadff.h"


class DFFOpenGLRenderer
{
public:
	DFFOpenGLRenderer(OpenGLResourceManager* resourceManager);
	void renderGeometry(DFFGeometry* geom, bool transform = true);
	void renderGeometryPart(DFFGeometry* geom, DFFGeometryPart* part);
	void renderStaticMesh(DFFMesh* mesh);
	void renderDummy(DFFMesh* mesh, const char* dummyName, const char* geomName);
	void applyFrameTransformation(DFFFrame* frame);
	void applyTexture(const char*);

	void setTexturing(bool texturing) { this->texturing = texturing; }

	/*virtual void applyTexture(TXDTexture* texture, uint8_t* data);
	virtual void renderGeometryPart(DFFGeometry* geometry, DFFGeometryPart* part);
	virtual void applyFrameTransformation(DFFFrame* frame);
	virtual void endFrameTransformation(DFFFrame* frame);*/

private:
	OpenGLResourceManager* resourceManager;
	bool texturing;
};

#endif /* DFFOPENGLRENDERER_H_ */
