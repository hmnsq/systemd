/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ "format-util.h"

ಅಕ್ಷರ* format_ಬೈಟುಗಳು_ತುಂಬಿದ(ಅಕ್ಷರ *buf, size_t l, uint64_t t, Format_ಬೈಟುಗಳು_Flag flag) {
        ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *suffix;
                uint64_t ಗುಣಕ;
        } suffix_ಕೋಷ್ಟಕ;
        ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ suffix_ಕೋಷ್ಟಕ ಕೋಷ್ಟಕ_iec[] = {
                { "E", UINT64_C(1024)*UINT64_C(1024)*UINT64_C(1024)*UINT64_C(1024)*UINT64_C(1024)*UINT64_C(1024) },
                { "P", UINT64_C(1024)*UINT64_C(1024)*UINT64_C(1024)*UINT64_C(1024)*UINT64_C(1024) },
                { "T", UINT64_C(1024)*UINT64_C(1024)*UINT64_C(1024)*UINT64_C(1024) },
                { "G", UINT64_C(1024)*UINT64_C(1024)*UINT64_C(1024) },
                { "M", UINT64_C(1024)*UINT64_C(1024) },
                { "K", UINT64_C(1024) },
        }, ಕೋಷ್ಟಕ_si[] = {
                { "E", UINT64_C(1000)*UINT64_C(1000)*UINT64_C(1000)*UINT64_C(1000)*UINT64_C(1000)*UINT64_C(1000) },
                { "P", UINT64_C(1000)*UINT64_C(1000)*UINT64_C(1000)*UINT64_C(1000)*UINT64_C(1000) },
                { "T", UINT64_C(1000)*UINT64_C(1000)*UINT64_C(1000)*UINT64_C(1000) },
                { "G", UINT64_C(1000)*UINT64_C(1000)*UINT64_C(1000) },
                { "M", UINT64_C(1000)*UINT64_C(1000) },
                { "K", UINT64_C(1000) },
        };
        ಸ್ಥಿರಾಂಕ suffix_ಕೋಷ್ಟಕ *ಕೋಷ್ಟಕ;
        size_t n;

        ಪ್ರತಿಜ್ಞಾ(buf);
        assert_cc(ELEMENTSOF(ಕೋಷ್ಟಕ_iec) == ELEMENTSOF(ಕೋಷ್ಟಕ_si));

        ಆದರೆ (t == UINT64_MAX)
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        ಕೋಷ್ಟಕ = flag & FORMAT_ಬೈಟುಗಳು_USE_IEC ? ಕೋಷ್ಟಕ_iec : ಕೋಷ್ಟಕ_si;
        n = ELEMENTSOF(ಕೋಷ್ಟಕ_iec);

        ಗಾಗಿ (size_t i = 0; i < n; i++)
                ಆದರೆ (t >= ಕೋಷ್ಟಕ[i].ಗುಣಕ) {
                        uint64_t remainder = i != n - 1 ?
                                (t / ಕೋಷ್ಟಕ[i + 1].ಗುಣಕ * 10 / ಕೋಷ್ಟಕ[n - 1].ಗುಣಕ) % 10 :
                                (t * 10 / ಕೋಷ್ಟಕ[i].ಗುಣಕ) % 10;

                        ಆದರೆ (FLAGS_ಗಣ(flag, FORMAT_ಬೈಟುಗಳು_ALWAYS_ಬಿಂದು) ||
                            (FLAGS_ಗಣ(flag, FORMAT_ಬೈಟುಗಳು_ಕೆಳಗೆ_ಬಿಂದು) && remainder > 0))
                                (ಶೂನ್ಯ) snprintf(buf, l,
                                                "%" PRIu64 ".%" PRIu64 "%s",
                                                t / ಕೋಷ್ಟಕ[i].ಗುಣಕ,
                                                remainder,
                                                ಕೋಷ್ಟಕ[i].suffix);
                        ಇಲ್ಲದಿದ್ದರೆ
                                (ಶೂನ್ಯ) snprintf(buf, l,
                                                "%" PRIu64 "%s",
                                                t / ಕೋಷ್ಟಕ[i].ಗುಣಕ,
                                                ಕೋಷ್ಟಕ[i].suffix);

                        ಹೋಗು ಮುಕ್ತಾಯ;
                }

        (ಶೂನ್ಯ) snprintf(buf, l, "%" PRIu64 "%s", t, flag & FORMAT_ಬೈಟುಗಳು_TRAILING_B ? "B" : "");

ಮುಕ್ತಾಯ:
        buf[l-1] = 0;
        ಹಿಂತಿರುಗಿ buf;
}
