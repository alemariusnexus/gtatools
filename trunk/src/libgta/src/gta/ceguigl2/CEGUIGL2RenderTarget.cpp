/*
	Copyright 2010-2011 David "Alemarius Nexus" Lerch

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

#include "CEGUIGL2RenderTarget.h"
#include "CEGUIGL2Renderer.h"
#include "../gl.h"
#include <gtaformats/util/math/project.h>
#include <gtaformats/util/math/Vector3.h>

#define TWO_TAN_HALF_YFOV 0.53589838f




CEGUIGL2RenderTarget::CEGUIGL2RenderTarget(CEGUIGL2Renderer* renderer)
		: renderer(renderer), area(0, 0, 0, 0), pMatrixValid(false)
{
}


void CEGUIGL2RenderTarget::draw(const GeometryBuffer& geom)
{
	geom.draw();
}


void CEGUIGL2RenderTarget::draw(const RenderQueue& queue)
{
	queue.draw();
}


void CEGUIGL2RenderTarget::setArea(const Rect& area)
{
	this->area = area;
	pMatrixValid = false;
}


void CEGUIGL2RenderTarget::activate()
{
	glViewport((GLsizei) area.d_left, (GLsizei) area.d_top,
			(GLsizei) area.getWidth(), (GLsizei) area.getHeight());
	updateMatrix();
	renderer->setActiveRenderTarget(this);
}


void CEGUIGL2RenderTarget::deactivate()
{
	renderer->setActiveRenderTarget(NULL);
}


void CEGUIGL2RenderTarget::unprojectPoint(const GeometryBuffer& geom, const Vector2& pin, Vector2& pout) const
{
	// TODO Implement
	// I still don't know what EXACTLY this method should do.

	CEGUI::Vector3 pos = ((const CEGUIGL2GeometryBuffer&) geom).getTranslation();
	pout = Vector2(pin.d_x - pos.d_x, pin.d_y - pos.d_y);
}


void CEGUIGL2RenderTarget::updateMatrix() const
{
	if (!pMatrixValid) {
		// We want to define a perspective viewing so that the visible range of X and Y coordinates is the
		// same as the area width and height when Z is 0. We'll calculate a view distance with which we
		// translate the camera so that we have this situation.

		float w = area.getWidth();
		float h = area.getHeight();

		float aspect = w/h;
		float halfX = w * 0.5f;
		float halfY = h * 0.5f;

		// Calculate the view distance which gives us a visible XY range of WH
		viewDist = h / TWO_TAN_HALF_YFOV;

		// Set the near and far plane to some value that includes viewDist.
		pMatrix = Matrix4::perspective(0.52359878f, aspect, viewDist*0.5f, viewDist*2.0f);

		// Now we abuse our projection matrix for viewing transformation. We set the camera so that positive
		// Z values range into the screen and that Y values start from the top. We also translate the camera
		// by -viewDist so that a Z value of 0 starts at viewDist.
		// We may use the projection matrix for viewing because the actual order of multiplication applied to
		// each vertex v is P*V*M*v, so we can take P*V as one matrix as we do here.
		Vector3 eye(halfX, halfY, -viewDist);
		Vector3 center(halfX, halfY, 1.0f);
		Vector3 up(0.0f, -1.0f, 0.0f);
		pMatrix *= Matrix4::lookAt(center-eye, up) * Matrix4::translation(-eye);

		pMatrixValid = true;
	}
}
