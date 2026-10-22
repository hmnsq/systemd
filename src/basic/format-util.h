/* SPDX-License-Identifier: LGPL-2.1-or-later */
#సూచన once

#అన్తర్భావ "forward.h"
#అన్తర్భావ "cgroup-util.h"
#అన్తర్భావ "stdio-util.h"         /* IWYU pragma: keep */

assert_cc(పరిమాణం(pid_t) == పరిమాణం(int32_t));
#నిర్వచించు PID_PRI PRIi32
#నిర్వచించు PID_FMT "%" PID_PRI

assert_cc(పరిమాణం(uid_t) == పరిమాణం(uint32_t));
#నిర్వచించు UID_FMT "%" PRIu32

assert_cc(పరిమాణం(gid_t) == పరిమాణం(uint32_t));
#నిర్వచించు GID_FMT "%" PRIu32

/* Note: the lifetime of the compound literal is the immediately surrounding block,
 * see C11 §6.5.2.5, and
 * https://stackoverflow.com/questions/34880638/compound-literal-lifetime-and-if-blocks */
#నిర్వచించు FORMAT_UID(uid) \
        snprintf_ok((అక్షరం[DECIMAL_STR_MAX(uid_t)]){}, DECIMAL_STR_MAX(uid_t), UID_FMT, uid)
#నిర్వచించు FORMAT_GID(gid) \
        snprintf_ok((అక్షరం[DECIMAL_STR_MAX(gid_t)]){}, DECIMAL_STR_MAX(gid_t), GID_FMT, gid)

#ఒకవేళ SIZEOF_సమయం_T == 8
#  define PRI_సమయం PRIi64
#లేదావేళ SIZEOF_సమయం_T == 4
#  define PRI_సమయం "li"
#లేకపోతే
#  error Unknown time_t కొలత
#అయితే_ముగింపు

#ఒకవేళ SIZEOF_TIMEX_MEMBER == 8
#  define PRI_TIMEX PRIi64
#లేదావేళ SIZEOF_TIMEX_MEMBER == 4
#  define PRI_TIMEX "li"
#లేకపోతే
#  error Unknown timex member కొలత
#అయితే_ముగింపు

#నిర్వచితమైతే __GLIBC__
#  ఒకవేళ SIZEOF_RLIM_T == 8
#    define RLIM_FMT "%" PRIu64
#  elif SIZEOF_RLIM_T == 4
#    define RLIM_FMT "%" PRIu32
#  లేకపోతే
#    error Unknown rlim_t కొలత
#  endif
#లేకపోతే
/* Assume musl, and it unconditionally uses unsigned long long. */
assert_cc(SIZEOF_RLIM_T == 8);
#  define RLIM_FMT "%llu"
#అయితే_ముగింపు

#ఒకవేళ SIZEOF_DEV_T == 8
#  define DEV_FMT "%" PRIu64
#లేదావేళ SIZEOF_DEV_T == 4
#  define DEV_FMT "%" PRIu32
#లేకపోతే
#  error Unknown dev_t కొలత
#అయితే_ముగింపు

#ఒకవేళ SIZEOF_INO_T == 8
#  define INO_FMT "%" PRIu64
#లేదావేళ SIZEOF_INO_T == 4
#  define INO_FMT "%" PRIu32
#లేకపోతే
#  error Unknown ino_t కొలత
#అయితే_ముగింపు

రకనిర్వచనం గణన {
        FORMAT_బైట్లు_USE_IEC      = 1 << 0, /* use base 1024 rather than 1000 */
        FORMAT_బైట్లు_క్రింద_బిందువు  = 1 << 1, /* show one digit after the point, if non-zero */
        FORMAT_బైట్లు_ALWAYS_బిందువు = 1 << 2, /* show one digit after the point, always */
        FORMAT_బైట్లు_TRAILING_B   = 1 << 3, /* suffix the expression with a "B" for "bytes" */
} Format_బైట్లు_Flag;

#నిర్వచించు FORMAT_బైట్లు_MAX 16U

అక్షరం* format_బైట్లు_నిండిన(అక్షరం *buf, size_t l, uint64_t t, Format_బైట్లు_Flag flag) warn_వాడని_ఫలితం;

warn_వాడని_ఫలితం
స్థిర అంతఃవరుస అక్షరం* format_బైట్లు(అక్షరం *buf, size_t l, uint64_t t) {
        తిరిగి format_బైట్లు_నిండిన(buf, l, t, FORMAT_బైట్లు_USE_IEC | FORMAT_బైట్లు_క్రింద_బిందువు | FORMAT_బైట్లు_TRAILING_B);
}

/* Note: the lifetime of the compound literal is the immediately surrounding block,
 * see C11 §6.5.2.5, and
 * https://stackoverflow.com/questions/34880638/compound-literal-lifetime-and-if-blocks */
#నిర్వచించు FORMAT_బైట్లు(t) format_బైట్లు((అక్షరం[FORMAT_బైట్లు_MAX]){}, FORMAT_బైట్లు_MAX, t)
#నిర్వచించు FORMAT_బైట్లు_నిండిన(t, flags) format_బైట్లు_నిండిన((అక్షరం[FORMAT_బైట్లు_MAX]){}, FORMAT_బైట్లు_MAX, t, flags)
#నిర్వచించు FORMAT_బైట్లు_WITH_బిందువు(t) format_బైట్లు_నిండిన((అక్షరం[FORMAT_బైట్లు_MAX]){}, FORMAT_బైట్లు_MAX, t, FORMAT_బైట్లు_USE_IEC|FORMAT_బైట్లు_ALWAYS_బిందువు|FORMAT_బైట్లు_TRAILING_B)

#నిర్వచించు FORMAT_బైట్లు_CGROUP_PROTECTION(t) (t == CGROUP_హద్దు_MAX ? "infinity" : FORMAT_బైట్లు(t))
