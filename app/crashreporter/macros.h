#pragma once

#define ATTR_NONNULL_ALL __attribute__((nonnull))
#define ATTR_NONNULL(...) __attribute__((nonnull(__VA_ARGS__)))

#define ATTR_PRINTF(fmtpos, argpos) __attribute__(( format(printf, (fmtpos), (argpos) ) ))
#define ATTR_STRFTIME(fmtpos) __attribute__(( format(strftime, (fmtpos), 0 ) ))

#define ATTR_PACKED __attribute__((packed))

#define ATTR_WEAK __attribute__((weak))

#define ATTR_LIKELY(x) __builtin_expect((x), 1)
#define ATTR_UNLIKELY(x) __builtin_expect((x), 0)

#define ATTR_NORETURN __attribute__((noreturn))

// Allow to use the GCC10 access attribute even with older compilers (by ignoring it if unsupported)
#if !defined(__clang__) && defined(__GNUC__) && (__GNUC__ >= 10)
#define ATTR_CHK_R1(x) __attribute__((access(read_only,x)))
#define ATTR_CHK_RW1(x) __attribute__((access(read_write,x)))
#define ATTR_CHK_W1(x) __attribute__((access(write_only,x)))

#define ATTR_CHK_R(x,y) __attribute__((access(read_only,x,y)))
#define ATTR_CHK_RW(x,y) __attribute__((access(read_write,x,y)))
#define ATTR_CHK_W(x,y) __attribute__((access(write_only,x,y)))
#else
#define ATTR_CHK_R1(x)
#define ATTR_CHK_RW1(x)
#define ATTR_CHK_W1(x)

#define ATTR_CHK_R(x,y)
#define ATTR_CHK_RW(x,y)
#define ATTR_CHK_W(x,y)
#endif

#define NUM_ELEMS(X) (sizeof(X) / sizeof(X[0]))
