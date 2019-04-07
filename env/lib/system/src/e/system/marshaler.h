#pragma once
#include <cstdint>
#include <cstring>
#include <type_traits>
#include "array.h"
#include "struct.h"
#include "object.h"
#include "string.h"
#include <vector>
namespace e
{
	namespace system
	{
		//Raw
		template<typename ManagedType> struct marshaler
		{
			using NativeType = typename ManagedType;
			static constexpr bool SameMemoryStruct = true;
			static void marshal(NativeType &v, ManagedType &r)
			{
				v = r;
			}
			static void cleanup(NativeType &v, ManagedType &r)
			{
				r = v;
			}
		};

		//String
		template<> struct marshaler<e::system::string>
		{
			using NativeType = char*;
			static constexpr bool SameMemoryStruct = true;
			static void marshal(NativeType &v, e::system::string &r)
			{
				v = r.data;
			}
			static void cleanup(NativeType &v, e::system::string &r)
			{
				r.data = v;
			}
		};

		//LPArray
		template<> struct marshaler<e::system::bin>
		{
			using NativeType = typename uint8_t*;
			static constexpr bool SameMemoryStruct = false;
			static void marshal(NativeType &v, e::system::bin &r)
			{
				v = r.GetElemPtr();
			}
			static void cleanup(NativeType &v, e::system::bin &r)
			{
				
			}
		};
		template<typename ElemType> struct marshaler<e::system::array<ElemType>>
		{
		private:
			using ElemMarshalerType = typename marshaler<ElemType>;
			using NativeElemType = typename ElemMarshalerType::NativeType;
			using ManagedType = e::system::array<ElemType>;
		public:
			using NativeType = NativeElemType * ;
			static constexpr bool SameMemoryStruct = false;
			static void marshal(NativeType &v, ManagedType &r)
			{
				if (ElemMarshalerType::SameMemoryStruct)
				{
					v = r.data != 0 ? &r.At_CStyle(0) : nullptr;
				}
				else
				{
					auto size = r.GetSize();
					v = new NativeElemType[size];
					for (size_t i = 0; i < size; i++)
					{
						ElemMarshalerType::marshal(v[i], r.At_CStyle(i));
					}
				}
			}
			static void cleanup(NativeType &v, ManagedType &r)
			{
				if (!ElemMarshalerType::SameMemoryStruct)
				{
					auto size = r.GetSize();
					for (size_t i = 0; i < size; i++)
					{
						ElemMarshalerType::cleanup(v[i], r.At_CStyle(i));
					}
					delete[] v;
				}
			}
		};

		//ByValArray
		template<typename ManagedType, size_t N, std::enable_if_t<e::system::is_e_array_type_v<ManagedType>,int> = 0> struct marshalerForArray
		{
		private:
			using ElemType = typename e::system::remove_e_array_type_t<ManagedType>;
			using ElemMarshalerType = typename marshaler<ElemType>;
			using NativeElemType = typename ElemMarshalerType::NativeType;
		public:
			struct NativeType {
				NativeElemType content[N];
			};
			static constexpr bool SameMemoryStruct = false;
		public:
			static void marshal(NativeType &v, ManagedType &r)
			{
				for (size_t i = 0; i < N; i++)
				{
					ElemMarshalerType::marshal(v.content[i], r.At_CStyle(i));
				}
			}
			static void cleanup(NativeType &v, ManagedType &r)
			{
				for (size_t i = 0; i < N; i++)
				{
					ElemMarshalerType::cleanup(v.content[i], r.At_CStyle(i));
				}
			}
		};

		//LPRaw
		template<typename ManagedType> struct refMarshaler
		{
		private:
			using ValueMarshalerType = typename marshaler<ManagedType>;
			using NativeValueType = typename ValueMarshalerType::NativeType;
		public:
			using NativeType = typename NativeValueType*;
			static constexpr bool SameMemoryStruct = false;
			static void marshal(NativeType &v, ManagedType &r)
			{
				if (ValueMarshalerType::SameMemoryStruct)
				{
					v = reinterpret_cast<NativeType>(&r);
				}
				else
				{
					v = new NativeValueType();
					ValueMarshalerType::marshal(*v, r);
				}
			}
			static void cleanup(NativeType &v, ManagedType &r)
			{
				if (!ValueMarshalerType::SameMemoryStruct)
				{
					ValueMarshalerType::cleanup(*v, r);
					delete v;
				}
			}
		};

		template<> struct refMarshaler<e::system::bin> : marshaler<e::system::bin>
		{
		};
		template<typename ElemType> struct refMarshaler<e::system::array<ElemType>> : marshaler<e::system::array<ElemType>>
		{
		};
		template<> struct refMarshaler<e::system::string> : marshaler<e::system::string>
		{
		};
		//refMarshaler不能接受const类型
		template<typename ManagedType> struct refMarshaler<const ManagedType>
		{

		};
		//引用类型
		template<typename ManagedType> struct refMarshaler<ManagedType&> : refMarshaler<ManagedType>
		{
		};
		template<typename ManagedType> struct refMarshaler<ManagedType&&> : refMarshaler<ManagedType>
		{
		};

		template<typename ManagedType> struct valMarshaler : marshaler<ManagedType>
		{
		};
		template<typename ElemType> struct valMarshaler<e::system::struct_ptr<ElemType>> : refMarshaler<e::system::struct_ptr<ElemType>>
		{
		};
		template<typename ElemType> struct valMarshaler<e::system::object_ptr<ElemType>> : refMarshaler<e::system::object_ptr<ElemType>>
		{
		};
		//引用类型-->refMarshaler
		template<typename ManagedType> struct valMarshaler<ManagedType&> : refMarshaler<ManagedType>
		{
		};
		template<typename ManagedType> struct valMarshaler<ManagedType&&> : refMarshaler<ManagedType>
		{
		};

		//Raw
		template<typename ManagedType> struct resultReceiver
		{
			using NativeType = typename ManagedType;
			static ManagedType receive(NativeType&& v)
			{
				return v;
			}
		};
		template<> struct resultReceiver<void>
		{
			using NativeType = void;
		};
		//String
		template<> struct resultReceiver<e::system::string>
		{
			using NativeType = char*;
			static e::system::string receive(NativeType&& v)
			{
				return e::system::string(v);
			}
		};
	}
}

#define StructMarshaler_DefineMember(type, name) e::system::marshaler<decltype(type::data->name)>::NativeType name
#define StructMarshaler_DefineMember_Ref(type, name) e::system::refMarshaler<decltype(type::data->name)>::NativeType name
#define StructMarshaler_DefineMember_Array(type, name, N) e::system::marshalerForArray<decltype(type::data->name), N>::NativeType name

#define StructMarshaler_MarshalMember(type, name) e::system::marshaler<decltype(type::data->name)>::marshal(v.name, r->name)
#define StructMarshaler_MarshalMember_Ref(type, name) e::system::refMarshaler<decltype(type::data->name)>::marshal(v.name, r->name)
#define StructMarshaler_MarshalMember_Array(type, name, N) e::system::marshalerForArray<decltype(type::data->name), N>::marshal(v.name, r->name)

#define StructMarshaler_CleanupMember(type, name) e::system::marshaler<decltype(type::data->name)>::cleanup(v.name, r->name)
#define StructMarshaler_CleanupMember_Ref(type, name) e::system::refMarshaler<decltype(type::data->name)>::cleanup(v.name, r->name)
#define StructMarshaler_CleanupMember_Array(type, name, N) e::system::marshalerForArray<decltype(type::data->name), N>::cleanup(v.name, r->name)