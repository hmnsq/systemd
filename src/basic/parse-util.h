/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#अन्तर्भाव "forward.h"

प्रकारव्याख्या अचिन्हित दीर्घ loadavg_t;

पूर्णांक parse_boolean(स्थिरांक अक्षर *v) _pure_;
पूर्णांक parse_tristate_full(स्थिरांक अक्षर *v, स्थिरांक अक्षर *third, पूर्णांक *ret);
स्थिर ओळीत पूर्णांक parse_tristate(स्थिरांक अक्षर *v, पूर्णांक *ret) {
        परत parse_tristate_full(v, रिक्त, ret);
}
पूर्णांक parse_pid(स्थिरांक अक्षर *s, pid_t *ret);
पूर्णांक parse_mode(स्थिरांक अक्षर *s, mode_t *ret);
पूर्णांक parse_ifindex(स्थिरांक अक्षर *s);
पूर्णांक parse_mtu(पूर्णांक family, स्थिरांक अक्षर *s, uint32_t *ret);

पूर्णांक parse_size(स्थिरांक अक्षर *t, uint64_t base, uint64_t *size);
पूर्णांक parse_sector_size(स्थिरांक अक्षर *t, uint64_t *ret);
पूर्णांक parse_range(स्थिरांक अक्षर *t, अचिन्हित *lower, अचिन्हित *upper);
पूर्णांक parse_errno(स्थिरांक अक्षर *t);
पूर्णांक parse_fd(स्थिरांक अक्षर *t);
पूर्णांक parse_user_shell(स्थिरांक अक्षर *s, अक्षर **ret_sh, bool *ret_copy);

पूर्णांक parse_capability_set(स्थिरांक अक्षर *s, uint64_t initial, uint64_t *current);

#define SAFE_ATO_REFUSE_PLUS_MINUS (1U << 30)
#define SAFE_ATO_REFUSE_LEADING_ZERO (1U << 29)
#define SAFE_ATO_REFUSE_LEADING_WHITESPACE (1U << 28)
#define SAFE_ATO_ALL_FLAGS (SAFE_ATO_REFUSE_PLUS_MINUS|SAFE_ATO_REFUSE_LEADING_ZERO|SAFE_ATO_REFUSE_LEADING_WHITESPACE)
#define SAFE_ATO_MASK_FLAGS(base) ((base) & ~SAFE_ATO_ALL_FLAGS)

पूर्णांक safe_atou_full(स्थिरांक अक्षर *s, अचिन्हित base, अचिन्हित *ret_u);
स्थिर ओळीत पूर्णांक safe_atou(स्थिरांक अक्षर *s, अचिन्हित *ret_u) {
        परत safe_atou_full(s, 0, ret_u);
}

पूर्णांक safe_atou_bounded(स्थिरांक अक्षर *s, अचिन्हित min, अचिन्हित max, अचिन्हित *ret);

पूर्णांक safe_atoi(स्थिरांक अक्षर *s, पूर्णांक *ret_i);
पूर्णांक safe_atolli(स्थिरांक अक्षर *s, दीर्घ दीर्घ *ret_lli);

पूर्णांक safe_atou8_full(स्थिरांक अक्षर *s, अचिन्हित base, uint8_t *ret);

स्थिर ओळीत पूर्णांक safe_atou8(स्थिरांक अक्षर *s, uint8_t *ret) {
        परत safe_atou8_full(s, 0, ret);
}

पूर्णांक safe_atou16_full(स्थिरांक अक्षर *s, अचिन्हित base, uint16_t *ret);

स्थिर ओळीत पूर्णांक safe_atou16(स्थिरांक अक्षर *s, uint16_t *ret) {
        परत safe_atou16_full(s, 0, ret);
}

स्थिर ओळीत पूर्णांक safe_atoux16(स्थिरांक अक्षर *s, uint16_t *ret) {
        परत safe_atou16_full(s, 16, ret);
}

पूर्णांक safe_atoi16(स्थिरांक अक्षर *s, int16_t *ret);

स्थिर ओळीत पूर्णांक safe_atou32_full(स्थिरांक अक्षर *s, अचिन्हित base, uint32_t *ret_u) {
        assert_cc(आकार(uint32_t) == आकार(अचिन्हित));
        परत safe_atou_full(s, base, (अचिन्हित*) ret_u);
}

स्थिर ओळीत पूर्णांक safe_atou32(स्थिरांक अक्षर *s, uint32_t *ret_u) {
        परत safe_atou32_full(s, 0, (अचिन्हित*) ret_u);
}

स्थिर ओळीत पूर्णांक safe_atoi32(स्थिरांक अक्षर *s, int32_t *ret_i) {
        assert_cc(आकार(int32_t) == आकार(पूर्णांक));
        परत safe_atoi(s, (पूर्णांक*) ret_i);
}

पूर्णांक safe_atollu_full(स्थिरांक अक्षर *s, अचिन्हित base, अचिन्हित दीर्घ दीर्घ *ret_llu);

स्थिर ओळीत पूर्णांक safe_atollu(स्थिरांक अक्षर *s, अचिन्हित दीर्घ दीर्घ *ret_llu) {
        परत safe_atollu_full(s, 0, ret_llu);
}

स्थिर ओळीत पूर्णांक safe_atou64_full(स्थिरांक अक्षर *s, अचिन्हित base, uint64_t *ret_u) {
        assert_cc(आकार(uint64_t) == आकार(अचिन्हित दीर्घ दीर्घ));
        परत safe_atollu_full(s, base, (अचिन्हित दीर्घ दीर्घ*) ret_u);
}

स्थिर ओळीत पूर्णांक safe_atou64(स्थिरांक अक्षर *s, uint64_t *ret_u) {
        परत safe_atou64_full(s, 0, ret_u);
}

स्थिर ओळीत पूर्णांक safe_atoi64(स्थिरांक अक्षर *s, int64_t *ret_i) {
        assert_cc(आकार(int64_t) == आकार(दीर्घ दीर्घ));
        परत safe_atolli(s, (दीर्घ दीर्घ*) ret_i);
}

स्थिर ओळीत पूर्णांक safe_atoux64(स्थिरांक अक्षर *s, uint64_t *ret) {
        assert_cc(आकार(int64_t) == आकार(अचिन्हित दीर्घ दीर्घ));
        परत safe_atollu_full(s, 16, (अचिन्हित दीर्घ दीर्घ*) ret);
}

#जर दीर्घ_उच्च == पूर्णांक_उच्च
स्थिर ओळीत पूर्णांक safe_atolu_full(स्थिरांक अक्षर *s, अचिन्हित base, अचिन्हित दीर्घ *ret_u) {
        assert_cc(आकार(अचिन्हित दीर्घ) == आकार(अचिन्हित));
        परत safe_atou_full(s, base, (अचिन्हित*) ret_u);
}
स्थिर ओळीत पूर्णांक safe_atoli(स्थिरांक अक्षर *s, दीर्घ *ret_u) {
        assert_cc(आकार(दीर्घ) == आकार(पूर्णांक));
        परत safe_atoi(s, (पूर्णांक*) ret_u);
}
#अन्यथा
स्थिर ओळीत पूर्णांक safe_atolu_full(स्थिरांक अक्षर *s, अचिन्हित base, अचिन्हित दीर्घ *ret_u) {
        assert_cc(आकार(अचिन्हित दीर्घ) == आकार(अचिन्हित दीर्घ दीर्घ));
        परत safe_atollu_full(s, base, (अचिन्हित दीर्घ दीर्घ*) ret_u);
}
स्थिर ओळीत पूर्णांक safe_atoli(स्थिरांक अक्षर *s, दीर्घ *ret_u) {
        assert_cc(आकार(दीर्घ) == आकार(दीर्घ दीर्घ));
        परत safe_atolli(s, (दीर्घ दीर्घ*) ret_u);
}
#endif

स्थिर ओळीत पूर्णांक safe_atolu(स्थिरांक अक्षर *s, अचिन्हित दीर्घ *ret_u) {
        परत safe_atolu_full(s, 0, ret_u);
}

#जर SIZE_MAX == अच_पूर्णांक_उच्च
स्थिर ओळीत पूर्णांक safe_atozu(स्थिरांक अक्षर *s, माप_प्रकार *ret_u) {
        assert_cc(आकार(माप_प्रकार) == आकार(अचिन्हित));
        परत safe_atou(s, (अचिन्हित *) ret_u);
}
#अन्यथा
स्थिर ओळीत पूर्णांक safe_atozu(स्थिरांक अक्षर *s, माप_प्रकार *ret_u) {
        assert_cc(आकार(माप_प्रकार) == आकार(अचिन्हित दीर्घ));
        परत safe_atolu(s, ret_u);
}
#endif

पूर्णांक safe_atod(स्थिरांक अक्षर *s, द्विगुण *ret_d);

पूर्णांक parse_fractional_part_u(स्थिरांक अक्षर **p, माप_प्रकार digits, अचिन्हित *res);

पूर्णांक parse_nice(स्थिरांक अक्षर *s, पूर्णांक *ret);

पूर्णांक parse_ip_port(स्थिरांक अक्षर *s, uint16_t *ret);
पूर्णांक parse_ip_port_range(स्थिरांक अक्षर *s, uint16_t *low, uint16_t *high, bool allow_zero);

पूर्णांक parse_oom_score_adjust(स्थिरांक अक्षर *s, पूर्णांक *ret);

/* Implement floating point using fixed integers, to improve performance when
 * calculating load averages. These macros can be used to extract the integer
 * and decimal parts of a value. */
#define LOADAVG_PRECISION_BITS  11
#define LOADAVG_FIXED_POINT_1_0 (1 << LOADAVG_PRECISION_BITS)
#define LOADAVG_INT_SIDE(x)     ((x) >> LOADAVG_PRECISION_BITS)
#define LOADAVG_DECIMAL_SIDE(x) LOADAVG_INT_SIDE(((x) & (LOADAVG_FIXED_POINT_1_0 - 1)) * 100)

/* Given a Linux load average (e.g. decimal number 34.89 where 34 is passed as i and 89 is passed as f), convert it
 * to a loadavg_t. */
पूर्णांक store_loadavg_fixed_point(अचिन्हित दीर्घ i, अचिन्हित दीर्घ f, loadavg_t *ret);
पूर्णांक parse_loadavg_fixed_point(स्थिरांक अक्षर *s, loadavg_t *ret);

bool nft_identifier_valid(स्थिरांक अक्षर *id);
