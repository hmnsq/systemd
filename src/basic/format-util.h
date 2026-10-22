/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ "forward.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cgroup-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "stdio-util.h"         /* IWYU pragma: keep */

assert_cc(ਆਕਾਰ(pid_t) == ਆਕਾਰ(int32_t));
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PID_PRI PRIi32
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PID_FMT "%" PID_PRI

assert_cc(ਆਕਾਰ(uid_t) == ਆਕਾਰ(uint32_t));
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UID_FMT "%" PRIu32

assert_cc(ਆਕਾਰ(gid_t) == ਆਕਾਰ(uint32_t));
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ GID_FMT "%" PRIu32

/* Note: the lifetime of the compound literal is the immediately surrounding block,
 * see C11 §6.5.2.5, and
 * https://stackoverflow.com/questions/34880638/compound-literal-lifetime-and-if-blocks */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FORMAT_UID(uid) \
        snprintf_ok((ਅੱਖਰ[DECIMAL_STR_MAX(uid_t)]){}, DECIMAL_STR_MAX(uid_t), UID_FMT, uid)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FORMAT_GID(gid) \
        snprintf_ok((ਅੱਖਰ[DECIMAL_STR_MAX(gid_t)]){}, DECIMAL_STR_MAX(gid_t), GID_FMT, gid)

#ਜੇ SIZEOF_ਸਮਾਂ_T == 8
#  define PRI_ਸਮਾਂ PRIi64
#ਜਾਂਜੇ SIZEOF_ਸਮਾਂ_T == 4
#  define PRI_ਸਮਾਂ "li"
#ਨਹੀਂਤਾਂ
#  error Unknown time_t ਮਾਪ
#ਜੇ_ਅੰਤ

#ਜੇ SIZEOF_TIMEX_MEMBER == 8
#  define PRI_TIMEX PRIi64
#ਜਾਂਜੇ SIZEOF_TIMEX_MEMBER == 4
#  define PRI_TIMEX "li"
#ਨਹੀਂਤਾਂ
#  error Unknown timex member ਮਾਪ
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __GLIBC__
#  ਜੇ SIZEOF_RLIM_T == 8
#    define RLIM_FMT "%" PRIu64
#  elif SIZEOF_RLIM_T == 4
#    define RLIM_FMT "%" PRIu32
#  ਨਹੀਂਤਾਂ
#    error Unknown rlim_t ਮਾਪ
#  endif
#ਨਹੀਂਤਾਂ
/* Assume musl, and it unconditionally uses unsigned long long. */
assert_cc(SIZEOF_RLIM_T == 8);
#  define RLIM_FMT "%llu"
#ਜੇ_ਅੰਤ

#ਜੇ SIZEOF_DEV_T == 8
#  define DEV_FMT "%" PRIu64
#ਜਾਂਜੇ SIZEOF_DEV_T == 4
#  define DEV_FMT "%" PRIu32
#ਨਹੀਂਤਾਂ
#  error Unknown dev_t ਮਾਪ
#ਜੇ_ਅੰਤ

#ਜੇ SIZEOF_INO_T == 8
#  define INO_FMT "%" PRIu64
#ਜਾਂਜੇ SIZEOF_INO_T == 4
#  define INO_FMT "%" PRIu32
#ਨਹੀਂਤਾਂ
#  error Unknown ino_t ਮਾਪ
#ਜੇ_ਅੰਤ

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ {
        FORMAT_ਬਾਈਟਾਂ_USE_IEC      = 1 << 0, /* use base 1024 rather than 1000 */
        FORMAT_ਬਾਈਟਾਂ_ਹੇਠਾਂ_ਬਿੰਦੂ  = 1 << 1, /* show one digit after the point, if non-zero */
        FORMAT_ਬਾਈਟਾਂ_ALWAYS_ਬਿੰਦੂ = 1 << 2, /* show one digit after the point, always */
        FORMAT_ਬਾਈਟਾਂ_TRAILING_B   = 1 << 3, /* suffix the expression with a "B" for "bytes" */
} Format_ਬਾਈਟਾਂ_Flag;

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FORMAT_ਬਾਈਟਾਂ_MAX 16U

ਅੱਖਰ* format_ਬਾਈਟਾਂ_ਭਰਿਆ(ਅੱਖਰ *buf, size_t l, uint64_t t, Format_ਬਾਈਟਾਂ_Flag flag) warn_ਅਣਵਰਤਿਆ_ਨਤੀਜਾ;

warn_ਅਣਵਰਤਿਆ_ਨਤੀਜਾ
ਸਥਿਰ ਸਤਰਅੰਦਰ ਅੱਖਰ* format_ਬਾਈਟਾਂ(ਅੱਖਰ *buf, size_t l, uint64_t t) {
        ਵਾਪਸ format_ਬਾਈਟਾਂ_ਭਰਿਆ(buf, l, t, FORMAT_ਬਾਈਟਾਂ_USE_IEC | FORMAT_ਬਾਈਟਾਂ_ਹੇਠਾਂ_ਬਿੰਦੂ | FORMAT_ਬਾਈਟਾਂ_TRAILING_B);
}

/* Note: the lifetime of the compound literal is the immediately surrounding block,
 * see C11 §6.5.2.5, and
 * https://stackoverflow.com/questions/34880638/compound-literal-lifetime-and-if-blocks */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FORMAT_ਬਾਈਟਾਂ(t) format_ਬਾਈਟਾਂ((ਅੱਖਰ[FORMAT_ਬਾਈਟਾਂ_MAX]){}, FORMAT_ਬਾਈਟਾਂ_MAX, t)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FORMAT_ਬਾਈਟਾਂ_ਭਰਿਆ(t, flags) format_ਬਾਈਟਾਂ_ਭਰਿਆ((ਅੱਖਰ[FORMAT_ਬਾਈਟਾਂ_MAX]){}, FORMAT_ਬਾਈਟਾਂ_MAX, t, flags)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FORMAT_ਬਾਈਟਾਂ_WITH_ਬਿੰਦੂ(t) format_ਬਾਈਟਾਂ_ਭਰਿਆ((ਅੱਖਰ[FORMAT_ਬਾਈਟਾਂ_MAX]){}, FORMAT_ਬਾਈਟਾਂ_MAX, t, FORMAT_ਬਾਈਟਾਂ_USE_IEC|FORMAT_ਬਾਈਟਾਂ_ALWAYS_ਬਿੰਦੂ|FORMAT_ਬਾਈਟਾਂ_TRAILING_B)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FORMAT_ਬਾਈਟਾਂ_CGROUP_PROTECTION(t) (t == CGROUP_ਹੱਦਬੰਦੀ_MAX ? "infinity" : FORMAT_ਬਾਈਟਾਂ(t))
