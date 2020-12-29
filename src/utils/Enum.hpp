#ifndef TULA_ENUM_HPP
#define TULA_ENUM_HPP

#include <type_traits>

#define ENABLE_BITMASK_OPERATORS(x)  \
template<>                           \
struct EnableBitMaskOperators<x>     \
{                                    \
    static const bool enable = true; \
}

template<typename Enum>
struct EnableBitMaskOperators {
    static const bool enable = false;
};

template<typename Enum>
typename std::enable_if<EnableBitMaskOperators<Enum>::enable, Enum>::type
operator|(Enum lhs, Enum rhs) {
    using underlying = typename std::underlying_type<Enum>::type;
    return static_cast<Enum> (
            static_cast<underlying>(lhs) | static_cast<underlying>(rhs)
    );
}

template<typename Enum>
bool operator&(Enum lhs, Enum rhs) {
    using underlying = typename std::underlying_type<Enum>::type;
    return static_cast<underlying>(lhs) & static_cast<underlying>(rhs);
}

#endif //TULA_ENUM_HPP
