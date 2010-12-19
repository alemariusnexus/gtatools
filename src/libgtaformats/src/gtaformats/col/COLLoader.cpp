/*
 * COLLoader.cpp
 *
 *  Created on: 31.10.2010
 *      Author: alemariusnexus
 */

#include "COLLoader.h"
#include "COLException.h"
#include "COLSphere.h"
#include "COLBox.h"
#include "COLFace.h"
#include "COLBounds.h"
#include "../util/stream/BufferedInputStream.h"
#include <cstdio>



COLModel* COLLoader::loadModel(InputStream* stream)
{
	InputStream::streampos colStart = stream->tell();

	COLVersion version = getVersion(stream);

	if (version == ColEnd) {
		return NULL;
	}

	COLModel* model = new COLModel;

	uint32_t fileSize;
	stream->read((char*) &fileSize, 4);

	stream->read(model->name, 20);

	// Unknown, maybe part of the name, but IMG entry names are only 20 bytes without .col extension
	stream->skip(4);

	if (version == COL1) {
		stream->read((char*) &model->bounds.radius, 4);
		stream->read((char*) &model->bounds.center, 12);
		stream->read((char*) &model->bounds.min, 24); // min and max
	} else {
		stream->read((char*) &model->bounds, 40);
	}

	if (version == COL1) {
		uint32_t numSpheres, numUnknown1, numBoxes, numVertices, numFaces;

		stream->read((char*) &numSpheres, 4);
		COLSphere* spheres = new COLSphere[numSpheres];
		stream->read((char*) spheres, numSpheres*20);

		// Unfortunately, radius and center are swapped in COL1
		for (uint32_t i = 0 ; i < numSpheres ; i++) {
			COLSphere& sphere = spheres[i];
			float rad = sphere.center.getX();
			sphere.center = Vector3(sphere.center.getY(), sphere.center.getZ(), sphere.radius);
			sphere.radius = rad;
		}

		stream->read((char*) &numUnknown1, 4); // Should always be 0

		stream->read((char*) &numBoxes, 4);
		COLBox* boxes = new COLBox[numBoxes];
		stream->read((char*) boxes, numBoxes*28);

		stream->read((char*) &numVertices, 4);
		float* vertices = new float[numVertices*3];
		stream->read((char*) vertices, numVertices*12);

		stream->read((char*) &numFaces, 4);
		COLFace* faces = new COLFace[numFaces];
		stream->read((char*) faces, numFaces*16);

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

		stream->read((char*) &numSpheres, 2);
		stream->read((char*) &numBoxes, 2);
		stream->read((char*) &numFaces, 4);
		stream->read((char*) &flags, 4);
		stream->read((char*) &sphereOffset, 4);
		stream->read((char*) &boxOffset, 4);
		stream->read((char*) &unknownOffset1, 4);
		stream->read((char*) &vertexOffset, 4);
		stream->read((char*) &faceOffset, 4);
		stream->read((char*) &unknownOffset2, 4);

		if (version == COL3) {
			stream->read((char*) &numShadowMeshFaces, 4);
			stream->read((char*) &shadowMeshVertexOffset, 4);
			stream->read((char*) &shadowMeshFaceOffset, 4);
		}

		stream->seek(colStart+sphereOffset+4, InputStream::STREAM_SEEK_BEGIN);
		COLSphere* spheres = new COLSphere[numSpheres];
		stream->read((char*) spheres, numSpheres*20);

		stream->seek(colStart+boxOffset+4, InputStream::STREAM_SEEK_BEGIN);
		COLBox* boxes = new COLBox[numBoxes];
		stream->read((char*) boxes, numBoxes*28);

		stream->seek(colStart+faceOffset+4, InputStream::STREAM_SEEK_BEGIN);
		COLFace* faces = new COLFace[numFaces];

		int32_t greatestVertexIndex = 0;

		for (uint32_t i = 0 ; i < numFaces ; i++) {
			COLFace& face = faces[i];
			face.surface.brightness = 0;
			face.surface.flags = 0;
			uint16_t indices[3];
			stream->read((char*) indices, 6);
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

		stream->seek(colStart+vertexOffset+4, InputStream::STREAM_SEEK_BEGIN);
		int16_t* compressedVertices = new int16_t[numVertices*3];
		stream->read((char*) compressedVertices, numVertices*6);

		float* vertices = new float[numVertices*3];

		for (uint32_t i = 0 ; i < numVertices ; i++) {
			vertices[i*3] = compressedVertices[i*3] / 128.0f;
			vertices[i*3+1] = compressedVertices[i*3+1] / 128.0f;
			vertices[i*3+2] = compressedVertices[i*3+2] / 128.0f;
			//printf("    %f   %f   %f\n", vertices[0], vertices[1], vertices[2]);
		}

		delete[] compressedVertices;

		uint32_t numFaceGroups = 0;

		COLFaceGroup* faceGroups = NULL;

		if ((flags & COLModel::FlagFaceGroups) != 0) {
			stream->seek(colStart+faceOffset, InputStream::STREAM_SEEK_BEGIN);
			stream->read((char*) &numFaceGroups, 4);
			faceGroups = new COLFaceGroup[numFaceGroups];
			stream->seek(stream->tell()-numFaceGroups*28, InputStream::STREAM_SEEK_BEGIN);
			stream->read((char*) faceGroups, numFaceGroups*28);
		}

		COLShadowMesh* shadowMesh = NULL;

		if (version == COL3) {
			if ((flags & COLModel::FlagShadowMesh) != 0) {
				COLFace* shadowMeshFaces = new COLFace[numShadowMeshFaces];
				stream->seek(colStart+shadowMeshFaceOffset+4, InputStream::STREAM_SEEK_BEGIN);

				int32_t greatestVertexIndex = -1;

				for (uint32_t i = 0 ; i < numShadowMeshFaces ; i++) {
					COLFace& face = shadowMeshFaces[i];
					face.surface.brightness = 0;
					face.surface.flags = 0;
					uint16_t indices[3];
					stream->read((char*) indices, 6);
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

				stream->seek(colStart+shadowMeshVertexOffset+4, InputStream::STREAM_SEEK_BEGIN);
				int16_t* compressedVertices = new int16_t[numShadowMeshVertices*3];
				stream->read((char*) compressedVertices, numShadowMeshVertices*6);

				float* shadowMeshVertices = new float[numShadowMeshVertices*3];

				for (uint32_t i = 0 ; i < numShadowMeshVertices ; i++) {
					shadowMeshVertices[0] = compressedVertices[0] / 128.0f;
					shadowMeshVertices[1] = compressedVertices[1] / 128.0f;
					shadowMeshVertices[2] = compressedVertices[2] / 128.0f;
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

	stream->seek(colStart + fileSize + 8, InputStream::STREAM_SEEK_BEGIN);

	return model;
}


COLModel* COLLoader::loadModel(const File& file)
{
	//BufferedInputStream bis(file.openStream(STREAM_BINARY));
	//return loadModel(&bis);
	InputStream* stream = file.openStream(STREAM_BINARY);
	COLModel* model = loadModel(stream);
	delete stream;
	return model;
}


bool COLLoader::loadModelName(InputStream* stream, char* name)
{
	if (getVersion(stream) == ColEnd) {
		return false;
	}

	uint32_t size;

	stream->read((char*) &size, 4);
	stream->read(name, 20);

	stream->skip(size-20);

	return true;
}


bool COLLoader::loadModelName(const File& file, char* name)
{
	InputStream* stream = file.openStream(STREAM_BINARY);
	bool retval = loadModelName(stream, name);
	delete stream;
	return retval;
}


COLVersion COLLoader::getVersion(InputStream* stream)
{
	if (stream->hasReachedEnd()) {
		return ColEnd;
	}

	char fourCC[4];
	stream->read(fourCC, 4);

	if (stream->getLastReadCount() == 0) {
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
	InputStream* stream = file.openStream(STREAM_BINARY);
	COLVersion version = getVersion(stream);
	delete stream;
	return version;
}
