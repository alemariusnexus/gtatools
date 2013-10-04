#ifndef SHADERPLUGIN_H_
#define SHADERPLUGIN_H_

#include "../Shader.h"
#include "UniformBuffer.h"



class ShaderPlugin
{
public:
	typedef list<UniformBuffer*> UniformBufferList;
	typedef UniformBufferList::iterator UniformBufferIterator;
	typedef UniformBufferList::const_iterator ConstUniformBufferIterator;

public:
	enum Flags
	{
		UsesVertexShader = 1 << 0,
		UsesFragmentShader = 1 << 1
	};

public:
	ShaderPlugin();
	virtual ~ShaderPlugin() {}
	virtual uint32_t getFlags() const = 0;
	virtual uint32_t getUniqueID() const { return uniqueID; }
	virtual bool usesHook(const CString& baseName) const = 0;
	virtual Shader* getVertexShader() const { return NULL; }
	virtual Shader* getFragmentShader() const { return NULL; }

	void addUniformBuffer(UniformBuffer* buf) { uniformBufs.push_back(buf); }
	UniformBufferIterator getUniformBufferBegin() { return uniformBufs.begin(); }
	UniformBufferIterator getUniformBufferEnd() { return uniformBufs.end(); }
	ConstUniformBufferIterator getUniformBufferBegin() const { return uniformBufs.begin(); }
	ConstUniformBufferIterator getUniformBufferEnd() const { return uniformBufs.end(); }
	void applyUniformBuffers(AdvancedShaderProgram* program) const;

protected:
	CString getVertexHeaderCode() const;
	CString getFragmentHeaderCode() const;

protected:
	uint32_t uniqueID;
	CString vertexHeaderCode;
	CString fragmentHeaderCode;
	UniformBufferList uniformBufs;

private:
	static uint32_t nextUniqueID;
};

#endif /* SHADERPLUGIN_H_ */
