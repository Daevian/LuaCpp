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

#ifndef CLOUD_LUA_CPP_STATE_EX_HEADER
#define CLOUD_LUA_CPP_STATE_EX_HEADER

#include "function.h"
#include "stack_sentry.h"
#include "config.h"

namespace Cloud
{
    class LuaStateEx : public LuaState
    {
    public:
        LuaStateEx();
        LuaStateEx(const LuaStateEx&) = delete;
        LuaStateEx(LuaStateEx&& other);
        virtual ~LuaStateEx() override {};

        template <typename _Return, typename... _Args>
        void RegisterFunction(const CLchar* funcName, const Lua::Function<_Return(_Args...)>& func)
        {
            LuaStackSentry sentry(*this);

            auto luaFunc = Lua::MakeUnique<LuaFunction<_Return, _Args...>>(*this, funcName, func);
            m_functions[funcName] = std::move(luaFunc);
        }

        template <typename _Return>
        void RegisterFunction(const CLchar* funcName, const Lua::Function<_Return()>& func)
        {
            LuaStackSentry sentry(*this);

            auto luaFunc = Lua::MakeUnique<LuaFunction<_Return>>(*this, funcName, func);
            m_functions[funcName] = std::move(luaFunc);
        }

        template <CLsize_t, typename... _Types>
        struct ReadbackTypeTrait
        {
            template <typename _T>
            static auto Readback(const LuaStateEx& state, const CLint stackIndex)
            {
                auto value = std::make_tuple(state.To<_T>(stackIndex));
                return value;
            }

            template <typename _T0, typename _T1, typename... _Rest>
            static auto Readback(const LuaStateEx& state, const CLint stackIndex)
            {
                auto first = std::make_tuple(state.To<_T0>(stackIndex));
                return std::tuple_cat(first, Readback<_T1, _Rest...>(state, stackIndex + 1));
            }

            static auto Apply(LuaStateEx& state)
            {
                constexpr CLint elementCount = static_cast<CLint>(sizeof...(_Types));
                auto value = Readback<_Types...>(state, -elementCount);
                state.Pop(sizeof...(_Types));
                return value;
            }
        };

        template <typename _T>
        struct ReadbackTypeTrait<1, _T>
        {
            static auto Apply(LuaStateEx& state)
            {
                auto value = state.To<_T>(-1);
                state.Pop(1);
                return value;
            }
        };

        template <typename... _Types>
        struct ReadbackTypeTrait<0, _Types...>
        {
            static void Apply(LuaStateEx&) {}
        };

        template <typename... _Types>
        auto PopReturn()
        {
            return ReadbackTypeTrait<sizeof...(_Types), _Types...>::Apply(*this);
        }

        template<typename... _ReturnArgs, typename... _Args>
        auto Call(const CLchar* functionName, _Args&&... args)
        {
            LuaStackSentry sentry(*this);

            const auto type = GetGlobal(functionName);
            if (type != Lua::Type::Function)
            {
                // type error
                //return Lua::ErrorCode::ErrErr;
            }

            // check stack size
            Push(std::forward<_Args>(args)...);

            constexpr auto argCount = sizeof...(_Args);
            constexpr auto retCount = sizeof...(_ReturnArgs);
            auto error = PCall(argCount, retCount);
            LUACPP_UNUSED(error);

            return PopReturn<_ReturnArgs...>();

        }

        void ForEach()
        {
            // push nil     [..., {a, b, c, ...}, nil]
            // next -2      [..., {a, b, c, ...}, 1, a]
            // call key, val
            // pop 1        [..., {a, b, c, ...}, 1]
            // next -2      [..., {a, b, c, ...}, 2, b]
            // ...

            // ORDER NOT GUARATEED!
        }

        template <class _T>
        static void PushLightUserDataChecked(_T* pointer)
        {
            auto typeId = getUniqueTypeId<_T>();
            // lock
            s_pointerTypeRegister[pointer] = typeId;
            //unlock

            PushLightUserData(pointer);
        }

        template <class _T>
        static _T* ToUserDataChecked(CLint index)
        {
            const auto castTypeId = getUniqueTypeId<_T>();

            auto&& pointer = ToUserData<_T>(index);

            // lock
            const auto registeredTypeId = s_pointerTypeRegister[pointer];
            // unlock

            if (registeredTypeId == castTypeId)
            {
                return pointer;
            }
            else
            {
                // lock
#ifdef LUACPP_DEBUG
                const char* registeredTypeName = s_typenames[registeredTypeId].c_str();
#else
                const char* registeredTypeName = "<type not available>";
#endif
                LUACPP_TRACE("Lua ToUserData cast error:\n"
                             "Trying to cast stack index %d, ptr:%p '%s (typeid:%zu)' to '%s (typeid:%zu)'\n"
                             "ToUserData will return nullptr",
                             index, pointer, registeredTypeName, s_pointerTypeRegister[pointer], typeid(_T).name(), castTypeId);
                // unlock
                return nullptr;
            }
        }

    private:
        static size_t NextTypeId(const char* name)
        {
            static size_t id = 0;
            size_t result = id;
            // lock
            ++id;
            LUACPP_UNUSED(name);
#ifdef LUACPP_DEBUG

            s_typenames[result] = name;

#endif
            // unlock
            return result;
        }

        template <typename _T>
        static size_t getUniqueTypeId()
        {
            static size_t id = NextTypeId(typeid(_T).name());
            return id;
        }

        static Lua::UnorderedMap<void*, size_t> s_pointerTypeRegister;
#ifdef LUACPP_DEBUG
        static Lua::UnorderedMap<size_t, Lua::String> s_typenames;
#endif

        Lua::UnorderedMap<const char*, Lua::UniquePtr<LuaFunctionBase>> m_functions;
    };
}

#endif // CLOUD_LUA_CPP_STATE_EX_HEADER
