#pragma once
#include <type_traits>
#include <cmath>

namespace ssge
{
    template <class T,
        class Decayed = std::decay_t<T>,
        class = std::enable_if_t<!std::is_pointer<Decayed>::value>,
        class = std::enable_if_t<
        std::is_arithmetic<Decayed>::value ||
        std::is_enum<Decayed>::value ||
        std::is_convertible<T, long double>::value>>
        constexpr int sign(T x) noexcept {
        const long double v = static_cast<long double>(x);
        return (0.0L < v) - (v < 0.0L);   // NaN -> 0; unsigned -> 0/1; signed/float -> -1/0/1
    }
}