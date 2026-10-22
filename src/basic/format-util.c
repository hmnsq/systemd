/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ਅਨ੍ਤਰ੍ਭਾਵ "format-util.h"

ਅੱਖਰ* format_ਬਾਈਟਾਂ_ਭਰਿਆ(ਅੱਖਰ *buf, size_t l, uint64_t t, Format_ਬਾਈਟਾਂ_Flag flag) {
        ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
                ਅਚਲ ਅੱਖਰ *suffix;
                uint64_t ਗੁਣਕ;
        } suffix_ਸਾਰਣੀ;
        ਸਥਿਰ ਅਚਲ suffix_ਸਾਰਣੀ ਸਾਰਣੀ_iec[] = {
                { "E", UINT64_C(1024)*UINT64_C(1024)*UINT64_C(1024)*UINT64_C(1024)*UINT64_C(1024)*UINT64_C(1024) },
                { "P", UINT64_C(1024)*UINT64_C(1024)*UINT64_C(1024)*UINT64_C(1024)*UINT64_C(1024) },
                { "T", UINT64_C(1024)*UINT64_C(1024)*UINT64_C(1024)*UINT64_C(1024) },
                { "G", UINT64_C(1024)*UINT64_C(1024)*UINT64_C(1024) },
                { "M", UINT64_C(1024)*UINT64_C(1024) },
                { "K", UINT64_C(1024) },
        }, ਸਾਰਣੀ_si[] = {
                { "E", UINT64_C(1000)*UINT64_C(1000)*UINT64_C(1000)*UINT64_C(1000)*UINT64_C(1000)*UINT64_C(1000) },
                { "P", UINT64_C(1000)*UINT64_C(1000)*UINT64_C(1000)*UINT64_C(1000)*UINT64_C(1000) },
                { "T", UINT64_C(1000)*UINT64_C(1000)*UINT64_C(1000)*UINT64_C(1000) },
                { "G", UINT64_C(1000)*UINT64_C(1000)*UINT64_C(1000) },
                { "M", UINT64_C(1000)*UINT64_C(1000) },
                { "K", UINT64_C(1000) },
        };
        ਅਚਲ suffix_ਸਾਰਣੀ *ਸਾਰਣੀ;
        size_t n;

        ਪ੍ਰਤਿਜ੍ਞਾ(buf);
        assert_cc(ELEMENTSOF(ਸਾਰਣੀ_iec) == ELEMENTSOF(ਸਾਰਣੀ_si));

        ਜੇ (t == UINT64_MAX)
                ਵਾਪਸ ਨਿਰਾ;

        ਸਾਰਣੀ = flag & FORMAT_ਬਾਈਟਾਂ_USE_IEC ? ਸਾਰਣੀ_iec : ਸਾਰਣੀ_si;
        n = ELEMENTSOF(ਸਾਰਣੀ_iec);

        ਲਈ (size_t i = 0; i < n; i++)
                ਜੇ (t >= ਸਾਰਣੀ[i].ਗੁਣਕ) {
                        uint64_t remainder = i != n - 1 ?
                                (t / ਸਾਰਣੀ[i + 1].ਗੁਣਕ * 10 / ਸਾਰਣੀ[n - 1].ਗੁਣਕ) % 10 :
                                (t * 10 / ਸਾਰਣੀ[i].ਗੁਣਕ) % 10;

                        ਜੇ (FLAGS_ਸੈੱਟ(flag, FORMAT_ਬਾਈਟਾਂ_ALWAYS_ਬਿੰਦੂ) ||
                            (FLAGS_ਸੈੱਟ(flag, FORMAT_ਬਾਈਟਾਂ_ਹੇਠਾਂ_ਬਿੰਦੂ) && remainder > 0))
                                (ਖਾਲੀ) snprintf(buf, l,
                                                "%" PRIu64 ".%" PRIu64 "%s",
                                                t / ਸਾਰਣੀ[i].ਗੁਣਕ,
                                                remainder,
                                                ਸਾਰਣੀ[i].suffix);
                        ਨਹੀਂਤਾਂ
                                (ਖਾਲੀ) snprintf(buf, l,
                                                "%" PRIu64 "%s",
                                                t / ਸਾਰਣੀ[i].ਗੁਣਕ,
                                                ਸਾਰਣੀ[i].suffix);

                        ਜਾਓ ਅੰਤ;
                }

        (ਖਾਲੀ) snprintf(buf, l, "%" PRIu64 "%s", t, flag & FORMAT_ਬਾਈਟਾਂ_TRAILING_B ? "B" : "");

ਅੰਤ:
        buf[l-1] = 0;
        ਵਾਪਸ buf;
}
