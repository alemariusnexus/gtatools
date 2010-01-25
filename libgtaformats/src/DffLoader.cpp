/*
 * DffLoader.cpp
 *
 *  Created on: 03.01.2010
 *      Author: alemariusnexus
 */

#include "DffLoader.h"
#include <cstdlib>
#include <cstdio>


DffLoader::DffLoader() {

}

DffLoader::~DffLoader() {

}

DffMesh* DffLoader::loadMesh(istream* stream) {
	DffMesh* mesh = new DffMesh;

	RwSectionHeader clump;
	RwReadSectionHeaderWithID(stream, clump, RW_SECTION_CLUMP);

	if (verbose) {
		char version[64];
		RwGetVersionName(clump.version, version);
		printf("File version: %s\n", version);
	}

	RwSkipSectionWithID(stream, RW_SECTION_STRUCT);
	RwSkipSectionWithID(stream, RW_SECTION_FRAMELIST);

	RwSkipSectionHeaderWithID(stream, RW_SECTION_GEOMETRYLIST);

	RwSectionHeader geomListStruct;
	RwReadSectionHeaderWithID(stream, geomListStruct, RW_SECTION_STRUCT);

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

	return mesh;
}


void DffLoader::parseGeometry(istream* stream, DffMesh* mesh) {
	char skipBuffer[128];
	int size;

	RwSectionHeader geometry;
	RwReadSectionHeaderWithID(stream, geometry, RW_SECTION_GEOMETRY);

	//istream::pos_type start = stream.tellg();
	//gtastreampos start = GetStreamPosition(stream);
	int readCount = 0;

	RwSectionHeader geomStruct;
	readCount += RwReadSectionHeaderWithID(stream, geomStruct, RW_SECTION_STRUCT);

	DffGeometryStructHeader header;
	//readCount += ReadBytes(stream, &header, sizeof(DffGeometryStructHeader));
	stream->read((char*) &header, sizeof(DffGeometryStructHeader));
	readCount += sizeof(DffGeometryStructHeader);

	mesh->facesUseTriStrips = (header.flags & GEOMETRY_FLAG_TRISTRIP) != 0;


	mesh->vertexCount = header.vertexCount;

	if ((header.flags & GEOMETRY_FLAG_TEXCOORDS) != 0) {
		mesh->textureCoordinateSetCount = header.uvSetCount;
	}

	if (verbose) {
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

	if ((header.flags & GEOMETRY_FLAG_COLORS) != 0) {
		mesh->vertexColors = new uint8_t[mesh->vertexCount * 4];
		//readCount += ReadBytes(stream, mesh->vertexColors, sizeof(uint8_t) * mesh->vertexCount * 4);
		size = sizeof(uint8_t) * mesh->vertexCount * 4;
		stream->read((char*) mesh->vertexColors, size);
		readCount += size;
	}

	if ((header.flags & GEOMETRY_FLAG_TEXCOORDS) != 0) {
		size = mesh->textureCoordinateSetCount * mesh->vertexCount * 2 * sizeof(float);
		mesh->textureCoordinateSets = new float[size];
		//readCount += ReadBytes(stream, mesh->textureCoordinateSets, size);
		stream->read((char*) mesh->textureCoordinateSets, size);
		readCount += size;
	}

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
	}

	mesh->bounds = new DffBoundingSphere;
	//readCount += ReadBytes(stream, mesh->bounds, sizeof(DffBoundingSphere));
	stream->read((char*) mesh->bounds, sizeof(DffBoundingSphere));
	readCount += sizeof(DffBoundingSphere);

	//readCount += SkipBytes(stream, 8);
	stream->read(skipBuffer, 8);
	readCount += 8;

	if ((header.flags & GEOMETRY_FLAG_POSITIONS) != 0) {
		mesh->vertexPositions = new float[mesh->vertexCount * 3];
		//readCount += ReadBytes(stream, mesh->vertexPositions, sizeof(float) * mesh->vertexCount * 3);
		size = sizeof(float) * mesh->vertexCount * 3;
		stream->read((char*) mesh->vertexPositions, size);
		readCount += size;
	}

	if ((header.flags & GEOMETRY_FLAG_NORMALS) != 0) {
		mesh->vertexNormals = new float[mesh->vertexCount * 3];
		//readCount += ReadBytes(stream, mesh->vertexNormals, sizeof(float) * mesh->vertexCount * 3);
		size = sizeof(float) * mesh->vertexCount * 3;
		stream->read((char*) mesh->vertexNormals, size);
		readCount += size;
	}


	readCount += RwSkipSectionHeaderWithID(stream, RW_SECTION_MATERIALLIST);

	RwSectionHeader h;
	readCount += RwReadSectionHeaderWithID(stream, h, RW_SECTION_STRUCT);

	//readCount += ReadBytes(stream, &mesh->materialCount, 4);
	stream->read((char*) &mesh->materialCount, 4);
	readCount += 4;

	if (verbose) {
		printf("  Material count: %d\n", mesh->materialCount);
	}

	/*for (int i = h.size - 4 ; i > 0 ;) {
		int skip = i % sizeof(skipBuffer);
		skip = (skipBuffer == 0) ? sizeof(skipBuffer) : skipBuffer;
		stream->read(skipBuffer, skip);
		i -= skip;
	}*/

	SkipBytes(stream, h.size - 4, skipBuffer, sizeof(skipBuffer));
	readCount += h.size - 4;

	mesh->materials = new DffMaterial*[mesh->materialCount];

	for (int32_t i = 0 ; i < mesh->materialCount ; i++) {
		if (verbose) {
			printf("  Material %d:\n", i+1);
		}

		RwSectionHeader materialHeader;
		readCount += RwReadSectionHeaderWithID(stream, materialHeader, RW_SECTION_MATERIAL);
		int bytesReadBefore = readCount;

		readCount += RwSkipSectionHeaderWithID(stream, RW_SECTION_STRUCT);

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
			readCount += RwReadSectionHeaderWithID(stream, textureHeader, RW_SECTION_TEXTURE);
			int bytesReadBeforeTexture = readCount;

			readCount += RwSkipSectionHeaderWithID(stream, RW_SECTION_STRUCT);

			//readCount += ReadBytes(stream, &texture->filterModeFlags, sizeof(uint16_t));
			//readCount += SkipBytes(stream, 2);
			stream->read((char*) &texture->filterModeFlags, 2);
			stream->read(skipBuffer, 2);
			readCount += 4;

			RwSectionHeader strHeader;

			readCount += RwReadSectionHeaderWithID(stream, strHeader, RW_SECTION_STRING);
			texture->textureName = new char[strHeader.size];
			//readCount += ReadBytes(stream, texture->textureName, strHeader.size);
			stream->read((char*) texture->textureName, strHeader.size);
			readCount += strHeader.size;

			readCount += RwReadSectionHeaderWithID(stream, strHeader, RW_SECTION_STRING);
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
			stream->read(skipBuffer, size);
			readCount += size;
		}

		//readCount += SkipBytes(stream, materialHeader.size - (readCount - bytesReadBefore));
		size = materialHeader.size - (readCount - bytesReadBefore);
		stream->read(skipBuffer, size);
		readCount += size;
	}

	readCount += RwSkipSectionHeaderWithID(stream, RW_SECTION_EXTENSION);

	RwSectionHeader materialSplit;
	readCount += RwReadSectionHeaderWithID(stream, materialSplit, RW_SECTION_MATERIALSPLIT);

	int32_t triangleStrip, splitCount, faceCount;

	/*readCount += ReadBytes(stream, &triangleStrip, 4);
	readCount += ReadBytes(stream, &splitCount, 4);
	readCount += ReadBytes(stream, &faceCount, 4);*/
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

		/*readCount += ReadBytes(stream, &faceIdx, 4);
		readCount += ReadBytes(stream, &matIdx, 4);

		readCount += ReadBytes(stream, mesh->faceIndices + absFaceIdx, faceIdx * sizeof(int32_t));*/

		stream->read((char*) &faceIdx, 4);
		stream->read((char*) &matIdx, 4);
		readCount += 8;

		size = faceIdx * sizeof(int32_t);
		stream->read((char*) mesh->faceIndices + absFaceIdx, size);
		readCount += size;

		absFaceIdx += faceIdx;

		mesh->submeshFaceIndexCounts[i] = faceIdx;
	}

	//SkipBytes(stream, geometry.size - readCount);
	stream->read(skipBuffer, geometry.size - readCount);
}

