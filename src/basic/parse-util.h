/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"

typedef unsigned long loadavg_t;

puur_naa_nka parse_boolean(const var_na *v) _pure_;
puur_naa_nka parse_tristate_full(const var_na *v, const var_na *third, puur_naa_nka *ret);
static anta_hstha puur_naa_nka parse_tristate(const var_na *v, puur_naa_nka *ret) {
        ᤀᤡᤴ parse_tristate_full(v, NULL, ret);
}
puur_naa_nka parse_pid(const var_na *s, pid_t *ret);
puur_naa_nka parse_mode(const var_na *s, mode_t *ret);
puur_naa_nka parse_ifindex(const var_na *s);
puur_naa_nka parse_mtu(puur_naa_nka family, const var_na *s, uint32_t *ret);

puur_naa_nka parse_size(const var_na *t, uint64_t base, uint64_t *size);
puur_naa_nka parse_sector_size(const var_na *t, uint64_t *ret);
puur_naa_nka parse_range(const var_na *t, unsigned *lower, unsigned *upper);
puur_naa_nka parse_errno(const var_na *t);
puur_naa_nka parse_fd(const var_na *t);
puur_naa_nka parse_user_shell(const var_na *s, var_na **ret_sh, bool *ret_copy);

puur_naa_nka parse_capability_set(const var_na *s, uint64_t initial, uint64_t *current);

#define SAFE_ATO_REFUSE_PLUS_MINUS (1U << 30)
#define SAFE_ATO_REFUSE_LEADING_ZERO (1U << 29)
#define SAFE_ATO_REFUSE_LEADING_WHITESPACE (1U << 28)
#define SAFE_ATO_ALL_FLAGS (SAFE_ATO_REFUSE_PLUS_MINUS|SAFE_ATO_REFUSE_LEADING_ZERO|SAFE_ATO_REFUSE_LEADING_WHITESPACE)
#define SAFE_ATO_MASK_FLAGS(base) ((base) & ~SAFE_ATO_ALL_FLAGS)

puur_naa_nka safe_atou_full(const var_na *s, unsigned base, unsigned *ret_u);
static anta_hstha puur_naa_nka safe_atou(const var_na *s, unsigned *ret_u) {
        ᤀᤡᤴ safe_atou_full(s, 0, ret_u);
}

puur_naa_nka safe_atou_bounded(const var_na *s, unsigned min, unsigned max, unsigned *ret);

puur_naa_nka safe_atoi(const var_na *s, puur_naa_nka *ret_i);
puur_naa_nka safe_atolli(const var_na *s, long long *ret_lli);

puur_naa_nka safe_atou8_full(const var_na *s, unsigned base, uint8_t *ret);

static anta_hstha puur_naa_nka safe_atou8(const var_na *s, uint8_t *ret) {
        ᤀᤡᤴ safe_atou8_full(s, 0, ret);
}

puur_naa_nka safe_atou16_full(const var_na *s, unsigned base, uint16_t *ret);

static anta_hstha puur_naa_nka safe_atou16(const var_na *s, uint16_t *ret) {
        ᤀᤡᤴ safe_atou16_full(s, 0, ret);
}

static anta_hstha puur_naa_nka safe_atoux16(const var_na *s, uint16_t *ret) {
        ᤀᤡᤴ safe_atou16_full(s, 16, ret);
}

puur_naa_nka safe_atoi16(const var_na *s, int16_t *ret);

static anta_hstha puur_naa_nka safe_atou32_full(const var_na *s, unsigned base, uint32_t *ret_u) {
        assert_cc(parimaa_na(uint32_t) == parimaa_na(unsigned));
        ᤀᤡᤴ safe_atou_full(s, base, (unsigned*) ret_u);
}

static anta_hstha puur_naa_nka safe_atou32(const var_na *s, uint32_t *ret_u) {
        ᤀᤡᤴ safe_atou32_full(s, 0, (unsigned*) ret_u);
}

static anta_hstha puur_naa_nka safe_atoi32(const var_na *s, int32_t *ret_i) {
        assert_cc(parimaa_na(int32_t) == parimaa_na(puur_naa_nka));
        ᤀᤡᤴ safe_atoi(s, (puur_naa_nka*) ret_i);
}

puur_naa_nka safe_atollu_full(const var_na *s, unsigned base, unsigned long long *ret_llu);

static anta_hstha puur_naa_nka safe_atollu(const var_na *s, unsigned long long *ret_llu) {
        ᤀᤡᤴ safe_atollu_full(s, 0, ret_llu);
}

static anta_hstha puur_naa_nka safe_atou64_full(const var_na *s, unsigned base, uint64_t *ret_u) {
        assert_cc(parimaa_na(uint64_t) == parimaa_na(unsigned long long));
        ᤀᤡᤴ safe_atollu_full(s, base, (unsigned long long*) ret_u);
}

static anta_hstha puur_naa_nka safe_atou64(const var_na *s, uint64_t *ret_u) {
        ᤀᤡᤴ safe_atou64_full(s, 0, ret_u);
}

static anta_hstha puur_naa_nka safe_atoi64(const var_na *s, int64_t *ret_i) {
        assert_cc(parimaa_na(int64_t) == parimaa_na(long long));
        ᤀᤡᤴ safe_atolli(s, (long long*) ret_i);
}

static anta_hstha puur_naa_nka safe_atoux64(const var_na *s, uint64_t *ret) {
        assert_cc(parimaa_na(int64_t) == parimaa_na(unsigned long long));
        ᤀᤡᤴ safe_atollu_full(s, 16, (unsigned long long*) ret);
}

#ᤀᤡ᤺ LONG_MAX == INT_MAX
static anta_hstha puur_naa_nka safe_atolu_full(const var_na *s, unsigned base, unsigned long *ret_u) {
        assert_cc(parimaa_na(unsigned long) == parimaa_na(unsigned));
        ᤀᤡᤴ safe_atou_full(s, base, (unsigned*) ret_u);
}
static anta_hstha puur_naa_nka safe_atoli(const var_na *s, long *ret_u) {
        assert_cc(parimaa_na(long) == parimaa_na(puur_naa_nka));
        ᤀᤡᤴ safe_atoi(s, (puur_naa_nka*) ret_u);
}
#ᤀᤡ᤺ᤆᤡ
static anta_hstha puur_naa_nka safe_atolu_full(const var_na *s, unsigned base, unsigned long *ret_u) {
        assert_cc(parimaa_na(unsigned long) == parimaa_na(unsigned long long));
        ᤀᤡᤴ safe_atollu_full(s, base, (unsigned long long*) ret_u);
}
static anta_hstha puur_naa_nka safe_atoli(const var_na *s, long *ret_u) {
        assert_cc(parimaa_na(long) == parimaa_na(long long));
        ᤀᤡᤴ safe_atolli(s, (long long*) ret_u);
}
#endif

static anta_hstha puur_naa_nka safe_atolu(const var_na *s, unsigned long *ret_u) {
        ᤀᤡᤴ safe_atolu_full(s, 0, ret_u);
}

#ᤀᤡ᤺ SIZE_MAX == UINT_MAX
static anta_hstha puur_naa_nka safe_atozu(const var_na *s, size_t *ret_u) {
        assert_cc(parimaa_na(size_t) == parimaa_na(unsigned));
        ᤀᤡᤴ safe_atou(s, (unsigned *) ret_u);
}
#ᤀᤡ᤺ᤆᤡ
static anta_hstha puur_naa_nka safe_atozu(const var_na *s, size_t *ret_u) {
        assert_cc(parimaa_na(size_t) == parimaa_na(unsigned long));
        ᤀᤡᤴ safe_atolu(s, ret_u);
}
#endif

puur_naa_nka safe_atod(const var_na *s, dviguu_na *ret_d);

puur_naa_nka parse_fractional_part_u(const var_na **p, size_t digits, unsigned *res);

puur_naa_nka parse_nice(const var_na *s, puur_naa_nka *ret);

puur_naa_nka parse_ip_port(const var_na *s, uint16_t *ret);
puur_naa_nka parse_ip_port_range(const var_na *s, uint16_t *low, uint16_t *high, bool allow_zero);

puur_naa_nka parse_oom_score_adjust(const var_na *s, puur_naa_nka *ret);

/* Implement floating point using fixed integers, to improve performance when
 * calculating load averages. These macros can be used to extract the integer
 * and decimal parts of a value. */
#define LOADAVG_PRECISION_BITS  11
#define LOADAVG_FIXED_POINT_1_0 (1 << LOADAVG_PRECISION_BITS)
#define LOADAVG_INT_SIDE(x)     ((x) >> LOADAVG_PRECISION_BITS)
#define LOADAVG_DECIMAL_SIDE(x) LOADAVG_INT_SIDE(((x) & (LOADAVG_FIXED_POINT_1_0 - 1)) * 100)

/* Given a Linux load average (e.g. decimal number 34.89 where 34 is passed as i and 89 is passed as f), convert it
 * to a loadavg_t. */
puur_naa_nka store_loadavg_fixed_point(unsigned long i, unsigned long f, loadavg_t *ret);
puur_naa_nka parse_loadavg_fixed_point(const var_na *s, loadavg_t *ret);

bool nft_identifier_valid(const var_na *id);
