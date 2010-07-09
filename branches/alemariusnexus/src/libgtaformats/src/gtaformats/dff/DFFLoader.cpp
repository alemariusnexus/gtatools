/*
	Copyright 2010 David "Alemarius Nexus" Lerch

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

#include "DFFLoader.h"
#include "DFFGeometry.h"
#include "DFFGeometryPart.h"
#include "DFFFrame.h"
#include "DFFMaterial.h"
#include "DFFTexture.h"
#include <cstdlib>
#include <cstdio>
#include "../util/stream/FileInputStream.h"


DFFLoader::DFFLoader()
		: verbose(false)
{

}

DFFLoader::~DFFLoader() {

}


int DFFLoader::parseSection(InputStream* stream, RwSectionHeader* parent, DFFLoadContext* context)
{
	char skipBuf[1024];

	int readCount = 0;

	while (readCount < parent->size) {
		RwSectionHeader header;
		stream->read((char*) &header, sizeof(RwSectionHeader));
		readCount += 12;

		int readBefore = readCount;

		context->depth++;

		switch (header.id) {
		case RW_SECTION_CLUMP:
			readCount += parseSection(stream, &header, context);
			break;
		case RW_SECTION_FRAMELIST:
			readCount += parseSection(stream, &header, context);
			break;
		case RW_SECTION_EXTENSION:
			readCount += parseSection(stream, &header, context);
			break;
		case RW_SECTION_GEOMETRYLIST:
			readCount += parseSection(stream, &header, context);
			break;
		case RW_SECTION_GEOMETRY:
			readCount += parseSection(stream, &header, context);
			break;
		case RW_SECTION_MATERIALLIST:
			readCount += parseSection(stream, &header, context);
			break;
		case RW_SECTION_MATERIAL:
			readCount += parseSection(stream, &header, context);
			break;
		case RW_SECTION_TEXTURE:
			readCount += parseSection(stream, &header, context);
			break;
		case RW_SECTION_ATOMIC:
			readCount += parseSection(stream, &header, context);
			break;


		case RW_SECTION_STRING:
			readCount += parseString(stream, header, parent, context);
			break;
		case RW_SECTION_STRUCT:
			readCount += parseStruct(stream, header, parent, context);
			break;
		case RW_SECTION_FRAME:
			readCount += parseFrame(stream, header, parent, context);
			break;
		case RW_SECTION_MATERIALSPLIT:
			readCount += parseMaterialSplit(stream, header, parent, context);
			break;
		}

		context->depth--;

		int skipSize = header.size - (readCount - readBefore);
		//SkipBytes(stream, skipSize, skipBuf, sizeof(skipBuf));
		stream->skip(skipSize);
		readCount += skipSize;
	}

	return parent->size;
}


int DFFLoader::parseStruct(InputStream* stream, RwSectionHeader& structHeader, RwSectionHeader* parent,
		DFFLoadContext* context)
{
	char skipBuf[1024];

	int rc = 0;

	DFFMesh* mesh = context->mesh;

	if (parent->id == RW_SECTION_FRAMELIST) {
		int32_t numFrames;
		stream->read((char*) &numFrames, 4);
		rc += 4;

		if (verbose) {
			printf("  Frame count: %d\n", numFrames);
		}

		DFFFrame** frames = new DFFFrame*[numFrames];
		int32_t* parentIndices = new int32_t[numFrames];

		for (int32_t i = 0 ; i < numFrames ; i++) {
			DFFFrame* frame = new DFFFrame;
			int32_t parentIdx;

			stream->read((char*) frame->rotation, 36);
			stream->read((char*) frame->translation, 12);
			stream->read((char*) &parentIdx, 4);
			stream->read((char*) &frame->flags, 4);

			frames[i] = frame;
			parentIndices[i] = parentIdx;
			rc += 56;
		}

		for (int32_t i = 0 ; i < numFrames ; i++) {
			int pidx = parentIndices[i];

			if (pidx != -1) {
				frames[i]->parent = frames[pidx];
			} else {
				frames[i]->parent = NULL;
			}
		}

		delete[] parentIndices;

		mesh->frameCount = numFrames;
		mesh->frames = frames;
		context->nextFrameIndex = 0;
	} else if (parent->id == RW_SECTION_GEOMETRYLIST) {
		stream->read((char*) &mesh->geometryCount, 4);
		rc += 4;
		mesh->geometries = new DFFGeometry*[mesh->geometryCount];
		context->nextGeometryIndex = 0;

		if (verbose) {
			printf("  Geometries[%d] :\n", mesh->geometryCount);
		}
	} else if (parent->id == RW_SECTION_GEOMETRY) {
		DFFGeometry* geom = new DFFGeometry;
		mesh->geometries[context->nextGeometryIndex++] = geom;

		if (verbose) {
			printf("    Geometry %d :\n", context->nextGeometryIndex);
		}

		DFFGeometryStructHeader header;
		stream->read((char*) &header, sizeof(DFFGeometryStructHeader));
		rc += sizeof(DFFGeometryStructHeader);

		geom->vertexCount = header.vertexCount;

		if (	(header.flags & GEOMETRY_FLAG_TEXCOORDS) != 0
				||  (header.flags & GEOMETRY_FLAG_MULTIPLEUVSETS) != 0
		) {
			geom->uvSetCount = header.uvSetCount;
		} else {
			geom->uvSetCount = 0;
		}

		if (verbose) {
			printHeaderInfo(header, context);
		}

		if (context->version < DFF_VERSION_GTAVC_2) {
			stream->skip(12);
		}

		if ((header.flags & GEOMETRY_FLAG_COLORS) != 0) {
			int size = geom->vertexCount * 4;
			geom->vertexColors = new uint8_t[size];
			stream->read((char*) geom->vertexColors, size);
			rc += size;
		}

		if (	(header.flags & GEOMETRY_FLAG_TEXCOORDS) != 0
				||  (header.flags & GEOMETRY_FLAG_MULTIPLEUVSETS) != 0
		) {
			geom->uvCoordSets = new float*[geom->uvSetCount];

			for (int8_t i = 0 ; i < geom->uvSetCount ; i++) {
				int size = geom->vertexCount * 2 * sizeof(float);
				float* uvSet = new float[size];
				stream->read((char*) uvSet, size);
				geom->uvCoordSets[i] = uvSet;
				rc += size;
			}
		}

		//SkipBytes(stream, header.faceCount * sizeof(int16_t) * 4, skipBuf, sizeof(skipBuf));
		stream->skip(header.faceCount * sizeof(int16_t) * 4);
		rc += header.faceCount * sizeof(int16_t) * 4;

		/*int readCount = 0;

		mesh->faceIndexCount = header.faceCount*3;
		mesh->faceIndices = new int32_t[mesh->faceIndexCount];

		for (int32_t i = 0 ; i < header.faceCount ; i++) {
			int16_t face[4];
			//readCount += ReadBytes(stream, face, sizeof(face));
			stream->read((char*) face, sizeof(face));
			readCount += sizeof(face);

			mesh->faceIndices[i*3] = face[1];
			mesh->faceIndices[i*3 + 1] = face[0];
			mesh->faceIndices[i*3 + 2] = face[3];
		}*/

		stream->read((char*) &geom->bounds, 16);
		rc += 16;

		stream->skip(8);
		rc += 8;

		if ((header.flags & GEOMETRY_FLAG_POSITIONS) != 0) {
			geom->vertices = new float[geom->vertexCount * 3];
			int size = sizeof(float) * geom->vertexCount * 3;
			stream->read((char*) geom->vertices, size);
			rc += size;
		}

		if ((header.flags & GEOMETRY_FLAG_NORMALS) != 0) {
			geom->normals = new float[geom->vertexCount * 3];
			int size = sizeof(float) * geom->vertexCount * 3;
			stream->read((char*) geom->normals, size);
			rc += size;
		}
	} else if (parent->id == RW_SECTION_MATERIALLIST) {
		DFFGeometry* geom = mesh->geometries[context->nextGeometryIndex-1];
		stream->read((char*) &geom->materialCount, 4);
		rc += 4;
		geom->materials = new DFFMaterial*[geom->materialCount];
		context->nextMaterialIndex = 0;

		if (verbose) {
			printf("      Materials[%d] :\n", geom->materialCount);
		}
	} else if (parent->id == RW_SECTION_MATERIAL) {
		DFFGeometry* geom = mesh->geometries[context->nextGeometryIndex-1];
		DFFMaterial* mat = new DFFMaterial;

		stream->skip(4);
		stream->read((char*) mat->color, 4);
		stream->skip(4);
		stream->read((char*) &mat->textureCount, 4);
		rc += 16;

		mat->textures = new DFFTexture*[mat->textureCount];
		geom->materials[context->nextMaterialIndex++] = mat;
		context->nextTextureIndex = 0;

		if (verbose) {
			printf("        Material %d :\n", context->nextMaterialIndex);
			printf("          Color: #%02X%02X%02X%02X\n", mat->color[0], mat->color[1], mat->color[2],
					mat->color[3]);
			printf("          Textures[%d] :\n", mat->textureCount);
		}
	} else if (parent->id == RW_SECTION_TEXTURE) {
		DFFMaterial* mat = mesh->geometries[context->nextGeometryIndex-1]
		                                    ->materials[context->nextMaterialIndex-1];
		DFFTexture* tex = new DFFTexture;
		stream->read((char*) &tex->filterModeFlags, 2);
		mat->textures[context->nextTextureIndex++] = tex;
		rc += 2;

		if (verbose) {
			printf("            Texture %d :\n", context->nextTextureIndex);
			printf("              Filter flags: %d\n", tex->filterModeFlags);
		}
	} else if (parent->id == RW_SECTION_ATOMIC) {
		int32_t frameIdx, geomIdx;
		stream->read((char*) &frameIdx, 4);
		stream->read((char*) &geomIdx, 4);
		rc += 8;
		mesh->getGeometry(geomIdx)->associatedFrame = mesh->getFrame(frameIdx);
	}

	return rc;
}


int DFFLoader::parseString(InputStream* stream, RwSectionHeader& stringHeader, RwSectionHeader* parent,
		DFFLoadContext* context)
{
	char* str = new char[stringHeader.size];
	stream->read(str, stringHeader.size);

	if (parent->id == RW_SECTION_TEXTURE) {
		DFFTexture* tex = context->mesh->geometries[context->nextGeometryIndex-1]
		                                            ->materials[context->nextMaterialIndex-1]
		                                                        ->textures[context->nextTextureIndex-1];

		if (tex->diffuseName == NULL) {
			tex->diffuseName = str;

			if (verbose) {
				printf("              Diffuse name: %s\n", tex->diffuseName);
			}
		} else {
			tex->alphaName = str;

			if (verbose) {
				printf("              Alpha name: %s\n", tex->alphaName);
			}
		}
	}

	return stringHeader.size;
}


int DFFLoader::parseFrame(InputStream* stream, RwSectionHeader& frameHeader, RwSectionHeader* parent,
		DFFLoadContext* context)
{
	int idx = context->nextFrameIndex;

	DFFFrame** frames = context->mesh->frames;
	frames[idx]->name = new char[frameHeader.size+1];
	stream->read(frames[idx]->name, frameHeader.size);
	frames[idx]->name[frameHeader.size] = '\0';

	context->nextFrameIndex++;

	return frameHeader.size;
}


int DFFLoader::parseMaterialSplit(InputStream* stream, RwSectionHeader& matsplitHeader, RwSectionHeader* parent,
			DFFLoadContext* context)
{
	DFFGeometry* geom = context->mesh->geometries[context->nextGeometryIndex-1];

	int readCount = 0;

	int32_t triangleStrip, splitCount, faceCount;

	stream->read((char*) &triangleStrip, 4);
	stream->read((char*) &splitCount, 4);
	stream->read((char*) &faceCount, 4);
	readCount += 12;

	geom->partCount = splitCount;
	geom->parts = new DFFGeometryPart*[splitCount];

	geom->triangleStrips = (triangleStrip == 1);

	if (verbose) {
		printf("      Face format: triangle %s\n", geom->triangleStrips ? "strip" : "list");
		printf("      Submesh count (material splits): %d\n", splitCount);
		printf("      Face index count: %d\n", faceCount);
	}


	for (int32_t i = 0 ; i < splitCount ; i++) {
		DFFGeometryPart* part = new DFFGeometryPart;

		int32_t faceIdx, matIdx;

		stream->read((char*) &faceIdx, 4);
		stream->read((char*) &matIdx, 4);
		readCount += 8;

		int size = faceIdx * 4;
		part->indexCount = faceIdx;
		part->indices = new int32_t[faceIdx];
		stream->read((char*) part->indices, size);
		readCount += size;

		part->material = geom->materials[matIdx];

		geom->parts[i] = part;
	}

	return readCount;
}










DFFMesh* DFFLoader::loadMesh(const File& file)
{
	FileInputStream stream(file, STREAM_BINARY);
	DFFMesh* mesh = loadMesh(&stream);
	return mesh;
}


DFFMesh* DFFLoader::loadMesh(InputStream* stream) {
	/*if (stream->fail()) {
		throw DFFException(DFFException::IOError, "failbit was set in the given DFF stream");
	}*/

	//istream::iostate exState = stream->exceptions();

	DFFMesh* mesh = new DFFMesh;
	DFFLoadContext context;
	context.mesh = mesh;
	context.depth = 0;

	RwSectionHeader clump;

	char skipBuf[1024];

	while (!stream->hasReachedEnd()) {
		RwReadSectionHeader(stream, clump);

		if (clump.id != RW_SECTION_CLUMP) {
			//SkipBytes(stream, clump.size, skipBuf, sizeof(skipBuf));
			stream->skip(clump.size);
		} else {
			context.version = clump.version;
			break;
		}
	}

	if (verbose) {
		printf("===== DFF Mesh version 0x%X =====\n", clump.version);
	}

	parseSection(stream, &clump, &context);

	//stream->exceptions(exState);

	return mesh;
}





void DFFLoader::printHeaderInfo(DFFGeometryStructHeader& header, DFFLoadContext* context)
{
	printf("      Flags: %d\n", header.flags);
	printf("      Vertex positions: %s\n", ((header.flags & GEOMETRY_FLAG_POSITIONS) != 0)
			? "yes" : "no");
	printf("      Texture coordinates: ");

	if ((header.flags & GEOMETRY_FLAG_TEXCOORDS) != 0) {
		printf("yes\n");
	} else if ((header.flags & GEOMETRY_FLAG_MULTIPLEUVSETS) != 0) {
		printf("yes (%d sets)\n", header.uvSetCount);
	}

	printf("      Normals: %s\n", ((header.flags & GEOMETRY_FLAG_NORMALS) != 0)
			? "yes" : "no");
	printf("      Vertex colors: %s\n", ((header.flags & GEOMETRY_FLAG_COLORS) != 0)
			? "yes" : "no");
	printf("      Vertex count: %d\n", header.vertexCount);
	printf("      Texture coordinate sets: %d\n", header.uvSetCount);
}




