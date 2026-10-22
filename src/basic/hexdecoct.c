/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ <ctype.h>
#ಅನ್ತರ್ಭಾವ <stdio.h>

#ಅನ್ತರ್ಭಾವ "alloc-util.h"
#ಅನ್ತರ್ಭಾವ "hexdecoct.h"
#ಅನ್ತರ್ಭಾವ "memory-util.h"
#ಅನ್ತರ್ಭಾವ "string-util.h"

ಅಕ್ಷರ octchar(ಪೂರ್ಣಾಂಕ x) {
        ಹಿಂತಿರುಗಿ '0' + (x & 7);
}

ಪೂರ್ಣಾಂಕ unoctchar(ಅಕ್ಷರ c) {

        ಆದರೆ (c >= '0' && c <= '7')
                ಹಿಂತಿರುಗಿ c - '0';

        ಹಿಂತಿರುಗಿ -EINVAL;
}

ಅಕ್ಷರ decchar(ಪೂರ್ಣಾಂಕ x) {
        ಹಿಂತಿರುಗಿ '0' + (x % 10);
}

ಪೂರ್ಣಾಂಕ undecchar(ಅಕ್ಷರ c) {

        ಆದರೆ (c >= '0' && c <= '9')
                ಹಿಂತಿರುಗಿ c - '0';

        ಹಿಂತಿರುಗಿ -EINVAL;
}

ಅಕ್ಷರ hexchar(ಪೂರ್ಣಾಂಕ x) {
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕೋಷ್ಟಕ = LOWERCASE_HEXDIGITS;
        ಹಿಂತಿರುಗಿ ಕೋಷ್ಟಕ[x & 15];
}

ಪೂರ್ಣಾಂಕ unhexchar(ಅಕ್ಷರ c) {

        ಆದರೆ (c >= '0' && c <= '9')
                ಹಿಂತಿರುಗಿ c - '0';

        ಆದರೆ (c >= 'a' && c <= 'f')
                ಹಿಂತಿರುಗಿ c - 'a' + 10;

        ಆದರೆ (c >= 'A' && c <= 'F')
                ಹಿಂತಿರುಗಿ c - 'A' + 10;

        ಹಿಂತಿರುಗಿ -EINVAL;
}

ಅಕ್ಷರ* hexmem(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *p, size_t l) {
        ಸ್ಥಿರಾಂಕ uint8_t *x;
        ಅಕ್ಷರ *r, *z;

        ಪ್ರತಿಜ್ಞಾ(p || l == 0);

        z = r = new(ಅಕ್ಷರ, l * 2 + 1);
        ಆದರೆ (!r)
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        ಗಾಗಿ (x = p; x && x < (ಸ್ಥಿರಾಂಕ uint8_t*) p + l; x++) {
                *(z++) = hexchar(*x >> 4);
                *(z++) = hexchar(*x & 15);
        }

        *z = 0;
        ಹಿಂತಿರುಗಿ r;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ unhex_ಮುಂದಿನ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **p, size_t *l) {
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(p);
        ಪ್ರತಿಜ್ಞಾ(l);

        /* Find the next non-whitespace character, and decode it. We
         * greedily skip all preceding and all following whitespace. */

        ಗಾಗಿ (;;) {
                ಆದರೆ (*l == 0)
                        ಹಿಂತಿರುಗಿ -EPIPE;

                ಆದರೆ (!strchr(WHITESPACE, **p))
                        ನಿಲ್ಲಿಸು;

                /* Skip leading whitespace */
                (*p)++, (*l)--;
        }

        r = unhexchar(**p);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಗಾಗಿ (;;) {
                (*p)++, (*l)--;

                ಆದರೆ (*l == 0 || !strchr(WHITESPACE, **p))
                        ನಿಲ್ಲಿಸು;

                /* Skip following whitespace */
        }

        ಹಿಂತಿರುಗಿ r;
}

ಪೂರ್ಣಾಂಕ unhexmem_ತುಂಬಿದ(
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p,
                size_t l,
                bool secure,
                ಶೂನ್ಯ **ret_ದತ್ತಾಂಶ,
                size_t *ret_ಗಾತ್ರ) {

        _cleanup_free_ uint8_t *buf = ಖಾಲಿ;
        size_t buf_size;
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *x;
        uint8_t *z;

        ಪ್ರತಿಜ್ಞಾ(p || l == 0);

        ಆದರೆ (l == SIZE_MAX)
                l = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(p);

        /* Note that the calculation of memory size is an upper boundary, as we ignore whitespace while decoding */
        buf_size = (l + 1) / 2 + 1;
        buf = ಸ್ಮರಣೆ_ಕೊಡು(buf_size);
        ಆದರೆ (!buf)
                ಹಿಂತಿರುಗಿ -ENOMEM;

        CLEANUP_ERASE_PTR(secure ? &buf : ಖಾಲಿ, buf_size);

        ಗಾಗಿ (x = p, z = buf;;) {
                ಪೂರ್ಣಾಂಕ a, b;

                a = unhex_ಮುಂದಿನ(&x, &l);
                ಆದರೆ (a == -EPIPE) /* End of string */
                        ನಿಲ್ಲಿಸು;
                ಆದರೆ (a < 0)
                        ಹಿಂತಿರುಗಿ a;

                b = unhex_ಮುಂದಿನ(&x, &l);
                ಆದರೆ (b < 0)
                        ಹಿಂತಿರುಗಿ b;

                *(z++) = (uint8_t) a << 4 | (uint8_t) b;
        }

        *z = 0;

        ಆದರೆ (ret_ಗಾತ್ರ)
                *ret_ಗಾತ್ರ = (size_t) (z - buf);
        ಆದರೆ (ret_ದತ್ತಾಂಶ)
                *ret_ದತ್ತಾಂಶ = TAKE_PTR(buf);

        ಹಿಂತಿರುಗಿ 0;
}

/* https://tools.ietf.org/html/rfc4648#section-6
 * Notice that base32hex differs from base32 in the alphabet it uses.
 * The distinction is that the base32hex representation preserves the
 * order of the underlying data when compared as bytestrings, this is
 * useful when representing NSEC3 hashes, as one can then verify the
 * order of hashes directly from their representation. */
ಅಕ್ಷರ base32hexchar(ಪೂರ್ಣಾಂಕ x) {
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕೋಷ್ಟಕ = DIGITS "ABCDEFGHIJKLMNOPQRSTUV";
        ಹಿಂತಿರುಗಿ ಕೋಷ್ಟಕ[x & 31];
}

ಪೂರ್ಣಾಂಕ unbase32hexchar(ಅಕ್ಷರ c) {
        ಚಿಹ್ನೆರಹಿತ ಸ್ಥಾನಾಂತರ;

        ಆದರೆ (c >= '0' && c <= '9')
                ಹಿಂತಿರುಗಿ c - '0';

        ಸ್ಥಾನಾಂತರ = '9' - '0' + 1;

        ಆದರೆ (c >= 'A' && c <= 'V')
                ಹಿಂತಿರುಗಿ c - 'A' + ಸ್ಥಾನಾಂತರ;

        ಹಿಂತಿರುಗಿ -EINVAL;
}

ಅಕ್ಷರ* base32hexmem(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *p, size_t l, bool padding) {
        ಅಕ್ಷರ *r, *z;
        ಸ್ಥಿರಾಂಕ uint8_t *x;
        size_t len;

        ಪ್ರತಿಜ್ಞಾ(p || l == 0);

        ಆದರೆ (padding)
                /* five input bytes makes eight output bytes, padding is added so we must round up */
                len = 8 * (l + 4) / 5;
        ಇಲ್ಲದಿದ್ದರೆ {
                /* same, but round down as there is no padding */
                len = 8 * l / 5;

                ಆಯ್ಕೆ (l % 5) {
                ಸ್ಥಿತಿ 4:
                        len += 7;
                        ನಿಲ್ಲಿಸು;
                ಸ್ಥಿತಿ 3:
                        len += 5;
                        ನಿಲ್ಲಿಸು;
                ಸ್ಥಿತಿ 2:
                        len += 4;
                        ನಿಲ್ಲಿಸು;
                ಸ್ಥಿತಿ 1:
                        len += 2;
                        ನಿಲ್ಲಿಸು;
                }
        }

        z = r = ಸ್ಮರಣೆ_ಕೊಡು(len + 1);
        ಆದರೆ (!r)
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        ಗಾಗಿ (x = p; x < (ಸ್ಥಿರಾಂಕ uint8_t*) p + (l / 5) * 5; x += 5) {
                /* x[0] == XXXXXXXX; x[1] == YYYYYYYY; x[2] == ZZZZZZZZ
                 * x[3] == QQQQQQQQ; x[4] == WWWWWWWW */
                *(z++) = base32hexchar(x[0] >> 3);                    /* 000XXXXX */
                *(z++) = base32hexchar((x[0] & 7) << 2 | x[1] >> 6);  /* 000XXXYY */
                *(z++) = base32hexchar((x[1] & 63) >> 1);             /* 000YYYYY */
                *(z++) = base32hexchar((x[1] & 1) << 4 | x[2] >> 4);  /* 000YZZZZ */
                *(z++) = base32hexchar((x[2] & 15) << 1 | x[3] >> 7); /* 000ZZZZQ */
                *(z++) = base32hexchar((x[3] & 127) >> 2);            /* 000QQQQQ */
                *(z++) = base32hexchar((x[3] & 3) << 3 | x[4] >> 5);  /* 000QQWWW */
                *(z++) = base32hexchar((x[4] & 31));                  /* 000WWWWW */
        }

        ಆಯ್ಕೆ (l % 5) {
        ಸ್ಥಿತಿ 4:
                *(z++) = base32hexchar(x[0] >> 3);                    /* 000XXXXX */
                *(z++) = base32hexchar((x[0] & 7) << 2 | x[1] >> 6);  /* 000XXXYY */
                *(z++) = base32hexchar((x[1] & 63) >> 1);             /* 000YYYYY */
                *(z++) = base32hexchar((x[1] & 1) << 4 | x[2] >> 4);   /* 000YZZZZ */
                *(z++) = base32hexchar((x[2] & 15) << 1 | x[3] >> 7); /* 000ZZZZQ */
                *(z++) = base32hexchar((x[3] & 127) >> 2);            /* 000QQQQQ */
                *(z++) = base32hexchar((x[3] & 3) << 3);              /* 000QQ000 */
                ಆದರೆ (padding)
                        *(z++) = '=';

                ನಿಲ್ಲಿಸು;

        ಸ್ಥಿತಿ 3:
                *(z++) = base32hexchar(x[0] >> 3);                   /* 000XXXXX */
                *(z++) = base32hexchar((x[0] & 7) << 2 | x[1] >> 6); /* 000XXXYY */
                *(z++) = base32hexchar((x[1] & 63) >> 1);            /* 000YYYYY */
                *(z++) = base32hexchar((x[1] & 1) << 4 | x[2] >> 4); /* 000YZZZZ */
                *(z++) = base32hexchar((x[2] & 15) << 1);            /* 000ZZZZ0 */
                ಆದರೆ (padding) {
                        *(z++) = '=';
                        *(z++) = '=';
                        *(z++) = '=';
                }

                ನಿಲ್ಲಿಸು;

        ಸ್ಥಿತಿ 2:
                *(z++) = base32hexchar(x[0] >> 3);                   /* 000XXXXX */
                *(z++) = base32hexchar((x[0] & 7) << 2 | x[1] >> 6); /* 000XXXYY */
                *(z++) = base32hexchar((x[1] & 63) >> 1);            /* 000YYYYY */
                *(z++) = base32hexchar((x[1] & 1) << 4);             /* 000Y0000 */
                ಆದರೆ (padding) {
                        *(z++) = '=';
                        *(z++) = '=';
                        *(z++) = '=';
                        *(z++) = '=';
                }

                ನಿಲ್ಲಿಸು;

        ಸ್ಥಿತಿ 1:
                *(z++) = base32hexchar(x[0] >> 3);       /* 000XXXXX */
                *(z++) = base32hexchar((x[0] & 7) << 2); /* 000XXX00 */
                ಆದರೆ (padding) {
                        *(z++) = '=';
                        *(z++) = '=';
                        *(z++) = '=';
                        *(z++) = '=';
                        *(z++) = '=';
                        *(z++) = '=';
                }

                ನಿಲ್ಲಿಸು;
        }

        *z = 0;
        ಹಿಂತಿರುಗಿ r;
}

ಪೂರ್ಣಾಂಕ unbase32hexmem(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p, size_t l, bool padding, ಶೂನ್ಯ **mem, size_t *_len) {
        _cleanup_free_ uint8_t *r = ಖಾಲಿ;
        ಪೂರ್ಣಾಂಕ a, b, c, d, e, f, g, h;
        uint8_t *z;
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *x;
        size_t len;
        ಚಿಹ್ನೆರಹಿತ pad = 0;

        ಪ್ರತಿಜ್ಞಾ(p || l == 0);
        ಪ್ರತಿಜ್ಞಾ(mem);
        ಪ್ರತಿಜ್ಞಾ(_len);

        ಆದರೆ (l == SIZE_MAX)
                l = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(p);

        /* padding ensures any base32hex input has input divisible by 8 */
        ಆದರೆ (padding && l % 8 != 0)
                ಹಿಂತಿರುಗಿ -EINVAL;

        ಆದರೆ (padding) {
                /* strip the padding */
                ಆಗ (l > 0 && p[l - 1] == '=' && pad < 7) {
                        pad++;
                        l--;
                }
        }

        /* a group of eight input bytes needs five output bytes, in case of
         * padding we need to add some extra bytes */
        len = (l / 8) * 5;

        ಆಯ್ಕೆ (l % 8) {
        ಸ್ಥಿತಿ 7:
                len += 4;
                ನಿಲ್ಲಿಸು;
        ಸ್ಥಿತಿ 5:
                len += 3;
                ನಿಲ್ಲಿಸು;
        ಸ್ಥಿತಿ 4:
                len += 2;
                ನಿಲ್ಲಿಸು;
        ಸ್ಥಿತಿ 2:
                len += 1;
                ನಿಲ್ಲಿಸು;
        ಸ್ಥಿತಿ 0:
                ನಿಲ್ಲಿಸು;
        ಪೂರ್ವನಿಯೋಜಿತ:
                ಹಿಂತಿರುಗಿ -EINVAL;
        }

        z = r = ಸ್ಮರಣೆ_ಕೊಡು(len + 1);
        ಆದರೆ (!r)
                ಹಿಂತಿರುಗಿ -ENOMEM;

        ಗಾಗಿ (x = p; x < p + (l / 8) * 8; x += 8) {
                /* a == 000XXXXX; b == 000YYYYY; c == 000ZZZZZ; d == 000WWWWW
                 * e == 000SSSSS; f == 000QQQQQ; g == 000VVVVV; h == 000RRRRR */
                a = unbase32hexchar(x[0]);
                ಆದರೆ (a < 0)
                        ಹಿಂತಿರುಗಿ -EINVAL;

                b = unbase32hexchar(x[1]);
                ಆದರೆ (b < 0)
                        ಹಿಂತಿರುಗಿ -EINVAL;

                c = unbase32hexchar(x[2]);
                ಆದರೆ (c < 0)
                        ಹಿಂತಿರುಗಿ -EINVAL;

                d = unbase32hexchar(x[3]);
                ಆದರೆ (d < 0)
                        ಹಿಂತಿರುಗಿ -EINVAL;

                e = unbase32hexchar(x[4]);
                ಆದರೆ (e < 0)
                        ಹಿಂತಿರುಗಿ -EINVAL;

                f = unbase32hexchar(x[5]);
                ಆದರೆ (f < 0)
                        ಹಿಂತಿರುಗಿ -EINVAL;

                g = unbase32hexchar(x[6]);
                ಆದರೆ (g < 0)
                        ಹಿಂತಿರುಗಿ -EINVAL;

                h = unbase32hexchar(x[7]);
                ಆದರೆ (h < 0)
                        ಹಿಂತಿರುಗಿ -EINVAL;

                *(z++) = (uint8_t) a << 3 | (uint8_t) b >> 2;                    /* XXXXXYYY */
                *(z++) = (uint8_t) b << 6 | (uint8_t) c << 1 | (uint8_t) d >> 4; /* YYZZZZZW */
                *(z++) = (uint8_t) d << 4 | (uint8_t) e >> 1;                    /* WWWWSSSS */
                *(z++) = (uint8_t) e << 7 | (uint8_t) f << 2 | (uint8_t) g >> 3; /* SQQQQQVV */
                *(z++) = (uint8_t) g << 5 | (uint8_t) h;                         /* VVVRRRRR */
        }

        ಆಯ್ಕೆ (l % 8) {
        ಸ್ಥಿತಿ 7:
                a = unbase32hexchar(x[0]);
                ಆದರೆ (a < 0)
                        ಹಿಂತಿರುಗಿ -EINVAL;

                b = unbase32hexchar(x[1]);
                ಆದರೆ (b < 0)
                        ಹಿಂತಿರುಗಿ -EINVAL;

                c = unbase32hexchar(x[2]);
                ಆದರೆ (c < 0)
                        ಹಿಂತಿರುಗಿ -EINVAL;

                d = unbase32hexchar(x[3]);
                ಆದರೆ (d < 0)
                        ಹಿಂತಿರುಗಿ -EINVAL;

                e = unbase32hexchar(x[4]);
                ಆದರೆ (e < 0)
                        ಹಿಂತಿರುಗಿ -EINVAL;

                f = unbase32hexchar(x[5]);
                ಆದರೆ (f < 0)
                        ಹಿಂತಿರುಗಿ -EINVAL;

                g = unbase32hexchar(x[6]);
                ಆದರೆ (g < 0)
                        ಹಿಂತಿರುಗಿ -EINVAL;

                /* g == 000VV000 */
                ಆದರೆ (g & 7)
                        ಹಿಂತಿರುಗಿ -EINVAL;

                *(z++) = (uint8_t) a << 3 | (uint8_t) b >> 2;                    /* XXXXXYYY */
                *(z++) = (uint8_t) b << 6 | (uint8_t) c << 1 | (uint8_t) d >> 4; /* YYZZZZZW */
                *(z++) = (uint8_t) d << 4 | (uint8_t) e >> 1;                    /* WWWWSSSS */
                *(z++) = (uint8_t) e << 7 | (uint8_t) f << 2 | (uint8_t) g >> 3; /* SQQQQQVV */

                ನಿಲ್ಲಿಸು;
        ಸ್ಥಿತಿ 5:
                a = unbase32hexchar(x[0]);
                ಆದರೆ (a < 0)
                        ಹಿಂತಿರುಗಿ -EINVAL;

                b = unbase32hexchar(x[1]);
                ಆದರೆ (b < 0)
                        ಹಿಂತಿರುಗಿ -EINVAL;

                c = unbase32hexchar(x[2]);
                ಆದರೆ (c < 0)
                        ಹಿಂತಿರುಗಿ -EINVAL;

                d = unbase32hexchar(x[3]);
                ಆದರೆ (d < 0)
                        ಹಿಂತಿರುಗಿ -EINVAL;

                e = unbase32hexchar(x[4]);
                ಆದರೆ (e < 0)
                        ಹಿಂತಿರುಗಿ -EINVAL;

                /* e == 000SSSS0 */
                ಆದರೆ (e & 1)
                        ಹಿಂತಿರುಗಿ -EINVAL;

                *(z++) = (uint8_t) a << 3 | (uint8_t) b >> 2;                    /* XXXXXYYY */
                *(z++) = (uint8_t) b << 6 | (uint8_t) c << 1 | (uint8_t) d >> 4; /* YYZZZZZW */
                *(z++) = (uint8_t) d << 4 | (uint8_t) e >> 1;                    /* WWWWSSSS */

                ನಿಲ್ಲಿಸು;
        ಸ್ಥಿತಿ 4:
                a = unbase32hexchar(x[0]);
                ಆದರೆ (a < 0)
                        ಹಿಂತಿರುಗಿ -EINVAL;

                b = unbase32hexchar(x[1]);
                ಆದರೆ (b < 0)
                        ಹಿಂತಿರುಗಿ -EINVAL;

                c = unbase32hexchar(x[2]);
                ಆದರೆ (c < 0)
                        ಹಿಂತಿರುಗಿ -EINVAL;

                d = unbase32hexchar(x[3]);
                ಆದರೆ (d < 0)
                        ಹಿಂತಿರುಗಿ -EINVAL;

                /* d == 000W0000 */
                ಆದರೆ (d & 15)
                        ಹಿಂತಿರುಗಿ -EINVAL;

                *(z++) = (uint8_t) a << 3 | (uint8_t) b >> 2;                    /* XXXXXYYY */
                *(z++) = (uint8_t) b << 6 | (uint8_t) c << 1 | (uint8_t) d >> 4; /* YYZZZZZW */

                ನಿಲ್ಲಿಸು;
        ಸ್ಥಿತಿ 2:
                a = unbase32hexchar(x[0]);
                ಆದರೆ (a < 0)
                        ಹಿಂತಿರುಗಿ -EINVAL;

                b = unbase32hexchar(x[1]);
                ಆದರೆ (b < 0)
                        ಹಿಂತಿರುಗಿ -EINVAL;

                /* b == 000YYY00 */
                ಆದರೆ (b & 3)
                        ಹಿಂತಿರುಗಿ -EINVAL;

                *(z++) = (uint8_t) a << 3 | (uint8_t) b >> 2; /* XXXXXYYY */

                ನಿಲ್ಲಿಸು;
        ಸ್ಥಿತಿ 0:
                ನಿಲ್ಲಿಸು;
        ಪೂರ್ವನಿಯೋಜಿತ:
                ಹಿಂತಿರುಗಿ -EINVAL;
        }

        *z = 0;

        *mem = TAKE_PTR(r);
        *_len = len;

        ಹಿಂತಿರುಗಿ 0;
}

/* https://tools.ietf.org/html/rfc4648#section-4 */
ಅಕ್ಷರ base64char(ಪೂರ್ಣಾಂಕ x) {
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕೋಷ್ಟಕ = UPPERCASE_LETTERS LOWERCASE_LETTERS DIGITS "+/";
        ಹಿಂತಿರುಗಿ ಕೋಷ್ಟಕ[x & 63];
}

/* This is almost base64char(), but not entirely, as it uses the "url and filename safe" alphabet,
 * since we don't want "/" appear in interface names (since interfaces appear in sysfs as filenames).
 * See section #5 of RFC 4648. */
ಅಕ್ಷರ urlsafe_base64char(ಪೂರ್ಣಾಂಕ x) {
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕೋಷ್ಟಕ = UPPERCASE_LETTERS LOWERCASE_LETTERS DIGITS "-_";
        ಹಿಂತಿರುಗಿ ಕೋಷ್ಟಕ[x & 63];
}

ಪೂರ್ಣಾಂಕ unbase64char(ಅಕ್ಷರ c) {
        ಚಿಹ್ನೆರಹಿತ ಸ್ಥಾನಾಂತರ;

        ಆದರೆ (c >= 'A' && c <= 'Z')
                ಹಿಂತಿರುಗಿ c - 'A';

        ಸ್ಥಾನಾಂತರ = 'Z' - 'A' + 1;

        ಆದರೆ (c >= 'a' && c <= 'z')
                ಹಿಂತಿರುಗಿ c - 'a' + ಸ್ಥಾನಾಂತರ;

        ಸ್ಥಾನಾಂತರ += 'z' - 'a' + 1;

        ಆದರೆ (c >= '0' && c <= '9')
                ಹಿಂತಿರುಗಿ c - '0' + ಸ್ಥಾನಾಂತರ;

        ಸ್ಥಾನಾಂತರ += '9' - '0' + 1;

        ಆದರೆ (ಒಳ_ಗಣ(c, '+', '-')) /* Support both the regular and the URL safe character set (see above) */
                ಹಿಂತಿರುಗಿ ಸ್ಥಾನಾಂತರ;

        ಸ್ಥಾನಾಂತರ++;

        ಆದರೆ (ಒಳ_ಗಣ(c, '/', '_')) /* ditto */
                ಹಿಂತಿರುಗಿ ಸ್ಥಾನಾಂತರ;

        ಹಿಂತಿರುಗಿ -EINVAL;
}

ಸ್ಥಿರ ಶೂನ್ಯ maybe_ಸಾಲು_break(ಅಕ್ಷರ **x, ಅಕ್ಷರ *ಆರಂಭ, size_t ಸಾಲು_break) {
        size_t n;

        ಪ್ರತಿಜ್ಞಾ(x);
        ಪ್ರತಿಜ್ಞಾ(*x);
        ಪ್ರತಿಜ್ಞಾ(ಆರಂಭ);
        ಪ್ರತಿಜ್ಞಾ(*x >= ಆರಂಭ);

        ಆದರೆ (ಸಾಲು_break == SIZE_MAX)
                ಹಿಂತಿರುಗಿ;

        n = *x - ಆರಂಭ;

        ಆದರೆ (n % (ಸಾಲು_break + 1) == ಸಾಲು_break)
                *((*x)++) = '\n';
}

ssize_t base64mem_ತುಂಬಿದ(
                ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *p,
                size_t l,
                size_t ಸಾಲು_break,
                ಅಕ್ಷರ **ret) {

        ಸ್ಥಿರಾಂಕ uint8_t *x;
        ಅಕ್ಷರ *b, *z;
        size_t m;

        ಪ್ರತಿಜ್ಞಾ(p || l == 0);
        ಪ್ರತಿಜ್ಞಾ(ಸಾಲು_break > 0);
        ಪ್ರತಿಜ್ಞಾ(ret);

        /* three input bytes makes four output bytes, padding is added so we must round up */
        m = 4 * (l + 2) / 3 + 1;
        ಆದರೆ (ಸಾಲು_break != SIZE_MAX)
                m += m / ಸಾಲು_break;

        z = b = ಸ್ಮರಣೆ_ಕೊಡು(m);
        ಆದರೆ (!b)
                ಹಿಂತಿರುಗಿ -ENOMEM;

        ಗಾಗಿ (x = p; x && x < (ಸ್ಥಿರಾಂಕ uint8_t*) p + (l / 3) * 3; x += 3) {
                /* x[0] == XXXXXXXX; x[1] == YYYYYYYY; x[2] == ZZZZZZZZ */
                maybe_ಸಾಲು_break(&z, b, ಸಾಲು_break);
                *(z++) = base64char(x[0] >> 2);                    /* 00XXXXXX */
                maybe_ಸಾಲು_break(&z, b, ಸಾಲು_break);
                *(z++) = base64char((x[0] & 3) << 4 | x[1] >> 4);  /* 00XXYYYY */
                maybe_ಸಾಲು_break(&z, b, ಸಾಲು_break);
                *(z++) = base64char((x[1] & 15) << 2 | x[2] >> 6); /* 00YYYYZZ */
                maybe_ಸಾಲು_break(&z, b, ಸಾಲು_break);
                *(z++) = base64char(x[2] & 63);                    /* 00ZZZZZZ */
        }

        ಆಯ್ಕೆ (l % 3) {
        ಸ್ಥಿತಿ 2:
                maybe_ಸಾಲು_break(&z, b, ಸಾಲು_break);
                *(z++) = base64char(x[0] >> 2);                   /* 00XXXXXX */
                maybe_ಸಾಲು_break(&z, b, ಸಾಲು_break);
                *(z++) = base64char((x[0] & 3) << 4 | x[1] >> 4); /* 00XXYYYY */
                maybe_ಸಾಲು_break(&z, b, ಸಾಲು_break);
                *(z++) = base64char((x[1] & 15) << 2);            /* 00YYYY00 */
                maybe_ಸಾಲು_break(&z, b, ಸಾಲು_break);
                *(z++) = '=';
                ನಿಲ್ಲಿಸು;

        ಸ್ಥಿತಿ 1:
                maybe_ಸಾಲು_break(&z, b, ಸಾಲು_break);
                *(z++) = base64char(x[0] >> 2);        /* 00XXXXXX */
                maybe_ಸಾಲು_break(&z, b, ಸಾಲು_break);
                *(z++) = base64char((x[0] & 3) << 4);  /* 00XX0000 */
                maybe_ಸಾಲು_break(&z, b, ಸಾಲು_break);
                *(z++) = '=';
                maybe_ಸಾಲು_break(&z, b, ಸಾಲು_break);
                *(z++) = '=';
                ನಿಲ್ಲಿಸು;
        }

        *z = 0;
        *ret = b;

        ಪ್ರತಿಜ್ಞಾ(z >= b); /* Let static analyzers know that the answer is non-negative. */
        ಹಿಂತಿರುಗಿ z - b;
}

ಸ್ಥಿರ ssize_t base64_append_ಅಗಲ(
                ಅಕ್ಷರ **ಪೂರ್ವಪ್ರತ್ಯಯ,
                size_t plen,
                ಅಕ್ಷರ sep,
                size_t indent,
                ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *p,
                size_t l,
                size_t width) {

        _cleanup_free_ ಅಕ್ಷರ *x = ಖಾಲಿ;
        ಅಕ್ಷರ *t, *s;
        size_t ಸಾಲುಗಳು;
        ssize_t len;

        ಪ್ರತಿಜ್ಞಾ(ಪೂರ್ವಪ್ರತ್ಯಯ);
        ಪ್ರತಿಜ್ಞಾ(*ಪೂರ್ವಪ್ರತ್ಯಯ || plen == 0);
        ಪ್ರತಿಜ್ಞಾ(p || l == 0);

        len = base64mem(p, l, &x);
        ಆದರೆ (len < 0)
                ಹಿಂತಿರುಗಿ len;
        ಆದರೆ (len == 0)
                ಹಿಂತಿರುಗಿ plen;

        ಸಾಲುಗಳು = DIV_ಸುತ್ತು_ಮೇಲೆ(len, width);

        ಆದರೆ (plen >= SSIZE_MAX - 1 - 1 ||
            ಸಾಲುಗಳು > (SSIZE_MAX - plen - 1 - 1) / (indent + width + 1))
                ಹಿಂತಿರುಗಿ -ENOMEM;

        t = realloc(*ಪೂರ್ವಪ್ರತ್ಯಯ, plen + 1 + 1 + (indent + width + 1) * ಸಾಲುಗಳು);
        ಆದರೆ (!t)
                ಹಿಂತಿರುಗಿ -ENOMEM;

        s = t + plen;
        ಗಾಗಿ (size_t ಸಾಲು = 0; ಸಾಲು < ಸಾಲುಗಳು; ಸಾಲು++) {
                size_t act = MIN(width, (size_t) len);

                ಆದರೆ (ಸಾಲು > 0)
                        sep = '\n';

                ಆದರೆ (s > t) {
                        *s++ = sep;
                        ಆದರೆ (sep == '\n')
                                s = mempset(s, ' ', indent);
                }

                s = mempcpy(s, x + width * ಸಾಲು, act);
                len -= act;
        }
        ಪ್ರತಿಜ್ಞಾ(len == 0);

        *s = '\0';
        *ಪೂರ್ವಪ್ರತ್ಯಯ = t;
        ಹಿಂತಿರುಗಿ s - t;
}

ssize_t base64_append(
                ಅಕ್ಷರ **ಪೂರ್ವಪ್ರತ್ಯಯ,
                size_t plen,
                ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *p,
                size_t l,
                size_t indent,
                size_t width) {

        ಆದರೆ (plen > width / 2 || plen + indent > width)
                /* leave indent on the left, keep last column free */
                ಹಿಂತಿರುಗಿ base64_append_ಅಗಲ(ಪೂರ್ವಪ್ರತ್ಯಯ, plen, '\n', indent, p, l, width - indent);
        ಇಲ್ಲದಿದ್ದರೆ
                /* leave plen on the left, keep last column free */
                ಹಿಂತಿರುಗಿ base64_append_ಅಗಲ(ಪೂರ್ವಪ್ರತ್ಯಯ, plen, ' ', plen + 1, p, l, width - plen - 1);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ unbase64_ಮುಂದಿನ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **p, size_t *l) {
        ಪೂರ್ಣಾಂಕ ret;

        ಪ್ರತಿಜ್ಞಾ(p);
        ಪ್ರತಿಜ್ಞಾ(l);

        /* Find the next non-whitespace character, and decode it. If we find padding, we return it as INT_MAX. We
         * greedily skip all preceding and all following whitespace. */

        ಗಾಗಿ (;;) {
                ಆದರೆ (*l == 0)
                        ಹಿಂತಿರುಗಿ -EPIPE;

                ಆದರೆ (!strchr(WHITESPACE, **p))
                        ನಿಲ್ಲಿಸು;

                /* Skip leading whitespace */
                (*p)++, (*l)--;
        }

        ಆದರೆ (**p == '=')
                ret = ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ; /* return padding as INT_MAX */
        ಇಲ್ಲದಿದ್ದರೆ {
                ret = unbase64char(**p);
                ಆದರೆ (ret < 0)
                        ಹಿಂತಿರುಗಿ ret;
        }

        ಗಾಗಿ (;;) {
                (*p)++, (*l)--;

                ಆದರೆ (*l == 0)
                        ನಿಲ್ಲಿಸು;
                ಆದರೆ (!strchr(WHITESPACE, **p))
                        ನಿಲ್ಲಿಸು;

                /* Skip following whitespace */
        }

        ಹಿಂತಿರುಗಿ ret;
}

ಪೂರ್ಣಾಂಕ unbase64mem_ತುಂಬಿದ(
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p,
                size_t l,
                bool secure,
                ಶೂನ್ಯ **ret_ದತ್ತಾಂಶ,
                size_t *ret_ಗಾತ್ರ) {

        _cleanup_free_ uint8_t *buf = ಖಾಲಿ;
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *x;
        uint8_t *z;
        size_t len;

        ಪ್ರತಿಜ್ಞಾ(p || l == 0);

        ಆದರೆ (l == SIZE_MAX)
                l = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(p);

        /* A group of four input bytes needs three output bytes, in case of padding we need to add two or three extra
         * bytes. Note that this calculation is an upper boundary, as we ignore whitespace while decoding */
        len = (l / 4) * 3 + (l % 4 != 0 ? (l % 4) - 1 : 0);

        buf = ಸ್ಮರಣೆ_ಕೊಡು(len + 1);
        ಆದರೆ (!buf)
                ಹಿಂತಿರುಗಿ -ENOMEM;

        CLEANUP_ERASE_PTR(secure ? &buf : ಖಾಲಿ, len);

        ಗಾಗಿ (x = p, z = buf;;) {
                ಪೂರ್ಣಾಂಕ a, b, c, d; /* a == 00XXXXXX; b == 00YYYYYY; c == 00ZZZZZZ; d == 00WWWWWW */

                a = unbase64_ಮುಂದಿನ(&x, &l);
                ಆದರೆ (a == -EPIPE) /* End of string */
                        ನಿಲ್ಲಿಸು;
                ಆದರೆ (a < 0)
                        ಹಿಂತಿರುಗಿ a;
                ಆದರೆ (a == ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ) /* Padding is not allowed at the beginning of a 4ch block */
                        ಹಿಂತಿರುಗಿ -EINVAL;

                b = unbase64_ಮುಂದಿನ(&x, &l);
                ಆದರೆ (b < 0)
                        ಹಿಂತಿರುಗಿ b;
                ಆದರೆ (b == ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ) /* Padding is not allowed at the second character of a 4ch block either */
                        ಹಿಂತಿರುಗಿ -EINVAL;

                c = unbase64_ಮುಂದಿನ(&x, &l);
                ಆದರೆ (c < 0)
                        ಹಿಂತಿರುಗಿ c;

                d = unbase64_ಮುಂದಿನ(&x, &l);
                ಆದರೆ (d < 0)
                        ಹಿಂತಿರುಗಿ d;

                ಆದರೆ (c == ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ) { /* Padding at the third character */

                        ಆದರೆ (d != ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ) /* If the third character is padding, the fourth must be too */
                                ಹಿಂತಿರುಗಿ -EINVAL;

                        /* b == 00YY0000 */
                        ಆದರೆ (b & 15)
                                ಹಿಂತಿರುಗಿ -EINVAL;

                        ಆದರೆ (l > 0) /* Trailing rubbish? */
                                ಹಿಂತಿರುಗಿ -ENAMETOOLONG;

                        *(z++) = (uint8_t) a << 2 | (uint8_t) (b >> 4); /* XXXXXXYY */
                        ನಿಲ್ಲಿಸು;
                }

                ಆದರೆ (d == ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ) {
                        /* c == 00ZZZZ00 */
                        ಆದರೆ (c & 3)
                                ಹಿಂತಿರುಗಿ -EINVAL;

                        ಆದರೆ (l > 0) /* Trailing rubbish? */
                                ಹಿಂತಿರುಗಿ -ENAMETOOLONG;

                        *(z++) = (uint8_t) a << 2 | (uint8_t) b >> 4; /* XXXXXXYY */
                        *(z++) = (uint8_t) b << 4 | (uint8_t) c >> 2; /* YYYYZZZZ */
                        ನಿಲ್ಲಿಸು;
                }

                *(z++) = (uint8_t) a << 2 | (uint8_t) b >> 4; /* XXXXXXYY */
                *(z++) = (uint8_t) b << 4 | (uint8_t) c >> 2; /* YYYYZZZZ */
                *(z++) = (uint8_t) c << 6 | (uint8_t) d;      /* ZZWWWWWW */
        }

        *z = 0;

        ಪ್ರತಿಜ್ಞಾ((size_t) (z - buf) <= len);

        ಆದರೆ (ret_ಗಾತ್ರ)
                *ret_ಗಾತ್ರ = (size_t) (z - buf);
        ಆದರೆ (ret_ದತ್ತಾಂಶ)
                *ret_ದತ್ತಾಂಶ = TAKE_PTR(buf);

        ಹಿಂತಿರುಗಿ 0;
}

ಶೂನ್ಯ hexdump(FILE *f, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *p, size_t s) {
        ಸ್ಥಿರಾಂಕ uint8_t *b = p;
        ಚಿಹ್ನೆರಹಿತ n = 0;

        ಪ್ರತಿಜ್ಞಾ(b || s == 0);

        ಆದರೆ (s == SIZE_MAX)
                s = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(p);

        ಆದರೆ (!f)
                f = stdout;

        ಆಗ (s > 0) {
                size_t i;

                fprintf(f, "%04x  ", n);

                ಗಾಗಿ (i = 0; i < 16; i++) {

                        ಆದರೆ (i >= s)
                                fputs("   ", f);
                        ಇಲ್ಲದಿದ್ದರೆ
                                fprintf(f, "%02x ", b[i]);

                        ಆದರೆ (i == 7)
                                fputc(' ', f);
                }

                fputc(' ', f);

                ಗಾಗಿ (i = 0; i < 16; i++) {

                        ಆದರೆ (i >= s)
                                fputc(' ', f);
                        ಇಲ್ಲದಿದ್ದರೆ
                                fputc(isprint(b[i]) ? (ಅಕ್ಷರ) b[i] : '.', f);
                }

                fputc('\n', f);

                ಆದರೆ (s < 16)
                        ನಿಲ್ಲಿಸು;

                n += 16;
                b += 16;
                s -= 16;
        }
}
