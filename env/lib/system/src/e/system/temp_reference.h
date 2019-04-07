#pragma once
#include <functional>
namespace e
{
	namespace system
	{
		template <typename TElem> class temp_reference
		{
		private:
			TElem elem;
		public:
			temp_reference(const temp_reference<TElem>& x)
			{
				this->elem = x.elem;
			}
			temp_reference(temp_reference<TElem>&& x)
			{
				this->elem = std::move(x.elem);
			}
			temp_reference(const TElem& x)
			{
				this->elem = x;
			}
			temp_reference(TElem&& x)
			{
				this->elem = std::move(x);
			}
			~temp_reference()
			{
			}
			temp_reference& operator=(const temp_reference& x)
			{
				if (this != &x)
				{
					this->elem = x.elem;
				}
				return *this;
			}
			temp_reference& operator=(temp_reference&& x) noexcept
			{
				this->elem = std::move(x.elem);
				return *this;
			}
			operator std::reference_wrapper<TElem>() noexcept
			{
				return std::ref(this->elem);
			}
			operator TElem&() noexcept
			{
				return get();
			}
			[[nodiscard]] TElem& get() noexcept
			{
				return this->elem;
			}
			template<class... T> auto operator()(T&&... args) const
			{
				return std::invoke(get(), std::forward<T>(args)...);
			}
		};
	}
}