/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাব "forward.h"

প্রকারনির্দেশ unsigned long loadavg_t;

পূর্ণাঙ্ক parse_boolean(ধ্রুব বর্ণ *v) _pure_;
পূর্ণাঙ্ক parse_tristate_full(ধ্রুব বর্ণ *v, ধ্রুব বর্ণ *third, পূর্ণাঙ্ক *ret);
স্থির অন্তঃস্থ পূর্ণাঙ্ক parse_tristate(ধ্রুব বর্ণ *v, পূর্ণাঙ্ক *ret) {
        প্রত্যাগম parse_tristate_full(v, রিক্ত, ret);
}
পূর্ণাঙ্ক parse_pid(ধ্রুব বর্ণ *s, pid_t *ret);
পূর্ণাঙ্ক parse_mode(ধ্রুব বর্ণ *s, mode_t *ret);
পূর্ণাঙ্ক parse_ifindex(ধ্রুব বর্ণ *s);
পূর্ণাঙ্ক parse_mtu(পূর্ণাঙ্ক family, ধ্রুব বর্ণ *s, uint32_t *ret);

পূর্ণাঙ্ক parse_size(ধ্রুব বর্ণ *t, uint64_t base, uint64_t *size);
পূর্ণাঙ্ক parse_sector_size(ধ্রুব বর্ণ *t, uint64_t *ret);
পূর্ণাঙ্ক parse_range(ধ্রুব বর্ণ *t, unsigned *lower, unsigned *upper);
পূর্ণাঙ্ক parse_errno(ধ্রুব বর্ণ *t);
পূর্ণাঙ্ক parse_fd(ধ্রুব বর্ণ *t);
পূর্ণাঙ্ক parse_user_shell(ধ্রুব বর্ণ *s, বর্ণ **ret_sh, bool *ret_copy);

পূর্ণাঙ্ক parse_capability_set(ধ্রুব বর্ণ *s, uint64_t initial, uint64_t *current);

#define SAFE_ATO_REFUSE_PLUS_MINUS (1U << 30)
#define SAFE_ATO_REFUSE_LEADING_ZERO (1U << 29)
#define SAFE_ATO_REFUSE_LEADING_WHITESPACE (1U << 28)
#define SAFE_ATO_ALL_FLAGS (SAFE_ATO_REFUSE_PLUS_MINUS|SAFE_ATO_REFUSE_LEADING_ZERO|SAFE_ATO_REFUSE_LEADING_WHITESPACE)
#define SAFE_ATO_MASK_FLAGS(base) ((base) & ~SAFE_ATO_ALL_FLAGS)

পূর্ণাঙ্ক safe_atou_full(ধ্রুব বর্ণ *s, unsigned base, unsigned *ret_u);
স্থির অন্তঃস্থ পূর্ণাঙ্ক safe_atou(ধ্রুব বর্ণ *s, unsigned *ret_u) {
        প্রত্যাগম safe_atou_full(s, 0, ret_u);
}

পূর্ণাঙ্ক safe_atou_bounded(ধ্রুব বর্ণ *s, unsigned min, unsigned max, unsigned *ret);

পূর্ণাঙ্ক safe_atoi(ধ্রুব বর্ণ *s, পূর্ণাঙ্ক *ret_i);
পূর্ণাঙ্ক safe_atolli(ধ্রুব বর্ণ *s, long long *ret_lli);

পূর্ণাঙ্ক safe_atou8_full(ধ্রুব বর্ণ *s, unsigned base, uint8_t *ret);

স্থির অন্তঃস্থ পূর্ণাঙ্ক safe_atou8(ধ্রুব বর্ণ *s, uint8_t *ret) {
        প্রত্যাগম safe_atou8_full(s, 0, ret);
}

পূর্ণাঙ্ক safe_atou16_full(ধ্রুব বর্ণ *s, unsigned base, uint16_t *ret);

স্থির অন্তঃস্থ পূর্ণাঙ্ক safe_atou16(ধ্রুব বর্ণ *s, uint16_t *ret) {
        প্রত্যাগম safe_atou16_full(s, 0, ret);
}

স্থির অন্তঃস্থ পূর্ণাঙ্ক safe_atoux16(ধ্রুব বর্ণ *s, uint16_t *ret) {
        প্রত্যাগম safe_atou16_full(s, 16, ret);
}

পূর্ণাঙ্ক safe_atoi16(ধ্রুব বর্ণ *s, int16_t *ret);

স্থির অন্তঃস্থ পূর্ণাঙ্ক safe_atou32_full(ধ্রুব বর্ণ *s, unsigned base, uint32_t *ret_u) {
        assert_cc(পরিমাণ(uint32_t) == পরিমাণ(unsigned));
        প্রত্যাগম safe_atou_full(s, base, (unsigned*) ret_u);
}

স্থির অন্তঃস্থ পূর্ণাঙ্ক safe_atou32(ধ্রুব বর্ণ *s, uint32_t *ret_u) {
        প্রত্যাগম safe_atou32_full(s, 0, (unsigned*) ret_u);
}

স্থির অন্তঃস্থ পূর্ণাঙ্ক safe_atoi32(ধ্রুব বর্ণ *s, int32_t *ret_i) {
        assert_cc(পরিমাণ(int32_t) == পরিমাণ(পূর্ণাঙ্ক));
        প্রত্যাগম safe_atoi(s, (পূর্ণাঙ্ক*) ret_i);
}

পূর্ণাঙ্ক safe_atollu_full(ধ্রুব বর্ণ *s, unsigned base, unsigned long long *ret_llu);

স্থির অন্তঃস্থ পূর্ণাঙ্ক safe_atollu(ধ্রুব বর্ণ *s, unsigned long long *ret_llu) {
        প্রত্যাগম safe_atollu_full(s, 0, ret_llu);
}

স্থির অন্তঃস্থ পূর্ণাঙ্ক safe_atou64_full(ধ্রুব বর্ণ *s, unsigned base, uint64_t *ret_u) {
        assert_cc(পরিমাণ(uint64_t) == পরিমাণ(unsigned long long));
        প্রত্যাগম safe_atollu_full(s, base, (unsigned long long*) ret_u);
}

স্থির অন্তঃস্থ পূর্ণাঙ্ক safe_atou64(ধ্রুব বর্ণ *s, uint64_t *ret_u) {
        প্রত্যাগম safe_atou64_full(s, 0, ret_u);
}

স্থির অন্তঃস্থ পূর্ণাঙ্ক safe_atoi64(ধ্রুব বর্ণ *s, int64_t *ret_i) {
        assert_cc(পরিমাণ(int64_t) == পরিমাণ(long long));
        প্রত্যাগম safe_atolli(s, (long long*) ret_i);
}

স্থির অন্তঃস্থ পূর্ণাঙ্ক safe_atoux64(ধ্রুব বর্ণ *s, uint64_t *ret) {
        assert_cc(পরিমাণ(int64_t) == পরিমাণ(unsigned long long));
        প্রত্যাগম safe_atollu_full(s, 16, (unsigned long long*) ret);
}

#যদি LONG_MAX == INT_MAX
স্থির অন্তঃস্থ পূর্ণাঙ্ক safe_atolu_full(ধ্রুব বর্ণ *s, unsigned base, unsigned long *ret_u) {
        assert_cc(পরিমাণ(unsigned long) == পরিমাণ(unsigned));
        প্রত্যাগম safe_atou_full(s, base, (unsigned*) ret_u);
}
স্থির অন্তঃস্থ পূর্ণাঙ্ক safe_atoli(ধ্রুব বর্ণ *s, long *ret_u) {
        assert_cc(পরিমাণ(long) == পরিমাণ(পূর্ণাঙ্ক));
        প্রত্যাগম safe_atoi(s, (পূর্ণাঙ্ক*) ret_u);
}
#অন্যথা
স্থির অন্তঃস্থ পূর্ণাঙ্ক safe_atolu_full(ধ্রুব বর্ণ *s, unsigned base, unsigned long *ret_u) {
        assert_cc(পরিমাণ(unsigned long) == পরিমাণ(unsigned long long));
        প্রত্যাগম safe_atollu_full(s, base, (unsigned long long*) ret_u);
}
স্থির অন্তঃস্থ পূর্ণাঙ্ক safe_atoli(ধ্রুব বর্ণ *s, long *ret_u) {
        assert_cc(পরিমাণ(long) == পরিমাণ(long long));
        প্রত্যাগম safe_atolli(s, (long long*) ret_u);
}
#endif

স্থির অন্তঃস্থ পূর্ণাঙ্ক safe_atolu(ধ্রুব বর্ণ *s, unsigned long *ret_u) {
        প্রত্যাগম safe_atolu_full(s, 0, ret_u);
}

#যদি SIZE_MAX == UINT_MAX
স্থির অন্তঃস্থ পূর্ণাঙ্ক safe_atozu(ধ্রুব বর্ণ *s, size_t *ret_u) {
        assert_cc(পরিমাণ(size_t) == পরিমাণ(unsigned));
        প্রত্যাগম safe_atou(s, (unsigned *) ret_u);
}
#অন্যথা
স্থির অন্তঃস্থ পূর্ণাঙ্ক safe_atozu(ধ্রুব বর্ণ *s, size_t *ret_u) {
        assert_cc(পরিমাণ(size_t) == পরিমাণ(unsigned long));
        প্রত্যাগম safe_atolu(s, ret_u);
}
#endif

পূর্ণাঙ্ক safe_atod(ধ্রুব বর্ণ *s, দ্বিগুণ *ret_d);

পূর্ণাঙ্ক parse_fractional_part_u(ধ্রুব বর্ণ **p, size_t digits, unsigned *res);

পূর্ণাঙ্ক parse_nice(ধ্রুব বর্ণ *s, পূর্ণাঙ্ক *ret);

পূর্ণাঙ্ক parse_ip_port(ধ্রুব বর্ণ *s, uint16_t *ret);
পূর্ণাঙ্ক parse_ip_port_range(ধ্রুব বর্ণ *s, uint16_t *low, uint16_t *high, bool allow_zero);

পূর্ণাঙ্ক parse_oom_score_adjust(ধ্রুব বর্ণ *s, পূর্ণাঙ্ক *ret);

/* Implement floating point using fixed integers, to improve performance when
 * calculating load averages. These macros can be used to extract the integer
 * and decimal parts of a value. */
#define LOADAVG_PRECISION_BITS  11
#define LOADAVG_FIXED_POINT_1_0 (1 << LOADAVG_PRECISION_BITS)
#define LOADAVG_INT_SIDE(x)     ((x) >> LOADAVG_PRECISION_BITS)
#define LOADAVG_DECIMAL_SIDE(x) LOADAVG_INT_SIDE(((x) & (LOADAVG_FIXED_POINT_1_0 - 1)) * 100)

/* Given a Linux load average (e.g. decimal number 34.89 where 34 is passed as i and 89 is passed as f), convert it
 * to a loadavg_t. */
পূর্ণাঙ্ক store_loadavg_fixed_point(unsigned long i, unsigned long f, loadavg_t *ret);
পূর্ণাঙ্ক parse_loadavg_fixed_point(ধ্রুব বর্ণ *s, loadavg_t *ret);

bool nft_identifier_valid(ধ্রুব বর্ণ *id);
