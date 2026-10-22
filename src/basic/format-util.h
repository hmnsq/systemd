/* SPDX-License-Identifier: LGPL-2.1-or-later */
#لارښوونه once

#ضمن "forward.h"
#ضمن "cgroup-util.h"
#ضمن "stdio-util.h"         /* IWYU pragma: keep */

assert_cc(حجم(pid_t) == حجم(int32_t));
#تعریف_کړه PID_PRI PRIi32
#تعریف_کړه PID_FMT "%" PID_PRI

assert_cc(حجم(uid_t) == حجم(uint32_t));
#تعریف_کړه UID_FMT "%" PRIu32

assert_cc(حجم(gid_t) == حجم(uint32_t));
#تعریف_کړه GID_FMT "%" PRIu32

/* Note: the lifetime of the compound literal is the immediately surrounding block,
 * see C11 §6.5.2.5, and
 * https://stackoverflow.com/questions/34880638/compound-literal-lifetime-and-if-blocks */
#تعریف_کړه FORMAT_UID(uid) \
        snprintf_ok((حرف[DECIMAL_STR_MAX(uid_t)]){}, DECIMAL_STR_MAX(uid_t), UID_FMT, uid)
#تعریف_کړه FORMAT_GID(gid) \
        snprintf_ok((حرف[DECIMAL_STR_MAX(gid_t)]){}, DECIMAL_STR_MAX(gid_t), GID_FMT, gid)

#إذا SIZEOF_وخت_T == 8
#  define PRI_وخت PRIi64
#وإلاإذا SIZEOF_وخت_T == 4
#  define PRI_وخت "li"
#وإلا
#  error Unknown time_t اندازه
#د_که_پای

#إذا SIZEOF_TIMEX_MEMBER == 8
#  define PRI_TIMEX PRIi64
#وإلاإذا SIZEOF_TIMEX_MEMBER == 4
#  define PRI_TIMEX "li"
#وإلا
#  error Unknown timex member اندازه
#د_که_پای

#که_تعریف_شوی __GLIBC__
#  إذا SIZEOF_RLIM_T == 8
#    define RLIM_FMT "%" PRIu64
#  elif SIZEOF_RLIM_T == 4
#    define RLIM_FMT "%" PRIu32
#  وإلا
#    error Unknown rlim_t اندازه
#  endif
#وإلا
/* Assume musl, and it unconditionally uses unsigned long long. */
assert_cc(SIZEOF_RLIM_T == 8);
#  define RLIM_FMT "%llu"
#د_که_پای

#إذا SIZEOF_DEV_T == 8
#  define DEV_FMT "%" PRIu64
#وإلاإذا SIZEOF_DEV_T == 4
#  define DEV_FMT "%" PRIu32
#وإلا
#  error Unknown dev_t اندازه
#د_که_پای

#إذا SIZEOF_INO_T == 8
#  define INO_FMT "%" PRIu64
#وإلاإذا SIZEOF_INO_T == 4
#  define INO_FMT "%" PRIu32
#وإلا
#  error Unknown ino_t اندازه
#د_که_پای

تعريفنوع تعداد {
        FORMAT_بایټونه_USE_IEC      = 1 << 0, /* use base 1024 rather than 1000 */
        FORMAT_بایټونه_لاندې_ټکی  = 1 << 1, /* show one digit after the point, if non-zero */
        FORMAT_بایټونه_ALWAYS_ټکی = 1 << 2, /* show one digit after the point, always */
        FORMAT_بایټونه_TRAILING_B   = 1 << 3, /* suffix the expression with a "B" for "bytes" */
} Format_بایټونه_Flag;

#تعریف_کړه FORMAT_بایټونه_MAX 16U

حرف* format_بایټونه_ډک(حرف *buf, size_t l, uint64_t t, Format_بایټونه_Flag flag) warn_ناکارول_شوی_پایله;

warn_ناکارول_شوی_پایله
ساكن مضمن حرف* format_بایټونه(حرف *buf, size_t l, uint64_t t) {
        أرجع format_بایټونه_ډک(buf, l, t, FORMAT_بایټونه_USE_IEC | FORMAT_بایټونه_لاندې_ټکی | FORMAT_بایټونه_TRAILING_B);
}

/* Note: the lifetime of the compound literal is the immediately surrounding block,
 * see C11 §6.5.2.5, and
 * https://stackoverflow.com/questions/34880638/compound-literal-lifetime-and-if-blocks */
#تعریف_کړه FORMAT_بایټونه(t) format_بایټونه((حرف[FORMAT_بایټونه_MAX]){}, FORMAT_بایټونه_MAX, t)
#تعریف_کړه FORMAT_بایټونه_ډک(t, flags) format_بایټونه_ډک((حرف[FORMAT_بایټونه_MAX]){}, FORMAT_بایټونه_MAX, t, flags)
#تعریف_کړه FORMAT_بایټونه_WITH_ټکی(t) format_بایټونه_ډک((حرف[FORMAT_بایټونه_MAX]){}, FORMAT_بایټونه_MAX, t, FORMAT_بایټونه_USE_IEC|FORMAT_بایټونه_ALWAYS_ټکی|FORMAT_بایټونه_TRAILING_B)

#تعریف_کړه FORMAT_بایټونه_CGROUP_PROTECTION(t) (t == CGROUP_حد_MAX ? "infinity" : FORMAT_بایټونه(t))
