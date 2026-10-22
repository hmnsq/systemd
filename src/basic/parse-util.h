/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ضمن "forward.h"

تعريفنوع unsigned long loadavg_t;

صحيح parse_boolean(ثابت حرف *v) _pure_;
صحيح parse_tristate_full(ثابت حرف *v, ثابت حرف *third, صحيح *ret);
ساكن مضمن صحيح parse_tristate(ثابت حرف *v, صحيح *ret) {
        أرجع parse_tristate_full(v, عدم, ret);
}
صحيح parse_pid(ثابت حرف *s, pid_t *ret);
صحيح parse_mode(ثابت حرف *s, mode_t *ret);
صحيح parse_ifindex(ثابت حرف *s);
صحيح parse_mtu(صحيح family, ثابت حرف *s, uint32_t *ret);

صحيح parse_size(ثابت حرف *t, uint64_t base, uint64_t *size);
صحيح parse_sector_size(ثابت حرف *t, uint64_t *ret);
صحيح parse_range(ثابت حرف *t, unsigned *lower, unsigned *upper);
صحيح parse_errno(ثابت حرف *t);
صحيح parse_fd(ثابت حرف *t);
صحيح parse_user_shell(ثابت حرف *s, حرف **ret_sh, bool *ret_copy);

صحيح parse_capability_set(ثابت حرف *s, uint64_t initial, uint64_t *current);

#define SAFE_ATO_REFUSE_PLUS_MINUS (1U << 30)
#define SAFE_ATO_REFUSE_LEADING_ZERO (1U << 29)
#define SAFE_ATO_REFUSE_LEADING_WHITESPACE (1U << 28)
#define SAFE_ATO_ALL_FLAGS (SAFE_ATO_REFUSE_PLUS_MINUS|SAFE_ATO_REFUSE_LEADING_ZERO|SAFE_ATO_REFUSE_LEADING_WHITESPACE)
#define SAFE_ATO_MASK_FLAGS(base) ((base) & ~SAFE_ATO_ALL_FLAGS)

صحيح safe_atou_full(ثابت حرف *s, unsigned base, unsigned *ret_u);
ساكن مضمن صحيح safe_atou(ثابت حرف *s, unsigned *ret_u) {
        أرجع safe_atou_full(s, 0, ret_u);
}

صحيح safe_atou_bounded(ثابت حرف *s, unsigned min, unsigned max, unsigned *ret);

صحيح safe_atoi(ثابت حرف *s, صحيح *ret_i);
صحيح safe_atolli(ثابت حرف *s, long long *ret_lli);

صحيح safe_atou8_full(ثابت حرف *s, unsigned base, uint8_t *ret);

ساكن مضمن صحيح safe_atou8(ثابت حرف *s, uint8_t *ret) {
        أرجع safe_atou8_full(s, 0, ret);
}

صحيح safe_atou16_full(ثابت حرف *s, unsigned base, uint16_t *ret);

ساكن مضمن صحيح safe_atou16(ثابت حرف *s, uint16_t *ret) {
        أرجع safe_atou16_full(s, 0, ret);
}

ساكن مضمن صحيح safe_atoux16(ثابت حرف *s, uint16_t *ret) {
        أرجع safe_atou16_full(s, 16, ret);
}

صحيح safe_atoi16(ثابت حرف *s, int16_t *ret);

ساكن مضمن صحيح safe_atou32_full(ثابت حرف *s, unsigned base, uint32_t *ret_u) {
        assert_cc(حجم(uint32_t) == حجم(unsigned));
        أرجع safe_atou_full(s, base, (unsigned*) ret_u);
}

ساكن مضمن صحيح safe_atou32(ثابت حرف *s, uint32_t *ret_u) {
        أرجع safe_atou32_full(s, 0, (unsigned*) ret_u);
}

ساكن مضمن صحيح safe_atoi32(ثابت حرف *s, int32_t *ret_i) {
        assert_cc(حجم(int32_t) == حجم(صحيح));
        أرجع safe_atoi(s, (صحيح*) ret_i);
}

صحيح safe_atollu_full(ثابت حرف *s, unsigned base, unsigned long long *ret_llu);

ساكن مضمن صحيح safe_atollu(ثابت حرف *s, unsigned long long *ret_llu) {
        أرجع safe_atollu_full(s, 0, ret_llu);
}

ساكن مضمن صحيح safe_atou64_full(ثابت حرف *s, unsigned base, uint64_t *ret_u) {
        assert_cc(حجم(uint64_t) == حجم(unsigned long long));
        أرجع safe_atollu_full(s, base, (unsigned long long*) ret_u);
}

ساكن مضمن صحيح safe_atou64(ثابت حرف *s, uint64_t *ret_u) {
        أرجع safe_atou64_full(s, 0, ret_u);
}

ساكن مضمن صحيح safe_atoi64(ثابت حرف *s, int64_t *ret_i) {
        assert_cc(حجم(int64_t) == حجم(long long));
        أرجع safe_atolli(s, (long long*) ret_i);
}

ساكن مضمن صحيح safe_atoux64(ثابت حرف *s, uint64_t *ret) {
        assert_cc(حجم(int64_t) == حجم(unsigned long long));
        أرجع safe_atollu_full(s, 16, (unsigned long long*) ret);
}

#𐡄𐡍 LONG_MAX == INT_MAX
ساكن مضمن صحيح safe_atolu_full(ثابت حرف *s, unsigned base, unsigned long *ret_u) {
        assert_cc(حجم(unsigned long) == حجم(unsigned));
        أرجع safe_atou_full(s, base, (unsigned*) ret_u);
}
ساكن مضمن صحيح safe_atoli(ثابت حرف *s, long *ret_u) {
        assert_cc(حجم(long) == حجم(صحيح));
        أرجع safe_atoi(s, (صحيح*) ret_u);
}
#وإلا
ساكن مضمن صحيح safe_atolu_full(ثابت حرف *s, unsigned base, unsigned long *ret_u) {
        assert_cc(حجم(unsigned long) == حجم(unsigned long long));
        أرجع safe_atollu_full(s, base, (unsigned long long*) ret_u);
}
ساكن مضمن صحيح safe_atoli(ثابت حرف *s, long *ret_u) {
        assert_cc(حجم(long) == حجم(long long));
        أرجع safe_atolli(s, (long long*) ret_u);
}
#endif

ساكن مضمن صحيح safe_atolu(ثابت حرف *s, unsigned long *ret_u) {
        أرجع safe_atolu_full(s, 0, ret_u);
}

#𐡄𐡍 SIZE_MAX == UINT_MAX
ساكن مضمن صحيح safe_atozu(ثابت حرف *s, size_t *ret_u) {
        assert_cc(حجم(size_t) == حجم(unsigned));
        أرجع safe_atou(s, (unsigned *) ret_u);
}
#وإلا
ساكن مضمن صحيح safe_atozu(ثابت حرف *s, size_t *ret_u) {
        assert_cc(حجم(size_t) == حجم(unsigned long));
        أرجع safe_atolu(s, ret_u);
}
#endif

صحيح safe_atod(ثابت حرف *s, مزدوج *ret_d);

صحيح parse_fractional_part_u(ثابت حرف **p, size_t digits, unsigned *res);

صحيح parse_nice(ثابت حرف *s, صحيح *ret);

صحيح parse_ip_port(ثابت حرف *s, uint16_t *ret);
صحيح parse_ip_port_range(ثابت حرف *s, uint16_t *low, uint16_t *high, bool allow_zero);

صحيح parse_oom_score_adjust(ثابت حرف *s, صحيح *ret);

/* Implement floating point using fixed integers, to improve performance when
 * calculating load averages. These macros can be used to extract the integer
 * and decimal parts of a value. */
#define LOADAVG_PRECISION_BITS  11
#define LOADAVG_FIXED_POINT_1_0 (1 << LOADAVG_PRECISION_BITS)
#define LOADAVG_INT_SIDE(x)     ((x) >> LOADAVG_PRECISION_BITS)
#define LOADAVG_DECIMAL_SIDE(x) LOADAVG_INT_SIDE(((x) & (LOADAVG_FIXED_POINT_1_0 - 1)) * 100)

/* Given a Linux load average (e.g. decimal number 34.89 where 34 is passed as i and 89 is passed as f), convert it
 * to a loadavg_t. */
صحيح store_loadavg_fixed_point(unsigned long i, unsigned long f, loadavg_t *ret);
صحيح parse_loadavg_fixed_point(ثابت حرف *s, loadavg_t *ret);

bool nft_identifier_valid(ثابت حرف *id);
