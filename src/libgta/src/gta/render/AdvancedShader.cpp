#include "AdvancedShader.h"



AdvancedShader::AdvancedShader(GLenum type, const CString& name)
		: shader(new Shader(type, name))
{
}


void AdvancedShader::compile(const CString& headerCode)
{
	CString combinedCode = headerCode;
	combinedCode += "\n#line 0\n";
	combinedCode += code;
	shader->loadSourceCode(combinedCode);
	shader->compile();
}
