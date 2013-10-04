#ifndef TESTSHADERPLUGIN_H_
#define TESTSHADERPLUGIN_H_

#include "ShaderPlugin.h"



class TestShaderPlugin : public ShaderPlugin
{
private:
	static const char* hooks[];

public:
	TestShaderPlugin(uint32_t num);
	virtual ~TestShaderPlugin();
	virtual uint32_t getFlags() const;
	virtual bool usesHook(const CString& baseName) const;
	virtual Shader* getVertexShader() const;
	virtual Shader* getFragmentShader() const;

private:
	Shader* vshader;
	Shader* fshader;
};

#endif /* TESTSHADERPLUGIN_H_ */
