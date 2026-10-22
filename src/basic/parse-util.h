/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#અન્તર્ભાવ "forward.h"

પ્રકારવ્યાખ્યા unsigned long loadavg_t;

પૂર્ણાંક parse_boolean(અચલ અક્ષર *v) _pure_;
પૂર્ણાંક parse_tristate_full(અચલ અક્ષર *v, અચલ અક્ષર *third, પૂર્ણાંક *ret);
સ્થિર અંતઃરેખ પૂર્ણાંક parse_tristate(અચલ અક્ષર *v, પૂર્ણાંક *ret) {
        પાછા parse_tristate_full(v, ખાલી, ret);
}
પૂર્ણાંક parse_pid(અચલ અક્ષર *s, pid_t *ret);
પૂર્ણાંક parse_mode(અચલ અક્ષર *s, mode_t *ret);
પૂર્ણાંક parse_ifindex(અચલ અક્ષર *s);
પૂર્ણાંક parse_mtu(પૂર્ણાંક family, અચલ અક્ષર *s, uint32_t *ret);

પૂર્ણાંક parse_size(અચલ અક્ષર *t, uint64_t base, uint64_t *size);
પૂર્ણાંક parse_sector_size(અચલ અક્ષર *t, uint64_t *ret);
પૂર્ણાંક parse_range(અચલ અક્ષર *t, unsigned *lower, unsigned *upper);
પૂર્ણાંક parse_errno(અચલ અક્ષર *t);
પૂર્ણાંક parse_fd(અચલ અક્ષર *t);
પૂર્ણાંક parse_user_shell(અચલ અક્ષર *s, અક્ષર **ret_sh, bool *ret_copy);

પૂર્ણાંક parse_capability_set(અચલ અક્ષર *s, uint64_t initial, uint64_t *current);

#define SAFE_ATO_REFUSE_PLUS_MINUS (1U << 30)
#define SAFE_ATO_REFUSE_LEADING_ZERO (1U << 29)
#define SAFE_ATO_REFUSE_LEADING_WHITESPACE (1U << 28)
#define SAFE_ATO_ALL_FLAGS (SAFE_ATO_REFUSE_PLUS_MINUS|SAFE_ATO_REFUSE_LEADING_ZERO|SAFE_ATO_REFUSE_LEADING_WHITESPACE)
#define SAFE_ATO_MASK_FLAGS(base) ((base) & ~SAFE_ATO_ALL_FLAGS)

પૂર્ણાંક safe_atou_full(અચલ અક્ષર *s, unsigned base, unsigned *ret_u);
સ્થિર અંતઃરેખ પૂર્ણાંક safe_atou(અચલ અક્ષર *s, unsigned *ret_u) {
        પાછા safe_atou_full(s, 0, ret_u);
}

પૂર્ણાંક safe_atou_bounded(અચલ અક્ષર *s, unsigned min, unsigned max, unsigned *ret);

પૂર્ણાંક safe_atoi(અચલ અક્ષર *s, પૂર્ણાંક *ret_i);
પૂર્ણાંક safe_atolli(અચલ અક્ષર *s, long long *ret_lli);

પૂર્ણાંક safe_atou8_full(અચલ અક્ષર *s, unsigned base, uint8_t *ret);

સ્થિર અંતઃરેખ પૂર્ણાંક safe_atou8(અચલ અક્ષર *s, uint8_t *ret) {
        પાછા safe_atou8_full(s, 0, ret);
}

પૂર્ણાંક safe_atou16_full(અચલ અક્ષર *s, unsigned base, uint16_t *ret);

સ્થિર અંતઃરેખ પૂર્ણાંક safe_atou16(અચલ અક્ષર *s, uint16_t *ret) {
        પાછા safe_atou16_full(s, 0, ret);
}

સ્થિર અંતઃરેખ પૂર્ણાંક safe_atoux16(અચલ અક્ષર *s, uint16_t *ret) {
        પાછા safe_atou16_full(s, 16, ret);
}

પૂર્ણાંક safe_atoi16(અચલ અક્ષર *s, int16_t *ret);

સ્થિર અંતઃરેખ પૂર્ણાંક safe_atou32_full(અચલ અક્ષર *s, unsigned base, uint32_t *ret_u) {
        assert_cc(કદ(uint32_t) == કદ(unsigned));
        પાછા safe_atou_full(s, base, (unsigned*) ret_u);
}

સ્થિર અંતઃરેખ પૂર્ણાંક safe_atou32(અચલ અક્ષર *s, uint32_t *ret_u) {
        પાછા safe_atou32_full(s, 0, (unsigned*) ret_u);
}

સ્થિર અંતઃરેખ પૂર્ણાંક safe_atoi32(અચલ અક્ષર *s, int32_t *ret_i) {
        assert_cc(કદ(int32_t) == કદ(પૂર્ણાંક));
        પાછા safe_atoi(s, (પૂર્ણાંક*) ret_i);
}

પૂર્ણાંક safe_atollu_full(અચલ અક્ષર *s, unsigned base, unsigned long long *ret_llu);

સ્થિર અંતઃરેખ પૂર્ણાંક safe_atollu(અચલ અક્ષર *s, unsigned long long *ret_llu) {
        પાછા safe_atollu_full(s, 0, ret_llu);
}

સ્થિર અંતઃરેખ પૂર્ણાંક safe_atou64_full(અચલ અક્ષર *s, unsigned base, uint64_t *ret_u) {
        assert_cc(કદ(uint64_t) == કદ(unsigned long long));
        પાછા safe_atollu_full(s, base, (unsigned long long*) ret_u);
}

સ્થિર અંતઃરેખ પૂર્ણાંક safe_atou64(અચલ અક્ષર *s, uint64_t *ret_u) {
        પાછા safe_atou64_full(s, 0, ret_u);
}

સ્થિર અંતઃરેખ પૂર્ણાંક safe_atoi64(અચલ અક્ષર *s, int64_t *ret_i) {
        assert_cc(કદ(int64_t) == કદ(long long));
        પાછા safe_atolli(s, (long long*) ret_i);
}

સ્થિર અંતઃરેખ પૂર્ણાંક safe_atoux64(અચલ અક્ષર *s, uint64_t *ret) {
        assert_cc(કદ(int64_t) == કદ(unsigned long long));
        પાછા safe_atollu_full(s, 16, (unsigned long long*) ret);
}

#જો LONG_MAX == INT_MAX
સ્થિર અંતઃરેખ પૂર્ણાંક safe_atolu_full(અચલ અક્ષર *s, unsigned base, unsigned long *ret_u) {
        assert_cc(કદ(unsigned long) == કદ(unsigned));
        પાછા safe_atou_full(s, base, (unsigned*) ret_u);
}
સ્થિર અંતઃરેખ પૂર્ણાંક safe_atoli(અચલ અક્ષર *s, long *ret_u) {
        assert_cc(કદ(long) == કદ(પૂર્ણાંક));
        પાછા safe_atoi(s, (પૂર્ણાંક*) ret_u);
}
#નહીંતર
સ્થિર અંતઃરેખ પૂર્ણાંક safe_atolu_full(અચલ અક્ષર *s, unsigned base, unsigned long *ret_u) {
        assert_cc(કદ(unsigned long) == કદ(unsigned long long));
        પાછા safe_atollu_full(s, base, (unsigned long long*) ret_u);
}
સ્થિર અંતઃરેખ પૂર્ણાંક safe_atoli(અચલ અક્ષર *s, long *ret_u) {
        assert_cc(કદ(long) == કદ(long long));
        પાછા safe_atolli(s, (long long*) ret_u);
}
#endif

સ્થિર અંતઃરેખ પૂર્ણાંક safe_atolu(અચલ અક્ષર *s, unsigned long *ret_u) {
        પાછા safe_atolu_full(s, 0, ret_u);
}

#જો SIZE_MAX == UINT_MAX
સ્થિર અંતઃરેખ પૂર્ણાંક safe_atozu(અચલ અક્ષર *s, size_t *ret_u) {
        assert_cc(કદ(size_t) == કદ(unsigned));
        પાછા safe_atou(s, (unsigned *) ret_u);
}
#નહીંતર
સ્થિર અંતઃરેખ પૂર્ણાંક safe_atozu(અચલ અક્ષર *s, size_t *ret_u) {
        assert_cc(કદ(size_t) == કદ(unsigned long));
        પાછા safe_atolu(s, ret_u);
}
#endif

પૂર્ણાંક safe_atod(અચલ અક્ષર *s, દ્વિગુણ *ret_d);

પૂર્ણાંક parse_fractional_part_u(અચલ અક્ષર **p, size_t digits, unsigned *res);

પૂર્ણાંક parse_nice(અચલ અક્ષર *s, પૂર્ણાંક *ret);

પૂર્ણાંક parse_ip_port(અચલ અક્ષર *s, uint16_t *ret);
પૂર્ણાંક parse_ip_port_range(અચલ અક્ષર *s, uint16_t *low, uint16_t *high, bool allow_zero);

પૂર્ણાંક parse_oom_score_adjust(અચલ અક્ષર *s, પૂર્ણાંક *ret);

/* Implement floating point using fixed integers, to improve performance when
 * calculating load averages. These macros can be used to extract the integer
 * and decimal parts of a value. */
#define LOADAVG_PRECISION_BITS  11
#define LOADAVG_FIXED_POINT_1_0 (1 << LOADAVG_PRECISION_BITS)
#define LOADAVG_INT_SIDE(x)     ((x) >> LOADAVG_PRECISION_BITS)
#define LOADAVG_DECIMAL_SIDE(x) LOADAVG_INT_SIDE(((x) & (LOADAVG_FIXED_POINT_1_0 - 1)) * 100)

/* Given a Linux load average (e.g. decimal number 34.89 where 34 is passed as i and 89 is passed as f), convert it
 * to a loadavg_t. */
પૂર્ણાંક store_loadavg_fixed_point(unsigned long i, unsigned long f, loadavg_t *ret);
પૂર્ણાંક parse_loadavg_fixed_point(અચલ અક્ષર *s, loadavg_t *ret);

bool nft_identifier_valid(અચલ અક્ષર *id);
