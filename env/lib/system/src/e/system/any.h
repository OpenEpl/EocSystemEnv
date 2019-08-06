#pragma once
#include <new>
#include <typeinfo>
#include <vector>
#include "marshaler.h"
namespace e
{
	namespace system
	{
		class bad_any_cast : public std::bad_cast {
		public:
			virtual const char* what() const noexcept override {
				return "Bad e::system::any::cast<T>";
			}
		};

		class any;

		template<typename T, std::enable_if_t<!std::is_same_v<T, e::system::any>, int> = 0> void marshalToVector(std::vector<intptr_t> &vector, T& that)
		{
			using TMarshal = valMarshaler<T>;
			constexpr size_t elemCount = (sizeof(typename TMarshal::NativeType) + sizeof(intptr_t) - 1) / sizeof(intptr_t);
			size_t p = vector.size();
			for (size_t i = 0; i < elemCount; i++)
			{
				vector.push_back(0);
			}
			typename TMarshal::NativeType& native = *reinterpret_cast<typename TMarshal::NativeType*>(&vector.at(p));
			TMarshal::marshal(native, that);
		}

		template<typename T, std::enable_if_t<!std::is_same_v<T, e::system::any>, int> = 0> void cleanupFromVector(std::vector<intptr_t> &vector, T& that)
		{
			using TMarshal = valMarshaler<T>;
			constexpr size_t elemCount = (sizeof(typename TMarshal::NativeType) + sizeof(intptr_t) - 1) / sizeof(intptr_t);
			size_t p = vector.size() - elemCount;
			typename TMarshal::NativeType& native = *reinterpret_cast<typename TMarshal::NativeType*>(&vector.at(p));
			TMarshal::cleanup(native, that);
			for (size_t i = 0; i < elemCount; i++)
			{
				vector.pop_back();
			}
		}

		union _Any_DataStorage
		{
			void* ptr;
			uint8_t data[sizeof(double)];
			template<class T> constexpr static bool IsInlineType = std::is_nothrow_move_constructible_v<T> && sizeof(T) <= sizeof(_Any_DataStorage::data);
			template<class T> T* get()
			{
				if constexpr (_Any_DataStorage::IsInlineType<T>)
				{
					return static_cast<T*>((void*)(&this->data));
				}
				else
				{
					return static_cast<T*>(this->ptr);
				}
			}
			template<class T> const T* get() const
			{
				if constexpr (_Any_DataStorage::IsInlineType<T>)
				{
					return static_cast<const T*>((void*)(&this->data));
				}
				else
				{
					return static_cast<const T*>(this->ptr);
				}
			}
		};

		class _Any_RuntimeInfo
		{
		public:
			virtual void clone(const _Any_DataStorage& that, _Any_DataStorage& target) const = 0;
			virtual void move(_Any_DataStorage& that, _Any_DataStorage& target) const noexcept = 0;
			virtual void destroy(_Any_DataStorage& that) const noexcept = 0;
			virtual void byRef_Marshal(void* &v, _Any_DataStorage& that) const = 0;
			virtual void byRef_Cleanup(void* &v, _Any_DataStorage& that) const = 0;
			virtual void byVal_MarshalToVector(std::vector<intptr_t> &stack, _Any_DataStorage& that) const = 0;
			virtual void byVal_CleanupFromVector(std::vector<intptr_t> &stack, _Any_DataStorage& that) const = 0;
#ifdef _M_X64
			virtual void byVal_Marshal(__int64 &v, _Any_DataStorage& that) const = 0;
			virtual void byVal_Cleanup(__int64 &v, _Any_DataStorage& that) const = 0;
#endif
		};

		template<class T> class _Any_RuntimeInfoImpl : public _Any_RuntimeInfo
		{
		public:
			virtual void clone(const _Any_DataStorage& that, _Any_DataStorage& target) const
			{
				if constexpr (_Any_DataStorage::IsInlineType<T>)
				{
					new (target.get<T>()) T(*that.get<T>());
				}
				else
				{
					target.ptr = new T(*that.get<T>());
				}
			}
			virtual void move(_Any_DataStorage& that, _Any_DataStorage& target) const noexcept
			{
				if constexpr (_Any_DataStorage::IsInlineType<T>)
				{
					new (target.get<T>()) T(std::move(*that.get<T>()));
				}
				else
				{
					target.ptr = that.ptr;
					that.ptr = nullptr;
				}
			}
			virtual void destroy(_Any_DataStorage& that) const noexcept
			{
				if constexpr (_Any_DataStorage::IsInlineType<T>)
				{
					that.get<T>()->~T();
				}
				else
				{
					delete that.get<T>();
				}
			}
			virtual void byRef_Marshal(void* &v, _Any_DataStorage& that) const
			{
				using TMarshal = refMarshaler<T>;
				typename TMarshal::NativeType& native = reinterpret_cast<typename TMarshal::NativeType&>(v);
				TMarshal::marshal(native, *that.get<T>());
			}
			virtual void byRef_Cleanup(void* &v, _Any_DataStorage& that) const
			{
				using TMarshal = refMarshaler<T>;
				typename TMarshal::NativeType& native = reinterpret_cast<typename TMarshal::NativeType&>(v);
				TMarshal::cleanup(native, *that.get<T>());
			}
			virtual void byVal_MarshalToVector(std::vector<intptr_t> &vector, _Any_DataStorage& that) const
			{
				marshalToVector<T>(vector, *that.get<T>());

			}
			virtual void byVal_CleanupFromVector(std::vector<intptr_t> &vector, _Any_DataStorage& that) const
			{
				cleanupFromVector<T>(vector, *that.get<T>());
			}
#ifdef _M_X64
			virtual void byVal_Marshal(__int64 &v, _Any_DataStorage& that) const
			{
				using TMarshal = valMarshaler<T>;
				typename TMarshal::NativeType& native = reinterpret_cast<typename TMarshal::NativeType&>(v);
				TMarshal::marshal(native, *that.get<T>());
			}
			virtual void byVal_Cleanup(__int64 &v, _Any_DataStorage& that) const
			{
				using TMarshal = valMarshaler<T>;
				typename TMarshal::NativeType& native = reinterpret_cast<typename TMarshal::NativeType&>(v);
				TMarshal::cleanup(native, *that.get<T>());
			}
#endif 
		};
		template<class T> constexpr static _Any_RuntimeInfoImpl<T> _Any_RuntimeInfoInstance{};

		class any
		{
		private:
			const type_info* typeInfo;
			const _Any_RuntimeInfo* runtimeInfo;
			_Any_DataStorage storage;
		public:
			any() noexcept :typeInfo(nullptr), runtimeInfo(nullptr), storage()
			{

			}
			~any() noexcept
			{
				if (runtimeInfo != nullptr) runtimeInfo->destroy(storage);
			}
			template<class T> any(const T& value) : typeInfo(&typeid(T)), runtimeInfo(&_Any_RuntimeInfoInstance<T>), storage()
			{
				if constexpr (_Any_DataStorage::IsInlineType<T>)
				{
					new (reinterpret_cast<T*>(&storage.data)) T(value);
				}
				else
				{
					storage.ptr = new T(value);
				}
			}
			void reset() noexcept
			{
				if (runtimeInfo != nullptr) runtimeInfo->destroy(storage);
				typeInfo = nullptr;
				runtimeInfo = nullptr;
			}
			e::system::any& operator=(const e::system::any& x)
			{
				if (&x == this)
					return *this;
				this->~any();
				typeInfo = x.typeInfo;
				runtimeInfo = x.runtimeInfo;
				if (runtimeInfo != nullptr) runtimeInfo->clone(x.storage, storage);
				return *this;
			}
			e::system::any& operator=(e::system::any&& x) noexcept
			{
				this->~any();
				typeInfo = x.typeInfo;
				runtimeInfo = x.runtimeInfo;
				if (runtimeInfo != nullptr)
				{
					runtimeInfo->move(x.storage, storage);
					x.reset();
				}
				return *this;
			}
			any(const e::system::any& x) :typeInfo(x.typeInfo), runtimeInfo(x.runtimeInfo)
			{
				if (runtimeInfo != nullptr) runtimeInfo->clone(x.storage, storage);
			}
			any(e::system::any&& x)  noexcept : typeInfo(x.typeInfo), runtimeInfo(x.runtimeInfo)
			{
				if (runtimeInfo != nullptr)
				{
					runtimeInfo->move(x.storage, storage);
					x.reset();
				}
			}
			const type_info& type() const noexcept
			{
				return has_value() ? *typeInfo : typeid(void);
			}
			bool has_value() const noexcept
			{
				return typeInfo != nullptr;
			}
			template<class T> T& cast() 
			{
				if (typeid(T) != this->type())
					throw bad_any_cast{};
				return *storage.get<T>();
			}
			template<class T> const T& cast() const
			{
				if (typeid(T) != this->type())
					throw bad_any_cast{};
				return *storage.get<T>();
			}
			void byRef_Marshal(void* &v)
			{
				if (runtimeInfo != nullptr)
				{
					runtimeInfo->byRef_Marshal(v, storage);
				}
				else
				{
					v = nullptr;
				}
			}
			void byRef_Cleanup(void* &v) 
			{
				if (runtimeInfo != nullptr)
					runtimeInfo->byRef_Cleanup(v, storage);
			}

#ifdef _M_X64
			void byVal_Marshal(__int64 &v)
			{
				v = 0;
				if (runtimeInfo != nullptr)
				{
					runtimeInfo->byVal_Marshal(v, storage);
				}
			}
			void byVal_Cleanup(__int64 &v)
			{
				if (runtimeInfo != nullptr)
					runtimeInfo->byVal_Cleanup(v, storage);
			}
#endif

			void byVal_MarshalToVector(std::vector<intptr_t> &vector)
			{
				if (runtimeInfo != nullptr)
				{
					runtimeInfo->byVal_MarshalToVector(vector, storage);
				}
				else
				{
					vector.push_back(0);
				}
			}
			void byVal_CleanupFromVector(std::vector<intptr_t> &vector)
			{
				if (runtimeInfo != nullptr)
				{
					runtimeInfo->byVal_CleanupFromVector(vector, storage);
				}
				else
				{
					vector.pop_back();
				}
			}
		};

		inline void marshalToVector(std::vector<intptr_t> &vector, e::system::any& that)
		{
			that.byVal_MarshalToVector(vector);
		}
		inline void cleanupFromVector(std::vector<intptr_t> &vector, e::system::any& that)
		{
			that.byVal_CleanupFromVector(vector);
		}

		template<typename TFirst, typename... TOthers> void marshalToVector(std::vector<intptr_t> &vector, TFirst& that, TOthers&... those)
		{
			marshalToVector(vector, that);
			if constexpr (sizeof...(those) != 0)
			{
				cleanupFromVector(vector, those...);
			}
		}

		template<typename TFirst, typename... TOthers> void cleanupFromVector(std::vector<intptr_t> &vector, TFirst& that, TOthers&... those)
		{
			if constexpr (sizeof...(those) != 0)
			{
				cleanupFromVector(vector, those...);
			}
			cleanupFromVector(vector, that);
		}

		//对结构体内通用型只支持传址封送
		template<> struct marshaler<e::system::any>
		{
			using NativeType = void*;
			static constexpr bool SameMemoryStruct = false;
			static void marshal(NativeType &v, e::system::any &r)
			{
				r.byRef_Marshal(v);
			}
			static void cleanup(NativeType &v, e::system::any &r)
			{
				r.byRef_Cleanup(v);
			}
		};
		template<> struct refMarshaler<e::system::any> : marshaler<e::system::any>
		{

		};
		//对通用型参数的传值封送由Caller实现
		template<> struct valMarshaler<e::system::any> : refMarshaler<e::system::any>
		{

		};
	}
}