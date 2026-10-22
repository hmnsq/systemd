/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ଅନ୍ତର୍ଭାଵ "forward.h"

ପ୍ରକାରବ୍ୟାଖ୍ୟା unsigned long loadavg_t;

ପୂର୍ଣ୍ଣସଂଖ୍ୟା parse_boolean(ଅଚଳ ଅକ୍ଷର *v) _pure_;
ପୂର୍ଣ୍ଣସଂଖ୍ୟା parse_tristate_full(ଅଚଳ ଅକ୍ଷର *v, ଅଚଳ ଅକ୍ଷର *third, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ret);
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା parse_tristate(ଅଚଳ ଅକ୍ଷର *v, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ret) {
        ଫେରନ୍ତୁ parse_tristate_full(v, ଖାଲି, ret);
}
ପୂର୍ଣ୍ଣସଂଖ୍ୟା parse_pid(ଅଚଳ ଅକ୍ଷର *s, pid_t *ret);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା parse_mode(ଅଚଳ ଅକ୍ଷର *s, mode_t *ret);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା parse_ifindex(ଅଚଳ ଅକ୍ଷର *s);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା parse_mtu(ପୂର୍ଣ୍ଣସଂଖ୍ୟା family, ଅଚଳ ଅକ୍ଷର *s, uint32_t *ret);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା parse_size(ଅଚଳ ଅକ୍ଷର *t, uint64_t base, uint64_t *size);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା parse_sector_size(ଅଚଳ ଅକ୍ଷର *t, uint64_t *ret);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା parse_range(ଅଚଳ ଅକ୍ଷର *t, unsigned *lower, unsigned *upper);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା parse_errno(ଅଚଳ ଅକ୍ଷର *t);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା parse_fd(ଅଚଳ ଅକ୍ଷର *t);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା parse_user_shell(ଅଚଳ ଅକ୍ଷର *s, ଅକ୍ଷର **ret_sh, bool *ret_copy);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା parse_capability_set(ଅଚଳ ଅକ୍ଷର *s, uint64_t initial, uint64_t *current);

#define SAFE_ATO_REFUSE_PLUS_MINUS (1U << 30)
#define SAFE_ATO_REFUSE_LEADING_ZERO (1U << 29)
#define SAFE_ATO_REFUSE_LEADING_WHITESPACE (1U << 28)
#define SAFE_ATO_ALL_FLAGS (SAFE_ATO_REFUSE_PLUS_MINUS|SAFE_ATO_REFUSE_LEADING_ZERO|SAFE_ATO_REFUSE_LEADING_WHITESPACE)
#define SAFE_ATO_MASK_FLAGS(base) ((base) & ~SAFE_ATO_ALL_FLAGS)

ପୂର୍ଣ୍ଣସଂଖ୍ୟା safe_atou_full(ଅଚଳ ଅକ୍ଷର *s, unsigned base, unsigned *ret_u);
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା safe_atou(ଅଚଳ ଅକ୍ଷର *s, unsigned *ret_u) {
        ଫେରନ୍ତୁ safe_atou_full(s, 0, ret_u);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା safe_atou_bounded(ଅଚଳ ଅକ୍ଷର *s, unsigned min, unsigned max, unsigned *ret);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା safe_atoi(ଅଚଳ ଅକ୍ଷର *s, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ret_i);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା safe_atolli(ଅଚଳ ଅକ୍ଷର *s, long long *ret_lli);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା safe_atou8_full(ଅଚଳ ଅକ୍ଷର *s, unsigned base, uint8_t *ret);

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା safe_atou8(ଅଚଳ ଅକ୍ଷର *s, uint8_t *ret) {
        ଫେରନ୍ତୁ safe_atou8_full(s, 0, ret);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା safe_atou16_full(ଅଚଳ ଅକ୍ଷର *s, unsigned base, uint16_t *ret);

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା safe_atou16(ଅଚଳ ଅକ୍ଷର *s, uint16_t *ret) {
        ଫେରନ୍ତୁ safe_atou16_full(s, 0, ret);
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା safe_atoux16(ଅଚଳ ଅକ୍ଷର *s, uint16_t *ret) {
        ଫେରନ୍ତୁ safe_atou16_full(s, 16, ret);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା safe_atoi16(ଅଚଳ ଅକ୍ଷର *s, int16_t *ret);

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା safe_atou32_full(ଅଚଳ ଅକ୍ଷର *s, unsigned base, uint32_t *ret_u) {
        assert_cc(ଆକାର(uint32_t) == ଆକାର(unsigned));
        ଫେରନ୍ତୁ safe_atou_full(s, base, (unsigned*) ret_u);
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା safe_atou32(ଅଚଳ ଅକ୍ଷର *s, uint32_t *ret_u) {
        ଫେରନ୍ତୁ safe_atou32_full(s, 0, (unsigned*) ret_u);
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା safe_atoi32(ଅଚଳ ଅକ୍ଷର *s, int32_t *ret_i) {
        assert_cc(ଆକାର(int32_t) == ଆକାର(ପୂର୍ଣ୍ଣସଂଖ୍ୟା));
        ଫେରନ୍ତୁ safe_atoi(s, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା*) ret_i);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା safe_atollu_full(ଅଚଳ ଅକ୍ଷର *s, unsigned base, unsigned long long *ret_llu);

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା safe_atollu(ଅଚଳ ଅକ୍ଷର *s, unsigned long long *ret_llu) {
        ଫେରନ୍ତୁ safe_atollu_full(s, 0, ret_llu);
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା safe_atou64_full(ଅଚଳ ଅକ୍ଷର *s, unsigned base, uint64_t *ret_u) {
        assert_cc(ଆକାର(uint64_t) == ଆକାର(unsigned long long));
        ଫେରନ୍ତୁ safe_atollu_full(s, base, (unsigned long long*) ret_u);
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା safe_atou64(ଅଚଳ ଅକ୍ଷର *s, uint64_t *ret_u) {
        ଫେରନ୍ତୁ safe_atou64_full(s, 0, ret_u);
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା safe_atoi64(ଅଚଳ ଅକ୍ଷର *s, int64_t *ret_i) {
        assert_cc(ଆକାର(int64_t) == ଆକାର(long long));
        ଫେରନ୍ତୁ safe_atolli(s, (long long*) ret_i);
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା safe_atoux64(ଅଚଳ ଅକ୍ଷର *s, uint64_t *ret) {
        assert_cc(ଆକାର(int64_t) == ଆକାର(unsigned long long));
        ଫେରନ୍ତୁ safe_atollu_full(s, 16, (unsigned long long*) ret);
}

#ଯଦି LONG_MAX == INT_MAX
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା safe_atolu_full(ଅଚଳ ଅକ୍ଷର *s, unsigned base, unsigned long *ret_u) {
        assert_cc(ଆକାର(unsigned long) == ଆକାର(unsigned));
        ଫେରନ୍ତୁ safe_atou_full(s, base, (unsigned*) ret_u);
}
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା safe_atoli(ଅଚଳ ଅକ୍ଷର *s, long *ret_u) {
        assert_cc(ଆକାର(long) == ଆକାର(ପୂର୍ଣ୍ଣସଂଖ୍ୟା));
        ଫେରନ୍ତୁ safe_atoi(s, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା*) ret_u);
}
#ଅନ୍ୟଥା
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା safe_atolu_full(ଅଚଳ ଅକ୍ଷର *s, unsigned base, unsigned long *ret_u) {
        assert_cc(ଆକାର(unsigned long) == ଆକାର(unsigned long long));
        ଫେରନ୍ତୁ safe_atollu_full(s, base, (unsigned long long*) ret_u);
}
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା safe_atoli(ଅଚଳ ଅକ୍ଷର *s, long *ret_u) {
        assert_cc(ଆକାର(long) == ଆକାର(long long));
        ଫେରନ୍ତୁ safe_atolli(s, (long long*) ret_u);
}
#endif

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା safe_atolu(ଅଚଳ ଅକ୍ଷର *s, unsigned long *ret_u) {
        ଫେରନ୍ତୁ safe_atolu_full(s, 0, ret_u);
}

#ଯଦି SIZE_MAX == UINT_MAX
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା safe_atozu(ଅଚଳ ଅକ୍ଷର *s, size_t *ret_u) {
        assert_cc(ଆକାର(size_t) == ଆକାର(unsigned));
        ଫେରନ୍ତୁ safe_atou(s, (unsigned *) ret_u);
}
#ଅନ୍ୟଥା
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା safe_atozu(ଅଚଳ ଅକ୍ଷର *s, size_t *ret_u) {
        assert_cc(ଆକାର(size_t) == ଆକାର(unsigned long));
        ଫେରନ୍ତୁ safe_atolu(s, ret_u);
}
#endif

ପୂର୍ଣ୍ଣସଂଖ୍ୟା safe_atod(ଅଚଳ ଅକ୍ଷର *s, ଦ୍ୱିଗୁଣ *ret_d);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା parse_fractional_part_u(ଅଚଳ ଅକ୍ଷର **p, size_t digits, unsigned *res);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା parse_nice(ଅଚଳ ଅକ୍ଷର *s, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ret);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା parse_ip_port(ଅଚଳ ଅକ୍ଷର *s, uint16_t *ret);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା parse_ip_port_range(ଅଚଳ ଅକ୍ଷର *s, uint16_t *low, uint16_t *high, bool allow_zero);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା parse_oom_score_adjust(ଅଚଳ ଅକ୍ଷର *s, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ret);

/* Implement floating point using fixed integers, to improve performance when
 * calculating load averages. These macros can be used to extract the integer
 * and decimal parts of a value. */
#define LOADAVG_PRECISION_BITS  11
#define LOADAVG_FIXED_POINT_1_0 (1 << LOADAVG_PRECISION_BITS)
#define LOADAVG_INT_SIDE(x)     ((x) >> LOADAVG_PRECISION_BITS)
#define LOADAVG_DECIMAL_SIDE(x) LOADAVG_INT_SIDE(((x) & (LOADAVG_FIXED_POINT_1_0 - 1)) * 100)

/* Given a Linux load average (e.g. decimal number 34.89 where 34 is passed as i and 89 is passed as f), convert it
 * to a loadavg_t. */
ପୂର୍ଣ୍ଣସଂଖ୍ୟା store_loadavg_fixed_point(unsigned long i, unsigned long f, loadavg_t *ret);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା parse_loadavg_fixed_point(ଅଚଳ ଅକ୍ଷର *s, loadavg_t *ret);

bool nft_identifier_valid(ଅଚଳ ଅକ୍ଷର *id);
