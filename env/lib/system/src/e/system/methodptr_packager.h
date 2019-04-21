#pragma once
#include "basic_type.h"
namespace e
{
	namespace system
	{
		template<typename ManagedType> struct MethodPtrPackager_Arg
		{
			using NativeType = ManagedType;
			static ManagedType receive(NativeType v)
			{
				return v;
			}
		};
		template<> struct MethodPtrPackager_Arg<e::system::string&>
		{
			using NativeType = char*;
			static e::system::string& receive(NativeType& v)
			{
				return reinterpret_cast<e::system::string&>(v);
			}
		};
		template<typename ManagedType> struct MethodPtrPackager_Result
		{
			using NativeType = ManagedType;
			NativeType of(ManagedType&& v)
			{
				return v;
			}
		};
		template<> struct MethodPtrPackager_Result<void>
		{
			using NativeType = void;
		};
		template<> struct MethodPtrPackager_Result<e::system::string>
		{
			e::system::string data;
			using NativeType = char*;
			NativeType of(e::system::string&& v)
			{
				data = std::move(v);
				return data.c_str();
			}
		};
		template<> struct MethodPtrPackager_Result<e::system::bin>
		{
			e::system::bin data;
			using NativeType = void*;
			NativeType of(e::system::bin&& v)
			{
				data = std::move(v);
				return data.GetElemPtr();
			}
		};
		template<typename TResult, typename... TArgs>
		struct MethodPtrPackager
		{
		};
		template<typename TResult, typename... TArgs>
		struct MethodPtrPackager<TResult(TArgs...)>
		{
			template<TResult(__stdcall *f)(TArgs... v)> 
			static typename MethodPtrPackager_Result<TResult>::NativeType __stdcall func(typename MethodPtrPackager_Arg<TArgs>::NativeType... args)
			{
				if constexpr (std::is_void_v<TResult>) 
				{
					f(MethodPtrPackager_Arg<TArgs>::receive(args)...);
				}
				else
				{
					static thread_local MethodPtrPackager_Result<TResult> result;
					return result.of(f(MethodPtrPackager_Arg<TArgs>::receive(args)...));
				}
			}
			template<TResult(__stdcall *f)(TArgs... v)> 
			constexpr const static e::system::methodptr ptr = static_cast<e::system::methodptr>(&func<f>);
		};
	}
}