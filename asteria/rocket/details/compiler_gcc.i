// This file is part of Asteria.
// Copyleft 2018 - 2021, LH_Mouse. All wrongs reserved.

#ifndef ROCKET_COMPILER_H_
#  error Please #include <rocket/compiler.h> instead.
#endif

#define ROCKET_ATTRIBUTE_PRINTF(...)        __attribute__((__format__(__gnu_printf__, __VA_ARGS__)))
#define ROCKET_SELECTANY                    __attribute__((__weak__))
#define ROCKET_SECTION(...)                 __attribute__((__section__(__VA_ARGS__)))
#define ROCKET_NOINLINE                     __attribute__((__noinline__))
#define ROCKET_PURE_FUNCTION                __attribute__((__pure__))
#define ROCKET_CONST_FUNCTION               __attribute__((__const__))
#define ROCKET_ARTIFICIAL_FUNCTION          __attribute__((__artificial__, __always_inline__))
#define ROCKET_FLATTEN_FUNCTION             __attribute__((__flatten__))
#define ROCKET_FORCED_INLINE_FUNCTION       __attribute__((__always_inline__)) __inline__

#define ROCKET_UNREACHABLE()                __builtin_unreachable()
#define ROCKET_EXPECT(...)                  __builtin_expect(!!(__VA_ARGS__), 1)
#define ROCKET_UNEXPECT(...)                __builtin_expect(!!(__VA_ARGS__), 0)
#define ROCKET_CONSTANT_P(...)              __builtin_constant_p(__VA_ARGS__)

#define ROCKET_FUNCSIG                      __PRETTY_FUNCTION__

#define ROCKET_LZCNT32(...)                 ((__VA_ARGS__ == 0) ? 32 : __builtin_clz(__VA_ARGS__))
#define ROCKET_LZCNT64(...)                 ((__VA_ARGS__ == 0) ? 64 : __builtin_clzll(__VA_ARGS__))
#define ROCKET_TZCNT32(...)                 ((__VA_ARGS__ == 0) ? 32 : __builtin_ctz(__VA_ARGS__))
#define ROCKET_TZCNT64(...)                 ((__VA_ARGS__ == 0) ? 64 : __builtin_ctzll(__VA_ARGS__))
#define ROCKET_POPCNT32(...)                __builtin_popcount(__VA_ARGS__)
#define ROCKET_POPCNT64(...)                __builtin_popcountll(__VA_ARGS__)

// Check for libstdc++.
#if defined(_GLIBCXX_DEBUG)
#  define ROCKET_DEBUG                      1
#endif
