/*
	Copyright 2010-2014 David "Alemarius Nexus" Lerch

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

#ifndef CEGUIGL2RENDERER_H_
#define CEGUIGL2RENDERER_H_

#include <CEGUI/CEGUI.h>
#include <vector>
#include "../ShaderProgram.h"
#include "CEGUIGL2GeometryBuffer.h"
#include "CEGUIGL2TextureTarget.h"
#include "CEGUIGL2Texture.h"
#include "CEGUIGL2ViewportTarget.h"

using CEGUI::RenderingRoot;
using CEGUI::GeometryBuffer;
using CEGUI::TextureTarget;
using CEGUI::String;
using CEGUI::Size;
using CEGUI::RenderTarget;
using CEGUI::Vector2;
using CEGUI::BlendMode;
using std::vector;


class CEGUIGL2Renderer : public CEGUI::Renderer {
private:
	typedef vector<CEGUIGL2GeometryBuffer*> GeomBufList;
	typedef vector<CEGUIGL2TextureTarget*> TexTargetList;
	typedef vector<CEGUIGL2Texture*> TexList;

public:
	static CEGUIGL2Renderer& bootstrapSystem();

public:
	CEGUIGL2Renderer();
	virtual RenderingRoot& getDefaultRenderingRoot();
	virtual GeometryBuffer& createGeometryBuffer();
	virtual void destroyGeometryBuffer(const GeometryBuffer& buf);
	virtual void destroyAllGeometryBuffers();
	virtual TextureTarget* createTextureTarget();
	virtual void destroyTextureTarget(TextureTarget* target);
	virtual void destroyAllTextureTargets();
	virtual CEGUI::Texture& createTexture();
	virtual CEGUI::Texture& createTexture(const String& fileName, const String& resGroup);
	virtual CEGUI::Texture& createTexture(const Size& size);
	virtual void destroyTexture(CEGUI::Texture& tex);
	virtual void destroyAllTextures();
	virtual void beginRendering();
	virtual void endRendering();
	virtual void setDisplaySize(const Size& size);
	virtual const Size& getDisplaySize() const;
	virtual const Vector2& getDisplayDPI() const;
	virtual uint getMaxTextureSize() const;
	virtual const String& getIdentifierString() const;
	CEGUIGL2RenderTarget* getActiveRenderTarget() { return activeTarget; }
	void setupBlendMode(BlendMode mode, bool force = false);

private:
	void setActiveRenderTarget(CEGUIGL2RenderTarget* target);
	void destroyGeometryBuffer(GeomBufList::iterator it);
	void destroyTextureTarget(TexTargetList::iterator it);
	void destroyTexture(TexList::iterator it);

private:
	ShaderProgram* program;
	CEGUIGL2ViewportTarget* rootTarget;
	RenderingRoot* root;
	GeomBufList geomBufs;
	TexTargetList texTargets;
	TexList texes;
	Size displaySize;
	Vector2 displayDPI;
	GLint maxTexSize;
	String idStr;
	CEGUIGL2RenderTarget* activeTarget;
	BlendMode blendMode;

	GLint mvpMatrixUniform, texturedUniform, textureUniform;
	GLint vertexAttrib, texCoordAttrib, colorAttrib;

private:
	friend class CEGUIGL2GeometryBuffer;
	friend class CEGUIGL2RenderTarget;
};

#endif /* CEGUIGL2RENDERER_H_ */
