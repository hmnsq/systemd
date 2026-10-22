/* SPDX-License-Identifier: LGPL-2.1-or-later */
#رهنمود once

#ضمن "forward.h"
#ضمن "cgroup-util.h"
#ضمن "stdio-util.h"         /* IWYU pragma: keep */

assert_cc(حجم(pid_t) == حجم(int32_t));
#تعریف_کن PID_PRI PRIi32
#تعریف_کن PID_FMT "%" PID_PRI

assert_cc(حجم(uid_t) == حجم(uint32_t));
#تعریف_کن UID_FMT "%" PRIu32

assert_cc(حجم(gid_t) == حجم(uint32_t));
#تعریف_کن GID_FMT "%" PRIu32

/* Note: the lifetime of the compound literal is the immediately surrounding block,
 * see C11 §6.5.2.5, and
 * https://stackoverflow.com/questions/34880638/compound-literal-lifetime-and-if-blocks */
#تعریف_کن FORMAT_UID(uid) \
        snprintf_ok((حرف[DECIMAL_STR_MAX(uid_t)]){}, DECIMAL_STR_MAX(uid_t), UID_FMT, uid)
#تعریف_کن FORMAT_GID(gid) \
        snprintf_ok((حرف[DECIMAL_STR_MAX(gid_t)]){}, DECIMAL_STR_MAX(gid_t), GID_FMT, gid)

#إذا SIZEOF_زمان_T == 8
#  define PRI_زمان PRIi64
#وإلاإذا SIZEOF_زمان_T == 4
#  define PRI_زمان "li"
#وإلا
#  error Unknown time_t اندازه
#پایان_اگر

#إذا SIZEOF_TIMEX_MEMBER == 8
#  define PRI_TIMEX PRIi64
#وإلاإذا SIZEOF_TIMEX_MEMBER == 4
#  define PRI_TIMEX "li"
#وإلا
#  error Unknown timex member اندازه
#پایان_اگر

#اگر_تعریف_شده __GLIBC__
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
#پایان_اگر

#إذا SIZEOF_DEV_T == 8
#  define DEV_FMT "%" PRIu64
#وإلاإذا SIZEOF_DEV_T == 4
#  define DEV_FMT "%" PRIu32
#وإلا
#  error Unknown dev_t اندازه
#پایان_اگر

#إذا SIZEOF_INO_T == 8
#  define INO_FMT "%" PRIu64
#وإلاإذا SIZEOF_INO_T == 4
#  define INO_FMT "%" PRIu32
#وإلا
#  error Unknown ino_t اندازه
#پایان_اگر

تعريفنوع تعداد {
        FORMAT_بایتها_USE_IEC      = 1 << 0, /* use base 1024 rather than 1000 */
        FORMAT_بایتها_زیر_نقطه  = 1 << 1, /* show one digit after the point, if non-zero */
        FORMAT_بایتها_ALWAYS_نقطه = 1 << 2, /* show one digit after the point, always */
        FORMAT_بایتها_TRAILING_B   = 1 << 3, /* suffix the expression with a "B" for "bytes" */
} Format_بایتها_Flag;

#تعریف_کن FORMAT_بایتها_MAX 16U

حرف* format_بایتها_پر(حرف *buf, size_t l, uint64_t t, Format_بایتها_Flag flag) warn_استفادهنشده_نتیجه;

warn_استفادهنشده_نتیجه
ساكن مضمن حرف* format_بایتها(حرف *buf, size_t l, uint64_t t) {
        أرجع format_بایتها_پر(buf, l, t, FORMAT_بایتها_USE_IEC | FORMAT_بایتها_زیر_نقطه | FORMAT_بایتها_TRAILING_B);
}

/* Note: the lifetime of the compound literal is the immediately surrounding block,
 * see C11 §6.5.2.5, and
 * https://stackoverflow.com/questions/34880638/compound-literal-lifetime-and-if-blocks */
#تعریف_کن FORMAT_بایتها(t) format_بایتها((حرف[FORMAT_بایتها_MAX]){}, FORMAT_بایتها_MAX, t)
#تعریف_کن FORMAT_بایتها_پر(t, flags) format_بایتها_پر((حرف[FORMAT_بایتها_MAX]){}, FORMAT_بایتها_MAX, t, flags)
#تعریف_کن FORMAT_بایتها_WITH_نقطه(t) format_بایتها_پر((حرف[FORMAT_بایتها_MAX]){}, FORMAT_بایتها_MAX, t, FORMAT_بایتها_USE_IEC|FORMAT_بایتها_ALWAYS_نقطه|FORMAT_بایتها_TRAILING_B)

#تعریف_کن FORMAT_بایتها_CGROUP_PROTECTION(t) (t == CGROUP_حد_MAX ? "infinity" : FORMAT_بایتها(t))
