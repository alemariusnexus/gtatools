#ifndef DPTESTSHADERPLUGIN_H_
#define DPTESTSHADERPLUGIN_H_

#include "ShaderPlugin.h"



class DPTestShaderPlugin : public ShaderPlugin
{
private:
	static const char* hooks[];

public:
	DPTestShaderPlugin();
	virtual ~DPTestShaderPlugin();
	virtual uint32_t getFlags() const;
	virtual bool usesHook(const CString& baseName) const;
	virtual Shader* getVertexShader() const;
	virtual Shader* getFragmentShader() const;

private:
	Shader* vshader;
	Shader* fshader;
};

#endif /* DPTESTSHADERPLUGIN_H_ */
