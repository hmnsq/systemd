/* SPDX-License-Identifier: LGPL-2.1-or-later */
#لارښوونه once

#ضمن "forward.h"

بنية strbuf {
        حرف *buf;
        size_t len;
        بنية strbuf_غوټه *ریښه;

        size_t nodes_شمېر;
        size_t دننه_شمېر;
        size_t دننه_len;
        size_t dedup_len;
        size_t dedup_شمېر;
};

بنية strbuf_غوټه {
        size_t ارزښت_off;
        size_t ارزښت_len;

        بنية strbuf_ماشوم_ثبت *children;
        uint8_t children_شمېر;
};

بنية strbuf_ماشوم_ثبت {
        uint8_t c;
        بنية strbuf_غوټه *ماشوم;
};

بنية strbuf* strbuf_new(فراغ);
ssize_t strbuf_جمع_کړه_string_ډک(بنية strbuf *str, ثابت حرف *s, size_t len);
ساكن مضمن ssize_t strbuf_جمع_کړه_string(بنية strbuf *str, ثابت حرف *s) {
        أرجع strbuf_جمع_کړه_string_ډک(str, s, SIZE_MAX);
}
فراغ strbuf_complete(بنية strbuf *str);
بنية strbuf* strbuf_free(بنية strbuf *str);
DEFINE_TRIVIAL_CLEANUP_FUNC(بنية strbuf*, strbuf_free);
