/* SPDX-License-Identifier: LGPL-2.1-or-later */
#आशय once

#समावेश "forward.h"

प्रकार अचिन्हित दीर्घ loadavg_t;

पूर्णांक parse_boolean(स्थिर अक्षर *v) _pure_;
पूर्णांक parse_tristate_full(स्थिर अक्षर *v, स्थिर अक्षर *third, पूर्णांक *ret);
अटल अंतरभूत पूर्णांक parse_tristate(स्थिर अक्षर *v, पूर्णांक *ret) {
        वापस parse_tristate_full(v, शून्य, ret);
}
पूर्णांक parse_pid(स्थिर अक्षर *s, pid_t *ret);
पूर्णांक parse_mode(स्थिर अक्षर *s, mode_t *ret);
पूर्णांक parse_ifindex(स्थिर अक्षर *s);
पूर्णांक parse_mtu(पूर्णांक family, स्थिर अक्षर *s, uint32_t *ret);

पूर्णांक parse_आकार(स्थिर अक्षर *t, uint64_t base, uint64_t *आकार);
पूर्णांक parse_sector_आकार(स्थिर अक्षर *t, uint64_t *ret);
पूर्णांक parse_range(स्थिर अक्षर *t, अचिन्हित *lower, अचिन्हित *upper);
पूर्णांक parse_errno(स्थिर अक्षर *t);
पूर्णांक parse_fd(स्थिर अक्षर *t);
पूर्णांक parse_user_shell(स्थिर अक्षर *s, अक्षर **ret_sh, bool *ret_copy);

पूर्णांक parse_capability_set(स्थिर अक्षर *s, uint64_t initial, uint64_t *current);

#घोषणा SAFE_ATO_REFUSE_PLUS_MINUS (1U << 30)
#घोषणा SAFE_ATO_REFUSE_LEADING_ZERO (1U << 29)
#घोषणा SAFE_ATO_REFUSE_LEADING_WHITESPACE (1U << 28)
#घोषणा SAFE_ATO_ALL_FLAGS (SAFE_ATO_REFUSE_PLUS_MINUS|SAFE_ATO_REFUSE_LEADING_ZERO|SAFE_ATO_REFUSE_LEADING_WHITESPACE)
#घोषणा SAFE_ATO_MASK_FLAGS(base) ((base) & ~SAFE_ATO_ALL_FLAGS)

पूर्णांक safe_atou_full(स्थिर अक्षर *s, अचिन्हित base, अचिन्हित *ret_u);
अटल अंतरभूत पूर्णांक safe_atou(स्थिर अक्षर *s, अचिन्हित *ret_u) {
        वापस safe_atou_full(s, 0, ret_u);
}

पूर्णांक safe_atou_bounded(स्थिर अक्षर *s, अचिन्हित min, अचिन्हित max, अचिन्हित *ret);

पूर्णांक safe_atoi(स्थिर अक्षर *s, पूर्णांक *ret_i);
पूर्णांक safe_atolli(स्थिर अक्षर *s, दीर्घ दीर्घ *ret_lli);

पूर्णांक safe_atou8_full(स्थिर अक्षर *s, अचिन्हित base, uint8_t *ret);

अटल अंतरभूत पूर्णांक safe_atou8(स्थिर अक्षर *s, uint8_t *ret) {
        वापस safe_atou8_full(s, 0, ret);
}

पूर्णांक safe_atou16_full(स्थिर अक्षर *s, अचिन्हित base, uint16_t *ret);

अटल अंतरभूत पूर्णांक safe_atou16(स्थिर अक्षर *s, uint16_t *ret) {
        वापस safe_atou16_full(s, 0, ret);
}

अटल अंतरभूत पूर्णांक safe_atoux16(स्थिर अक्षर *s, uint16_t *ret) {
        वापस safe_atou16_full(s, 16, ret);
}

पूर्णांक safe_atoi16(स्थिर अक्षर *s, int16_t *ret);

अटल अंतरभूत पूर्णांक safe_atou32_full(स्थिर अक्षर *s, अचिन्हित base, uint32_t *ret_u) {
        assert_cc(माप(uint32_t) == माप(अचिन्हित));
        वापस safe_atou_full(s, base, (अचिन्हित*) ret_u);
}

अटल अंतरभूत पूर्णांक safe_atou32(स्थिर अक्षर *s, uint32_t *ret_u) {
        वापस safe_atou32_full(s, 0, (अचिन्हित*) ret_u);
}

अटल अंतरभूत पूर्णांक safe_atoi32(स्थिर अक्षर *s, int32_t *ret_i) {
        assert_cc(माप(int32_t) == माप(पूर्णांक));
        वापस safe_atoi(s, (पूर्णांक*) ret_i);
}

पूर्णांक safe_atollu_full(स्थिर अक्षर *s, अचिन्हित base, अचिन्हित दीर्घ दीर्घ *ret_llu);

अटल अंतरभूत पूर्णांक safe_atollu(स्थिर अक्षर *s, अचिन्हित दीर्घ दीर्घ *ret_llu) {
        वापस safe_atollu_full(s, 0, ret_llu);
}

अटल अंतरभूत पूर्णांक safe_atou64_full(स्थिर अक्षर *s, अचिन्हित base, uint64_t *ret_u) {
        assert_cc(माप(uint64_t) == माप(अचिन्हित दीर्घ दीर्घ));
        वापस safe_atollu_full(s, base, (अचिन्हित दीर्घ दीर्घ*) ret_u);
}

अटल अंतरभूत पूर्णांक safe_atou64(स्थिर अक्षर *s, uint64_t *ret_u) {
        वापस safe_atou64_full(s, 0, ret_u);
}

अटल अंतरभूत पूर्णांक safe_atoi64(स्थिर अक्षर *s, int64_t *ret_i) {
        assert_cc(माप(int64_t) == माप(दीर्घ दीर्घ));
        वापस safe_atolli(s, (दीर्घ दीर्घ*) ret_i);
}

अटल अंतरभूत पूर्णांक safe_atoux64(स्थिर अक्षर *s, uint64_t *ret) {
        assert_cc(माप(int64_t) == माप(अचिन्हित दीर्घ दीर्घ));
        वापस safe_atollu_full(s, 16, (अचिन्हित दीर्घ दीर्घ*) ret);
}

#अगर दीर्घ_उच्च == पूर्णांक_उच्च
अटल अंतरभूत पूर्णांक safe_atolu_full(स्थिर अक्षर *s, अचिन्हित base, अचिन्हित दीर्घ *ret_u) {
        assert_cc(माप(अचिन्हित दीर्घ) == माप(अचिन्हित));
        वापस safe_atou_full(s, base, (अचिन्हित*) ret_u);
}
अटल अंतरभूत पूर्णांक safe_atoli(स्थिर अक्षर *s, दीर्घ *ret_u) {
        assert_cc(माप(दीर्घ) == माप(पूर्णांक));
        वापस safe_atoi(s, (पूर्णांक*) ret_u);
}
#अन्यथा
अटल अंतरभूत पूर्णांक safe_atolu_full(स्थिर अक्षर *s, अचिन्हित base, अचिन्हित दीर्घ *ret_u) {
        assert_cc(माप(अचिन्हित दीर्घ) == माप(अचिन्हित दीर्घ दीर्घ));
        वापस safe_atollu_full(s, base, (अचिन्हित दीर्घ दीर्घ*) ret_u);
}
अटल अंतरभूत पूर्णांक safe_atoli(स्थिर अक्षर *s, दीर्घ *ret_u) {
        assert_cc(माप(दीर्घ) == माप(दीर्घ दीर्घ));
        वापस safe_atolli(s, (दीर्घ दीर्घ*) ret_u);
}
#पूर्ण_अगर

अटल अंतरभूत पूर्णांक safe_atolu(स्थिर अक्षर *s, अचिन्हित दीर्घ *ret_u) {
        वापस safe_atolu_full(s, 0, ret_u);
}

#अगर SIZE_MAX == अच_पूर्णांक_उच्च
अटल अंतरभूत पूर्णांक safe_atozu(स्थिर अक्षर *s, माप_प्रकार *ret_u) {
        assert_cc(माप(माप_प्रकार) == माप(अचिन्हित));
        वापस safe_atou(s, (अचिन्हित *) ret_u);
}
#अन्यथा
अटल अंतरभूत पूर्णांक safe_atozu(स्थिर अक्षर *s, माप_प्रकार *ret_u) {
        assert_cc(माप(माप_प्रकार) == माप(अचिन्हित दीर्घ));
        वापस safe_atolu(s, ret_u);
}
#पूर्ण_अगर

पूर्णांक safe_atod(स्थिर अक्षर *s, द्विगुन *ret_d);

पूर्णांक parse_fractional_part_u(स्थिर अक्षर **p, माप_प्रकार digits, अचिन्हित *res);

पूर्णांक parse_nice(स्थिर अक्षर *s, पूर्णांक *ret);

पूर्णांक parse_ip_port(स्थिर अक्षर *s, uint16_t *ret);
पूर्णांक parse_ip_port_range(स्थिर अक्षर *s, uint16_t *low, uint16_t *high, bool allow_zero);

पूर्णांक parse_oom_score_adjust(स्थिर अक्षर *s, पूर्णांक *ret);

/* Implement floating point using fixed integers, to improve performance when
 * calculating load averages. These macros can be used to extract the integer
 * and decimal parts of a value. */
#घोषणा LOADAVG_PRECISION_BITS  11
#घोषणा LOADAVG_FIXED_POINT_1_0 (1 << LOADAVG_PRECISION_BITS)
#घोषणा LOADAVG_INT_SIDE(x)     ((x) >> LOADAVG_PRECISION_BITS)
#घोषणा LOADAVG_DECIMAL_SIDE(x) LOADAVG_INT_SIDE(((x) & (LOADAVG_FIXED_POINT_1_0 - 1)) * 100)

/* Given a Linux load average (e.g. decimal number 34.89 where 34 is passed as i and 89 is passed as f), convert it
 * to a loadavg_t. */
पूर्णांक store_loadavg_fixed_point(अचिन्हित दीर्घ i, अचिन्हित दीर्घ f, loadavg_t *ret);
पूर्णांक parse_loadavg_fixed_point(स्थिर अक्षर *s, loadavg_t *ret);

bool nft_identifier_valid(स्थिर अक्षर *id);
