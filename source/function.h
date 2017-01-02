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

#ifndef CLOUD_LUA_CPP_FUNCTION_HEADER
#define CLOUD_LUA_CPP_FUNCTION_HEADER

#include "state.h"

namespace Cloud
{
    class LuaFunctionBase
    {
    public:
        LuaFunctionBase() {};
        LuaFunctionBase(const LuaFunctionBase&) = delete;
        virtual ~LuaFunctionBase() {};
        virtual CLint Invoke(LuaState& state) = 0;
    };

    template <typename _Return, typename... _Args>
    class LuaFunction : public LuaFunctionBase
    {
        using Func = Lua::Function<_Return(_Args...)>;
        using ArgsTuple = std::tuple<_Args...>;

    public:
        LuaFunction(LuaState& state, const CLchar* funcName, const Func& func)
            : LuaFunctionBase()
            , m_state(state)
            , m_funcName(funcName)
            , m_function(func)
        {
            m_state.PushLightUserData(this);
            m_state.PushCClosure(LuaFunction::InvokeBase, 1);
            m_state.SetGlobal(funcName);
        }

        LuaFunction(const LuaFunction&) = delete;

        LuaFunction(LuaFunction&& other)
            : m_state(other.m_state)
            , m_funcName(other.m_funcName)
            , m_function(std::move(other.m_function))
        {}

        ~LuaFunction() override
        {
            m_state.PushNil();
            m_state.SetGlobal(m_funcName);
        }

        template <typename... _Types, CLsize_t... _N>
        static auto GetArgs(LuaState& state, const CLint stackIndex, Indices<_N...>)
        {
            LUACPP_UNUSED(state);
            LUACPP_UNUSED(stackIndex);
            return std::make_tuple(state.To<_Types>(stackIndex + _N)...);
        }

        template <typename... _Types>
        static auto GetArgs(LuaState& state, const CLint stackIndex)
        {
            constexpr auto numArgs = sizeof...(_Types);
            return GetArgs<_Types...>(state, stackIndex, IndicesBuilderType<numArgs>());
        }

        template <typename... _Args, CLsize_t... _N>
        static auto CallFunction(Func&& func, ArgsTuple&& args, Indices<_N...>)
        {
            LUACPP_UNUSED(args);
            return std::forward<Func>(func)(std::get<_N>(std::forward<ArgsTuple>(args))...);
        }

        static auto CallFunction(Func&& func, ArgsTuple&& args)
        {
            return CallFunction(std::forward<Func>(func), std::forward<ArgsTuple>(args), IndicesBuilderType<sizeof...(_Args)>());
        }

        template<typename _Return, CLsize_t _NumArgs, typename... _Args>
        struct Invoker
        {
            static CLint Apply(LuaState& state, Func&& func)
            {
                constexpr auto numArgs = static_cast<CLint>(sizeof...(_Args));
                auto&& args = GetArgs<_Args...>(state, -numArgs);
                auto&& ret = CallFunction(std::forward<Func>(func), std::forward<ArgsTuple>(args));
                state.Push(ret);
                return 1;
            }
        };

        template<CLsize_t _NumArgs, typename... _Args>
        struct Invoker<void, _NumArgs, _Args...>
        {
            static CLint Apply(LuaState& state, Func&& func)
            {
                constexpr auto numArgs = static_cast<CLint>(sizeof...(_Args));
                auto&& args = GetArgs<_Args...>(state, -numArgs);
                CallFunction(std::forward<Func>(func), std::forward<ArgsTuple>(args));
                return 0;
            }
        };

        CLint Invoke(LuaState& state) override
        {
            return Invoker<_Return, sizeof...(_Args), _Args...>::Apply(state, std::forward<Func>(m_function));
        }

        static CLint InvokeBase(lua_State* state)
        {
            auto* func = static_cast<LuaFunction<_Return, _Args...>*>(lua_touserdata(state, lua_upvalueindex(1)));
            return func->Invoke(func->m_state);
        }

    private:
        LuaState& m_state;
        const CLchar* m_funcName;
        Func m_function;
    };
}

#endif // CLOUD_LUA_CPP_FUNCTION_HEADER
