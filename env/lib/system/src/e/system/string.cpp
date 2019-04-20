#include <cstring>
#include "string.h"

static int cmp_str(const e::system::string & _Left, const e::system::string & _Right)
{
	auto len = _Left.len();
	auto r_len = _Right.len();
	if (len == 0)
		return r_len == 0 ? 0 : -1;
	if (r_len == 0)
		return len == 0 ? 0 : 1;
	auto r = std::memcmp(_Left.data, _Right.data, len < r_len ? len : r_len);
	if (r != 0)
		return r;
	if (len == r_len)
		return 0;
	else if (len > r_len)
		return 1;
	else
		return -1;
}


bool e::system::operator==(const string & _Left, const string & _Right)
{
	return cmp_str(_Left, _Right) == 0;
}
bool e::system::operator!=(const string & _Left, const string & _Right)
{
	return cmp_str(_Left, _Right) != 0;
}
bool e::system::operator<(const string & _Left, const string & _Right)
{
	return cmp_str(_Left, _Right) < 0;
}
bool e::system::operator<=(const string & _Left, const string & _Right)
{
	return cmp_str(_Left, _Right) <= 0;
}
bool e::system::operator>=(const string & _Left, const string & _Right)
{
	return cmp_str(_Left, _Right) >= 0;
}
bool e::system::operator>(const string & _Left, const string & _Right)
{
	return cmp_str(_Left, _Right) > 0;
}