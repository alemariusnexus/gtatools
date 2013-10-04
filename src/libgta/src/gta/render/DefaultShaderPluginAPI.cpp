#include "DefaultShaderPluginAPI.h"




DefaultShaderPluginAPI::DefaultShaderPluginAPI()
{
	defineVertexHook("VertexPostProcessing", "inout vec4 vertex", "vertex");
	defineVertexHook("VertexPreProcessing", "inout vec4 vertex", "vertex");
	defineVertexHook("VertexPostModelTransformationProcessing", "inout vec4 vertex", "vertex");

	defineFragmentHook("FragmentPostProcessing", "inout vec4 frag", "frag");
	defineFragmentHook("FragmentPreProcessing", "inout vec4 frag", "frag");

	defineFragmentHook("DPBlendLayerFragmentPostProcessing", "inout vec4 frag", "frag");
}


void DefaultShaderPluginAPI::defineVertexHook(const CString& baseName, const CString& paramList, const CString& paramNameList)
{
	ShaderPluginAPIHook* hook = new ShaderPluginAPIHook(baseName, paramList, paramNameList);
	vertexHooks.push_back(hook);
}


void DefaultShaderPluginAPI::defineFragmentHook(const CString& baseName, const CString& paramList, const CString& paramNameList)
{
	ShaderPluginAPIHook* hook = new ShaderPluginAPIHook(baseName, paramList, paramNameList);
	fragmentHooks.push_back(hook);
}


void DefaultShaderPluginAPI::getVertexHooks(list<ShaderPluginAPIHook*>& hooks)
{
	hooks = vertexHooks;
}


void DefaultShaderPluginAPI::getFragmentHooks(list<ShaderPluginAPIHook*>& hooks)
{
	hooks = fragmentHooks;
}
