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
#ifndef CEGUIGL2GEOMETRYBUFFER_H_
#define CEGUIGL2GEOMETRYBUFFER_H_

#include <CEGUI/CEGUI.h>
#include "CEGUIGL2Texture.h"
#include <gtaformats/util/math/Matrix4.h>
#include <vector>

using CEGUI::Rect;
using CEGUI::Vertex;
using CEGUI::RenderEffect;
using CEGUI::uint;
using CEGUI::BlendMode;
using std::vector;


class CEGUIGL2Renderer;



class CEGUIGL2GeometryBuffer : public CEGUI::GeometryBuffer {
private:
	struct CEGUIGL2Vertex
	{
		float v[3];
		float tc[2];
		uint8_t c[4];
	};

	typedef vector<CEGUIGL2Vertex> VertexList;

	struct CEGUIGL2Batch
	{
		CEGUIGL2Batch(GLuint dataBuf, CEGUIGL2Texture* tex) : dataBuf(dataBuf), tex(tex) {}
		VertexList vertices;
		GLuint dataBuf;
		CEGUIGL2Texture* tex;
	};

	typedef vector<CEGUIGL2Batch> BatchList;

public:
	CEGUIGL2GeometryBuffer(CEGUIGL2Renderer* renderer);
	virtual ~CEGUIGL2GeometryBuffer();
	virtual void draw() const;
	virtual void setTranslation(const CEGUI::Vector3& t);
	virtual void setRotation(const CEGUI::Vector3& r);
	virtual void setPivot(const CEGUI::Vector3& p);
	virtual void setClippingRegion(const Rect& r);
	virtual void appendVertex(const Vertex& v);
	virtual void appendGeometry(const Vertex* const vbuf, uint vcount);
	virtual void setActiveTexture(CEGUI::Texture* tex);
	virtual void reset();
	virtual CEGUI::Texture* getActiveTexture() const { return activeTex; }
	virtual uint getVertexCount() const { return vertexCount; }
	virtual uint getBatchCount() const { return batches.size(); }
	virtual void setRenderEffect(RenderEffect* eff) { effect = eff; }
	virtual RenderEffect* getRenderEffect() { return effect; }
	const Matrix4& getMatrix() const { return mvMatrix; }
	const CEGUI::Vector3& getTranslation() const { return trans; }

private:
	void updateMVMatrix() const;
	void updateBatches(uint vcountAdd = 0);

private:
	CEGUIGL2Renderer* renderer;
	CEGUI::Vector3 trans;
	CEGUI::Vector3 rot;
	CEGUI::Vector3 pivot;
	CEGUIGL2Texture* activeTex;
	mutable Matrix4 mvMatrix;
	mutable bool mvMatrixValid;
	Rect clipRegion;
	BatchList batches;
	RenderEffect* effect;
	uint vertexCount;
};

#endif /* CEGUIGL2GEOMETRYBUFFER_H_ */
