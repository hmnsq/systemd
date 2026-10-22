/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"

typedef unsigned long loadavg_t;

цео parse_boolean(const char *v) _pure_;
цео parse_tristate_full(const char *v, const char *third, цео *ret);
static inline цео parse_tristate(const char *v, цео *ret) {
        врати parse_tristate_full(v, NULL, ret);
}
цео parse_pid(const char *s, pid_t *ret);
цео parse_mode(const char *s, mode_t *ret);
цео parse_ifindex(const char *s);
цео parse_mtu(цео family, const char *s, uint32_t *ret);

цео parse_size(const char *t, uint64_t base, uint64_t *size);
цео parse_sector_size(const char *t, uint64_t *ret);
цео parse_range(const char *t, unsigned *lower, unsigned *upper);
цео parse_errno(const char *t);
цео parse_fd(const char *t);
цео parse_user_shell(const char *s, char **ret_sh, bool *ret_copy);

цео parse_capability_set(const char *s, uint64_t initial, uint64_t *current);

#define SAFE_ATO_REFUSE_PLUS_MINUS (1U << 30)
#define SAFE_ATO_REFUSE_LEADING_ZERO (1U << 29)
#define SAFE_ATO_REFUSE_LEADING_WHITESPACE (1U << 28)
#define SAFE_ATO_ALL_FLAGS (SAFE_ATO_REFUSE_PLUS_MINUS|SAFE_ATO_REFUSE_LEADING_ZERO|SAFE_ATO_REFUSE_LEADING_WHITESPACE)
#define SAFE_ATO_MASK_FLAGS(base) ((base) & ~SAFE_ATO_ALL_FLAGS)

цео safe_atou_full(const char *s, unsigned base, unsigned *ret_u);
static inline цео safe_atou(const char *s, unsigned *ret_u) {
        врати safe_atou_full(s, 0, ret_u);
}

цео safe_atou_bounded(const char *s, unsigned min, unsigned max, unsigned *ret);

цео safe_atoi(const char *s, цео *ret_i);
цео safe_atolli(const char *s, long long *ret_lli);

цео safe_atou8_full(const char *s, unsigned base, uint8_t *ret);

static inline цео safe_atou8(const char *s, uint8_t *ret) {
        врати safe_atou8_full(s, 0, ret);
}

цео safe_atou16_full(const char *s, unsigned base, uint16_t *ret);

static inline цео safe_atou16(const char *s, uint16_t *ret) {
        врати safe_atou16_full(s, 0, ret);
}

static inline цео safe_atoux16(const char *s, uint16_t *ret) {
        врати safe_atou16_full(s, 16, ret);
}

цео safe_atoi16(const char *s, int16_t *ret);

static inline цео safe_atou32_full(const char *s, unsigned base, uint32_t *ret_u) {
        assert_cc(sizeof(uint32_t) == sizeof(unsigned));
        врати safe_atou_full(s, base, (unsigned*) ret_u);
}

static inline цео safe_atou32(const char *s, uint32_t *ret_u) {
        врати safe_atou32_full(s, 0, (unsigned*) ret_u);
}

static inline цео safe_atoi32(const char *s, int32_t *ret_i) {
        assert_cc(sizeof(int32_t) == sizeof(цео));
        врати safe_atoi(s, (цео*) ret_i);
}

цео safe_atollu_full(const char *s, unsigned base, unsigned long long *ret_llu);

static inline цео safe_atollu(const char *s, unsigned long long *ret_llu) {
        врати safe_atollu_full(s, 0, ret_llu);
}

static inline цео safe_atou64_full(const char *s, unsigned base, uint64_t *ret_u) {
        assert_cc(sizeof(uint64_t) == sizeof(unsigned long long));
        врати safe_atollu_full(s, base, (unsigned long long*) ret_u);
}

static inline цео safe_atou64(const char *s, uint64_t *ret_u) {
        врати safe_atou64_full(s, 0, ret_u);
}

static inline цео safe_atoi64(const char *s, int64_t *ret_i) {
        assert_cc(sizeof(int64_t) == sizeof(long long));
        врати safe_atolli(s, (long long*) ret_i);
}

static inline цео safe_atoux64(const char *s, uint64_t *ret) {
        assert_cc(sizeof(int64_t) == sizeof(unsigned long long));
        врати safe_atollu_full(s, 16, (unsigned long long*) ret);
}

#ако LONG_MAX == INT_MAX
static inline цео safe_atolu_full(const char *s, unsigned base, unsigned long *ret_u) {
        assert_cc(sizeof(unsigned long) == sizeof(unsigned));
        врати safe_atou_full(s, base, (unsigned*) ret_u);
}
static inline цео safe_atoli(const char *s, long *ret_u) {
        assert_cc(sizeof(long) == sizeof(цео));
        врати safe_atoi(s, (цео*) ret_u);
}
#инако
static inline цео safe_atolu_full(const char *s, unsigned base, unsigned long *ret_u) {
        assert_cc(sizeof(unsigned long) == sizeof(unsigned long long));
        врати safe_atollu_full(s, base, (unsigned long long*) ret_u);
}
static inline цео safe_atoli(const char *s, long *ret_u) {
        assert_cc(sizeof(long) == sizeof(long long));
        врати safe_atolli(s, (long long*) ret_u);
}
#endif

static inline цео safe_atolu(const char *s, unsigned long *ret_u) {
        врати safe_atolu_full(s, 0, ret_u);
}

#ако SIZE_MAX == UINT_MAX
static inline цео safe_atozu(const char *s, size_t *ret_u) {
        assert_cc(sizeof(size_t) == sizeof(unsigned));
        врати safe_atou(s, (unsigned *) ret_u);
}
#инако
static inline цео safe_atozu(const char *s, size_t *ret_u) {
        assert_cc(sizeof(size_t) == sizeof(unsigned long));
        врати safe_atolu(s, ret_u);
}
#endif

цео safe_atod(const char *s, double *ret_d);

цео parse_fractional_part_u(const char **p, size_t digits, unsigned *res);

цео parse_nice(const char *s, цео *ret);

цео parse_ip_port(const char *s, uint16_t *ret);
цео parse_ip_port_range(const char *s, uint16_t *low, uint16_t *high, bool allow_zero);

цео parse_oom_score_adjust(const char *s, цео *ret);

/* Implement floating point using fixed integers, to improve performance when
 * calculating load averages. These macros can be used to extract the integer
 * and decimal parts of a value. */
#define LOADAVG_PRECISION_BITS  11
#define LOADAVG_FIXED_POINT_1_0 (1 << LOADAVG_PRECISION_BITS)
#define LOADAVG_INT_SIDE(x)     ((x) >> LOADAVG_PRECISION_BITS)
#define LOADAVG_DECIMAL_SIDE(x) LOADAVG_INT_SIDE(((x) & (LOADAVG_FIXED_POINT_1_0 - 1)) * 100)

/* Given a Linux load average (e.g. decimal number 34.89 where 34 is passed as i and 89 is passed as f), convert it
 * to a loadavg_t. */
цео store_loadavg_fixed_point(unsigned long i, unsigned long f, loadavg_t *ret);
цео parse_loadavg_fixed_point(const char *s, loadavg_t *ret);

bool nft_identifier_valid(const char *id);
