#pragma once
#include "marshaler.h"
#include "any.h"
namespace e
{
	namespace system
	{

#ifdef _M_IX86
		static void __declspec(naked) __fastcall _MethodPtrCaller_RawCallWithDynamicParam(void* pFunc/*ecx*/, void* pParam/*edx*/, size_t pParamSize)
		{
			__asm {
				mov eax, [esp+4]

				test eax,eax
				jz endPush
				continuToPush:
				add eax, -4
				push dword ptr[edx + eax]
					jz endPush
					jmp continuToPush
					endPush :

				call ecx
				ret 4
			}
		}
		static void __declspec(naked) __fastcall _MethodPtrCaller_RawCdcelCallWithDynamicParam(void* pFunc/*ecx*/, void* pParam/*edx*/, size_t pParamSize)
		{
			__asm {
				push esi
				mov esi, [esp + 4]
				test esi, esi
				jz endPush
				continuToPush :
				add esi, -4
					push dword ptr[edx + esi]
					jz endPush
					jmp continuToPush
					endPush :

				call ecx
					add esp, esi
					pop esi
					ret 4
			}
		}
#endif 

		enum class CallingConventions
		{
			StdCall,
			Cdcel
		};

		template<typename TResult, typename... TArgs>
		struct MethodPtrCaller
		{
		};
		template<typename TResult, typename... TArgs>
		struct MethodPtrCaller<TResult(TArgs...)>
		{
		private:

			template<typename T> struct ParamMarshalHelper
			{
			private:
				using TMarshal = valMarshaler<T>;
			public:
				using NativeType = typename TMarshal::NativeType;
			private:
				T& value;
				NativeType x;
			public:
				ParamMarshalHelper(T& _value) :value(_value)
				{
					TMarshal::marshal(x, value);
				}
				~ParamMarshalHelper()
				{
					TMarshal::cleanup(x, value);
				}
				NativeType& get()
				{
					return x;
				}
			};

#ifdef _M_X64
			template<> struct ParamMarshalHelper<e::system::any>
			{
			public:
				using NativeType = double;
			private:
				typename e::system::any& value;
				double x;
			public:
				ParamMarshalHelper(e::system::any& _value) :value(_value)
				{
					value.byVal_Marshal(reinterpret_cast<__int64 &>(x));
				}
				~ParamMarshalHelper()
				{
					value.byVal_Cleanup(reinterpret_cast<__int64 &>(x));
				}
				double& get()
				{
					return x;
				}
			};
#endif

			template<typename T> constexpr static auto isByValAnyArg_v = std::is_same_v<T, e::system::any>;
			constexpr static int getIndexOfFirstByValAnyParam()
			{
				constexpr bool x[] = { isByValAnyArg_v<TArgs>...};
				for (int i = 0; i < sizeof(x); i++)        
				{
					if (x[i]) return i;
				}
				return -1;
			};

		public:
			template <CallingConventions callingConvention = CallingConventions::StdCall>
			static TResult call(void* func, TArgs... args)
			{
				using FuncPtrType = std::conditional_t<callingConvention == CallingConventions::Cdcel, 
					typename resultReceiver<TResult>::NativeType (__cdecl *)(typename ParamMarshalHelper<TArgs>::NativeType...),
					typename resultReceiver<TResult>::NativeType (__stdcall *)(typename ParamMarshalHelper<TArgs>::NativeType...)> ;
				constexpr auto indexOfFirstAnyParam = getIndexOfFirstByValAnyParam();
				if constexpr (indexOfFirstAnyParam != -1)
				{
#ifdef _M_X64
					//参考 https://docs.microsoft.com/zh-cn/cpp/build/x64-calling-convention#varargs
					//使用Varargs并实现ParamMarshalHelper<e::system::any>产生double，可保证 浮点寄存器 和 整数寄存器 都存在值

					//注2：stdcall与cdcel在x64都会被忽略，并使用标准x64调用约定
					typename resultReceiver<TResult>::NativeType (*pFunc)(...);
					pFunc = static_cast<decltype(pFunc)>(func);
					if constexpr (std::is_void_v<TResult>)
					{
						return pFunc(ParamMarshalHelper<TArgs>(args).get()...);
					}
					else
					{
						return resultReceiver<TResult>::receive(pFunc(ParamMarshalHelper<TArgs>(args).get()...));
					}
#endif
#ifdef _M_IX86
					//参考：
					//https://docs.microsoft.com/en-us/cpp/cpp/stdcall
					//https://docs.microsoft.com/en-us/cpp/cpp/calling-example-function-prototype-and-call
					//https://docs.microsoft.com/en-us/cpp/cpp/results-of-calling-example
					//stdcall对double, int64等QWORD类型需要8bytes栈空间（或更多），而对于其他数据只需要4bytes空间。
					//这意味这无法像x64那样在编译时确定栈空间大小

					std::vector<intptr_t> paramVector; //注：此处为正序，区别于stdcall
					marshalToVector<TArgs...>(paramVector, args...);
					typename resultReceiver<TResult>::NativeType(__fastcall *pCallerFunc)(void*, void*, size_t);
					if constexpr (callingConvention == CallingConventions::Cdcel)
					{
						pCallerFunc = reinterpret_cast<decltype(pCallerFunc)>(_MethodPtrCaller_RawCdcelCallWithDynamicParam);
					}
					else
					{
						pCallerFunc = reinterpret_cast<decltype(pCallerFunc)>(_MethodPtrCaller_RawCallWithDynamicParam);
					}
					if constexpr (std::is_void_v<TResult>)
					{
						pCallerFunc(func, paramVector.data(), paramVector.size() * sizeof(intptr_t));
						cleanupFromVector<TArgs...>(paramVector, args...);
					}
					else
					{
						auto r = resultReceiver<TResult>::receive(pCallerFunc(func, paramVector.data(), paramVector.size() * sizeof(intptr_t)));
						cleanupFromVector<TArgs...>(paramVector, args...);
						return r;
					}
#endif 
				}
				else
				{
					FuncPtrType pFunc;
					pFunc = static_cast<decltype(pFunc)>(func);
					if constexpr (std::is_void_v<TResult>)
					{
						return pFunc(ParamMarshalHelper<TArgs>(args).get()...);
					}
					else
					{
						return resultReceiver<TResult>::receive(pFunc(ParamMarshalHelper<TArgs>(args).get()...));
					}
				}
			}
		};
	}
}