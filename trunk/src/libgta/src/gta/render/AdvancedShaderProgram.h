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

#ifndef ADVANCEDSHADERPROGRAM_H_
#define ADVANCEDSHADERPROGRAM_H_

#include "../Shader.h"
#include "../ShaderProgram.h"
#include "AdvancedShader.h"
#include "ShaderPlugin.h"
#include "ShaderPluginAPI.h"
#include <nxcommon/CString.h>
#include <list>
#include <map>
#include <utility>

using std::list;
using std::map;
using std::pair;



class AdvancedShaderProgram
{
public:
	typedef list<ShaderPlugin*> PluginList;
	typedef PluginList::iterator PluginIterator;
	typedef PluginList::const_iterator ConstPluginIterator;

private:
	typedef list<AdvancedShader*> ShaderList;
	typedef ShaderList::iterator ShaderIterator;

public:
	AdvancedShaderProgram(const CString& programName);
	~AdvancedShaderProgram();
	void attachShader(AdvancedShader* shader);
	void addShaderPlugin(ShaderPlugin* plugin);
	void build();
	void makeCurrent();
	GLint getAttributeLocation(const CString& name) const;
	void bindAttributeLocation(GLuint index, const CString& name);
	GLint getUniformLocation(const CString& name) const { return program->getUniformLocation(name); }
	GLuint getUniformBlockIndex(const CString& name) const { return program->getUniformBlockIndex(name); }
	void setUniformBlockBinding(GLuint blockIndex, GLuint bindingPoint) { program->setUniformBlockBinding(blockIndex, bindingPoint); }

	template <class ItType>
	void addShaderPlugins(ItType beg, ItType end)
	{
		for (ItType it = beg ; it != end ; it++)
			addShaderPlugin(*it);
	}

	PluginIterator getShaderPluginBegin() { return plugins.begin(); }
	PluginIterator getShaderPluginEnd() { return plugins.end(); }
	ConstPluginIterator getShaderPluginBegin() const { return plugins.begin(); }
	ConstPluginIterator getShaderPluginEnd() const { return plugins.end(); }

private:
	void generateShaders();
	CString generateDispatchShaderCode(list<ShaderPluginAPIHook*>& hooks);
	void generateDispatchShaders();
	void generateCoreShaderHeaderCode();
	CString generateCoreShaderHeaderCode(list<ShaderPluginAPIHook*>& hooks);

private:
	CString name;
	ShaderList baseShaders;
	PluginList plugins;
	ShaderProgram* program;

	CString vertexCoreHeaderCode;
	CString fragmentCoreHeaderCode;

	Shader* vertexDispatchShader;
	Shader* fragmentDispatchShader;
	bool shadersValid;
};

#endif /* ADVANCEDSHADERPROGRAM_H_ */
