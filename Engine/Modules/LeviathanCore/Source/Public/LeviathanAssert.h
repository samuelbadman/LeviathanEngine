#pragma once

#ifdef LEVIATHAN_BUILD_CONFIG_DEBUG
#define LEVIATHAN_ASSERT(condition) assert(condition);
#else
#define LEVIATHAN_ASSERT(condition)
#endif // LEVIATHAN_BUILD_CONFIG_DEBUG