#pragma once
namespace e
{
	namespace system
	{
		class basic_object
		{
		public:
			virtual ~basic_object()
			{
				//nothing need doing
			}
			virtual basic_object* __stdcall clone() = 0;
		};

		//类似std::unique_ptr但可以支持复制（对应复制内部对象，非ARC/GC）
		template <typename TElem> struct object_ptr
		{
			basic_object* data;
			object_ptr() : data(new TElem())
			{
			}
			object_ptr(std::nullptr_t) noexcept : data(nullptr)
			{
			}
			object_ptr(const object_ptr& obj)
			{
				if (obj.data == nullptr)
					this->data = nullptr;
				else
					this->data = obj->clone();
			}
			object_ptr(object_ptr&& obj) noexcept
			{
				this->data = obj.data;
				obj.data = nullptr;
			}
			~object_ptr()
			{
				delete data;
			}
			object_ptr& operator=(const object_ptr& obj)
			{
				if (this != &obj)
				{
					delete this->data;

					if (obj.data == nullptr)
						this->data = nullptr;
					else
						this->data = obj->clone();
				}
				return *this;
			}
			object_ptr& operator=(object_ptr&& obj)
			{
				this->~object_ptr();
				this->data = obj.data;
				obj.data = nullptr;
				return *this;
			}
			object_ptr& operator=(std::nullptr_t)
			{
				this->~object_ptr();
				this->data = nullptr;
				return *this;
			}
			TElem& operator*() const
			{
				TElem* a;
				if (this->data == nullptr)
				{
					a = nullptr;
				}
				else
				{
					a = static_cast<TElem*>(this->data);
				}
				return *a;
			}
			TElem* operator->() const
			{
				return &(**this);
			}

			template <typename TNew, std::enable_if_t<std::is_base_of_v<TNew, TElem>, int> = 0> operator object_ptr<TNew>&()
			{
				return *(reinterpret_cast<object_ptr<TNew>*>(this));
			}

			template <typename TNew, std::enable_if_t<std::negation_v<std::is_same<TNew, TElem>> && std::is_base_of_v<TElem, TNew>, int> = 0> explicit operator object_ptr<TNew>&()
			{
				dynamic_cast<TNew *>(this->data);
				return *(reinterpret_cast<object_ptr<TNew>*>(this));
			}
		};
		template <typename TElem> bool operator==(const object_ptr<TElem>& _Left, const object_ptr<TElem>& _Right)
		{
			if (_Left.data == nullptr)
			{
				return _Right.data == nullptr;
			}
			return *_Left.data == *_Right.data;
		}
		template <typename TElem> bool operator!=(const object_ptr<TElem>& _Left, const object_ptr<TElem>& _Right)
		{
			if (_Left.data == nullptr)
			{
				return _Right.data != nullptr;
			}
			return *_Left.data != *_Right.data;
		}
	}
}