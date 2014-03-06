#ifndef SCRIPTMATRIX4_H_
#define SCRIPTMATRIX4_H_

#include "../ScriptObject.h"
#include <gtaformats/util/math/Matrix4.h>



class ScriptMatrix4 : public ScriptObject
{
	SCRIPTOBJECT_CREATE_HELPER_METHODS(ScriptMatrix4, "Matrix4")

public:
	static void registerClass(lua_State* lua);
	static shared_ptr<ScriptMatrix4> create(lua_State* lua, const Matrix4& mat = Matrix4::Identity);

private:
	ScriptMatrix4(lua_State* lua, const Matrix4& vec) : ScriptObject(lua), mat(vec) {}
	ScriptMatrix4(lua_State* lua) : ScriptObject(lua), mat(Matrix4::Identity) {}

private:
	Matrix4 mat;
};

#endif /* SCRIPTMATRIX4_H_ */
