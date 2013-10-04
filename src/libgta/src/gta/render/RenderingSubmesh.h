#ifndef RENDERINGSUBMESH_H_
#define RENDERINGSUBMESH_H_

#include <gta/config.h>
#include <GL/glew.h>
#include "../resource/texture/TextureSource.h"


class RenderingMesh;


class RenderingSubmesh
{
public:
	RenderingSubmesh(RenderingMesh* mesh, int indexCount, GLuint indexOffset, GLuint texture);
	int getIndexCount() const { return indexCount; }
	GLuint getIndexOffset() const { return indexOffset; }
	GLuint getTexture() const { return texture; }
	void setMaterialColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
			{ materialColor[0] = r; materialColor[1] = g; materialColor[2] = b; materialColor[3] = a; }
	void getMaterialColor(uint8_t& r, uint8_t& g, uint8_t& b, uint8_t& a)
			{ r = materialColor[0]; g = materialColor[1]; b = materialColor[2]; a = materialColor[3]; }

	~RenderingSubmesh()
	{
		if (texSrc) {
			texSrc->release();
			delete texSrc;
		}
	}

private:
	int indexCount;
	GLuint indexOffset;
	GLuint texture;
	uint8_t materialColor[4];

	TextureSource* texSrc;


	friend class RenderingEntityGenerator;
};

#endif /* RENDERINGSUBMESH_H_ */
