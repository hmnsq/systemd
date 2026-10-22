/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাব "forward.h"

প্রকারলেখ unsigned long loadavg_t;

পূর্ণসংখ্যা parse_boolean(ধ্রুবক অক্ষর *v) _pure_;
পূর্ণসংখ্যা parse_tristate_full(ধ্রুবক অক্ষর *v, ধ্রুবক অক্ষর *third, পূর্ণসংখ্যা *ret);
স্থির অন্তঃরেখ পূর্ণসংখ্যা parse_tristate(ধ্রুবক অক্ষর *v, পূর্ণসংখ্যা *ret) {
        ফেরত parse_tristate_full(v, ফাঁকা, ret);
}
পূর্ণসংখ্যা parse_pid(ধ্রুবক অক্ষর *s, pid_t *ret);
পূর্ণসংখ্যা parse_mode(ধ্রুবক অক্ষর *s, mode_t *ret);
পূর্ণসংখ্যা parse_ifindex(ধ্রুবক অক্ষর *s);
পূর্ণসংখ্যা parse_mtu(পূর্ণসংখ্যা family, ধ্রুবক অক্ষর *s, uint32_t *ret);

পূর্ণসংখ্যা parse_আকার(ধ্রুবক অক্ষর *t, uint64_t base, uint64_t *আকার_2);
পূর্ণসংখ্যা parse_sector_আকার(ধ্রুবক অক্ষর *t, uint64_t *ret);
পূর্ণসংখ্যা parse_range(ধ্রুবক অক্ষর *t, unsigned *lower, unsigned *upper);
পূর্ণসংখ্যা parse_errno(ধ্রুবক অক্ষর *t);
পূর্ণসংখ্যা parse_fd(ধ্রুবক অক্ষর *t);
পূর্ণসংখ্যা parse_user_shell(ধ্রুবক অক্ষর *s, অক্ষর **ret_sh, bool *ret_copy);

পূর্ণসংখ্যা parse_capability_set(ধ্রুবক অক্ষর *s, uint64_t initial, uint64_t *current);

#define SAFE_ATO_REFUSE_PLUS_MINUS (1U << 30)
#define SAFE_ATO_REFUSE_LEADING_ZERO (1U << 29)
#define SAFE_ATO_REFUSE_LEADING_WHITESPACE (1U << 28)
#define SAFE_ATO_ALL_FLAGS (SAFE_ATO_REFUSE_PLUS_MINUS|SAFE_ATO_REFUSE_LEADING_ZERO|SAFE_ATO_REFUSE_LEADING_WHITESPACE)
#define SAFE_ATO_MASK_FLAGS(base) ((base) & ~SAFE_ATO_ALL_FLAGS)

পূর্ণসংখ্যা safe_atou_full(ধ্রুবক অক্ষর *s, unsigned base, unsigned *ret_u);
স্থির অন্তঃরেখ পূর্ণসংখ্যা safe_atou(ধ্রুবক অক্ষর *s, unsigned *ret_u) {
        ফেরত safe_atou_full(s, 0, ret_u);
}

পূর্ণসংখ্যা safe_atou_bounded(ধ্রুবক অক্ষর *s, unsigned min, unsigned max, unsigned *ret);

পূর্ণসংখ্যা safe_atoi(ধ্রুবক অক্ষর *s, পূর্ণসংখ্যা *ret_i);
পূর্ণসংখ্যা safe_atolli(ধ্রুবক অক্ষর *s, long long *ret_lli);

পূর্ণসংখ্যা safe_atou8_full(ধ্রুবক অক্ষর *s, unsigned base, uint8_t *ret);

স্থির অন্তঃরেখ পূর্ণসংখ্যা safe_atou8(ধ্রুবক অক্ষর *s, uint8_t *ret) {
        ফেরত safe_atou8_full(s, 0, ret);
}

পূর্ণসংখ্যা safe_atou16_full(ধ্রুবক অক্ষর *s, unsigned base, uint16_t *ret);

স্থির অন্তঃরেখ পূর্ণসংখ্যা safe_atou16(ধ্রুবক অক্ষর *s, uint16_t *ret) {
        ফেরত safe_atou16_full(s, 0, ret);
}

স্থির অন্তঃরেখ পূর্ণসংখ্যা safe_atoux16(ধ্রুবক অক্ষর *s, uint16_t *ret) {
        ফেরত safe_atou16_full(s, 16, ret);
}

পূর্ণসংখ্যা safe_atoi16(ধ্রুবক অক্ষর *s, int16_t *ret);

স্থির অন্তঃরেখ পূর্ণসংখ্যা safe_atou32_full(ধ্রুবক অক্ষর *s, unsigned base, uint32_t *ret_u) {
        assert_cc(আকার(uint32_t) == আকার(unsigned));
        ফেরত safe_atou_full(s, base, (unsigned*) ret_u);
}

স্থির অন্তঃরেখ পূর্ণসংখ্যা safe_atou32(ধ্রুবক অক্ষর *s, uint32_t *ret_u) {
        ফেরত safe_atou32_full(s, 0, (unsigned*) ret_u);
}

স্থির অন্তঃরেখ পূর্ণসংখ্যা safe_atoi32(ধ্রুবক অক্ষর *s, int32_t *ret_i) {
        assert_cc(আকার(int32_t) == আকার(পূর্ণসংখ্যা));
        ফেরত safe_atoi(s, (পূর্ণসংখ্যা*) ret_i);
}

পূর্ণসংখ্যা safe_atollu_full(ধ্রুবক অক্ষর *s, unsigned base, unsigned long long *ret_llu);

স্থির অন্তঃরেখ পূর্ণসংখ্যা safe_atollu(ধ্রুবক অক্ষর *s, unsigned long long *ret_llu) {
        ফেরত safe_atollu_full(s, 0, ret_llu);
}

স্থির অন্তঃরেখ পূর্ণসংখ্যা safe_atou64_full(ধ্রুবক অক্ষর *s, unsigned base, uint64_t *ret_u) {
        assert_cc(আকার(uint64_t) == আকার(unsigned long long));
        ফেরত safe_atollu_full(s, base, (unsigned long long*) ret_u);
}

স্থির অন্তঃরেখ পূর্ণসংখ্যা safe_atou64(ধ্রুবক অক্ষর *s, uint64_t *ret_u) {
        ফেরত safe_atou64_full(s, 0, ret_u);
}

স্থির অন্তঃরেখ পূর্ণসংখ্যা safe_atoi64(ধ্রুবক অক্ষর *s, int64_t *ret_i) {
        assert_cc(আকার(int64_t) == আকার(long long));
        ফেরত safe_atolli(s, (long long*) ret_i);
}

স্থির অন্তঃরেখ পূর্ণসংখ্যা safe_atoux64(ধ্রুবক অক্ষর *s, uint64_t *ret) {
        assert_cc(আকার(int64_t) == আকার(unsigned long long));
        ফেরত safe_atollu_full(s, 16, (unsigned long long*) ret);
}

#যদি LONG_MAX == INT_MAX
স্থির অন্তঃরেখ পূর্ণসংখ্যা safe_atolu_full(ধ্রুবক অক্ষর *s, unsigned base, unsigned long *ret_u) {
        assert_cc(আকার(unsigned long) == আকার(unsigned));
        ফেরত safe_atou_full(s, base, (unsigned*) ret_u);
}
স্থির অন্তঃরেখ পূর্ণসংখ্যা safe_atoli(ধ্রুবক অক্ষর *s, long *ret_u) {
        assert_cc(আকার(long) == আকার(পূর্ণসংখ্যা));
        ফেরত safe_atoi(s, (পূর্ণসংখ্যা*) ret_u);
}
#অন্যথা
স্থির অন্তঃরেখ পূর্ণসংখ্যা safe_atolu_full(ধ্রুবক অক্ষর *s, unsigned base, unsigned long *ret_u) {
        assert_cc(আকার(unsigned long) == আকার(unsigned long long));
        ফেরত safe_atollu_full(s, base, (unsigned long long*) ret_u);
}
স্থির অন্তঃরেখ পূর্ণসংখ্যা safe_atoli(ধ্রুবক অক্ষর *s, long *ret_u) {
        assert_cc(আকার(long) == আকার(long long));
        ফেরত safe_atolli(s, (long long*) ret_u);
}
#endif

স্থির অন্তঃরেখ পূর্ণসংখ্যা safe_atolu(ধ্রুবক অক্ষর *s, unsigned long *ret_u) {
        ফেরত safe_atolu_full(s, 0, ret_u);
}

#যদি SIZE_MAX == UINT_MAX
স্থির অন্তঃরেখ পূর্ণসংখ্যা safe_atozu(ধ্রুবক অক্ষর *s, size_t *ret_u) {
        assert_cc(আকার(size_t) == আকার(unsigned));
        ফেরত safe_atou(s, (unsigned *) ret_u);
}
#অন্যথা
স্থির অন্তঃরেখ পূর্ণসংখ্যা safe_atozu(ধ্রুবক অক্ষর *s, size_t *ret_u) {
        assert_cc(আকার(size_t) == আকার(unsigned long));
        ফেরত safe_atolu(s, ret_u);
}
#endif

পূর্ণসংখ্যা safe_atod(ধ্রুবক অক্ষর *s, দ্বিগুণ *ret_d);

পূর্ণসংখ্যা parse_fractional_part_u(ধ্রুবক অক্ষর **p, size_t digits, unsigned *res);

পূর্ণসংখ্যা parse_nice(ধ্রুবক অক্ষর *s, পূর্ণসংখ্যা *ret);

পূর্ণসংখ্যা parse_ip_port(ধ্রুবক অক্ষর *s, uint16_t *ret);
পূর্ণসংখ্যা parse_ip_port_range(ধ্রুবক অক্ষর *s, uint16_t *low, uint16_t *high, bool allow_zero);

পূর্ণসংখ্যা parse_oom_score_adjust(ধ্রুবক অক্ষর *s, পূর্ণসংখ্যা *ret);

/* Implement floating point using fixed integers, to improve performance when
 * calculating load averages. These macros can be used to extract the integer
 * and decimal parts of a value. */
#define LOADAVG_PRECISION_BITS  11
#define LOADAVG_FIXED_POINT_1_0 (1 << LOADAVG_PRECISION_BITS)
#define LOADAVG_INT_SIDE(x)     ((x) >> LOADAVG_PRECISION_BITS)
#define LOADAVG_DECIMAL_SIDE(x) LOADAVG_INT_SIDE(((x) & (LOADAVG_FIXED_POINT_1_0 - 1)) * 100)

/* Given a Linux load average (e.g. decimal number 34.89 where 34 is passed as i and 89 is passed as f), convert it
 * to a loadavg_t. */
পূর্ণসংখ্যা store_loadavg_fixed_point(unsigned long i, unsigned long f, loadavg_t *ret);
পূর্ণসংখ্যা parse_loadavg_fixed_point(ধ্রুবক অক্ষর *s, loadavg_t *ret);

bool nft_identifier_valid(ধ্রুবক অক্ষর *id);
