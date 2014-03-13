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

#ifndef TESTSHADERPLUGIN_H_
#define TESTSHADERPLUGIN_H_

#include "ShaderPlugin.h"



class TestShaderPlugin : public ShaderPlugin
{
private:
	static const char* hooks[];

public:
	TestShaderPlugin(uint32_t num);
	virtual ~TestShaderPlugin();
	virtual uint32_t getFlags() const;
	virtual bool usesHook(const CString& baseName) const;
	virtual Shader* getVertexShader() const;
	virtual Shader* getFragmentShader() const;

private:
	Shader* vshader;
	Shader* fshader;
};

#endif /* TESTSHADERPLUGIN_H_ */
