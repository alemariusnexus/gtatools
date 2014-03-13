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

#ifndef SHADERPLUGIN_H_
#define SHADERPLUGIN_H_

#include "../Shader.h"
#include "UniformBuffer.h"
#include <list>

using std::list;



class ShaderPlugin
{
public:
	typedef list<UniformBuffer*> UniformBufferList;
	typedef UniformBufferList::iterator UniformBufferIterator;
	typedef UniformBufferList::const_iterator ConstUniformBufferIterator;

public:
	enum Flags
	{
		UsesVertexShader = 1 << 0,
		UsesFragmentShader = 1 << 1
	};

public:
	ShaderPlugin();
	virtual ~ShaderPlugin() {}
	virtual uint32_t getFlags() const = 0;
	virtual uint32_t getUniqueID() const { return uniqueID; }
	virtual bool usesHook(const CString& baseName) const = 0;
	virtual Shader* getVertexShader() const { return NULL; }
	virtual Shader* getFragmentShader() const { return NULL; }

	void addUniformBuffer(UniformBuffer* buf) { uniformBufs.push_back(buf); }
	UniformBufferIterator getUniformBufferBegin() { return uniformBufs.begin(); }
	UniformBufferIterator getUniformBufferEnd() { return uniformBufs.end(); }
	ConstUniformBufferIterator getUniformBufferBegin() const { return uniformBufs.begin(); }
	ConstUniformBufferIterator getUniformBufferEnd() const { return uniformBufs.end(); }
	void applyUniformBuffers(AdvancedShaderProgram* program) const;

protected:
	CString getVertexHeaderCode() const;
	CString getFragmentHeaderCode() const;

protected:
	uint32_t uniqueID;
	CString vertexHeaderCode;
	CString fragmentHeaderCode;
	UniformBufferList uniformBufs;

private:
	static uint32_t nextUniqueID;
};

#endif /* SHADERPLUGIN_H_ */
