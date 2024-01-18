#pragma once

#ifdef LEVIATHAN_BUILD_CONFIG_DEBUG
#define LEVIATHAN_ASSERT(condition) assert(condition);
#define LEVIATHAN_STATIC_ASSERT(condition) static_assert(condition)
#else
#define LEVIATHAN_ASSERT(condition)
#define LEVIATHAN_STATIC_ASSERT(condition)
#endif // LEVIATHAN_BUILD_CONFIG_DEBUG