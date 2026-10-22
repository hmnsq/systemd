/* SPDX-License-Identifier: LGPL-2.1-or-later */
#هدايت once

#ضمن "forward.h"
#ضمن "cgroup-util.h"
#ضمن "stdio-util.h"         /* IWYU pragma: keep */

assert_cc(حجم(pid_t) == حجم(int32_t));
#وصف_ڪريو PID_PRI PRIi32
#وصف_ڪريو PID_FMT "%" PID_PRI

assert_cc(حجم(uid_t) == حجم(uint32_t));
#وصف_ڪريو UID_FMT "%" PRIu32

assert_cc(حجم(gid_t) == حجم(uint32_t));
#وصف_ڪريو GID_FMT "%" PRIu32

/* Note: the lifetime of the compound literal is the immediately surrounding block,
 * see C11 §6.5.2.5, and
 * https://stackoverflow.com/questions/34880638/compound-literal-lifetime-and-if-blocks */
#وصف_ڪريو FORMAT_UID(uid) \
        snprintf_ok((حرف[DECIMAL_STR_MAX(uid_t)]){}, DECIMAL_STR_MAX(uid_t), UID_FMT, uid)
#وصف_ڪريو FORMAT_GID(gid) \
        snprintf_ok((حرف[DECIMAL_STR_MAX(gid_t)]){}, DECIMAL_STR_MAX(gid_t), GID_FMT, gid)

#إذا SIZEOF_وقت_T == 8
#  define PRI_وقت PRIi64
#وإلاإذا SIZEOF_وقت_T == 4
#  define PRI_وقت "li"
#وإلا
#  error Unknown time_t سائيز
#جيڪڏهن_پورو

#إذا SIZEOF_TIMEX_MEMBER == 8
#  define PRI_TIMEX PRIi64
#وإلاإذا SIZEOF_TIMEX_MEMBER == 4
#  define PRI_TIMEX "li"
#وإلا
#  error Unknown timex member سائيز
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل __GLIBC__
#  إذا SIZEOF_RLIM_T == 8
#    define RLIM_FMT "%" PRIu64
#  elif SIZEOF_RLIM_T == 4
#    define RLIM_FMT "%" PRIu32
#  وإلا
#    error Unknown rlim_t سائيز
#  endif
#وإلا
/* Assume musl, and it unconditionally uses unsigned long long. */
assert_cc(SIZEOF_RLIM_T == 8);
#  define RLIM_FMT "%llu"
#جيڪڏهن_پورو

#إذا SIZEOF_DEV_T == 8
#  define DEV_FMT "%" PRIu64
#وإلاإذا SIZEOF_DEV_T == 4
#  define DEV_FMT "%" PRIu32
#وإلا
#  error Unknown dev_t سائيز
#جيڪڏهن_پورو

#إذا SIZEOF_INO_T == 8
#  define INO_FMT "%" PRIu64
#وإلاإذا SIZEOF_INO_T == 4
#  define INO_FMT "%" PRIu32
#وإلا
#  error Unknown ino_t سائيز
#جيڪڏهن_پورو

تعريفنوع تعداد {
        FORMAT_بائيٽون_USE_IEC      = 1 << 0, /* use base 1024 rather than 1000 */
        FORMAT_بائيٽون_هيٺ_نقطو  = 1 << 1, /* show one digit after the point, if non-zero */
        FORMAT_بائيٽون_ALWAYS_نقطو = 1 << 2, /* show one digit after the point, always */
        FORMAT_بائيٽون_TRAILING_B   = 1 << 3, /* suffix the expression with a "B" for "bytes" */
} Format_بائيٽون_Flag;

#وصف_ڪريو FORMAT_بائيٽون_MAX 16U

حرف* format_بائيٽون_ڀريل(حرف *buf, size_t l, uint64_t t, Format_بائيٽون_Flag flag) warn_اڻ_استعمال_ٿيل_نتيجو;

warn_اڻ_استعمال_ٿيل_نتيجو
ساكن مضمن حرف* format_بائيٽون(حرف *buf, size_t l, uint64_t t) {
        أرجع format_بائيٽون_ڀريل(buf, l, t, FORMAT_بائيٽون_USE_IEC | FORMAT_بائيٽون_هيٺ_نقطو | FORMAT_بائيٽون_TRAILING_B);
}

/* Note: the lifetime of the compound literal is the immediately surrounding block,
 * see C11 §6.5.2.5, and
 * https://stackoverflow.com/questions/34880638/compound-literal-lifetime-and-if-blocks */
#وصف_ڪريو FORMAT_بائيٽون(t) format_بائيٽون((حرف[FORMAT_بائيٽون_MAX]){}, FORMAT_بائيٽون_MAX, t)
#وصف_ڪريو FORMAT_بائيٽون_ڀريل(t, flags) format_بائيٽون_ڀريل((حرف[FORMAT_بائيٽون_MAX]){}, FORMAT_بائيٽون_MAX, t, flags)
#وصف_ڪريو FORMAT_بائيٽون_WITH_نقطو(t) format_بائيٽون_ڀريل((حرف[FORMAT_بائيٽون_MAX]){}, FORMAT_بائيٽون_MAX, t, FORMAT_بائيٽون_USE_IEC|FORMAT_بائيٽون_ALWAYS_نقطو|FORMAT_بائيٽون_TRAILING_B)

#وصف_ڪريو FORMAT_بائيٽون_CGROUP_PROTECTION(t) (t == CGROUP_حد_بندي_MAX ? "infinity" : FORMAT_بائيٽون(t))
