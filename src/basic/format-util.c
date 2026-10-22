/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাব "format-util.h"

বর্ণ* format_bytes_full(বর্ণ *buf, size_t l, uint64_t t, FormatBytesFlag flag) {
        প্রকারনির্দেশ সংরচনা {
                ধ্রুব বর্ণ *suffix;
                uint64_t factor;
        } suffix_table;
        স্থির ধ্রুব suffix_table table_iec[] = {
                { "E", UINT64_C(1024)*UINT64_C(1024)*UINT64_C(1024)*UINT64_C(1024)*UINT64_C(1024)*UINT64_C(1024) },
                { "P", UINT64_C(1024)*UINT64_C(1024)*UINT64_C(1024)*UINT64_C(1024)*UINT64_C(1024) },
                { "T", UINT64_C(1024)*UINT64_C(1024)*UINT64_C(1024)*UINT64_C(1024) },
                { "G", UINT64_C(1024)*UINT64_C(1024)*UINT64_C(1024) },
                { "M", UINT64_C(1024)*UINT64_C(1024) },
                { "K", UINT64_C(1024) },
        }, table_si[] = {
                { "E", UINT64_C(1000)*UINT64_C(1000)*UINT64_C(1000)*UINT64_C(1000)*UINT64_C(1000)*UINT64_C(1000) },
                { "P", UINT64_C(1000)*UINT64_C(1000)*UINT64_C(1000)*UINT64_C(1000)*UINT64_C(1000) },
                { "T", UINT64_C(1000)*UINT64_C(1000)*UINT64_C(1000)*UINT64_C(1000) },
                { "G", UINT64_C(1000)*UINT64_C(1000)*UINT64_C(1000) },
                { "M", UINT64_C(1000)*UINT64_C(1000) },
                { "K", UINT64_C(1000) },
        };
        ধ্রুব suffix_table *table;
        size_t n;

        প্রতিজ্ঞা(buf);
        assert_cc(ELEMENTSOF(table_iec) == ELEMENTSOF(table_si));

        যদি (t == UINT64_MAX)
                প্রত্যাগম রিক্ত;

        table = flag & FORMAT_BYTES_USE_IEC ? table_iec : table_si;
        n = ELEMENTSOF(table_iec);

        প্রতি (size_t i = 0; i < n; i++)
                যদি (t >= table[i].factor) {
                        uint64_t remainder = i != n - 1 ?
                                (t / table[i + 1].factor * 10 / table[n - 1].factor) % 10 :
                                (t * 10 / table[i].factor) % 10;

                        যদি (FLAGS_SET(flag, FORMAT_BYTES_ALWAYS_POINT) ||
                            (FLAGS_SET(flag, FORMAT_BYTES_BELOW_POINT) && remainder > 0))
                                (শূন্য) snprintf(buf, l,
                                                "%" PRIu64 ".%" PRIu64 "%s",
                                                t / table[i].factor,
                                                remainder,
                                                table[i].suffix);
                        অন্যথা
                                (শূন্য) snprintf(buf, l,
                                                "%" PRIu64 "%s",
                                                t / table[i].factor,
                                                table[i].suffix);

                        গচ্ছ finish;
                }

        (শূন্য) snprintf(buf, l, "%" PRIu64 "%s", t, flag & FORMAT_BYTES_TRAILING_B ? "B" : "");

finish:
        buf[l-1] = 0;
        প্রত্যাগম buf;
}
