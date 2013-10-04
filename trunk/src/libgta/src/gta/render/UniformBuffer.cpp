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
