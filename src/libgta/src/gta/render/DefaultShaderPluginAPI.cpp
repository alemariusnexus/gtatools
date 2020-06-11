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

#include "DefaultShaderPluginAPI.h"




DefaultShaderPluginAPI::DefaultShaderPluginAPI()
{
	defineVertexHook("VertexPostProcessing", "inout vec4 vertex", "vertex");
	defineVertexHook("VertexPreProcessing", "inout vec4 vertex", "vertex");
	defineVertexHook("VertexPostModelTransformationProcessing", "inout vec4 vertex", "vertex");

	defineFragmentHook("FragmentPostProcessing", "inout vec4 frag", "frag");
	defineFragmentHook("FragmentPreProcessing", "inout vec4 frag", "frag");

	defineFragmentHook("DPBlendLayerFragmentPostProcessing", "inout vec4 frag", "frag");
}


DefaultShaderPluginAPI::~DefaultShaderPluginAPI()
{
	for (ShaderPluginAPIHook* hook : vertexHooks) {
		delete hook;
	}
	for (ShaderPluginAPIHook* hook : fragmentHooks) {
		delete hook;
	}
}


void DefaultShaderPluginAPI::defineVertexHook(const CString& baseName, const CString& paramList, const CString& paramNameList)
{
	ShaderPluginAPIHook* hook = new ShaderPluginAPIHook(baseName, paramList, paramNameList);
	vertexHooks.push_back(hook);
}


void DefaultShaderPluginAPI::defineFragmentHook(const CString& baseName, const CString& paramList, const CString& paramNameList)
{
	ShaderPluginAPIHook* hook = new ShaderPluginAPIHook(baseName, paramList, paramNameList);
	fragmentHooks.push_back(hook);
}


void DefaultShaderPluginAPI::getVertexHooks(list<ShaderPluginAPIHook*>& hooks)
{
	hooks = vertexHooks;
}


void DefaultShaderPluginAPI::getFragmentHooks(list<ShaderPluginAPIHook*>& hooks)
{
	hooks = fragmentHooks;
}
