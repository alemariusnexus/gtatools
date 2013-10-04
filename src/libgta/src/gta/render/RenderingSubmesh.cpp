#include "RenderingSubmesh.h"
#include "RenderingMesh.h"



RenderingSubmesh::RenderingSubmesh(RenderingMesh* mesh, int indexCount, GLuint indexOffset, GLuint texture)
			: indexCount(indexCount), indexOffset(indexOffset), texture(texture), texSrc(NULL)
{
	materialColor[0] = 255;
	materialColor[1] = 255;
	materialColor[2] = 255;
	materialColor[3] = 255;

	mesh->addSubmesh(this);
}
