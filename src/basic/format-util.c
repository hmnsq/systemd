/* SPDX-License-Identifier: LGPL-2.1-or-later */

#شامل "format-util.h"

حرف* format_بائٹیں_بھرا(حرف *buf, size_t l, uint64_t t, Format_بائٹیں_Flag flag) {
        قسمتعریف ساخت {
                ثابت حرف *suffix;
                uint64_t ضربی_عدد;
        } suffix_جدول;
        ساکن ثابت suffix_جدول جدول_iec[] = {
                { "E", UINT64_C(1024)*UINT64_C(1024)*UINT64_C(1024)*UINT64_C(1024)*UINT64_C(1024)*UINT64_C(1024) },
                { "P", UINT64_C(1024)*UINT64_C(1024)*UINT64_C(1024)*UINT64_C(1024)*UINT64_C(1024) },
                { "T", UINT64_C(1024)*UINT64_C(1024)*UINT64_C(1024)*UINT64_C(1024) },
                { "G", UINT64_C(1024)*UINT64_C(1024)*UINT64_C(1024) },
                { "M", UINT64_C(1024)*UINT64_C(1024) },
                { "K", UINT64_C(1024) },
        }, جدول_si[] = {
                { "E", UINT64_C(1000)*UINT64_C(1000)*UINT64_C(1000)*UINT64_C(1000)*UINT64_C(1000)*UINT64_C(1000) },
                { "P", UINT64_C(1000)*UINT64_C(1000)*UINT64_C(1000)*UINT64_C(1000)*UINT64_C(1000) },
                { "T", UINT64_C(1000)*UINT64_C(1000)*UINT64_C(1000)*UINT64_C(1000) },
                { "G", UINT64_C(1000)*UINT64_C(1000)*UINT64_C(1000) },
                { "M", UINT64_C(1000)*UINT64_C(1000) },
                { "K", UINT64_C(1000) },
        };
        ثابت suffix_جدول *جدول;
        size_t n;

        أكد(buf);
        assert_cc(ELEMENTSOF(جدول_iec) == ELEMENTSOF(جدول_si));

        اگر (t == UINT64_MAX)
                واپس تہی;

        جدول = flag & FORMAT_بائٹیں_USE_IEC ? جدول_iec : جدول_si;
        n = ELEMENTSOF(جدول_iec);

        برائے (size_t i = 0; i < n; i++)
                اگر (t >= جدول[i].ضربی_عدد) {
                        uint64_t remainder = i != n - 1 ?
                                (t / جدول[i + 1].ضربی_عدد * 10 / جدول[n - 1].ضربی_عدد) % 10 :
                                (t * 10 / جدول[i].ضربی_عدد) % 10;

                        اگر (FLAGS_سیٹ(flag, FORMAT_بائٹیں_ALWAYS_نقطہ) ||
                            (FLAGS_سیٹ(flag, FORMAT_بائٹیں_نیچے_نقطہ) && remainder > 0))
                                (خالی) snprintf(buf, l,
                                                "%" PRIu64 ".%" PRIu64 "%s",
                                                t / جدول[i].ضربی_عدد,
                                                remainder,
                                                جدول[i].suffix);
                        ورنہ
                                (خالی) snprintf(buf, l,
                                                "%" PRIu64 "%s",
                                                t / جدول[i].ضربی_عدد,
                                                جدول[i].suffix);

                        جاؤ اختتام;
                }

        (خالی) snprintf(buf, l, "%" PRIu64 "%s", t, flag & FORMAT_بائٹیں_TRAILING_B ? "B" : "");

اختتام:
        buf[l-1] = 0;
        واپس buf;
}
