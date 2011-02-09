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

#ifndef ENGINE_H_
#define ENGINE_H_

#include "config.h"
#include "ShaderProgram.h"
#include <locale>
#include <cstring>

using std::locale;
using std::collate;
using std::use_facet;

class ResourceManager;



typedef long hash_t;

inline hash_t Hash(const char* str)
{
	return use_facet< collate<char> >(locale()).hash(str, str+strlen(str));
}


class Engine {
public:
	static Engine* getInstance();

public:
	ResourceManager* getResourceManager() { return resMgr; }
	void setResourceManager(ResourceManager* mgr) { resMgr = mgr; }
	ShaderProgram* getCurrentShaderProgram() { return currentShader; }
	void setCurrentShaderProgram(ShaderProgram* program);

private:
	Engine();

private:
	ResourceManager* resMgr;
	ShaderProgram* currentShader;

};

#endif /* ENGINE_H_ */
