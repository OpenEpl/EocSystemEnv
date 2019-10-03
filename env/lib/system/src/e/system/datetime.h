#pragma once
namespace e
{
	namespace system
	{

		struct datetime
		{
			double value;
			datetime() noexcept : value(0)
			{

			}
			datetime(double _v) noexcept : value(_v)
			{

			}
			datetime& operator+=(const datetime& _Right) noexcept
			{
				this->value += _Right.value;
				return *this;
			}
			datetime& operator-=(const datetime& _Right) noexcept
			{
				this->value -= _Right.value;
				return *this;
			}
		};

		inline datetime operator+(const datetime& _Left, const datetime& _Right) noexcept
		{
			datetime r = _Left;
			r += _Right;
			return r;
		}
		inline datetime operator-(const datetime& _Left, const datetime& _Right) noexcept
		{
			datetime r = _Left;
			r -= _Right;
			return r;
		}
		inline bool operator==(const datetime& _Left, const datetime& _Right) noexcept
		{
			return _Left.value == _Right.value;
		}
		inline bool operator!=(const datetime& _Left, const datetime& _Right) noexcept
		{
			return _Left.value != _Right.value;
		}
		inline bool operator<(const datetime& _Left, const datetime& _Right) noexcept
		{
			return _Left.value < _Right.value;
		}
		inline bool operator<=(const datetime& _Left, const datetime& _Right) noexcept
		{
			return _Left.value <= _Right.value;
		}
		inline bool operator>=(const datetime& _Left, const datetime& _Right) noexcept
		{
			return _Left.value >= _Right.value;
		}
		inline bool operator>(const datetime& _Left, const datetime& _Right) noexcept
		{
			return _Left.value > _Right.value;
		}
	}
}