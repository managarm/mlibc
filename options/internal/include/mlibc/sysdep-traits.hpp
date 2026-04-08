#pragma once

#include <type_traits>

template <typename T>
concept AllSysdepTraits = requires {
	typename std::bool_constant<T::usesRtNetlink>;
};
