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
#include "DFFException.h"
#include "../util/util.h"
#include "../util/math/Matrix3.h"
#include "../util/math/Vector3.h"
#include "../gta.h"
#include <cstdio>
#include <cstdlib>



struct IndexedDFFFrame
{
	DFFFrame* frame;
	uint32_t parentIdx;
	int32_t boneID;
};





DFFMesh* DFFLoader::loadMesh(istream* stream)
{
	RWSection* sect = NULL;

	while ((sect = RWSection::readSection(stream, NULL))  !=  NULL) {
		if (sect->getID() == RW_SECTION_CLUMP) {
			break;
		}

		delete sect;
	}

	if (!sect)
		return NULL;

	DFFMesh* mesh = loadMesh(sect);
	delete sect;
	return mesh;
}


DFFMesh* DFFLoader::loadMesh(const File& file)
{
	istream* stream = file.openInputStream(istream::binary);
	DFFMesh* mesh = loadMesh(stream);
	delete stream;
	return mesh;
}


DFFMesh* DFFLoader::loadMesh(RWSection* clump)
{
	DFFMesh* mesh = new DFFMesh;

	// **************************************************
	// *				LOAD THE FRAMES					*
	// **************************************************

	RWSection* frameList = clump->getChild(RW_SECTION_FRAMELIST);

	RWSection* frameListStruct = frameList->getChild(RW_SECTION_STRUCT);
	uint8_t* frameListStructData = frameListStruct->getData();

	uint32_t numFrames = FromLittleEndian32(*((uint32_t*) frameListStructData));

	// In DFF, the frames are stored and indexed as a flat data structure, so at first we keep them flat.
	IndexedDFFFrame* indexedFrames = new IndexedDFFFrame[numFrames];

	map<int32_t, DFFBone*> bones;

	for (uint32_t i = 0 ; i < numFrames ; i++) {
		IndexedDFFFrame& indexedFrame = indexedFrames[i];
		indexedFrame.boneID = -1;
		uint8_t* offData = frameListStructData + 4 + i*56;

#ifdef GTAFORMATS_LITTLE_ENDIAN
		Matrix3* rotMatrix = new Matrix3((float*) offData);
		Vector3* posVector = new Vector3((float*) (offData + 9*4));
#else
		float* rData = (float*) offData;
		float* pData = (float*) (offData + 9*4);
		Matrix3* rotMatrix = new Matrix3(
				FromLittleEndianF32(rData[0]), FromLittleEndianF32(rData[1]), FromLittleEndianF32(rData[2]),
				FromLittleEndianF32(rData[3]), FromLittleEndianF32(rData[4]), FromLittleEndianF32(rData[5]),
				FromLittleEndianF32(rData[6]), FromLittleEndianF32(rData[7]), FromLittleEndianF32(rData[8])
				);
		Vector3* posVector = new Vector3(FromLittleEndianF32(pData[0]), FromLittleEndianF32(pData[1]),
				FromLittleEndianF32(pData[2]));
#endif

		indexedFrame.parentIdx = FromLittleEndian32(*((uint32_t*) (offData + 12*4)));
		DFFFrame* frame = new DFFFrame(posVector, rotMatrix);
		frame->flags = FromLittleEndian32(*((uint32_t*) (offData + 13*4)));
		indexedFrame.frame = frame;
	}

	// Now we read the frame names.
	uint32_t i = 0;
	RWSection::ChildIterator it = frameList->getChildBegin();
	while ((it = frameList->nextChild(RW_SECTION_EXTENSION, it))  !=  frameList->getChildEnd()) {
		if (i >= numFrames) {
			throw DFFException("Too many frame list extensions", __FILE__, __LINE__);
		}

		DFFFrame* frame = indexedFrames[i].frame;

		RWSection* frameListExt = *it;

		RWSection* frameSect = frameListExt->getChild(RW_SECTION_FRAME);

		if (frameSect) {
			char* frameName = new char[frameSect->getSize() + 1];
			frameName[frameSect->getSize()] = '\0';
			memcpy(frameName, frameSect->getData(), frameSect->getSize());
			frame->setName(frameName);
		}

		RWSection* hAnimPlg = frameListExt->getChild(RW_SECTION_HANIM_PLG);

		if (hAnimPlg) {
			uint8_t* adata = hAnimPlg->getData();
			int32_t boneID = FromLittleEndian32(*((int32_t*) (adata+4)));
			int32_t boneCount = FromLittleEndian32(*((int32_t*) (adata+8)));
			adata += 12;

			indexedFrames[i].boneID = boneID;

			if (boneCount != 0) {
				adata += 8;

				for (map<int32_t, DFFBone*>::iterator it = bones.begin() ; it != bones.end() ; it++)
					delete it->second;
				bones.clear();

				for (int32_t i = 0 ; i < boneCount ; i++) {
					// NOTE: The 'type' value might be the node topology flags. I don't know this for sure
					// and we don't seem to need this either way, but it might be like this:
					// 0 = NONE
					// 1 = POP
					// 2 = PUSH
					// 3 = PUSH/POP

					DFFBone* bone = new DFFBone;
					memcpy(bone, adata, 12);

#ifndef GTAFORMATS_LITTLE_ENDIAN
					bone->index = FromLittleEndian32(bone->index);
					bone->num = FromLittleEndian32(bone->num);
					bone->type = FromLittleEndian32(bone->type);
#endif

					bones[bone->getIndex()] = bone;

					adata += 12;
				}
			}
		}

		i++;
		it++;
	}

	// Associate frames with bones
	if (bones.size() != 0) {
		for (uint32_t i = 0 ; i < numFrames ; i++) {
			if (indexedFrames[i].boneID != -1) {
				indexedFrames[i].frame->bone = bones[indexedFrames[i].boneID];
			}
		}
	}

	// And now we will actually build the frame hierarchy.
	// We still keep the flat structure (indexedFrames), because we will need this later when we read the
	// ATOMIC sections, which link frames and geometries.

	DFFFrame* rootFrame = mesh->getRootFrame();

	for (i = 0 ; i < numFrames ; i++) {
		IndexedDFFFrame& indexedFrame = indexedFrames[i];

		if (indexedFrame.parentIdx != -1)
			indexedFrames[indexedFrame.parentIdx].frame->addChild(indexedFrame.frame);
		else
			rootFrame->addChild(indexedFrame.frame);
	}



	// ******************************************************
	// *				LOAD THE GEOMETRIES					*
	// ******************************************************

	RWSection* geomList = clump->getChild(RW_SECTION_GEOMETRYLIST);
	RWSection* geomListStruct = geomList->getChild(RW_SECTION_STRUCT);

	uint32_t numGeoms = FromLittleEndian32(*((uint32_t*) geomListStruct->getData()));

	RWSection::ChildIterator geomIt = geomList->getChildBegin();
	while ((geomIt = geomList->nextChild(RW_SECTION_GEOMETRY, geomIt))  !=  geomList->getChildEnd()) {
		RWSection* geomSect = *geomIt;


		// ********** LOAD THE GEOMETRY STRUCT **********

		// This is most notably the vertex data.

		RWSection* geomStruct = geomSect->getChild(RW_SECTION_STRUCT);
		uint8_t* geomData = geomStruct->getData();

		uint16_t flags = FromLittleEndian16(*((uint16_t*) geomData));
		uint8_t uvSetCount = *(geomData+2);
		// uint8_t unknown = *(geomData+3);
		uint32_t triCount = FromLittleEndian32(*((uint32_t*) (geomData+4)));
		uint32_t vertexCount = FromLittleEndian32(*((uint32_t*) (geomData+8)));
		uint32_t frameCount = FromLittleEndian32(*((uint32_t*) (geomData+12)));

		if ((flags & GEOMETRY_FLAG_TEXCOORDS)  !=  0  &&  (flags & GEOMETRY_FLAG_MULTIPLEUVSETS) == 0) {
			// At least some meshes in GTA 3 have the UV set count set to 0 although GEOMETRY_FLAG_TEXCOORDS
			// is set.
			uvSetCount = 1;
		}

		// Check if the flags are correct.
		/*assert (
					(flags & GEOMETRY_FLAG_MULTIPLEUVSETS) != 0
				||	(
							(flags & GEOMETRY_FLAG_TEXCOORDS) != 0
						&&	uvSetCount == 1
					)
				||	uvSetCount == 0
		);*/

		float ambientColor = 0.0f;
		float diffuseColor = 0.0f;
		float specularColor = 0.0f;

		uint8_t* vertexColors = NULL;
		float* uvCoords = NULL;
		float* vertices = NULL;
		float* normals = NULL;

		geomData += 16;

		if (geomStruct->getVersion() < RW_VERSION_GTAVC_2) {
			ambientColor = FromLittleEndianF32(*((float*) geomData));
			diffuseColor = FromLittleEndianF32(*((float*) (geomData + 4)));
			specularColor = FromLittleEndianF32(*((float*) (geomData + 8)));
			geomData += 12;
		}

		if ((flags & GEOMETRY_FLAG_COLORS)  !=  0) {
			vertexColors = new uint8_t[vertexCount*4];
			memcpy(vertexColors, geomData, vertexCount*4);
			geomData += vertexCount*4;
		}

		if ((flags & (GEOMETRY_FLAG_TEXCOORDS | GEOMETRY_FLAG_MULTIPLEUVSETS))  !=  0) {
			size_t size = uvSetCount * vertexCount * 2;
			uvCoords = new float[size];
			memcpy(uvCoords, geomData, size*4);

#ifndef GTAFORMATS_LITTLE_ENDIAN
			for (size_t i = 0 ; i < size ; i++) {
				uvCoords[i] = SwapEndiannessF32(uvCoords[i]);
			}
#endif

			geomData += size*4;
		}

		// Skip the indices, we'll use the ones from the material split section
		geomData += triCount * 8;

		DFFBoundingSphere* bounds = new DFFBoundingSphere;
		memcpy(bounds, geomData, 4*4);

#ifndef GTAFORMATS_LITTLE_ENDIAN
		bounds->x = SwapEndiannessF32(bounds->x);
		bounds->y = SwapEndiannessF32(bounds->y);
		bounds->z = SwapEndiannessF32(bounds->z);
		bounds->radius = SwapEndiannessF32(bounds->radius);
#endif

		// uint32_t hasPositions = FromLittleEndian32(*((uint32_t*) (geomData+16)));
		// uint32_t hasNormals = FromLittleEndian32(*((uint32_t*) (geomData+20)));
		geomData += 6*4;

		// We ignore the setting of GEOMETRY_FLAG_POSITIONS. There are some meshes in GTA 3 where this flag
		// is not set but which have vertex positions nonetheless. The engine seems to ignore the flag.
		size_t size = vertexCount*3;
		vertices = new float[size];
		memcpy(vertices, geomData, size*4);

#ifndef GTAFORMATS_LITTLE_ENDIAN
		for (size_t i = 0 ; i < size ; i++) {
			vertices[i] = SwapEndiannessF32(vertices[i]);
		}
#endif

		geomData += size*4;

		if ((flags & GEOMETRY_FLAG_NORMALS)  !=  0) {
			size = vertexCount*3;
			normals = new float[size];
			memcpy(normals, geomData, size*4);

#ifndef GTAFORMATS_LITTLE_ENDIAN
			for (size_t i = 0 ; i < size ; i++) {
				normals[i] = SwapEndiannessF32(normals[i]);
			}
#endif

			geomData += size*4;
		}

		DFFGeometry* geom = new DFFGeometry(vertexCount, vertices, normals, uvCoords, uvSetCount,
				vertexColors);
		geom->setFlags(flags);
		geom->setFrameCount(frameCount);
		geom->setBounds(bounds);

		if (geomStruct->getVersion() < RW_VERSION_GTAVC_2) {
			geom->setAmbientLight(ambientColor);
			geom->setDiffuseLight(diffuseColor);
			geom->setSpecularLight(specularColor);
		}


		// ********** LOAD THE MATERIALS **********

		RWSection* materialList = geomSect->getChild(RW_SECTION_MATERIALLIST);

		RWSection* materialListStruct = materialList->getChild(RW_SECTION_STRUCT);

		uint32_t numMaterials = FromLittleEndian32(*((uint32_t*) materialListStruct->getData()));

		RWSection::ChildIterator matIt = materialList->getChildBegin();
		while ((matIt = materialList->nextChild(RW_SECTION_MATERIAL, matIt))
				!=  materialList->getChildEnd())
		{
			RWSection* matSect = *matIt;

			RWSection* matStruct = matSect->getChild(RW_SECTION_STRUCT);
			uint8_t* matData = matStruct->getData();

			DFFMaterial* mat = new DFFMaterial;

			// uint32_t unknown = *((uint32_t*) matData);
			memcpy(mat->color, matData+4, 4);
			// uint32_t unknown = *((uint32_t*) (matData+8));
			uint32_t texCount = FromLittleEndian32(*((uint32_t*) (matData+12)));

			// Load the textures

			RWSection::ChildIterator texIt = matSect->getChildBegin();
			while ((texIt = matSect->nextChild(RW_SECTION_TEXTURE, texIt))  !=  matSect->getChildEnd()) {
				RWSection* texSect = *texIt;

				RWSection* texStruct = texSect->getChild(RW_SECTION_STRUCT);
				uint16_t filterFlags = FromLittleEndian16(*((uint16_t*) texStruct->getData()));

				RWSection::ChildIterator it = texSect->getChildBegin();

				it = texSect->nextChild(RW_SECTION_STRING, it);
				RWSection* diffNameSect = *it;
				char* diffuseName = new char[diffNameSect->getSize()];
				memcpy(diffuseName, diffNameSect->getData(), diffNameSect->getSize());

				it = texSect->nextChild(RW_SECTION_STRING, it);
				RWSection* alphaNameSect = *it;
				char* alphaName = new char[alphaNameSect->getSize()];
				memcpy(alphaName, alphaNameSect->getData(), alphaNameSect->getSize());

				DFFTexture* tex = new DFFTexture(diffuseName, alphaName, filterFlags);

				mat->addTexture(tex);

				texIt++;
			}

			geom->addMaterial(mat);

			matIt++;
		}


		RWSection* geomExt = geomSect->getChild(RW_SECTION_EXTENSION);

		// ********** Load the material split data **********

		RWSection* materialSplit = geomExt->getChild(RW_SECTION_MATERIALSPLIT);
		uint8_t* msData = materialSplit->getData();

		// uint32_t faceType = FromLittleEndian32(*((uint32_t*) msData));
		uint32_t splitCount = FromLittleEndian32(*((uint32_t*) (msData+4)));
		// uint32_t numFaces = FromLittleEndian32(*((uint32_t*) (msData+8)));
		msData += 12;

		for (uint32_t j = 0 ; j < splitCount ; j++) {
			uint32_t idxCount = FromLittleEndian32(*((uint32_t*) msData));
			uint32_t materialIdx = FromLittleEndian32(*((uint32_t*) (msData+4)));
			msData += 8;

			uint32_t* indices = new uint32_t[idxCount];
			memcpy(indices, msData, idxCount*4);

#ifndef GTAFORMATS_LITTLE_ENDIAN
			for (size_t k = 0 ; k < idxCount ; k++) {
				indices[k] = SwapEndianness32(indices[k]);
			}
#endif

			msData += idxCount*4;

			DFFGeometryPart* part = new DFFGeometryPart(idxCount, indices);

			geom->addPart(part);
			part->setMaterial(geom->getMaterial(materialIdx));
		}


		// ********** Load the vertex skinning data **********

		RWSection* skin = geomExt->getChild(RW_SECTION_SKIN_PLG);

		if (skin) {
			uint8_t* sData = skin->getData();

			uint8_t boneCount = *sData;
			uint8_t spCount = *(sData + 1);
			//uint8_t unknown1 = *(sData + 2);
			//uint8_t unknown2 = *(sData + 3);

			sData += 4;
			sData += spCount; // spCount times unknown3

			geom->boneIndices = new uint8_t[vertexCount*4];
			memcpy(geom->boneIndices, sData, vertexCount*4);
			sData += vertexCount*4;

			geom->boneWeights = new float[vertexCount*4];
			memcpy(geom->boneWeights, sData, vertexCount*4*sizeof(float));
			sData += vertexCount*4*sizeof(float);

#ifndef GTAFORMATS_LITTLE_ENDIAN
			for (size_t j = 0 ; j < vertexCount*4 ; j++) {
				geom->boneWeights[j] = SwapEndiannessF32(geom->boneWeights[j]);
			}
#endif

			// Inverse Bone Matrices and possibly (version dependent) some unknown data follows...
		}


		mesh->addGeometry(geom);

		geomIt++;
	}


	// **********************************************************************
	// *				LOAD THE GEOMETRY <-> FRAME LINKS					*
	// **********************************************************************

	RWSection::ChildIterator atomicIt = clump->getChildBegin();
	while ((atomicIt = clump->nextChild(RW_SECTION_ATOMIC, atomicIt))  !=  clump->getChildEnd()) {
		RWSection* atomic = *atomicIt;

		RWSection* atomicStruct = atomic->getChild(RW_SECTION_STRUCT);
		uint8_t* asData = atomicStruct->getData();

		uint32_t frameIdx = FromLittleEndian32(*((uint32_t*) asData));
		uint32_t geomIdx = FromLittleEndian32(*((uint32_t*) (asData+4)));

		mesh->getGeometry(geomIdx)->setAssociatedFrame(indexedFrames[frameIdx].frame);

		atomicIt++;
	}


	// Delete the flat frame structure.
	delete[] indexedFrames;

	return mesh;
}
