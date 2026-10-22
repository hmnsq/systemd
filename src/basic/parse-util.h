/* SPDX-License-Identifier: LGPL-2.1-or-later */
#رهنمود once

#ضمن "forward.h"

تعريفنوع بیعلامت دراز loadavg_t;

صحيح parse_boolean(ثابت حرف *v) خالص;
صحيح parse_tristate_پر(ثابت حرف *v, ثابت حرف *سوم, صحيح *ret);
ساكن مضمن صحيح parse_tristate(ثابت حرف *v, صحيح *ret) {
        أرجع parse_tristate_پر(v, عدم, ret);
}
صحيح parse_pid(ثابت حرف *s, pid_t *ret);
صحيح parse_نما(ثابت حرف *s, mode_t *ret);
صحيح parse_ifindex(ثابت حرف *s);
صحيح parse_mtu(صحيح family, ثابت حرف *s, uint32_t *ret);

صحيح parse_اندازه(ثابت حرف *t, uint64_t base, uint64_t *اندازه);
صحيح parse_sector_اندازه(ثابت حرف *t, uint64_t *ret);
صحيح parse_range(ثابت حرف *t, بیعلامت *پایینی, بیعلامت *فوقانی);
صحيح parse_errno(ثابت حرف *t);
صحيح parse_fd(ثابت حرف *t);
صحيح parse_user_shell(ثابت حرف *s, حرف **ret_sh, bool *ret_copy);

صحيح parse_capability_مجموعه(ثابت حرف *s, uint64_t initial, uint64_t *current);

#تعریف_کن امن_ATO_REFUSE_PLUS_MINUS (1U << 30)
#تعریف_کن امن_ATO_REFUSE_LEADING_ZERO (1U << 29)
#تعریف_کن امن_ATO_REFUSE_LEADING_WHITESPACE (1U << 28)
#تعریف_کن امن_ATO_ALL_FLAGS (امن_ATO_REFUSE_PLUS_MINUS|امن_ATO_REFUSE_LEADING_ZERO|امن_ATO_REFUSE_LEADING_WHITESPACE)
#تعریف_کن امن_ATO_ماسک_FLAGS(base) ((base) & ~امن_ATO_ALL_FLAGS)

صحيح امن_atou_پر(ثابت حرف *s, بیعلامت base, بیعلامت *ret_u);
ساكن مضمن صحيح امن_atou(ثابت حرف *s, بیعلامت *ret_u) {
        أرجع امن_atou_پر(s, 0, ret_u);
}

صحيح امن_atou_bounded(ثابت حرف *s, بیعلامت min, بیعلامت max, بیعلامت *ret);

صحيح امن_atoi(ثابت حرف *s, صحيح *ret_i);
صحيح امن_atolli(ثابت حرف *s, دراز دراز *ret_lli);

صحيح امن_atou8_پر(ثابت حرف *s, بیعلامت base, uint8_t *ret);

ساكن مضمن صحيح امن_atou8(ثابت حرف *s, uint8_t *ret) {
        أرجع امن_atou8_پر(s, 0, ret);
}

صحيح امن_atou16_پر(ثابت حرف *s, بیعلامت base, uint16_t *ret);

ساكن مضمن صحيح امن_atou16(ثابت حرف *s, uint16_t *ret) {
        أرجع امن_atou16_پر(s, 0, ret);
}

ساكن مضمن صحيح امن_atoux16(ثابت حرف *s, uint16_t *ret) {
        أرجع امن_atou16_پر(s, 16, ret);
}

صحيح امن_atoi16(ثابت حرف *s, int16_t *ret);

ساكن مضمن صحيح امن_atou32_پر(ثابت حرف *s, بیعلامت base, uint32_t *ret_u) {
        assert_cc(حجم(uint32_t) == حجم(بیعلامت));
        أرجع امن_atou_پر(s, base, (بیعلامت*) ret_u);
}

ساكن مضمن صحيح امن_atou32(ثابت حرف *s, uint32_t *ret_u) {
        أرجع امن_atou32_پر(s, 0, (بیعلامت*) ret_u);
}

ساكن مضمن صحيح امن_atoi32(ثابت حرف *s, int32_t *ret_i) {
        assert_cc(حجم(int32_t) == حجم(صحيح));
        أرجع امن_atoi(s, (صحيح*) ret_i);
}

صحيح امن_atollu_پر(ثابت حرف *s, بیعلامت base, بیعلامت دراز دراز *ret_llu);

ساكن مضمن صحيح امن_atollu(ثابت حرف *s, بیعلامت دراز دراز *ret_llu) {
        أرجع امن_atollu_پر(s, 0, ret_llu);
}

ساكن مضمن صحيح امن_atou64_پر(ثابت حرف *s, بیعلامت base, uint64_t *ret_u) {
        assert_cc(حجم(uint64_t) == حجم(بیعلامت دراز دراز));
        أرجع امن_atollu_پر(s, base, (بیعلامت دراز دراز*) ret_u);
}

ساكن مضمن صحيح امن_atou64(ثابت حرف *s, uint64_t *ret_u) {
        أرجع امن_atou64_پر(s, 0, ret_u);
}

ساكن مضمن صحيح امن_atoi64(ثابت حرف *s, int64_t *ret_i) {
        assert_cc(حجم(int64_t) == حجم(دراز دراز));
        أرجع امن_atolli(s, (دراز دراز*) ret_i);
}

ساكن مضمن صحيح امن_atoux64(ثابت حرف *s, uint64_t *ret) {
        assert_cc(حجم(int64_t) == حجم(بیعلامت دراز دراز));
        أرجع امن_atollu_پر(s, 16, (بیعلامت دراز دراز*) ret);
}

#إذا LONG_MAX == بیشترین_صحیح
ساكن مضمن صحيح امن_atolu_پر(ثابت حرف *s, بیعلامت base, بیعلامت دراز *ret_u) {
        assert_cc(حجم(بیعلامت دراز) == حجم(بیعلامت));
        أرجع امن_atou_پر(s, base, (بیعلامت*) ret_u);
}
ساكن مضمن صحيح امن_atoli(ثابت حرف *s, دراز *ret_u) {
        assert_cc(حجم(دراز) == حجم(صحيح));
        أرجع امن_atoi(s, (صحيح*) ret_u);
}
#وإلا
ساكن مضمن صحيح امن_atolu_پر(ثابت حرف *s, بیعلامت base, بیعلامت دراز *ret_u) {
        assert_cc(حجم(بیعلامت دراز) == حجم(بیعلامت دراز دراز));
        أرجع امن_atollu_پر(s, base, (بیعلامت دراز دراز*) ret_u);
}
ساكن مضمن صحيح امن_atoli(ثابت حرف *s, دراز *ret_u) {
        assert_cc(حجم(دراز) == حجم(دراز دراز));
        أرجع امن_atolli(s, (دراز دراز*) ret_u);
}
#پایان_اگر

ساكن مضمن صحيح امن_atolu(ثابت حرف *s, بیعلامت دراز *ret_u) {
        أرجع امن_atolu_پر(s, 0, ret_u);
}

#إذا SIZE_MAX == UINT_MAX
ساكن مضمن صحيح امن_atozu(ثابت حرف *s, size_t *ret_u) {
        assert_cc(حجم(size_t) == حجم(بیعلامت));
        أرجع امن_atou(s, (بیعلامت *) ret_u);
}
#وإلا
ساكن مضمن صحيح امن_atozu(ثابت حرف *s, size_t *ret_u) {
        assert_cc(حجم(size_t) == حجم(بیعلامت دراز));
        أرجع امن_atolu(s, ret_u);
}
#پایان_اگر

صحيح امن_atod(ثابت حرف *s, مزدوج *ret_d);

صحيح parse_fractional_part_u(ثابت حرف **p, size_t digits, بیعلامت *res);

صحيح parse_nice(ثابت حرف *s, صحيح *ret);

صحيح parse_ip_پورت(ثابت حرف *s, uint16_t *ret);
صحيح parse_ip_پورت_range(ثابت حرف *s, uint16_t *low, uint16_t *high, bool allow_zero);

صحيح parse_oom_score_adjust(ثابت حرف *s, صحيح *ret);

/* Implement floating point using fixed integers, to improve performance when
 * calculating load averages. These macros can be used to extract the integer
 * and decimal parts of a value. */
#تعریف_کن LOADAVG_PRECISION_بیتها  11
#تعریف_کن LOADAVG_FIXED_نقطه_1_0 (1 << LOADAVG_PRECISION_بیتها)
#تعریف_کن LOADAVG_INT_SIDE(x)     ((x) >> LOADAVG_PRECISION_بیتها)
#تعریف_کن LOADAVG_DECIMAL_SIDE(x) LOADAVG_INT_SIDE(((x) & (LOADAVG_FIXED_نقطه_1_0 - 1)) * 100)

/* Given a Linux load average (e.g. decimal number 34.89 where 34 is passed as i and 89 is passed as f), convert it
 * to a loadavg_t. */
صحيح store_loadavg_fixed_نقطه(بیعلامت دراز i, بیعلامت دراز f, loadavg_t *ret);
صحيح parse_loadavg_fixed_نقطه(ثابت حرف *s, loadavg_t *ret);

bool nft_identifier_valid(ثابت حرف *id);
