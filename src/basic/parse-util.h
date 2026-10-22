/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#अन्तर्भाव "forward.h"

प्रकारनिर्देश unsigned long loadavg_t;

पूर्णाङ्क parse_boolean(ध्रुव वर्ण *v) _pure_;
पूर्णाङ्क parse_tristate_full(ध्रुव वर्ण *v, ध्रुव वर्ण *third, पूर्णाङ्क *ret);
स्थिर अन्तःस्थ पूर्णाङ्क parse_tristate(ध्रुव वर्ण *v, पूर्णाङ्क *ret) {
        प्रत्यागम parse_tristate_full(v, रिक्त, ret);
}
पूर्णाङ्क parse_pid(ध्रुव वर्ण *s, pid_t *ret);
पूर्णाङ्क parse_mode(ध्रुव वर्ण *s, mode_t *ret);
पूर्णाङ्क parse_ifindex(ध्रुव वर्ण *s);
पूर्णाङ्क parse_mtu(पूर्णाङ्क family, ध्रुव वर्ण *s, uint32_t *ret);

पूर्णाङ्क parse_size(ध्रुव वर्ण *t, uint64_t base, uint64_t *size);
पूर्णाङ्क parse_sector_size(ध्रुव वर्ण *t, uint64_t *ret);
पूर्णाङ्क parse_range(ध्रुव वर्ण *t, unsigned *lower, unsigned *upper);
पूर्णाङ्क parse_errno(ध्रुव वर्ण *t);
पूर्णाङ्क parse_fd(ध्रुव वर्ण *t);
पूर्णाङ्क parse_user_shell(ध्रुव वर्ण *s, वर्ण **ret_sh, bool *ret_copy);

पूर्णाङ्क parse_capability_set(ध्रुव वर्ण *s, uint64_t initial, uint64_t *current);

#define SAFE_ATO_REFUSE_PLUS_MINUS (1U << 30)
#define SAFE_ATO_REFUSE_LEADING_ZERO (1U << 29)
#define SAFE_ATO_REFUSE_LEADING_WHITESPACE (1U << 28)
#define SAFE_ATO_ALL_FLAGS (SAFE_ATO_REFUSE_PLUS_MINUS|SAFE_ATO_REFUSE_LEADING_ZERO|SAFE_ATO_REFUSE_LEADING_WHITESPACE)
#define SAFE_ATO_MASK_FLAGS(base) ((base) & ~SAFE_ATO_ALL_FLAGS)

पूर्णाङ्क safe_atou_full(ध्रुव वर्ण *s, unsigned base, unsigned *ret_u);
स्थिर अन्तःस्थ पूर्णाङ्क safe_atou(ध्रुव वर्ण *s, unsigned *ret_u) {
        प्रत्यागम safe_atou_full(s, 0, ret_u);
}

पूर्णाङ्क safe_atou_bounded(ध्रुव वर्ण *s, unsigned min, unsigned max, unsigned *ret);

पूर्णाङ्क safe_atoi(ध्रुव वर्ण *s, पूर्णाङ्क *ret_i);
पूर्णाङ्क safe_atolli(ध्रुव वर्ण *s, long long *ret_lli);

पूर्णाङ्क safe_atou8_full(ध्रुव वर्ण *s, unsigned base, uint8_t *ret);

स्थिर अन्तःस्थ पूर्णाङ्क safe_atou8(ध्रुव वर्ण *s, uint8_t *ret) {
        प्रत्यागम safe_atou8_full(s, 0, ret);
}

पूर्णाङ्क safe_atou16_full(ध्रुव वर्ण *s, unsigned base, uint16_t *ret);

स्थिर अन्तःस्थ पूर्णाङ्क safe_atou16(ध्रुव वर्ण *s, uint16_t *ret) {
        प्रत्यागम safe_atou16_full(s, 0, ret);
}

स्थिर अन्तःस्थ पूर्णाङ्क safe_atoux16(ध्रुव वर्ण *s, uint16_t *ret) {
        प्रत्यागम safe_atou16_full(s, 16, ret);
}

पूर्णाङ्क safe_atoi16(ध्रुव वर्ण *s, int16_t *ret);

स्थिर अन्तःस्थ पूर्णाङ्क safe_atou32_full(ध्रुव वर्ण *s, unsigned base, uint32_t *ret_u) {
        assert_cc(परिमाण(uint32_t) == परिमाण(unsigned));
        प्रत्यागम safe_atou_full(s, base, (unsigned*) ret_u);
}

स्थिर अन्तःस्थ पूर्णाङ्क safe_atou32(ध्रुव वर्ण *s, uint32_t *ret_u) {
        प्रत्यागम safe_atou32_full(s, 0, (unsigned*) ret_u);
}

स्थिर अन्तःस्थ पूर्णाङ्क safe_atoi32(ध्रुव वर्ण *s, int32_t *ret_i) {
        assert_cc(परिमाण(int32_t) == परिमाण(पूर्णाङ्क));
        प्रत्यागम safe_atoi(s, (पूर्णाङ्क*) ret_i);
}

पूर्णाङ्क safe_atollu_full(ध्रुव वर्ण *s, unsigned base, unsigned long long *ret_llu);

स्थिर अन्तःस्थ पूर्णाङ्क safe_atollu(ध्रुव वर्ण *s, unsigned long long *ret_llu) {
        प्रत्यागम safe_atollu_full(s, 0, ret_llu);
}

स्थिर अन्तःस्थ पूर्णाङ्क safe_atou64_full(ध्रुव वर्ण *s, unsigned base, uint64_t *ret_u) {
        assert_cc(परिमाण(uint64_t) == परिमाण(unsigned long long));
        प्रत्यागम safe_atollu_full(s, base, (unsigned long long*) ret_u);
}

स्थिर अन्तःस्थ पूर्णाङ्क safe_atou64(ध्रुव वर्ण *s, uint64_t *ret_u) {
        प्रत्यागम safe_atou64_full(s, 0, ret_u);
}

स्थिर अन्तःस्थ पूर्णाङ्क safe_atoi64(ध्रुव वर्ण *s, int64_t *ret_i) {
        assert_cc(परिमाण(int64_t) == परिमाण(long long));
        प्रत्यागम safe_atolli(s, (long long*) ret_i);
}

स्थिर अन्तःस्थ पूर्णाङ्क safe_atoux64(ध्रुव वर्ण *s, uint64_t *ret) {
        assert_cc(परिमाण(int64_t) == परिमाण(unsigned long long));
        प्रत्यागम safe_atollu_full(s, 16, (unsigned long long*) ret);
}

#जइ LONG_MAX == INT_MAX
स्थिर अन्तःस्थ पूर्णाङ्क safe_atolu_full(ध्रुव वर्ण *s, unsigned base, unsigned long *ret_u) {
        assert_cc(परिमाण(unsigned long) == परिमाण(unsigned));
        प्रत्यागम safe_atou_full(s, base, (unsigned*) ret_u);
}
स्थिर अन्तःस्थ पूर्णाङ्क safe_atoli(ध्रुव वर्ण *s, long *ret_u) {
        assert_cc(परिमाण(long) == परिमाण(पूर्णाङ्क));
        प्रत्यागम safe_atoi(s, (पूर्णाङ्क*) ret_u);
}
#अण्णहा
स्थिर अन्तःस्थ पूर्णाङ्क safe_atolu_full(ध्रुव वर्ण *s, unsigned base, unsigned long *ret_u) {
        assert_cc(परिमाण(unsigned long) == परिमाण(unsigned long long));
        प्रत्यागम safe_atollu_full(s, base, (unsigned long long*) ret_u);
}
स्थिर अन्तःस्थ पूर्णाङ्क safe_atoli(ध्रुव वर्ण *s, long *ret_u) {
        assert_cc(परिमाण(long) == परिमाण(long long));
        प्रत्यागम safe_atolli(s, (long long*) ret_u);
}
#endif

स्थिर अन्तःस्थ पूर्णाङ्क safe_atolu(ध्रुव वर्ण *s, unsigned long *ret_u) {
        प्रत्यागम safe_atolu_full(s, 0, ret_u);
}

#जइ SIZE_MAX == UINT_MAX
स्थिर अन्तःस्थ पूर्णाङ्क safe_atozu(ध्रुव वर्ण *s, size_t *ret_u) {
        assert_cc(परिमाण(size_t) == परिमाण(unsigned));
        प्रत्यागम safe_atou(s, (unsigned *) ret_u);
}
#अण्णहा
स्थिर अन्तःस्थ पूर्णाङ्क safe_atozu(ध्रुव वर्ण *s, size_t *ret_u) {
        assert_cc(परिमाण(size_t) == परिमाण(unsigned long));
        प्रत्यागम safe_atolu(s, ret_u);
}
#endif

पूर्णाङ्क safe_atod(ध्रुव वर्ण *s, द्विगुण *ret_d);

पूर्णाङ्क parse_fractional_part_u(ध्रुव वर्ण **p, size_t digits, unsigned *res);

पूर्णाङ्क parse_nice(ध्रुव वर्ण *s, पूर्णाङ्क *ret);

पूर्णाङ्क parse_ip_port(ध्रुव वर्ण *s, uint16_t *ret);
पूर्णाङ्क parse_ip_port_range(ध्रुव वर्ण *s, uint16_t *low, uint16_t *high, bool allow_zero);

पूर्णाङ्क parse_oom_score_adjust(ध्रुव वर्ण *s, पूर्णाङ्क *ret);

/* Implement floating point using fixed integers, to improve performance when
 * calculating load averages. These macros can be used to extract the integer
 * and decimal parts of a value. */
#define LOADAVG_PRECISION_BITS  11
#define LOADAVG_FIXED_POINT_1_0 (1 << LOADAVG_PRECISION_BITS)
#define LOADAVG_INT_SIDE(x)     ((x) >> LOADAVG_PRECISION_BITS)
#define LOADAVG_DECIMAL_SIDE(x) LOADAVG_INT_SIDE(((x) & (LOADAVG_FIXED_POINT_1_0 - 1)) * 100)

/* Given a Linux load average (e.g. decimal number 34.89 where 34 is passed as i and 89 is passed as f), convert it
 * to a loadavg_t. */
पूर्णाङ्क store_loadavg_fixed_point(unsigned long i, unsigned long f, loadavg_t *ret);
पूर्णाङ्क parse_loadavg_fixed_point(ध्रुव वर्ण *s, loadavg_t *ret);

bool nft_identifier_valid(ध्रुव वर्ण *id);
