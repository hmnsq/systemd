/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ "forward.h"

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ loadavg_t;

ਪੂਰਨ parse_boolean(ਅਚਲ ਅੱਖਰ *v) ਸ਼ੁੱਧ;
ਪੂਰਨ parse_tristate_ਭਰਿਆ(ਅਚਲ ਅੱਖਰ *v, ਅਚਲ ਅੱਖਰ *ਤੀਜਾ, ਪੂਰਨ *ret);
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ parse_tristate(ਅਚਲ ਅੱਖਰ *v, ਪੂਰਨ *ret) {
        ਵਾਪਸ parse_tristate_ਭਰਿਆ(v, ਨਿਰਾ, ret);
}
ਪੂਰਨ parse_pid(ਅਚਲ ਅੱਖਰ *s, pid_t *ret);
ਪੂਰਨ parse_ਬਹੁਲਕ(ਅਚਲ ਅੱਖਰ *s, mode_t *ret);
ਪੂਰਨ parse_ifindex(ਅਚਲ ਅੱਖਰ *s);
ਪੂਰਨ parse_mtu(ਪੂਰਨ family, ਅਚਲ ਅੱਖਰ *s, uint32_t *ret);

ਪੂਰਨ parse_ਮਾਪ(ਅਚਲ ਅੱਖਰ *t, uint64_t base, uint64_t *ਮਾਪ);
ਪੂਰਨ parse_sector_ਮਾਪ(ਅਚਲ ਅੱਖਰ *t, uint64_t *ret);
ਪੂਰਨ parse_range(ਅਚਲ ਅੱਖਰ *t, ਬਿਨਾਨਿਸ਼ਾਨੀ *ਹੇਠਲਾ, ਬਿਨਾਨਿਸ਼ਾਨੀ *ਉੱਤਲਾ);
ਪੂਰਨ parse_errno(ਅਚਲ ਅੱਖਰ *t);
ਪੂਰਨ parse_fd(ਅਚਲ ਅੱਖਰ *t);
ਪੂਰਨ parse_user_shell(ਅਚਲ ਅੱਖਰ *s, ਅੱਖਰ **ret_sh, bool *ret_copy);

ਪੂਰਨ parse_capability_ਸੈੱਟ(ਅਚਲ ਅੱਖਰ *s, uint64_t initial, uint64_t *current);

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸੁਰੱਖਿਅਤ_ATO_REFUSE_PLUS_MINUS (1U << 30)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸੁਰੱਖਿਅਤ_ATO_REFUSE_LEADING_ZERO (1U << 29)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸੁਰੱਖਿਅਤ_ATO_REFUSE_LEADING_WHITESPACE (1U << 28)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸੁਰੱਖਿਅਤ_ATO_ALL_FLAGS (ਸੁਰੱਖਿਅਤ_ATO_REFUSE_PLUS_MINUS|ਸੁਰੱਖਿਅਤ_ATO_REFUSE_LEADING_ZERO|ਸੁਰੱਖਿਅਤ_ATO_REFUSE_LEADING_WHITESPACE)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸੁਰੱਖਿਅਤ_ATO_ਨਕਾਬ_FLAGS(base) ((base) & ~ਸੁਰੱਖਿਅਤ_ATO_ALL_FLAGS)

ਪੂਰਨ ਸੁਰੱਖਿਅਤ_atou_ਭਰਿਆ(ਅਚਲ ਅੱਖਰ *s, ਬਿਨਾਨਿਸ਼ਾਨੀ base, ਬਿਨਾਨਿਸ਼ਾਨੀ *ret_u);
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ ਸੁਰੱਖਿਅਤ_atou(ਅਚਲ ਅੱਖਰ *s, ਬਿਨਾਨਿਸ਼ਾਨੀ *ret_u) {
        ਵਾਪਸ ਸੁਰੱਖਿਅਤ_atou_ਭਰਿਆ(s, 0, ret_u);
}

ਪੂਰਨ ਸੁਰੱਖਿਅਤ_atou_bounded(ਅਚਲ ਅੱਖਰ *s, ਬਿਨਾਨਿਸ਼ਾਨੀ min, ਬਿਨਾਨਿਸ਼ਾਨੀ max, ਬਿਨਾਨਿਸ਼ਾਨੀ *ret);

ਪੂਰਨ ਸੁਰੱਖਿਅਤ_atoi(ਅਚਲ ਅੱਖਰ *s, ਪੂਰਨ *ret_i);
ਪੂਰਨ ਸੁਰੱਖਿਅਤ_atolli(ਅਚਲ ਅੱਖਰ *s, ਲੰਮਾ ਲੰਮਾ *ret_lli);

ਪੂਰਨ ਸੁਰੱਖਿਅਤ_atou8_ਭਰਿਆ(ਅਚਲ ਅੱਖਰ *s, ਬਿਨਾਨਿਸ਼ਾਨੀ base, uint8_t *ret);

ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ ਸੁਰੱਖਿਅਤ_atou8(ਅਚਲ ਅੱਖਰ *s, uint8_t *ret) {
        ਵਾਪਸ ਸੁਰੱਖਿਅਤ_atou8_ਭਰਿਆ(s, 0, ret);
}

ਪੂਰਨ ਸੁਰੱਖਿਅਤ_atou16_ਭਰਿਆ(ਅਚਲ ਅੱਖਰ *s, ਬਿਨਾਨਿਸ਼ਾਨੀ base, uint16_t *ret);

ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ ਸੁਰੱਖਿਅਤ_atou16(ਅਚਲ ਅੱਖਰ *s, uint16_t *ret) {
        ਵਾਪਸ ਸੁਰੱਖਿਅਤ_atou16_ਭਰਿਆ(s, 0, ret);
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ ਸੁਰੱਖਿਅਤ_atoux16(ਅਚਲ ਅੱਖਰ *s, uint16_t *ret) {
        ਵਾਪਸ ਸੁਰੱਖਿਅਤ_atou16_ਭਰਿਆ(s, 16, ret);
}

ਪੂਰਨ ਸੁਰੱਖਿਅਤ_atoi16(ਅਚਲ ਅੱਖਰ *s, int16_t *ret);

ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ ਸੁਰੱਖਿਅਤ_atou32_ਭਰਿਆ(ਅਚਲ ਅੱਖਰ *s, ਬਿਨਾਨਿਸ਼ਾਨੀ base, uint32_t *ret_u) {
        assert_cc(ਆਕਾਰ(uint32_t) == ਆਕਾਰ(ਬਿਨਾਨਿਸ਼ਾਨੀ));
        ਵਾਪਸ ਸੁਰੱਖਿਅਤ_atou_ਭਰਿਆ(s, base, (ਬਿਨਾਨਿਸ਼ਾਨੀ*) ret_u);
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ ਸੁਰੱਖਿਅਤ_atou32(ਅਚਲ ਅੱਖਰ *s, uint32_t *ret_u) {
        ਵਾਪਸ ਸੁਰੱਖਿਅਤ_atou32_ਭਰਿਆ(s, 0, (ਬਿਨਾਨਿਸ਼ਾਨੀ*) ret_u);
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ ਸੁਰੱਖਿਅਤ_atoi32(ਅਚਲ ਅੱਖਰ *s, int32_t *ret_i) {
        assert_cc(ਆਕਾਰ(int32_t) == ਆਕਾਰ(ਪੂਰਨ));
        ਵਾਪਸ ਸੁਰੱਖਿਅਤ_atoi(s, (ਪੂਰਨ*) ret_i);
}

ਪੂਰਨ ਸੁਰੱਖਿਅਤ_atollu_ਭਰਿਆ(ਅਚਲ ਅੱਖਰ *s, ਬਿਨਾਨਿਸ਼ਾਨੀ base, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਲੰਮਾ *ret_llu);

ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ ਸੁਰੱਖਿਅਤ_atollu(ਅਚਲ ਅੱਖਰ *s, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਲੰਮਾ *ret_llu) {
        ਵਾਪਸ ਸੁਰੱਖਿਅਤ_atollu_ਭਰਿਆ(s, 0, ret_llu);
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ ਸੁਰੱਖਿਅਤ_atou64_ਭਰਿਆ(ਅਚਲ ਅੱਖਰ *s, ਬਿਨਾਨਿਸ਼ਾਨੀ base, uint64_t *ret_u) {
        assert_cc(ਆਕਾਰ(uint64_t) == ਆਕਾਰ(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਲੰਮਾ));
        ਵਾਪਸ ਸੁਰੱਖਿਅਤ_atollu_ਭਰਿਆ(s, base, (ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਲੰਮਾ*) ret_u);
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ ਸੁਰੱਖਿਅਤ_atou64(ਅਚਲ ਅੱਖਰ *s, uint64_t *ret_u) {
        ਵਾਪਸ ਸੁਰੱਖਿਅਤ_atou64_ਭਰਿਆ(s, 0, ret_u);
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ ਸੁਰੱਖਿਅਤ_atoi64(ਅਚਲ ਅੱਖਰ *s, int64_t *ret_i) {
        assert_cc(ਆਕਾਰ(int64_t) == ਆਕਾਰ(ਲੰਮਾ ਲੰਮਾ));
        ਵਾਪਸ ਸੁਰੱਖਿਅਤ_atolli(s, (ਲੰਮਾ ਲੰਮਾ*) ret_i);
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ ਸੁਰੱਖਿਅਤ_atoux64(ਅਚਲ ਅੱਖਰ *s, uint64_t *ret) {
        assert_cc(ਆਕਾਰ(int64_t) == ਆਕਾਰ(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਲੰਮਾ));
        ਵਾਪਸ ਸੁਰੱਖਿਅਤ_atollu_ਭਰਿਆ(s, 16, (ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਲੰਮਾ*) ret);
}

#ਜੇ LONG_MAX == ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ ਸੁਰੱਖਿਅਤ_atolu_ਭਰਿਆ(ਅਚਲ ਅੱਖਰ *s, ਬਿਨਾਨਿਸ਼ਾਨੀ base, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ *ret_u) {
        assert_cc(ਆਕਾਰ(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ) == ਆਕਾਰ(ਬਿਨਾਨਿਸ਼ਾਨੀ));
        ਵਾਪਸ ਸੁਰੱਖਿਅਤ_atou_ਭਰਿਆ(s, base, (ਬਿਨਾਨਿਸ਼ਾਨੀ*) ret_u);
}
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ ਸੁਰੱਖਿਅਤ_atoli(ਅਚਲ ਅੱਖਰ *s, ਲੰਮਾ *ret_u) {
        assert_cc(ਆਕਾਰ(ਲੰਮਾ) == ਆਕਾਰ(ਪੂਰਨ));
        ਵਾਪਸ ਸੁਰੱਖਿਅਤ_atoi(s, (ਪੂਰਨ*) ret_u);
}
#ਨਹੀਂਤਾਂ
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ ਸੁਰੱਖਿਅਤ_atolu_ਭਰਿਆ(ਅਚਲ ਅੱਖਰ *s, ਬਿਨਾਨਿਸ਼ਾਨੀ base, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ *ret_u) {
        assert_cc(ਆਕਾਰ(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ) == ਆਕਾਰ(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਲੰਮਾ));
        ਵਾਪਸ ਸੁਰੱਖਿਅਤ_atollu_ਭਰਿਆ(s, base, (ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਲੰਮਾ*) ret_u);
}
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ ਸੁਰੱਖਿਅਤ_atoli(ਅਚਲ ਅੱਖਰ *s, ਲੰਮਾ *ret_u) {
        assert_cc(ਆਕਾਰ(ਲੰਮਾ) == ਆਕਾਰ(ਲੰਮਾ ਲੰਮਾ));
        ਵਾਪਸ ਸੁਰੱਖਿਅਤ_atolli(s, (ਲੰਮਾ ਲੰਮਾ*) ret_u);
}
#ਜੇ_ਅੰਤ

ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ ਸੁਰੱਖਿਅਤ_atolu(ਅਚਲ ਅੱਖਰ *s, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ *ret_u) {
        ਵਾਪਸ ਸੁਰੱਖਿਅਤ_atolu_ਭਰਿਆ(s, 0, ret_u);
}

#ਜੇ SIZE_MAX == UINT_MAX
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ ਸੁਰੱਖਿਅਤ_atozu(ਅਚਲ ਅੱਖਰ *s, size_t *ret_u) {
        assert_cc(ਆਕਾਰ(size_t) == ਆਕਾਰ(ਬਿਨਾਨਿਸ਼ਾਨੀ));
        ਵਾਪਸ ਸੁਰੱਖਿਅਤ_atou(s, (ਬਿਨਾਨਿਸ਼ਾਨੀ *) ret_u);
}
#ਨਹੀਂਤਾਂ
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ ਸੁਰੱਖਿਅਤ_atozu(ਅਚਲ ਅੱਖਰ *s, size_t *ret_u) {
        assert_cc(ਆਕਾਰ(size_t) == ਆਕਾਰ(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ));
        ਵਾਪਸ ਸੁਰੱਖਿਅਤ_atolu(s, ret_u);
}
#ਜੇ_ਅੰਤ

ਪੂਰਨ ਸੁਰੱਖਿਅਤ_atod(ਅਚਲ ਅੱਖਰ *s, ਦੁੱਗਣਾ *ret_d);

ਪੂਰਨ parse_fractional_part_u(ਅਚਲ ਅੱਖਰ **p, size_t digits, ਬਿਨਾਨਿਸ਼ਾਨੀ *res);

ਪੂਰਨ parse_nice(ਅਚਲ ਅੱਖਰ *s, ਪੂਰਨ *ret);

ਪੂਰਨ parse_ip_ਪੋਰਟ(ਅਚਲ ਅੱਖਰ *s, uint16_t *ret);
ਪੂਰਨ parse_ip_ਪੋਰਟ_range(ਅਚਲ ਅੱਖਰ *s, uint16_t *low, uint16_t *high, bool allow_zero);

ਪੂਰਨ parse_oom_score_adjust(ਅਚਲ ਅੱਖਰ *s, ਪੂਰਨ *ret);

/* Implement floating point using fixed integers, to improve performance when
 * calculating load averages. These macros can be used to extract the integer
 * and decimal parts of a value. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LOADAVG_PRECISION_ਬਿੱਟ  11
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LOADAVG_FIXED_ਬਿੰਦੂ_1_0 (1 << LOADAVG_PRECISION_ਬਿੱਟ)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LOADAVG_INT_SIDE(x)     ((x) >> LOADAVG_PRECISION_ਬਿੱਟ)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LOADAVG_DECIMAL_SIDE(x) LOADAVG_INT_SIDE(((x) & (LOADAVG_FIXED_ਬਿੰਦੂ_1_0 - 1)) * 100)

/* Given a Linux load average (e.g. decimal number 34.89 where 34 is passed as i and 89 is passed as f), convert it
 * to a loadavg_t. */
ਪੂਰਨ store_loadavg_fixed_ਬਿੰਦੂ(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ i, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ f, loadavg_t *ret);
ਪੂਰਨ parse_loadavg_fixed_ਬਿੰਦੂ(ਅਚਲ ਅੱਖਰ *s, loadavg_t *ret);

bool nft_identifier_valid(ਅਚਲ ਅੱਖਰ *id);
