#ifndef SCRIPTOBJECT_H_
#define SCRIPTOBJECT_H_

#include "luasys.h"
#include <gtaformats/util/CString.h>
#include <boost/shared_ptr.hpp>

using boost::shared_ptr;
using boost::dynamic_pointer_cast;



template <class T>
int _ScriptObject__gc(lua_State* lua)
{
	luaS_getfield(lua, "__instance", 1);
	shared_ptr<T>* sptr = (shared_ptr<T>*) lua_touserdata(lua, -1);

	// The shared_ptr memory is managed by Lua (full userdata), so we don't delete sptr, but we have to call the
	// destructor manually.
	sptr->~shared_ptr();

	return 0;
}




#define SCRIPTOBJECT_CREATE_HELPER_METHODS(cls,scriptclsname) \
	public: \
		static constexpr const char* CLASS_NAME = scriptclsname; \
		static shared_ptr<cls> getInstance(lua_State* lua, int instIdx) { return ScriptObject::getInstance<cls>(lua, instIdx); } \
		static shared_ptr<cls> getSelf(lua_State* lua) { return ScriptObject::getSelf<cls>(lua); } \
		static shared_ptr<cls> checkInstanceof(lua_State* lua, int instIdx) \
				{ return ScriptObject::checkInstanceof<cls>(lua, CLASS_NAME, instIdx); } \
		static void initClass(lua_State* lua) { ScriptObject::initClass<cls>(lua, CLASS_NAME); } \
		static void createClass(lua_State* lua) { ScriptObject::createClass<cls>(lua, CLASS_NAME); }



class ScriptObject
{
protected:
	template <class T>
	static void initClass(lua_State* lua, const CString& clsName);

	template <class T>
	static void createClass(lua_State* lua, const CString& clsName);

	template <class T>
	static shared_ptr<T> createInstance(lua_State* lua, T* sobj) { luaS_newinstance(lua); return initInstance(lua, sobj); }

	template <class T>
	static shared_ptr<T> initInstance(lua_State* lua, T* sobj, int instIdx = -1);

	template <class T>
	static shared_ptr<T> getInstance(lua_State* lua, int instIdx);

	template <class T>
	static shared_ptr<T> getSelf(lua_State* lua) { return getInstance<T>(lua, 1); }

	template <class T>
	static shared_ptr<T> checkInstanceof(lua_State* lua, const CString& clsName, int instIdx);

public:
	virtual ~ScriptObject();

protected:
	ScriptObject(lua_State* lua);

protected:
	lua_State* lua;

private:
	friend int _LuaObject__gc(lua_State* lua);
};








template <class T>
void ScriptObject::initClass(lua_State* lua, const CString& clsName)
{
	lua_pushstring(lua, "__gc");
	lua_pushcfunction(lua, &_ScriptObject__gc<T>);
	lua_settable(lua, -3);
}


template <class T>
void ScriptObject::createClass(lua_State* lua, const CString& clsName)
{
	luaS_createclass(lua, clsName);
	lua_getglobal(lua, clsName.get());
	initClass<T>(lua, clsName);
}


template <class T>
shared_ptr<T> ScriptObject::initInstance(lua_State* lua, T* sobj, int instIdx)
{
	instIdx = lua_absindex(lua, instIdx);
	void* sptrMem = lua_newuserdata(lua, sizeof(shared_ptr<T>));
	shared_ptr<T>* sptr = new(sptrMem) shared_ptr<T>(sobj);
	luaS_setfield(lua, "__instance", instIdx);
	return *sptr;
}


template <class T>
shared_ptr<T> ScriptObject::getInstance(lua_State* lua, int instIdx)
{
	luaS_getfield(lua, "__instance", instIdx);
	shared_ptr<T>* sptr = (shared_ptr<T>*) lua_touserdata(lua, -1);
	lua_pop(lua, 1);
	return *sptr;
}


template <class T>
shared_ptr<T> ScriptObject::checkInstanceof(lua_State* lua, const CString& clsName, int instIdx)
{
	if (luaS_checkinstanceof(lua, clsName, instIdx)) {
		return getInstance<T>(lua, instIdx);
	}

	return shared_ptr<T>();
}

#endif /* SCRIPTOBJECT_H_ */
