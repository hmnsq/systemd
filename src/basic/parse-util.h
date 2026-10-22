/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ہدایت once

#شامل "forward.h"

قسمتعریف غیرعلامتی طویل loadavg_t;

صحیح parse_boolean(ثابت حرف *v) خالص;
صحیح parse_tristate_بھرا(ثابت حرف *v, ثابت حرف *تیسرا, صحیح *ret);
ساکن اندرونی صحیح parse_tristate(ثابت حرف *v, صحیح *ret) {
        واپس parse_tristate_بھرا(v, تہی, ret);
}
صحیح parse_pid(ثابت حرف *s, pid_t *ret);
صحیح parse_منوال(ثابت حرف *s, mode_t *ret);
صحیح parse_ifindex(ثابت حرف *s);
صحیح parse_mtu(صحیح family, ثابت حرف *s, uint32_t *ret);

صحیح parse_سائز(ثابت حرف *t, uint64_t base, uint64_t *سائز);
صحیح parse_sector_سائز(ثابت حرف *t, uint64_t *ret);
صحیح parse_range(ثابت حرف *t, غیرعلامتی *زیریں, غیرعلامتی *بالائی);
صحیح parse_errno(ثابت حرف *t);
صحیح parse_fd(ثابت حرف *t);
صحیح parse_user_shell(ثابت حرف *s, حرف **ret_sh, bool *ret_copy);

صحیح parse_capability_سیٹ(ثابت حرف *s, uint64_t initial, uint64_t *current);

#تعریف_کرو محفوظ_ATO_REFUSE_PLUS_MINUS (1U << 30)
#تعریف_کرو محفوظ_ATO_REFUSE_LEADING_ZERO (1U << 29)
#تعریف_کرو محفوظ_ATO_REFUSE_LEADING_WHITESPACE (1U << 28)
#تعریف_کرو محفوظ_ATO_ALL_FLAGS (محفوظ_ATO_REFUSE_PLUS_MINUS|محفوظ_ATO_REFUSE_LEADING_ZERO|محفوظ_ATO_REFUSE_LEADING_WHITESPACE)
#تعریف_کرو محفوظ_ATO_نقاب_FLAGS(base) ((base) & ~محفوظ_ATO_ALL_FLAGS)

صحیح محفوظ_atou_بھرا(ثابت حرف *s, غیرعلامتی base, غیرعلامتی *ret_u);
ساکن اندرونی صحیح محفوظ_atou(ثابت حرف *s, غیرعلامتی *ret_u) {
        واپس محفوظ_atou_بھرا(s, 0, ret_u);
}

صحیح محفوظ_atou_bounded(ثابت حرف *s, غیرعلامتی min, غیرعلامتی max, غیرعلامتی *ret);

صحیح محفوظ_atoi(ثابت حرف *s, صحیح *ret_i);
صحیح محفوظ_atolli(ثابت حرف *s, طویل طویل *ret_lli);

صحیح محفوظ_atou8_بھرا(ثابت حرف *s, غیرعلامتی base, uint8_t *ret);

ساکن اندرونی صحیح محفوظ_atou8(ثابت حرف *s, uint8_t *ret) {
        واپس محفوظ_atou8_بھرا(s, 0, ret);
}

صحیح محفوظ_atou16_بھرا(ثابت حرف *s, غیرعلامتی base, uint16_t *ret);

ساکن اندرونی صحیح محفوظ_atou16(ثابت حرف *s, uint16_t *ret) {
        واپس محفوظ_atou16_بھرا(s, 0, ret);
}

ساکن اندرونی صحیح محفوظ_atoux16(ثابت حرف *s, uint16_t *ret) {
        واپس محفوظ_atou16_بھرا(s, 16, ret);
}

صحیح محفوظ_atoi16(ثابت حرف *s, int16_t *ret);

ساکن اندرونی صحیح محفوظ_atou32_بھرا(ثابت حرف *s, غیرعلامتی base, uint32_t *ret_u) {
        assert_cc(حجم(uint32_t) == حجم(غیرعلامتی));
        واپس محفوظ_atou_بھرا(s, base, (غیرعلامتی*) ret_u);
}

ساکن اندرونی صحیح محفوظ_atou32(ثابت حرف *s, uint32_t *ret_u) {
        واپس محفوظ_atou32_بھرا(s, 0, (غیرعلامتی*) ret_u);
}

ساکن اندرونی صحیح محفوظ_atoi32(ثابت حرف *s, int32_t *ret_i) {
        assert_cc(حجم(int32_t) == حجم(صحیح));
        واپس محفوظ_atoi(s, (صحیح*) ret_i);
}

صحیح محفوظ_atollu_بھرا(ثابت حرف *s, غیرعلامتی base, غیرعلامتی طویل طویل *ret_llu);

ساکن اندرونی صحیح محفوظ_atollu(ثابت حرف *s, غیرعلامتی طویل طویل *ret_llu) {
        واپس محفوظ_atollu_بھرا(s, 0, ret_llu);
}

ساکن اندرونی صحیح محفوظ_atou64_بھرا(ثابت حرف *s, غیرعلامتی base, uint64_t *ret_u) {
        assert_cc(حجم(uint64_t) == حجم(غیرعلامتی طویل طویل));
        واپس محفوظ_atollu_بھرا(s, base, (غیرعلامتی طویل طویل*) ret_u);
}

ساکن اندرونی صحیح محفوظ_atou64(ثابت حرف *s, uint64_t *ret_u) {
        واپس محفوظ_atou64_بھرا(s, 0, ret_u);
}

ساکن اندرونی صحیح محفوظ_atoi64(ثابت حرف *s, int64_t *ret_i) {
        assert_cc(حجم(int64_t) == حجم(طویل طویل));
        واپس محفوظ_atolli(s, (طویل طویل*) ret_i);
}

ساکن اندرونی صحیح محفوظ_atoux64(ثابت حرف *s, uint64_t *ret) {
        assert_cc(حجم(int64_t) == حجم(غیرعلامتی طویل طویل));
        واپس محفوظ_atollu_بھرا(s, 16, (غیرعلامتی طویل طویل*) ret);
}

#اگر LONG_MAX == زیادہ_سے_زیادہ_عدد
ساکن اندرونی صحیح محفوظ_atolu_بھرا(ثابت حرف *s, غیرعلامتی base, غیرعلامتی طویل *ret_u) {
        assert_cc(حجم(غیرعلامتی طویل) == حجم(غیرعلامتی));
        واپس محفوظ_atou_بھرا(s, base, (غیرعلامتی*) ret_u);
}
ساکن اندرونی صحیح محفوظ_atoli(ثابت حرف *s, طویل *ret_u) {
        assert_cc(حجم(طویل) == حجم(صحیح));
        واپس محفوظ_atoi(s, (صحیح*) ret_u);
}
#ورنہ
ساکن اندرونی صحیح محفوظ_atolu_بھرا(ثابت حرف *s, غیرعلامتی base, غیرعلامتی طویل *ret_u) {
        assert_cc(حجم(غیرعلامتی طویل) == حجم(غیرعلامتی طویل طویل));
        واپس محفوظ_atollu_بھرا(s, base, (غیرعلامتی طویل طویل*) ret_u);
}
ساکن اندرونی صحیح محفوظ_atoli(ثابت حرف *s, طویل *ret_u) {
        assert_cc(حجم(طویل) == حجم(طویل طویل));
        واپس محفوظ_atolli(s, (طویل طویل*) ret_u);
}
#اگر_ختم

ساکن اندرونی صحیح محفوظ_atolu(ثابت حرف *s, غیرعلامتی طویل *ret_u) {
        واپس محفوظ_atolu_بھرا(s, 0, ret_u);
}

#اگر SIZE_MAX == UINT_MAX
ساکن اندرونی صحیح محفوظ_atozu(ثابت حرف *s, size_t *ret_u) {
        assert_cc(حجم(size_t) == حجم(غیرعلامتی));
        واپس محفوظ_atou(s, (غیرعلامتی *) ret_u);
}
#ورنہ
ساکن اندرونی صحیح محفوظ_atozu(ثابت حرف *s, size_t *ret_u) {
        assert_cc(حجم(size_t) == حجم(غیرعلامتی طویل));
        واپس محفوظ_atolu(s, ret_u);
}
#اگر_ختم

صحیح محفوظ_atod(ثابت حرف *s, دوہرا *ret_d);

صحیح parse_fractional_part_u(ثابت حرف **p, size_t digits, غیرعلامتی *res);

صحیح parse_nice(ثابت حرف *s, صحیح *ret);

صحیح parse_ip_پورٹ(ثابت حرف *s, uint16_t *ret);
صحیح parse_ip_پورٹ_range(ثابت حرف *s, uint16_t *low, uint16_t *high, bool allow_zero);

صحیح parse_oom_score_adjust(ثابت حرف *s, صحیح *ret);

/* Implement floating point using fixed integers, to improve performance when
 * calculating load averages. These macros can be used to extract the integer
 * and decimal parts of a value. */
#تعریف_کرو LOADAVG_PRECISION_بٹ  11
#تعریف_کرو LOADAVG_FIXED_نقطہ_1_0 (1 << LOADAVG_PRECISION_بٹ)
#تعریف_کرو LOADAVG_INT_SIDE(x)     ((x) >> LOADAVG_PRECISION_بٹ)
#تعریف_کرو LOADAVG_DECIMAL_SIDE(x) LOADAVG_INT_SIDE(((x) & (LOADAVG_FIXED_نقطہ_1_0 - 1)) * 100)

/* Given a Linux load average (e.g. decimal number 34.89 where 34 is passed as i and 89 is passed as f), convert it
 * to a loadavg_t. */
صحیح store_loadavg_fixed_نقطہ(غیرعلامتی طویل i, غیرعلامتی طویل f, loadavg_t *ret);
صحیح parse_loadavg_fixed_نقطہ(ثابت حرف *s, loadavg_t *ret);

bool nft_identifier_valid(ثابت حرف *id);
