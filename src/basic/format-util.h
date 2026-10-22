/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#अन्तर्भाव "forward.h"
#अन्तर्भाव "cgroup-util.h"
#अन्तर्भाव "stdio-util.h"         /* IWYU pragma: keep */

assert_cc(आकार(pid_t) == आकार(int32_t));
#define PID_PRI PRIi32
#define PID_FMT "%" PID_PRI

assert_cc(आकार(uid_t) == आकार(uint32_t));
#define UID_FMT "%" PRIu32

assert_cc(आकार(gid_t) == आकार(uint32_t));
#define GID_FMT "%" PRIu32

/* Note: the lifetime of the compound literal is the immediately surrounding block,
 * see C11 §6.5.2.5, and
 * https://stackoverflow.com/questions/34880638/compound-literal-lifetime-and-if-blocks */
#define FORMAT_UID(uid) \
        snprintf_ok((अक्षर[DECIMAL_STR_MAX(uid_t)]){}, DECIMAL_STR_MAX(uid_t), UID_FMT, uid)
#define FORMAT_GID(gid) \
        snprintf_ok((अक्षर[DECIMAL_STR_MAX(gid_t)]){}, DECIMAL_STR_MAX(gid_t), GID_FMT, gid)

#जर SIZEOF_TIME_T == 8
#  define PRI_TIME PRIi64
#किंवाजर SIZEOF_TIME_T == 4
#  define PRI_TIME "li"
#अन्यथा
#  error Unknown समय_प्रकार size
#endif

#जर SIZEOF_TIMEX_MEMBER == 8
#  define PRI_TIMEX PRIi64
#किंवाजर SIZEOF_TIMEX_MEMBER == 4
#  define PRI_TIMEX "li"
#अन्यथा
#  error Unknown timex member size
#endif

#ifdef __GLIBC__
#  जर SIZEOF_RLIM_T == 8
#    define RLIM_FMT "%" PRIu64
#  elif SIZEOF_RLIM_T == 4
#    define RLIM_FMT "%" PRIu32
#  अन्यथा
#    error Unknown rlim_t size
#  endif
#अन्यथा
/* Assume musl, and it unconditionally uses unsigned long long. */
assert_cc(SIZEOF_RLIM_T == 8);
#  define RLIM_FMT "%llu"
#endif

#जर SIZEOF_DEV_T == 8
#  define DEV_FMT "%" PRIu64
#किंवाजर SIZEOF_DEV_T == 4
#  define DEV_FMT "%" PRIu32
#अन्यथा
#  error Unknown dev_t size
#endif

#जर SIZEOF_INO_T == 8
#  define INO_FMT "%" PRIu64
#किंवाजर SIZEOF_INO_T == 4
#  define INO_FMT "%" PRIu32
#अन्यथा
#  error Unknown ino_t size
#endif

प्रकारव्याख्या गणना {
        FORMAT_BYTES_USE_IEC      = 1 << 0, /* use base 1024 rather than 1000 */
        FORMAT_BYTES_BELOW_POINT  = 1 << 1, /* show one digit after the point, if non-zero */
        FORMAT_BYTES_ALWAYS_POINT = 1 << 2, /* show one digit after the point, always */
        FORMAT_BYTES_TRAILING_B   = 1 << 3, /* suffix the expression with a "B" for "bytes" */
} FormatBytesFlag;

#define FORMAT_BYTES_MAX 16U

अक्षर* format_bytes_full(अक्षर *buf, माप_प्रकार l, uint64_t t, FormatBytesFlag flag) _warn_unused_result_;

_warn_unused_result_
स्थिर ओळीत अक्षर* format_bytes(अक्षर *buf, माप_प्रकार l, uint64_t t) {
        परत format_bytes_full(buf, l, t, FORMAT_BYTES_USE_IEC | FORMAT_BYTES_BELOW_POINT | FORMAT_BYTES_TRAILING_B);
}

/* Note: the lifetime of the compound literal is the immediately surrounding block,
 * see C11 §6.5.2.5, and
 * https://stackoverflow.com/questions/34880638/compound-literal-lifetime-and-if-blocks */
#define FORMAT_BYTES(t) format_bytes((अक्षर[FORMAT_BYTES_MAX]){}, FORMAT_BYTES_MAX, t)
#define FORMAT_BYTES_FULL(t, flags) format_bytes_full((अक्षर[FORMAT_BYTES_MAX]){}, FORMAT_BYTES_MAX, t, flags)
#define FORMAT_BYTES_WITH_POINT(t) format_bytes_full((अक्षर[FORMAT_BYTES_MAX]){}, FORMAT_BYTES_MAX, t, FORMAT_BYTES_USE_IEC|FORMAT_BYTES_ALWAYS_POINT|FORMAT_BYTES_TRAILING_B)

#define FORMAT_BYTES_CGROUP_PROTECTION(t) (t == CGROUP_LIMIT_MAX ? "infinity" : FORMAT_BYTES(t))
