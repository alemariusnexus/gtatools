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

#include "DPTestShaderPlugin.h"
#include <res_dp_test_vertex_shader.h>
#include <res_dp_test_fragment_shader.h>



const char* DPTestShaderPlugin::hooks[] = {
		"DPBlendLayerFragmentPostProcessing"
};




DPTestShaderPlugin::DPTestShaderPlugin()
		: ShaderPlugin(),
		  vshader(new Shader(GL_VERTEX_SHADER, "Depth Peeling Test Vertex Shader")),
		  fshader(new Shader(GL_FRAGMENT_SHADER, "Depth Peeling Test Fragment Shader"))
{
	CString headerCode = getVertexHeaderCode();
	CString fheaderCode = getFragmentHeaderCode();

	const char* data;
	size_t dataLen;


	data = (const char*) res_dp_test_vertex_shader_data;
	dataLen = sizeof(res_dp_test_vertex_shader_data);

	char* code = new char[headerCode.length() + dataLen + 32];
	CString resCode(data, dataLen);
	sprintf(code, "%s\n%s", headerCode.get(), resCode.get());

	CString ccode = CString::from(code);

	vshader->loadSourceCode(ccode);
	vshader->compile();


	data = (const char*) res_dp_test_fragment_shader_data;
	dataLen = sizeof(res_dp_test_fragment_shader_data);

	char* fcode = new char[fheaderCode.length() + dataLen + 32];
	CString fresCode(data, dataLen);
	sprintf(fcode, "%s\n%s", fheaderCode.get(), fresCode.get());

	CString fccode = CString::from(fcode);

	fshader->loadSourceCode(fccode);
	fshader->compile();
}


DPTestShaderPlugin::~DPTestShaderPlugin()
{
	delete vshader;
	delete fshader;
}


uint32_t DPTestShaderPlugin::getFlags() const
{
	return 0;
}


bool DPTestShaderPlugin::usesHook(const CString& baseName) const
{
	for (size_t i = 0 ; i < sizeof(hooks) / sizeof(const char*) ; i++) {
		if (baseName == hooks[i])
			return true;
	}

	return false;
}


Shader* DPTestShaderPlugin::getVertexShader() const
{
	return vshader;
}


Shader* DPTestShaderPlugin::getFragmentShader() const
{
	return fshader;
}
