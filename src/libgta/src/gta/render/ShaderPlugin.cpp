#include "ShaderPlugin.h"
#include "../Engine.h"
#include "ShaderPluginAPI.h"



uint32_t ShaderPlugin::nextUniqueID = 0;



ShaderPlugin::ShaderPlugin()
		: uniqueID(nextUniqueID++)
{
	vertexHeaderCode.reserve(4096);
	fragmentHeaderCode.reserve(4096);

	char tmpStr[4096];

	sprintf(tmpStr,
			"#define GTAGL_SHADER_PLUGIN_ID %u\n"
			"#define GTAGL_SHADER_PLUGIN\n\n",
			uniqueID, uniqueID);
	vertexHeaderCode += tmpStr;
	fragmentHeaderCode += tmpStr;

	ShaderPluginAPI* api = Engine::getInstance()->getShaderPluginAPI();

	list<ShaderPluginAPIHook*> hooks;
	api->getVertexHooks(hooks);

	for (list<ShaderPluginAPIHook*>::iterator it = hooks.begin() ; it != hooks.end() ; it++) {
		ShaderPluginAPIHook* hook = *it;

		CString baseName = hook->getBaseName();

		sprintf(tmpStr, "#define %s %s%u\n", baseName.get(), baseName.get(), uniqueID);
		vertexHeaderCode += tmpStr;
	}

	hooks.clear();
	api->getFragmentHooks(hooks);

	for (list<ShaderPluginAPIHook*>::iterator it = hooks.begin() ; it != hooks.end() ; it++) {
		ShaderPluginAPIHook* hook = *it;

		CString baseName = hook->getBaseName();

		sprintf(tmpStr, "#define %s %s%u\n", baseName.get(), baseName.get(), uniqueID);
		fragmentHeaderCode += tmpStr;
	}
}


CString ShaderPlugin::getVertexHeaderCode() const
{
	return vertexHeaderCode;
}


CString ShaderPlugin::getFragmentHeaderCode() const
{
	return fragmentHeaderCode;
}


void ShaderPlugin::applyUniformBuffers(AdvancedShaderProgram* program) const
{
	ConstUniformBufferIterator beg = uniformBufs.begin();
	ConstUniformBufferIterator end = uniformBufs.end();

	for (ConstUniformBufferIterator it = beg ; it != end ; it++) {
		const UniformBuffer* buf = *it;
		buf->apply(program);
	}
}
