/* SPDX-License-Identifier: LGPL-2.1-or-later */
#هدايت once

#ضمن "forward.h"

تعريفنوع بنانشان ڊگهو loadavg_t;

صحيح parse_boolean(ثابت حرف *v) خالص;
صحيح parse_tristate_ڀريل(ثابت حرف *v, ثابت حرف *ٽيون, صحيح *ret);
ساكن مضمن صحيح parse_tristate(ثابت حرف *v, صحيح *ret) {
        أرجع parse_tristate_ڀريل(v, عدم, ret);
}
صحيح parse_pid(ثابت حرف *s, pid_t *ret);
صحيح parse_گهڻائي(ثابت حرف *s, mode_t *ret);
صحيح parse_ifindex(ثابت حرف *s);
صحيح parse_mtu(صحيح family, ثابت حرف *s, uint32_t *ret);

صحيح parse_سائيز(ثابت حرف *t, uint64_t base, uint64_t *سائيز);
صحيح parse_sector_سائيز(ثابت حرف *t, uint64_t *ret);
صحيح parse_range(ثابت حرف *t, بنانشان *هيٺاهون, بنانشان *مٿانهون);
صحيح parse_errno(ثابت حرف *t);
صحيح parse_fd(ثابت حرف *t);
صحيح parse_user_shell(ثابت حرف *s, حرف **ret_sh, bool *ret_copy);

صحيح parse_capability_سيٽ(ثابت حرف *s, uint64_t initial, uint64_t *current);

#وصف_ڪريو محفوظ_ATO_REFUSE_PLUS_MINUS (1U << 30)
#وصف_ڪريو محفوظ_ATO_REFUSE_LEADING_ZERO (1U << 29)
#وصف_ڪريو محفوظ_ATO_REFUSE_LEADING_WHITESPACE (1U << 28)
#وصف_ڪريو محفوظ_ATO_ALL_FLAGS (محفوظ_ATO_REFUSE_PLUS_MINUS|محفوظ_ATO_REFUSE_LEADING_ZERO|محفوظ_ATO_REFUSE_LEADING_WHITESPACE)
#وصف_ڪريو محفوظ_ATO_نقاب_FLAGS(base) ((base) & ~محفوظ_ATO_ALL_FLAGS)

صحيح محفوظ_atou_ڀريل(ثابت حرف *s, بنانشان base, بنانشان *ret_u);
ساكن مضمن صحيح محفوظ_atou(ثابت حرف *s, بنانشان *ret_u) {
        أرجع محفوظ_atou_ڀريل(s, 0, ret_u);
}

صحيح محفوظ_atou_bounded(ثابت حرف *s, بنانشان min, بنانشان max, بنانشان *ret);

صحيح محفوظ_atoi(ثابت حرف *s, صحيح *ret_i);
صحيح محفوظ_atolli(ثابت حرف *s, ڊگهو ڊگهو *ret_lli);

صحيح محفوظ_atou8_ڀريل(ثابت حرف *s, بنانشان base, uint8_t *ret);

ساكن مضمن صحيح محفوظ_atou8(ثابت حرف *s, uint8_t *ret) {
        أرجع محفوظ_atou8_ڀريل(s, 0, ret);
}

صحيح محفوظ_atou16_ڀريل(ثابت حرف *s, بنانشان base, uint16_t *ret);

ساكن مضمن صحيح محفوظ_atou16(ثابت حرف *s, uint16_t *ret) {
        أرجع محفوظ_atou16_ڀريل(s, 0, ret);
}

ساكن مضمن صحيح محفوظ_atoux16(ثابت حرف *s, uint16_t *ret) {
        أرجع محفوظ_atou16_ڀريل(s, 16, ret);
}

صحيح محفوظ_atoi16(ثابت حرف *s, int16_t *ret);

ساكن مضمن صحيح محفوظ_atou32_ڀريل(ثابت حرف *s, بنانشان base, uint32_t *ret_u) {
        assert_cc(حجم(uint32_t) == حجم(بنانشان));
        أرجع محفوظ_atou_ڀريل(s, base, (بنانشان*) ret_u);
}

ساكن مضمن صحيح محفوظ_atou32(ثابت حرف *s, uint32_t *ret_u) {
        أرجع محفوظ_atou32_ڀريل(s, 0, (بنانشان*) ret_u);
}

ساكن مضمن صحيح محفوظ_atoi32(ثابت حرف *s, int32_t *ret_i) {
        assert_cc(حجم(int32_t) == حجم(صحيح));
        أرجع محفوظ_atoi(s, (صحيح*) ret_i);
}

صحيح محفوظ_atollu_ڀريل(ثابت حرف *s, بنانشان base, بنانشان ڊگهو ڊگهو *ret_llu);

ساكن مضمن صحيح محفوظ_atollu(ثابت حرف *s, بنانشان ڊگهو ڊگهو *ret_llu) {
        أرجع محفوظ_atollu_ڀريل(s, 0, ret_llu);
}

ساكن مضمن صحيح محفوظ_atou64_ڀريل(ثابت حرف *s, بنانشان base, uint64_t *ret_u) {
        assert_cc(حجم(uint64_t) == حجم(بنانشان ڊگهو ڊگهو));
        أرجع محفوظ_atollu_ڀريل(s, base, (بنانشان ڊگهو ڊگهو*) ret_u);
}

ساكن مضمن صحيح محفوظ_atou64(ثابت حرف *s, uint64_t *ret_u) {
        أرجع محفوظ_atou64_ڀريل(s, 0, ret_u);
}

ساكن مضمن صحيح محفوظ_atoi64(ثابت حرف *s, int64_t *ret_i) {
        assert_cc(حجم(int64_t) == حجم(ڊگهو ڊگهو));
        أرجع محفوظ_atolli(s, (ڊگهو ڊگهو*) ret_i);
}

ساكن مضمن صحيح محفوظ_atoux64(ثابت حرف *s, uint64_t *ret) {
        assert_cc(حجم(int64_t) == حجم(بنانشان ڊگهو ڊگهو));
        أرجع محفوظ_atollu_ڀريل(s, 16, (بنانشان ڊگهو ڊگهو*) ret);
}

#إذا LONG_MAX == وڌ_کان_وڌ_عدد
ساكن مضمن صحيح محفوظ_atolu_ڀريل(ثابت حرف *s, بنانشان base, بنانشان ڊگهو *ret_u) {
        assert_cc(حجم(بنانشان ڊگهو) == حجم(بنانشان));
        أرجع محفوظ_atou_ڀريل(s, base, (بنانشان*) ret_u);
}
ساكن مضمن صحيح محفوظ_atoli(ثابت حرف *s, ڊگهو *ret_u) {
        assert_cc(حجم(ڊگهو) == حجم(صحيح));
        أرجع محفوظ_atoi(s, (صحيح*) ret_u);
}
#وإلا
ساكن مضمن صحيح محفوظ_atolu_ڀريل(ثابت حرف *s, بنانشان base, بنانشان ڊگهو *ret_u) {
        assert_cc(حجم(بنانشان ڊگهو) == حجم(بنانشان ڊگهو ڊگهو));
        أرجع محفوظ_atollu_ڀريل(s, base, (بنانشان ڊگهو ڊگهو*) ret_u);
}
ساكن مضمن صحيح محفوظ_atoli(ثابت حرف *s, ڊگهو *ret_u) {
        assert_cc(حجم(ڊگهو) == حجم(ڊگهو ڊگهو));
        أرجع محفوظ_atolli(s, (ڊگهو ڊگهو*) ret_u);
}
#جيڪڏهن_پورو

ساكن مضمن صحيح محفوظ_atolu(ثابت حرف *s, بنانشان ڊگهو *ret_u) {
        أرجع محفوظ_atolu_ڀريل(s, 0, ret_u);
}

#إذا SIZE_MAX == UINT_MAX
ساكن مضمن صحيح محفوظ_atozu(ثابت حرف *s, size_t *ret_u) {
        assert_cc(حجم(size_t) == حجم(بنانشان));
        أرجع محفوظ_atou(s, (بنانشان *) ret_u);
}
#وإلا
ساكن مضمن صحيح محفوظ_atozu(ثابت حرف *s, size_t *ret_u) {
        assert_cc(حجم(size_t) == حجم(بنانشان ڊگهو));
        أرجع محفوظ_atolu(s, ret_u);
}
#جيڪڏهن_پورو

صحيح محفوظ_atod(ثابت حرف *s, مزدوج *ret_d);

صحيح parse_fractional_part_u(ثابت حرف **p, size_t digits, بنانشان *res);

صحيح parse_nice(ثابت حرف *s, صحيح *ret);

صحيح parse_ip_پورٽ(ثابت حرف *s, uint16_t *ret);
صحيح parse_ip_پورٽ_range(ثابت حرف *s, uint16_t *low, uint16_t *high, bool allow_zero);

صحيح parse_oom_score_adjust(ثابت حرف *s, صحيح *ret);

/* Implement floating point using fixed integers, to improve performance when
 * calculating load averages. These macros can be used to extract the integer
 * and decimal parts of a value. */
#وصف_ڪريو LOADAVG_PRECISION_بٽ  11
#وصف_ڪريو LOADAVG_FIXED_نقطو_1_0 (1 << LOADAVG_PRECISION_بٽ)
#وصف_ڪريو LOADAVG_INT_SIDE(x)     ((x) >> LOADAVG_PRECISION_بٽ)
#وصف_ڪريو LOADAVG_DECIMAL_SIDE(x) LOADAVG_INT_SIDE(((x) & (LOADAVG_FIXED_نقطو_1_0 - 1)) * 100)

/* Given a Linux load average (e.g. decimal number 34.89 where 34 is passed as i and 89 is passed as f), convert it
 * to a loadavg_t. */
صحيح store_loadavg_fixed_نقطو(بنانشان ڊگهو i, بنانشان ڊگهو f, loadavg_t *ret);
صحيح parse_loadavg_fixed_نقطو(ثابت حرف *s, loadavg_t *ret);

bool nft_identifier_valid(ثابت حرف *id);
