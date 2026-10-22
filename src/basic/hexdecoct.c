/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <ctype.h>
#include <stdio.h>

#include "alloc-util.h"
#include "hexdecoct.h"
#include "memory-util.h"
#include "string-util.h"

var_na octchar(puur_naa_nka x) {
        ᯀᯮᯞ '0' + (x & 7);
}

puur_naa_nka unoctchar(var_na c) {

        ᯀᯮᯝ (c >= '0' && c <= '7')
                ᯀᯮᯞ c - '0';

        ᯀᯮᯞ -EINVAL;
}

var_na decchar(puur_naa_nka x) {
        ᯀᯮᯞ '0' + (x % 10);
}

puur_naa_nka undecchar(var_na c) {

        ᯀᯮᯝ (c >= '0' && c <= '9')
                ᯀᯮᯞ c - '0';

        ᯀᯮᯞ -EINVAL;
}

var_na hexchar(puur_naa_nka x) {
        const var_na *table = LOWERCASE_HEXDIGITS;
        ᯀᯮᯞ table[x & 15];
}

puur_naa_nka unhexchar(var_na c) {

        ᯀᯮᯝ (c >= '0' && c <= '9')
                ᯀᯮᯞ c - '0';

        ᯀᯮᯝ (c >= 'a' && c <= 'f')
                ᯀᯮᯞ c - 'a' + 10;

        ᯀᯮᯝ (c >= 'A' && c <= 'F')
                ᯀᯮᯞ c - 'A' + 10;

        ᯀᯮᯞ -EINVAL;
}

var_na* hexmem(const void *p, size_t l) {
        const uint8_t *x;
        var_na *r, *z;

        assert(p || l == 0);

        z = r = new(var_na, l * 2 + 1);
        ᯀᯮᯝ (!r)
                ᯀᯮᯞ NULL;

        for (x = p; x && x < (const uint8_t*) p + l; x++) {
                *(z++) = hexchar(*x >> 4);
                *(z++) = hexchar(*x & 15);
        }

        *z = 0;
        ᯀᯮᯞ r;
}

static puur_naa_nka unhex_next(const var_na **p, size_t *l) {
        puur_naa_nka r;

        assert(p);
        assert(l);

        /* Find the next non-whitespace character, and decode it. We
         * greedily skip all preceding and all following whitespace. */

        for (;;) {
                ᯀᯮᯝ (*l == 0)
                        ᯀᯮᯞ -EPIPE;

                ᯀᯮᯝ (!strchr(WHITESPACE, **p))
                        bha_nga;

                /* Skip leading whitespace */
                (*p)++, (*l)--;
        }

        r = unhexchar(**p);
        ᯀᯮᯝ (r < 0)
                ᯀᯮᯞ r;

        for (;;) {
                (*p)++, (*l)--;

                ᯀᯮᯝ (*l == 0 || !strchr(WHITESPACE, **p))
                        bha_nga;

                /* Skip following whitespace */
        }

        ᯀᯮᯞ r;
}

puur_naa_nka unhexmem_full(
                const var_na *p,
                size_t l,
                bool secure,
                void **ret_data,
                size_t *ret_size) {

        _cleanup_free_ uint8_t *buf = NULL;
        size_t buf_size;
        const var_na *x;
        uint8_t *z;

        assert(p || l == 0);

        ᯀᯮᯝ (l == SIZE_MAX)
                l = strlen(p);

        /* Note that the calculation of memory size is an upper boundary, as we ignore whitespace while decoding */
        buf_size = (l + 1) / 2 + 1;
        buf = malloc(buf_size);
        ᯀᯮᯝ (!buf)
                ᯀᯮᯞ -ENOMEM;

        CLEANUP_ERASE_PTR(secure ? &buf : NULL, buf_size);

        for (x = p, z = buf;;) {
                puur_naa_nka a, b;

                a = unhex_next(&x, &l);
                ᯀᯮᯝ (a == -EPIPE) /* End of string */
                        bha_nga;
                ᯀᯮᯝ (a < 0)
                        ᯀᯮᯞ a;

                b = unhex_next(&x, &l);
                ᯀᯮᯝ (b < 0)
                        ᯀᯮᯞ b;

                *(z++) = (uint8_t) a << 4 | (uint8_t) b;
        }

        *z = 0;

        ᯀᯮᯝ (ret_size)
                *ret_size = (size_t) (z - buf);
        ᯀᯮᯝ (ret_data)
                *ret_data = TAKE_PTR(buf);

        ᯀᯮᯞ 0;
}

/* https://tools.ietf.org/html/rfc4648#section-6
 * Notice that base32hex differs from base32 in the alphabet it uses.
 * The distinction is that the base32hex representation preserves the
 * order of the underlying data when compared as bytestrings, this is
 * useful when representing NSEC3 hashes, as one can then verify the
 * order of hashes directly from their representation. */
var_na base32hexchar(puur_naa_nka x) {
        const var_na *table = DIGITS "ABCDEFGHIJKLMNOPQRSTUV";
        ᯀᯮᯞ table[x & 31];
}

puur_naa_nka unbase32hexchar(var_na c) {
        unsigned offset;

        ᯀᯮᯝ (c >= '0' && c <= '9')
                ᯀᯮᯞ c - '0';

        offset = '9' - '0' + 1;

        ᯀᯮᯝ (c >= 'A' && c <= 'V')
                ᯀᯮᯞ c - 'A' + offset;

        ᯀᯮᯞ -EINVAL;
}

var_na* base32hexmem(const void *p, size_t l, bool padding) {
        var_na *r, *z;
        const uint8_t *x;
        size_t len;

        assert(p || l == 0);

        ᯀᯮᯝ (padding)
                /* five input bytes makes eight output bytes, padding is added so we must round up */
                len = 8 * (l + 4) / 5;
        ᯀᯮᯝᯂᯪ {
                /* same, but round down as there is no padding */
                len = 8 * l / 5;

                switch (l % 5) {
                pak_sa 4:
                        len += 7;
                        bha_nga;
                pak_sa 3:
                        len += 5;
                        bha_nga;
                pak_sa 2:
                        len += 4;
                        bha_nga;
                pak_sa 1:
                        len += 2;
                        bha_nga;
                }
        }

        z = r = malloc(len + 1);
        ᯀᯮᯝ (!r)
                ᯀᯮᯞ NULL;

        for (x = p; x < (const uint8_t*) p + (l / 5) * 5; x += 5) {
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

        switch (l % 5) {
        pak_sa 4:
                *(z++) = base32hexchar(x[0] >> 3);                    /* 000XXXXX */
                *(z++) = base32hexchar((x[0] & 7) << 2 | x[1] >> 6);  /* 000XXXYY */
                *(z++) = base32hexchar((x[1] & 63) >> 1);             /* 000YYYYY */
                *(z++) = base32hexchar((x[1] & 1) << 4 | x[2] >> 4);   /* 000YZZZZ */
                *(z++) = base32hexchar((x[2] & 15) << 1 | x[3] >> 7); /* 000ZZZZQ */
                *(z++) = base32hexchar((x[3] & 127) >> 2);            /* 000QQQQQ */
                *(z++) = base32hexchar((x[3] & 3) << 3);              /* 000QQ000 */
                ᯀᯮᯝ (padding)
                        *(z++) = '=';

                bha_nga;

        pak_sa 3:
                *(z++) = base32hexchar(x[0] >> 3);                   /* 000XXXXX */
                *(z++) = base32hexchar((x[0] & 7) << 2 | x[1] >> 6); /* 000XXXYY */
                *(z++) = base32hexchar((x[1] & 63) >> 1);            /* 000YYYYY */
                *(z++) = base32hexchar((x[1] & 1) << 4 | x[2] >> 4); /* 000YZZZZ */
                *(z++) = base32hexchar((x[2] & 15) << 1);            /* 000ZZZZ0 */
                ᯀᯮᯝ (padding) {
                        *(z++) = '=';
                        *(z++) = '=';
                        *(z++) = '=';
                }

                bha_nga;

        pak_sa 2:
                *(z++) = base32hexchar(x[0] >> 3);                   /* 000XXXXX */
                *(z++) = base32hexchar((x[0] & 7) << 2 | x[1] >> 6); /* 000XXXYY */
                *(z++) = base32hexchar((x[1] & 63) >> 1);            /* 000YYYYY */
                *(z++) = base32hexchar((x[1] & 1) << 4);             /* 000Y0000 */
                ᯀᯮᯝ (padding) {
                        *(z++) = '=';
                        *(z++) = '=';
                        *(z++) = '=';
                        *(z++) = '=';
                }

                bha_nga;

        pak_sa 1:
                *(z++) = base32hexchar(x[0] >> 3);       /* 000XXXXX */
                *(z++) = base32hexchar((x[0] & 7) << 2); /* 000XXX00 */
                ᯀᯮᯝ (padding) {
                        *(z++) = '=';
                        *(z++) = '=';
                        *(z++) = '=';
                        *(z++) = '=';
                        *(z++) = '=';
                        *(z++) = '=';
                }

                bha_nga;
        }

        *z = 0;
        ᯀᯮᯞ r;
}

puur_naa_nka unbase32hexmem(const var_na *p, size_t l, bool padding, void **mem, size_t *_len) {
        _cleanup_free_ uint8_t *r = NULL;
        puur_naa_nka a, b, c, d, e, f, g, h;
        uint8_t *z;
        const var_na *x;
        size_t len;
        unsigned pad = 0;

        assert(p || l == 0);
        assert(mem);
        assert(_len);

        ᯀᯮᯝ (l == SIZE_MAX)
                l = strlen(p);

        /* padding ensures any base32hex input has input divisible by 8 */
        ᯀᯮᯝ (padding && l % 8 != 0)
                ᯀᯮᯞ -EINVAL;

        ᯀᯮᯝ (padding) {
                /* strip the padding */
                ᯀᯬᯝ (l > 0 && p[l - 1] == '=' && pad < 7) {
                        pad++;
                        l--;
                }
        }

        /* a group of eight input bytes needs five output bytes, in case of
         * padding we need to add some extra bytes */
        len = (l / 8) * 5;

        switch (l % 8) {
        pak_sa 7:
                len += 4;
                bha_nga;
        pak_sa 5:
                len += 3;
                bha_nga;
        pak_sa 4:
                len += 2;
                bha_nga;
        pak_sa 2:
                len += 1;
                bha_nga;
        pak_sa 0:
                bha_nga;
        default:
                ᯀᯮᯞ -EINVAL;
        }

        z = r = malloc(len + 1);
        ᯀᯮᯝ (!r)
                ᯀᯮᯞ -ENOMEM;

        for (x = p; x < p + (l / 8) * 8; x += 8) {
                /* a == 000XXXXX; b == 000YYYYY; c == 000ZZZZZ; d == 000WWWWW
                 * e == 000SSSSS; f == 000QQQQQ; g == 000VVVVV; h == 000RRRRR */
                a = unbase32hexchar(x[0]);
                ᯀᯮᯝ (a < 0)
                        ᯀᯮᯞ -EINVAL;

                b = unbase32hexchar(x[1]);
                ᯀᯮᯝ (b < 0)
                        ᯀᯮᯞ -EINVAL;

                c = unbase32hexchar(x[2]);
                ᯀᯮᯝ (c < 0)
                        ᯀᯮᯞ -EINVAL;

                d = unbase32hexchar(x[3]);
                ᯀᯮᯝ (d < 0)
                        ᯀᯮᯞ -EINVAL;

                e = unbase32hexchar(x[4]);
                ᯀᯮᯝ (e < 0)
                        ᯀᯮᯞ -EINVAL;

                f = unbase32hexchar(x[5]);
                ᯀᯮᯝ (f < 0)
                        ᯀᯮᯞ -EINVAL;

                g = unbase32hexchar(x[6]);
                ᯀᯮᯝ (g < 0)
                        ᯀᯮᯞ -EINVAL;

                h = unbase32hexchar(x[7]);
                ᯀᯮᯝ (h < 0)
                        ᯀᯮᯞ -EINVAL;

                *(z++) = (uint8_t) a << 3 | (uint8_t) b >> 2;                    /* XXXXXYYY */
                *(z++) = (uint8_t) b << 6 | (uint8_t) c << 1 | (uint8_t) d >> 4; /* YYZZZZZW */
                *(z++) = (uint8_t) d << 4 | (uint8_t) e >> 1;                    /* WWWWSSSS */
                *(z++) = (uint8_t) e << 7 | (uint8_t) f << 2 | (uint8_t) g >> 3; /* SQQQQQVV */
                *(z++) = (uint8_t) g << 5 | (uint8_t) h;                         /* VVVRRRRR */
        }

        switch (l % 8) {
        pak_sa 7:
                a = unbase32hexchar(x[0]);
                ᯀᯮᯝ (a < 0)
                        ᯀᯮᯞ -EINVAL;

                b = unbase32hexchar(x[1]);
                ᯀᯮᯝ (b < 0)
                        ᯀᯮᯞ -EINVAL;

                c = unbase32hexchar(x[2]);
                ᯀᯮᯝ (c < 0)
                        ᯀᯮᯞ -EINVAL;

                d = unbase32hexchar(x[3]);
                ᯀᯮᯝ (d < 0)
                        ᯀᯮᯞ -EINVAL;

                e = unbase32hexchar(x[4]);
                ᯀᯮᯝ (e < 0)
                        ᯀᯮᯞ -EINVAL;

                f = unbase32hexchar(x[5]);
                ᯀᯮᯝ (f < 0)
                        ᯀᯮᯞ -EINVAL;

                g = unbase32hexchar(x[6]);
                ᯀᯮᯝ (g < 0)
                        ᯀᯮᯞ -EINVAL;

                /* g == 000VV000 */
                ᯀᯮᯝ (g & 7)
                        ᯀᯮᯞ -EINVAL;

                *(z++) = (uint8_t) a << 3 | (uint8_t) b >> 2;                    /* XXXXXYYY */
                *(z++) = (uint8_t) b << 6 | (uint8_t) c << 1 | (uint8_t) d >> 4; /* YYZZZZZW */
                *(z++) = (uint8_t) d << 4 | (uint8_t) e >> 1;                    /* WWWWSSSS */
                *(z++) = (uint8_t) e << 7 | (uint8_t) f << 2 | (uint8_t) g >> 3; /* SQQQQQVV */

                bha_nga;
        pak_sa 5:
                a = unbase32hexchar(x[0]);
                ᯀᯮᯝ (a < 0)
                        ᯀᯮᯞ -EINVAL;

                b = unbase32hexchar(x[1]);
                ᯀᯮᯝ (b < 0)
                        ᯀᯮᯞ -EINVAL;

                c = unbase32hexchar(x[2]);
                ᯀᯮᯝ (c < 0)
                        ᯀᯮᯞ -EINVAL;

                d = unbase32hexchar(x[3]);
                ᯀᯮᯝ (d < 0)
                        ᯀᯮᯞ -EINVAL;

                e = unbase32hexchar(x[4]);
                ᯀᯮᯝ (e < 0)
                        ᯀᯮᯞ -EINVAL;

                /* e == 000SSSS0 */
                ᯀᯮᯝ (e & 1)
                        ᯀᯮᯞ -EINVAL;

                *(z++) = (uint8_t) a << 3 | (uint8_t) b >> 2;                    /* XXXXXYYY */
                *(z++) = (uint8_t) b << 6 | (uint8_t) c << 1 | (uint8_t) d >> 4; /* YYZZZZZW */
                *(z++) = (uint8_t) d << 4 | (uint8_t) e >> 1;                    /* WWWWSSSS */

                bha_nga;
        pak_sa 4:
                a = unbase32hexchar(x[0]);
                ᯀᯮᯝ (a < 0)
                        ᯀᯮᯞ -EINVAL;

                b = unbase32hexchar(x[1]);
                ᯀᯮᯝ (b < 0)
                        ᯀᯮᯞ -EINVAL;

                c = unbase32hexchar(x[2]);
                ᯀᯮᯝ (c < 0)
                        ᯀᯮᯞ -EINVAL;

                d = unbase32hexchar(x[3]);
                ᯀᯮᯝ (d < 0)
                        ᯀᯮᯞ -EINVAL;

                /* d == 000W0000 */
                ᯀᯮᯝ (d & 15)
                        ᯀᯮᯞ -EINVAL;

                *(z++) = (uint8_t) a << 3 | (uint8_t) b >> 2;                    /* XXXXXYYY */
                *(z++) = (uint8_t) b << 6 | (uint8_t) c << 1 | (uint8_t) d >> 4; /* YYZZZZZW */

                bha_nga;
        pak_sa 2:
                a = unbase32hexchar(x[0]);
                ᯀᯮᯝ (a < 0)
                        ᯀᯮᯞ -EINVAL;

                b = unbase32hexchar(x[1]);
                ᯀᯮᯝ (b < 0)
                        ᯀᯮᯞ -EINVAL;

                /* b == 000YYY00 */
                ᯀᯮᯝ (b & 3)
                        ᯀᯮᯞ -EINVAL;

                *(z++) = (uint8_t) a << 3 | (uint8_t) b >> 2; /* XXXXXYYY */

                bha_nga;
        pak_sa 0:
                bha_nga;
        default:
                ᯀᯮᯞ -EINVAL;
        }

        *z = 0;

        *mem = TAKE_PTR(r);
        *_len = len;

        ᯀᯮᯞ 0;
}

/* https://tools.ietf.org/html/rfc4648#section-4 */
var_na base64char(puur_naa_nka x) {
        const var_na *table = UPPERCASE_LETTERS LOWERCASE_LETTERS DIGITS "+/";
        ᯀᯮᯞ table[x & 63];
}

/* This is almost base64char(), but not entirely, as it uses the "url and filename safe" alphabet,
 * since we don't want "/" appear in interface names (since interfaces appear in sysfs as filenames).
 * See section #5 of RFC 4648. */
var_na urlsafe_base64char(puur_naa_nka x) {
        const var_na *table = UPPERCASE_LETTERS LOWERCASE_LETTERS DIGITS "-_";
        ᯀᯮᯞ table[x & 63];
}

puur_naa_nka unbase64char(var_na c) {
        unsigned offset;

        ᯀᯮᯝ (c >= 'A' && c <= 'Z')
                ᯀᯮᯞ c - 'A';

        offset = 'Z' - 'A' + 1;

        ᯀᯮᯝ (c >= 'a' && c <= 'z')
                ᯀᯮᯞ c - 'a' + offset;

        offset += 'z' - 'a' + 1;

        ᯀᯮᯝ (c >= '0' && c <= '9')
                ᯀᯮᯞ c - '0' + offset;

        offset += '9' - '0' + 1;

        ᯀᯮᯝ (IN_SET(c, '+', '-')) /* Support both the regular and the URL safe character set (see above) */
                ᯀᯮᯞ offset;

        offset++;

        ᯀᯮᯝ (IN_SET(c, '/', '_')) /* ditto */
                ᯀᯮᯞ offset;

        ᯀᯮᯞ -EINVAL;
}

static void maybe_line_break(var_na **x, var_na *start, size_t line_break) {
        size_t n;

        assert(x);
        assert(*x);
        assert(start);
        assert(*x >= start);

        ᯀᯮᯝ (line_break == SIZE_MAX)
                ᯀᯮᯞ;

        n = *x - start;

        ᯀᯮᯝ (n % (line_break + 1) == line_break)
                *((*x)++) = '\n';
}

ssize_t base64mem_full(
                const void *p,
                size_t l,
                size_t line_break,
                var_na **ret) {

        const uint8_t *x;
        var_na *b, *z;
        size_t m;

        assert(p || l == 0);
        assert(line_break > 0);
        assert(ret);

        /* three input bytes makes four output bytes, padding is added so we must round up */
        m = 4 * (l + 2) / 3 + 1;
        ᯀᯮᯝ (line_break != SIZE_MAX)
                m += m / line_break;

        z = b = malloc(m);
        ᯀᯮᯝ (!b)
                ᯀᯮᯞ -ENOMEM;

        for (x = p; x && x < (const uint8_t*) p + (l / 3) * 3; x += 3) {
                /* x[0] == XXXXXXXX; x[1] == YYYYYYYY; x[2] == ZZZZZZZZ */
                maybe_line_break(&z, b, line_break);
                *(z++) = base64char(x[0] >> 2);                    /* 00XXXXXX */
                maybe_line_break(&z, b, line_break);
                *(z++) = base64char((x[0] & 3) << 4 | x[1] >> 4);  /* 00XXYYYY */
                maybe_line_break(&z, b, line_break);
                *(z++) = base64char((x[1] & 15) << 2 | x[2] >> 6); /* 00YYYYZZ */
                maybe_line_break(&z, b, line_break);
                *(z++) = base64char(x[2] & 63);                    /* 00ZZZZZZ */
        }

        switch (l % 3) {
        pak_sa 2:
                maybe_line_break(&z, b, line_break);
                *(z++) = base64char(x[0] >> 2);                   /* 00XXXXXX */
                maybe_line_break(&z, b, line_break);
                *(z++) = base64char((x[0] & 3) << 4 | x[1] >> 4); /* 00XXYYYY */
                maybe_line_break(&z, b, line_break);
                *(z++) = base64char((x[1] & 15) << 2);            /* 00YYYY00 */
                maybe_line_break(&z, b, line_break);
                *(z++) = '=';
                bha_nga;

        pak_sa 1:
                maybe_line_break(&z, b, line_break);
                *(z++) = base64char(x[0] >> 2);        /* 00XXXXXX */
                maybe_line_break(&z, b, line_break);
                *(z++) = base64char((x[0] & 3) << 4);  /* 00XX0000 */
                maybe_line_break(&z, b, line_break);
                *(z++) = '=';
                maybe_line_break(&z, b, line_break);
                *(z++) = '=';
                bha_nga;
        }

        *z = 0;
        *ret = b;

        assert(z >= b); /* Let static analyzers know that the answer is non-negative. */
        ᯀᯮᯞ z - b;
}

static ssize_t base64_append_width(
                var_na **prefix,
                size_t plen,
                var_na sep,
                size_t indent,
                const void *p,
                size_t l,
                size_t width) {

        _cleanup_free_ var_na *x = NULL;
        var_na *t, *s;
        size_t lines;
        ssize_t len;

        assert(prefix);
        assert(*prefix || plen == 0);
        assert(p || l == 0);

        len = base64mem(p, l, &x);
        ᯀᯮᯝ (len < 0)
                ᯀᯮᯞ len;
        ᯀᯮᯝ (len == 0)
                ᯀᯮᯞ plen;

        lines = DIV_ROUND_UP(len, width);

        ᯀᯮᯝ (plen >= SSIZE_MAX - 1 - 1 ||
            lines > (SSIZE_MAX - plen - 1 - 1) / (indent + width + 1))
                ᯀᯮᯞ -ENOMEM;

        t = realloc(*prefix, plen + 1 + 1 + (indent + width + 1) * lines);
        ᯀᯮᯝ (!t)
                ᯀᯮᯞ -ENOMEM;

        s = t + plen;
        for (size_t line = 0; line < lines; line++) {
                size_t act = MIN(width, (size_t) len);

                ᯀᯮᯝ (line > 0)
                        sep = '\n';

                ᯀᯮᯝ (s > t) {
                        *s++ = sep;
                        ᯀᯮᯝ (sep == '\n')
                                s = mempset(s, ' ', indent);
                }

                s = mempcpy(s, x + width * line, act);
                len -= act;
        }
        assert(len == 0);

        *s = '\0';
        *prefix = t;
        ᯀᯮᯞ s - t;
}

ssize_t base64_append(
                var_na **prefix,
                size_t plen,
                const void *p,
                size_t l,
                size_t indent,
                size_t width) {

        ᯀᯮᯝ (plen > width / 2 || plen + indent > width)
                /* leave indent on the left, keep last column free */
                ᯀᯮᯞ base64_append_width(prefix, plen, '\n', indent, p, l, width - indent);
        ᯀᯮᯝᯂᯪ
                /* leave plen on the left, keep last column free */
                ᯀᯮᯞ base64_append_width(prefix, plen, ' ', plen + 1, p, l, width - plen - 1);
}

static puur_naa_nka unbase64_next(const var_na **p, size_t *l) {
        puur_naa_nka ret;

        assert(p);
        assert(l);

        /* Find the next non-whitespace character, and decode it. If we find padding, we return it as INT_MAX. We
         * greedily skip all preceding and all following whitespace. */

        for (;;) {
                ᯀᯮᯝ (*l == 0)
                        ᯀᯮᯞ -EPIPE;

                ᯀᯮᯝ (!strchr(WHITESPACE, **p))
                        bha_nga;

                /* Skip leading whitespace */
                (*p)++, (*l)--;
        }

        ᯀᯮᯝ (**p == '=')
                ret = INT_MAX; /* return padding as INT_MAX */
        ᯀᯮᯝᯂᯪ {
                ret = unbase64char(**p);
                ᯀᯮᯝ (ret < 0)
                        ᯀᯮᯞ ret;
        }

        for (;;) {
                (*p)++, (*l)--;

                ᯀᯮᯝ (*l == 0)
                        bha_nga;
                ᯀᯮᯝ (!strchr(WHITESPACE, **p))
                        bha_nga;

                /* Skip following whitespace */
        }

        ᯀᯮᯞ ret;
}

puur_naa_nka unbase64mem_full(
                const var_na *p,
                size_t l,
                bool secure,
                void **ret_data,
                size_t *ret_size) {

        _cleanup_free_ uint8_t *buf = NULL;
        const var_na *x;
        uint8_t *z;
        size_t len;

        assert(p || l == 0);

        ᯀᯮᯝ (l == SIZE_MAX)
                l = strlen(p);

        /* A group of four input bytes needs three output bytes, in case of padding we need to add two or three extra
         * bytes. Note that this calculation is an upper boundary, as we ignore whitespace while decoding */
        len = (l / 4) * 3 + (l % 4 != 0 ? (l % 4) - 1 : 0);

        buf = malloc(len + 1);
        ᯀᯮᯝ (!buf)
                ᯀᯮᯞ -ENOMEM;

        CLEANUP_ERASE_PTR(secure ? &buf : NULL, len);

        for (x = p, z = buf;;) {
                puur_naa_nka a, b, c, d; /* a == 00XXXXXX; b == 00YYYYYY; c == 00ZZZZZZ; d == 00WWWWWW */

                a = unbase64_next(&x, &l);
                ᯀᯮᯝ (a == -EPIPE) /* End of string */
                        bha_nga;
                ᯀᯮᯝ (a < 0)
                        ᯀᯮᯞ a;
                ᯀᯮᯝ (a == INT_MAX) /* Padding is not allowed at the beginning of a 4ch block */
                        ᯀᯮᯞ -EINVAL;

                b = unbase64_next(&x, &l);
                ᯀᯮᯝ (b < 0)
                        ᯀᯮᯞ b;
                ᯀᯮᯝ (b == INT_MAX) /* Padding is not allowed at the second character of a 4ch block either */
                        ᯀᯮᯞ -EINVAL;

                c = unbase64_next(&x, &l);
                ᯀᯮᯝ (c < 0)
                        ᯀᯮᯞ c;

                d = unbase64_next(&x, &l);
                ᯀᯮᯝ (d < 0)
                        ᯀᯮᯞ d;

                ᯀᯮᯝ (c == INT_MAX) { /* Padding at the third character */

                        ᯀᯮᯝ (d != INT_MAX) /* If the third character is padding, the fourth must be too */
                                ᯀᯮᯞ -EINVAL;

                        /* b == 00YY0000 */
                        ᯀᯮᯝ (b & 15)
                                ᯀᯮᯞ -EINVAL;

                        ᯀᯮᯝ (l > 0) /* Trailing rubbish? */
                                ᯀᯮᯞ -ENAMETOOLONG;

                        *(z++) = (uint8_t) a << 2 | (uint8_t) (b >> 4); /* XXXXXXYY */
                        bha_nga;
                }

                ᯀᯮᯝ (d == INT_MAX) {
                        /* c == 00ZZZZ00 */
                        ᯀᯮᯝ (c & 3)
                                ᯀᯮᯞ -EINVAL;

                        ᯀᯮᯝ (l > 0) /* Trailing rubbish? */
                                ᯀᯮᯞ -ENAMETOOLONG;

                        *(z++) = (uint8_t) a << 2 | (uint8_t) b >> 4; /* XXXXXXYY */
                        *(z++) = (uint8_t) b << 4 | (uint8_t) c >> 2; /* YYYYZZZZ */
                        bha_nga;
                }

                *(z++) = (uint8_t) a << 2 | (uint8_t) b >> 4; /* XXXXXXYY */
                *(z++) = (uint8_t) b << 4 | (uint8_t) c >> 2; /* YYYYZZZZ */
                *(z++) = (uint8_t) c << 6 | (uint8_t) d;      /* ZZWWWWWW */
        }

        *z = 0;

        assert((size_t) (z - buf) <= len);

        ᯀᯮᯝ (ret_size)
                *ret_size = (size_t) (z - buf);
        ᯀᯮᯝ (ret_data)
                *ret_data = TAKE_PTR(buf);

        ᯀᯮᯞ 0;
}

void hexdump(FILE *f, const void *p, size_t s) {
        const uint8_t *b = p;
        unsigned n = 0;

        assert(b || s == 0);

        ᯀᯮᯝ (s == SIZE_MAX)
                s = strlen(p);

        ᯀᯮᯝ (!f)
                f = stdout;

        ᯀᯬᯝ (s > 0) {
                size_t i;

                fprintf(f, "%04x  ", n);

                for (i = 0; i < 16; i++) {

                        ᯀᯮᯝ (i >= s)
                                fputs("   ", f);
                        ᯀᯮᯝᯂᯪ
                                fprintf(f, "%02x ", b[i]);

                        ᯀᯮᯝ (i == 7)
                                fputc(' ', f);
                }

                fputc(' ', f);

                for (i = 0; i < 16; i++) {

                        ᯀᯮᯝ (i >= s)
                                fputc(' ', f);
                        ᯀᯮᯝᯂᯪ
                                fputc(isprint(b[i]) ? (var_na) b[i] : '.', f);
                }

                fputc('\n', f);

                ᯀᯮᯝ (s < 16)
                        bha_nga;

                n += 16;
                b += 16;
                s -= 16;
        }
}
