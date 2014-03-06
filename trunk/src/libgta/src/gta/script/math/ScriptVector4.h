#ifndef SCRIPTVECTOR4_H_
#define SCRIPTVECTOR4_H_

#include "../ScriptObject.h"
#include <gtaformats/util/math/Vector4.h>



class ScriptVector4 : public ScriptObject
{
	SCRIPTOBJECT_CREATE_HELPER_METHODS(ScriptVector4, "Vector4")

public:
	static void registerClass(lua_State* lua);
	static shared_ptr<ScriptVector4> create(lua_State* lua, const Vector4& vec = Vector4::Zero);
	Vector4 getVector() const { return vec; }

private:
	ScriptVector4(lua_State* lua, const Vector4& vec) : ScriptObject(lua), vec(vec) {}
	ScriptVector4(lua_State* lua) : ScriptObject(lua), vec(Vector4::Zero) {}

private:
	Vector4 vec;
};

#endif /* SCRIPTVECTOR4_H_ */
