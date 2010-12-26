/*
	Copyright 2010 David "Alemarius Nexus" Lerch

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
 */

#ifndef COLMESHCONVERTER_H_
#define COLMESHCONVERTER_H_

#include "config.h"
#include <gtaformats/col/COLModel.h>
#include "Mesh.h"


class COLMeshConverter {
public:
	Mesh* convert(const COLModel& model);

private:
	void getMaterialColors(uint8_t mat, uint8_t& r, uint8_t& g, uint8_t& b);
	void convertVertexModel(const COLModel& model, int& vertexCount, float*& vertices, uint8_t*& colors,
			int32_t*& indices);
};

#endif /* COLMESHCONVERTER_H_ */
