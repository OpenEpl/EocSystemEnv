#pragma once
#include <cstdint>
namespace e
{
	namespace system
	{
		//From http://loungecpp.net/cpp/indices-trick/
		//Thanks very much.

		template <size_t... Is>
		struct indices {};

		template <size_t N, size_t... Is>
		struct build_indices : build_indices<N - 1, N - 1, Is...> {};

		template <size_t... Is>
		struct build_indices<0, Is...> : indices<Is...> {};
	}
}