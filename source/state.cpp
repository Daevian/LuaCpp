/*
* LuaCpp
* 
* Copyright (c) 2016 Robin Doeleman
* 
* Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"),
* to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
* and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
* 
* The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
* 
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, 
* DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE 
* OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "state.h"

Cloud::LuaState::LuaState()
{
    m_state = Lua::NewStateAndSetup();
}

Cloud::LuaState::LuaState(LuaState&& other)
{
    m_state = std::move(other.m_state);
}

void Cloud::LuaState::Register(const CLchar* funcName, lua_CFunction func)
{
    lua_register(GetState(), funcName, func);
}

CLbool Cloud::LuaState::CheckStack(CLint requiredStackSlots)
{
    return lua_checkstack(GetState(), requiredStackSlots) != 0 ? true : false;
}

CLint Cloud::LuaState::GetTop() const
{
    return lua_gettop(GetState());
}

void Cloud::LuaState::SetTop(CLint stackIndex)
{
    lua_settop(GetState(), stackIndex);
}

Cloud::Lua::Type Cloud::LuaState::Type(CLint stackIndex) const
{
    return static_cast<Lua::Type>(lua_type(GetState(), stackIndex));
}

const CLchar* Cloud::LuaState::Typename(CLint stackIndex) const
{
    return luaL_typename(GetState(), stackIndex);
}

void Cloud::LuaState::Push(CLbool value)
{
    lua_pushboolean(GetState(), value ? 1 : 0);
}

void Cloud::LuaState::Push(CLint value)
{
    lua_pushinteger(GetState(), static_cast<lua_Integer>(value));
}

void Cloud::LuaState::Push(CLfloat value)
{
    lua_pushnumber(GetState(), static_cast<lua_Number>(value));
}

const CLchar* Cloud::LuaState::Push(const CLchar* value)
{
    return lua_pushstring(GetState(), value);
}

void Cloud::LuaState::Pop(CLint numElements)
{
    lua_pop(GetState(), numElements);
}

void Cloud::LuaState::Remove(CLint stackIndex)
{
    lua_remove(GetState(), stackIndex);
}

void Cloud::LuaState::PushValue(CLint sourceIndex)
{
    lua_pushvalue(GetState(), sourceIndex);
}

void Cloud::LuaState::Insert(CLint targetIndex)
{
    lua_insert(GetState(), targetIndex);
}

void Cloud::LuaState::Replace(CLint targetIndex)
{
    lua_replace(GetState(), targetIndex);
}

void Cloud::LuaState::CreateTable(CLint narr, CLint nrec)
{
    lua_createtable(GetState(), narr, nrec);
}

Cloud::Lua::Type Cloud::LuaState::GetTable(CLint stackIndex)
{
    return static_cast<Lua::Type>(GetState(), stackIndex);
}

void Cloud::LuaState::SetTable(CLint stackIndex)
{
    // todo: check if table
    lua_settable(GetState(), stackIndex);
}

Cloud::Lua::Type Cloud::LuaState::GetField(CLint stackIndex, const CLchar* key)
{
    return static_cast<Lua::Type>(lua_getfield(GetState(), stackIndex, key));
}

void Cloud::LuaState::SetField(CLint targetIndex, const CLchar* key)
{
    // todo: check if table
    lua_setfield(GetState(), targetIndex, key);
}

CLbool Cloud::LuaState::GetMetatable(CLint stackIndex)
{
    return lua_getmetatable(GetState(), stackIndex) != 0 ? true : false;
}

void Cloud::LuaState::SetMetatable(CLint stackIndex)
{
    lua_setmetatable(GetState(), stackIndex);
}

Cloud::Lua::Type Cloud::LuaState::GetGlobal(const CLchar* name)
{
    return static_cast<Lua::Type>(lua_getglobal(GetState(), name));
}

void Cloud::LuaState::SetGlobal(const CLchar* name)
{
    lua_setglobal(GetState(), name);
}

Cloud::Lua::Type Cloud::LuaState::RawGet(CLint stackIndex)
{
    return static_cast<Lua::Type>(lua_rawget(GetState(), stackIndex));
}

void Cloud::LuaState::RawSet(CLint stackIndex)
{
    lua_rawset(GetState(), stackIndex);
}

Cloud::Lua::Type Cloud::LuaState::RawGetI(CLint stackIndex, CLint tableIndex)
{
    return static_cast<Lua::Type>(lua_rawgeti(GetState(), stackIndex, tableIndex));
}

void Cloud::LuaState::RawSetI(CLint stackIndex, CLint tableIndex)
{
    lua_rawseti(GetState(), stackIndex, tableIndex);
}

CLsize_t Cloud::LuaState::RawLen(CLint stackIndex) const
{
    return lua_rawlen(GetState(), stackIndex);
}

CLbool Cloud::LuaState::Next(CLint stackIndex)
{
    return lua_next(GetState(), stackIndex) != 0 ? true : false;
}

Cloud::Lua::ErrorCode Cloud::LuaState::LoadFile(const CLchar* fileName)
{
    Lua::ErrorCode result = static_cast<Lua::ErrorCode>(luaL_loadfile(GetState(), fileName));

    if (result == Lua::ErrorCode::ErrFile)
    {
        if (lua_isstring(GetState(), -1))
        {
            const char* err = lua_tostring(GetState(), -1);
            LUACPP_TRACE("Lua file error:\n%s", err);
        }
    }

    if (result == Lua::ErrorCode::ErrSyntax)
    {
        if (lua_isstring(GetState(), -1))
        {
            const char* err = lua_tostring(GetState(), -1);
            LUACPP_TRACE("Lua syntax error:\n%s", err);
        }
    }

    return result;
}

Cloud::Lua::ErrorCode Cloud::LuaState::DoFile(const CLchar* fileName)
{
    Lua::ErrorCode result;
    result = LoadFile(fileName);
    if (result != Lua::ErrorCode::Ok)
    {
        return result;
    }

    result = PCall();
    return result;
}

Cloud::Lua::ErrorCode Cloud::LuaState::PCall(CLint argCount, CLint retArgCount)
{
    Lua::ErrorCode result = static_cast<Lua::ErrorCode>(lua_pcall(GetState(), argCount, retArgCount, 0));

    if (result == Lua::ErrorCode::ErrRun)
    {
        if (lua_isstring(GetState(), -1))
        {
            const char* err = lua_tostring(GetState(), -1);
            LUACPP_TRACE("Lua run error:\n%s", err);
        }
    }

    return result;
}