/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن "format-util.h"

حرف* format_بائيٽون_ڀريل(حرف *buf, size_t l, uint64_t t, Format_بائيٽون_Flag flag) {
        تعريفنوع بنية {
                ثابت حرف *suffix;
                uint64_t ضرب_ڪندڙ;
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

        جدول = flag & FORMAT_بائيٽون_USE_IEC ? جدول_iec : جدول_si;
        n = ELEMENTSOF(جدول_iec);

        لكل (size_t i = 0; i < n; i++)
                إذا (t >= جدول[i].ضرب_ڪندڙ) {
                        uint64_t remainder = i != n - 1 ?
                                (t / جدول[i + 1].ضرب_ڪندڙ * 10 / جدول[n - 1].ضرب_ڪندڙ) % 10 :
                                (t * 10 / جدول[i].ضرب_ڪندڙ) % 10;

                        إذا (FLAGS_سيٽ(flag, FORMAT_بائيٽون_ALWAYS_نقطو) ||
                            (FLAGS_سيٽ(flag, FORMAT_بائيٽون_هيٺ_نقطو) && remainder > 0))
                                (فراغ) snprintf(buf, l,
                                                "%" PRIu64 ".%" PRIu64 "%s",
                                                t / جدول[i].ضرب_ڪندڙ,
                                                remainder,
                                                جدول[i].suffix);
                        وإلا
                                (فراغ) snprintf(buf, l,
                                                "%" PRIu64 "%s",
                                                t / جدول[i].ضرب_ڪندڙ,
                                                جدول[i].suffix);

                        اذهب پڄاڻي;
                }

        (فراغ) snprintf(buf, l, "%" PRIu64 "%s", t, flag & FORMAT_بائيٽون_TRAILING_B ? "B" : "");

پڄاڻي:
        buf[l-1] = 0;
        أرجع buf;
}
