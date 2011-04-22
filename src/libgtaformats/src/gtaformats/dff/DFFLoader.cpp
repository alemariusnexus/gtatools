/*
	Copyright 2010-2011 David "Alemarius Nexus" Lerch

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

	Additional permissions are granted, which are listed in the file
	GPLADDITIONS.
 */

#include "DFFLoader.h"
#include "DFFGeometry.h"
#include "DFFGeometryPart.h"
#include "DFFFrame.h"
#include "DFFMaterial.h"
#include "DFFTexture.h"
#include <cstdlib>
#include <cstdio>
#include <utility>

using std::pair;


DFFLoader::DFFLoader()
		: verbose(false)
{

}

DFFLoader::~DFFLoader() {

}


int DFFLoader::parseSection(istream* stream, RwSectionHeader* parent, DFFLoadContext* context)
{
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
		char* skipBuf = new char[skipSize];
		stream->read(skipBuf, skipSize);
		delete[] skipBuf;
		readCount += skipSize;
	}

	return parent->size;
}


int DFFLoader::parseStruct(istream* stream, RwSectionHeader& structHeader, RwSectionHeader* parent,
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
			printf("  Frames[%d] :\n", numFrames);
		}

		DFFFrame** frames = new DFFFrame*[numFrames];
		int32_t* parentIndices = new int32_t[numFrames];

		for (int32_t i = 0 ; i < numFrames ; i++) {
			DFFFrame* frame = new DFFFrame;
			int32_t parentIdx;

			context->frameInternalIndexMap.insert(pair<int32_t, DFFFrame*>(i, frame));

			float rotData[9];
			float transData[3];

			stream->read((char*) rotData, 36);
			stream->read((char*) transData, 12);
			stream->read((char*) &parentIdx, 4);
			stream->read((char*) &frame->flags, 4);

			Matrix3* rot = new Matrix3(rotData);
			Vector3* trans = new Vector3(transData);

			frame->setRotation(rot);
			frame->setTranslation(trans);

			frames[i] = frame;
			parentIndices[i] = parentIdx;
			rc += 56;
		}

		for (int32_t i = 0 ; i < numFrames ; i++) {
			int pidx = parentIndices[i];

			if (pidx != -1) {
				frames[pidx]->addChild(frames[i]);
			} else {
				mesh->getRootFrame()->addChild(frames[i]);
			}
		}

		delete[] parentIndices;
		delete[] frames;
	} else if (parent->id == RW_SECTION_GEOMETRYLIST) {
		int32_t geometryCount;
		stream->read((char*) &geometryCount, 4);
		rc += 4;

		if (verbose) {
			printf("  Geometries[%d] :\n", geometryCount);
		}
	} else if (parent->id == RW_SECTION_GEOMETRY) {
		//DFFGeometry* geom = new DFFGeometry;
		//mesh->geometries[context->nextGeometryIndex++] = geom;

		if (verbose) {
			printf("    Geometry %d :\n", mesh->getGeometryCount());
		}

		DFFGeometryStructHeader header;
		stream->read((char*) &header, sizeof(DFFGeometryStructHeader));
		rc += sizeof(DFFGeometryStructHeader);

		int32_t vertexCount = header.vertexCount;
		float* vertices = new float[vertexCount*3];
		int8_t uvSetCount = 0;
		uint8_t* vertexColors = NULL;
		float* uvCoordSets = NULL;
		DFFBoundingSphere* bounds = new DFFBoundingSphere;
		float* normals = NULL;

		//DFFGeometry* geom = new DFFGeometry(header.vertexCount, vertices);

		if (	(header.flags & GEOMETRY_FLAG_TEXCOORDS) != 0
				||  (header.flags & GEOMETRY_FLAG_MULTIPLEUVSETS) != 0
		) {
			uvSetCount = header.uvSetCount;
		}

		if (verbose) {
			printHeaderInfo(header, context);
		}

		if (context->version < DFF_VERSION_GTAVC_2) {
			stream->read(skipBuf, 12);
		}

		if ((header.flags & GEOMETRY_FLAG_COLORS) != 0) {
			int size = vertexCount * 4;
			vertexColors = new uint8_t[size];
			stream->read((char*) vertexColors, size);
			rc += size;
		}

		if (	(header.flags & GEOMETRY_FLAG_TEXCOORDS) != 0
				||  (header.flags & GEOMETRY_FLAG_MULTIPLEUVSETS) != 0
		) {
			int size = uvSetCount * vertexCount * 2 * sizeof(float);
			uvCoordSets = new float[size];
			stream->read((char*) uvCoordSets, size);
			rc += size;
		}

		char* tmpSkipBuf = new char[header.faceCount*sizeof(int16_t)*4];
		stream->read(tmpSkipBuf, header.faceCount * sizeof(int16_t) * 4);
		delete[] tmpSkipBuf;
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

		stream->read((char*) bounds, 16);
		rc += 16;

		stream->read(skipBuf, 8);
		rc += 8;

		if ((header.flags & GEOMETRY_FLAG_POSITIONS) != 0) {
			int size = sizeof(float) * vertexCount * 3;
			stream->read((char*) vertices, size);
			rc += size;
		} else {
			throw DFFException("This geometry states it doesn't have any vertices. How is that possible?",
					__FILE__, __LINE__);
		}

		if ((header.flags & GEOMETRY_FLAG_NORMALS) != 0) {
			normals = new float[vertexCount * 3];
			int size = sizeof(float) * vertexCount * 3;
			stream->read((char*) normals, size);
			rc += size;
		}

		DFFGeometry* geom = new DFFGeometry(vertexCount, vertices, normals, uvCoordSets, uvSetCount,
				vertexColors);
		geom->setFlags(header.flags);
		geom->setFrameCount(header.frameCount);
		geom->setBounds(bounds);
		mesh->addGeometry(geom);
	} else if (parent->id == RW_SECTION_MATERIALLIST) {
		//DFFGeometry* geom = mesh->getGeometry(mesh->getGeometryCount()-1);
		int32_t materialCount;
		stream->read((char*) &materialCount, 4);
		rc += 4;

		if (verbose) {
			printf("      Materials[%d] :\n", materialCount);
		}
	} else if (parent->id == RW_SECTION_MATERIAL) {
		DFFGeometry* geom = mesh->getGeometry(mesh->getGeometryCount()-1);
		DFFMaterial* mat = new DFFMaterial;

		int32_t textureCount;

		uint8_t* color = mat->getColor();

		stream->read(skipBuf, 4);
		stream->read((char*) color, 4);
		stream->read(skipBuf, 4);
		stream->read((char*) &textureCount, 4);
		rc += 16;

		geom->addMaterial(mat);

		if (verbose) {
			printf("        Material %d :\n", geom->getMaterialCount());
			printf("          Color: #%02X%02X%02X%02X\n", color[0], color[1], color[2], color[3]);
			printf("          Textures[%d] :\n", textureCount);
		}
	} else if (parent->id == RW_SECTION_TEXTURE) {
		DFFGeometry* geom = mesh->getGeometry(mesh->getGeometryCount()-1);
		DFFMaterial* mat = geom->getMaterial(geom->getMaterialCount()-1);
		DFFTexture* tex = new DFFTexture(NULL);
		stream->read((char*) &tex->filterModeFlags, 2);
		mat->addTexture(tex);
		rc += 2;

		if (verbose) {
			printf("            Texture %d :\n", mat->getTextureCount());
			printf("              Filter flags: %d\n", tex->filterModeFlags);
		}
	} else if (parent->id == RW_SECTION_ATOMIC) {
		int32_t frameIdx, geomIdx;
		stream->read((char*) &frameIdx, 4);
		stream->read((char*) &geomIdx, 4);
		rc += 8;
		mesh->getGeometry(geomIdx)->setAssociatedFrame(context->frameInternalIndexMap.find(frameIdx)->second);
	}

	return rc;
}


int DFFLoader::parseString(istream* stream, RwSectionHeader& stringHeader, RwSectionHeader* parent,
		DFFLoadContext* context)
{
	char* str = new char[stringHeader.size];
	stream->read(str, stringHeader.size);

	if (parent->id == RW_SECTION_TEXTURE) {
		DFFMesh* mesh = context->mesh;
		DFFGeometry* geom = mesh->getGeometry(mesh->getGeometryCount()-1);
		DFFMaterial* mat = geom->getMaterial(geom->getMaterialCount()-1);
		DFFTexture* tex = mat->getTexture(mat->getTextureCount()-1);

		if (tex->getDiffuseName() == NULL) {
			tex->setDiffuseName(str);

			if (verbose) {
				printf("              Diffuse name: %s\n", str);
			}
		} else {
			tex->setAlphaName(str);

			if (verbose) {
				printf("              Alpha name: %s\n", str);
			}
		}
	} else {
		delete[] str;
	}

	return stringHeader.size;
}


int DFFLoader::parseFrame(istream* stream, RwSectionHeader& frameHeader, RwSectionHeader* parent,
		DFFLoadContext* context)
{
	DFFFrame* frame = context->frameInternalIndexMap[context->frameCurrentIndex++];
	char* name = new char[frameHeader.size+1];
	stream->read(name, frameHeader.size);
	name[frameHeader.size] = '\0';
	frame->setName(name);

	if (verbose) {
		printf("    Frame :\n");
		printf("      Name : %s\n", name == NULL ? "[none]" : name);
	}

	return frameHeader.size;
}


int DFFLoader::parseMaterialSplit(istream* stream, RwSectionHeader& matsplitHeader, RwSectionHeader* parent,
			DFFLoadContext* context)
{
	DFFGeometry* geom = context->mesh->getGeometry(context->mesh->getGeometryCount()-1);

	int readCount = 0;

	int32_t triangleStrip, splitCount, faceCount;

	stream->read((char*) &triangleStrip, 4);
	stream->read((char*) &splitCount, 4);
	stream->read((char*) &faceCount, 4);
	readCount += 12;

	if (triangleStrip == 1) {
		geom->setFlags(geom->getFlags() | GEOMETRY_FLAG_TRISTRIP);
	} else {
		geom->setFlags(geom->getFlags() & ~GEOMETRY_FLAG_TRISTRIP);
	}

	if (verbose) {
		printf("      Face format: triangle %s\n", triangleStrip == 1 ? "strip" : "list");
		printf("      Submesh count (material splits): %d\n", splitCount);
		printf("      Face index count: %d\n", faceCount);
	}


	for (int32_t i = 0 ; i < splitCount ; i++) {
		int32_t faceIdx, matIdx;

		stream->read((char*) &faceIdx, 4);
		stream->read((char*) &matIdx, 4);
		readCount += 8;

		int size = faceIdx * 4;
		int32_t* indices = new int32_t[faceIdx];
		stream->read((char*) indices, size);
		readCount += size;

		DFFGeometryPart* part = new DFFGeometryPart(faceIdx, indices);
		geom->addPart(part);

		part->setMaterial(geom->getMaterial(matIdx));
	}

	return readCount;
}










DFFMesh* DFFLoader::loadMesh(const File& file)
{
	//InputStream* stream = file.openStream(STREAM_BINARY);
	istream* stream = file.openInputStream(istream::binary);
	DFFMesh* mesh = loadMesh(stream);
	delete stream;
	return mesh;
}


DFFMesh* DFFLoader::loadMesh(istream* stream) {
	/*if (stream->fail()) {
		throw DFFException(DFFException::IOError, "failbit was set in the given DFF stream");
	}*/

	//istream::iostate exState = stream->exceptions();

	DFFMesh* mesh = new DFFMesh;
	DFFLoadContext context;
	context.mesh = mesh;
	context.depth = 0;
	context.frameCurrentIndex = 0;

	RwSectionHeader clump;

	while (!stream->eof()) {
		RwReadSectionHeader(stream, clump);

		if (clump.id != RW_SECTION_CLUMP) {
			char expected[64], found[64];
			RwGetSectionName(RW_SECTION_CLUMP, expected);
			RwGetSectionName(clump.id, found);
			char* errmsg = new char[256];
			sprintf(errmsg, "Found section with type %s where %s was expected (is it really a DFF file?)",
					found, expected);
			DFFException ex(errmsg, __FILE__, __LINE__);
			delete[] errmsg;
			throw ex;
			/*char* skipBuf = new char[clump.size];
			stream->read(skipBuf, clump.size);
			delete[] skipBuf;*/
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




