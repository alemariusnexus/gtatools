#include "ScriptVector3.h"




void ScriptVector3::registerClass(lua_State* lua)
{
	createClass(lua);


	luaS_createmethod(lua, "_init", [] (lua_State* lua) {
		Vector3 vec = Vector3::Zero;

		size_t argc = lua_gettop(lua)-1;

		if (argc == 0) {
		} else if (argc == 1) {
			if (!luaS_instanceof(lua, CLASS_NAME, 2))
				return luaL_error(lua, "Invalid argument for Vector3(vec) constructor!");
			auto osvec = ScriptVector3::getInstance(lua, 2);
			vec = osvec->vec;
		} else if (argc == 3) {
			vec = Vector3(lua_tonumber(lua, 2), lua_tonumber(lua, 3), lua_tonumber(lua, 4));
		}

		ScriptVector3* svec = new ScriptVector3(lua, vec);
		initInstance(lua, svec, 1);
		return 0;
	});

	luaS_createmethod(lua, "__add", [] (lua_State* lua) {
		auto svec = getSelf(lua);
		auto osvec = getInstance(lua, 2);
		create(lua, svec->vec + osvec->vec);
		return 1;
	});

	luaS_createmethod(lua, "__sub", [] (lua_State* lua) {
		auto svec = getSelf(lua);
		auto osvec = getInstance(lua, 2);
		create(lua, svec->vec - osvec->vec);
		return 1;
	});

	luaS_createmethod(lua, "__mul", [] (lua_State* lua) {
		shared_ptr<ScriptVector3> svec;
		int otherIdx;

		if (lua_istable(lua, 1)) {
			svec = getSelf(lua);
			otherIdx = 2;
		} else {
			svec = getInstance(lua, 2);
			otherIdx = 1;
		}

		if (lua_isnumber(lua, otherIdx)) {
			create(lua, svec->vec * lua_tonumber(lua, otherIdx));
			return 1;
		} else if (luaS_instanceof(lua, CLASS_NAME, otherIdx)) {
			auto osvec = getInstance(lua, otherIdx);
			lua_pushnumber(lua, svec->vec.dot(osvec->vec));
			return 1;
		} else {
			return luaL_argerror(lua, otherIdx, "Argument must be a Vector3 or a number!");
		}
	});

	luaS_createmethod(lua, "__tostring", [] (lua_State* lua) {
		auto svec = getSelf(lua);
		char buf[128];
		sprintf(buf, "%f, %f, %f", svec->vec.getX(), svec->vec.getY(), svec->vec.getZ());
		lua_pushstring(lua, buf);
		return 1;
	});

	luaS_createmethod(lua, "get", [] (lua_State* lua) {
		auto svec = getSelf(lua);
		lua_pushnumber(lua, svec->vec.getX());
		lua_pushnumber(lua, svec->vec.getY());
		lua_pushnumber(lua, svec->vec.getZ());
		return 3;
	});

	luaS_createmethod(lua, "getX", [] (lua_State* lua) {
		auto svec = getSelf(lua);
		lua_pushnumber(lua, svec->vec.getX());
		return 1;
	});

	luaS_createmethod(lua, "getY", [] (lua_State* lua) {
		auto svec = getSelf(lua);
		lua_pushnumber(lua, svec->vec.getX());
		return 1;
	});

	luaS_createmethod(lua, "getZ", [] (lua_State* lua) {
		auto svec = getSelf(lua);
		lua_pushnumber(lua, svec->vec.getX());
		return 1;
	});

	luaS_createmethod(lua, "set", [] (lua_State* lua) {
		auto svec = getSelf(lua);
		float x = luaL_checknumber(lua, 2);
		float y = luaL_checknumber(lua, 3);
		float z = luaL_checknumber(lua, 4);
		svec->vec = Vector3(x, y, z);
		return 0;
	});

	luaS_createmethod(lua, "dot", [] (lua_State* lua) {
		auto svec = getSelf(lua);
		auto osvec = checkInstanceof(lua, 2);
		lua_pushnumber(lua, svec->vec.dot(osvec->vec));
		return 1;
	});

	luaS_createmethod(lua, "cross", [] (lua_State* lua) {
		auto svec = getSelf(lua);
		auto osvec = checkInstanceof(lua, 2);
		create(lua, svec->vec.cross(osvec->vec));
		return 1;
	});

	luaS_createmethod(lua, "length", [] (lua_State* lua) {
		auto svec = getSelf(lua);
		lua_pushnumber(lua, svec->vec.length());
		return 1;
	});

	luaS_createmethod(lua, "normalize", [] (lua_State* lua) {
		auto svec = getSelf(lua);
		svec->vec.normalize();
		return 0;
	});

	luaS_createmethod(lua, "normalized", [] (lua_State* lua) {
		auto svec = getSelf(lua);
		Vector3 vec = svec->vec;
		vec.normalize();
		create(lua, vec);
		return 1;
	});
}


shared_ptr<ScriptVector3> ScriptVector3::create(lua_State* lua, const Vector3& vec)
{
	lua_getglobal(lua, CLASS_NAME);
	ScriptVector3* inst = new ScriptVector3(lua, vec);
	return createInstance(lua, inst);
}
