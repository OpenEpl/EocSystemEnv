#pragma once
#include <cstring>
#include <utility>
#include "constblock.h"
namespace e
{
	namespace system
	{
		struct string
		{
		public:
			char* data;
			bool isConst() const noexcept
			{
				return isConstBlock(data);
			}

			string() noexcept : data(nullptr)
			{
			}
			string(const string& str):string(str.c_str())
			{
			}
			string(string&& str) noexcept
			{
				this->data = str.data;
				str.data = nullptr;
			}
			string(size_t maxLength)
			{
				if (maxLength == 0)
				{
					this->data = nullptr;
					return;
				}
				this->data = new char[maxLength + 1];
				this->data[0] = '\0';
			}
			string(std::nullptr_t) noexcept : data(nullptr)
			{

			}
			string(std::in_place_t, const char* e) noexcept : data((char*)e)
			{

			}
			string(const char* rdata)
			{
				if (rdata == nullptr)
				{
					this->data = nullptr;
					return;
				}
				auto len = std::strlen(rdata) + 1;
				auto newData = new char[len];
				std::memcpy(newData, rdata, len);
				this->data = newData;
			}
			char* c_str() const noexcept
			{
				return this->data;
			}
			string& operator=(const string& str)
			{
				if (this != &str)
				{
					this->~string();

					if (str.data == nullptr)
					{
						this->data = nullptr;
					}
					else
					{
						auto len = std::strlen(str.data) + 1;
						auto newData = new char[len];
						std::memcpy(newData, str.data, len);
						this->data = newData;
					}
				}
				return *this;
			}
			string& operator=(string&& str) noexcept
			{
				this->~string();
				this->data = str.data;
				str.data = nullptr;
				return *this;
			}
			string& operator=(std::nullptr_t) noexcept
			{
				this->~string();
				this->data = nullptr;
				return *this;
			}
			size_t len() const noexcept
			{
				if (data == nullptr)
					return 0;
				return std::strlen(data);
			}
			string& operator+=(const string& _Right)
			{
				auto len2 = _Right.len();
				if (len2 == 0)
				{
					return *this;
				}
				auto len1 = this->len();
				if (len1 == 0)
				{
					return this->operator=(_Right);
				}
				auto newData = new char[len1 + len2 + 1];
				std::memcpy(newData, this->data, len1);
				std::memcpy(newData + len1, _Right.data, len2 + 1);
				this->~string();
				this->data = newData;
				return *this;
			}
			~string() noexcept
			{
				if (!isConst())
				{
					delete data;
				}
			}
		};
		inline string operator+(const string& _Left, const string& _Right)
		{
			string r = _Left;
			r += _Right;
			return r;
		}
		bool operator==(const string& _Left, const string& _Right);
		bool operator!=(const string& _Left, const string& _Right);
		bool operator<(const string & _Left, const string & _Right);
		bool operator<=(const string & _Left, const string & _Right);
		bool operator>=(const string & _Left, const string & _Right);
		bool operator>(const string & _Left, const string & _Right);
	}
}

#define EOC_STR_CONST(x) e::system::string(std::in_place, x)