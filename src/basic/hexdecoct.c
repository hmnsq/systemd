/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ <ctype.h>
#ಅನ್ತರ್ಭಾವ <stdio.h>

#ಅನ್ತರ್ಭಾವ "alloc-util.h"
#ಅನ್ತರ್ಭಾವ "hexdecoct.h"
#ಅನ್ತರ್ಭಾವ "memory-util.h"
#ಅನ್ತರ್ಭಾವ "string-util.h"

ವರ್ಣ octchar(ಪೂರ್ಣಾಙ್ಕ x) {
        ಪ್ರತ್ಯಾಗಮ '0' + (x & 7);
}

ಪೂರ್ಣಾಙ್ಕ unoctchar(ವರ್ಣ c) {

        ಯದಿ (c >= '0' && c <= '7')
                ಪ್ರತ್ಯಾಗಮ c - '0';

        ಪ್ರತ್ಯಾಗಮ -EINVAL;
}

ವರ್ಣ decchar(ಪೂರ್ಣಾಙ್ಕ x) {
        ಪ್ರತ್ಯಾಗಮ '0' + (x % 10);
}

ಪೂರ್ಣಾಙ್ಕ undecchar(ವರ್ಣ c) {

        ಯದಿ (c >= '0' && c <= '9')
                ಪ್ರತ್ಯಾಗಮ c - '0';

        ಪ್ರತ್ಯಾಗಮ -EINVAL;
}

ವರ್ಣ hexchar(ಪೂರ್ಣಾಙ್ಕ x) {
        ಧ್ರುವ ವರ್ಣ *table = LOWERCASE_HEXDIGITS;
        ಪ್ರತ್ಯಾಗಮ table[x & 15];
}

ಪೂರ್ಣಾಙ್ಕ unhexchar(ವರ್ಣ c) {

        ಯದಿ (c >= '0' && c <= '9')
                ಪ್ರತ್ಯಾಗಮ c - '0';

        ಯದಿ (c >= 'a' && c <= 'f')
                ಪ್ರತ್ಯಾಗಮ c - 'a' + 10;

        ಯದಿ (c >= 'A' && c <= 'F')
                ಪ್ರತ್ಯಾಗಮ c - 'A' + 10;

        ಪ್ರತ್ಯಾಗಮ -EINVAL;
}

ವರ್ಣ* hexmem(ಧ್ರುವ ಶೂನ್ಯ *p, size_t l) {
        ಧ್ರುವ uint8_t *x;
        ವರ್ಣ *r, *z;

        ಪ್ರತಿಜ್ಞಾ(p || l == 0);

        z = r = new(ವರ್ಣ, l * 2 + 1);
        ಯದಿ (!r)
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        ಪ್ರತಿ (x = p; x && x < (ಧ್ರುವ uint8_t*) p + l; x++) {
                *(z++) = hexchar(*x >> 4);
                *(z++) = hexchar(*x & 15);
        }

        *z = 0;
        ಪ್ರತ್ಯಾಗಮ r;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ unhex_next(ಧ್ರುವ ವರ್ಣ **p, size_t *l) {
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(p);
        ಪ್ರತಿಜ್ಞಾ(l);

        /* Find the next non-whitespace character, and decode it. We
         * greedily skip all preceding and all following whitespace. */

        ಪ್ರತಿ (;;) {
                ಯದಿ (*l == 0)
                        ಪ್ರತ್ಯಾಗಮ -EPIPE;

                ಯದಿ (!strchr(WHITESPACE, **p))
                        ಭಙ್ಗ;

                /* Skip leading whitespace */
                (*p)++, (*l)--;
        }

        r = unhexchar(**p);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಪ್ರತಿ (;;) {
                (*p)++, (*l)--;

                ಯದಿ (*l == 0 || !strchr(WHITESPACE, **p))
                        ಭಙ್ಗ;

                /* Skip following whitespace */
        }

        ಪ್ರತ್ಯಾಗಮ r;
}

ಪೂರ್ಣಾಙ್ಕ unhexmem_full(
                ಧ್ರುವ ವರ್ಣ *p,
                size_t l,
                bool secure,
                ಶೂನ್ಯ **ret_data,
                size_t *ret_size) {

        _cleanup_free_ uint8_t *buf = ರಿಕ್ತ;
        size_t buf_size;
        ಧ್ರುವ ವರ್ಣ *x;
        uint8_t *z;

        ಪ್ರತಿಜ್ಞಾ(p || l == 0);

        ಯದಿ (l == SIZE_MAX)
                l = strlen(p);

        /* Note that the calculation of memory size is an upper boundary, as we ignore whitespace while decoding */
        buf_size = (l + 1) / 2 + 1;
        buf = malloc(buf_size);
        ಯದಿ (!buf)
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        CLEANUP_ERASE_PTR(secure ? &buf : ರಿಕ್ತ, buf_size);

        ಪ್ರತಿ (x = p, z = buf;;) {
                ಪೂರ್ಣಾಙ್ಕ a, b;

                a = unhex_next(&x, &l);
                ಯದಿ (a == -EPIPE) /* End of string */
                        ಭಙ್ಗ;
                ಯದಿ (a < 0)
                        ಪ್ರತ್ಯಾಗಮ a;

                b = unhex_next(&x, &l);
                ಯದಿ (b < 0)
                        ಪ್ರತ್ಯಾಗಮ b;

                *(z++) = (uint8_t) a << 4 | (uint8_t) b;
        }

        *z = 0;

        ಯದಿ (ret_size)
                *ret_size = (size_t) (z - buf);
        ಯದಿ (ret_data)
                *ret_data = TAKE_PTR(buf);

        ಪ್ರತ್ಯಾಗಮ 0;
}

/* https://tools.ietf.org/html/rfc4648#section-6
 * Notice that base32hex differs from base32 in the alphabet it uses.
 * The distinction is that the base32hex representation preserves the
 * order of the underlying data when compared as bytestrings, this is
 * useful when representing NSEC3 hashes, as one can then verify the
 * order of hashes directly from their representation. */
ವರ್ಣ base32hexchar(ಪೂರ್ಣಾಙ್ಕ x) {
        ಧ್ರುವ ವರ್ಣ *table = DIGITS "ABCDEFGHIJKLMNOPQRSTUV";
        ಪ್ರತ್ಯಾಗಮ table[x & 31];
}

ಪೂರ್ಣಾಙ್ಕ unbase32hexchar(ವರ್ಣ c) {
        unsigned offset;

        ಯದಿ (c >= '0' && c <= '9')
                ಪ್ರತ್ಯಾಗಮ c - '0';

        offset = '9' - '0' + 1;

        ಯದಿ (c >= 'A' && c <= 'V')
                ಪ್ರತ್ಯಾಗಮ c - 'A' + offset;

        ಪ್ರತ್ಯಾಗಮ -EINVAL;
}

ವರ್ಣ* base32hexmem(ಧ್ರುವ ಶೂನ್ಯ *p, size_t l, bool padding) {
        ವರ್ಣ *r, *z;
        ಧ್ರುವ uint8_t *x;
        size_t len;

        ಪ್ರತಿಜ್ಞಾ(p || l == 0);

        ಯದಿ (padding)
                /* five input bytes makes eight output bytes, padding is added so we must round up */
                len = 8 * (l + 4) / 5;
        ಅನ್ಯಥಾ {
                /* same, but round down as there is no padding */
                len = 8 * l / 5;

                ವಿಕಲ್ಪ (l % 5) {
                ಪಕ್ಷ 4:
                        len += 7;
                        ಭಙ್ಗ;
                ಪಕ್ಷ 3:
                        len += 5;
                        ಭಙ್ಗ;
                ಪಕ್ಷ 2:
                        len += 4;
                        ಭಙ್ಗ;
                ಪಕ್ಷ 1:
                        len += 2;
                        ಭಙ್ಗ;
                }
        }

        z = r = malloc(len + 1);
        ಯದಿ (!r)
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        ಪ್ರತಿ (x = p; x < (ಧ್ರುವ uint8_t*) p + (l / 5) * 5; x += 5) {
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

        ವಿಕಲ್ಪ (l % 5) {
        ಪಕ್ಷ 4:
                *(z++) = base32hexchar(x[0] >> 3);                    /* 000XXXXX */
                *(z++) = base32hexchar((x[0] & 7) << 2 | x[1] >> 6);  /* 000XXXYY */
                *(z++) = base32hexchar((x[1] & 63) >> 1);             /* 000YYYYY */
                *(z++) = base32hexchar((x[1] & 1) << 4 | x[2] >> 4);   /* 000YZZZZ */
                *(z++) = base32hexchar((x[2] & 15) << 1 | x[3] >> 7); /* 000ZZZZQ */
                *(z++) = base32hexchar((x[3] & 127) >> 2);            /* 000QQQQQ */
                *(z++) = base32hexchar((x[3] & 3) << 3);              /* 000QQ000 */
                ಯದಿ (padding)
                        *(z++) = '=';

                ಭಙ್ಗ;

        ಪಕ್ಷ 3:
                *(z++) = base32hexchar(x[0] >> 3);                   /* 000XXXXX */
                *(z++) = base32hexchar((x[0] & 7) << 2 | x[1] >> 6); /* 000XXXYY */
                *(z++) = base32hexchar((x[1] & 63) >> 1);            /* 000YYYYY */
                *(z++) = base32hexchar((x[1] & 1) << 4 | x[2] >> 4); /* 000YZZZZ */
                *(z++) = base32hexchar((x[2] & 15) << 1);            /* 000ZZZZ0 */
                ಯದಿ (padding) {
                        *(z++) = '=';
                        *(z++) = '=';
                        *(z++) = '=';
                }

                ಭಙ್ಗ;

        ಪಕ್ಷ 2:
                *(z++) = base32hexchar(x[0] >> 3);                   /* 000XXXXX */
                *(z++) = base32hexchar((x[0] & 7) << 2 | x[1] >> 6); /* 000XXXYY */
                *(z++) = base32hexchar((x[1] & 63) >> 1);            /* 000YYYYY */
                *(z++) = base32hexchar((x[1] & 1) << 4);             /* 000Y0000 */
                ಯದಿ (padding) {
                        *(z++) = '=';
                        *(z++) = '=';
                        *(z++) = '=';
                        *(z++) = '=';
                }

                ಭಙ್ಗ;

        ಪಕ್ಷ 1:
                *(z++) = base32hexchar(x[0] >> 3);       /* 000XXXXX */
                *(z++) = base32hexchar((x[0] & 7) << 2); /* 000XXX00 */
                ಯದಿ (padding) {
                        *(z++) = '=';
                        *(z++) = '=';
                        *(z++) = '=';
                        *(z++) = '=';
                        *(z++) = '=';
                        *(z++) = '=';
                }

                ಭಙ್ಗ;
        }

        *z = 0;
        ಪ್ರತ್ಯಾಗಮ r;
}

ಪೂರ್ಣಾಙ್ಕ unbase32hexmem(ಧ್ರುವ ವರ್ಣ *p, size_t l, bool padding, ಶೂನ್ಯ **mem, size_t *_len) {
        _cleanup_free_ uint8_t *r = ರಿಕ್ತ;
        ಪೂರ್ಣಾಙ್ಕ a, b, c, d, e, f, g, h;
        uint8_t *z;
        ಧ್ರುವ ವರ್ಣ *x;
        size_t len;
        unsigned pad = 0;

        ಪ್ರತಿಜ್ಞಾ(p || l == 0);
        ಪ್ರತಿಜ್ಞಾ(mem);
        ಪ್ರತಿಜ್ಞಾ(_len);

        ಯದಿ (l == SIZE_MAX)
                l = strlen(p);

        /* padding ensures any base32hex input has input divisible by 8 */
        ಯದಿ (padding && l % 8 != 0)
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

        ಯದಿ (padding) {
                /* strip the padding */
                ಯಾವತ್ (l > 0 && p[l - 1] == '=' && pad < 7) {
                        pad++;
                        l--;
                }
        }

        /* a group of eight input bytes needs five output bytes, in case of
         * padding we need to add some extra bytes */
        len = (l / 8) * 5;

        ವಿಕಲ್ಪ (l % 8) {
        ಪಕ್ಷ 7:
                len += 4;
                ಭಙ್ಗ;
        ಪಕ್ಷ 5:
                len += 3;
                ಭಙ್ಗ;
        ಪಕ್ಷ 4:
                len += 2;
                ಭಙ್ಗ;
        ಪಕ್ಷ 2:
                len += 1;
                ಭಙ್ಗ;
        ಪಕ್ಷ 0:
                ಭಙ್ಗ;
        ಸ್ವಭಾವ:
                ಪ್ರತ್ಯಾಗಮ -EINVAL;
        }

        z = r = malloc(len + 1);
        ಯದಿ (!r)
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        ಪ್ರತಿ (x = p; x < p + (l / 8) * 8; x += 8) {
                /* a == 000XXXXX; b == 000YYYYY; c == 000ZZZZZ; d == 000WWWWW
                 * e == 000SSSSS; f == 000QQQQQ; g == 000VVVVV; h == 000RRRRR */
                a = unbase32hexchar(x[0]);
                ಯದಿ (a < 0)
                        ಪ್ರತ್ಯಾಗಮ -EINVAL;

                b = unbase32hexchar(x[1]);
                ಯದಿ (b < 0)
                        ಪ್ರತ್ಯಾಗಮ -EINVAL;

                c = unbase32hexchar(x[2]);
                ಯದಿ (c < 0)
                        ಪ್ರತ್ಯಾಗಮ -EINVAL;

                d = unbase32hexchar(x[3]);
                ಯದಿ (d < 0)
                        ಪ್ರತ್ಯಾಗಮ -EINVAL;

                e = unbase32hexchar(x[4]);
                ಯದಿ (e < 0)
                        ಪ್ರತ್ಯಾಗಮ -EINVAL;

                f = unbase32hexchar(x[5]);
                ಯದಿ (f < 0)
                        ಪ್ರತ್ಯಾಗಮ -EINVAL;

                g = unbase32hexchar(x[6]);
                ಯದಿ (g < 0)
                        ಪ್ರತ್ಯಾಗಮ -EINVAL;

                h = unbase32hexchar(x[7]);
                ಯದಿ (h < 0)
                        ಪ್ರತ್ಯಾಗಮ -EINVAL;

                *(z++) = (uint8_t) a << 3 | (uint8_t) b >> 2;                    /* XXXXXYYY */
                *(z++) = (uint8_t) b << 6 | (uint8_t) c << 1 | (uint8_t) d >> 4; /* YYZZZZZW */
                *(z++) = (uint8_t) d << 4 | (uint8_t) e >> 1;                    /* WWWWSSSS */
                *(z++) = (uint8_t) e << 7 | (uint8_t) f << 2 | (uint8_t) g >> 3; /* SQQQQQVV */
                *(z++) = (uint8_t) g << 5 | (uint8_t) h;                         /* VVVRRRRR */
        }

        ವಿಕಲ್ಪ (l % 8) {
        ಪಕ್ಷ 7:
                a = unbase32hexchar(x[0]);
                ಯದಿ (a < 0)
                        ಪ್ರತ್ಯಾಗಮ -EINVAL;

                b = unbase32hexchar(x[1]);
                ಯದಿ (b < 0)
                        ಪ್ರತ್ಯಾಗಮ -EINVAL;

                c = unbase32hexchar(x[2]);
                ಯದಿ (c < 0)
                        ಪ್ರತ್ಯಾಗಮ -EINVAL;

                d = unbase32hexchar(x[3]);
                ಯದಿ (d < 0)
                        ಪ್ರತ್ಯಾಗಮ -EINVAL;

                e = unbase32hexchar(x[4]);
                ಯದಿ (e < 0)
                        ಪ್ರತ್ಯಾಗಮ -EINVAL;

                f = unbase32hexchar(x[5]);
                ಯದಿ (f < 0)
                        ಪ್ರತ್ಯಾಗಮ -EINVAL;

                g = unbase32hexchar(x[6]);
                ಯದಿ (g < 0)
                        ಪ್ರತ್ಯಾಗಮ -EINVAL;

                /* g == 000VV000 */
                ಯದಿ (g & 7)
                        ಪ್ರತ್ಯಾಗಮ -EINVAL;

                *(z++) = (uint8_t) a << 3 | (uint8_t) b >> 2;                    /* XXXXXYYY */
                *(z++) = (uint8_t) b << 6 | (uint8_t) c << 1 | (uint8_t) d >> 4; /* YYZZZZZW */
                *(z++) = (uint8_t) d << 4 | (uint8_t) e >> 1;                    /* WWWWSSSS */
                *(z++) = (uint8_t) e << 7 | (uint8_t) f << 2 | (uint8_t) g >> 3; /* SQQQQQVV */

                ಭಙ್ಗ;
        ಪಕ್ಷ 5:
                a = unbase32hexchar(x[0]);
                ಯದಿ (a < 0)
                        ಪ್ರತ್ಯಾಗಮ -EINVAL;

                b = unbase32hexchar(x[1]);
                ಯದಿ (b < 0)
                        ಪ್ರತ್ಯಾಗಮ -EINVAL;

                c = unbase32hexchar(x[2]);
                ಯದಿ (c < 0)
                        ಪ್ರತ್ಯಾಗಮ -EINVAL;

                d = unbase32hexchar(x[3]);
                ಯದಿ (d < 0)
                        ಪ್ರತ್ಯಾಗಮ -EINVAL;

                e = unbase32hexchar(x[4]);
                ಯದಿ (e < 0)
                        ಪ್ರತ್ಯಾಗಮ -EINVAL;

                /* e == 000SSSS0 */
                ಯದಿ (e & 1)
                        ಪ್ರತ್ಯಾಗಮ -EINVAL;

                *(z++) = (uint8_t) a << 3 | (uint8_t) b >> 2;                    /* XXXXXYYY */
                *(z++) = (uint8_t) b << 6 | (uint8_t) c << 1 | (uint8_t) d >> 4; /* YYZZZZZW */
                *(z++) = (uint8_t) d << 4 | (uint8_t) e >> 1;                    /* WWWWSSSS */

                ಭಙ್ಗ;
        ಪಕ್ಷ 4:
                a = unbase32hexchar(x[0]);
                ಯದಿ (a < 0)
                        ಪ್ರತ್ಯಾಗಮ -EINVAL;

                b = unbase32hexchar(x[1]);
                ಯದಿ (b < 0)
                        ಪ್ರತ್ಯಾಗಮ -EINVAL;

                c = unbase32hexchar(x[2]);
                ಯದಿ (c < 0)
                        ಪ್ರತ್ಯಾಗಮ -EINVAL;

                d = unbase32hexchar(x[3]);
                ಯದಿ (d < 0)
                        ಪ್ರತ್ಯಾಗಮ -EINVAL;

                /* d == 000W0000 */
                ಯದಿ (d & 15)
                        ಪ್ರತ್ಯಾಗಮ -EINVAL;

                *(z++) = (uint8_t) a << 3 | (uint8_t) b >> 2;                    /* XXXXXYYY */
                *(z++) = (uint8_t) b << 6 | (uint8_t) c << 1 | (uint8_t) d >> 4; /* YYZZZZZW */

                ಭಙ್ಗ;
        ಪಕ್ಷ 2:
                a = unbase32hexchar(x[0]);
                ಯದಿ (a < 0)
                        ಪ್ರತ್ಯಾಗಮ -EINVAL;

                b = unbase32hexchar(x[1]);
                ಯದಿ (b < 0)
                        ಪ್ರತ್ಯಾಗಮ -EINVAL;

                /* b == 000YYY00 */
                ಯದಿ (b & 3)
                        ಪ್ರತ್ಯಾಗಮ -EINVAL;

                *(z++) = (uint8_t) a << 3 | (uint8_t) b >> 2; /* XXXXXYYY */

                ಭಙ್ಗ;
        ಪಕ್ಷ 0:
                ಭಙ್ಗ;
        ಸ್ವಭಾವ:
                ಪ್ರತ್ಯಾಗಮ -EINVAL;
        }

        *z = 0;

        *mem = TAKE_PTR(r);
        *_len = len;

        ಪ್ರತ್ಯಾಗಮ 0;
}

/* https://tools.ietf.org/html/rfc4648#section-4 */
ವರ್ಣ base64char(ಪೂರ್ಣಾಙ್ಕ x) {
        ಧ್ರುವ ವರ್ಣ *table = UPPERCASE_LETTERS LOWERCASE_LETTERS DIGITS "+/";
        ಪ್ರತ್ಯಾಗಮ table[x & 63];
}

/* This is almost base64char(), but not entirely, as it uses the "url and filename safe" alphabet,
 * since we don't want "/" appear in interface names (since interfaces appear in sysfs as filenames).
 * See section #5 of RFC 4648. */
ವರ್ಣ urlsafe_base64char(ಪೂರ್ಣಾಙ್ಕ x) {
        ಧ್ರುವ ವರ್ಣ *table = UPPERCASE_LETTERS LOWERCASE_LETTERS DIGITS "-_";
        ಪ್ರತ್ಯಾಗಮ table[x & 63];
}

ಪೂರ್ಣಾಙ್ಕ unbase64char(ವರ್ಣ c) {
        unsigned offset;

        ಯದಿ (c >= 'A' && c <= 'Z')
                ಪ್ರತ್ಯಾಗಮ c - 'A';

        offset = 'Z' - 'A' + 1;

        ಯದಿ (c >= 'a' && c <= 'z')
                ಪ್ರತ್ಯಾಗಮ c - 'a' + offset;

        offset += 'z' - 'a' + 1;

        ಯದಿ (c >= '0' && c <= '9')
                ಪ್ರತ್ಯಾಗಮ c - '0' + offset;

        offset += '9' - '0' + 1;

        ಯದಿ (IN_SET(c, '+', '-')) /* Support both the regular and the URL safe character set (see above) */
                ಪ್ರತ್ಯಾಗಮ offset;

        offset++;

        ಯದಿ (IN_SET(c, '/', '_')) /* ditto */
                ಪ್ರತ್ಯಾಗಮ offset;

        ಪ್ರತ್ಯಾಗಮ -EINVAL;
}

ಸ್ಥಿರ ಶೂನ್ಯ maybe_ಸಾಲು_break(ವರ್ಣ **x, ವರ್ಣ *start, size_t ಸಾಲು_break) {
        size_t n;

        ಪ್ರತಿಜ್ಞಾ(x);
        ಪ್ರತಿಜ್ಞಾ(*x);
        ಪ್ರತಿಜ್ಞಾ(start);
        ಪ್ರತಿಜ್ಞಾ(*x >= start);

        ಯದಿ (ಸಾಲು_break == SIZE_MAX)
                ಪ್ರತ್ಯಾಗಮ;

        n = *x - start;

        ಯದಿ (n % (ಸಾಲು_break + 1) == ಸಾಲು_break)
                *((*x)++) = '\n';
}

ssize_t base64mem_full(
                ಧ್ರುವ ಶೂನ್ಯ *p,
                size_t l,
                size_t ಸಾಲು_break,
                ವರ್ಣ **ret) {

        ಧ್ರುವ uint8_t *x;
        ವರ್ಣ *b, *z;
        size_t m;

        ಪ್ರತಿಜ್ಞಾ(p || l == 0);
        ಪ್ರತಿಜ್ಞಾ(ಸಾಲು_break > 0);
        ಪ್ರತಿಜ್ಞಾ(ret);

        /* three input bytes makes four output bytes, padding is added so we must round up */
        m = 4 * (l + 2) / 3 + 1;
        ಯದಿ (ಸಾಲು_break != SIZE_MAX)
                m += m / ಸಾಲು_break;

        z = b = malloc(m);
        ಯದಿ (!b)
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        ಪ್ರತಿ (x = p; x && x < (ಧ್ರುವ uint8_t*) p + (l / 3) * 3; x += 3) {
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

        ವಿಕಲ್ಪ (l % 3) {
        ಪಕ್ಷ 2:
                maybe_ಸಾಲು_break(&z, b, ಸಾಲು_break);
                *(z++) = base64char(x[0] >> 2);                   /* 00XXXXXX */
                maybe_ಸಾಲು_break(&z, b, ಸಾಲು_break);
                *(z++) = base64char((x[0] & 3) << 4 | x[1] >> 4); /* 00XXYYYY */
                maybe_ಸಾಲು_break(&z, b, ಸಾಲು_break);
                *(z++) = base64char((x[1] & 15) << 2);            /* 00YYYY00 */
                maybe_ಸಾಲು_break(&z, b, ಸಾಲು_break);
                *(z++) = '=';
                ಭಙ್ಗ;

        ಪಕ್ಷ 1:
                maybe_ಸಾಲು_break(&z, b, ಸಾಲು_break);
                *(z++) = base64char(x[0] >> 2);        /* 00XXXXXX */
                maybe_ಸಾಲು_break(&z, b, ಸಾಲು_break);
                *(z++) = base64char((x[0] & 3) << 4);  /* 00XX0000 */
                maybe_ಸಾಲು_break(&z, b, ಸಾಲು_break);
                *(z++) = '=';
                maybe_ಸಾಲು_break(&z, b, ಸಾಲು_break);
                *(z++) = '=';
                ಭಙ್ಗ;
        }

        *z = 0;
        *ret = b;

        ಪ್ರತಿಜ್ಞಾ(z >= b); /* Let static analyzers know that the answer is non-negative. */
        ಪ್ರತ್ಯಾಗಮ z - b;
}

ಸ್ಥಿರ ssize_t base64_append_width(
                ವರ್ಣ **prefix,
                size_t plen,
                ವರ್ಣ sep,
                size_t indent,
                ಧ್ರುವ ಶೂನ್ಯ *p,
                size_t l,
                size_t width) {

        _cleanup_free_ ವರ್ಣ *x = ರಿಕ್ತ;
        ವರ್ಣ *t, *s;
        size_t ಸಾಲುಗಳು;
        ssize_t len;

        ಪ್ರತಿಜ್ಞಾ(prefix);
        ಪ್ರತಿಜ್ಞಾ(*prefix || plen == 0);
        ಪ್ರತಿಜ್ಞಾ(p || l == 0);

        len = base64mem(p, l, &x);
        ಯದಿ (len < 0)
                ಪ್ರತ್ಯಾಗಮ len;
        ಯದಿ (len == 0)
                ಪ್ರತ್ಯಾಗಮ plen;

        ಸಾಲುಗಳು = DIV_ROUND_UP(len, width);

        ಯದಿ (plen >= SSIZE_MAX - 1 - 1 ||
            ಸಾಲುಗಳು > (SSIZE_MAX - plen - 1 - 1) / (indent + width + 1))
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        t = realloc(*prefix, plen + 1 + 1 + (indent + width + 1) * ಸಾಲುಗಳು);
        ಯದಿ (!t)
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        s = t + plen;
        ಪ್ರತಿ (size_t ಸಾಲು = 0; ಸಾಲು < ಸಾಲುಗಳು; ಸಾಲು++) {
                size_t act = MIN(width, (size_t) len);

                ಯದಿ (ಸಾಲು > 0)
                        sep = '\n';

                ಯದಿ (s > t) {
                        *s++ = sep;
                        ಯದಿ (sep == '\n')
                                s = mempset(s, ' ', indent);
                }

                s = mempcpy(s, x + width * ಸಾಲು, act);
                len -= act;
        }
        ಪ್ರತಿಜ್ಞಾ(len == 0);

        *s = '\0';
        *prefix = t;
        ಪ್ರತ್ಯಾಗಮ s - t;
}

ssize_t base64_append(
                ವರ್ಣ **prefix,
                size_t plen,
                ಧ್ರುವ ಶೂನ್ಯ *p,
                size_t l,
                size_t indent,
                size_t width) {

        ಯದಿ (plen > width / 2 || plen + indent > width)
                /* leave indent on the left, keep last column free */
                ಪ್ರತ್ಯಾಗಮ base64_append_width(prefix, plen, '\n', indent, p, l, width - indent);
        ಅನ್ಯಥಾ
                /* leave plen on the left, keep last column free */
                ಪ್ರತ್ಯಾಗಮ base64_append_width(prefix, plen, ' ', plen + 1, p, l, width - plen - 1);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ unbase64_next(ಧ್ರುವ ವರ್ಣ **p, size_t *l) {
        ಪೂರ್ಣಾಙ್ಕ ret;

        ಪ್ರತಿಜ್ಞಾ(p);
        ಪ್ರತಿಜ್ಞಾ(l);

        /* Find the next non-whitespace character, and decode it. If we find padding, we return it as INT_MAX. We
         * greedily skip all preceding and all following whitespace. */

        ಪ್ರತಿ (;;) {
                ಯದಿ (*l == 0)
                        ಪ್ರತ್ಯಾಗಮ -EPIPE;

                ಯದಿ (!strchr(WHITESPACE, **p))
                        ಭಙ್ಗ;

                /* Skip leading whitespace */
                (*p)++, (*l)--;
        }

        ಯದಿ (**p == '=')
                ret = INT_MAX; /* return padding as INT_MAX */
        ಅನ್ಯಥಾ {
                ret = unbase64char(**p);
                ಯದಿ (ret < 0)
                        ಪ್ರತ್ಯಾಗಮ ret;
        }

        ಪ್ರತಿ (;;) {
                (*p)++, (*l)--;

                ಯದಿ (*l == 0)
                        ಭಙ್ಗ;
                ಯದಿ (!strchr(WHITESPACE, **p))
                        ಭಙ್ಗ;

                /* Skip following whitespace */
        }

        ಪ್ರತ್ಯಾಗಮ ret;
}

ಪೂರ್ಣಾಙ್ಕ unbase64mem_full(
                ಧ್ರುವ ವರ್ಣ *p,
                size_t l,
                bool secure,
                ಶೂನ್ಯ **ret_data,
                size_t *ret_size) {

        _cleanup_free_ uint8_t *buf = ರಿಕ್ತ;
        ಧ್ರುವ ವರ್ಣ *x;
        uint8_t *z;
        size_t len;

        ಪ್ರತಿಜ್ಞಾ(p || l == 0);

        ಯದಿ (l == SIZE_MAX)
                l = strlen(p);

        /* A group of four input bytes needs three output bytes, in case of padding we need to add two or three extra
         * bytes. Note that this calculation is an upper boundary, as we ignore whitespace while decoding */
        len = (l / 4) * 3 + (l % 4 != 0 ? (l % 4) - 1 : 0);

        buf = malloc(len + 1);
        ಯದಿ (!buf)
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        CLEANUP_ERASE_PTR(secure ? &buf : ರಿಕ್ತ, len);

        ಪ್ರತಿ (x = p, z = buf;;) {
                ಪೂರ್ಣಾಙ್ಕ a, b, c, d; /* a == 00XXXXXX; b == 00YYYYYY; c == 00ZZZZZZ; d == 00WWWWWW */

                a = unbase64_next(&x, &l);
                ಯದಿ (a == -EPIPE) /* End of string */
                        ಭಙ್ಗ;
                ಯದಿ (a < 0)
                        ಪ್ರತ್ಯಾಗಮ a;
                ಯದಿ (a == INT_MAX) /* Padding is not allowed at the beginning of a 4ch block */
                        ಪ್ರತ್ಯಾಗಮ -EINVAL;

                b = unbase64_next(&x, &l);
                ಯದಿ (b < 0)
                        ಪ್ರತ್ಯಾಗಮ b;
                ಯದಿ (b == INT_MAX) /* Padding is not allowed at the second character of a 4ch block either */
                        ಪ್ರತ್ಯಾಗಮ -EINVAL;

                c = unbase64_next(&x, &l);
                ಯದಿ (c < 0)
                        ಪ್ರತ್ಯಾಗಮ c;

                d = unbase64_next(&x, &l);
                ಯದಿ (d < 0)
                        ಪ್ರತ್ಯಾಗಮ d;

                ಯದಿ (c == INT_MAX) { /* Padding at the third character */

                        ಯದಿ (d != INT_MAX) /* If the third character is padding, the fourth must be too */
                                ಪ್ರತ್ಯಾಗಮ -EINVAL;

                        /* b == 00YY0000 */
                        ಯದಿ (b & 15)
                                ಪ್ರತ್ಯಾಗಮ -EINVAL;

                        ಯದಿ (l > 0) /* Trailing rubbish? */
                                ಪ್ರತ್ಯಾಗಮ -ENAMETOOLONG;

                        *(z++) = (uint8_t) a << 2 | (uint8_t) (b >> 4); /* XXXXXXYY */
                        ಭಙ್ಗ;
                }

                ಯದಿ (d == INT_MAX) {
                        /* c == 00ZZZZ00 */
                        ಯದಿ (c & 3)
                                ಪ್ರತ್ಯಾಗಮ -EINVAL;

                        ಯದಿ (l > 0) /* Trailing rubbish? */
                                ಪ್ರತ್ಯಾಗಮ -ENAMETOOLONG;

                        *(z++) = (uint8_t) a << 2 | (uint8_t) b >> 4; /* XXXXXXYY */
                        *(z++) = (uint8_t) b << 4 | (uint8_t) c >> 2; /* YYYYZZZZ */
                        ಭಙ್ಗ;
                }

                *(z++) = (uint8_t) a << 2 | (uint8_t) b >> 4; /* XXXXXXYY */
                *(z++) = (uint8_t) b << 4 | (uint8_t) c >> 2; /* YYYYZZZZ */
                *(z++) = (uint8_t) c << 6 | (uint8_t) d;      /* ZZWWWWWW */
        }

        *z = 0;

        ಪ್ರತಿಜ್ಞಾ((size_t) (z - buf) <= len);

        ಯದಿ (ret_size)
                *ret_size = (size_t) (z - buf);
        ಯದಿ (ret_data)
                *ret_data = TAKE_PTR(buf);

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಶೂನ್ಯ hexdump(FILE *f, ಧ್ರುವ ಶೂನ್ಯ *p, size_t s) {
        ಧ್ರುವ uint8_t *b = p;
        unsigned n = 0;

        ಪ್ರತಿಜ್ಞಾ(b || s == 0);

        ಯದಿ (s == SIZE_MAX)
                s = strlen(p);

        ಯದಿ (!f)
                f = stdout;

        ಯಾವತ್ (s > 0) {
                size_t i;

                fprintf(f, "%04x  ", n);

                ಪ್ರತಿ (i = 0; i < 16; i++) {

                        ಯದಿ (i >= s)
                                fputs("   ", f);
                        ಅನ್ಯಥಾ
                                fprintf(f, "%02x ", b[i]);

                        ಯದಿ (i == 7)
                                fputc(' ', f);
                }

                fputc(' ', f);

                ಪ್ರತಿ (i = 0; i < 16; i++) {

                        ಯದಿ (i >= s)
                                fputc(' ', f);
                        ಅನ್ಯಥಾ
                                fputc(isprint(b[i]) ? (ವರ್ಣ) b[i] : '.', f);
                }

                fputc('\n', f);

                ಯದಿ (s < 16)
                        ಭಙ್ಗ;

                n += 16;
                b += 16;
                s -= 16;
        }
}
