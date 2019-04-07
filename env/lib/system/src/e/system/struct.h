#pragma once
#pragma once
namespace e
{
	namespace system
	{
		//类似std::unique_ptr但可以支持复制（对应复制内部对象，非ARC/GC）
		template <typename TElem> struct struct_ptr
		{
			TElem* data;
			struct_ptr() : data(new TElem())
			{
			}
			struct_ptr(std::nullptr_t) noexcept : data(nullptr)
			{
			}
			struct_ptr(const struct_ptr& obj)
			{
				if (obj.data == nullptr)
					this->data = nullptr;
				else
					this->data = new TElem(*obj.data);
			}
			struct_ptr(struct_ptr&& obj) noexcept
			{
				this->data = obj.data;
				obj.data = nullptr;
			}
			~struct_ptr()
			{
				delete data;
			}
			struct_ptr& operator=(const struct_ptr& obj)
			{
				if (this != &obj)
				{
					delete this->data;

					if (obj.data == nullptr)
						this->data = nullptr;
					else
						this->data = new TElem(*obj.data);
				}
				return *this;
			}
			struct_ptr& operator=(struct_ptr&& obj)
			{
				this->~struct_ptr();
				this->data = obj.data;
				obj.data = nullptr;
				return *this;
			}
			struct_ptr& operator=(std::nullptr_t)
			{
				this->~struct_ptr();
				this->data = nullptr;
				return *this;
			}
			TElem& operator*() const
			{
				return *this->data;
			}
			TElem* operator->() const
			{
				return this->data;
			}
		};
		template <typename TElem> bool operator==(const struct_ptr<TElem>& _Left, const struct_ptr<TElem>& _Right)
		{
			if (_Left.data == nullptr)
			{
				return _Right.data == nullptr;
			}
			return *_Left.data == *_Right.data;
		}
		template <typename TElem> bool operator!=(const struct_ptr<TElem>& _Left, const struct_ptr<TElem>& _Right)
		{
			if (_Left.data == nullptr)
			{
				return _Right.data != nullptr;
			}
			return *_Left.data != *_Right.data;
		}
	}
}