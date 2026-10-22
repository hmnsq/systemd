/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#അന്തര്ഭാവ "forward.h"

തരനിർവചനം unsigned long loadavg_t;

പൂർണ്ണസംഖ്യ parse_boolean(സ്ഥിരം അക്ഷരം *v) _pure_;
പൂർണ്ണസംഖ്യ parse_tristate_full(സ്ഥിരം അക്ഷരം *v, സ്ഥിരം അക്ഷരം *third, പൂർണ്ണസംഖ്യ *ret);
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ parse_tristate(സ്ഥിരം അക്ഷരം *v, പൂർണ്ണസംഖ്യ *ret) {
        തിരികെ parse_tristate_full(v, ശൂന്യ, ret);
}
പൂർണ്ണസംഖ്യ parse_pid(സ്ഥിരം അക്ഷരം *s, pid_t *ret);
പൂർണ്ണസംഖ്യ parse_mode(സ്ഥിരം അക്ഷരം *s, mode_t *ret);
പൂർണ്ണസംഖ്യ parse_ifindex(സ്ഥിരം അക്ഷരം *s);
പൂർണ്ണസംഖ്യ parse_mtu(പൂർണ്ണസംഖ്യ family, സ്ഥിരം അക്ഷരം *s, uint32_t *ret);

പൂർണ്ണസംഖ്യ parse_size(സ്ഥിരം അക്ഷരം *t, uint64_t base, uint64_t *size);
പൂർണ്ണസംഖ്യ parse_sector_size(സ്ഥിരം അക്ഷരം *t, uint64_t *ret);
പൂർണ്ണസംഖ്യ parse_range(സ്ഥിരം അക്ഷരം *t, unsigned *lower, unsigned *upper);
പൂർണ്ണസംഖ്യ parse_errno(സ്ഥിരം അക്ഷരം *t);
പൂർണ്ണസംഖ്യ parse_fd(സ്ഥിരം അക്ഷരം *t);
പൂർണ്ണസംഖ്യ parse_user_shell(സ്ഥിരം അക്ഷരം *s, അക്ഷരം **ret_sh, bool *ret_copy);

പൂർണ്ണസംഖ്യ parse_capability_set(സ്ഥിരം അക്ഷരം *s, uint64_t initial, uint64_t *current);

#define SAFE_ATO_REFUSE_PLUS_MINUS (1U << 30)
#define SAFE_ATO_REFUSE_LEADING_ZERO (1U << 29)
#define SAFE_ATO_REFUSE_LEADING_WHITESPACE (1U << 28)
#define SAFE_ATO_ALL_FLAGS (SAFE_ATO_REFUSE_PLUS_MINUS|SAFE_ATO_REFUSE_LEADING_ZERO|SAFE_ATO_REFUSE_LEADING_WHITESPACE)
#define SAFE_ATO_MASK_FLAGS(base) ((base) & ~SAFE_ATO_ALL_FLAGS)

പൂർണ്ണസംഖ്യ safe_atou_full(സ്ഥിരം അക്ഷരം *s, unsigned base, unsigned *ret_u);
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ safe_atou(സ്ഥിരം അക്ഷരം *s, unsigned *ret_u) {
        തിരികെ safe_atou_full(s, 0, ret_u);
}

പൂർണ്ണസംഖ്യ safe_atou_bounded(സ്ഥിരം അക്ഷരം *s, unsigned min, unsigned max, unsigned *ret);

പൂർണ്ണസംഖ്യ safe_atoi(സ്ഥിരം അക്ഷരം *s, പൂർണ്ണസംഖ്യ *ret_i);
പൂർണ്ണസംഖ്യ safe_atolli(സ്ഥിരം അക്ഷരം *s, long long *ret_lli);

പൂർണ്ണസംഖ്യ safe_atou8_full(സ്ഥിരം അക്ഷരം *s, unsigned base, uint8_t *ret);

സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ safe_atou8(സ്ഥിരം അക്ഷരം *s, uint8_t *ret) {
        തിരികെ safe_atou8_full(s, 0, ret);
}

പൂർണ്ണസംഖ്യ safe_atou16_full(സ്ഥിരം അക്ഷരം *s, unsigned base, uint16_t *ret);

സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ safe_atou16(സ്ഥിരം അക്ഷരം *s, uint16_t *ret) {
        തിരികെ safe_atou16_full(s, 0, ret);
}

സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ safe_atoux16(സ്ഥിരം അക്ഷരം *s, uint16_t *ret) {
        തിരികെ safe_atou16_full(s, 16, ret);
}

പൂർണ്ണസംഖ്യ safe_atoi16(സ്ഥിരം അക്ഷരം *s, int16_t *ret);

സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ safe_atou32_full(സ്ഥിരം അക്ഷരം *s, unsigned base, uint32_t *ret_u) {
        assert_cc(വലുപ്പം(uint32_t) == വലുപ്പം(unsigned));
        തിരികെ safe_atou_full(s, base, (unsigned*) ret_u);
}

സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ safe_atou32(സ്ഥിരം അക്ഷരം *s, uint32_t *ret_u) {
        തിരികെ safe_atou32_full(s, 0, (unsigned*) ret_u);
}

സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ safe_atoi32(സ്ഥിരം അക്ഷരം *s, int32_t *ret_i) {
        assert_cc(വലുപ്പം(int32_t) == വലുപ്പം(പൂർണ്ണസംഖ്യ));
        തിരികെ safe_atoi(s, (പൂർണ്ണസംഖ്യ*) ret_i);
}

പൂർണ്ണസംഖ്യ safe_atollu_full(സ്ഥിരം അക്ഷരം *s, unsigned base, unsigned long long *ret_llu);

സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ safe_atollu(സ്ഥിരം അക്ഷരം *s, unsigned long long *ret_llu) {
        തിരികെ safe_atollu_full(s, 0, ret_llu);
}

സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ safe_atou64_full(സ്ഥിരം അക്ഷരം *s, unsigned base, uint64_t *ret_u) {
        assert_cc(വലുപ്പം(uint64_t) == വലുപ്പം(unsigned long long));
        തിരികെ safe_atollu_full(s, base, (unsigned long long*) ret_u);
}

സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ safe_atou64(സ്ഥിരം അക്ഷരം *s, uint64_t *ret_u) {
        തിരികെ safe_atou64_full(s, 0, ret_u);
}

സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ safe_atoi64(സ്ഥിരം അക്ഷരം *s, int64_t *ret_i) {
        assert_cc(വലുപ്പം(int64_t) == വലുപ്പം(long long));
        തിരികെ safe_atolli(s, (long long*) ret_i);
}

സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ safe_atoux64(സ്ഥിരം അക്ഷരം *s, uint64_t *ret) {
        assert_cc(വലുപ്പം(int64_t) == വലുപ്പം(unsigned long long));
        തിരികെ safe_atollu_full(s, 16, (unsigned long long*) ret);
}

#എങ്കിൽ LONG_MAX == INT_MAX
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ safe_atolu_full(സ്ഥിരം അക്ഷരം *s, unsigned base, unsigned long *ret_u) {
        assert_cc(വലുപ്പം(unsigned long) == വലുപ്പം(unsigned));
        തിരികെ safe_atou_full(s, base, (unsigned*) ret_u);
}
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ safe_atoli(സ്ഥിരം അക്ഷരം *s, long *ret_u) {
        assert_cc(വലുപ്പം(long) == വലുപ്പം(പൂർണ്ണസംഖ്യ));
        തിരികെ safe_atoi(s, (പൂർണ്ണസംഖ്യ*) ret_u);
}
#അല്ലെങ്കിൽ
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ safe_atolu_full(സ്ഥിരം അക്ഷരം *s, unsigned base, unsigned long *ret_u) {
        assert_cc(വലുപ്പം(unsigned long) == വലുപ്പം(unsigned long long));
        തിരികെ safe_atollu_full(s, base, (unsigned long long*) ret_u);
}
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ safe_atoli(സ്ഥിരം അക്ഷരം *s, long *ret_u) {
        assert_cc(വലുപ്പം(long) == വലുപ്പം(long long));
        തിരികെ safe_atolli(s, (long long*) ret_u);
}
#endif

സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ safe_atolu(സ്ഥിരം അക്ഷരം *s, unsigned long *ret_u) {
        തിരികെ safe_atolu_full(s, 0, ret_u);
}

#എങ്കിൽ SIZE_MAX == UINT_MAX
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ safe_atozu(സ്ഥിരം അക്ഷരം *s, size_t *ret_u) {
        assert_cc(വലുപ്പം(size_t) == വലുപ്പം(unsigned));
        തിരികെ safe_atou(s, (unsigned *) ret_u);
}
#അല്ലെങ്കിൽ
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ safe_atozu(സ്ഥിരം അക്ഷരം *s, size_t *ret_u) {
        assert_cc(വലുപ്പം(size_t) == വലുപ്പം(unsigned long));
        തിരികെ safe_atolu(s, ret_u);
}
#endif

പൂർണ്ണസംഖ്യ safe_atod(സ്ഥിരം അക്ഷരം *s, ഇരട്ട *ret_d);

പൂർണ്ണസംഖ്യ parse_fractional_part_u(സ്ഥിരം അക്ഷരം **p, size_t digits, unsigned *res);

പൂർണ്ണസംഖ്യ parse_nice(സ്ഥിരം അക്ഷരം *s, പൂർണ്ണസംഖ്യ *ret);

പൂർണ്ണസംഖ്യ parse_ip_port(സ്ഥിരം അക്ഷരം *s, uint16_t *ret);
പൂർണ്ണസംഖ്യ parse_ip_port_range(സ്ഥിരം അക്ഷരം *s, uint16_t *low, uint16_t *high, bool allow_zero);

പൂർണ്ണസംഖ്യ parse_oom_score_adjust(സ്ഥിരം അക്ഷരം *s, പൂർണ്ണസംഖ്യ *ret);

/* Implement floating point using fixed integers, to improve performance when
 * calculating load averages. These macros can be used to extract the integer
 * and decimal parts of a value. */
#define LOADAVG_PRECISION_BITS  11
#define LOADAVG_FIXED_POINT_1_0 (1 << LOADAVG_PRECISION_BITS)
#define LOADAVG_INT_SIDE(x)     ((x) >> LOADAVG_PRECISION_BITS)
#define LOADAVG_DECIMAL_SIDE(x) LOADAVG_INT_SIDE(((x) & (LOADAVG_FIXED_POINT_1_0 - 1)) * 100)

/* Given a Linux load average (e.g. decimal number 34.89 where 34 is passed as i and 89 is passed as f), convert it
 * to a loadavg_t. */
പൂർണ്ണസംഖ്യ store_loadavg_fixed_point(unsigned long i, unsigned long f, loadavg_t *ret);
പൂർണ്ണസംഖ്യ parse_loadavg_fixed_point(സ്ഥിരം അക്ഷരം *s, loadavg_t *ret);

bool nft_identifier_valid(സ്ഥിരം അക്ഷരം *id);
