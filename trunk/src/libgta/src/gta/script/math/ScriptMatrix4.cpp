/*
	Copyright 2010-2014 David "Alemarius Nexus" Lerch

	This file is part of libgta.

	libgta is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	libgta is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with libgta.  If not, see <http://www.gnu.org/licenses/>.

	Additional permissions are granted, which are listed in the file
	GPLADDITIONS.
 */

#include "ScriptMatrix4.h"
#include "ScriptVector4.h"
#include "ScriptVector3.h"




void ScriptMatrix4::registerClass(lua_State* lua)
{
	createClass(lua);


	luaS_createmethod(lua, "_init", [] (lua_State* lua) {
		//void* mem = initInstance(lua, sizeof(ScriptMatrix4), 1);
		Matrix4 mat = Matrix4::Identity;

		size_t argc = lua_gettop(lua)-1;

		if (argc == 0) {
		} else if (argc == 1) {
			if (!luaS_instanceof(lua, CLASS_NAME, 2))
				return luaL_error(lua, "Invalid argument for Matrix4(mat4) constructor!");
			shared_ptr<ScriptMatrix4> osmat = getInstance(lua, 2);
			mat = osmat->mat;
		} else if (argc == 4) {
			if (	luaS_instanceof(lua, "Vector4", 2)
					&&  luaS_instanceof(lua, "Vector4", 3)
					&&  luaS_instanceof(lua, "Vector4", 4)
					&&  luaS_instanceof(lua, "Vector4", 5)
			) {
				shared_ptr<ScriptVector4> v1 = ScriptVector4::getInstance(lua, 2);
				shared_ptr<ScriptVector4> v2 = ScriptVector4::getInstance(lua, 3);
				shared_ptr<ScriptVector4> v3 = ScriptVector4::getInstance(lua, 4);
				shared_ptr<ScriptVector4> v4 = ScriptVector4::getInstance(lua, 5);

				mat = Matrix4 (
						v1->getVector().getX(), v1->getVector().getY(), v1->getVector().getZ(), v1->getVector().getW(),
						v2->getVector().getX(), v2->getVector().getY(), v2->getVector().getZ(), v2->getVector().getW(),
						v3->getVector().getX(), v3->getVector().getY(), v3->getVector().getZ(), v3->getVector().getW(),
						v4->getVector().getX(), v4->getVector().getY(), v4->getVector().getZ(), v4->getVector().getW()
				);
			} else {
				return luaL_error(lua, "Invalid parameters for Matrix4 constructor!");
			}
		} else if (argc == 16) {
			mat = Matrix4 (
					luaL_checknumber(lua, 2), luaL_checknumber(lua, 3), luaL_checknumber(lua, 4), luaL_checknumber(lua, 5),
					luaL_checknumber(lua, 6), luaL_checknumber(lua, 7), luaL_checknumber(lua, 8), luaL_checknumber(lua, 9),
					luaL_checknumber(lua, 10), luaL_checknumber(lua, 11), luaL_checknumber(lua, 12), luaL_checknumber(lua, 13),
					luaL_checknumber(lua, 14), luaL_checknumber(lua, 15), luaL_checknumber(lua, 16), luaL_checknumber(lua, 17)
			);
		}

		ScriptMatrix4* smat = new ScriptMatrix4(lua, mat);
		initInstance(lua, smat, 1);
		//ScriptMatrix4* smat = new(mem) ScriptMatrix4(lua, mat);
		return 0;
	});

	luaS_createmethod(lua, "__add", [] (lua_State* lua) {
		shared_ptr<ScriptMatrix4> smat = getSelf(lua);
		shared_ptr<ScriptMatrix4> osmat = checkInstanceof(lua, 2);
		create(lua, smat->mat + osmat->mat);
		return 1;
	});

	luaS_createmethod(lua, "__sub", [] (lua_State* lua) {
		shared_ptr<ScriptMatrix4> smat = getSelf(lua);
		shared_ptr<ScriptMatrix4> osmat = getInstance(lua, 2);
		create(lua, smat->mat - osmat->mat);
		return 1;
	});

	luaS_createmethod(lua, "__mul", [] (lua_State* lua) {
		shared_ptr<ScriptMatrix4> smat;
		int otherIdx;

		if (lua_istable(lua, 1)  &&  luaS_instanceof(lua, CLASS_NAME, 1)) {
			smat = getSelf(lua);
			otherIdx = 2;
		} else {
			smat = getInstance(lua, 2);
			otherIdx = 1;
		}

		if (lua_isnumber(lua, otherIdx)) {
			create(lua, smat->mat * lua_tonumber(lua, otherIdx));
			return 1;
		} else if (luaS_instanceof(lua, CLASS_NAME, otherIdx)) {
			shared_ptr<ScriptMatrix4> osmat = getInstance(lua, otherIdx);
			Matrix4 resMat = otherIdx == 2 ? smat->mat * osmat->mat : osmat->mat * smat->mat;
			create(lua, resMat);
			return 1;
		} else if (luaS_instanceof(lua, "Vector4", otherIdx)) {
			shared_ptr<ScriptVector4> osvec = ScriptVector4::getInstance(lua, otherIdx);
			ScriptVector4::create(lua, smat->mat * osvec->getVector());
			return 1;
		} else {
			return luaL_argerror(lua, otherIdx, "Argument must be a Matrix4 or a number!");
		}
	});

	luaS_createmethod(lua, "__tostring", [] (lua_State* lua) {
		shared_ptr<ScriptMatrix4> smat = getSelf(lua);
		const float* m = smat->mat.toArray();
		char buf[1024];
		sprintf(buf, "((%f, %f, %f, %f), (%f, %f, %f, %f), (%f, %f, %f, %f), (%f, %f, %f, %f))",
				m[0], m[1], m[2], m[3],
				m[4], m[5], m[6], m[7],
				m[8], m[9], m[10], m[11],
				m[12], m[13], m[14], m[15]);
		lua_pushstring(lua, buf);
		return 1;
	});

	luaS_createmethod(lua, "get", [] (lua_State* lua) {
		shared_ptr<ScriptMatrix4> smat = getSelf(lua);
		const float* m = smat->mat.toArray();

		if (lua_isnumber(lua, 2)) {
			unsigned int i = luaS_checkuintrange(lua, 2, 1, 16);
			lua_pushnumber(lua, smat->mat.toArray()[i-1]);
			return 1;
		} else {
			lua_pushnumber(lua, m[0]);
			lua_pushnumber(lua, m[1]);
			lua_pushnumber(lua, m[2]);
			lua_pushnumber(lua, m[3]);
			lua_pushnumber(lua, m[4]);
			lua_pushnumber(lua, m[5]);
			lua_pushnumber(lua, m[6]);
			lua_pushnumber(lua, m[7]);
			lua_pushnumber(lua, m[8]);
			lua_pushnumber(lua, m[9]);
			lua_pushnumber(lua, m[10]);
			lua_pushnumber(lua, m[11]);
			lua_pushnumber(lua, m[12]);
			lua_pushnumber(lua, m[13]);
			lua_pushnumber(lua, m[14]);
			lua_pushnumber(lua, m[15]);
			return 16;
		}
	});

	luaS_createmethod(lua, "getColumn", [] (lua_State* lua) {
		shared_ptr<ScriptMatrix4> smat = getSelf(lua);
		unsigned int col = luaS_checkuintrange(lua, 2, 1, 4);
		ScriptVector4::create(lua, smat->mat.column(col-1));
		return 1;
	});

	luaS_createmethod(lua, "getColumns", [] (lua_State* lua) {
		shared_ptr<ScriptMatrix4> smat = getSelf(lua);
		ScriptVector4::create(lua, smat->mat.column(0));
		ScriptVector4::create(lua, smat->mat.column(1));
		ScriptVector4::create(lua, smat->mat.column(2));
		ScriptVector4::create(lua, smat->mat.column(3));
		return 4;
	});

	luaS_createmethod(lua, "getRow", [] (lua_State* lua) {
		shared_ptr<ScriptMatrix4> smat = getSelf(lua);
		unsigned int row = luaS_checkuintrange(lua, 2, 1, 4);
		ScriptVector4::create(lua, smat->mat.row(row-1));
		return 1;
	});

	luaS_createmethod(lua, "getRows", [] (lua_State* lua) {
		shared_ptr<ScriptMatrix4> smat = getSelf(lua);
		ScriptVector4::create(lua, smat->mat.row(0));
		ScriptVector4::create(lua, smat->mat.row(1));
		ScriptVector4::create(lua, smat->mat.row(2));
		ScriptVector4::create(lua, smat->mat.row(3));
		return 4;
	});

	luaS_createmethod(lua, "set", [] (lua_State* lua) {
		shared_ptr<ScriptMatrix4> smat = getSelf(lua);
		unsigned int argc = lua_gettop(lua)-1;

		if (argc == 2) {
			unsigned int i = luaS_checkuintrange(lua, 2, 1, 16);
			float val = luaL_checknumber(lua, 3);
			smat->mat.toArray()[i-1] = val;
			return 0;
		} else if (argc == 16) {
			Matrix4 mat (
					luaL_checknumber(lua, 2), luaL_checknumber(lua, 3), luaL_checknumber(lua, 4), luaL_checknumber(lua, 5),
					luaL_checknumber(lua, 6), luaL_checknumber(lua, 7), luaL_checknumber(lua, 8), luaL_checknumber(lua, 9),
					luaL_checknumber(lua, 10), luaL_checknumber(lua, 11), luaL_checknumber(lua, 12), luaL_checknumber(lua, 13),
					luaL_checknumber(lua, 14), luaL_checknumber(lua, 15), luaL_checknumber(lua, 16), luaL_checknumber(lua, 17)
			);
			smat->mat = mat;
			return 0;
		} else {
			return luaL_error(lua, "Invalid number of arguments for Matrix4::set()");
		}
	});

	luaS_createmethod(lua, "transpose", [] (lua_State* lua) {
		shared_ptr<ScriptMatrix4> smat = getSelf(lua);
		smat->mat.transpose();
		return 0;
	});

	luaS_createmethod(lua, "transposed", [] (lua_State* lua) {
		shared_ptr<ScriptMatrix4> smat = getSelf(lua);
		Matrix4 mat = smat->mat;
		mat.transpose();
		create(lua, mat);
		return 1;
	});

	luaS_createmethod(lua, "invert", [] (lua_State* lua) {
		shared_ptr<ScriptMatrix4> smat = getSelf(lua);
		smat->mat.invert();
		return 0;
	});

	luaS_createmethod(lua, "inverted", [] (lua_State* lua) {
		shared_ptr<ScriptMatrix4> smat = getSelf(lua);
		Matrix4 mat = smat->mat;
		mat.invert();
		create(lua, mat);
		return 1;
	});

	luaS_createmethod(lua, "determinant", [] (lua_State* lua) {
		shared_ptr<ScriptMatrix4> smat = getSelf(lua);
		lua_pushnumber(lua, smat->mat.determinant());
		return 1;
	});


	luaS_createmethod(lua, "translation", [] (lua_State* lua) {
		unsigned int argc = lua_gettop(lua);

		Vector3 trans;

		if (argc == 1) {
			if (luaS_instanceof(lua, "Vector3", 1)) {
				auto svec = ScriptVector3::getInstance(lua, 1);
				trans = Vector4(svec->getVector());
			} else {
				luaL_argerror(lua, 1, "The single argument form takes a Vector3 only!");
			}
		} else if (argc == 3) {
			trans = Vector3(luaL_checknumber(lua, 1), luaL_checknumber(lua, 2), luaL_checknumber(lua, 3));
		} else {
			return luaL_error(lua, "Invalid number of arguments for Matrix4:translation()");
		}

		create(lua, Matrix4::translation(trans));
		return 1;
	});

	luaS_createmethod(lua, "rotationAxisAngle", [] (lua_State* lua) {
		unsigned int argc = lua_gettop(lua);

		Matrix4 mat;

		if (argc == 2) {
			auto svec = ScriptVector3::checkInstanceof(lua, 1);
			float angle = luaL_checknumber(lua, 2);

			mat = Matrix4::rotation(angle, svec->getVector());
		} else if (argc == 4) {
			mat = Matrix4::rotation(luaL_checknumber(lua, 4), luaL_checknumber(lua, 1), luaL_checknumber(lua, 2),
					luaL_checknumber(lua, 3));
		}

		create(lua, mat);
		return 1;
	});

	luaS_createmethod(lua, "rotationX", [] (lua_State* lua) {
		float angle = luaL_checknumber(lua, 1);
		create(lua, Matrix4::rotationX(angle));
		return 1;
	});

	luaS_createmethod(lua, "rotationY", [] (lua_State* lua) {
		float angle = luaL_checknumber(lua, 1);
		create(lua, Matrix4::rotationY(angle));
		return 1;
	});

	luaS_createmethod(lua, "rotationZ", [] (lua_State* lua) {
		float angle = luaL_checknumber(lua, 1);
		create(lua, Matrix4::rotationZ(angle));
		return 1;
	});

	luaS_createmethod(lua, "scale", [] (lua_State* lua) {
		unsigned int argc = lua_gettop(lua);

		Vector3 scale;

		if (argc == 1) {
			auto svec = ScriptVector3::checkInstanceof(lua, 1);
			scale = svec->getVector();
		} else if (argc == 3) {
			scale = Vector3(luaL_checknumber(lua, 1), luaL_checknumber(lua, 2), luaL_checknumber(lua, 3));
		}

		create(lua, Matrix4::scale(scale));
		return 1;
	});
}


shared_ptr<ScriptMatrix4> ScriptMatrix4::create(lua_State* lua, const Matrix4& mat)
{
	lua_getglobal(lua, CLASS_NAME);
	ScriptMatrix4* smat = new ScriptMatrix4(lua, mat);
	return createInstance(lua, smat);
}
