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

#include "AdvancedShaderProgram.h"
#include "../Engine.h"
#include <nxcommon/log.h>


AdvancedShaderProgram::AdvancedShaderProgram(const CString& programName)
		: name(programName), program(new ShaderProgram(programName)), vertexDispatchShader(NULL),
		  fragmentDispatchShader(NULL), shadersValid(false)
{
}


AdvancedShaderProgram::~AdvancedShaderProgram()
{
	delete program;

	if (vertexDispatchShader)
		delete vertexDispatchShader;
	if (fragmentDispatchShader)
		delete fragmentDispatchShader;
}


void AdvancedShaderProgram::attachShader(AdvancedShader* shader)
{
	baseShaders.push_back(shader);

	shadersValid = false;
}


void AdvancedShaderProgram::addShaderPlugin(ShaderPlugin* plugin)
{
	plugins.push_back(plugin);

	shadersValid = false;
}


void AdvancedShaderProgram::build()
{
	program->detachAllShaders();

	generateShaders();

	for (ShaderIterator it = baseShaders.begin() ; it != baseShaders.end() ; it++) {
		AdvancedShader* shader = *it;
		program->attachShader(shader->shader);
	}

	for (PluginIterator it = plugins.begin() ; it != plugins.end() ; it++) {
		ShaderPlugin* plugin = *it;

		Shader* vshader = plugin->getVertexShader();
		Shader* fshader = plugin->getFragmentShader();

		if (vshader)
			program->attachShader(vshader);
		if (fshader)
			program->attachShader(fshader);
	}

	program->attachShader(vertexDispatchShader);
	program->attachShader(fragmentDispatchShader);

	program->link();
}


void AdvancedShaderProgram::makeCurrent()
{
	program->makeCurrent();
}


GLint AdvancedShaderProgram::getAttributeLocation(const CString& name) const
{
	return program->getAttributeLocation(name);
}


void AdvancedShaderProgram::bindAttributeLocation(GLuint index, const CString& name)
{
	program->bindAttributeLocation(index, name);
}


void AdvancedShaderProgram::generateShaders()
{
	if (shadersValid)
		return;

	generateDispatchShaders();
	generateCoreShaderHeaderCode();

	for (ShaderIterator it = baseShaders.begin() ; it != baseShaders.end() ; it++) {
		AdvancedShader* shader = *it;
		GLenum type = shader->shader->getType();

		if (type == GL_VERTEX_SHADER)
			shader->compile(vertexCoreHeaderCode);
		else
			shader->compile(fragmentCoreHeaderCode);
	}

	shadersValid = true;
}


void AdvancedShaderProgram::generateDispatchShaders()
{
	if (shadersValid)
		return;

	if (vertexDispatchShader)
		delete vertexDispatchShader;
	if (fragmentDispatchShader)
		delete fragmentDispatchShader;

	ShaderPluginAPI* api = Engine::getInstance()->getShaderPluginAPI();
	list<ShaderPluginAPIHook*> hooks;

	char* shaderName = new char[name.length() + 128];

	sprintf(shaderName, "Plugin Dispatch Vertex Shader for program '%s'", name.get());
	api->getVertexHooks(hooks);
	CString vertexCode = generateDispatchShaderCode(hooks);
	vertexDispatchShader = new Shader(GL_VERTEX_SHADER, shaderName);
	vertexDispatchShader->loadSourceCode(vertexCode);
	vertexDispatchShader->compile();

	hooks.clear();

	sprintf(shaderName, "Plugin Dispatch Fragment Shader for program '%s'", name.get());
	api->getFragmentHooks(hooks);
	CString fragmentCode = generateDispatchShaderCode(hooks);
	fragmentDispatchShader = new Shader(GL_FRAGMENT_SHADER, shaderName);
	fragmentDispatchShader->loadSourceCode(fragmentCode);
	fragmentDispatchShader->compile();

	delete[] shaderName;
}


void AdvancedShaderProgram::generateCoreShaderHeaderCode()
{
	if (shadersValid)
		return;

	ShaderPluginAPI* api = Engine::getInstance()->getShaderPluginAPI();
	list<ShaderPluginAPIHook*> hooks;

	api->getVertexHooks(hooks);
	vertexCoreHeaderCode = generateCoreShaderHeaderCode(hooks);

	api->getFragmentHooks(hooks);
	fragmentCoreHeaderCode = generateCoreShaderHeaderCode(hooks);
}


CString AdvancedShaderProgram::generateCoreShaderHeaderCode(list<ShaderPluginAPIHook*>& hooks)
{
	CString usedDefineCode;
	usedDefineCode.reserve(2048);

	CString funcDeclCode;
	funcDeclCode.reserve(2048);

	CString funcDefCode;
	funcDefCode.reserve(2048);

	for (list<ShaderPluginAPIHook*>::iterator it = hooks.begin() ; it != hooks.end() ; it++) {
		ShaderPluginAPIHook* hook = *it;
		CString baseName = hook->getBaseName();
		CString paramList = hook->getParameterList();
		CString paramNameList = hook->getParameterNameList();

		bool hookUsed = false;

		for (PluginIterator iit = plugins.begin() ; iit != plugins.end() ; iit++) {
			ShaderPlugin* plugin = *iit;
			uint32_t pluginID = plugin->getUniqueID();

			if (plugin->usesHook(baseName)) {
				hookUsed = true;
				break;
			}
		}

		char tmpBuf[4096];

		if (hookUsed) {
			CString uBaseName = baseName;
			uBaseName.upper();
			sprintf(tmpBuf, "#define SHADER_PLUGIN_HOOK_%s_USED\n", uBaseName.get());
			usedDefineCode += tmpBuf;

			sprintf(tmpBuf, "void Dispatch%s(%s);\n", baseName.get(), paramList.get());
			funcDeclCode += tmpBuf;
		} else {
			sprintf(tmpBuf, "#define Dispatch%s(%s)\n", baseName.get(), paramNameList.get());
			funcDefCode += tmpBuf;
		}
	}

	CString code;
	code.reserve(usedDefineCode.length() + funcDeclCode.length() + funcDefCode.length() + 64);

	code += usedDefineCode;
	code += "\n";
	code += funcDefCode;
	code += "\n";
	code += funcDeclCode;

	return code;
}


CString AdvancedShaderProgram::generateDispatchShaderCode(list<ShaderPluginAPIHook*>& hooks)
{
	CString hookDeclareCode;
	hookDeclareCode.reserve(4096);

	CString hookCode;
	hookCode.reserve(4096);

	char tmpStr[4096];

	for (list<ShaderPluginAPIHook*>::iterator it = hooks.begin() ; it != hooks.end() ; it++) {
		ShaderPluginAPIHook* hook = *it;
		CString baseName = hook->getBaseName();
		CString paramList = hook->getParameterList();
		CString paramNameList = hook->getParameterNameList();

		sprintf(tmpStr, "void Dispatch%s(%s) {\n", baseName.get(), paramList.get());
		hookCode += tmpStr;

		for (PluginIterator iit = plugins.begin() ; iit != plugins.end() ; iit++) {
			ShaderPlugin* plugin = *iit;
			uint32_t pluginID = plugin->getUniqueID();

			if (plugin->usesHook(baseName)) {
				sprintf(tmpStr, "void %s%u(%s);\n", baseName.get(), pluginID, paramList.get());
				hookDeclareCode += tmpStr;

				sprintf(tmpStr, "    %s%u(%s);\n", baseName.get(), pluginID, paramNameList.get());
				hookCode += tmpStr;
			}
		}

		hookCode += "}\n";
	}

	CString code;
	code.reserve(hookCode.length() + hookDeclareCode.length() + 8);
	code += hookDeclareCode;
	code += "\n\n";
	code += hookCode;

	return code;
}
