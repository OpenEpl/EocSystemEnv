#include "constblock.h"
#include "windows.h"

namespace e
{
	namespace system
	{
		struct _const_block_range_t
		{
			intptr_t start = 0;
			intptr_t end = 0;
			_const_block_range_t()
			{
				MEMORY_BASIC_INFORMATION info;
				VirtualQuery("EOC", &info, sizeof(info));
				start = (intptr_t)info.AllocationBase;
				end = (intptr_t)info.BaseAddress + info.RegionSize;
			}
		};
		static _const_block_range_t _const_block_range;

		bool isConstBlock(const void * x) noexcept
		{
			if ((intptr_t)x >= _const_block_range.start && (intptr_t)x < _const_block_range.end)
			{
				return true;
			}
			return false;
		}
	}
}


