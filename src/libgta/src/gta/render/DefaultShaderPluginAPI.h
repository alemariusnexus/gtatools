#ifndef DEFAULTSHADERPLUGINAPI_H_
#define DEFAULTSHADERPLUGINAPI_H_

#include "ShaderPluginAPI.h"
#include "ShaderPluginAPIHook.h"



class DefaultShaderPluginAPI : public ShaderPluginAPI
{
public:
	DefaultShaderPluginAPI();
	virtual void getVertexHooks(list<ShaderPluginAPIHook*>& hooks);
	virtual void getFragmentHooks(list<ShaderPluginAPIHook*>& hooks);

private:
	void defineVertexHook(const CString& baseName, const CString& paramList, const CString& paramNameList);
	void defineFragmentHook(const CString& baseName, const CString& paramList, const CString& paramNameList);

private:
	list<ShaderPluginAPIHook*> vertexHooks;
	list<ShaderPluginAPIHook*> fragmentHooks;
};

#endif /* DEFAULTSHADERPLUGINAPI_H_ */
