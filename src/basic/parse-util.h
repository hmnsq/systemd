/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ہدایت once

#ضمن "forward.h"

تعريفنوع بنانشانی لما loadavg_t;

صحيح parse_boolean(ثابت حرف *v) خالص;
صحيح parse_tristate_بھریا(ثابت حرف *v, ثابت حرف *تیجا, صحيح *ret);
ساكن مضمن صحيح parse_tristate(ثابت حرف *v, صحيح *ret) {
        أرجع parse_tristate_بھریا(v, عدم, ret);
}
صحيح parse_pid(ثابت حرف *s, pid_t *ret);
صحيح parse_منوال(ثابت حرف *s, mode_t *ret);
صحيح parse_ifindex(ثابت حرف *s);
صحيح parse_mtu(صحيح family, ثابت حرف *s, uint32_t *ret);

صحيح parse_ناپ(ثابت حرف *t, uint64_t base, uint64_t *ناپ);
صحيح parse_sector_ناپ(ثابت حرف *t, uint64_t *ret);
صحيح parse_range(ثابت حرف *t, بنانشانی *ہیٹھلا, بنانشانی *اتلا);
صحيح parse_errno(ثابت حرف *t);
صحيح parse_fd(ثابت حرف *t);
صحيح parse_user_shell(ثابت حرف *s, حرف **ret_sh, bool *ret_copy);

صحيح parse_capability_سیٹ(ثابت حرف *s, uint64_t initial, uint64_t *current);

#تعریف_کرو محفوظ_ATO_REFUSE_PLUS_MINUS (1U << 30)
#تعریف_کرو محفوظ_ATO_REFUSE_LEADING_ZERO (1U << 29)
#تعریف_کرو محفوظ_ATO_REFUSE_LEADING_WHITESPACE (1U << 28)
#تعریف_کرو محفوظ_ATO_ALL_FLAGS (محفوظ_ATO_REFUSE_PLUS_MINUS|محفوظ_ATO_REFUSE_LEADING_ZERO|محفوظ_ATO_REFUSE_LEADING_WHITESPACE)
#تعریف_کرو محفوظ_ATO_نقاب_FLAGS(base) ((base) & ~محفوظ_ATO_ALL_FLAGS)

صحيح محفوظ_atou_بھریا(ثابت حرف *s, بنانشانی base, بنانشانی *ret_u);
ساكن مضمن صحيح محفوظ_atou(ثابت حرف *s, بنانشانی *ret_u) {
        أرجع محفوظ_atou_بھریا(s, 0, ret_u);
}

صحيح محفوظ_atou_bounded(ثابت حرف *s, بنانشانی min, بنانشانی max, بنانشانی *ret);

صحيح محفوظ_atoi(ثابت حرف *s, صحيح *ret_i);
صحيح محفوظ_atolli(ثابت حرف *s, لما لما *ret_lli);

صحيح محفوظ_atou8_بھریا(ثابت حرف *s, بنانشانی base, uint8_t *ret);

ساكن مضمن صحيح محفوظ_atou8(ثابت حرف *s, uint8_t *ret) {
        أرجع محفوظ_atou8_بھریا(s, 0, ret);
}

صحيح محفوظ_atou16_بھریا(ثابت حرف *s, بنانشانی base, uint16_t *ret);

ساكن مضمن صحيح محفوظ_atou16(ثابت حرف *s, uint16_t *ret) {
        أرجع محفوظ_atou16_بھریا(s, 0, ret);
}

ساكن مضمن صحيح محفوظ_atoux16(ثابت حرف *s, uint16_t *ret) {
        أرجع محفوظ_atou16_بھریا(s, 16, ret);
}

صحيح محفوظ_atoi16(ثابت حرف *s, int16_t *ret);

ساكن مضمن صحيح محفوظ_atou32_بھریا(ثابت حرف *s, بنانشانی base, uint32_t *ret_u) {
        assert_cc(حجم(uint32_t) == حجم(بنانشانی));
        أرجع محفوظ_atou_بھریا(s, base, (بنانشانی*) ret_u);
}

ساكن مضمن صحيح محفوظ_atou32(ثابت حرف *s, uint32_t *ret_u) {
        أرجع محفوظ_atou32_بھریا(s, 0, (بنانشانی*) ret_u);
}

ساكن مضمن صحيح محفوظ_atoi32(ثابت حرف *s, int32_t *ret_i) {
        assert_cc(حجم(int32_t) == حجم(صحيح));
        أرجع محفوظ_atoi(s, (صحيح*) ret_i);
}

صحيح محفوظ_atollu_بھریا(ثابت حرف *s, بنانشانی base, بنانشانی لما لما *ret_llu);

ساكن مضمن صحيح محفوظ_atollu(ثابت حرف *s, بنانشانی لما لما *ret_llu) {
        أرجع محفوظ_atollu_بھریا(s, 0, ret_llu);
}

ساكن مضمن صحيح محفوظ_atou64_بھریا(ثابت حرف *s, بنانشانی base, uint64_t *ret_u) {
        assert_cc(حجم(uint64_t) == حجم(بنانشانی لما لما));
        أرجع محفوظ_atollu_بھریا(s, base, (بنانشانی لما لما*) ret_u);
}

ساكن مضمن صحيح محفوظ_atou64(ثابت حرف *s, uint64_t *ret_u) {
        أرجع محفوظ_atou64_بھریا(s, 0, ret_u);
}

ساكن مضمن صحيح محفوظ_atoi64(ثابت حرف *s, int64_t *ret_i) {
        assert_cc(حجم(int64_t) == حجم(لما لما));
        أرجع محفوظ_atolli(s, (لما لما*) ret_i);
}

ساكن مضمن صحيح محفوظ_atoux64(ثابت حرف *s, uint64_t *ret) {
        assert_cc(حجم(int64_t) == حجم(بنانشانی لما لما));
        أرجع محفوظ_atollu_بھریا(s, 16, (بنانشانی لما لما*) ret);
}

#إذا LONG_MAX == ودھ_توں_ودھ_عدد
ساكن مضمن صحيح محفوظ_atolu_بھریا(ثابت حرف *s, بنانشانی base, بنانشانی لما *ret_u) {
        assert_cc(حجم(بنانشانی لما) == حجم(بنانشانی));
        أرجع محفوظ_atou_بھریا(s, base, (بنانشانی*) ret_u);
}
ساكن مضمن صحيح محفوظ_atoli(ثابت حرف *s, لما *ret_u) {
        assert_cc(حجم(لما) == حجم(صحيح));
        أرجع محفوظ_atoi(s, (صحيح*) ret_u);
}
#وإلا
ساكن مضمن صحيح محفوظ_atolu_بھریا(ثابت حرف *s, بنانشانی base, بنانشانی لما *ret_u) {
        assert_cc(حجم(بنانشانی لما) == حجم(بنانشانی لما لما));
        أرجع محفوظ_atollu_بھریا(s, base, (بنانشانی لما لما*) ret_u);
}
ساكن مضمن صحيح محفوظ_atoli(ثابت حرف *s, لما *ret_u) {
        assert_cc(حجم(لما) == حجم(لما لما));
        أرجع محفوظ_atolli(s, (لما لما*) ret_u);
}
#جے_انت

ساكن مضمن صحيح محفوظ_atolu(ثابت حرف *s, بنانشانی لما *ret_u) {
        أرجع محفوظ_atolu_بھریا(s, 0, ret_u);
}

#إذا SIZE_MAX == UINT_MAX
ساكن مضمن صحيح محفوظ_atozu(ثابت حرف *s, size_t *ret_u) {
        assert_cc(حجم(size_t) == حجم(بنانشانی));
        أرجع محفوظ_atou(s, (بنانشانی *) ret_u);
}
#وإلا
ساكن مضمن صحيح محفوظ_atozu(ثابت حرف *s, size_t *ret_u) {
        assert_cc(حجم(size_t) == حجم(بنانشانی لما));
        أرجع محفوظ_atolu(s, ret_u);
}
#جے_انت

صحيح محفوظ_atod(ثابت حرف *s, مزدوج *ret_d);

صحيح parse_fractional_part_u(ثابت حرف **p, size_t digits, بنانشانی *res);

صحيح parse_nice(ثابت حرف *s, صحيح *ret);

صحيح parse_ip_پورٹ(ثابت حرف *s, uint16_t *ret);
صحيح parse_ip_پورٹ_range(ثابت حرف *s, uint16_t *low, uint16_t *high, bool allow_zero);

صحيح parse_oom_score_adjust(ثابت حرف *s, صحيح *ret);

/* Implement floating point using fixed integers, to improve performance when
 * calculating load averages. These macros can be used to extract the integer
 * and decimal parts of a value. */
#تعریف_کرو LOADAVG_PRECISION_بٹ  11
#تعریف_کرو LOADAVG_FIXED_نقطہ_1_0 (1 << LOADAVG_PRECISION_بٹ)
#تعریف_کرو LOADAVG_INT_SIDE(x)     ((x) >> LOADAVG_PRECISION_بٹ)
#تعریف_کرو LOADAVG_DECIMAL_SIDE(x) LOADAVG_INT_SIDE(((x) & (LOADAVG_FIXED_نقطہ_1_0 - 1)) * 100)

/* Given a Linux load average (e.g. decimal number 34.89 where 34 is passed as i and 89 is passed as f), convert it
 * to a loadavg_t. */
صحيح store_loadavg_fixed_نقطہ(بنانشانی لما i, بنانشانی لما f, loadavg_t *ret);
صحيح parse_loadavg_fixed_نقطہ(ثابت حرف *s, loadavg_t *ret);

bool nft_identifier_valid(ثابت حرف *id);
