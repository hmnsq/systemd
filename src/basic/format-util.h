/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#અન્તર્ભાવ "forward.h"
#અન્તર્ભાવ "cgroup-util.h"
#અન્તર્ભાવ "stdio-util.h"         /* IWYU pragma: keep */

assert_cc(કદ(pid_t) == કદ(int32_t));
#define PID_PRI PRIi32
#define PID_FMT "%" PID_PRI

assert_cc(કદ(uid_t) == કદ(uint32_t));
#define UID_FMT "%" PRIu32

assert_cc(કદ(gid_t) == કદ(uint32_t));
#define GID_FMT "%" PRIu32

/* Note: the lifetime of the compound literal is the immediately surrounding block,
 * see C11 §6.5.2.5, and
 * https://stackoverflow.com/questions/34880638/compound-literal-lifetime-and-if-blocks */
#define FORMAT_UID(uid) \
        snprintf_ok((અક્ષર[DECIMAL_STR_MAX(uid_t)]){}, DECIMAL_STR_MAX(uid_t), UID_FMT, uid)
#define FORMAT_GID(gid) \
        snprintf_ok((અક્ષર[DECIMAL_STR_MAX(gid_t)]){}, DECIMAL_STR_MAX(gid_t), GID_FMT, gid)

#જો SIZEOF_TIME_T == 8
#  define PRI_TIME PRIi64
#અથવાજો SIZEOF_TIME_T == 4
#  define PRI_TIME "li"
#નહીંતર
#  error Unknown time_t size
#endif

#જો SIZEOF_TIMEX_MEMBER == 8
#  define PRI_TIMEX PRIi64
#અથવાજો SIZEOF_TIMEX_MEMBER == 4
#  define PRI_TIMEX "li"
#નહીંતર
#  error Unknown timex member size
#endif

#ifdef __GLIBC__
#  જો SIZEOF_RLIM_T == 8
#    define RLIM_FMT "%" PRIu64
#  elif SIZEOF_RLIM_T == 4
#    define RLIM_FMT "%" PRIu32
#  નહીંતર
#    error Unknown rlim_t size
#  endif
#નહીંતર
/* Assume musl, and it unconditionally uses unsigned long long. */
assert_cc(SIZEOF_RLIM_T == 8);
#  define RLIM_FMT "%llu"
#endif

#જો SIZEOF_DEV_T == 8
#  define DEV_FMT "%" PRIu64
#અથવાજો SIZEOF_DEV_T == 4
#  define DEV_FMT "%" PRIu32
#નહીંતર
#  error Unknown dev_t size
#endif

#જો SIZEOF_INO_T == 8
#  define INO_FMT "%" PRIu64
#અથવાજો SIZEOF_INO_T == 4
#  define INO_FMT "%" PRIu32
#નહીંતર
#  error Unknown ino_t size
#endif

પ્રકારવ્યાખ્યા ગણના {
        FORMAT_BYTES_USE_IEC      = 1 << 0, /* use base 1024 rather than 1000 */
        FORMAT_BYTES_BELOW_POINT  = 1 << 1, /* show one digit after the point, if non-zero */
        FORMAT_BYTES_ALWAYS_POINT = 1 << 2, /* show one digit after the point, always */
        FORMAT_BYTES_TRAILING_B   = 1 << 3, /* suffix the expression with a "B" for "bytes" */
} FormatBytesFlag;

#define FORMAT_BYTES_MAX 16U

અક્ષર* format_bytes_full(અક્ષર *buf, size_t l, uint64_t t, FormatBytesFlag flag) _warn_unused_result_;

_warn_unused_result_
સ્થિર અંતઃરેખ અક્ષર* format_bytes(અક્ષર *buf, size_t l, uint64_t t) {
        પાછા format_bytes_full(buf, l, t, FORMAT_BYTES_USE_IEC | FORMAT_BYTES_BELOW_POINT | FORMAT_BYTES_TRAILING_B);
}

/* Note: the lifetime of the compound literal is the immediately surrounding block,
 * see C11 §6.5.2.5, and
 * https://stackoverflow.com/questions/34880638/compound-literal-lifetime-and-if-blocks */
#define FORMAT_BYTES(t) format_bytes((અક્ષર[FORMAT_BYTES_MAX]){}, FORMAT_BYTES_MAX, t)
#define FORMAT_BYTES_FULL(t, flags) format_bytes_full((અક્ષર[FORMAT_BYTES_MAX]){}, FORMAT_BYTES_MAX, t, flags)
#define FORMAT_BYTES_WITH_POINT(t) format_bytes_full((અક્ષર[FORMAT_BYTES_MAX]){}, FORMAT_BYTES_MAX, t, FORMAT_BYTES_USE_IEC|FORMAT_BYTES_ALWAYS_POINT|FORMAT_BYTES_TRAILING_B)

#define FORMAT_BYTES_CGROUP_PROTECTION(t) (t == CGROUP_LIMIT_MAX ? "infinity" : FORMAT_BYTES(t))
