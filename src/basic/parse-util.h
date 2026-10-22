/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#अन्तर्भाव "forward.h"

प्रकारनिर्देश अचिन्हित दीर्घ loadavg_t;

पूर्णाङ्क parse_boolean(अचर वर्ण *v) _pure_;
पूर्णाङ्क parse_tristate_full(अचर वर्ण *v, अचर वर्ण *third, पूर्णाङ्क *ret);
स्थिर अन्तःस्थ पूर्णाङ्क parse_tristate(अचर वर्ण *v, पूर्णाङ्क *ret) {
        फर्काऊ parse_tristate_full(v, रिक्त, ret);
}
पूर्णाङ्क parse_pid(अचर वर्ण *s, pid_t *ret);
पूर्णाङ्क parse_mode(अचर वर्ण *s, mode_t *ret);
पूर्णाङ्क parse_ifindex(अचर वर्ण *s);
पूर्णाङ्क parse_mtu(पूर्णाङ्क family, अचर वर्ण *s, uint32_t *ret);

पूर्णाङ्क parse_size(अचर वर्ण *t, uint64_t base, uint64_t *size);
पूर्णाङ्क parse_sector_size(अचर वर्ण *t, uint64_t *ret);
पूर्णाङ्क parse_range(अचर वर्ण *t, अचिन्हित *lower, अचिन्हित *upper);
पूर्णाङ्क parse_errno(अचर वर्ण *t);
पूर्णाङ्क parse_fd(अचर वर्ण *t);
पूर्णाङ्क parse_user_shell(अचर वर्ण *s, वर्ण **ret_sh, bool *ret_copy);

पूर्णाङ्क parse_capability_set(अचर वर्ण *s, uint64_t initial, uint64_t *current);

#define SAFE_ATO_REFUSE_PLUS_MINUS (1U << 30)
#define SAFE_ATO_REFUSE_LEADING_ZERO (1U << 29)
#define SAFE_ATO_REFUSE_LEADING_WHITESPACE (1U << 28)
#define SAFE_ATO_ALL_FLAGS (SAFE_ATO_REFUSE_PLUS_MINUS|SAFE_ATO_REFUSE_LEADING_ZERO|SAFE_ATO_REFUSE_LEADING_WHITESPACE)
#define SAFE_ATO_MASK_FLAGS(base) ((base) & ~SAFE_ATO_ALL_FLAGS)

पूर्णाङ्क safe_atou_full(अचर वर्ण *s, अचिन्हित base, अचिन्हित *ret_u);
स्थिर अन्तःस्थ पूर्णाङ्क safe_atou(अचर वर्ण *s, अचिन्हित *ret_u) {
        फर्काऊ safe_atou_full(s, 0, ret_u);
}

पूर्णाङ्क safe_atou_bounded(अचर वर्ण *s, अचिन्हित min, अचिन्हित max, अचिन्हित *ret);

पूर्णाङ्क safe_atoi(अचर वर्ण *s, पूर्णाङ्क *ret_i);
पूर्णाङ्क safe_atolli(अचर वर्ण *s, दीर्घ दीर्घ *ret_lli);

पूर्णाङ्क safe_atou8_full(अचर वर्ण *s, अचिन्हित base, uint8_t *ret);

स्थिर अन्तःस्थ पूर्णाङ्क safe_atou8(अचर वर्ण *s, uint8_t *ret) {
        फर्काऊ safe_atou8_full(s, 0, ret);
}

पूर्णाङ्क safe_atou16_full(अचर वर्ण *s, अचिन्हित base, uint16_t *ret);

स्थिर अन्तःस्थ पूर्णाङ्क safe_atou16(अचर वर्ण *s, uint16_t *ret) {
        फर्काऊ safe_atou16_full(s, 0, ret);
}

स्थिर अन्तःस्थ पूर्णाङ्क safe_atoux16(अचर वर्ण *s, uint16_t *ret) {
        फर्काऊ safe_atou16_full(s, 16, ret);
}

पूर्णाङ्क safe_atoi16(अचर वर्ण *s, int16_t *ret);

स्थिर अन्तःस्थ पूर्णाङ्क safe_atou32_full(अचर वर्ण *s, अचिन्हित base, uint32_t *ret_u) {
        assert_cc(परिमाण(uint32_t) == परिमाण(अचिन्हित));
        फर्काऊ safe_atou_full(s, base, (अचिन्हित*) ret_u);
}

स्थिर अन्तःस्थ पूर्णाङ्क safe_atou32(अचर वर्ण *s, uint32_t *ret_u) {
        फर्काऊ safe_atou32_full(s, 0, (अचिन्हित*) ret_u);
}

स्थिर अन्तःस्थ पूर्णाङ्क safe_atoi32(अचर वर्ण *s, int32_t *ret_i) {
        assert_cc(परिमाण(int32_t) == परिमाण(पूर्णाङ्क));
        फर्काऊ safe_atoi(s, (पूर्णाङ्क*) ret_i);
}

पूर्णाङ्क safe_atollu_full(अचर वर्ण *s, अचिन्हित base, अचिन्हित दीर्घ दीर्घ *ret_llu);

स्थिर अन्तःस्थ पूर्णाङ्क safe_atollu(अचर वर्ण *s, अचिन्हित दीर्घ दीर्घ *ret_llu) {
        फर्काऊ safe_atollu_full(s, 0, ret_llu);
}

स्थिर अन्तःस्थ पूर्णाङ्क safe_atou64_full(अचर वर्ण *s, अचिन्हित base, uint64_t *ret_u) {
        assert_cc(परिमाण(uint64_t) == परिमाण(अचिन्हित दीर्घ दीर्घ));
        फर्काऊ safe_atollu_full(s, base, (अचिन्हित दीर्घ दीर्घ*) ret_u);
}

स्थिर अन्तःस्थ पूर्णाङ्क safe_atou64(अचर वर्ण *s, uint64_t *ret_u) {
        फर्काऊ safe_atou64_full(s, 0, ret_u);
}

स्थिर अन्तःस्थ पूर्णाङ्क safe_atoi64(अचर वर्ण *s, int64_t *ret_i) {
        assert_cc(परिमाण(int64_t) == परिमाण(दीर्घ दीर्घ));
        फर्काऊ safe_atolli(s, (दीर्घ दीर्घ*) ret_i);
}

स्थिर अन्तःस्थ पूर्णाङ्क safe_atoux64(अचर वर्ण *s, uint64_t *ret) {
        assert_cc(परिमाण(int64_t) == परिमाण(अचिन्हित दीर्घ दीर्घ));
        फर्काऊ safe_atollu_full(s, 16, (अचिन्हित दीर्घ दीर्घ*) ret);
}

#यदि दीर्घ_उच्च == पूर्णांक_उच्च
स्थिर अन्तःस्थ पूर्णाङ्क safe_atolu_full(अचर वर्ण *s, अचिन्हित base, अचिन्हित दीर्घ *ret_u) {
        assert_cc(परिमाण(अचिन्हित दीर्घ) == परिमाण(अचिन्हित));
        फर्काऊ safe_atou_full(s, base, (अचिन्हित*) ret_u);
}
स्थिर अन्तःस्थ पूर्णाङ्क safe_atoli(अचर वर्ण *s, दीर्घ *ret_u) {
        assert_cc(परिमाण(दीर्घ) == परिमाण(पूर्णाङ्क));
        फर्काऊ safe_atoi(s, (पूर्णाङ्क*) ret_u);
}
#अन्यथा
स्थिर अन्तःस्थ पूर्णाङ्क safe_atolu_full(अचर वर्ण *s, अचिन्हित base, अचिन्हित दीर्घ *ret_u) {
        assert_cc(परिमाण(अचिन्हित दीर्घ) == परिमाण(अचिन्हित दीर्घ दीर्घ));
        फर्काऊ safe_atollu_full(s, base, (अचिन्हित दीर्घ दीर्घ*) ret_u);
}
स्थिर अन्तःस्थ पूर्णाङ्क safe_atoli(अचर वर्ण *s, दीर्घ *ret_u) {
        assert_cc(परिमाण(दीर्घ) == परिमाण(दीर्घ दीर्घ));
        फर्काऊ safe_atolli(s, (दीर्घ दीर्घ*) ret_u);
}
#endif

स्थिर अन्तःस्थ पूर्णाङ्क safe_atolu(अचर वर्ण *s, अचिन्हित दीर्घ *ret_u) {
        फर्काऊ safe_atolu_full(s, 0, ret_u);
}

#यदि SIZE_MAX == अच_पूर्णांक_उच्च
स्थिर अन्तःस्थ पूर्णाङ्क safe_atozu(अचर वर्ण *s, माप_प्रकार *ret_u) {
        assert_cc(परिमाण(माप_प्रकार) == परिमाण(अचिन्हित));
        फर्काऊ safe_atou(s, (अचिन्हित *) ret_u);
}
#अन्यथा
स्थिर अन्तःस्थ पूर्णाङ्क safe_atozu(अचर वर्ण *s, माप_प्रकार *ret_u) {
        assert_cc(परिमाण(माप_प्रकार) == परिमाण(अचिन्हित दीर्घ));
        फर्काऊ safe_atolu(s, ret_u);
}
#endif

पूर्णाङ्क safe_atod(अचर वर्ण *s, द्विगुण *ret_d);

पूर्णाङ्क parse_fractional_part_u(अचर वर्ण **p, माप_प्रकार digits, अचिन्हित *res);

पूर्णाङ्क parse_nice(अचर वर्ण *s, पूर्णाङ्क *ret);

पूर्णाङ्क parse_ip_port(अचर वर्ण *s, uint16_t *ret);
पूर्णाङ्क parse_ip_port_range(अचर वर्ण *s, uint16_t *low, uint16_t *high, bool allow_zero);

पूर्णाङ्क parse_oom_score_adjust(अचर वर्ण *s, पूर्णाङ्क *ret);

/* Implement floating point using fixed integers, to improve performance when
 * calculating load averages. These macros can be used to extract the integer
 * and decimal parts of a value. */
#define LOADAVG_PRECISION_BITS  11
#define LOADAVG_FIXED_POINT_1_0 (1 << LOADAVG_PRECISION_BITS)
#define LOADAVG_INT_SIDE(x)     ((x) >> LOADAVG_PRECISION_BITS)
#define LOADAVG_DECIMAL_SIDE(x) LOADAVG_INT_SIDE(((x) & (LOADAVG_FIXED_POINT_1_0 - 1)) * 100)

/* Given a Linux load average (e.g. decimal number 34.89 where 34 is passed as i and 89 is passed as f), convert it
 * to a loadavg_t. */
पूर्णाङ्क store_loadavg_fixed_point(अचिन्हित दीर्घ i, अचिन्हित दीर्घ f, loadavg_t *ret);
पूर्णाङ्क parse_loadavg_fixed_point(अचर वर्ण *s, loadavg_t *ret);

bool nft_identifier_valid(अचर वर्ण *id);
