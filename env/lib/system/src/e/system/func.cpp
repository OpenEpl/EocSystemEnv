#include "func.h"
e::system::bin e::system::make_bin(void * data, size_t size)
{
	e::system::bin r(size);
	if (size)
	{
		std::memcpy(&r.At_CStyle(0), data, size);
	}
	return r;
}
