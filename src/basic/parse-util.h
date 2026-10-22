/* SPDX-License-Identifier: LGPL-2.1-or-later */
#لارښوونه once

#ضمن "forward.h"

تعريفنوع بینښه اوږد loadavg_t;

صحيح parse_boolean(ثابت حرف *v) خالص;
صحيح parse_tristate_ډک(ثابت حرف *v, ثابت حرف *دریم, صحيح *ret);
ساكن مضمن صحيح parse_tristate(ثابت حرف *v, صحيح *ret) {
        أرجع parse_tristate_ډک(v, عدم, ret);
}
صحيح parse_pid(ثابت حرف *s, pid_t *ret);
صحيح parse_ډېرپېښ(ثابت حرف *s, mode_t *ret);
صحيح parse_ifindex(ثابت حرف *s);
صحيح parse_mtu(صحيح family, ثابت حرف *s, uint32_t *ret);

صحيح parse_اندازه(ثابت حرف *t, uint64_t base, uint64_t *اندازه);
صحيح parse_sector_اندازه(ثابت حرف *t, uint64_t *ret);
صحيح parse_range(ثابت حرف *t, بینښه *ښکتنی, بینښه *پاسنی);
صحيح parse_errno(ثابت حرف *t);
صحيح parse_fd(ثابت حرف *t);
صحيح parse_user_shell(ثابت حرف *s, حرف **ret_sh, bool *ret_copy);

صحيح parse_capability_سېټ(ثابت حرف *s, uint64_t initial, uint64_t *current);

#تعریف_کړه خوندي_ATO_REFUSE_PLUS_MINUS (1U << 30)
#تعریف_کړه خوندي_ATO_REFUSE_LEADING_ZERO (1U << 29)
#تعریف_کړه خوندي_ATO_REFUSE_LEADING_WHITESPACE (1U << 28)
#تعریف_کړه خوندي_ATO_ALL_FLAGS (خوندي_ATO_REFUSE_PLUS_MINUS|خوندي_ATO_REFUSE_LEADING_ZERO|خوندي_ATO_REFUSE_LEADING_WHITESPACE)
#تعریف_کړه خوندي_ATO_ماسک_FLAGS(base) ((base) & ~خوندي_ATO_ALL_FLAGS)

صحيح خوندي_atou_ډک(ثابت حرف *s, بینښه base, بینښه *ret_u);
ساكن مضمن صحيح خوندي_atou(ثابت حرف *s, بینښه *ret_u) {
        أرجع خوندي_atou_ډک(s, 0, ret_u);
}

صحيح خوندي_atou_bounded(ثابت حرف *s, بینښه min, بینښه max, بینښه *ret);

صحيح خوندي_atoi(ثابت حرف *s, صحيح *ret_i);
صحيح خوندي_atolli(ثابت حرف *s, اوږد اوږد *ret_lli);

صحيح خوندي_atou8_ډک(ثابت حرف *s, بینښه base, uint8_t *ret);

ساكن مضمن صحيح خوندي_atou8(ثابت حرف *s, uint8_t *ret) {
        أرجع خوندي_atou8_ډک(s, 0, ret);
}

صحيح خوندي_atou16_ډک(ثابت حرف *s, بینښه base, uint16_t *ret);

ساكن مضمن صحيح خوندي_atou16(ثابت حرف *s, uint16_t *ret) {
        أرجع خوندي_atou16_ډک(s, 0, ret);
}

ساكن مضمن صحيح خوندي_atoux16(ثابت حرف *s, uint16_t *ret) {
        أرجع خوندي_atou16_ډک(s, 16, ret);
}

صحيح خوندي_atoi16(ثابت حرف *s, int16_t *ret);

ساكن مضمن صحيح خوندي_atou32_ډک(ثابت حرف *s, بینښه base, uint32_t *ret_u) {
        assert_cc(حجم(uint32_t) == حجم(بینښه));
        أرجع خوندي_atou_ډک(s, base, (بینښه*) ret_u);
}

ساكن مضمن صحيح خوندي_atou32(ثابت حرف *s, uint32_t *ret_u) {
        أرجع خوندي_atou32_ډک(s, 0, (بینښه*) ret_u);
}

ساكن مضمن صحيح خوندي_atoi32(ثابت حرف *s, int32_t *ret_i) {
        assert_cc(حجم(int32_t) == حجم(صحيح));
        أرجع خوندي_atoi(s, (صحيح*) ret_i);
}

صحيح خوندي_atollu_ډک(ثابت حرف *s, بینښه base, بینښه اوږد اوږد *ret_llu);

ساكن مضمن صحيح خوندي_atollu(ثابت حرف *s, بینښه اوږد اوږد *ret_llu) {
        أرجع خوندي_atollu_ډک(s, 0, ret_llu);
}

ساكن مضمن صحيح خوندي_atou64_ډک(ثابت حرف *s, بینښه base, uint64_t *ret_u) {
        assert_cc(حجم(uint64_t) == حجم(بینښه اوږد اوږد));
        أرجع خوندي_atollu_ډک(s, base, (بینښه اوږد اوږد*) ret_u);
}

ساكن مضمن صحيح خوندي_atou64(ثابت حرف *s, uint64_t *ret_u) {
        أرجع خوندي_atou64_ډک(s, 0, ret_u);
}

ساكن مضمن صحيح خوندي_atoi64(ثابت حرف *s, int64_t *ret_i) {
        assert_cc(حجم(int64_t) == حجم(اوږد اوږد));
        أرجع خوندي_atolli(s, (اوږد اوږد*) ret_i);
}

ساكن مضمن صحيح خوندي_atoux64(ثابت حرف *s, uint64_t *ret) {
        assert_cc(حجم(int64_t) == حجم(بینښه اوږد اوږد));
        أرجع خوندي_atollu_ډک(s, 16, (بینښه اوږد اوږد*) ret);
}

#إذا LONG_MAX == لوی_عدد
ساكن مضمن صحيح خوندي_atolu_ډک(ثابت حرف *s, بینښه base, بینښه اوږد *ret_u) {
        assert_cc(حجم(بینښه اوږد) == حجم(بینښه));
        أرجع خوندي_atou_ډک(s, base, (بینښه*) ret_u);
}
ساكن مضمن صحيح خوندي_atoli(ثابت حرف *s, اوږد *ret_u) {
        assert_cc(حجم(اوږد) == حجم(صحيح));
        أرجع خوندي_atoi(s, (صحيح*) ret_u);
}
#وإلا
ساكن مضمن صحيح خوندي_atolu_ډک(ثابت حرف *s, بینښه base, بینښه اوږد *ret_u) {
        assert_cc(حجم(بینښه اوږد) == حجم(بینښه اوږد اوږد));
        أرجع خوندي_atollu_ډک(s, base, (بینښه اوږد اوږد*) ret_u);
}
ساكن مضمن صحيح خوندي_atoli(ثابت حرف *s, اوږد *ret_u) {
        assert_cc(حجم(اوږد) == حجم(اوږد اوږد));
        أرجع خوندي_atolli(s, (اوږد اوږد*) ret_u);
}
#د_که_پای

ساكن مضمن صحيح خوندي_atolu(ثابت حرف *s, بینښه اوږد *ret_u) {
        أرجع خوندي_atolu_ډک(s, 0, ret_u);
}

#إذا SIZE_MAX == UINT_MAX
ساكن مضمن صحيح خوندي_atozu(ثابت حرف *s, size_t *ret_u) {
        assert_cc(حجم(size_t) == حجم(بینښه));
        أرجع خوندي_atou(s, (بینښه *) ret_u);
}
#وإلا
ساكن مضمن صحيح خوندي_atozu(ثابت حرف *s, size_t *ret_u) {
        assert_cc(حجم(size_t) == حجم(بینښه اوږد));
        أرجع خوندي_atolu(s, ret_u);
}
#د_که_پای

صحيح خوندي_atod(ثابت حرف *s, مزدوج *ret_d);

صحيح parse_fractional_part_u(ثابت حرف **p, size_t digits, بینښه *res);

صحيح parse_nice(ثابت حرف *s, صحيح *ret);

صحيح parse_ip_پورټ(ثابت حرف *s, uint16_t *ret);
صحيح parse_ip_پورټ_range(ثابت حرف *s, uint16_t *low, uint16_t *high, bool allow_zero);

صحيح parse_oom_score_adjust(ثابت حرف *s, صحيح *ret);

/* Implement floating point using fixed integers, to improve performance when
 * calculating load averages. These macros can be used to extract the integer
 * and decimal parts of a value. */
#تعریف_کړه LOADAVG_PRECISION_بټونه  11
#تعریف_کړه LOADAVG_FIXED_ټکی_1_0 (1 << LOADAVG_PRECISION_بټونه)
#تعریف_کړه LOADAVG_INT_SIDE(x)     ((x) >> LOADAVG_PRECISION_بټونه)
#تعریف_کړه LOADAVG_DECIMAL_SIDE(x) LOADAVG_INT_SIDE(((x) & (LOADAVG_FIXED_ټکی_1_0 - 1)) * 100)

/* Given a Linux load average (e.g. decimal number 34.89 where 34 is passed as i and 89 is passed as f), convert it
 * to a loadavg_t. */
صحيح store_loadavg_fixed_ټکی(بینښه اوږد i, بینښه اوږد f, loadavg_t *ret);
صحيح parse_loadavg_fixed_ټکی(ثابت حرف *s, loadavg_t *ret);

bool nft_identifier_valid(ثابت حرف *id);
