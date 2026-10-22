/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ہدایت once

#شامل "forward.h"
#شامل "cgroup-util.h"
#شامل "stdio-util.h"         /* IWYU pragma: keep */

assert_cc(حجم(pid_t) == حجم(int32_t));
#تعریف_کرو PID_PRI PRIi32
#تعریف_کرو PID_FMT "%" PID_PRI

assert_cc(حجم(uid_t) == حجم(uint32_t));
#تعریف_کرو UID_FMT "%" PRIu32

assert_cc(حجم(gid_t) == حجم(uint32_t));
#تعریف_کرو GID_FMT "%" PRIu32

/* Note: the lifetime of the compound literal is the immediately surrounding block,
 * see C11 §6.5.2.5, and
 * https://stackoverflow.com/questions/34880638/compound-literal-lifetime-and-if-blocks */
#تعریف_کرو FORMAT_UID(uid) \
        snprintf_ok((حرف[DECIMAL_STR_MAX(uid_t)]){}, DECIMAL_STR_MAX(uid_t), UID_FMT, uid)
#تعریف_کرو FORMAT_GID(gid) \
        snprintf_ok((حرف[DECIMAL_STR_MAX(gid_t)]){}, DECIMAL_STR_MAX(gid_t), GID_FMT, gid)

#اگر SIZEOF_وقت_T == 8
#  define PRI_وقت PRIi64
#وگرنہ_اگر SIZEOF_وقت_T == 4
#  define PRI_وقت "li"
#ورنہ
#  error Unknown time_t سائز
#اگر_ختم

#اگر SIZEOF_TIMEX_MEMBER == 8
#  define PRI_TIMEX PRIi64
#وگرنہ_اگر SIZEOF_TIMEX_MEMBER == 4
#  define PRI_TIMEX "li"
#ورنہ
#  error Unknown timex member سائز
#اگر_ختم

#اگر_تعریف_شدہ __GLIBC__
#  اگر SIZEOF_RLIM_T == 8
#    define RLIM_FMT "%" PRIu64
#  elif SIZEOF_RLIM_T == 4
#    define RLIM_FMT "%" PRIu32
#  ورنہ
#    error Unknown rlim_t سائز
#  endif
#ورنہ
/* Assume musl, and it unconditionally uses unsigned long long. */
assert_cc(SIZEOF_RLIM_T == 8);
#  define RLIM_FMT "%llu"
#اگر_ختم

#اگر SIZEOF_DEV_T == 8
#  define DEV_FMT "%" PRIu64
#وگرنہ_اگر SIZEOF_DEV_T == 4
#  define DEV_FMT "%" PRIu32
#ورنہ
#  error Unknown dev_t سائز
#اگر_ختم

#اگر SIZEOF_INO_T == 8
#  define INO_FMT "%" PRIu64
#وگرنہ_اگر SIZEOF_INO_T == 4
#  define INO_FMT "%" PRIu32
#ورنہ
#  error Unknown ino_t سائز
#اگر_ختم

قسمتعریف شمار {
        FORMAT_بائٹیں_USE_IEC      = 1 << 0, /* use base 1024 rather than 1000 */
        FORMAT_بائٹیں_نیچے_نقطہ  = 1 << 1, /* show one digit after the point, if non-zero */
        FORMAT_بائٹیں_ALWAYS_نقطہ = 1 << 2, /* show one digit after the point, always */
        FORMAT_بائٹیں_TRAILING_B   = 1 << 3, /* suffix the expression with a "B" for "bytes" */
} Format_بائٹیں_Flag;

#تعریف_کرو FORMAT_بائٹیں_MAX 16U

حرف* format_بائٹیں_بھرا(حرف *buf, size_t l, uint64_t t, Format_بائٹیں_Flag flag) warn_غیر_مستعمل_نتیجہ;

warn_غیر_مستعمل_نتیجہ
ساکن اندرونی حرف* format_بائٹیں(حرف *buf, size_t l, uint64_t t) {
        واپس format_بائٹیں_بھرا(buf, l, t, FORMAT_بائٹیں_USE_IEC | FORMAT_بائٹیں_نیچے_نقطہ | FORMAT_بائٹیں_TRAILING_B);
}

/* Note: the lifetime of the compound literal is the immediately surrounding block,
 * see C11 §6.5.2.5, and
 * https://stackoverflow.com/questions/34880638/compound-literal-lifetime-and-if-blocks */
#تعریف_کرو FORMAT_بائٹیں(t) format_بائٹیں((حرف[FORMAT_بائٹیں_MAX]){}, FORMAT_بائٹیں_MAX, t)
#تعریف_کرو FORMAT_بائٹیں_بھرا(t, flags) format_بائٹیں_بھرا((حرف[FORMAT_بائٹیں_MAX]){}, FORMAT_بائٹیں_MAX, t, flags)
#تعریف_کرو FORMAT_بائٹیں_WITH_نقطہ(t) format_بائٹیں_بھرا((حرف[FORMAT_بائٹیں_MAX]){}, FORMAT_بائٹیں_MAX, t, FORMAT_بائٹیں_USE_IEC|FORMAT_بائٹیں_ALWAYS_نقطہ|FORMAT_بائٹیں_TRAILING_B)

#تعریف_کرو FORMAT_بائٹیں_CGROUP_PROTECTION(t) (t == CGROUP_حد_بندی_MAX ? "infinity" : FORMAT_بائٹیں(t))
