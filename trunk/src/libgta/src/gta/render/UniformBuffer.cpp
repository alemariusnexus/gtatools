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

#include "UniformBuffer.h"
#include "AdvancedShaderProgram.h"
#include <utility>

using std::pair;



void UniformBuffer::setData(const CString& name, UniformData* data)
{
	//this->data.insert(pair<CString, UniformData*>(name, data));
	this->data[name] = data;
}


void UniformBuffer::apply(AdvancedShaderProgram* program) const
{
	ConstDataIterator beg = data.begin();
	ConstDataIterator end = data.end();

	for (ConstDataIterator it = beg ; it != end ; it++) {
		CString name = it->first;
		const UniformData* data = it->second;
		data->apply(program->getUniformLocation(name));
	}
}
