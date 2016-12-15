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

#ifndef CLOUD_LUA_CPP_STATE_HEADER
#define CLOUD_LUA_CPP_STATE_HEADER

#include "luacpp.h"

namespace Cloud
{
    class LuaState
    {
    public:
        LuaState();
        LuaState(const LuaState&) = delete;
        LuaState(LuaState&& other);
        virtual ~LuaState() {};

        void Register(const CLchar* funcName, lua_CFunction func);

        CLbool          CheckStack(CLint requiredStackSlots); // TODO: test behaviour
        CLint           GetTop() const;
        void            SetTop(CLint stackIndex);

        Lua::Type       Type(CLint stackIndex) const;
        const CLchar*   Typename(CLint stackIndex) const;

        template <Lua::Type _T>
        CLbool          IsType(CLint stackIndex) const
        {
            return Type(stackIndex) == _T;
        }

        template <class _T>
        _T* ToUserData(CLint stackIndex) const
        {
            return static_cast<_T*>(lua_touserdata(GetState(), stackIndex));
        }

        template <typename _T>
        _T            To(CLint stackIndex) const
        {
            return ToUserData<std::remove_pointer_t<_T>>(stackIndex);
        }

        CLint UpValueIndex(CLint upValueIndex) const
        {
            return lua_upvalueindex(upValueIndex);
        }

        template <typename _T>
        _T            Opt(CLint stackIndex, const _T& defaultValue) const;

        template <class _T>
        void PushLightUserData(_T* pointer)
        {
            lua_pushlightuserdata(GetState(), pointer);
        }

        void PushCClosure(lua_CFunction func, CLint numUpValues)
        {
            lua_pushcclosure(GetState(), func, numUpValues);
        }

        void PushNil()
        {
            lua_pushnil(GetState());
        }

        void            Push() {}
        void            Push(CLbool value);
        void            Push(CLint value);
        void            Push(CLfloat value);
        const CLchar*   Push(const CLchar* value);

        template <typename _T>
        void Push(_T* value)
        {
            PushLightUserData(value);
        }

        template<typename _FirstArg, typename... _MoreArgs>
        void            Push(_FirstArg&& firstArg, _MoreArgs&&... moreArgs)
        {
            Push(std::forward<_FirstArg>(firstArg));
            Push(std::forward<_MoreArgs>(moreArgs)...);
        }

        void            Pop(CLint numElements); // TODO: what happens with negative values?
        void            Remove(CLint stackIndex); // TODO: what happens OOB?

        void            PushValue(CLint sourceIndex); // TODO: what happens when indexing OOB?
        void            Insert(CLint targetIndex); // TODO: test what happens if inserting outside stack
        void            Replace(CLint targetIndex); // TODO: test what happens if inserting outside stack

        void            CreateTable(CLint narr = 0, CLint nrec = 0); // TODO: narr and nrec means? and can they be negative...?

        Lua::Type       GetTable(CLint stackIndex);
        void            SetTable(CLint stackIndex);

        Lua::Type       GetField(CLint stackIndex, const CLchar* key);
        void            SetField(CLint stackIndex, const CLchar* key);

        CLbool          GetMetatable(CLint stackIndex);
        void            SetMetatable(CLint stackIndex);

        Lua::Type       GetGlobal(const CLchar* name);
        void            SetGlobal(const CLchar* name);

        void            PushGlobal() // todo: implement
        {
            // push value
            // set global
        }

        Lua::Type       RawGet(CLint stackIndex);
        void            RawSet(CLint stackIndex);
        Lua::Type       RawGetI(CLint stackIndex, CLint tableIndex);
        void            RawSetI(CLint stackIndex, CLint tableIndex);
        CLsize_t        RawLen(CLint stackIndex) const;

        CLbool          Next(CLint stackIndex);

        Lua::ErrorCode LoadFile(const CLchar* fileName);
        Lua::ErrorCode DoFile(const CLchar* fileName);
        Lua::ErrorCode PCall(CLint argCount = 0, CLint retArgCount = LUA_MULTRET);

    protected:
        lua_State* GetState() const { return m_state.get(); }

    private:
        Lua::StateUniquePtr m_state;

    };
}

#include "state.inl"

#endif // CLOUD_LUA_CPP_STATE_HEADER
