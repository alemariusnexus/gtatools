#ifndef SHADERPLUGINREGISTRY_H_
#define SHADERPLUGINREGISTRY_H_

#include <gta/config.h>
#include "ShaderPlugin.h"
#include "UniformBuffer.h"
#include <map>
#include <set>
#include <list>
#include <boost/shared_ptr.hpp>

using std::set;
using std::map;
using std::list;
using boost::shared_ptr;



class ShaderPluginRegistry
{
public:
	typedef list<UniformBuffer*> UniformBufferList;
	typedef UniformBufferList::iterator UniformBufferIterator;
	typedef UniformBufferList::const_iterator ConstUniformBufferIterator;

private:
	class ShaderPluginComparator
	{
	public:
		bool operator()(const ShaderPlugin* p1, const ShaderPlugin* p2) { return p1->getUniqueID() < p2->getUniqueID(); }
	};

	class PluginEntry
	{
	public:


	public:
		//list<ShaderPlugin*> plugins;
		set<ShaderPlugin*, ShaderPluginComparator> plugins;
	};


	typedef map<uint32_t, PluginEntry*> PluginMap;
	typedef PluginMap::iterator PluginIterator;
	typedef PluginMap::const_iterator ConstPluginIterator;


	class Data
	{
	public:
		Data() : numPlugins(0), identData(new char[sizeof(uint32_t)]), identDataLen(4)
		{
			((uint32_t*) identData)[0] = 0;
			identDataHash = Hash(identData, sizeof(uint32_t));
		}
		~Data()
		{
			if (identData)
				delete[] identData;

			for (PluginIterator it = plugins.begin() ; it != plugins.end() ; it++) {
				delete it->second;
			}
		}

	public:
		PluginMap plugins;
		size_t numPlugins;
		UniformBufferList uniformBufs;

		char* identData;
		size_t identDataLen;
		size_t identDataHash;
	};

public:
	ShaderPluginRegistry();
	ShaderPluginRegistry(const ShaderPluginRegistry& other);
	void installPlugin(ShaderPlugin* plugin, uint32_t priority = 10000);
	void uninstallPlugin(ShaderPlugin* plugin);
	void getPlugins(list<ShaderPlugin*>& plugins) const;
	void addUniformBuffer(UniformBuffer* buf) { ensureUniqueness(); data->uniformBufs.push_back(buf); }
	UniformBufferIterator getUniformBufferBegin() { ensureUniqueness(); return data->uniformBufs.begin(); }
	UniformBufferIterator getUniformBufferEnd() { ensureUniqueness(); return data->uniformBufs.end(); }
	ConstUniformBufferIterator getUniformBufferBegin() const { return data->uniformBufs.begin(); }
	ConstUniformBufferIterator getUniformBufferEnd() const { return data->uniformBufs.end(); }
	void applyUniformBuffers(AdvancedShaderProgram* program) const;

	size_t getHash() const { return data->identDataHash; }
	bool operator<(const ShaderPluginRegistry& other) const;
	bool operator==(const ShaderPluginRegistry& other) const;
	bool operator!=(const ShaderPluginRegistry& other) const { return !(*this == other); }

private:
	void updateIdentData();
	void ensureUniqueness();

//private:
public:
	shared_ptr<Data> data;
};

#endif /* SHADERPLUGINREGISTRY_H_ */
