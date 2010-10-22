/*
 * Engine.h
 *
 *  Created on: 21.10.2010
 *      Author: alemariusnexus
 */

#ifndef ENGINE_H_
#define ENGINE_H_

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
	ShaderProgram* getCurrentShaderProgram() { return currentShader; }
	void setCurrentShaderProgram(ShaderProgram* program) { currentShader = program; }

private:
	Engine();

private:
	ResourceManager* resMgr;
	ShaderProgram* currentShader;

};

#endif /* ENGINE_H_ */
