/* SPDX-License-Identifier: LGPL-2.1-or-later */
#निर्देशः once

#अन्तर्भाव "forward.h"

प्रकारनिर्देश अचिह्नित दीर्घ loadavg_t;

पूर्णाङ्क parse_boolean(ध्रुव वर्ण *v) विशुद्धम्;
पूर्णाङ्क parse_tristate_पूर्णम्(ध्रुव वर्ण *v, ध्रुव वर्ण *third, पूर्णाङ्क *ret);
स्थिर अन्तःस्थ पूर्णाङ्क parse_tristate(ध्रुव वर्ण *v, पूर्णाङ्क *ret) {
        प्रत्यागम parse_tristate_पूर्णम्(v, रिक्त, ret);
}
पूर्णाङ्क parse_pid(ध्रुव वर्ण *s, pid_t *ret);
पूर्णाङ्क parse_बहुलकम्(ध्रुव वर्ण *s, mode_t *ret);
पूर्णाङ्क parse_ifindex(ध्रुव वर्ण *s);
पूर्णाङ्क parse_mtu(पूर्णाङ्क family, ध्रुव वर्ण *s, uint32_t *ret);

पूर्णाङ्क parse_आकार(ध्रुव वर्ण *t, uint64_t base, uint64_t *आकार);
पूर्णाङ्क parse_sector_आकार(ध्रुव वर्ण *t, uint64_t *ret);
पूर्णाङ्क parse_range(ध्रुव वर्ण *t, अचिह्नित *अधस्तनम्, अचिह्नित *ऊर्ध्वस्थम्);
पूर्णाङ्क parse_errno(ध्रुव वर्ण *t);
पूर्णाङ्क parse_fd(ध्रुव वर्ण *t);
पूर्णाङ्क parse_user_shell(ध्रुव वर्ण *s, वर्ण **ret_sh, bool *ret_copy);

पूर्णाङ्क parse_capability_समुच्चयः(ध्रुव वर्ण *s, uint64_t initial, uint64_t *current);

#परिभाषय SAFE_ATO_REFUSE_PLUS_MINUS (1U << 30)
#परिभाषय SAFE_ATO_REFUSE_LEADING_ZERO (1U << 29)
#परिभाषय SAFE_ATO_REFUSE_LEADING_WHITESPACE (1U << 28)
#परिभाषय SAFE_ATO_ALL_FLAGS (SAFE_ATO_REFUSE_PLUS_MINUS|SAFE_ATO_REFUSE_LEADING_ZERO|SAFE_ATO_REFUSE_LEADING_WHITESPACE)
#परिभाषय SAFE_ATO_आवरणम्_FLAGS(base) ((base) & ~SAFE_ATO_ALL_FLAGS)

पूर्णाङ्क safe_atou_पूर्णम्(ध्रुव वर्ण *s, अचिह्नित base, अचिह्नित *ret_u);
स्थिर अन्तःस्थ पूर्णाङ्क safe_atou(ध्रुव वर्ण *s, अचिह्नित *ret_u) {
        प्रत्यागम safe_atou_पूर्णम्(s, 0, ret_u);
}

पूर्णाङ्क safe_atou_bounded(ध्रुव वर्ण *s, अचिह्नित min, अचिह्नित max, अचिह्नित *ret);

पूर्णाङ्क safe_atoi(ध्रुव वर्ण *s, पूर्णाङ्क *ret_i);
पूर्णाङ्क safe_atolli(ध्रुव वर्ण *s, दीर्घ दीर्घ *ret_lli);

पूर्णाङ्क safe_atou8_पूर्णम्(ध्रुव वर्ण *s, अचिह्नित base, uint8_t *ret);

स्थिर अन्तःस्थ पूर्णाङ्क safe_atou8(ध्रुव वर्ण *s, uint8_t *ret) {
        प्रत्यागम safe_atou8_पूर्णम्(s, 0, ret);
}

पूर्णाङ्क safe_atou16_पूर्णम्(ध्रुव वर्ण *s, अचिह्नित base, uint16_t *ret);

स्थिर अन्तःस्थ पूर्णाङ्क safe_atou16(ध्रुव वर्ण *s, uint16_t *ret) {
        प्रत्यागम safe_atou16_पूर्णम्(s, 0, ret);
}

स्थिर अन्तःस्थ पूर्णाङ्क safe_atoux16(ध्रुव वर्ण *s, uint16_t *ret) {
        प्रत्यागम safe_atou16_पूर्णम्(s, 16, ret);
}

पूर्णाङ्क safe_atoi16(ध्रुव वर्ण *s, int16_t *ret);

स्थिर अन्तःस्थ पूर्णाङ्क safe_atou32_पूर्णम्(ध्रुव वर्ण *s, अचिह्नित base, uint32_t *ret_u) {
        assert_cc(परिमाण(uint32_t) == परिमाण(अचिह्नित));
        प्रत्यागम safe_atou_पूर्णम्(s, base, (अचिह्नित*) ret_u);
}

स्थिर अन्तःस्थ पूर्णाङ्क safe_atou32(ध्रुव वर्ण *s, uint32_t *ret_u) {
        प्रत्यागम safe_atou32_पूर्णम्(s, 0, (अचिह्नित*) ret_u);
}

स्थिर अन्तःस्थ पूर्णाङ्क safe_atoi32(ध्रुव वर्ण *s, int32_t *ret_i) {
        assert_cc(परिमाण(int32_t) == परिमाण(पूर्णाङ्क));
        प्रत्यागम safe_atoi(s, (पूर्णाङ्क*) ret_i);
}

पूर्णाङ्क safe_atollu_पूर्णम्(ध्रुव वर्ण *s, अचिह्नित base, अचिह्नित दीर्घ दीर्घ *ret_llu);

स्थिर अन्तःस्थ पूर्णाङ्क safe_atollu(ध्रुव वर्ण *s, अचिह्नित दीर्घ दीर्घ *ret_llu) {
        प्रत्यागम safe_atollu_पूर्णम्(s, 0, ret_llu);
}

स्थिर अन्तःस्थ पूर्णाङ्क safe_atou64_पूर्णम्(ध्रुव वर्ण *s, अचिह्नित base, uint64_t *ret_u) {
        assert_cc(परिमाण(uint64_t) == परिमाण(अचिह्नित दीर्घ दीर्घ));
        प्रत्यागम safe_atollu_पूर्णम्(s, base, (अचिह्नित दीर्घ दीर्घ*) ret_u);
}

स्थिर अन्तःस्थ पूर्णाङ्क safe_atou64(ध्रुव वर्ण *s, uint64_t *ret_u) {
        प्रत्यागम safe_atou64_पूर्णम्(s, 0, ret_u);
}

स्थिर अन्तःस्थ पूर्णाङ्क safe_atoi64(ध्रुव वर्ण *s, int64_t *ret_i) {
        assert_cc(परिमाण(int64_t) == परिमाण(दीर्घ दीर्घ));
        प्रत्यागम safe_atolli(s, (दीर्घ दीर्घ*) ret_i);
}

स्थिर अन्तःस्थ पूर्णाङ्क safe_atoux64(ध्रुव वर्ण *s, uint64_t *ret) {
        assert_cc(परिमाण(int64_t) == परिमाण(अचिह्नित दीर्घ दीर्घ));
        प्रत्यागम safe_atollu_पूर्णम्(s, 16, (अचिह्नित दीर्घ दीर्घ*) ret);
}

#यदि LONG_MAX == अधिकतम_पूर्णाङ्कः
स्थिर अन्तःस्थ पूर्णाङ्क safe_atolu_पूर्णम्(ध्रुव वर्ण *s, अचिह्नित base, अचिह्नित दीर्घ *ret_u) {
        assert_cc(परिमाण(अचिह्नित दीर्घ) == परिमाण(अचिह्नित));
        प्रत्यागम safe_atou_पूर्णम्(s, base, (अचिह्नित*) ret_u);
}
स्थिर अन्तःस्थ पूर्णाङ्क safe_atoli(ध्रुव वर्ण *s, दीर्घ *ret_u) {
        assert_cc(परिमाण(दीर्घ) == परिमाण(पूर्णाङ्क));
        प्रत्यागम safe_atoi(s, (पूर्णाङ्क*) ret_u);
}
#अन्यथा
स्थिर अन्तःस्थ पूर्णाङ्क safe_atolu_पूर्णम्(ध्रुव वर्ण *s, अचिह्नित base, अचिह्नित दीर्घ *ret_u) {
        assert_cc(परिमाण(अचिह्नित दीर्घ) == परिमाण(अचिह्नित दीर्घ दीर्घ));
        प्रत्यागम safe_atollu_पूर्णम्(s, base, (अचिह्नित दीर्घ दीर्घ*) ret_u);
}
स्थिर अन्तःस्थ पूर्णाङ्क safe_atoli(ध्रुव वर्ण *s, दीर्घ *ret_u) {
        assert_cc(परिमाण(दीर्घ) == परिमाण(दीर्घ दीर्घ));
        प्रत्यागम safe_atolli(s, (दीर्घ दीर्घ*) ret_u);
}
#यदि_अन्तः

स्थिर अन्तःस्थ पूर्णाङ्क safe_atolu(ध्रुव वर्ण *s, अचिह्नित दीर्घ *ret_u) {
        प्रत्यागम safe_atolu_पूर्णम्(s, 0, ret_u);
}

#यदि SIZE_MAX == UINT_MAX
स्थिर अन्तःस्थ पूर्णाङ्क safe_atozu(ध्रुव वर्ण *s, size_t *ret_u) {
        assert_cc(परिमाण(size_t) == परिमाण(अचिह्नित));
        प्रत्यागम safe_atou(s, (अचिह्नित *) ret_u);
}
#अन्यथा
स्थिर अन्तःस्थ पूर्णाङ्क safe_atozu(ध्रुव वर्ण *s, size_t *ret_u) {
        assert_cc(परिमाण(size_t) == परिमाण(अचिह्नित दीर्घ));
        प्रत्यागम safe_atolu(s, ret_u);
}
#यदि_अन्तः

पूर्णाङ्क safe_atod(ध्रुव वर्ण *s, द्विगुण *ret_d);

पूर्णाङ्क parse_fractional_part_u(ध्रुव वर्ण **p, size_t digits, अचिह्नित *res);

पूर्णाङ्क parse_nice(ध्रुव वर्ण *s, पूर्णाङ्क *ret);

पूर्णाङ्क parse_ip_पत्तनम्(ध्रुव वर्ण *s, uint16_t *ret);
पूर्णाङ्क parse_ip_पत्तनम्_range(ध्रुव वर्ण *s, uint16_t *low, uint16_t *high, bool allow_zero);

पूर्णाङ्क parse_oom_score_adjust(ध्रुव वर्ण *s, पूर्णाङ्क *ret);

/* Implement floating point using fixed integers, to improve performance when
 * calculating load averages. These macros can be used to extract the integer
 * and decimal parts of a value. */
#परिभाषय LOADAVG_PRECISION_द्वयङ्काः  11
#परिभाषय LOADAVG_FIXED_POINT_1_0 (1 << LOADAVG_PRECISION_द्वयङ्काः)
#परिभाषय LOADAVG_INT_SIDE(x)     ((x) >> LOADAVG_PRECISION_द्वयङ्काः)
#परिभाषय LOADAVG_DECIMAL_SIDE(x) LOADAVG_INT_SIDE(((x) & (LOADAVG_FIXED_POINT_1_0 - 1)) * 100)

/* Given a Linux load average (e.g. decimal number 34.89 where 34 is passed as i and 89 is passed as f), convert it
 * to a loadavg_t. */
पूर्णाङ्क store_loadavg_fixed_point(अचिह्नित दीर्घ i, अचिह्नित दीर्घ f, loadavg_t *ret);
पूर्णाङ्क parse_loadavg_fixed_point(ध्रुव वर्ण *s, loadavg_t *ret);

bool nft_identifier_valid(ध्रुव वर्ण *id);
