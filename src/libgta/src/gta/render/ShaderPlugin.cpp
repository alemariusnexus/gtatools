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

#include "ShaderPlugin.h"
#include "../Engine.h"
#include "ShaderPluginAPI.h"



uint32_t ShaderPlugin::nextUniqueID = 0;



ShaderPlugin::ShaderPlugin()
		: uniqueID(nextUniqueID++)
{
	vertexHeaderCode.reserve(4096);
	fragmentHeaderCode.reserve(4096);

	char tmpStr[4096];

	sprintf(tmpStr,
			"#define GTAGL_SHADER_PLUGIN_ID %u\n"
			"#define GTAGL_SHADER_PLUGIN\n\n",
			uniqueID);
	vertexHeaderCode += tmpStr;
	fragmentHeaderCode += tmpStr;

	ShaderPluginAPI* api = Engine::getInstance()->getShaderPluginAPI();

	list<ShaderPluginAPIHook*> hooks;
	api->getVertexHooks(hooks);

	for (list<ShaderPluginAPIHook*>::iterator it = hooks.begin() ; it != hooks.end() ; it++) {
		ShaderPluginAPIHook* hook = *it;

		CString baseName = hook->getBaseName();

		sprintf(tmpStr, "#define %s %s%u\n", baseName.get(), baseName.get(), uniqueID);
		vertexHeaderCode += tmpStr;
	}

	hooks.clear();
	api->getFragmentHooks(hooks);

	for (list<ShaderPluginAPIHook*>::iterator it = hooks.begin() ; it != hooks.end() ; it++) {
		ShaderPluginAPIHook* hook = *it;

		CString baseName = hook->getBaseName();

		sprintf(tmpStr, "#define %s %s%u\n", baseName.get(), baseName.get(), uniqueID);
		fragmentHeaderCode += tmpStr;
	}
}


CString ShaderPlugin::getVertexHeaderCode() const
{
	return vertexHeaderCode;
}


CString ShaderPlugin::getFragmentHeaderCode() const
{
	return fragmentHeaderCode;
}


void ShaderPlugin::applyUniformBuffers(AdvancedShaderProgram* program) const
{
	ConstUniformBufferIterator beg = uniformBufs.begin();
	ConstUniformBufferIterator end = uniformBufs.end();

	for (ConstUniformBufferIterator it = beg ; it != end ; it++) {
		const UniformBuffer* buf = *it;
		buf->apply(program);
	}
}
