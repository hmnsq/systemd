/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव "format-util.h"

अक्षर* format_bytes_full(अक्षर *buf, माप_प्रकार l, uint64_t t, FormatBytesFlag flag) {
        प्रकारव्याख्या संरचना {
                स्थिरांक अक्षर *suffix;
                uint64_t factor;
        } suffix_table;
        स्थिर स्थिरांक suffix_table table_iec[] = {
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
        स्थिरांक suffix_table *table;
        माप_प्रकार n;

        प्रतिज्ञा(buf);
        assert_cc(ELEMENTSOF(table_iec) == ELEMENTSOF(table_si));

        जर (t == UINT64_MAX)
                परत रिक्त;

        table = flag & FORMAT_BYTES_USE_IEC ? table_iec : table_si;
        n = ELEMENTSOF(table_iec);

        साठी (माप_प्रकार i = 0; i < n; i++)
                जर (t >= table[i].factor) {
                        uint64_t remainder = i != n - 1 ?
                                (t / table[i + 1].factor * 10 / table[n - 1].factor) % 10 :
                                (t * 10 / table[i].factor) % 10;

                        जर (FLAGS_SET(flag, FORMAT_BYTES_ALWAYS_POINT) ||
                            (FLAGS_SET(flag, FORMAT_BYTES_BELOW_POINT) && remainder > 0))
                                (शून्य) snprintf(buf, l,
                                                "%" PRIu64 ".%" PRIu64 "%s",
                                                t / table[i].factor,
                                                remainder,
                                                table[i].suffix);
                        अन्यथा
                                (शून्य) snprintf(buf, l,
                                                "%" PRIu64 "%s",
                                                t / table[i].factor,
                                                table[i].suffix);

                        जा finish;
                }

        (शून्य) snprintf(buf, l, "%" PRIu64 "%s", t, flag & FORMAT_BYTES_TRAILING_B ? "B" : "");

finish:
        buf[l-1] = 0;
        परत buf;
}
