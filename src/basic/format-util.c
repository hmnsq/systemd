/* SPDX-License-Identifier: LGPL-2.1-or-later */

#समावेश "format-util.h"

अक्षर* format_bytes_full(अक्षर *buf, माप_प्रकार l, uint64_t t, FormatBytesFlag flag) {
        प्रकार काष्ठा {
                स्थिर अक्षर *suffix;
                uint64_t factor;
        } suffix_table;
        अटल स्थिर suffix_table table_iec[] = {
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
        स्थिर suffix_table *table;
        माप_प्रकार n;

        निश्चित(buf);
        assert_cc(ELEMENTSOF(table_iec) == ELEMENTSOF(table_si));

        अगर (t == UINT64_MAX)
                वापस शून्य;

        table = flag & FORMAT_BYTES_USE_IEC ? table_iec : table_si;
        n = ELEMENTSOF(table_iec);

        क्रम (माप_प्रकार i = 0; i < n; i++)
                अगर (t >= table[i].factor) {
                        uint64_t remainder = i != n - 1 ?
                                (t / table[i + 1].factor * 10 / table[n - 1].factor) % 10 :
                                (t * 10 / table[i].factor) % 10;

                        अगर (FLAGS_SET(flag, FORMAT_BYTES_ALWAYS_POINT) ||
                            (FLAGS_SET(flag, FORMAT_BYTES_BELOW_POINT) && remainder > 0))
                                (व्योम) snprintf(buf, l,
                                                "%" PRIu64 ".%" PRIu64 "%s",
                                                t / table[i].factor,
                                                remainder,
                                                table[i].suffix);
                        अन्यथा
                                (व्योम) snprintf(buf, l,
                                                "%" PRIu64 "%s",
                                                t / table[i].factor,
                                                table[i].suffix);

                        जाओ finish;
                }

        (व्योम) snprintf(buf, l, "%" PRIu64 "%s", t, flag & FORMAT_BYTES_TRAILING_B ? "B" : "");

finish:
        buf[l-1] = 0;
        वापस buf;
}
