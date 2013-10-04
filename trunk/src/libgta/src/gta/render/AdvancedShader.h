#ifndef ADVANCEDSHADER_H_
#define ADVANCEDSHADER_H_

#include <gta/config.h>
#include <gtaformats/util/CString.h>
#include "../Shader.h"
#include "../gl.h"


class AdvancedShader
{
public:
	AdvancedShader(GLenum type, const CString& name = CString());
	void setSourceCode(const CString& code) { this->code = code; }

private:
	void compile(const CString& headerCode);

private:
	Shader* shader;
	CString code;

	friend class AdvancedShaderProgram;
};

#endif /* ADVANCEDSHADER_H_ */
