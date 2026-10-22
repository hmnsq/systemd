/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ଅନ୍ତର୍ଭାଵ "format-util.h"

ଅକ୍ଷର* format_bytes_full(ଅକ୍ଷର *buf, size_t l, uint64_t t, FormatBytesFlag flag) {
        ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା {
                ଅଚଳ ଅକ୍ଷର *suffix;
                uint64_t factor;
        } suffix_table;
        ସ୍ଥିର ଅଚଳ suffix_table table_iec[] = {
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
        ଅଚଳ suffix_table *table;
        size_t n;

        ପ୍ରତିଜ୍ଞା(buf);
        assert_cc(ELEMENTSOF(table_iec) == ELEMENTSOF(table_si));

        ଯଦି (t == UINT64_MAX)
                ଫେରନ୍ତୁ ଖାଲି;

        table = flag & FORMAT_BYTES_USE_IEC ? table_iec : table_si;
        n = ELEMENTSOF(table_iec);

        ପାଇଁ (size_t i = 0; i < n; i++)
                ଯଦି (t >= table[i].factor) {
                        uint64_t remainder = i != n - 1 ?
                                (t / table[i + 1].factor * 10 / table[n - 1].factor) % 10 :
                                (t * 10 / table[i].factor) % 10;

                        ଯଦି (FLAGS_SET(flag, FORMAT_BYTES_ALWAYS_POINT) ||
                            (FLAGS_SET(flag, FORMAT_BYTES_BELOW_POINT) && remainder > 0))
                                (ଶୂନ୍ୟ) snprintf(buf, l,
                                                "%" PRIu64 ".%" PRIu64 "%s",
                                                t / table[i].factor,
                                                remainder,
                                                table[i].suffix);
                        ଅନ୍ୟଥା
                                (ଶୂନ୍ୟ) snprintf(buf, l,
                                                "%" PRIu64 "%s",
                                                t / table[i].factor,
                                                table[i].suffix);

                        ଯାଅ finish;
                }

        (ଶୂନ୍ୟ) snprintf(buf, l, "%" PRIu64 "%s", t, flag & FORMAT_BYTES_TRAILING_B ? "B" : "");

finish:
        buf[l-1] = 0;
        ଫେରନ୍ତୁ buf;
}
