#ifndef SHADERPLUGINAPIHOOK_H_
#define SHADERPLUGINAPIHOOK_H_

#include <gta/config.h>
#include <gtaformats/util/CString.h>



class ShaderPluginAPIHook
{
public:
	ShaderPluginAPIHook(const CString& baseName, const CString& paramList, const CString& paramNameList);
	CString getBaseName() { return baseName; }
	CString getParameterList() { return paramList; }
	CString getParameterNameList() { return paramNameList; }

private:
	CString baseName;
	CString paramList;
	CString paramNameList;
};

#endif /* SHADERPLUGINAPIHOOK_H_ */
