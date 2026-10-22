/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ "format-util.h"

ವರ್ಣ* format_bytes_full(ವರ್ಣ *buf, size_t l, uint64_t t, FormatBytesFlag flag) {
        ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ {
                ಧ್ರುವ ವರ್ಣ *suffix;
                uint64_t factor;
        } suffix_table;
        ಸ್ಥಿರ ಧ್ರುವ suffix_table table_iec[] = {
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
        ಧ್ರುವ suffix_table *table;
        size_t n;

        ಪ್ರತಿಜ್ಞಾ(buf);
        assert_cc(ELEMENTSOF(table_iec) == ELEMENTSOF(table_si));

        ಯದಿ (t == UINT64_MAX)
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        table = flag & FORMAT_BYTES_USE_IEC ? table_iec : table_si;
        n = ELEMENTSOF(table_iec);

        ಪ್ರತಿ (size_t i = 0; i < n; i++)
                ಯದಿ (t >= table[i].factor) {
                        uint64_t remainder = i != n - 1 ?
                                (t / table[i + 1].factor * 10 / table[n - 1].factor) % 10 :
                                (t * 10 / table[i].factor) % 10;

                        ಯದಿ (FLAGS_SET(flag, FORMAT_BYTES_ALWAYS_POINT) ||
                            (FLAGS_SET(flag, FORMAT_BYTES_BELOW_POINT) && remainder > 0))
                                (ಶೂನ್ಯ) snprintf(buf, l,
                                                "%" PRIu64 ".%" PRIu64 "%s",
                                                t / table[i].factor,
                                                remainder,
                                                table[i].suffix);
                        ಅನ್ಯಥಾ
                                (ಶೂನ್ಯ) snprintf(buf, l,
                                                "%" PRIu64 "%s",
                                                t / table[i].factor,
                                                table[i].suffix);

                        ಗಚ್ಛ finish;
                }

        (ಶೂನ್ಯ) snprintf(buf, l, "%" PRIu64 "%s", t, flag & FORMAT_BYTES_TRAILING_B ? "B" : "");

finish:
        buf[l-1] = 0;
        ಪ್ರತ್ಯಾಗಮ buf;
}
