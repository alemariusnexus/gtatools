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

#ifndef RENDERINGENTITY_H_
#define RENDERINGENTITY_H_

#include "rendering.h"
#include "ShaderPlugin.h"
#include "ShaderPluginRegistry.h"
#include <list>

using std::list;



class RenderingEntity
{
public:
	typedef list<ShaderPlugin*> ShaderPluginList;
	typedef ShaderPluginList::iterator ShaderPluginIterator;
	typedef ShaderPluginList::const_iterator ConstShaderPluginIterator;

public:
	virtual ~RenderingEntity() {}
	virtual ShaderPluginRegistry& getPluginRegistry() { return pluginRegistry; }
	virtual const ShaderPluginRegistry& getPluginRegistry() const { return pluginRegistry; }
	virtual void setPluginRegistry(const ShaderPluginRegistry& reg) { pluginRegistry = reg; }

private:
	ShaderPluginRegistry pluginRegistry;
};

#endif /* RENDERINGENTITY_H_ */
