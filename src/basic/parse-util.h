/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#شامل "forward.h"

تعریفنوع unsigned long loadavg_t;

صحیح parse_boolean(ثابت نویسه *v) _pure_;
صحیح parse_tristate_full(ثابت نویسه *v, ثابت نویسه *third, صحیح *ret);
ایستا درونخطی صحیح parse_tristate(ثابت نویسه *v, صحیح *ret) {
        بازگشت parse_tristate_full(v, پوچ, ret);
}
صحیح parse_pid(ثابت نویسه *s, pid_t *ret);
صحیح parse_mode(ثابت نویسه *s, mode_t *ret);
صحیح parse_ifindex(ثابت نویسه *s);
صحیح parse_mtu(صحیح family, ثابت نویسه *s, uint32_t *ret);

صحیح parse_size(ثابت نویسه *t, uint64_t base, uint64_t *size);
صحیح parse_sector_size(ثابت نویسه *t, uint64_t *ret);
صحیح parse_range(ثابت نویسه *t, unsigned *lower, unsigned *upper);
صحیح parse_errno(ثابت نویسه *t);
صحیح parse_fd(ثابت نویسه *t);
صحیح parse_user_shell(ثابت نویسه *s, نویسه **ret_sh, bool *ret_copy);

صحیح parse_capability_set(ثابت نویسه *s, uint64_t initial, uint64_t *current);

#define SAFE_ATO_REFUSE_PLUS_MINUS (1U << 30)
#define SAFE_ATO_REFUSE_LEADING_ZERO (1U << 29)
#define SAFE_ATO_REFUSE_LEADING_WHITESPACE (1U << 28)
#define SAFE_ATO_ALL_FLAGS (SAFE_ATO_REFUSE_PLUS_MINUS|SAFE_ATO_REFUSE_LEADING_ZERO|SAFE_ATO_REFUSE_LEADING_WHITESPACE)
#define SAFE_ATO_MASK_FLAGS(base) ((base) & ~SAFE_ATO_ALL_FLAGS)

صحیح safe_atou_full(ثابت نویسه *s, unsigned base, unsigned *ret_u);
ایستا درونخطی صحیح safe_atou(ثابت نویسه *s, unsigned *ret_u) {
        بازگشت safe_atou_full(s, 0, ret_u);
}

صحیح safe_atou_bounded(ثابت نویسه *s, unsigned min, unsigned max, unsigned *ret);

صحیح safe_atoi(ثابت نویسه *s, صحیح *ret_i);
صحیح safe_atolli(ثابت نویسه *s, long long *ret_lli);

صحیح safe_atou8_full(ثابت نویسه *s, unsigned base, uint8_t *ret);

ایستا درونخطی صحیح safe_atou8(ثابت نویسه *s, uint8_t *ret) {
        بازگشت safe_atou8_full(s, 0, ret);
}

صحیح safe_atou16_full(ثابت نویسه *s, unsigned base, uint16_t *ret);

ایستا درونخطی صحیح safe_atou16(ثابت نویسه *s, uint16_t *ret) {
        بازگشت safe_atou16_full(s, 0, ret);
}

ایستا درونخطی صحیح safe_atoux16(ثابت نویسه *s, uint16_t *ret) {
        بازگشت safe_atou16_full(s, 16, ret);
}

صحیح safe_atoi16(ثابت نویسه *s, int16_t *ret);

ایستا درونخطی صحیح safe_atou32_full(ثابت نویسه *s, unsigned base, uint32_t *ret_u) {
        assert_cc(اندازه(uint32_t) == اندازه(unsigned));
        بازگشت safe_atou_full(s, base, (unsigned*) ret_u);
}

ایستا درونخطی صحیح safe_atou32(ثابت نویسه *s, uint32_t *ret_u) {
        بازگشت safe_atou32_full(s, 0, (unsigned*) ret_u);
}

ایستا درونخطی صحیح safe_atoi32(ثابت نویسه *s, int32_t *ret_i) {
        assert_cc(اندازه(int32_t) == اندازه(صحیح));
        بازگشت safe_atoi(s, (صحیح*) ret_i);
}

صحیح safe_atollu_full(ثابت نویسه *s, unsigned base, unsigned long long *ret_llu);

ایستا درونخطی صحیح safe_atollu(ثابت نویسه *s, unsigned long long *ret_llu) {
        بازگشت safe_atollu_full(s, 0, ret_llu);
}

ایستا درونخطی صحیح safe_atou64_full(ثابت نویسه *s, unsigned base, uint64_t *ret_u) {
        assert_cc(اندازه(uint64_t) == اندازه(unsigned long long));
        بازگشت safe_atollu_full(s, base, (unsigned long long*) ret_u);
}

ایستا درونخطی صحیح safe_atou64(ثابت نویسه *s, uint64_t *ret_u) {
        بازگشت safe_atou64_full(s, 0, ret_u);
}

ایستا درونخطی صحیح safe_atoi64(ثابت نویسه *s, int64_t *ret_i) {
        assert_cc(اندازه(int64_t) == اندازه(long long));
        بازگشت safe_atolli(s, (long long*) ret_i);
}

ایستا درونخطی صحیح safe_atoux64(ثابت نویسه *s, uint64_t *ret) {
        assert_cc(اندازه(int64_t) == اندازه(unsigned long long));
        بازگشت safe_atollu_full(s, 16, (unsigned long long*) ret);
}

#اگر LONG_MAX == INT_MAX
ایستا درونخطی صحیح safe_atolu_full(ثابت نویسه *s, unsigned base, unsigned long *ret_u) {
        assert_cc(اندازه(unsigned long) == اندازه(unsigned));
        بازگشت safe_atou_full(s, base, (unsigned*) ret_u);
}
ایستا درونخطی صحیح safe_atoli(ثابت نویسه *s, long *ret_u) {
        assert_cc(اندازه(long) == اندازه(صحیح));
        بازگشت safe_atoi(s, (صحیح*) ret_u);
}
#وگرنه
ایستا درونخطی صحیح safe_atolu_full(ثابت نویسه *s, unsigned base, unsigned long *ret_u) {
        assert_cc(اندازه(unsigned long) == اندازه(unsigned long long));
        بازگشت safe_atollu_full(s, base, (unsigned long long*) ret_u);
}
ایستا درونخطی صحیح safe_atoli(ثابت نویسه *s, long *ret_u) {
        assert_cc(اندازه(long) == اندازه(long long));
        بازگشت safe_atolli(s, (long long*) ret_u);
}
#endif

ایستا درونخطی صحیح safe_atolu(ثابت نویسه *s, unsigned long *ret_u) {
        بازگشت safe_atolu_full(s, 0, ret_u);
}

#اگر SIZE_MAX == UINT_MAX
ایستا درونخطی صحیح safe_atozu(ثابت نویسه *s, size_t *ret_u) {
        assert_cc(اندازه(size_t) == اندازه(unsigned));
        بازگشت safe_atou(s, (unsigned *) ret_u);
}
#وگرنه
ایستا درونخطی صحیح safe_atozu(ثابت نویسه *s, size_t *ret_u) {
        assert_cc(اندازه(size_t) == اندازه(unsigned long));
        بازگشت safe_atolu(s, ret_u);
}
#endif

صحیح safe_atod(ثابت نویسه *s, دوگانه *ret_d);

صحیح parse_fractional_part_u(ثابت نویسه **p, size_t digits, unsigned *res);

صحیح parse_nice(ثابت نویسه *s, صحیح *ret);

صحیح parse_ip_port(ثابت نویسه *s, uint16_t *ret);
صحیح parse_ip_port_range(ثابت نویسه *s, uint16_t *low, uint16_t *high, bool allow_zero);

صحیح parse_oom_score_adjust(ثابت نویسه *s, صحیح *ret);

/* Implement floating point using fixed integers, to improve performance when
 * calculating load averages. These macros can be used to extract the integer
 * and decimal parts of a value. */
#define LOADAVG_PRECISION_BITS  11
#define LOADAVG_FIXED_POINT_1_0 (1 << LOADAVG_PRECISION_BITS)
#define LOADAVG_INT_SIDE(x)     ((x) >> LOADAVG_PRECISION_BITS)
#define LOADAVG_DECIMAL_SIDE(x) LOADAVG_INT_SIDE(((x) & (LOADAVG_FIXED_POINT_1_0 - 1)) * 100)

/* Given a Linux load average (e.g. decimal number 34.89 where 34 is passed as i and 89 is passed as f), convert it
 * to a loadavg_t. */
صحیح store_loadavg_fixed_point(unsigned long i, unsigned long f, loadavg_t *ret);
صحیح parse_loadavg_fixed_point(ثابت نویسه *s, loadavg_t *ret);

bool nft_identifier_valid(ثابت نویسه *id);
