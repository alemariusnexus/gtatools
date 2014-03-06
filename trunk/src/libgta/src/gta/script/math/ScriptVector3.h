#ifndef SCRIPTVECTOR3_H_
#define SCRIPTVECTOR3_H_

#include "../luasys.h"
#include "../ScriptObject.h"
#include <gtaformats/util/math/Vector3.h>



class ScriptVector3 : public ScriptObject
{
	SCRIPTOBJECT_CREATE_HELPER_METHODS(ScriptVector3, "Vector3")

public:
	static void registerClass(lua_State* lua);
	static shared_ptr<ScriptVector3> create(lua_State* lua, const Vector3& vec = Vector3::Zero);
	Vector3 getVector() const { return vec; }

private:
	ScriptVector3(lua_State* lua, const Vector3& vec) : ScriptObject(lua), vec(vec) {}
	ScriptVector3(lua_State* lua) : ScriptObject(lua), vec(Vector3::Zero) {}

private:
	Vector3 vec;
};

#endif /* SCRIPTVECTOR3_H_ */
