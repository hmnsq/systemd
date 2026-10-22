/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ہدایت once

#ضمن "forward.h"
#ضمن "cgroup-util.h"
#ضمن "stdio-util.h"         /* IWYU pragma: keep */

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

#إذا SIZEOF_ویلا_T == 8
#  define PRI_ویلا PRIi64
#وإلاإذا SIZEOF_ویلا_T == 4
#  define PRI_ویلا "li"
#وإلا
#  error Unknown time_t ناپ
#جے_انت

#إذا SIZEOF_TIMEX_MEMBER == 8
#  define PRI_TIMEX PRIi64
#وإلاإذا SIZEOF_TIMEX_MEMBER == 4
#  define PRI_TIMEX "li"
#وإلا
#  error Unknown timex member ناپ
#جے_انت

#جے_تعریف_شدہ __GLIBC__
#  إذا SIZEOF_RLIM_T == 8
#    define RLIM_FMT "%" PRIu64
#  elif SIZEOF_RLIM_T == 4
#    define RLIM_FMT "%" PRIu32
#  وإلا
#    error Unknown rlim_t ناپ
#  endif
#وإلا
/* Assume musl, and it unconditionally uses unsigned long long. */
assert_cc(SIZEOF_RLIM_T == 8);
#  define RLIM_FMT "%llu"
#جے_انت

#إذا SIZEOF_DEV_T == 8
#  define DEV_FMT "%" PRIu64
#وإلاإذا SIZEOF_DEV_T == 4
#  define DEV_FMT "%" PRIu32
#وإلا
#  error Unknown dev_t ناپ
#جے_انت

#إذا SIZEOF_INO_T == 8
#  define INO_FMT "%" PRIu64
#وإلاإذا SIZEOF_INO_T == 4
#  define INO_FMT "%" PRIu32
#وإلا
#  error Unknown ino_t ناپ
#جے_انت

تعريفنوع تعداد {
        FORMAT_بائٹاں_USE_IEC      = 1 << 0, /* use base 1024 rather than 1000 */
        FORMAT_بائٹاں_ہیٹھاں_نقطہ  = 1 << 1, /* show one digit after the point, if non-zero */
        FORMAT_بائٹاں_ALWAYS_نقطہ = 1 << 2, /* show one digit after the point, always */
        FORMAT_بائٹاں_TRAILING_B   = 1 << 3, /* suffix the expression with a "B" for "bytes" */
} Format_بائٹاں_Flag;

#تعریف_کرو FORMAT_بائٹاں_MAX 16U

حرف* format_بائٹاں_بھریا(حرف *buf, size_t l, uint64_t t, Format_بائٹاں_Flag flag) warn_بن_ورتیا_نتیجہ;

warn_بن_ورتیا_نتیجہ
ساكن مضمن حرف* format_بائٹاں(حرف *buf, size_t l, uint64_t t) {
        أرجع format_بائٹاں_بھریا(buf, l, t, FORMAT_بائٹاں_USE_IEC | FORMAT_بائٹاں_ہیٹھاں_نقطہ | FORMAT_بائٹاں_TRAILING_B);
}

/* Note: the lifetime of the compound literal is the immediately surrounding block,
 * see C11 §6.5.2.5, and
 * https://stackoverflow.com/questions/34880638/compound-literal-lifetime-and-if-blocks */
#تعریف_کرو FORMAT_بائٹاں(t) format_بائٹاں((حرف[FORMAT_بائٹاں_MAX]){}, FORMAT_بائٹاں_MAX, t)
#تعریف_کرو FORMAT_بائٹاں_بھریا(t, flags) format_بائٹاں_بھریا((حرف[FORMAT_بائٹاں_MAX]){}, FORMAT_بائٹاں_MAX, t, flags)
#تعریف_کرو FORMAT_بائٹاں_WITH_نقطہ(t) format_بائٹاں_بھریا((حرف[FORMAT_بائٹاں_MAX]){}, FORMAT_بائٹاں_MAX, t, FORMAT_بائٹاں_USE_IEC|FORMAT_بائٹاں_ALWAYS_نقطہ|FORMAT_بائٹاں_TRAILING_B)

#تعریف_کرو FORMAT_بائٹاں_CGROUP_PROTECTION(t) (t == CGROUP_حد_بندی_MAX ? "infinity" : FORMAT_بائٹاں(t))
