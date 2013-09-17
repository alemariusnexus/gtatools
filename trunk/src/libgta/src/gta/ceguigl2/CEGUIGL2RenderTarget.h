/*
	Copyright 2010-2013 David "Alemarius Nexus" Lerch

	This file is part of libgta.

	libgta is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	libgta is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with libgta.  If not, see <http://www.gnu.org/licenses/>.

	Additional permissions are granted, which are listed in the file
	GPLADDITIONS.
 */

#ifndef CEGUIGL2RENDERTARGET_H_
#define CEGUIGL2RENDERTARGET_H_

#include <CEGUI/CEGUI.h>
#include <gtaformats/util/math/Matrix4.h>
#include <gtaformats/util/util.h>

using CEGUI::GeometryBuffer;
using CEGUI::RenderQueue;
using CEGUI::Rect;
using CEGUI::Vector2;


class CEGUIGL2Renderer;



class CEGUIGL2RenderTarget : public virtual CEGUI::RenderTarget {
public:
	CEGUIGL2RenderTarget(CEGUIGL2Renderer* renderer);
	virtual void draw(const GeometryBuffer& geom);
	virtual void draw(const RenderQueue& queue);
	virtual void setArea(const Rect& area);
	virtual const Rect& getArea() const { return area; }
	virtual void activate();
	virtual void deactivate();
	virtual void unprojectPoint(const GeometryBuffer& geom, const Vector2& pin, Vector2& pout) const;
	const Matrix4& getProjectionMatrix() const { updateMatrix(); return pMatrix; }
	virtual const char* getName() const = 0;

private:
	void updateMatrix() const;

protected:
	CEGUIGL2Renderer* renderer;

private:
	Rect area;
	mutable Matrix4 pMatrix;
	mutable float viewDist;
	mutable bool pMatrixValid;
};

#endif /* CEGUIGL2RENDERTARGET_H_ */
