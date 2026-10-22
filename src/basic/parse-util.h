/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#அந்தர்பாவ "forward.h"

வகைவரை unsigned long loadavg_t;

முழு parse_boolean(மாறிலி எழுத்து *v) _pure_;
முழு parse_tristate_full(மாறிலி எழுத்து *v, மாறிலி எழுத்து *third, முழு *ret);
நிலையான உள்வரி முழு parse_tristate(மாறிலி எழுத்து *v, முழு *ret) {
        திரும்பு parse_tristate_full(v, வெற்று, ret);
}
முழு parse_pid(மாறிலி எழுத்து *s, pid_t *ret);
முழு parse_mode(மாறிலி எழுத்து *s, mode_t *ret);
முழு parse_ifindex(மாறிலி எழுத்து *s);
முழு parse_mtu(முழு family, மாறிலி எழுத்து *s, uint32_t *ret);

முழு parse_size(மாறிலி எழுத்து *t, uint64_t base, uint64_t *size);
முழு parse_sector_size(மாறிலி எழுத்து *t, uint64_t *ret);
முழு parse_range(மாறிலி எழுத்து *t, unsigned *lower, unsigned *upper);
முழு parse_errno(மாறிலி எழுத்து *t);
முழு parse_fd(மாறிலி எழுத்து *t);
முழு parse_user_shell(மாறிலி எழுத்து *s, எழுத்து **ret_sh, bool *ret_copy);

முழு parse_capability_set(மாறிலி எழுத்து *s, uint64_t initial, uint64_t *current);

#define SAFE_ATO_REFUSE_PLUS_MINUS (1U << 30)
#define SAFE_ATO_REFUSE_LEADING_ZERO (1U << 29)
#define SAFE_ATO_REFUSE_LEADING_WHITESPACE (1U << 28)
#define SAFE_ATO_ALL_FLAGS (SAFE_ATO_REFUSE_PLUS_MINUS|SAFE_ATO_REFUSE_LEADING_ZERO|SAFE_ATO_REFUSE_LEADING_WHITESPACE)
#define SAFE_ATO_MASK_FLAGS(base) ((base) & ~SAFE_ATO_ALL_FLAGS)

முழு safe_atou_full(மாறிலி எழுத்து *s, unsigned base, unsigned *ret_u);
நிலையான உள்வரி முழு safe_atou(மாறிலி எழுத்து *s, unsigned *ret_u) {
        திரும்பு safe_atou_full(s, 0, ret_u);
}

முழு safe_atou_bounded(மாறிலி எழுத்து *s, unsigned min, unsigned max, unsigned *ret);

முழு safe_atoi(மாறிலி எழுத்து *s, முழு *ret_i);
முழு safe_atolli(மாறிலி எழுத்து *s, long long *ret_lli);

முழு safe_atou8_full(மாறிலி எழுத்து *s, unsigned base, uint8_t *ret);

நிலையான உள்வரி முழு safe_atou8(மாறிலி எழுத்து *s, uint8_t *ret) {
        திரும்பு safe_atou8_full(s, 0, ret);
}

முழு safe_atou16_full(மாறிலி எழுத்து *s, unsigned base, uint16_t *ret);

நிலையான உள்வரி முழு safe_atou16(மாறிலி எழுத்து *s, uint16_t *ret) {
        திரும்பு safe_atou16_full(s, 0, ret);
}

நிலையான உள்வரி முழு safe_atoux16(மாறிலி எழுத்து *s, uint16_t *ret) {
        திரும்பு safe_atou16_full(s, 16, ret);
}

முழு safe_atoi16(மாறிலி எழுத்து *s, int16_t *ret);

நிலையான உள்வரி முழு safe_atou32_full(மாறிலி எழுத்து *s, unsigned base, uint32_t *ret_u) {
        assert_cc(அளவு(uint32_t) == அளவு(unsigned));
        திரும்பு safe_atou_full(s, base, (unsigned*) ret_u);
}

நிலையான உள்வரி முழு safe_atou32(மாறிலி எழுத்து *s, uint32_t *ret_u) {
        திரும்பு safe_atou32_full(s, 0, (unsigned*) ret_u);
}

நிலையான உள்வரி முழு safe_atoi32(மாறிலி எழுத்து *s, int32_t *ret_i) {
        assert_cc(அளவு(int32_t) == அளவு(முழு));
        திரும்பு safe_atoi(s, (முழு*) ret_i);
}

முழு safe_atollu_full(மாறிலி எழுத்து *s, unsigned base, unsigned long long *ret_llu);

நிலையான உள்வரி முழு safe_atollu(மாறிலி எழுத்து *s, unsigned long long *ret_llu) {
        திரும்பு safe_atollu_full(s, 0, ret_llu);
}

நிலையான உள்வரி முழு safe_atou64_full(மாறிலி எழுத்து *s, unsigned base, uint64_t *ret_u) {
        assert_cc(அளவு(uint64_t) == அளவு(unsigned long long));
        திரும்பு safe_atollu_full(s, base, (unsigned long long*) ret_u);
}

நிலையான உள்வரி முழு safe_atou64(மாறிலி எழுத்து *s, uint64_t *ret_u) {
        திரும்பு safe_atou64_full(s, 0, ret_u);
}

நிலையான உள்வரி முழு safe_atoi64(மாறிலி எழுத்து *s, int64_t *ret_i) {
        assert_cc(அளவு(int64_t) == அளவு(long long));
        திரும்பு safe_atolli(s, (long long*) ret_i);
}

நிலையான உள்வரி முழு safe_atoux64(மாறிலி எழுத்து *s, uint64_t *ret) {
        assert_cc(அளவு(int64_t) == அளவு(unsigned long long));
        திரும்பு safe_atollu_full(s, 16, (unsigned long long*) ret);
}

#எனில் LONG_MAX == INT_MAX
நிலையான உள்வரி முழு safe_atolu_full(மாறிலி எழுத்து *s, unsigned base, unsigned long *ret_u) {
        assert_cc(அளவு(unsigned long) == அளவு(unsigned));
        திரும்பு safe_atou_full(s, base, (unsigned*) ret_u);
}
நிலையான உள்வரி முழு safe_atoli(மாறிலி எழுத்து *s, long *ret_u) {
        assert_cc(அளவு(long) == அளவு(முழு));
        திரும்பு safe_atoi(s, (முழு*) ret_u);
}
#இல்லையெனில்
நிலையான உள்வரி முழு safe_atolu_full(மாறிலி எழுத்து *s, unsigned base, unsigned long *ret_u) {
        assert_cc(அளவு(unsigned long) == அளவு(unsigned long long));
        திரும்பு safe_atollu_full(s, base, (unsigned long long*) ret_u);
}
நிலையான உள்வரி முழு safe_atoli(மாறிலி எழுத்து *s, long *ret_u) {
        assert_cc(அளவு(long) == அளவு(long long));
        திரும்பு safe_atolli(s, (long long*) ret_u);
}
#endif

நிலையான உள்வரி முழு safe_atolu(மாறிலி எழுத்து *s, unsigned long *ret_u) {
        திரும்பு safe_atolu_full(s, 0, ret_u);
}

#எனில் SIZE_MAX == UINT_MAX
நிலையான உள்வரி முழு safe_atozu(மாறிலி எழுத்து *s, size_t *ret_u) {
        assert_cc(அளவு(size_t) == அளவு(unsigned));
        திரும்பு safe_atou(s, (unsigned *) ret_u);
}
#இல்லையெனில்
நிலையான உள்வரி முழு safe_atozu(மாறிலி எழுத்து *s, size_t *ret_u) {
        assert_cc(அளவு(size_t) == அளவு(unsigned long));
        திரும்பு safe_atolu(s, ret_u);
}
#endif

முழு safe_atod(மாறிலி எழுத்து *s, இரட்டை *ret_d);

முழு parse_fractional_part_u(மாறிலி எழுத்து **p, size_t digits, unsigned *res);

முழு parse_nice(மாறிலி எழுத்து *s, முழு *ret);

முழு parse_ip_port(மாறிலி எழுத்து *s, uint16_t *ret);
முழு parse_ip_port_range(மாறிலி எழுத்து *s, uint16_t *low, uint16_t *high, bool allow_zero);

முழு parse_oom_score_adjust(மாறிலி எழுத்து *s, முழு *ret);

/* Implement floating point using fixed integers, to improve performance when
 * calculating load averages. These macros can be used to extract the integer
 * and decimal parts of a value. */
#define LOADAVG_PRECISION_BITS  11
#define LOADAVG_FIXED_POINT_1_0 (1 << LOADAVG_PRECISION_BITS)
#define LOADAVG_INT_SIDE(x)     ((x) >> LOADAVG_PRECISION_BITS)
#define LOADAVG_DECIMAL_SIDE(x) LOADAVG_INT_SIDE(((x) & (LOADAVG_FIXED_POINT_1_0 - 1)) * 100)

/* Given a Linux load average (e.g. decimal number 34.89 where 34 is passed as i and 89 is passed as f), convert it
 * to a loadavg_t. */
முழு store_loadavg_fixed_point(unsigned long i, unsigned long f, loadavg_t *ret);
முழு parse_loadavg_fixed_point(மாறிலி எழுத்து *s, loadavg_t *ret);

bool nft_identifier_valid(மாறிலி எழுத்து *id);
