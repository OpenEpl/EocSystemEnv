#pragma once
#include <cstring>
#include <cstdint>
#include <new>
#include <stdexcept>
namespace e
{
	namespace system
	{
		template <typename TElem> struct basic_array
		{
			void* data;
			basic_array(const std::initializer_list<TElem> &v) : basic_array(v.size())
			{
				auto pTarget = GetElemPtr();
				for (auto item : v)
				{
					*(pTarget++) = item;
				}
			}
			basic_array() noexcept : data(nullptr)
			{

			}
			template <typename ... Args> basic_array(size_t dim1, Args... dims) : data(nullptr)
			{
				Redim(false, dim1, dims...);
			}
			basic_array(std::nullptr_t) noexcept : data(nullptr)
			{
			}
			basic_array(const basic_array& arr) : data(nullptr)
			{
				this->operator=(arr);
			}
			basic_array(basic_array&& arr) noexcept
			{
				this->data = arr.data;
				arr.data = nullptr;
			}
			~basic_array()
			{
				if (data == nullptr)
				{
					return;
				}
				TElem *elem = GetElemPtr();
				size_t size = GetSize();
				for (size_t i = 0; i < size; i++)
				{
					elem[i].~TElem();
				}
				delete static_cast<char *>(data);
			}
			bool empty() const
			{
				return GetSize() == 0;
			}
			size_t* GetUBoundPtr() const
			{
				if (data == nullptr)
				{
					return nullptr;
				}
				return (size_t*)data + 1;
			}
			TElem* GetElemPtr() const
			{
				if (data == nullptr)
				{
					return nullptr;
				}
				size_t ndim = GetRank();
				TElem *elem = (TElem*)((size_t*)data + 1 + ndim);
				return elem;
			}
			size_t GetSize() const
			{
				size_t ndim = GetRank();
				size_t size = 1;
				for (size_t i = 1; i <= ndim; i++)
				{
					size *= GetUBound(i);
				}
				return size;
			}
			basic_array& operator=(const basic_array& arr)
			{
				this->~basic_array();
				if (arr.data == nullptr)
				{
					data = nullptr;
					return *this;
				}
				size_t size = arr.GetSize();
				size_t ndim = arr.GetRank();
				data = new char[sizeof(size_t) + sizeof(size_t) * ndim + sizeof(TElem) * size];
				*(size_t*)data = ndim;
				std::memcpy((size_t*)data + 1, arr.GetUBoundPtr(), ndim * sizeof(size_t));
				TElem *elem = GetElemPtr();
				TElem *oldElem = arr.GetElemPtr();
				for (size_t i = 0; i < size; i++)
				{
					new(&elem[i]) TElem(oldElem[i]);
				}
				return *this;
			} 
			basic_array& operator=(basic_array&& arr) noexcept
			{
				this->~basic_array();
				this->data = arr.data;
				arr.data = nullptr;
				return *this;
			}
			basic_array& operator=(std::nullptr_t)
			{
				this->~basic_array();
				this->data = nullptr;
				return *this;
			}
			void RedimWithDynamicRank(bool preserve, size_t ndim, size_t* dim)
			{
				size_t size = ndim ? 1 : 0;
				for (size_t i = 0; i < ndim; i++)
				{
					size *= dim[i];
				}
				if (size == 0)
				{
					this->~basic_array();
					data = nullptr;
					return;
				}
				void* newDataPtr = new char[sizeof(size_t) + sizeof(size_t) * ndim + sizeof(TElem) * size];
				*(size_t*)newDataPtr = ndim;
				std::memcpy((size_t*)newDataPtr + 1, &dim[0], sizeof(dim[0]) * ndim);
				TElem *elem = (TElem*)((size_t*)newDataPtr + 1 + ndim);
				auto oldSize = GetSize();
				auto oldElem = GetElemPtr();
				for (size_t i = 0; i < size; i++)
				{
					if (preserve && i < oldSize)
					{
						new(&elem[i]) TElem(std::move(oldElem[i]));
					}
					else
					{
						new(&elem[i]) TElem();
					}
				}

				this->~basic_array();
				data = newDataPtr;
			}
			template <typename ... Args> void Redim(bool preserve, Args... dims)
			{
				size_t dim[]{ size_t(dims)... };
				size_t ndim = sizeof...(dims);
				size_t size = ndim ? 1 : 0;
				for (size_t i = 0; i < ndim; i++)
				{
					size *= dim[i];
				}
				if (size == 0)
				{
					this->~basic_array();
					data = nullptr;
					return;
				}
				void* newDataPtr = new char[sizeof(size_t) + sizeof(size_t) * ndim + sizeof(TElem) * size];
				*(size_t*)newDataPtr = ndim;
				std::memcpy((size_t*)newDataPtr + 1, &dim[0], sizeof(dim[0]) * ndim);
				TElem *elem = (TElem*)((size_t*)newDataPtr + 1 + ndim);
				auto oldSize = GetSize();
				auto oldElem = GetElemPtr();
				for (size_t i = 0; i < size; i++)
				{
					if (preserve && i < oldSize)
					{
						new(&elem[i]) TElem(std::move(oldElem[i]));
					}
					else
					{
						new(&elem[i]) TElem();
					}
				}

				this->~basic_array();
				data = newDataPtr;
			}
			size_t GetRank() const
			{
				return data == nullptr ? 1 : *(size_t*)data;
			}
			//dimension从0开始 
			size_t GetUBound_CStyle(size_t dimension) const
			{
				if (dimension >= GetRank())
				{
					throw std::out_of_range("e::system::basic_array RankCheck");
				}
				if (data == nullptr)
				{
					return 0;
				}
				return *((size_t*)data + 1 + dimension);
			}
			//dimension从1开始
			size_t GetUBound(size_t dimension) const
			{
				if (dimension < 1 || dimension > GetRank())
				{
					throw std::out_of_range("e::system::basic_array RankCheck");
				}
				if (data == nullptr)
				{
					return 0;
				}
				return *((size_t*)data + dimension);
			}
			template <typename ... Args> TElem& At_CStyle(Args... indexes) const
			{
				size_t idx[]{ size_t(indexes)... };
				size_t ndim = sizeof...(indexes);
				size_t offest = 0;
				if (ndim != GetRank())
				{
					throw std::invalid_argument("e::system::basic_array RankCheck");
				}
				for (size_t i = 0; i < ndim; i++)
				{
					size_t t = idx[i];
					if (t >= GetUBound_CStyle(i))
					{
						throw std::out_of_range("e::system::basic_array IndexCheck");
					}
					for (size_t j = i + 2; j <= ndim; j++)
					{
						t *= GetUBound(j);
					}
					offest += t;
				}
				TElem *elem = (TElem*)((size_t*)data + 1 + ndim);
				return elem[offest];
			}
			template <typename ... Args> TElem& At(Args... indexes) const
			{
				return At_CStyle((indexes - 1)...);
			}

			basic_array& operator+=(const basic_array& _Right)
			{
				if (_Right.empty())
				{
					return *this;
				}
				if (empty())
				{
					return this->operator=(_Right);
				}

				auto ndim_r = _Right.GetRank();
				auto ndim = this->GetRank();
				if (ndim != ndim_r)
					throw std::invalid_argument("Only two array with the same rank can be combined");
				if (std::memcmp(this->GetUBoundPtr() + 1, _Right.GetUBoundPtr() + 1, (ndim - 1) * sizeof(size_t)) != 0)
					throw std::invalid_argument("Can't be combined");

				auto lsize = this->GetSize();
				auto rsize = _Right.GetSize();
				auto pElem_l = this->GetElemPtr();
				auto pElem_r = _Right.GetElemPtr();
				auto size = lsize + rsize;
				void* newDataPtr = new char[sizeof(size_t) + sizeof(size_t) * ndim + sizeof(TElem) * size];
				*(size_t*)newDataPtr = ndim;
				std::memcpy((size_t*)newDataPtr + 1, this->GetUBoundPtr(), sizeof(size_t) * ndim);
				*((size_t*)newDataPtr + 1) += _Right.GetUBound_CStyle(0);

				TElem *elem = (TElem*)((size_t*)newDataPtr + 1 + ndim);
				for (size_t i = 0; i < lsize; i++)
				{
					new(&elem[i]) TElem(std::move(pElem_l[i]));
				}
				elem += lsize;
				for (size_t i = 0; i < rsize; i++)
				{
					new(&elem[i]) TElem(pElem_r[i]);
				}

				this->~basic_array();
				data = newDataPtr;

				return *this;
			}
		};
		template <typename TElem> inline basic_array<TElem> operator+(const basic_array<TElem>& _Left, const basic_array<TElem>& _Right)
		{
			basic_array<TElem> r = _Left;
			r += _Right;
			return r;
		}
		template <typename TElem> bool operator==(const basic_array<TElem>& _Left, const basic_array<TElem>& _Right)
		{
			if (_Left.empty())
			{
				return _Right.empty();
			}

			auto ndim = _Left.GetRank();
			if (ndim != _Right.GetRank())
				return false;
			if (std::memcmp(_Left.GetUBoundPtr(), _Right.GetUBoundPtr(), ndim * sizeof(size_t)) != 0)
				return false;
			size_t size = _Left.GetSize();
			TElem *elem = _Left.GetElemPtr();
			TElem *oldElem = _Right.GetElemPtr();
			for (size_t i = 0; i < size; i++)
			{
				if (elem[i] != oldElem[i])
					return false;
			}
			return true;
		}
		template <typename TElem> bool operator!=(const basic_array<TElem>& _Left, const basic_array<TElem>& _Right)
		{
			return !(_Left == _Right);
		}



		template <typename TElem> struct array: basic_array<TElem>
		{
			array(const std::initializer_list<TElem> &v) : basic_array<TElem>(v)
			{
			}
			array() noexcept : basic_array<TElem>()
			{

			}
			template <typename ... Args> array(size_t dim1, Args... dims) : basic_array<TElem>(dim1, dims...)
			{
			}
			array(std::nullptr_t) noexcept : basic_array<TElem>(nullptr)
			{
			}
			array(const array<TElem>& arr) : basic_array<TElem>(std::forward<decltype(arr)>(arr))
			{
			}
			array(array<TElem>&& arr) noexcept : basic_array<TElem>(std::forward<decltype(arr)>(arr))
			{
			}
			array<TElem>& operator=(const array<TElem>& arr)
			{
				basic_array<TElem>::operator=(std::forward<decltype(arr)>(arr));
				return *this;
			}
			array<TElem>& operator=(array<TElem>&& arr) noexcept
			{
				basic_array<TElem>::operator=(std::forward<decltype(arr)>(arr));
				return *this;
			}
			array<TElem>& operator=(std::nullptr_t)
			{
				basic_array<TElem>::operator=(nullptr);
				return *this;
			}
			array<TElem>& operator+=(const array<TElem>& _Right)
			{
				basic_array<TElem>::operator+=(_Right);
				return *this;
			}
		};

		struct bin : basic_array<uint8_t>
		{
			bin(const std::initializer_list<uint8_t> &v) : basic_array<uint8_t>(v)
			{
			}
			bin() noexcept : basic_array<uint8_t>()
			{

			}
			bin(size_t size) : basic_array<uint8_t>(size)
			{
			}
			bin(std::nullptr_t) noexcept : basic_array<uint8_t>(nullptr)
			{
			}
			bin(const bin& arr) : basic_array<uint8_t>(std::forward<decltype(arr)>(arr))
			{
			}
			bin(bin&& arr) noexcept : basic_array<uint8_t>(std::forward<decltype(arr)>(arr))
			{
			}
			bin& operator=(const bin& arr)
			{
				basic_array<uint8_t>::operator=(std::forward<decltype(arr)>(arr));
				return *this;
			}
			bin& operator=(bin&& arr) noexcept
			{
				basic_array<uint8_t>::operator=(std::forward<decltype(arr)>(arr));
				return *this;
			}
			bin& operator=(std::nullptr_t)
			{
				basic_array<uint8_t>::operator=(nullptr);
				return *this;
			}
			bin& operator+=(const bin& _Right)
			{
				basic_array<uint8_t>::operator+=(_Right);
				return *this;
			}
		};

		template <typename TElem> inline array<TElem> operator+(const array<TElem>& _Left, const array<TElem>& _Right)
		{
			array<TElem> r = _Left;
			r += _Right;
			return r;
		}
		
		inline bin operator+(const bin& _Left, const bin& _Right)
		{
			bin r = _Left;
			r += _Right;
			return r;
		}

		template <typename T> struct is_e_array_type : std::bool_constant<false>
		{
		};
		template <typename TElem> struct is_e_array_type<e::system::array<TElem>> : std::bool_constant<true>
		{
		};
		template<class T> _INLINE_VAR constexpr bool is_e_array_type_v = e::system::is_e_array_type<T>::value;

		template <typename T> struct remove_e_array_type
		{
			using type = T;
		};
		template <typename TElem> struct remove_e_array_type<e::system::array<TElem>>
		{
			using type = TElem;
		};
		template <typename T> using remove_e_array_type_t = typename e::system::remove_e_array_type<T>::type;
	}
}