/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن "format-util.h"

حرف* format_بائٹاں_بھریا(حرف *buf, size_t l, uint64_t t, Format_بائٹاں_Flag flag) {
        تعريفنوع بنية {
                ثابت حرف *suffix;
                uint64_t ضارب;
        } suffix_جدول;
        ساكن ثابت suffix_جدول جدول_iec[] = {
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

        إذا (t == UINT64_MAX)
                أرجع عدم;

        جدول = flag & FORMAT_بائٹاں_USE_IEC ? جدول_iec : جدول_si;
        n = ELEMENTSOF(جدول_iec);

        لكل (size_t i = 0; i < n; i++)
                إذا (t >= جدول[i].ضارب) {
                        uint64_t remainder = i != n - 1 ?
                                (t / جدول[i + 1].ضارب * 10 / جدول[n - 1].ضارب) % 10 :
                                (t * 10 / جدول[i].ضارب) % 10;

                        إذا (FLAGS_سیٹ(flag, FORMAT_بائٹاں_ALWAYS_نقطہ) ||
                            (FLAGS_سیٹ(flag, FORMAT_بائٹاں_ہیٹھاں_نقطہ) && remainder > 0))
                                (فراغ) snprintf(buf, l,
                                                "%" PRIu64 ".%" PRIu64 "%s",
                                                t / جدول[i].ضارب,
                                                remainder,
                                                جدول[i].suffix);
                        وإلا
                                (فراغ) snprintf(buf, l,
                                                "%" PRIu64 "%s",
                                                t / جدول[i].ضارب,
                                                جدول[i].suffix);

                        اذهب انت;
                }

        (فراغ) snprintf(buf, l, "%" PRIu64 "%s", t, flag & FORMAT_بائٹاں_TRAILING_B ? "B" : "");

انت:
        buf[l-1] = 0;
        أرجع buf;
}
