/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ಅನ್ತರ್ಭಾವ "forward.h"

ಪ್ರಕಾರನಿರ್ದೇಶ unsigned long loadavg_t;

ಪೂರ್ಣಾಙ್ಕ parse_boolean(ಧ್ರುವ ವರ್ಣ *v) _pure_;
ಪೂರ್ಣಾಙ್ಕ parse_tristate_full(ಧ್ರುವ ವರ್ಣ *v, ಧ್ರುವ ವರ್ಣ *third, ಪೂರ್ಣಾಙ್ಕ *ret);
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ parse_tristate(ಧ್ರುವ ವರ್ಣ *v, ಪೂರ್ಣಾಙ್ಕ *ret) {
        ಪ್ರತ್ಯಾಗಮ parse_tristate_full(v, ರಿಕ್ತ, ret);
}
ಪೂರ್ಣಾಙ್ಕ parse_pid(ಧ್ರುವ ವರ್ಣ *s, pid_t *ret);
ಪೂರ್ಣಾಙ್ಕ parse_mode(ಧ್ರುವ ವರ್ಣ *s, mode_t *ret);
ಪೂರ್ಣಾಙ್ಕ parse_ifindex(ಧ್ರುವ ವರ್ಣ *s);
ಪೂರ್ಣಾಙ್ಕ parse_mtu(ಪೂರ್ಣಾಙ್ಕ family, ಧ್ರುವ ವರ್ಣ *s, uint32_t *ret);

ಪೂರ್ಣಾಙ್ಕ parse_size(ಧ್ರುವ ವರ್ಣ *t, uint64_t base, uint64_t *size);
ಪೂರ್ಣಾಙ್ಕ parse_sector_size(ಧ್ರುವ ವರ್ಣ *t, uint64_t *ret);
ಪೂರ್ಣಾಙ್ಕ parse_range(ಧ್ರುವ ವರ್ಣ *t, unsigned *lower, unsigned *upper);
ಪೂರ್ಣಾಙ್ಕ parse_errno(ಧ್ರುವ ವರ್ಣ *t);
ಪೂರ್ಣಾಙ್ಕ parse_fd(ಧ್ರುವ ವರ್ಣ *t);
ಪೂರ್ಣಾಙ್ಕ parse_user_shell(ಧ್ರುವ ವರ್ಣ *s, ವರ್ಣ **ret_sh, bool *ret_copy);

ಪೂರ್ಣಾಙ್ಕ parse_capability_set(ಧ್ರುವ ವರ್ಣ *s, uint64_t initial, uint64_t *current);

#define SAFE_ATO_REFUSE_PLUS_MINUS (1U << 30)
#define SAFE_ATO_REFUSE_LEADING_ZERO (1U << 29)
#define SAFE_ATO_REFUSE_LEADING_WHITESPACE (1U << 28)
#define SAFE_ATO_ALL_FLAGS (SAFE_ATO_REFUSE_PLUS_MINUS|SAFE_ATO_REFUSE_LEADING_ZERO|SAFE_ATO_REFUSE_LEADING_WHITESPACE)
#define SAFE_ATO_MASK_FLAGS(base) ((base) & ~SAFE_ATO_ALL_FLAGS)

ಪೂರ್ಣಾಙ್ಕ safe_atou_full(ಧ್ರುವ ವರ್ಣ *s, unsigned base, unsigned *ret_u);
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ safe_atou(ಧ್ರುವ ವರ್ಣ *s, unsigned *ret_u) {
        ಪ್ರತ್ಯಾಗಮ safe_atou_full(s, 0, ret_u);
}

ಪೂರ್ಣಾಙ್ಕ safe_atou_bounded(ಧ್ರುವ ವರ್ಣ *s, unsigned min, unsigned max, unsigned *ret);

ಪೂರ್ಣಾಙ್ಕ safe_atoi(ಧ್ರುವ ವರ್ಣ *s, ಪೂರ್ಣಾಙ್ಕ *ret_i);
ಪೂರ್ಣಾಙ್ಕ safe_atolli(ಧ್ರುವ ವರ್ಣ *s, long long *ret_lli);

ಪೂರ್ಣಾಙ್ಕ safe_atou8_full(ಧ್ರುವ ವರ್ಣ *s, unsigned base, uint8_t *ret);

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ safe_atou8(ಧ್ರುವ ವರ್ಣ *s, uint8_t *ret) {
        ಪ್ರತ್ಯಾಗಮ safe_atou8_full(s, 0, ret);
}

ಪೂರ್ಣಾಙ್ಕ safe_atou16_full(ಧ್ರುವ ವರ್ಣ *s, unsigned base, uint16_t *ret);

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ safe_atou16(ಧ್ರುವ ವರ್ಣ *s, uint16_t *ret) {
        ಪ್ರತ್ಯಾಗಮ safe_atou16_full(s, 0, ret);
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ safe_atoux16(ಧ್ರುವ ವರ್ಣ *s, uint16_t *ret) {
        ಪ್ರತ್ಯಾಗಮ safe_atou16_full(s, 16, ret);
}

ಪೂರ್ಣಾಙ್ಕ safe_atoi16(ಧ್ರುವ ವರ್ಣ *s, int16_t *ret);

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ safe_atou32_full(ಧ್ರುವ ವರ್ಣ *s, unsigned base, uint32_t *ret_u) {
        assert_cc(ಪರಿಮಾಣ(uint32_t) == ಪರಿಮಾಣ(unsigned));
        ಪ್ರತ್ಯಾಗಮ safe_atou_full(s, base, (unsigned*) ret_u);
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ safe_atou32(ಧ್ರುವ ವರ್ಣ *s, uint32_t *ret_u) {
        ಪ್ರತ್ಯಾಗಮ safe_atou32_full(s, 0, (unsigned*) ret_u);
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ safe_atoi32(ಧ್ರುವ ವರ್ಣ *s, int32_t *ret_i) {
        assert_cc(ಪರಿಮಾಣ(int32_t) == ಪರಿಮಾಣ(ಪೂರ್ಣಾಙ್ಕ));
        ಪ್ರತ್ಯಾಗಮ safe_atoi(s, (ಪೂರ್ಣಾಙ್ಕ*) ret_i);
}

ಪೂರ್ಣಾಙ್ಕ safe_atollu_full(ಧ್ರುವ ವರ್ಣ *s, unsigned base, unsigned long long *ret_llu);

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ safe_atollu(ಧ್ರುವ ವರ್ಣ *s, unsigned long long *ret_llu) {
        ಪ್ರತ್ಯಾಗಮ safe_atollu_full(s, 0, ret_llu);
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ safe_atou64_full(ಧ್ರುವ ವರ್ಣ *s, unsigned base, uint64_t *ret_u) {
        assert_cc(ಪರಿಮಾಣ(uint64_t) == ಪರಿಮಾಣ(unsigned long long));
        ಪ್ರತ್ಯಾಗಮ safe_atollu_full(s, base, (unsigned long long*) ret_u);
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ safe_atou64(ಧ್ರುವ ವರ್ಣ *s, uint64_t *ret_u) {
        ಪ್ರತ್ಯಾಗಮ safe_atou64_full(s, 0, ret_u);
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ safe_atoi64(ಧ್ರುವ ವರ್ಣ *s, int64_t *ret_i) {
        assert_cc(ಪರಿಮಾಣ(int64_t) == ಪರಿಮಾಣ(long long));
        ಪ್ರತ್ಯಾಗಮ safe_atolli(s, (long long*) ret_i);
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ safe_atoux64(ಧ್ರುವ ವರ್ಣ *s, uint64_t *ret) {
        assert_cc(ಪರಿಮಾಣ(int64_t) == ಪರಿಮಾಣ(unsigned long long));
        ಪ್ರತ್ಯಾಗಮ safe_atollu_full(s, 16, (unsigned long long*) ret);
}

#ಯದಿ LONG_MAX == INT_MAX
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ safe_atolu_full(ಧ್ರುವ ವರ್ಣ *s, unsigned base, unsigned long *ret_u) {
        assert_cc(ಪರಿಮಾಣ(unsigned long) == ಪರಿಮಾಣ(unsigned));
        ಪ್ರತ್ಯಾಗಮ safe_atou_full(s, base, (unsigned*) ret_u);
}
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ safe_atoli(ಧ್ರುವ ವರ್ಣ *s, long *ret_u) {
        assert_cc(ಪರಿಮಾಣ(long) == ಪರಿಮಾಣ(ಪೂರ್ಣಾಙ್ಕ));
        ಪ್ರತ್ಯಾಗಮ safe_atoi(s, (ಪೂರ್ಣಾಙ್ಕ*) ret_u);
}
#ಅನ್ಯಥಾ
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ safe_atolu_full(ಧ್ರುವ ವರ್ಣ *s, unsigned base, unsigned long *ret_u) {
        assert_cc(ಪರಿಮಾಣ(unsigned long) == ಪರಿಮಾಣ(unsigned long long));
        ಪ್ರತ್ಯಾಗಮ safe_atollu_full(s, base, (unsigned long long*) ret_u);
}
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ safe_atoli(ಧ್ರುವ ವರ್ಣ *s, long *ret_u) {
        assert_cc(ಪರಿಮಾಣ(long) == ಪರಿಮಾಣ(long long));
        ಪ್ರತ್ಯಾಗಮ safe_atolli(s, (long long*) ret_u);
}
#endif

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ safe_atolu(ಧ್ರುವ ವರ್ಣ *s, unsigned long *ret_u) {
        ಪ್ರತ್ಯಾಗಮ safe_atolu_full(s, 0, ret_u);
}

#ಯದಿ SIZE_MAX == UINT_MAX
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ safe_atozu(ಧ್ರುವ ವರ್ಣ *s, size_t *ret_u) {
        assert_cc(ಪರಿಮಾಣ(size_t) == ಪರಿಮಾಣ(unsigned));
        ಪ್ರತ್ಯಾಗಮ safe_atou(s, (unsigned *) ret_u);
}
#ಅನ್ಯಥಾ
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ safe_atozu(ಧ್ರುವ ವರ್ಣ *s, size_t *ret_u) {
        assert_cc(ಪರಿಮಾಣ(size_t) == ಪರಿಮಾಣ(unsigned long));
        ಪ್ರತ್ಯಾಗಮ safe_atolu(s, ret_u);
}
#endif

ಪೂರ್ಣಾಙ್ಕ safe_atod(ಧ್ರುವ ವರ್ಣ *s, ದ್ವಿಗುಣ *ret_d);

ಪೂರ್ಣಾಙ್ಕ parse_fractional_part_u(ಧ್ರುವ ವರ್ಣ **p, size_t digits, unsigned *res);

ಪೂರ್ಣಾಙ್ಕ parse_nice(ಧ್ರುವ ವರ್ಣ *s, ಪೂರ್ಣಾಙ್ಕ *ret);

ಪೂರ್ಣಾಙ್ಕ parse_ip_port(ಧ್ರುವ ವರ್ಣ *s, uint16_t *ret);
ಪೂರ್ಣಾಙ್ಕ parse_ip_port_range(ಧ್ರುವ ವರ್ಣ *s, uint16_t *low, uint16_t *high, bool allow_zero);

ಪೂರ್ಣಾಙ್ಕ parse_oom_score_adjust(ಧ್ರುವ ವರ್ಣ *s, ಪೂರ್ಣಾಙ್ಕ *ret);

/* Implement floating point using fixed integers, to improve performance when
 * calculating load averages. These macros can be used to extract the integer
 * and decimal parts of a value. */
#define LOADAVG_PRECISION_BITS  11
#define LOADAVG_FIXED_POINT_1_0 (1 << LOADAVG_PRECISION_BITS)
#define LOADAVG_INT_SIDE(x)     ((x) >> LOADAVG_PRECISION_BITS)
#define LOADAVG_DECIMAL_SIDE(x) LOADAVG_INT_SIDE(((x) & (LOADAVG_FIXED_POINT_1_0 - 1)) * 100)

/* Given a Linux load average (e.g. decimal number 34.89 where 34 is passed as i and 89 is passed as f), convert it
 * to a loadavg_t. */
ಪೂರ್ಣಾಙ್ಕ store_loadavg_fixed_point(unsigned long i, unsigned long f, loadavg_t *ret);
ಪೂರ್ಣಾಙ್ಕ parse_loadavg_fixed_point(ಧ್ರುವ ವರ್ಣ *s, loadavg_t *ret);

bool nft_identifier_valid(ಧ್ರುವ ವರ್ಣ *id);
