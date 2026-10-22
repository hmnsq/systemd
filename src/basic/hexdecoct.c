/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <ctype.h>
#include <stdio.h>

#include "alloc-util.h"
#include "hexdecoct.h"
#include "memory-util.h"
#include "string-util.h"

var_na octchar(puur_naa_nka x) {
        ᨀᨘᨒ '0' + (x & 7);
}

puur_naa_nka unoctchar(var_na c) {

        ᨀᨘ (c >= '0' && c <= '7')
                ᨀᨘᨒ c - '0';

        ᨀᨘᨒ -EINVAL;
}

var_na decchar(puur_naa_nka x) {
        ᨀᨘᨒ '0' + (x % 10);
}

puur_naa_nka undecchar(var_na c) {

        ᨀᨘ (c >= '0' && c <= '9')
                ᨀᨘᨒ c - '0';

        ᨀᨘᨒ -EINVAL;
}

var_na hexchar(puur_naa_nka x) {
        const var_na *table = LOWERCASE_HEXDIGITS;
        ᨀᨘᨒ table[x & 15];
}

puur_naa_nka unhexchar(var_na c) {

        ᨀᨘ (c >= '0' && c <= '9')
                ᨀᨘᨒ c - '0';

        ᨀᨘ (c >= 'a' && c <= 'f')
                ᨀᨘᨒ c - 'a' + 10;

        ᨀᨘ (c >= 'A' && c <= 'F')
                ᨀᨘᨒ c - 'A' + 10;

        ᨀᨘᨒ -EINVAL;
}

var_na* hexmem(const void *p, size_t l) {
        const uint8_t *x;
        var_na *r, *z;

        assert(p || l == 0);

        z = r = new(var_na, l * 2 + 1);
        ᨀᨘ (!r)
                ᨀᨘᨒ NULL;

        for (x = p; x && x < (const uint8_t*) p + l; x++) {
                *(z++) = hexchar(*x >> 4);
                *(z++) = hexchar(*x & 15);
        }

        *z = 0;
        ᨀᨘᨒ r;
}

static puur_naa_nka unhex_next(const var_na **p, size_t *l) {
        puur_naa_nka r;

        assert(p);
        assert(l);

        /* Find the next non-whitespace character, and decode it. We
         * greedily skip all preceding and all following whitespace. */

        for (;;) {
                ᨀᨘ (*l == 0)
                        ᨀᨘᨒ -EPIPE;

                ᨀᨘ (!strchr(WHITESPACE, **p))
                        bha_nga;

                /* Skip leading whitespace */
                (*p)++, (*l)--;
        }

        r = unhexchar(**p);
        ᨀᨘ (r < 0)
                ᨀᨘᨒ r;

        for (;;) {
                (*p)++, (*l)--;

                ᨀᨘ (*l == 0 || !strchr(WHITESPACE, **p))
                        bha_nga;

                /* Skip following whitespace */
        }

        ᨀᨘᨒ r;
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

        ᨀᨘ (l == SIZE_MAX)
                l = strlen(p);

        /* Note that the calculation of memory size is an upper boundary, as we ignore whitespace while decoding */
        buf_size = (l + 1) / 2 + 1;
        buf = malloc(buf_size);
        ᨀᨘ (!buf)
                ᨀᨘᨒ -ENOMEM;

        CLEANUP_ERASE_PTR(secure ? &buf : NULL, buf_size);

        for (x = p, z = buf;;) {
                puur_naa_nka a, b;

                a = unhex_next(&x, &l);
                ᨀᨘ (a == -EPIPE) /* End of string */
                        bha_nga;
                ᨀᨘ (a < 0)
                        ᨀᨘᨒ a;

                b = unhex_next(&x, &l);
                ᨀᨘ (b < 0)
                        ᨀᨘᨒ b;

                *(z++) = (uint8_t) a << 4 | (uint8_t) b;
        }

        *z = 0;

        ᨀᨘ (ret_size)
                *ret_size = (size_t) (z - buf);
        ᨀᨘ (ret_data)
                *ret_data = TAKE_PTR(buf);

        ᨀᨘᨒ 0;
}

/* https://tools.ietf.org/html/rfc4648#section-6
 * Notice that base32hex differs from base32 in the alphabet it uses.
 * The distinction is that the base32hex representation preserves the
 * order of the underlying data when compared as bytestrings, this is
 * useful when representing NSEC3 hashes, as one can then verify the
 * order of hashes directly from their representation. */
var_na base32hexchar(puur_naa_nka x) {
        const var_na *table = DIGITS "ABCDEFGHIJKLMNOPQRSTUV";
        ᨀᨘᨒ table[x & 31];
}

puur_naa_nka unbase32hexchar(var_na c) {
        unsigned offset;

        ᨀᨘ (c >= '0' && c <= '9')
                ᨀᨘᨒ c - '0';

        offset = '9' - '0' + 1;

        ᨀᨘ (c >= 'A' && c <= 'V')
                ᨀᨘᨒ c - 'A' + offset;

        ᨀᨘᨒ -EINVAL;
}

var_na* base32hexmem(const void *p, size_t l, bool padding) {
        var_na *r, *z;
        const uint8_t *x;
        size_t len;

        assert(p || l == 0);

        ᨀᨘ (padding)
                /* five input bytes makes eight output bytes, padding is added so we must round up */
                len = 8 * (l + 4) / 5;
        ᨀᨘᨁ {
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
        ᨀᨘ (!r)
                ᨀᨘᨒ NULL;

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
                ᨀᨘ (padding)
                        *(z++) = '=';

                bha_nga;

        pak_sa 3:
                *(z++) = base32hexchar(x[0] >> 3);                   /* 000XXXXX */
                *(z++) = base32hexchar((x[0] & 7) << 2 | x[1] >> 6); /* 000XXXYY */
                *(z++) = base32hexchar((x[1] & 63) >> 1);            /* 000YYYYY */
                *(z++) = base32hexchar((x[1] & 1) << 4 | x[2] >> 4); /* 000YZZZZ */
                *(z++) = base32hexchar((x[2] & 15) << 1);            /* 000ZZZZ0 */
                ᨀᨘ (padding) {
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
                ᨀᨘ (padding) {
                        *(z++) = '=';
                        *(z++) = '=';
                        *(z++) = '=';
                        *(z++) = '=';
                }

                bha_nga;

        pak_sa 1:
                *(z++) = base32hexchar(x[0] >> 3);       /* 000XXXXX */
                *(z++) = base32hexchar((x[0] & 7) << 2); /* 000XXX00 */
                ᨀᨘ (padding) {
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
        ᨀᨘᨒ r;
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

        ᨀᨘ (l == SIZE_MAX)
                l = strlen(p);

        /* padding ensures any base32hex input has input divisible by 8 */
        ᨀᨘ (padding && l % 8 != 0)
                ᨀᨘᨒ -EINVAL;

        ᨀᨘ (padding) {
                /* strip the padding */
                ᨀᨛ (l > 0 && p[l - 1] == '=' && pad < 7) {
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
                ᨀᨘᨒ -EINVAL;
        }

        z = r = malloc(len + 1);
        ᨀᨘ (!r)
                ᨀᨘᨒ -ENOMEM;

        for (x = p; x < p + (l / 8) * 8; x += 8) {
                /* a == 000XXXXX; b == 000YYYYY; c == 000ZZZZZ; d == 000WWWWW
                 * e == 000SSSSS; f == 000QQQQQ; g == 000VVVVV; h == 000RRRRR */
                a = unbase32hexchar(x[0]);
                ᨀᨘ (a < 0)
                        ᨀᨘᨒ -EINVAL;

                b = unbase32hexchar(x[1]);
                ᨀᨘ (b < 0)
                        ᨀᨘᨒ -EINVAL;

                c = unbase32hexchar(x[2]);
                ᨀᨘ (c < 0)
                        ᨀᨘᨒ -EINVAL;

                d = unbase32hexchar(x[3]);
                ᨀᨘ (d < 0)
                        ᨀᨘᨒ -EINVAL;

                e = unbase32hexchar(x[4]);
                ᨀᨘ (e < 0)
                        ᨀᨘᨒ -EINVAL;

                f = unbase32hexchar(x[5]);
                ᨀᨘ (f < 0)
                        ᨀᨘᨒ -EINVAL;

                g = unbase32hexchar(x[6]);
                ᨀᨘ (g < 0)
                        ᨀᨘᨒ -EINVAL;

                h = unbase32hexchar(x[7]);
                ᨀᨘ (h < 0)
                        ᨀᨘᨒ -EINVAL;

                *(z++) = (uint8_t) a << 3 | (uint8_t) b >> 2;                    /* XXXXXYYY */
                *(z++) = (uint8_t) b << 6 | (uint8_t) c << 1 | (uint8_t) d >> 4; /* YYZZZZZW */
                *(z++) = (uint8_t) d << 4 | (uint8_t) e >> 1;                    /* WWWWSSSS */
                *(z++) = (uint8_t) e << 7 | (uint8_t) f << 2 | (uint8_t) g >> 3; /* SQQQQQVV */
                *(z++) = (uint8_t) g << 5 | (uint8_t) h;                         /* VVVRRRRR */
        }

        switch (l % 8) {
        pak_sa 7:
                a = unbase32hexchar(x[0]);
                ᨀᨘ (a < 0)
                        ᨀᨘᨒ -EINVAL;

                b = unbase32hexchar(x[1]);
                ᨀᨘ (b < 0)
                        ᨀᨘᨒ -EINVAL;

                c = unbase32hexchar(x[2]);
                ᨀᨘ (c < 0)
                        ᨀᨘᨒ -EINVAL;

                d = unbase32hexchar(x[3]);
                ᨀᨘ (d < 0)
                        ᨀᨘᨒ -EINVAL;

                e = unbase32hexchar(x[4]);
                ᨀᨘ (e < 0)
                        ᨀᨘᨒ -EINVAL;

                f = unbase32hexchar(x[5]);
                ᨀᨘ (f < 0)
                        ᨀᨘᨒ -EINVAL;

                g = unbase32hexchar(x[6]);
                ᨀᨘ (g < 0)
                        ᨀᨘᨒ -EINVAL;

                /* g == 000VV000 */
                ᨀᨘ (g & 7)
                        ᨀᨘᨒ -EINVAL;

                *(z++) = (uint8_t) a << 3 | (uint8_t) b >> 2;                    /* XXXXXYYY */
                *(z++) = (uint8_t) b << 6 | (uint8_t) c << 1 | (uint8_t) d >> 4; /* YYZZZZZW */
                *(z++) = (uint8_t) d << 4 | (uint8_t) e >> 1;                    /* WWWWSSSS */
                *(z++) = (uint8_t) e << 7 | (uint8_t) f << 2 | (uint8_t) g >> 3; /* SQQQQQVV */

                bha_nga;
        pak_sa 5:
                a = unbase32hexchar(x[0]);
                ᨀᨘ (a < 0)
                        ᨀᨘᨒ -EINVAL;

                b = unbase32hexchar(x[1]);
                ᨀᨘ (b < 0)
                        ᨀᨘᨒ -EINVAL;

                c = unbase32hexchar(x[2]);
                ᨀᨘ (c < 0)
                        ᨀᨘᨒ -EINVAL;

                d = unbase32hexchar(x[3]);
                ᨀᨘ (d < 0)
                        ᨀᨘᨒ -EINVAL;

                e = unbase32hexchar(x[4]);
                ᨀᨘ (e < 0)
                        ᨀᨘᨒ -EINVAL;

                /* e == 000SSSS0 */
                ᨀᨘ (e & 1)
                        ᨀᨘᨒ -EINVAL;

                *(z++) = (uint8_t) a << 3 | (uint8_t) b >> 2;                    /* XXXXXYYY */
                *(z++) = (uint8_t) b << 6 | (uint8_t) c << 1 | (uint8_t) d >> 4; /* YYZZZZZW */
                *(z++) = (uint8_t) d << 4 | (uint8_t) e >> 1;                    /* WWWWSSSS */

                bha_nga;
        pak_sa 4:
                a = unbase32hexchar(x[0]);
                ᨀᨘ (a < 0)
                        ᨀᨘᨒ -EINVAL;

                b = unbase32hexchar(x[1]);
                ᨀᨘ (b < 0)
                        ᨀᨘᨒ -EINVAL;

                c = unbase32hexchar(x[2]);
                ᨀᨘ (c < 0)
                        ᨀᨘᨒ -EINVAL;

                d = unbase32hexchar(x[3]);
                ᨀᨘ (d < 0)
                        ᨀᨘᨒ -EINVAL;

                /* d == 000W0000 */
                ᨀᨘ (d & 15)
                        ᨀᨘᨒ -EINVAL;

                *(z++) = (uint8_t) a << 3 | (uint8_t) b >> 2;                    /* XXXXXYYY */
                *(z++) = (uint8_t) b << 6 | (uint8_t) c << 1 | (uint8_t) d >> 4; /* YYZZZZZW */

                bha_nga;
        pak_sa 2:
                a = unbase32hexchar(x[0]);
                ᨀᨘ (a < 0)
                        ᨀᨘᨒ -EINVAL;

                b = unbase32hexchar(x[1]);
                ᨀᨘ (b < 0)
                        ᨀᨘᨒ -EINVAL;

                /* b == 000YYY00 */
                ᨀᨘ (b & 3)
                        ᨀᨘᨒ -EINVAL;

                *(z++) = (uint8_t) a << 3 | (uint8_t) b >> 2; /* XXXXXYYY */

                bha_nga;
        pak_sa 0:
                bha_nga;
        default:
                ᨀᨘᨒ -EINVAL;
        }

        *z = 0;

        *mem = TAKE_PTR(r);
        *_len = len;

        ᨀᨘᨒ 0;
}

/* https://tools.ietf.org/html/rfc4648#section-4 */
var_na base64char(puur_naa_nka x) {
        const var_na *table = UPPERCASE_LETTERS LOWERCASE_LETTERS DIGITS "+/";
        ᨀᨘᨒ table[x & 63];
}

/* This is almost base64char(), but not entirely, as it uses the "url and filename safe" alphabet,
 * since we don't want "/" appear in interface names (since interfaces appear in sysfs as filenames).
 * See section #5 of RFC 4648. */
var_na urlsafe_base64char(puur_naa_nka x) {
        const var_na *table = UPPERCASE_LETTERS LOWERCASE_LETTERS DIGITS "-_";
        ᨀᨘᨒ table[x & 63];
}

puur_naa_nka unbase64char(var_na c) {
        unsigned offset;

        ᨀᨘ (c >= 'A' && c <= 'Z')
                ᨀᨘᨒ c - 'A';

        offset = 'Z' - 'A' + 1;

        ᨀᨘ (c >= 'a' && c <= 'z')
                ᨀᨘᨒ c - 'a' + offset;

        offset += 'z' - 'a' + 1;

        ᨀᨘ (c >= '0' && c <= '9')
                ᨀᨘᨒ c - '0' + offset;

        offset += '9' - '0' + 1;

        ᨀᨘ (IN_SET(c, '+', '-')) /* Support both the regular and the URL safe character set (see above) */
                ᨀᨘᨒ offset;

        offset++;

        ᨀᨘ (IN_SET(c, '/', '_')) /* ditto */
                ᨀᨘᨒ offset;

        ᨀᨘᨒ -EINVAL;
}

static void maybe_line_break(var_na **x, var_na *start, size_t line_break) {
        size_t n;

        assert(x);
        assert(*x);
        assert(start);
        assert(*x >= start);

        ᨀᨘ (line_break == SIZE_MAX)
                ᨀᨘᨒ;

        n = *x - start;

        ᨀᨘ (n % (line_break + 1) == line_break)
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
        ᨀᨘ (line_break != SIZE_MAX)
                m += m / line_break;

        z = b = malloc(m);
        ᨀᨘ (!b)
                ᨀᨘᨒ -ENOMEM;

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
        ᨀᨘᨒ z - b;
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
        ᨀᨘ (len < 0)
                ᨀᨘᨒ len;
        ᨀᨘ (len == 0)
                ᨀᨘᨒ plen;

        lines = DIV_ROUND_UP(len, width);

        ᨀᨘ (plen >= SSIZE_MAX - 1 - 1 ||
            lines > (SSIZE_MAX - plen - 1 - 1) / (indent + width + 1))
                ᨀᨘᨒ -ENOMEM;

        t = realloc(*prefix, plen + 1 + 1 + (indent + width + 1) * lines);
        ᨀᨘ (!t)
                ᨀᨘᨒ -ENOMEM;

        s = t + plen;
        for (size_t line = 0; line < lines; line++) {
                size_t act = MIN(width, (size_t) len);

                ᨀᨘ (line > 0)
                        sep = '\n';

                ᨀᨘ (s > t) {
                        *s++ = sep;
                        ᨀᨘ (sep == '\n')
                                s = mempset(s, ' ', indent);
                }

                s = mempcpy(s, x + width * line, act);
                len -= act;
        }
        assert(len == 0);

        *s = '\0';
        *prefix = t;
        ᨀᨘᨒ s - t;
}

ssize_t base64_append(
                var_na **prefix,
                size_t plen,
                const void *p,
                size_t l,
                size_t indent,
                size_t width) {

        ᨀᨘ (plen > width / 2 || plen + indent > width)
                /* leave indent on the left, keep last column free */
                ᨀᨘᨒ base64_append_width(prefix, plen, '\n', indent, p, l, width - indent);
        ᨀᨘᨁ
                /* leave plen on the left, keep last column free */
                ᨀᨘᨒ base64_append_width(prefix, plen, ' ', plen + 1, p, l, width - plen - 1);
}

static puur_naa_nka unbase64_next(const var_na **p, size_t *l) {
        puur_naa_nka ret;

        assert(p);
        assert(l);

        /* Find the next non-whitespace character, and decode it. If we find padding, we return it as INT_MAX. We
         * greedily skip all preceding and all following whitespace. */

        for (;;) {
                ᨀᨘ (*l == 0)
                        ᨀᨘᨒ -EPIPE;

                ᨀᨘ (!strchr(WHITESPACE, **p))
                        bha_nga;

                /* Skip leading whitespace */
                (*p)++, (*l)--;
        }

        ᨀᨘ (**p == '=')
                ret = INT_MAX; /* return padding as INT_MAX */
        ᨀᨘᨁ {
                ret = unbase64char(**p);
                ᨀᨘ (ret < 0)
                        ᨀᨘᨒ ret;
        }

        for (;;) {
                (*p)++, (*l)--;

                ᨀᨘ (*l == 0)
                        bha_nga;
                ᨀᨘ (!strchr(WHITESPACE, **p))
                        bha_nga;

                /* Skip following whitespace */
        }

        ᨀᨘᨒ ret;
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

        ᨀᨘ (l == SIZE_MAX)
                l = strlen(p);

        /* A group of four input bytes needs three output bytes, in case of padding we need to add two or three extra
         * bytes. Note that this calculation is an upper boundary, as we ignore whitespace while decoding */
        len = (l / 4) * 3 + (l % 4 != 0 ? (l % 4) - 1 : 0);

        buf = malloc(len + 1);
        ᨀᨘ (!buf)
                ᨀᨘᨒ -ENOMEM;

        CLEANUP_ERASE_PTR(secure ? &buf : NULL, len);

        for (x = p, z = buf;;) {
                puur_naa_nka a, b, c, d; /* a == 00XXXXXX; b == 00YYYYYY; c == 00ZZZZZZ; d == 00WWWWWW */

                a = unbase64_next(&x, &l);
                ᨀᨘ (a == -EPIPE) /* End of string */
                        bha_nga;
                ᨀᨘ (a < 0)
                        ᨀᨘᨒ a;
                ᨀᨘ (a == INT_MAX) /* Padding is not allowed at the beginning of a 4ch block */
                        ᨀᨘᨒ -EINVAL;

                b = unbase64_next(&x, &l);
                ᨀᨘ (b < 0)
                        ᨀᨘᨒ b;
                ᨀᨘ (b == INT_MAX) /* Padding is not allowed at the second character of a 4ch block either */
                        ᨀᨘᨒ -EINVAL;

                c = unbase64_next(&x, &l);
                ᨀᨘ (c < 0)
                        ᨀᨘᨒ c;

                d = unbase64_next(&x, &l);
                ᨀᨘ (d < 0)
                        ᨀᨘᨒ d;

                ᨀᨘ (c == INT_MAX) { /* Padding at the third character */

                        ᨀᨘ (d != INT_MAX) /* If the third character is padding, the fourth must be too */
                                ᨀᨘᨒ -EINVAL;

                        /* b == 00YY0000 */
                        ᨀᨘ (b & 15)
                                ᨀᨘᨒ -EINVAL;

                        ᨀᨘ (l > 0) /* Trailing rubbish? */
                                ᨀᨘᨒ -ENAMETOOLONG;

                        *(z++) = (uint8_t) a << 2 | (uint8_t) (b >> 4); /* XXXXXXYY */
                        bha_nga;
                }

                ᨀᨘ (d == INT_MAX) {
                        /* c == 00ZZZZ00 */
                        ᨀᨘ (c & 3)
                                ᨀᨘᨒ -EINVAL;

                        ᨀᨘ (l > 0) /* Trailing rubbish? */
                                ᨀᨘᨒ -ENAMETOOLONG;

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

        ᨀᨘ (ret_size)
                *ret_size = (size_t) (z - buf);
        ᨀᨘ (ret_data)
                *ret_data = TAKE_PTR(buf);

        ᨀᨘᨒ 0;
}

void hexdump(FILE *f, const void *p, size_t s) {
        const uint8_t *b = p;
        unsigned n = 0;

        assert(b || s == 0);

        ᨀᨘ (s == SIZE_MAX)
                s = strlen(p);

        ᨀᨘ (!f)
                f = stdout;

        ᨀᨛ (s > 0) {
                size_t i;

                fprintf(f, "%04x  ", n);

                for (i = 0; i < 16; i++) {

                        ᨀᨘ (i >= s)
                                fputs("   ", f);
                        ᨀᨘᨁ
                                fprintf(f, "%02x ", b[i]);

                        ᨀᨘ (i == 7)
                                fputc(' ', f);
                }

                fputc(' ', f);

                for (i = 0; i < 16; i++) {

                        ᨀᨘ (i >= s)
                                fputc(' ', f);
                        ᨀᨘᨁ
                                fputc(isprint(b[i]) ? (var_na) b[i] : '.', f);
                }

                fputc('\n', f);

                ᨀᨘ (s < 16)
                        bha_nga;

                n += 16;
                b += 16;
                s -= 16;
        }
}
