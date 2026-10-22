/* SPDX-License-Identifier: LGPL-2.1-or-later */
#رهنمود once

#ضمن "forward.h"

بنية strbuf {
        حرف *buf;
        size_t len;
        بنية strbuf_گره *ریشه;

        size_t nodes_شمار;
        size_t درون_شمار;
        size_t درون_len;
        size_t dedup_len;
        size_t dedup_شمار;
};

بنية strbuf_گره {
        size_t مقدار_off;
        size_t مقدار_len;

        بنية strbuf_فرزند_مدخل *children;
        uint8_t children_شمار;
};

بنية strbuf_فرزند_مدخل {
        uint8_t c;
        بنية strbuf_گره *فرزند;
};

بنية strbuf* strbuf_new(فراغ);
ssize_t strbuf_جمع_کن_string_پر(بنية strbuf *str, ثابت حرف *s, size_t len);
ساكن مضمن ssize_t strbuf_جمع_کن_string(بنية strbuf *str, ثابت حرف *s) {
        أرجع strbuf_جمع_کن_string_پر(str, s, SIZE_MAX);
}
فراغ strbuf_complete(بنية strbuf *str);
بنية strbuf* strbuf_free(بنية strbuf *str);
DEFINE_TRIVIAL_CLEANUP_FUNC(بنية strbuf*, strbuf_free);
