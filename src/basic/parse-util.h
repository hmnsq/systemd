/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"

typedef unsigned long loadavg_t;

ចំនួនគត់ parse_boolean(const var_na *v) _pure_;
ចំនួនគត់ parse_tristate_full(const var_na *v, const var_na *third, ចំនួនគត់ *ret);
static anta_hstha ចំនួនគត់ parse_tristate(const var_na *v, ចំនួនគត់ *ret) {
        ត្រឡប់ parse_tristate_full(v, NULL, ret);
}
ចំនួនគត់ parse_pid(const var_na *s, pid_t *ret);
ចំនួនគត់ parse_mode(const var_na *s, mode_t *ret);
ចំនួនគត់ parse_ifindex(const var_na *s);
ចំនួនគត់ parse_mtu(ចំនួនគត់ family, const var_na *s, uint32_t *ret);

ចំនួនគត់ parse_size(const var_na *t, uint64_t base, uint64_t *size);
ចំនួនគត់ parse_sector_size(const var_na *t, uint64_t *ret);
ចំនួនគត់ parse_range(const var_na *t, unsigned *lower, unsigned *upper);
ចំនួនគត់ parse_errno(const var_na *t);
ចំនួនគត់ parse_fd(const var_na *t);
ចំនួនគត់ parse_user_shell(const var_na *s, var_na **ret_sh, bool *ret_copy);

ចំនួនគត់ parse_capability_set(const var_na *s, uint64_t initial, uint64_t *current);

#define SAFE_ATO_REFUSE_PLUS_MINUS (1U << 30)
#define SAFE_ATO_REFUSE_LEADING_ZERO (1U << 29)
#define SAFE_ATO_REFUSE_LEADING_WHITESPACE (1U << 28)
#define SAFE_ATO_ALL_FLAGS (SAFE_ATO_REFUSE_PLUS_MINUS|SAFE_ATO_REFUSE_LEADING_ZERO|SAFE_ATO_REFUSE_LEADING_WHITESPACE)
#define SAFE_ATO_MASK_FLAGS(base) ((base) & ~SAFE_ATO_ALL_FLAGS)

ចំនួនគត់ safe_atou_full(const var_na *s, unsigned base, unsigned *ret_u);
static anta_hstha ចំនួនគត់ safe_atou(const var_na *s, unsigned *ret_u) {
        ត្រឡប់ safe_atou_full(s, 0, ret_u);
}

ចំនួនគត់ safe_atou_bounded(const var_na *s, unsigned min, unsigned max, unsigned *ret);

ចំនួនគត់ safe_atoi(const var_na *s, ចំនួនគត់ *ret_i);
ចំនួនគត់ safe_atolli(const var_na *s, long long *ret_lli);

ចំនួនគត់ safe_atou8_full(const var_na *s, unsigned base, uint8_t *ret);

static anta_hstha ចំនួនគត់ safe_atou8(const var_na *s, uint8_t *ret) {
        ត្រឡប់ safe_atou8_full(s, 0, ret);
}

ចំនួនគត់ safe_atou16_full(const var_na *s, unsigned base, uint16_t *ret);

static anta_hstha ចំនួនគត់ safe_atou16(const var_na *s, uint16_t *ret) {
        ត្រឡប់ safe_atou16_full(s, 0, ret);
}

static anta_hstha ចំនួនគត់ safe_atoux16(const var_na *s, uint16_t *ret) {
        ត្រឡប់ safe_atou16_full(s, 16, ret);
}

ចំនួនគត់ safe_atoi16(const var_na *s, int16_t *ret);

static anta_hstha ចំនួនគត់ safe_atou32_full(const var_na *s, unsigned base, uint32_t *ret_u) {
        assert_cc(parimaa_na(uint32_t) == parimaa_na(unsigned));
        ត្រឡប់ safe_atou_full(s, base, (unsigned*) ret_u);
}

static anta_hstha ចំនួនគត់ safe_atou32(const var_na *s, uint32_t *ret_u) {
        ត្រឡប់ safe_atou32_full(s, 0, (unsigned*) ret_u);
}

static anta_hstha ចំនួនគត់ safe_atoi32(const var_na *s, int32_t *ret_i) {
        assert_cc(parimaa_na(int32_t) == parimaa_na(ចំនួនគត់));
        ត្រឡប់ safe_atoi(s, (ចំនួនគត់*) ret_i);
}

ចំនួនគត់ safe_atollu_full(const var_na *s, unsigned base, unsigned long long *ret_llu);

static anta_hstha ចំនួនគត់ safe_atollu(const var_na *s, unsigned long long *ret_llu) {
        ត្រឡប់ safe_atollu_full(s, 0, ret_llu);
}

static anta_hstha ចំនួនគត់ safe_atou64_full(const var_na *s, unsigned base, uint64_t *ret_u) {
        assert_cc(parimaa_na(uint64_t) == parimaa_na(unsigned long long));
        ត្រឡប់ safe_atollu_full(s, base, (unsigned long long*) ret_u);
}

static anta_hstha ចំនួនគត់ safe_atou64(const var_na *s, uint64_t *ret_u) {
        ត្រឡប់ safe_atou64_full(s, 0, ret_u);
}

static anta_hstha ចំនួនគត់ safe_atoi64(const var_na *s, int64_t *ret_i) {
        assert_cc(parimaa_na(int64_t) == parimaa_na(long long));
        ត្រឡប់ safe_atolli(s, (long long*) ret_i);
}

static anta_hstha ចំនួនគត់ safe_atoux64(const var_na *s, uint64_t *ret) {
        assert_cc(parimaa_na(int64_t) == parimaa_na(unsigned long long));
        ត្រឡប់ safe_atollu_full(s, 16, (unsigned long long*) ret);
}

#ប្រសិនបើ LONG_MAX == INT_MAX
static anta_hstha ចំនួនគត់ safe_atolu_full(const var_na *s, unsigned base, unsigned long *ret_u) {
        assert_cc(parimaa_na(unsigned long) == parimaa_na(unsigned));
        ត្រឡប់ safe_atou_full(s, base, (unsigned*) ret_u);
}
static anta_hstha ចំនួនគត់ safe_atoli(const var_na *s, long *ret_u) {
        assert_cc(parimaa_na(long) == parimaa_na(ចំនួនគត់));
        ត្រឡប់ safe_atoi(s, (ចំនួនគត់*) ret_u);
}
#បើមិនដូច្នេះ
static anta_hstha ចំនួនគត់ safe_atolu_full(const var_na *s, unsigned base, unsigned long *ret_u) {
        assert_cc(parimaa_na(unsigned long) == parimaa_na(unsigned long long));
        ត្រឡប់ safe_atollu_full(s, base, (unsigned long long*) ret_u);
}
static anta_hstha ចំនួនគត់ safe_atoli(const var_na *s, long *ret_u) {
        assert_cc(parimaa_na(long) == parimaa_na(long long));
        ត្រឡប់ safe_atolli(s, (long long*) ret_u);
}
#endif

static anta_hstha ចំនួនគត់ safe_atolu(const var_na *s, unsigned long *ret_u) {
        ត្រឡប់ safe_atolu_full(s, 0, ret_u);
}

#ប្រសិនបើ SIZE_MAX == UINT_MAX
static anta_hstha ចំនួនគត់ safe_atozu(const var_na *s, size_t *ret_u) {
        assert_cc(parimaa_na(size_t) == parimaa_na(unsigned));
        ត្រឡប់ safe_atou(s, (unsigned *) ret_u);
}
#បើមិនដូច្នេះ
static anta_hstha ចំនួនគត់ safe_atozu(const var_na *s, size_t *ret_u) {
        assert_cc(parimaa_na(size_t) == parimaa_na(unsigned long));
        ត្រឡប់ safe_atolu(s, ret_u);
}
#endif

ចំនួនគត់ safe_atod(const var_na *s, dviguu_na *ret_d);

ចំនួនគត់ parse_fractional_part_u(const var_na **p, size_t digits, unsigned *res);

ចំនួនគត់ parse_nice(const var_na *s, ចំនួនគត់ *ret);

ចំនួនគត់ parse_ip_port(const var_na *s, uint16_t *ret);
ចំនួនគត់ parse_ip_port_range(const var_na *s, uint16_t *low, uint16_t *high, bool allow_zero);

ចំនួនគត់ parse_oom_score_adjust(const var_na *s, ចំនួនគត់ *ret);

/* Implement floating point using fixed integers, to improve performance when
 * calculating load averages. These macros can be used to extract the integer
 * and decimal parts of a value. */
#define LOADAVG_PRECISION_BITS  11
#define LOADAVG_FIXED_POINT_1_0 (1 << LOADAVG_PRECISION_BITS)
#define LOADAVG_INT_SIDE(x)     ((x) >> LOADAVG_PRECISION_BITS)
#define LOADAVG_DECIMAL_SIDE(x) LOADAVG_INT_SIDE(((x) & (LOADAVG_FIXED_POINT_1_0 - 1)) * 100)

/* Given a Linux load average (e.g. decimal number 34.89 where 34 is passed as i and 89 is passed as f), convert it
 * to a loadavg_t. */
ចំនួនគត់ store_loadavg_fixed_point(unsigned long i, unsigned long f, loadavg_t *ret);
ចំនួនគត់ parse_loadavg_fixed_point(const var_na *s, loadavg_t *ret);

bool nft_identifier_valid(const var_na *id);
