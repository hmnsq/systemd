/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव "format-util.h"

वर्ण* format_अष्टकानि_पूर्णम्(वर्ण *buf, size_t l, uint64_t t, Format_अष्टकानि_Flag flag) {
        प्रकारनिर्देश संरचना {
                ध्रुव वर्ण *suffix;
                uint64_t गुणकः;
        } suffix_सारणी;
        स्थिर ध्रुव suffix_सारणी सारणी_iec[] = {
                { "E", UINT64_C(1024)*UINT64_C(1024)*UINT64_C(1024)*UINT64_C(1024)*UINT64_C(1024)*UINT64_C(1024) },
                { "P", UINT64_C(1024)*UINT64_C(1024)*UINT64_C(1024)*UINT64_C(1024)*UINT64_C(1024) },
                { "T", UINT64_C(1024)*UINT64_C(1024)*UINT64_C(1024)*UINT64_C(1024) },
                { "G", UINT64_C(1024)*UINT64_C(1024)*UINT64_C(1024) },
                { "M", UINT64_C(1024)*UINT64_C(1024) },
                { "K", UINT64_C(1024) },
        }, सारणी_si[] = {
                { "E", UINT64_C(1000)*UINT64_C(1000)*UINT64_C(1000)*UINT64_C(1000)*UINT64_C(1000)*UINT64_C(1000) },
                { "P", UINT64_C(1000)*UINT64_C(1000)*UINT64_C(1000)*UINT64_C(1000)*UINT64_C(1000) },
                { "T", UINT64_C(1000)*UINT64_C(1000)*UINT64_C(1000)*UINT64_C(1000) },
                { "G", UINT64_C(1000)*UINT64_C(1000)*UINT64_C(1000) },
                { "M", UINT64_C(1000)*UINT64_C(1000) },
                { "K", UINT64_C(1000) },
        };
        ध्रुव suffix_सारणी *सारणी;
        size_t n;

        प्रतिज्ञा(buf);
        assert_cc(ELEMENTSOF(सारणी_iec) == ELEMENTSOF(सारणी_si));

        यदि (t == UINT64_MAX)
                प्रत्यागम रिक्त;

        सारणी = flag & FORMAT_अष्टकानि_USE_IEC ? सारणी_iec : सारणी_si;
        n = ELEMENTSOF(सारणी_iec);

        प्रति (size_t i = 0; i < n; i++)
                यदि (t >= सारणी[i].गुणकः) {
                        uint64_t remainder = i != n - 1 ?
                                (t / सारणी[i + 1].गुणकः * 10 / सारणी[n - 1].गुणकः) % 10 :
                                (t * 10 / सारणी[i].गुणकः) % 10;

                        यदि (FLAGS_समुच्चयः(flag, FORMAT_अष्टकानि_ALWAYS_POINT) ||
                            (FLAGS_समुच्चयः(flag, FORMAT_अष्टकानि_अधः_POINT) && remainder > 0))
                                (शून्य) snprintf(buf, l,
                                                "%" PRIu64 ".%" PRIu64 "%s",
                                                t / सारणी[i].गुणकः,
                                                remainder,
                                                सारणी[i].suffix);
                        अन्यथा
                                (शून्य) snprintf(buf, l,
                                                "%" PRIu64 "%s",
                                                t / सारणी[i].गुणकः,
                                                सारणी[i].suffix);

                        गच्छ finish;
                }

        (शून्य) snprintf(buf, l, "%" PRIu64 "%s", t, flag & FORMAT_अष्टकानि_TRAILING_B ? "B" : "");

finish:
        buf[l-1] = 0;
        प्रत्यागम buf;
}
