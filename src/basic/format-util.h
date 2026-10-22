/* SPDX-License-Identifier: LGPL-2.1-or-later */
#आशय once

#समावेश "forward.h"
#समावेश "cgroup-util.h"
#समावेश "stdio-util.h"         /* IWYU pragma: keep */

assert_cc(माप(pid_t) == माप(int32_t));
#घोषणा PID_PRI PRIi32
#घोषणा PID_FMT "%" PID_PRI

assert_cc(माप(uid_t) == माप(uint32_t));
#घोषणा UID_FMT "%" PRIu32

assert_cc(माप(gid_t) == माप(uint32_t));
#घोषणा GID_FMT "%" PRIu32

/* Note: the lifetime of the compound literal is the immediately surrounding block,
 * see C11 §6.5.2.5, and
 * https://stackoverflow.com/questions/34880638/compound-literal-lifetime-and-if-blocks */
#घोषणा FORMAT_UID(uid) \
        snprintf_ok((अक्षर[DECIMAL_STR_MAX(uid_t)]){}, DECIMAL_STR_MAX(uid_t), UID_FMT, uid)
#घोषणा FORMAT_GID(gid) \
        snprintf_ok((अक्षर[DECIMAL_STR_MAX(gid_t)]){}, DECIMAL_STR_MAX(gid_t), GID_FMT, gid)

#अगर SIZEOF_TIME_T == 8
#  define PRI_TIME PRIi64
#या_अगर SIZEOF_TIME_T == 4
#  define PRI_TIME "li"
#अन्यथा
#  error Unknown समय_प्रकार आकार
#पूर्ण_अगर

#अगर SIZEOF_TIMEX_MEMBER == 8
#  define PRI_TIMEX PRIi64
#या_अगर SIZEOF_TIMEX_MEMBER == 4
#  define PRI_TIMEX "li"
#अन्यथा
#  error Unknown timex member आकार
#पूर्ण_अगर

#अगर_घोषित __GLIBC__
#  अगर SIZEOF_RLIM_T == 8
#    define RLIM_FMT "%" PRIu64
#  elif SIZEOF_RLIM_T == 4
#    define RLIM_FMT "%" PRIu32
#  अन्यथा
#    error Unknown rlim_t आकार
#  endif
#अन्यथा
/* Assume musl, and it unconditionally uses unsigned long long. */
assert_cc(SIZEOF_RLIM_T == 8);
#  define RLIM_FMT "%llu"
#पूर्ण_अगर

#अगर SIZEOF_DEV_T == 8
#  define DEV_FMT "%" PRIu64
#या_अगर SIZEOF_DEV_T == 4
#  define DEV_FMT "%" PRIu32
#अन्यथा
#  error Unknown dev_t आकार
#पूर्ण_अगर

#अगर SIZEOF_INO_T == 8
#  define INO_FMT "%" PRIu64
#या_अगर SIZEOF_INO_T == 4
#  define INO_FMT "%" PRIu32
#अन्यथा
#  error Unknown ino_t आकार
#पूर्ण_अगर

प्रकार क्रमागत {
        FORMAT_BYTES_USE_IEC      = 1 << 0, /* use base 1024 rather than 1000 */
        FORMAT_BYTES_BELOW_POINT  = 1 << 1, /* show one digit after the point, if non-zero */
        FORMAT_BYTES_ALWAYS_POINT = 1 << 2, /* show one digit after the point, always */
        FORMAT_BYTES_TRAILING_B   = 1 << 3, /* suffix the expression with a "B" for "bytes" */
} FormatBytesFlag;

#घोषणा FORMAT_BYTES_MAX 16U

अक्षर* format_bytes_full(अक्षर *buf, माप_प्रकार l, uint64_t t, FormatBytesFlag flag) warn_unused_परिणाम;

warn_unused_परिणाम
अटल अंतरभूत अक्षर* format_bytes(अक्षर *buf, माप_प्रकार l, uint64_t t) {
        वापस format_bytes_full(buf, l, t, FORMAT_BYTES_USE_IEC | FORMAT_BYTES_BELOW_POINT | FORMAT_BYTES_TRAILING_B);
}

/* Note: the lifetime of the compound literal is the immediately surrounding block,
 * see C11 §6.5.2.5, and
 * https://stackoverflow.com/questions/34880638/compound-literal-lifetime-and-if-blocks */
#घोषणा FORMAT_BYTES(t) format_bytes((अक्षर[FORMAT_BYTES_MAX]){}, FORMAT_BYTES_MAX, t)
#घोषणा FORMAT_BYTES_FULL(t, flags) format_bytes_full((अक्षर[FORMAT_BYTES_MAX]){}, FORMAT_BYTES_MAX, t, flags)
#घोषणा FORMAT_BYTES_WITH_POINT(t) format_bytes_full((अक्षर[FORMAT_BYTES_MAX]){}, FORMAT_BYTES_MAX, t, FORMAT_BYTES_USE_IEC|FORMAT_BYTES_ALWAYS_POINT|FORMAT_BYTES_TRAILING_B)

#घोषणा FORMAT_BYTES_CGROUP_PROTECTION(t) (t == CGROUP_LIMIT_MAX ? "infinity" : FORMAT_BYTES(t))
