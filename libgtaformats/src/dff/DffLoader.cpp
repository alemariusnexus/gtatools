/*
 * DffLoader.cpp
 *
 *  Created on: 03.01.2010
 *      Author: alemariusnexus
 */

#include "DffLoader.h"
#include "DFFGeometry.h"
#include "DFFGeometryPart.h"
#include "DFFFrame.h"
#include "DFFMaterial.h"
#include "DFFTexture.h"
#include <cstdlib>
#include <cstdio>


DffLoader::DffLoader()
		: verbose(false)
{

}

DffLoader::~DffLoader() {

}


int DffLoader::parseSection(istream* stream, RwSectionHeader* parent, DffLoadContext* context)
{
	char skipBuf[1024];

	int readCount = 0;

	while (readCount < parent->size) {
		RwSectionHeader header;
		stream->read((char*) &header, sizeof(RwSectionHeader));
		readCount += 12;

		/*for (int i = 0 ; i < context->depth ; i++) {
			printf("  ");
		}

		char sname[64];
		RwGetSectionName(header.id, sname);

		printf("%s\n", sname);*/

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
		SkipBytes(stream, skipSize, skipBuf, sizeof(skipBuf));
		readCount += skipSize;
	}

	return parent->size;
}


int DffLoader::parseStruct(istream* stream, RwSectionHeader& structHeader, RwSectionHeader* parent,
		DffLoadContext* context)
{
	char skipBuf[1024];

	int rc = 0;

	DffMesh* mesh = context->mesh;

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
	} else if (parent->id == RW_SECTION_GEOMETRY) {
		DFFGeometry* geom = new DFFGeometry;
		mesh->geometries[context->nextGeometryIndex++] = geom;

		DffGeometryStructHeader header;
		stream->read((char*) &header, sizeof(DffGeometryStructHeader));
		rc += sizeof(DffGeometryStructHeader);

		geom->vertexCount = header.vertexCount;

		if ((header.flags & GEOMETRY_FLAG_TEXCOORDS) != 0) {
			geom->uvSetCount = header.uvSetCount;
		}

		if (verbose) {
			printHeaderInfo(header);
		}

		if ((header.flags & GEOMETRY_FLAG_COLORS) != 0) {
			int size = geom->vertexCount * 4;
			geom->vertexColors = new uint8_t[size];
			stream->read((char*) geom->vertexColors, size);
			rc += size;
		}

		if ((header.flags & GEOMETRY_FLAG_TEXCOORDS) != 0) {
			geom->uvCoordSets = new float*[geom->uvSetCount];

			for (int8_t i = 0 ; i < geom->uvSetCount ; i++) {
				int size = geom->vertexCount * 2 * sizeof(float);
				float* uvSet = new float[size];
				stream->read((char*) uvSet, size);
				geom->uvCoordSets[i] = uvSet;
				rc += size;
			}
		}

		SkipBytes(stream, header.faceCount * sizeof(int16_t) * 4, skipBuf, sizeof(skipBuf));
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

		stream->read(skipBuf, 8);
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
	} else if (parent->id == RW_SECTION_MATERIAL) {
		DFFGeometry* geom = mesh->geometries[context->nextGeometryIndex-1];
		DFFMaterial* mat = new DFFMaterial;

		stream->read(skipBuf, 4);
		stream->read((char*) mat->color, 4);
		stream->read(skipBuf, 4);
		stream->read((char*) &mat->textureCount, 4);
		rc += 16;

		mat->textures = new DFFTexture*[mat->textureCount];
		geom->materials[context->nextMaterialIndex++] = mat;
		context->nextTextureIndex = 0;
	} else if (parent->id == RW_SECTION_TEXTURE) {
		DFFMaterial* mat = mesh->geometries[context->nextGeometryIndex-1]->materials[context->nextMaterialIndex-1];
		DFFTexture* tex = new DFFTexture;
		stream->read((char*) &tex->filterModeFlags, 2);
		mat->textures[context->nextTextureIndex++] = tex;
		rc += 2;
	}

	return rc;
}


int DffLoader::parseString(istream* stream, RwSectionHeader& stringHeader, RwSectionHeader* parent,
		DffLoadContext* context)
{
	char* str = new char[stringHeader.size];
	stream->read(str, stringHeader.size);

	if (parent->id == RW_SECTION_TEXTURE) {
		DFFTexture* tex = context->mesh->geometries[context->nextGeometryIndex-1]
		                                            ->materials[context->nextMaterialIndex-1]
		                                                        ->textures[context->nextTextureIndex-1];

		if (tex->diffuseName == NULL) {
			tex->diffuseName = str;
		} else {
			tex->alphaName = str;
		}
	}

	return stringHeader.size;
}


int DffLoader::parseFrame(istream* stream, RwSectionHeader& frameHeader, RwSectionHeader* parent,
		DffLoadContext* context)
{
	int idx = context->nextFrameIndex;

	DFFFrame** frames = context->mesh->frames;
	frames[idx]->name = new char[frameHeader.size+1];
	stream->read(frames[idx]->name, frameHeader.size);
	frames[idx]->name[frameHeader.size] = '\0';

	context->nextFrameIndex++;

	return frameHeader.size;
}


int DffLoader::parseMaterialSplit(istream* stream, RwSectionHeader& matsplitHeader, RwSectionHeader* parent,
			DffLoadContext* context)
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
		printf("  Face format: triangle %s\n", geom->triangleStrips ? "strip" : "list");
		printf("  Submesh count (material splits): %d\n", splitCount);
		printf("  Face index count: %d\n", faceCount);
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











DffMesh* DffLoader::loadMesh(istream* stream) {
	DffMesh* mesh = new DffMesh;
	DffLoadContext context;
	context.mesh = mesh;
	context.depth = 0;

	RwSectionHeader clump;

	char skipBuf[1024];

	while (!stream->eof()) {
		RwReadSectionHeader(stream, clump);

		if (clump.id != RW_SECTION_CLUMP) {
			SkipBytes(stream, clump.size, skipBuf, sizeof(skipBuf));
		} else {
			break;
		}
	}

	parseSection(stream, &clump, &context);

	return mesh;

	/*RwSectionHeader clump;
	readSectionHeaderWithID(stream, clump, RW_SECTION_CLUMP);

	if (verbose) {
		char version[64];
		RwGetVersionName(clump.version, version);
		printf("File version: %s\n", version);
	}

	skipSectionWithID(stream, RW_SECTION_STRUCT);
	//skipSectionWithID(stream, RW_SECTION_FRAMELIST);


	RwSectionHeader frameListHeader;
	readSectionHeaderWithID(stream, frameListHeader, RW_SECTION_FRAMELIST);

	//skipSectionHeaderWithID(stream, RW_SECTION_FRAMELIST);
	//skipSectionHeaderWithID(stream, RW_STRUCT);
	parseFrameList(stream, mesh, frameListHeader);


	skipSectionHeaderWithID(stream, RW_SECTION_GEOMETRYLIST);

	RwSectionHeader geomListStruct;
	readSectionHeaderWithID(stream, geomListStruct, RW_SECTION_STRUCT);

	int32_t geomCount;
	//ReadBytes(stream, &geomCount, 4);
	stream->read((char*) &geomCount, 4);

	if (geomCount <= 0) {
		printf("Error: Invalid geometry count: %d\n", geomCount);
		exit(1);
	}

	if (verbose) {
		printf("Mesh has %d geometries.\n", geomCount);
	}


	for (int i = 0 ; i < geomCount ; i++) {
		if (verbose) {
			printf("Geometry %d:\n", i+1);
		}

		parseGeometry(stream, mesh);
	}

	return mesh;*/
}


/*void DffLoader::parseFrameList(istream* stream, DffMesh* mesh, RwSectionHeader& frameListHeader)
{
	//skipSectionHeaderWithID(stream, RW_SECTION_STRUCT);
	RwSectionHeader structHeader;
	readSectionHeaderWithID(stream, structHeader, RW_SECTION_STRUCT);


	int32_t numFrames;
	stream->read((char*) &numFrames, 4);

	if (verbose) {
		printf("  Frame count: %d\n", numFrames);
	}

	DffFrame** frames = new DffFrame*[numFrames];

	for (int32_t i = 0 ; i < numFrames ; i++) {
		frames[i] = new DffFrame;
		stream->read((char*) frames[i], 56);
	}

	char skipBuf[1024];

	for (int32_t i = 0 ; i < numFrames ; i++) {
		int bytesRead = 0;
		RwSectionHeader extHeader;
		readSectionHeaderWithID(stream, extHeader, RW_SECTION_EXTENSION);

		while (true) {
			if (bytesRead >= extHeader.size) {
				break;
			}

			RwSectionHeader header;
			RwReadSectionHeader(stream, header);
			bytesRead += sizeof(header);

			if (header.id == RW_SECTION_FRAME) {
				frames[i]->name = new char[header.size+1];
				stream->read(frames[i]->name, header.size);
				frames[i]->name[header.size] = '\0';
			} else {
				SkipBytes(stream, header.size, skipBuf, sizeof(skipBuf));
			}

			bytesRead += header.size;
		}
	}

	mesh->frames = frames;
	mesh->frameCount = numFrames;
}


void DffLoader::parseGeometry(istream* stream, DffMesh* mesh) {
	char skipBuffer[128];
	int size;

	RwSectionHeader geometry;
	readSectionHeaderWithID(stream, geometry, RW_SECTION_GEOMETRY);

	int readCount = 0;

	RwSectionHeader geomStruct;
	readCount += readSectionHeaderWithID(stream, geomStruct, RW_SECTION_STRUCT);

	int readBefore = readCount;

	DffGeometryStructHeader header;
	stream->read((char*) &header, sizeof(DffGeometryStructHeader));
	readCount += sizeof(DffGeometryStructHeader);

	mesh->facesUseTriStrips = (header.flags & GEOMETRY_FLAG_TRISTRIP) != 0;


	mesh->vertexCount = header.vertexCount;

	if ((header.flags & GEOMETRY_FLAG_TEXCOORDS) != 0) {
		mesh->textureCoordinateSetCount = header.uvSetCount;
	}

	if (verbose) {
		printHeaderInfo(header);
	}

	if ((header.flags & GEOMETRY_FLAG_COLORS) != 0) {
		readCount += parseGeometryColors(stream, mesh, header);
	}

	if ((header.flags & GEOMETRY_FLAG_TEXCOORDS) != 0) {
		readCount += parseGeometryTexCoords(stream, mesh, header);
	}

	readCount += parseGeometryFaces(stream, mesh);

	mesh->bounds = new DffBoundingSphere;
	stream->read((char*) mesh->bounds, sizeof(DffBoundingSphere));
	readCount += sizeof(DffBoundingSphere);

	stream->read(skipBuffer, 8);
	readCount += 8;

	if ((header.flags & GEOMETRY_FLAG_POSITIONS) != 0) {
		readCount += parseGeometryPositions(stream, mesh, header);
	}

	if ((header.flags & GEOMETRY_FLAG_NORMALS) != 0) {
		readCount += parseGeometryNormals(stream, mesh, header);
	}


	printf("I have just read %d bytes\n", readCount - readBefore);

	SkipBytes(stream, geomStruct.size - (readCount - readBefore), skipBuffer, sizeof(skipBuffer));

	readCount += skipSectionHeaderWithID(stream, RW_SECTION_MATERIALLIST);

	RwSectionHeader h;
	readCount += readSectionHeaderWithID(stream, h, RW_SECTION_STRUCT);

	//readCount += ReadBytes(stream, &mesh->materialCount, 4);
	stream->read((char*) &mesh->materialCount, 4);
	readCount += 4;

	SkipBytes(stream, h.size - 4, skipBuffer, sizeof(skipBuffer));
	readCount += h.size - 4;

	mesh->materials = new DffMaterial*[mesh->materialCount];

	for (int32_t i = 0 ; i < mesh->materialCount ; i++) {
		if (verbose) {
			printf("  Material %d:\n", i+1);
		}

		RwSectionHeader materialHeader;
		readCount += readSectionHeaderWithID(stream, materialHeader, RW_SECTION_MATERIAL);
		int bytesReadBefore = readCount;

		readCount += skipSectionHeaderWithID(stream, RW_SECTION_STRUCT);

		DffMaterial* material = new DffMaterial;
		mesh->materials[i] = material;
		//readCount += ReadBytes(stream, material, sizeof(DffMaterial) - sizeof(DffTexture**));
		size = sizeof(DffMaterial) - sizeof(DffTexture**);
		stream->read((char*) material, size);
		readCount += size;

		material->textures = new DffTexture*[material->textureCount];

		if (verbose) {
			printf("    Texture count: %d\n", material->textureCount);
		}

		for (int32_t j = 0 ; j < material->textureCount ; j++) {
			if (verbose) {
				printf("    Texture %d:\n", j+1);
			}

			DffTexture* texture = new DffTexture;

			RwSectionHeader textureHeader;
			readCount += readSectionHeaderWithID(stream, textureHeader, RW_SECTION_TEXTURE);
			int bytesReadBeforeTexture = readCount;

			readCount += skipSectionHeaderWithID(stream, RW_SECTION_STRUCT);

			//readCount += ReadBytes(stream, &texture->filterModeFlags, sizeof(uint16_t));
			//readCount += SkipBytes(stream, 2);
			stream->read((char*) &texture->filterModeFlags, 2);
			stream->read(skipBuffer, 2);
			readCount += 4;

			RwSectionHeader strHeader;

			readCount += readSectionHeaderWithID(stream, strHeader, RW_SECTION_STRING);
			texture->textureName = new char[strHeader.size];
			//readCount += ReadBytes(stream, texture->textureName, strHeader.size);
			stream->read((char*) texture->textureName, strHeader.size);
			readCount += strHeader.size;

			readCount += readSectionHeaderWithID(stream, strHeader, RW_SECTION_STRING);
			texture->maskName = new char[strHeader.size];
			//readCount += ReadBytes(stream, texture->maskName, strHeader.size);
			stream->read((char*) texture->maskName, strHeader.size);
			readCount += strHeader.size;

			material->textures[j] = texture;

			if (verbose) {
				printf("      Diffuse name: %s\n", texture->textureName);
				printf("      Alpha mask name: %s\n", texture->maskName);
			}

			//readCount += SkipBytes(stream, textureHeader.size - (readCount - bytesReadBeforeTexture));
			size = textureHeader.size - (readCount - bytesReadBeforeTexture);
			SkipBytes(stream, size, skipBuffer, sizeof(skipBuffer));
			//stream->read(skipBuffer, size);
			readCount += size;
		}

		//readCount += SkipBytes(stream, materialHeader.size - (readCount - bytesReadBefore));
		size = materialHeader.size - (readCount - bytesReadBefore);
		//stream->read(skipBuffer, size);
		SkipBytes(stream, size, skipBuffer, sizeof(skipBuffer));
		readCount += size;
	}

	readCount += skipSectionHeaderWithID(stream, RW_SECTION_EXTENSION);

	RwSectionHeader materialSplit;
	readCount += readSectionHeaderWithID(stream, materialSplit, RW_SECTION_MATERIALSPLIT);

	int32_t triangleStrip, splitCount, faceCount;

	stream->read((char*) &triangleStrip, 4);
	stream->read((char*) &splitCount, 4);
	stream->read((char*) &faceCount, 4);
	readCount += 12;

	delete[] mesh->faceIndices;
	mesh->facesUseTriStrips = (triangleStrip == 1);
	mesh->faceIndexCount = faceCount;
	mesh->faceIndices = new int32_t[mesh->faceIndexCount];
	int absFaceIdx = 0;

	if (verbose) {
		printf("  Face format: triangle %s\n", mesh->facesUseTriStrips ? "strip" : "list");
		printf("  Submesh count (material splits): %d\n", splitCount);
		printf("  Face index count: %d\n", mesh->faceIndexCount);
	}

	mesh->submeshCount = splitCount;
	mesh->submeshFaceIndexCounts = new int32_t[mesh->submeshCount];

	for (int i = 0 ; i < splitCount ; i++) {
		int32_t faceIdx, matIdx;

		stream->read((char*) &faceIdx, 4);
		stream->read((char*) &matIdx, 4);
		readCount += 8;

		size = faceIdx * sizeof(int32_t);
		stream->read((char*) mesh->faceIndices + absFaceIdx, size);
		readCount += size;

		absFaceIdx += faceIdx;

		mesh->submeshFaceIndexCounts[i] = faceIdx;
	}

	printf("Gonna skip %d bytes\n", geometry.size - readCount);

	//SkipBytes(stream, geometry.size - readCount);
	SkipBytes(stream, geometry.size - readCount, skipBuffer, sizeof(skipBuffer));
	printf("Now at %d\n", (int) stream->tellg());
	//stream->read(skipBuffer, geometry.size - readCount);
}*/





void DffLoader::printHeaderInfo(DffGeometryStructHeader& header)
{
	printf("  Vertex positions: %s\n", ((header.flags & GEOMETRY_FLAG_POSITIONS) != 0)
			? "yes" : "no");
	printf("  Texture coordinates: %s\n", ((header.flags & GEOMETRY_FLAG_TEXCOORDS) != 0)
			? "yes" : "no");
	printf("  Normals: %s\n", ((header.flags & GEOMETRY_FLAG_NORMALS) != 0)
			? "yes" : "no");
	printf("  Vertex colors: %s\n", ((header.flags & GEOMETRY_FLAG_COLORS) != 0)
			? "yes" : "no");
	printf("  Vertex count: %d\n", header.vertexCount);
	printf("  Texture coordinate sets: %d\n", header.uvSetCount);
}




