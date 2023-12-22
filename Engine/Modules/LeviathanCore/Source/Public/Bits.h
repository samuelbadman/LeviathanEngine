#pragma once
// A collection of macros to assist working with bits.

// Clears bit bit of value.
#define CLEAR_BIT(value, bit) (value &= static_cast<decltype(value)>(~(static_cast<decltype(value)>(1) << bit)))

// Sets bit bit of value.
#define SET_BIT(value, bit) (value |= static_cast<decltype(value)>(static_cast<decltype(value)>(1) << bit))

// Toggles bit bit of value.
#define TOGGLE_BIT(value, bit) (value ^= static_cast<decltype(value)>(static_cast<decltype(value)>(1) << bit))

// Checks if bit bit of value is set. Macro returns the value of bit bit (0 if cleared and 1 if set).
#define CHECK_BIT(value, bit) ((value >> bit) & 1)

// Declares bitwise operators for the enum type.
#define DECLARE_ENUM_BIT_OPERATORS(EnumType)																														\
constexpr EnumType operator |(const EnumType currentValue, const EnumType inValue)																					\
{																																									\
	return static_cast<EnumType>(static_cast<std::underlying_type<EnumType>::type>(currentValue) | static_cast<std::underlying_type<EnumType>::type>(inValue));		\
}																																									\
																																									\
constexpr EnumType operator &(const EnumType currentValue, const EnumType inValue)																					\
{																																									\
	return static_cast<EnumType>(static_cast<std::underlying_type<EnumType>::type>(currentValue) & static_cast<std::underlying_type<EnumType>::type>(inValue));		\
}																																									\
																																									\
constexpr EnumType operator ^(const EnumType currentValue, const EnumType inValue)																					\
{																																									\
	return static_cast<EnumType>(static_cast<std::underlying_type<EnumType>::type>(currentValue) ^ static_cast<std::underlying_type<EnumType>::type>(inValue));		\
}																																									\
																																									\
constexpr EnumType operator ~(const EnumType inValue)																												\
{																																									\
	return static_cast<EnumType>(static_cast<std::underlying_type<EnumType>::type>(~static_cast<std::underlying_type<EnumType>::type>(inValue)));					\
}																																									\
																																									\
constexpr void operator |=(EnumType& currentValue, const EnumType inValue)																							\
{																																									\
	currentValue = currentValue | inValue;																															\
}																																									\
																																									\
constexpr void operator &=(EnumType& currentValue, const EnumType inValue)																							\
{																																									\
	currentValue = currentValue & inValue;																															\
}																																									\
																																									\
constexpr void operator ^=(EnumType& currentValue, const EnumType inValue)																							\
{																																									\
	currentValue = currentValue ^ inValue;																									\
}																																									