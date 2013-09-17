/*
	Copyright 2010-2013 David "Alemarius Nexus" Lerch

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

#include "COLLoader.h"
#include "COLException.h"
#include "COLSphere.h"
#include "COLBox.h"
#include "COLFace.h"
#include "COLBounds.h"
#include <cstdio>
#include "../util/stream/StreamReader.h"
#include "../util/stream/EndianSwappingStreamReader.h"
#include "../util/stream/StreamWriter.h"
#include "../util/stream/EndianSwappingStreamWriter.h"

using std::streamoff;



COLModel* COLLoader::loadModel(istream* stream)
{
#ifdef GTAFORMATS_LITTLE_ENDIAN
	StreamReader reader(stream);
#else
	EndianSwappingStreamReader reader(stream);
#endif

	char skipBuf[4];

	streamoff colStart = stream->tellg();
    //InputStream::streampos colStart = stream->tell();

	COLVersion version = getVersion(stream);

	if (version == ColEnd) {
		return NULL;
	}

	COLModel* model = new COLModel;

	uint32_t fileSize = reader.readU32();

	stream->read(model->name, 20);

	// Unknown, maybe part of the name, but IMG entry names are only 20 bytes without .col extension
	// TODO But maybe these bytes can be used for COL files outside an IMG archive?
	stream->read(skipBuf, 4);

	if (version == COL1) {
		model->bounds.radius = reader.readFloat();
		reader.readArrayFloat((float*) &model->bounds.center, 3);
		reader.readArrayFloat((float*) &model->bounds.vmin, 6); // min and max
	} else {
		reader.readArrayFloat((float*) &model->bounds, 10);
	}

	if (version == COL1) {
		uint32_t numSpheres, numUnknown1, numBoxes, numVertices, numFaces;

		reader.readU32(&numSpheres);
		COLSphere* spheres = new COLSphere[numSpheres];

		stream->read((char*) spheres, numSpheres*20);

		// Unfortunately, radius and center are swapped in COL1
		for (uint32_t i = 0 ; i < numSpheres ; i++) {
			COLSphere& sphere = spheres[i];
			float rad = sphere.center.getX();
			sphere.center = Vector3(FromLittleEndianF32(sphere.center.getY()),
					FromLittleEndianF32(sphere.center.getZ()), FromLittleEndianF32(sphere.radius));
			sphere.radius = FromLittleEndianF32(rad);
		}

		reader.readU32(&numUnknown1); // Should always be 0

		reader.readU32(&numBoxes);
		COLBox* boxes = new COLBox[numBoxes];
		stream->read((char*) boxes, numBoxes*28);

		reader.readU32(&numVertices);
		float* vertices = new float[numVertices*3];
		reader.readArrayFloat(vertices, numVertices*3);

		reader.readU32(&numFaces);
		COLFace* faces = new COLFace[numFaces];
		stream->read((char*) faces, numFaces*16);

#ifndef GTAFORMATS_LITTLE_ENDIAN
		for (uint32_t i = 0 ; i < numBoxes ; i++) {
			COLBox& box = boxes[i];
			box.vmax = Vector3(SwapEndiannessF32(box.vmax.getX()), SwapEndiannessF32(box.vmax.getY()),
					SwapEndiannessF32(box.vmax.getZ()));
			box.vmin = Vector3(SwapEndiannessF32(box.vmin.getX()), SwapEndiannessF32(box.vmin.getY()),
					SwapEndiannessF32(box.vmin.getZ()));
		}
		for (uint32_t i = 0 ; i < numFaces ; i++) {
			COLFace& face = faces[i];
			face.indices[0] = SwapEndianness32(face.indices[0]);
			face.indices[1] = SwapEndianness32(face.indices[1]);
			face.indices[2] = SwapEndianness32(face.indices[2]);
		}
#endif

		model->numSpheres = numSpheres;
		model->spheres = spheres;

		model->numBoxes = numBoxes;
		model->boxes = boxes;

		model->numVertices = numVertices;
		model->vertices = vertices;

		model->numFaces = numFaces;
		model->faces = faces;

		model->numFaceGroups = 0;
		model->faceGroups = NULL;

		model->flags = 0;
	} else {
		uint16_t numSpheres, numBoxes;
		uint32_t numFaces, flags;
		uint32_t sphereOffset, boxOffset, unknownOffset1, vertexOffset, faceOffset, unknownOffset2;
		uint32_t numShadowMeshVertices = 0, numShadowMeshFaces = 0, shadowMeshVertexOffset,
				shadowMeshFaceOffset;

		reader.readU16(&numSpheres);
		reader.readU16(&numBoxes);
		reader.readU32(&numFaces);
		reader.readU32(&flags);
		reader.readU32(&sphereOffset);
		reader.readU32(&boxOffset);
		reader.readU32(&unknownOffset1);
		reader.readU32(&vertexOffset);
		reader.readU32(&faceOffset);
		reader.readU32(&unknownOffset2);

		if (version == COL3) {
			reader.readU32(&numShadowMeshFaces);
			reader.readU32(&shadowMeshVertexOffset);
			reader.readU32(&shadowMeshFaceOffset);
		}

		stream->seekg(colStart+sphereOffset+4, istream::beg);
		COLSphere* spheres = new COLSphere[numSpheres];
		stream->read((char*) spheres, numSpheres*20);

		stream->seekg(colStart+boxOffset+4, istream::beg);
		COLBox* boxes = new COLBox[numBoxes];
		stream->read((char*) boxes, numBoxes*28);

		stream->seekg(colStart+faceOffset+4, istream::beg);
		COLFace* faces = new COLFace[numFaces];

#ifndef GTAFORMATS_LITTLE_ENDIAN
		for (uint32_t i = 0 ; i < numSpheres ; i++) {
			COLSphere& sphere = spheres[i];
			sphere.center = Vector3(SwapEndiannessF32(sphere.center.getX()),
					SwapEndiannessF32(sphere.center.getY()), SwapEndiannessF32(sphere.center.getZ()));
			sphere.radius = SwapEndiannessF32(sphere.radius);
		}
		for (uint32_t i = 0 ; i < numBoxes ; i++) {
			COLBox& box = boxes[i];
			box.vmax = Vector3(SwapEndiannessF32(box.vmax.getX()), SwapEndiannessF32(box.vmax.getY()),
					SwapEndiannessF32(box.vmax.getZ()));
			box.vmin = Vector3(SwapEndiannessF32(box.vmin.getX()), SwapEndiannessF32(box.vmin.getY()),
					SwapEndiannessF32(box.vmin.getZ()));
		}
#endif

		uint32_t greatestVertexIndex = 0;

		for (uint32_t i = 0 ; i < numFaces ; i++) {
			COLFace& face = faces[i];
			face.surface.brightness = 0;
			face.surface.flags = 0;
			uint16_t indices[3];
			reader.readArrayU16(indices, 3);
			stream->read((char*) &face.surface.material, 1);
			stream->read((char*) &face.surface.light, 1);
			face.indices[0] = indices[0];
			face.indices[1] = indices[1];
			face.indices[2] = indices[2];

			if (face.indices[0] > greatestVertexIndex)
				greatestVertexIndex = face.indices[0];
			if (face.indices[1] > greatestVertexIndex)
				greatestVertexIndex = face.indices[1];
			if (face.indices[2] > greatestVertexIndex)
				greatestVertexIndex = face.indices[2];
		}

		uint32_t numVertices = numFaces == 0 ? 0 : greatestVertexIndex+1;

		stream->seekg(colStart+vertexOffset+4, istream::beg);
		int16_t* compressedVertices = new int16_t[numVertices*3];
		reader.readArray16(compressedVertices, numVertices*3);

		float* vertices = new float[numVertices*3];

		for (uint32_t i = 0 ; i < numVertices ; i++) {
			vertices[i*3] = compressedVertices[i*3] / 128.0f;
			vertices[i*3+1] = compressedVertices[i*3+1] / 128.0f;
			vertices[i*3+2] = compressedVertices[i*3+2] / 128.0f;
		}

		delete[] compressedVertices;

		uint32_t numFaceGroups = 0;

		COLFaceGroup* faceGroups = NULL;

		if ((flags & COLModel::FlagFaceGroups) != 0) {
			stream->seekg(colStart+faceOffset, istream::beg);
			reader.readU32(&numFaceGroups);

			faceGroups = new COLFaceGroup[numFaceGroups];
			stream->seekg((streamoff) stream->tellg() - numFaceGroups*28, istream::beg);
			stream->read((char*) faceGroups, numFaceGroups*28);

#ifndef GTAFORMATS_LITTLE_ENDIAN
			for (uint32_t i = 0 ; i < numFaceGroups ; i++) {
				COLFaceGroup& group = faceGroups[i];
				group.vmin = Vector3(SwapEndiannessF32(group.vmin.getX()),
						SwapEndiannessF32(group.vmin.getY()), SwapEndiannessF32(group.vmin.getZ()));
				group.vmax = Vector3(SwapEndiannessF32(group.vmax.getX()),
						SwapEndiannessF32(group.vmax.getY()), SwapEndiannessF32(group.vmax.getZ()));
				group.startFace = SwapEndianness16(group.startFace);
				group.endFace = SwapEndianness16(group.endFace);
			}
#endif
		}

		COLShadowMesh* shadowMesh = NULL;

		if (version == COL3) {
			if ((flags & COLModel::FlagShadowMesh) != 0) {
				COLFace* shadowMeshFaces = new COLFace[numShadowMeshFaces];
				stream->seekg(colStart+shadowMeshFaceOffset+4, istream::beg);

				int64_t greatestVertexIndex = -1;

				for (uint32_t i = 0 ; i < numShadowMeshFaces ; i++) {
					COLFace& face = shadowMeshFaces[i];
					face.surface.brightness = 0;
					face.surface.flags = 0;
					uint16_t indices[3];
					reader.readArrayU16(indices, 3);
					stream->read((char*) &face.surface.material, 1);
					stream->read((char*) &face.surface.light, 1);
					face.indices[0] = indices[0];
					face.indices[1] = indices[1];
					face.indices[2] = indices[2];

					if (face.indices[0] > greatestVertexIndex)
						greatestVertexIndex = face.indices[0];
					if (face.indices[1] > greatestVertexIndex)
						greatestVertexIndex = face.indices[1];
					if (face.indices[2] > greatestVertexIndex)
						greatestVertexIndex = face.indices[2];
				}

				numShadowMeshVertices = greatestVertexIndex+1;

				stream->seekg(colStart+shadowMeshVertexOffset+4, istream::beg);
				int16_t* compressedVertices = new int16_t[numShadowMeshVertices*3];
				reader.readArray16(compressedVertices, numShadowMeshVertices*3);

				float* shadowMeshVertices = new float[numShadowMeshVertices*3];

				for (uint32_t i = 0 ; i < numShadowMeshVertices ; i++) {
					shadowMeshVertices[i*3] = compressedVertices[i*3] / 128.0f;
					shadowMeshVertices[i*3+1] = compressedVertices[i*3+1] / 128.0f;
					shadowMeshVertices[i*3+2] = compressedVertices[i*3+2] / 128.0f;
				}

				delete[] compressedVertices;

				shadowMesh = new COLShadowMesh(numShadowMeshVertices, numShadowMeshFaces, shadowMeshVertices,
						shadowMeshFaces);
			}
		}

		// Actually, all known flags aren't needed after loading, but some unknown ones may be useful in the
		// future. COLModel doesn't update any flags when they actually should be changed, so we just clear
		// them now so that nobody accidentally uses them to query these properties.
		flags &= ~(COLModel::FlagFaceGroups | COLModel::FlagNotEmpty | COLModel::FlagShadowMesh);

		model->numSpheres = numSpheres;
		model->spheres = spheres;

		model->numBoxes = numBoxes;
		model->boxes = boxes;

		model->numFaces = numFaces;
		model->faces = faces;

		model->flags = flags;

		model->numVertices = numVertices;
		model->vertices = vertices;

		model->numFaceGroups = numFaceGroups;
		model->faceGroups = faceGroups;

		model->shadowMesh = shadowMesh;
	}

	stream->seekg(colStart + fileSize + 8, istream::beg);

	return model;
}


COLModel* COLLoader::loadModel(const File& file)
{
	//BufferedInputStream bis(file.openStream(STREAM_BINARY));
	//return loadModel(&bis);
	//InputStream* stream = file.openStream(STREAM_BINARY);
	istream* stream = file.openInputStream(istream::binary);
	COLModel* model = loadModel(stream);
	delete stream;
	return model;
}


bool COLLoader::loadModelName(istream* stream, char* name)
{
	if (getVersion(stream) == ColEnd) {
		return false;
	}

	uint32_t size;

	stream->read((char*) &size, 4);
	size = FromLittleEndian32(size);
	stream->read(name, 20);

	char* skipBuf = new char[size-20];
	stream->read(skipBuf, size-20);
	delete[] skipBuf;

	return true;
}


bool COLLoader::loadModelName(const File& file, char* name)
{
	//InputStream* stream = file.openStream(STREAM_BINARY);
	istream* stream = file.openInputStream(istream::binary);
	bool retval = loadModelName(stream, name);
	delete stream;
	return retval;
}


void COLLoader::skip(istream* stream, int numEntries)
{
	for (int i = 0 ; i < numEntries ; i++) {
		COLVersion ver = getVersion(stream);

		if (ver == ColEnd) {
			break;
		}

		uint32_t size;
		stream->read((char*) &size, 4);
		size = FromLittleEndian32(size);
		stream->seekg(size, istream::cur);
	}
}


COLVersion COLLoader::getVersion(istream* stream)
{
	if (stream->eof()) {
		return ColEnd;
	}

	char fourCC[4];
	stream->read(fourCC, 4);

	if (stream->gcount() == 0) {
		return ColEnd;
	}

	COLVersion version;

	if (fourCC[0] == 'C'  &&  fourCC[1] == 'O'  &&  fourCC[2] == 'L') {
		if (fourCC[3] == 'L') {
			version = COL1;
		} else if (fourCC[3] == '2') {
			version = COL2;
		} else if (fourCC[3] == '3') {
			version = COL3;
		} else {
			/*char errmsg[64];
			sprintf(errmsg, "Invalid COL version in header: %c%c%c%c", fourCC[0], fourCC[1], fourCC[2],
					fourCC[3]);
			throw COLException(errmsg, __FILE__, __LINE__);*/

			return ColEnd;
		}
	} else {
		/*char errmsg[64];
		sprintf(errmsg, "File header not recognized as COL object: %c%c%c%c", fourCC[0], fourCC[1],
				fourCC[2], fourCC[3]);
		throw COLException(errmsg, __FILE__, __LINE__);*/
		return ColEnd;
	}

	return version;
}


COLVersion COLLoader::getVersion(const File& file)
{
	//InputStream* stream = file.openStream(STREAM_BINARY);
	istream* stream = file.openInputStream(istream::binary);
	COLVersion version = getVersion(stream);
	delete stream;
	return version;
}
