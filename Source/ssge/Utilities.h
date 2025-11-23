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

	static inline void ltrim(std::string& s)
	{
		size_t i = 0u;
		while (i < s.size() && std::isspace((unsigned char)s[i]))
			i++;

		s.erase(0, i);
	}
	static inline void rtrim(std::string& s)
	{
		if (s.empty()) return;
		size_t i = s.size();
		while (i > 0 && std::isspace((unsigned char)s[i - 1]))
			i--;

		s.erase(i);
	}
	static inline std::string trim(std::string s)
	{
		ltrim(s);
		rtrim(s);
		return s;
	}

	static inline std::string lower(std::string s)
	{
		for (size_t i = 0; i < s.size(); i++)
			s[i] = (char)std::tolower((unsigned char)s[i]);

		return s;
	}
}