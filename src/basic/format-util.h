/* SPDX-License-Identifier: LGPL-2.1-or-later */
#निर्देशः once

#अन्तर्भाव "forward.h"
#अन्तर्भाव "cgroup-util.h"
#अन्तर्भाव "stdio-util.h"         /* IWYU pragma: keep */

assert_cc(परिमाण(pid_t) == परिमाण(int32_t));
#परिभाषय PID_PRI PRIi32
#परिभाषय PID_FMT "%" PID_PRI

assert_cc(परिमाण(uid_t) == परिमाण(uint32_t));
#परिभाषय UID_FMT "%" PRIu32

assert_cc(परिमाण(gid_t) == परिमाण(uint32_t));
#परिभाषय GID_FMT "%" PRIu32

/* Note: the lifetime of the compound literal is the immediately surrounding block,
 * see C11 §6.5.2.5, and
 * https://stackoverflow.com/questions/34880638/compound-literal-lifetime-and-if-blocks */
#परिभाषय FORMAT_UID(uid) \
        snprintf_ok((वर्ण[DECIMAL_STR_MAX(uid_t)]){}, DECIMAL_STR_MAX(uid_t), UID_FMT, uid)
#परिभाषय FORMAT_GID(gid) \
        snprintf_ok((वर्ण[DECIMAL_STR_MAX(gid_t)]){}, DECIMAL_STR_MAX(gid_t), GID_FMT, gid)

#यदि SIZEOF_TIME_T == 8
#  define PRI_TIME PRIi64
#नोचेद्यदि SIZEOF_TIME_T == 4
#  define PRI_TIME "li"
#अन्यथा
#  error Unknown time_t आकार
#यदि_अन्तः

#यदि SIZEOF_TIMEX_MEMBER == 8
#  define PRI_TIMEX PRIi64
#नोचेद्यदि SIZEOF_TIMEX_MEMBER == 4
#  define PRI_TIMEX "li"
#अन्यथा
#  error Unknown timex member आकार
#यदि_अन्तः

#यदि_परिभाषितम् __GLIBC__
#  यदि SIZEOF_RLIM_T == 8
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
#यदि_अन्तः

#यदि SIZEOF_DEV_T == 8
#  define DEV_FMT "%" PRIu64
#नोचेद्यदि SIZEOF_DEV_T == 4
#  define DEV_FMT "%" PRIu32
#अन्यथा
#  error Unknown dev_t आकार
#यदि_अन्तः

#यदि SIZEOF_INO_T == 8
#  define INO_FMT "%" PRIu64
#नोचेद्यदि SIZEOF_INO_T == 4
#  define INO_FMT "%" PRIu32
#अन्यथा
#  error Unknown ino_t आकार
#यदि_अन्तः

प्रकारनिर्देश गणना {
        FORMAT_अष्टकानि_USE_IEC      = 1 << 0, /* use base 1024 rather than 1000 */
        FORMAT_अष्टकानि_अधः_POINT  = 1 << 1, /* show one digit after the point, if non-zero */
        FORMAT_अष्टकानि_ALWAYS_POINT = 1 << 2, /* show one digit after the point, always */
        FORMAT_अष्टकानि_TRAILING_B   = 1 << 3, /* suffix the expression with a "B" for "bytes" */
} Format_अष्टकानि_Flag;

#परिभाषय FORMAT_अष्टकानि_MAX 16U

वर्ण* format_अष्टकानि_पूर्णम्(वर्ण *buf, size_t l, uint64_t t, Format_अष्टकानि_Flag flag) warn_अप्रयुक्तम्_परिणाम;

warn_अप्रयुक्तम्_परिणाम
स्थिर अन्तःस्थ वर्ण* format_अष्टकानि(वर्ण *buf, size_t l, uint64_t t) {
        प्रत्यागम format_अष्टकानि_पूर्णम्(buf, l, t, FORMAT_अष्टकानि_USE_IEC | FORMAT_अष्टकानि_अधः_POINT | FORMAT_अष्टकानि_TRAILING_B);
}

/* Note: the lifetime of the compound literal is the immediately surrounding block,
 * see C11 §6.5.2.5, and
 * https://stackoverflow.com/questions/34880638/compound-literal-lifetime-and-if-blocks */
#परिभाषय FORMAT_अष्टकानि(t) format_अष्टकानि((वर्ण[FORMAT_अष्टकानि_MAX]){}, FORMAT_अष्टकानि_MAX, t)
#परिभाषय FORMAT_अष्टकानि_पूर्णम्(t, flags) format_अष्टकानि_पूर्णम्((वर्ण[FORMAT_अष्टकानि_MAX]){}, FORMAT_अष्टकानि_MAX, t, flags)
#परिभाषय FORMAT_अष्टकानि_WITH_POINT(t) format_अष्टकानि_पूर्णम्((वर्ण[FORMAT_अष्टकानि_MAX]){}, FORMAT_अष्टकानि_MAX, t, FORMAT_अष्टकानि_USE_IEC|FORMAT_अष्टकानि_ALWAYS_POINT|FORMAT_अष्टकानि_TRAILING_B)

#परिभाषय FORMAT_अष्टकानि_CGROUP_PROTECTION(t) (t == CGROUP_सीमा_MAX ? "infinity" : FORMAT_अष्टकानि(t))
