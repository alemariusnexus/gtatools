/*
	Copyright 2010-2011 David "Alemarius Nexus" Lerch

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

#include "Engine.h"
#include "ResourceManager.h"



Engine* Engine::getInstance()
{
	static Engine inst;
	return &inst;
}


Engine::Engine()
		: resMgr(NULL), currentShader(NULL)
{
}


void Engine::setCurrentShaderProgram(ShaderProgram* program)
{
	currentShader = program;

	if (program) {
		program->makeCurrent();
	} else {
		ShaderProgram::disableShaders();
	}
}
