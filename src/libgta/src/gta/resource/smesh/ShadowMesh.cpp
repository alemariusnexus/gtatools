/*
	Copyright 2010-2013 David "Alemarius Nexus" Lerch

	This file is part of libgta.

	libgta is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	libgta is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with libgta.  If not, see <http://www.gnu.org/licenses/>.

	Additional permissions are granted, which are listed in the file
	GPLADDITIONS.
 */

#include "ShadowMesh.h"
#include <gtaformats/util/math/Vector3.h>
#include <gta/config.h>
#include <map>
#include <cstring>
#include <cstdio>
#include "../../EngineException.h"

using std::map;



ShadowMesh::ShadowMesh(uint32_t numVertices, uint32_t numFaces, float* vertices, uint32_t* indices)
		: numVertices(numVertices), numFaces(numFaces), vertices(vertices), indices(indices)
{
	calculateMissingData();
}


ShadowMesh::ShadowMesh(const COLShadowMesh& smesh)
		: numVertices(smesh.getVertexCount()), numFaces(smesh.getFaceCount()),
		  vertices(new float[numVertices*3]), indices(new uint32_t[numFaces*3])
{
	memcpy(vertices, smesh.getVertices(), numVertices*3*sizeof(float));

	const COLFace* faces = smesh.getFaces();

	for (uint32_t i = 0 ; i < numFaces ; i++) {
		memcpy(indices + i*3, faces + i, 3*sizeof(uint32_t));
	}

	calculateMissingData();
}


ShadowMesh::ShadowMesh(const ShadowMesh& other)
		: numVertices(other.numVertices), numFaces(other.numFaces),
		  vertices(new float[numVertices*3]), indices(new uint32_t[numFaces*3]),
		  faceNormals(new float[numFaces*3])
{
	memcpy(vertices, other.vertices, numVertices*4*sizeof(float));
	memcpy(indices, other.indices, numFaces*6*sizeof(uint32_t));
	memcpy(faceNormals, other.faceNormals, numFaces*3*sizeof(float));
}


ShadowMesh::~ShadowMesh()
{
	delete[] vertices;
	delete[] indices;
	delete[] faceNormals;

}


void ShadowMesh::decoupleEdges(EdgeMap& edges)
{
	vector<float> newVertices;
	EdgeMap newEdges;

	for (EdgeMapIterator it = edges.begin() ; it != edges.end() ; it++) {
		for (map<uint32_t, EdgeInfo>::iterator iit = it->second.begin() ; iit != it->second.end() ; iit++) {
			uint32_t idx1 = it->first;
			uint32_t idx2 = iit->first;

			EdgeInfo& info = iit->second;

			if (info.faceIndices.size() > 2) {
				// More than two faces on an edge. We will decouple them here

				vector<EdgeInfo::FaceInfo>::iterator idxIt = info.faceIndices.begin();
				vector<EdgeInfo::FaceInfo>::iterator nextIdxIt;
				idxIt++; idxIt++;

				for (; idxIt != info.faceIndices.end() ; idxIt++) {
					EdgeInfo::FaceInfo& finfo = *idxIt;
					uint32_t faceIdx = finfo.index;

					uint32_t newIdx1 = numVertices + newVertices.size()/3;
					float v11 = vertices[idx1*3];
					float v12 = vertices[idx1*3 + 1];
					float v13 = vertices[idx1*3 + 2];
					newVertices.push_back(v11);
					newVertices.push_back(v12);
					newVertices.push_back(v13);

					uint32_t newIdx2 = newIdx1+1;
					float v21 = vertices[idx2*3];
					float v22 = vertices[idx2*3 + 1];
					float v23 = vertices[idx2*3 + 2];
					newVertices.push_back(v21);
					newVertices.push_back(v22);
					newVertices.push_back(v23);

					EdgeInfo& newInfo = newEdges[newIdx2][newIdx1];
					newInfo.faceIndices.push_back(finfo);

					if (indices[faceIdx*3] == idx1) {
						indices[faceIdx*3] = newIdx1;

						if (indices[faceIdx*3 + 1] == idx2)
							indices[faceIdx*3 + 1] = newIdx2;
						else
							indices[faceIdx*3 + 2] = newIdx2;
					} else if (indices[faceIdx*3 + 1] == idx1) {
						indices[faceIdx*3 + 1] = newIdx1;

						if (indices[faceIdx*3] == idx2)
							indices[faceIdx*3] = newIdx2;
						else
							indices[faceIdx*3 + 2] = newIdx2;
					} else {
						indices[faceIdx*3 + 2] = newIdx1;

						if (indices[faceIdx*3] == idx2)
							indices[faceIdx*3] = newIdx2;
						else
							indices[faceIdx*3 + 1] = newIdx2;
					}

					nextIdxIt = idxIt;
					nextIdxIt++;

					if (nextIdxIt != info.faceIndices.end()) {
						// There's another face too much. This is true if we have an even number of faces on
						// this edge. In this case, we can group two of them together on a new edge so we
						// do not introduce any new cracks.

						EdgeInfo::FaceInfo& nextfInfo = *nextIdxIt;
						uint32_t nextFaceIdx = nextfInfo.index;

						newInfo.faceIndices.push_back(nextfInfo);

						if (indices[nextFaceIdx*3] == idx1) {
							indices[nextFaceIdx*3] = newIdx1;

							if (indices[nextFaceIdx*3 + 1] == idx2)
								indices[nextFaceIdx*3 + 1] = newIdx2;
							else
								indices[nextFaceIdx*3 + 2] = newIdx2;
						} else if (indices[nextFaceIdx*3 + 1] == idx1) {
							indices[nextFaceIdx*3 + 1] = newIdx1;

							if (indices[nextFaceIdx*3] == idx2)
								indices[nextFaceIdx*3] = newIdx2;
							else
								indices[nextFaceIdx*3 + 2] = newIdx2;
						} else {
							indices[nextFaceIdx*3 + 2] = newIdx1;

							if (indices[nextFaceIdx*3] == idx2)
								indices[nextFaceIdx*3] = newIdx2;
							else
								indices[nextFaceIdx*3 + 1] = newIdx2;
						}

						info.faceIndices.erase(nextIdxIt);
					}

					vector<EdgeInfo::FaceInfo>::iterator idxItCpy = idxIt;
					idxIt--;
					info.faceIndices.erase(idxItCpy);
				}
			}
		}
	}

	for (EdgeMapIterator it = newEdges.begin() ; it != newEdges.end() ; it++) {
		for (map<uint32_t, EdgeInfo>::iterator iit = it->second.begin() ; iit != it->second.end() ; iit++) {
			edges[it->first][iit->first] = iit->second;
		}
	}

	float* newVertexArr = new float[numVertices*3 + newVertices.size()];
	memcpy(newVertexArr, vertices, numVertices*3*sizeof(float));
	//copy(newVertices.begin(), newVertices.end(), newVertexArr + numVertices*3);

	for (uint32_t i = 0 ; i < newVertices.size() ; i++) {
		newVertexArr[numVertices*3 + i] = newVertices[i];
	}

	delete[] vertices;
	vertices = newVertexArr;

	numVertices += newVertices.size() / 3;
}


void ShadowMesh::calculateMissingData()
{
	EdgeMap edges;

	faceNormals = new float[numFaces*3];

	for (uint32_t i = 0 ; i < numFaces ; i++) {
		uint32_t idx1 = indices[i*3];
		uint32_t idx2 = indices[i*3 + 1];
		uint32_t idx3 = indices[i*3 + 2];

		Vector3& v1 = *((Vector3*) (vertices + idx1 * 3));
		Vector3& v2 = *((Vector3*) (vertices + idx2 * 3));
		Vector3& v3 = *((Vector3*) (vertices + idx3 * 3));

		Vector3 normal = (v2-v1).cross(v3-v1);
		memcpy(faceNormals + i*3, &normal, 3*sizeof(float));

		uint32_t e1Idx1, e1Idx2;
		uint32_t e2Idx1, e2Idx2;
		uint32_t e3Idx1, e3Idx2;

		if (idx1 > idx2) {
			e1Idx1 = idx1;
			e1Idx2 = idx2;
		} else {
			e1Idx1 = idx2;
			e1Idx2 = idx1;
		}

		if (idx2 > idx3) {
			e2Idx1 = idx2;
			e2Idx2 = idx3;
		} else {
			e2Idx1 = idx3;
			e2Idx2 = idx2;
		}

		if (idx3 > idx1) {
			e3Idx1 = idx3;
			e3Idx2 = idx1;
		} else {
			e3Idx1 = idx1;
			e3Idx2 = idx3;
		}

		EdgeInfo& e1Info = edges[e1Idx1][e1Idx2];
		EdgeInfo::FaceInfo e1fInfo;
		e1fInfo.index = i;
		e1fInfo.additionalIndex = 2;
		e1Info.faceIndices.push_back(e1fInfo);

		EdgeInfo& e2Info = edges[e2Idx1][e2Idx2];
		EdgeInfo::FaceInfo e2fInfo;
		e2fInfo.index = i;
		e2fInfo.additionalIndex = 0;
		e2Info.faceIndices.push_back(e2fInfo);

		EdgeInfo& e3Info = edges[e3Idx1][e3Idx2];
		EdgeInfo::FaceInfo e3fInfo;
		e3fInfo.index = i;
		e3fInfo.additionalIndex = 1;
		e3Info.faceIndices.push_back(e3fInfo);
	}

	bool decoupled = false;
	for (ConstEdgeMapIterator it = edges.begin() ; it != edges.end()  &&  !decoupled ; it++) {
		for (map<uint32_t, EdgeInfo>::const_iterator iit = it->second.begin() ; iit != it->second.end() ; iit++) {
			const EdgeInfo& info = iit->second;

			if (info.faceIndices.size() > 2) {
				decoupleEdges(edges);
				decoupled = true;
				break;
			}
		}
	}


	float* extendedVertices = new float[2*numVertices * 4];

	for (uint32_t i = 0 ; i < numVertices ; i++) {
		memcpy(extendedVertices + i*4, vertices + i*3, 3*sizeof(float));
		extendedVertices[i*4 + 3] = 1.0f;
	}
	for (uint32_t i = 0 ; i < numVertices ; i++) {
		memcpy(extendedVertices + (numVertices+i)*4, vertices + i*3, 3*sizeof(float));
		extendedVertices[(numVertices+i)*4 + 3] = 0.0f;
	}

	delete[] vertices;
	vertices = extendedVertices;


	uint32_t* extendedIndices = new uint32_t[numFaces*6];

	// Enter the base indices for all faces into extendedIndices
	for (uint32_t i = 0 ; i < numFaces ; i++) {
		extendedIndices[i*6] = indices[i*3];
		extendedIndices[i*6 + 2] = indices[i*3 + 1];
		extendedIndices[i*6 + 4] = indices[i*3 + 2];
	}

	uint32_t dummyIdx = numVertices*2 - 1;

	adjacentFaces = new uint32_t[numFaces*3];

	for (EdgeMapIterator it = edges.begin() ; it != edges.end() ; it++) {
		uint32_t idx1 = it->first;

		for (map<uint32_t, EdgeInfo>::iterator iit = it->second.begin() ; iit != it->second.end() ; iit++) {
			uint32_t idx2 = iit->first;

			EdgeInfo& info = iit->second;

			EdgeInfo::FaceInfo& f1 = info.faceIndices[0];
			uint32_t f1ExtendedAdditionalIdx = f1.index*6 + (2*f1.additionalIndex + 3) % 6;

			if (info.faceIndices.size() == 1) {
				// f1 has no adjacent face between idx1 and idx2

				extendedIndices[f1ExtendedAdditionalIdx] = dummyIdx;

				// This value is invalid and shall not be used when there is no adjacent face, so we can
				// basically set it to anything we want
				adjacentFaces[f1.index*3 + (f1.additionalIndex+1) % 3] = 0;
			} else {
				// There are two adjacent faces on edge idx1-idx2

				EdgeInfo::FaceInfo& f2 = info.faceIndices[1];
				uint32_t f2ExtendedAdditionalIdx = f2.index*6 + (2*f2.additionalIndex + 3) % 6;

				extendedIndices[f1ExtendedAdditionalIdx] = indices[f2.index*3 + f2.additionalIndex];
				extendedIndices[f2ExtendedAdditionalIdx] = indices[f1.index*3 + f1.additionalIndex];

				adjacentFaces[f1.index*3 + (f1.additionalIndex+1) % 3] = f2.index;
				adjacentFaces[f2.index*3 + (f2.additionalIndex+1) % 3] = f1.index;
			}
		}
	}

	delete[] indices;
	indices = extendedIndices;
}
