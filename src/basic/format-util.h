/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ಸೂಚನೆ once

#ಅನ್ತರ್ಭಾವ "forward.h"
#ಅನ್ತರ್ಭಾವ "cgroup-util.h"
#ಅನ್ತರ್ಭಾವ "stdio-util.h"         /* IWYU pragma: keep */

assert_cc(ಗಾತ್ರ(pid_t) == ಗಾತ್ರ(int32_t));
#ವ್ಯಾಖ್ಯಾನಿಸು PID_PRI PRIi32
#ವ್ಯಾಖ್ಯಾನಿಸು PID_FMT "%" PID_PRI

assert_cc(ಗಾತ್ರ(uid_t) == ಗಾತ್ರ(uint32_t));
#ವ್ಯಾಖ್ಯಾನಿಸು UID_FMT "%" PRIu32

assert_cc(ಗಾತ್ರ(gid_t) == ಗಾತ್ರ(uint32_t));
#ವ್ಯಾಖ್ಯಾನಿಸು GID_FMT "%" PRIu32

/* Note: the lifetime of the compound literal is the immediately surrounding block,
 * see C11 §6.5.2.5, and
 * https://stackoverflow.com/questions/34880638/compound-literal-lifetime-and-if-blocks */
#ವ್ಯಾಖ್ಯಾನಿಸು FORMAT_UID(uid) \
        snprintf_ok((ಅಕ್ಷರ[DECIMAL_STR_MAX(uid_t)]){}, DECIMAL_STR_MAX(uid_t), UID_FMT, uid)
#ವ್ಯಾಖ್ಯಾನಿಸು FORMAT_GID(gid) \
        snprintf_ok((ಅಕ್ಷರ[DECIMAL_STR_MAX(gid_t)]){}, DECIMAL_STR_MAX(gid_t), GID_FMT, gid)

#ಆದರೆ SIZEOF_ಸಮಯ_T == 8
#  define PRI_ಸಮಯ PRIi64
#ಅಥವಾಆದರೆ SIZEOF_ಸಮಯ_T == 4
#  define PRI_ಸಮಯ "li"
#ಇಲ್ಲದಿದ್ದರೆ
#  error Unknown time_t ಗಾತ್ರ_2
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ SIZEOF_TIMEX_MEMBER == 8
#  define PRI_TIMEX PRIi64
#ಅಥವಾಆದರೆ SIZEOF_TIMEX_MEMBER == 4
#  define PRI_TIMEX "li"
#ಇಲ್ಲದಿದ್ದರೆ
#  error Unknown timex member ಗಾತ್ರ_2
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __GLIBC__
#  ಆದರೆ SIZEOF_RLIM_T == 8
#    define RLIM_FMT "%" PRIu64
#  elif SIZEOF_RLIM_T == 4
#    define RLIM_FMT "%" PRIu32
#  ಇಲ್ಲದಿದ್ದರೆ
#    error Unknown rlim_t ಗಾತ್ರ_2
#  endif
#ಇಲ್ಲದಿದ್ದರೆ
/* Assume musl, and it unconditionally uses unsigned long long. */
assert_cc(SIZEOF_RLIM_T == 8);
#  define RLIM_FMT "%llu"
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ SIZEOF_DEV_T == 8
#  define DEV_FMT "%" PRIu64
#ಅಥವಾಆದರೆ SIZEOF_DEV_T == 4
#  define DEV_FMT "%" PRIu32
#ಇಲ್ಲದಿದ್ದರೆ
#  error Unknown dev_t ಗಾತ್ರ_2
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ SIZEOF_INO_T == 8
#  define INO_FMT "%" PRIu64
#ಅಥವಾಆದರೆ SIZEOF_INO_T == 4
#  define INO_FMT "%" PRIu32
#ಇಲ್ಲದಿದ್ದರೆ
#  error Unknown ino_t ಗಾತ್ರ_2
#ಆದರೆ_ಅಂತ್ಯ

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ {
        FORMAT_ಬೈಟುಗಳು_USE_IEC      = 1 << 0, /* use base 1024 rather than 1000 */
        FORMAT_ಬೈಟುಗಳು_ಕೆಳಗೆ_ಬಿಂದು  = 1 << 1, /* show one digit after the point, if non-zero */
        FORMAT_ಬೈಟುಗಳು_ALWAYS_ಬಿಂದು = 1 << 2, /* show one digit after the point, always */
        FORMAT_ಬೈಟುಗಳು_TRAILING_B   = 1 << 3, /* suffix the expression with a "B" for "bytes" */
} Format_ಬೈಟುಗಳು_Flag;

#ವ್ಯಾಖ್ಯಾನಿಸು FORMAT_ಬೈಟುಗಳು_MAX 16U

ಅಕ್ಷರ* format_ಬೈಟುಗಳು_ತುಂಬಿದ(ಅಕ್ಷರ *buf, size_t l, uint64_t t, Format_ಬೈಟುಗಳು_Flag flag) warn_ಬಳಸದ_ಫಲಿತಾಂಶ;

warn_ಬಳಸದ_ಫಲಿತಾಂಶ
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಅಕ್ಷರ* format_ಬೈಟುಗಳು(ಅಕ್ಷರ *buf, size_t l, uint64_t t) {
        ಹಿಂತಿರುಗಿ format_ಬೈಟುಗಳು_ತುಂಬಿದ(buf, l, t, FORMAT_ಬೈಟುಗಳು_USE_IEC | FORMAT_ಬೈಟುಗಳು_ಕೆಳಗೆ_ಬಿಂದು | FORMAT_ಬೈಟುಗಳು_TRAILING_B);
}

/* Note: the lifetime of the compound literal is the immediately surrounding block,
 * see C11 §6.5.2.5, and
 * https://stackoverflow.com/questions/34880638/compound-literal-lifetime-and-if-blocks */
#ವ್ಯಾಖ್ಯಾನಿಸು FORMAT_ಬೈಟುಗಳು(t) format_ಬೈಟುಗಳು((ಅಕ್ಷರ[FORMAT_ಬೈಟುಗಳು_MAX]){}, FORMAT_ಬೈಟುಗಳು_MAX, t)
#ವ್ಯಾಖ್ಯಾನಿಸು FORMAT_ಬೈಟುಗಳು_ತುಂಬಿದ(t, flags) format_ಬೈಟುಗಳು_ತುಂಬಿದ((ಅಕ್ಷರ[FORMAT_ಬೈಟುಗಳು_MAX]){}, FORMAT_ಬೈಟುಗಳು_MAX, t, flags)
#ವ್ಯಾಖ್ಯಾನಿಸು FORMAT_ಬೈಟುಗಳು_WITH_ಬಿಂದು(t) format_ಬೈಟುಗಳು_ತುಂಬಿದ((ಅಕ್ಷರ[FORMAT_ಬೈಟುಗಳು_MAX]){}, FORMAT_ಬೈಟುಗಳು_MAX, t, FORMAT_ಬೈಟುಗಳು_USE_IEC|FORMAT_ಬೈಟುಗಳು_ALWAYS_ಬಿಂದು|FORMAT_ಬೈಟುಗಳು_TRAILING_B)

#ವ್ಯಾಖ್ಯಾನಿಸು FORMAT_ಬೈಟುಗಳು_CGROUP_PROTECTION(t) (t == CGROUP_ಎಲ್ಲೆ_MAX ? "infinity" : FORMAT_ಬೈಟುಗಳು(t))
