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

#include "ShaderPluginRegistry.h"
#include <utility>

using std::pair;




ShaderPluginRegistry::ShaderPluginRegistry()
		: data(new Data)
{
}


ShaderPluginRegistry::ShaderPluginRegistry(const ShaderPluginRegistry& other)
		: data(other.data)
{
}


void ShaderPluginRegistry::ensureUniqueness()
{
	if (data.unique())
		return;

	Data* newData = new Data;
	newData->numPlugins = data->numPlugins;
	newData->identDataLen = data->identDataLen;

	if (data->identData) {
		newData->identData = new char[data->identDataLen];
		memcpy(newData->identData, data->identData, data->identDataLen);
	}

	newData->identDataHash = data->identDataHash;

	for (PluginIterator it = data->plugins.begin() ; it != data->plugins.end() ; it++) {
		PluginEntry* entry = it->second;

		PluginEntry* newEntry = new PluginEntry;

		for (set<ShaderPlugin*, ShaderPluginComparator>::iterator iit = entry->plugins.begin() ; iit != entry->plugins.end() ; iit++) {
			newEntry->plugins.insert(*iit);
		}

		newData->plugins.insert(pair<uint32_t, PluginEntry*>(it->first, newEntry));
	}

	data = shared_ptr<Data>(newData);
}


void ShaderPluginRegistry::installPlugin(ShaderPlugin* plugin, uint32_t priority)
{
	ensureUniqueness();

	PluginIterator it = data->plugins.find(priority);

	if (it == data->plugins.end()) {
		PluginEntry* entry = new PluginEntry;
		it = data->plugins.insert(pair<uint32_t, PluginEntry*>(priority, entry)).first;
	}

	PluginEntry* entry = it->second;
	entry->plugins.insert(plugin);

	data->numPlugins++;

	updateIdentData();
}


void ShaderPluginRegistry::uninstallPlugin(ShaderPlugin* plugin)
{
	ensureUniqueness();

	for (PluginIterator it = data->plugins.begin() ; it != data->plugins.end() ; it++) {
		PluginEntry* entry = it->second;

		set<ShaderPlugin*, ShaderPluginComparator>::iterator iit = entry->plugins.find(plugin);

		if (iit == entry->plugins.end())
			continue;

		entry->plugins.erase(iit);
		data->numPlugins--;

		updateIdentData();

		break;
	}
}


void ShaderPluginRegistry::getPlugins(list<ShaderPlugin*>& plugins) const
{
	for (ConstPluginIterator it = data->plugins.begin() ; it != data->plugins.end() ; it++) {
		PluginEntry* entry = it->second;

		set<ShaderPlugin*, ShaderPluginComparator>::iterator iit;
		for (iit = entry->plugins.begin() ; iit != entry->plugins.end() ; iit++) {
			ShaderPlugin* plugin = *iit;
			plugins.push_back(plugin);
		}
	}
}


void ShaderPluginRegistry::updateIdentData()
{
	ensureUniqueness();

	delete[] data->identData;

	data->identDataLen = data->numPlugins*sizeof(uint32_t) + sizeof(uint32_t);
	data->identData = new char[data->identDataLen];

	((uint32_t*) data->identData)[0] = data->plugins.size();

	size_t offset = 4;


	for (PluginIterator it = data->plugins.begin() ; it != data->plugins.end() ; it++) {
		PluginEntry* entry = it->second;

		set<ShaderPlugin*, ShaderPluginComparator>::iterator iit;
		for (iit = entry->plugins.begin() ; iit != entry->plugins.end() ; iit++) {
			ShaderPlugin* plugin = *iit;
			uint32_t uid = plugin->getUniqueID();

			memcpy(data->identData+offset, &uid, sizeof(uint32_t));
			offset += sizeof(uint32_t);
		}
	}

	data->identDataHash = Hash(data->identData, data->identDataLen);
}


bool ShaderPluginRegistry::operator<(const ShaderPluginRegistry& other) const
{
	if (data->identDataHash == other.data->identDataHash) {
		if (data->identDataLen == other.data->identDataLen) {
			return memcmp(data->identData, other.data->identData, data->identDataLen) < 0;
		} else {
			return data->identDataLen < other.data->identDataLen;
		}
	} else {
		return data->identDataHash < other.data->identDataHash;
	}
}


bool ShaderPluginRegistry::operator==(const ShaderPluginRegistry& other) const
{
	return data->identDataHash == other.data->identDataHash
			&&  data->identDataLen == other.data->identDataLen
			&&  memcmp(data->identData, other.data->identData, data->identDataLen) == 0;
}


void ShaderPluginRegistry::applyUniformBuffers(AdvancedShaderProgram* program) const
{
	ConstUniformBufferIterator beg = data->uniformBufs.begin();
	ConstUniformBufferIterator end = data->uniformBufs.end();

	for (ConstUniformBufferIterator it = beg ; it != end ; it++) {
		const UniformBuffer* buf = *it;
		buf->apply(program);
	}
}
