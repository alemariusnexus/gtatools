/*
	Copyright 2010-2014 David "Alemarius Nexus" Lerch

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

#include "ShadowVolumeGenerator.h"
#include <nxcommon/math/Vector3.h>
#include <vector>
#include <cstdio>

using std::vector;



void ShadowVolumeGenerator::generateShadowVolumes(ShadowMesh* mesh, const Vector4& osLightPos,
		vector<uint32_t>& vindices)
{
	Vector3 osLightPos3(osLightPos);

	bool* frontFacing = new bool[mesh->numFaces];

	for (uint32_t faceIdx = 0 ; faceIdx < mesh->numFaces ; faceIdx++) {
		Vector3& normal = *((Vector3*) (mesh->faceNormals + faceIdx*3));
		Vector3& v1 = *((Vector3*) (mesh->vertices + mesh->indices[faceIdx*6]*4));

		Vector3 faceToLight = osLightPos3 - v1*osLightPos.getW();

		frontFacing[faceIdx] = (faceToLight * normal) > 0;
	}

	// Compute the caps
	for (uint32_t i = 0 ; i < mesh->numFaces ; i++) {
		uint32_t idx1 = mesh->indices[i*6];
		uint32_t idx2 = mesh->indices[i*6 + 2];
		uint32_t idx3 = mesh->indices[i*6 + 4];

		if (!frontFacing[i]) {
			uint32_t oldIdx2 = idx2;
			idx2 = idx3;
			idx3 = oldIdx2;
		}

		// Front cap
		vindices.push_back(idx1);
		vindices.push_back(idx2);
		vindices.push_back(idx3);

		// Back cap
		vindices.push_back(mesh->numVertices + idx1);
		vindices.push_back(mesh->numVertices + idx3);
		vindices.push_back(mesh->numVertices + idx2);
	}

	// Compute the silhouette sides
	for (uint32_t faceIdx = 0 ; faceIdx < mesh->numFaces ; faceIdx++) {
		for (uint32_t j = 0 ; j < 3 ; j++) {
			uint32_t edgeIdx1 = mesh->indices[faceIdx*6 + j*2];
			uint32_t neighborIdx = mesh->indices[faceIdx*6 + j*2 + 1];
			uint32_t edgeIdx2 = mesh->indices[faceIdx*6 + (j*2 + 2) % 6];

			uint32_t neighborFaceIdx = mesh->adjacentFaces[faceIdx*3 + j];

			if (faceIdx == 15  &&  neighborFaceIdx == 16) {
				//printf("Here we are!\n");
			}

			if (neighborIdx >= mesh->numVertices  ||  frontFacing[faceIdx] != frontFacing[neighborFaceIdx]) {
				// This is a possible silhouette side

				uint32_t sideIdx1 = frontFacing[faceIdx] ? edgeIdx1 : edgeIdx2;
				uint32_t sideIdx2 = frontFacing[faceIdx] ? edgeIdx2 : edgeIdx1;

				vindices.push_back(sideIdx1);
				vindices.push_back(mesh->numVertices + sideIdx1);
				vindices.push_back(sideIdx2);

				vindices.push_back(sideIdx2);
				vindices.push_back(mesh->numVertices + sideIdx1);
				vindices.push_back(mesh->numVertices + sideIdx2);
			}
		}
	}

	delete[] frontFacing;
}
