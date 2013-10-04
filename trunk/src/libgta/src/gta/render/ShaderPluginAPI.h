#ifndef SHADERPLUGINAPI_H_
#define SHADERPLUGINAPI_H_

#include "ShaderPluginAPIHook.h"
#include <list>

using std::list;



class ShaderPluginAPI
{
public:
	virtual ~ShaderPluginAPI() {}
	virtual void getVertexHooks(list<ShaderPluginAPIHook*>& hooks) = 0;
	virtual void getFragmentHooks(list<ShaderPluginAPIHook*>& hooks) = 0;
};

#endif /* SHADERPLUGINAPI_H_ */
