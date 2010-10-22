/*
 * Engine.cpp
 *
 *  Created on: 21.10.2010
 *      Author: alemariusnexus
 */

#include "Engine.h"
#include "ResourceManager.h"



Engine* Engine::getInstance()
{
	static Engine inst;
	return &inst;
}


Engine::Engine()
		: resMgr(new ResourceManager)
{
}
