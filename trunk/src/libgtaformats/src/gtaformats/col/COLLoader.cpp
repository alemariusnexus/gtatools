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

#include "COLLoader.h"
#include "COLException.h"
#include "COLSphere.h"
#include "COLBox.h"
#include "COLFace.h"
#include "COLBounds.h"
#include <cstdio>

using std::streamoff;



COLModel* COLLoader::loadModel(istream* stream)
{
	char skipBuf[4];

	streamoff colStart = stream->tellg();
    //InputStream::streampos colStart = stream->tell();

	COLVersion version = getVersion(stream);

	if (version == ColEnd) {
		return NULL;
	}

	COLModel* model = new COLModel;

	uint32_t fileSize;
	stream->read((char*) &fileSize, 4);

	stream->read(model->name, 20);

	// Unknown, maybe part of the name, but IMG entry names are only 20 bytes without .col extension
	stream->read(skipBuf, 4);

	if (version == COL1) {
		stream->read((char*) &model->bounds.radius, 4);
		stream->read((char*) &model->bounds.center, 12);
		stream->read((char*) &model->bounds.vmin, 24); // min and max
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

		stream->seekg(colStart+sphereOffset+4, istream::beg);
		COLSphere* spheres = new COLSphere[numSpheres];
		stream->read((char*) spheres, numSpheres*20);

		stream->seekg(colStart+boxOffset+4, istream::beg);
		COLBox* boxes = new COLBox[numBoxes];
		stream->read((char*) boxes, numBoxes*28);

		stream->seekg(colStart+faceOffset+4, istream::beg);
		COLFace* faces = new COLFace[numFaces];

		uint32_t greatestVertexIndex = 0;

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

		stream->seekg(colStart+vertexOffset+4, istream::beg);
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
			stream->seekg(colStart+faceOffset, istream::beg);
			stream->read((char*) &numFaceGroups, 4);
			faceGroups = new COLFaceGroup[numFaceGroups];
			stream->seekg((streamoff) stream->tellg() - numFaceGroups*28, istream::beg);
			stream->read((char*) faceGroups, numFaceGroups*28);
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

				stream->seekg(colStart+shadowMeshVertexOffset+4, istream::beg);
				int16_t* compressedVertices = new int16_t[numShadowMeshVertices*3];
				stream->read((char*) compressedVertices, numShadowMeshVertices*6);

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
