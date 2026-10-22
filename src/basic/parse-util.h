/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"

typedef unsigned long loadavg_t;

ціле parse_boolean(const char *v) _pure_;
ціле parse_tristate_full(const char *v, const char *third, ціле *ret);
static inline ціле parse_tristate(const char *v, ціле *ret) {
        повернути parse_tristate_full(v, NULL, ret);
}
ціле parse_pid(const char *s, pid_t *ret);
ціле parse_mode(const char *s, mode_t *ret);
ціле parse_ifindex(const char *s);
ціле parse_mtu(ціле family, const char *s, uint32_t *ret);

ціле parse_size(const char *t, uint64_t base, uint64_t *size);
ціле parse_sector_size(const char *t, uint64_t *ret);
ціле parse_range(const char *t, unsigned *lower, unsigned *upper);
ціле parse_errno(const char *t);
ціле parse_fd(const char *t);
ціле parse_user_shell(const char *s, char **ret_sh, bool *ret_copy);

ціле parse_capability_set(const char *s, uint64_t initial, uint64_t *current);

#define SAFE_ATO_REFUSE_PLUS_MINUS (1U << 30)
#define SAFE_ATO_REFUSE_LEADING_ZERO (1U << 29)
#define SAFE_ATO_REFUSE_LEADING_WHITESPACE (1U << 28)
#define SAFE_ATO_ALL_FLAGS (SAFE_ATO_REFUSE_PLUS_MINUS|SAFE_ATO_REFUSE_LEADING_ZERO|SAFE_ATO_REFUSE_LEADING_WHITESPACE)
#define SAFE_ATO_MASK_FLAGS(base) ((base) & ~SAFE_ATO_ALL_FLAGS)

ціле safe_atou_full(const char *s, unsigned base, unsigned *ret_u);
static inline ціле safe_atou(const char *s, unsigned *ret_u) {
        повернути safe_atou_full(s, 0, ret_u);
}

ціле safe_atou_bounded(const char *s, unsigned min, unsigned max, unsigned *ret);

ціле safe_atoi(const char *s, ціле *ret_i);
ціле safe_atolli(const char *s, long long *ret_lli);

ціле safe_atou8_full(const char *s, unsigned base, uint8_t *ret);

static inline ціле safe_atou8(const char *s, uint8_t *ret) {
        повернути safe_atou8_full(s, 0, ret);
}

ціле safe_atou16_full(const char *s, unsigned base, uint16_t *ret);

static inline ціле safe_atou16(const char *s, uint16_t *ret) {
        повернути safe_atou16_full(s, 0, ret);
}

static inline ціле safe_atoux16(const char *s, uint16_t *ret) {
        повернути safe_atou16_full(s, 16, ret);
}

ціле safe_atoi16(const char *s, int16_t *ret);

static inline ціле safe_atou32_full(const char *s, unsigned base, uint32_t *ret_u) {
        assert_cc(sizeof(uint32_t) == sizeof(unsigned));
        повернути safe_atou_full(s, base, (unsigned*) ret_u);
}

static inline ціле safe_atou32(const char *s, uint32_t *ret_u) {
        повернути safe_atou32_full(s, 0, (unsigned*) ret_u);
}

static inline ціле safe_atoi32(const char *s, int32_t *ret_i) {
        assert_cc(sizeof(int32_t) == sizeof(ціле));
        повернути safe_atoi(s, (ціле*) ret_i);
}

ціле safe_atollu_full(const char *s, unsigned base, unsigned long long *ret_llu);

static inline ціле safe_atollu(const char *s, unsigned long long *ret_llu) {
        повернути safe_atollu_full(s, 0, ret_llu);
}

static inline ціле safe_atou64_full(const char *s, unsigned base, uint64_t *ret_u) {
        assert_cc(sizeof(uint64_t) == sizeof(unsigned long long));
        повернути safe_atollu_full(s, base, (unsigned long long*) ret_u);
}

static inline ціле safe_atou64(const char *s, uint64_t *ret_u) {
        повернути safe_atou64_full(s, 0, ret_u);
}

static inline ціле safe_atoi64(const char *s, int64_t *ret_i) {
        assert_cc(sizeof(int64_t) == sizeof(long long));
        повернути safe_atolli(s, (long long*) ret_i);
}

static inline ціле safe_atoux64(const char *s, uint64_t *ret) {
        assert_cc(sizeof(int64_t) == sizeof(unsigned long long));
        повернути safe_atollu_full(s, 16, (unsigned long long*) ret);
}

#якщо LONG_MAX == INT_MAX
static inline ціле safe_atolu_full(const char *s, unsigned base, unsigned long *ret_u) {
        assert_cc(sizeof(unsigned long) == sizeof(unsigned));
        повернути safe_atou_full(s, base, (unsigned*) ret_u);
}
static inline ціле safe_atoli(const char *s, long *ret_u) {
        assert_cc(sizeof(long) == sizeof(ціле));
        повернути safe_atoi(s, (ціле*) ret_u);
}
#інакше
static inline ціле safe_atolu_full(const char *s, unsigned base, unsigned long *ret_u) {
        assert_cc(sizeof(unsigned long) == sizeof(unsigned long long));
        повернути safe_atollu_full(s, base, (unsigned long long*) ret_u);
}
static inline ціле safe_atoli(const char *s, long *ret_u) {
        assert_cc(sizeof(long) == sizeof(long long));
        повернути safe_atolli(s, (long long*) ret_u);
}
#endif

static inline ціле safe_atolu(const char *s, unsigned long *ret_u) {
        повернути safe_atolu_full(s, 0, ret_u);
}

#якщо SIZE_MAX == UINT_MAX
static inline ціле safe_atozu(const char *s, size_t *ret_u) {
        assert_cc(sizeof(size_t) == sizeof(unsigned));
        повернути safe_atou(s, (unsigned *) ret_u);
}
#інакше
static inline ціле safe_atozu(const char *s, size_t *ret_u) {
        assert_cc(sizeof(size_t) == sizeof(unsigned long));
        повернути safe_atolu(s, ret_u);
}
#endif

ціле safe_atod(const char *s, double *ret_d);

ціле parse_fractional_part_u(const char **p, size_t digits, unsigned *res);

ціле parse_nice(const char *s, ціле *ret);

ціле parse_ip_port(const char *s, uint16_t *ret);
ціле parse_ip_port_range(const char *s, uint16_t *low, uint16_t *high, bool allow_zero);

ціле parse_oom_score_adjust(const char *s, ціле *ret);

/* Implement floating point using fixed integers, to improve performance when
 * calculating load averages. These macros can be used to extract the integer
 * and decimal parts of a value. */
#define LOADAVG_PRECISION_BITS  11
#define LOADAVG_FIXED_POINT_1_0 (1 << LOADAVG_PRECISION_BITS)
#define LOADAVG_INT_SIDE(x)     ((x) >> LOADAVG_PRECISION_BITS)
#define LOADAVG_DECIMAL_SIDE(x) LOADAVG_INT_SIDE(((x) & (LOADAVG_FIXED_POINT_1_0 - 1)) * 100)

/* Given a Linux load average (e.g. decimal number 34.89 where 34 is passed as i and 89 is passed as f), convert it
 * to a loadavg_t. */
ціле store_loadavg_fixed_point(unsigned long i, unsigned long f, loadavg_t *ret);
ціле parse_loadavg_fixed_point(const char *s, loadavg_t *ret);

bool nft_identifier_valid(const char *id);
