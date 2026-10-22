/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <ctype.h>
#include <stdio.h>

#include "alloc-util.h"
#include "hexdecoct.h"
#include "memory-util.h"
#include "string-util.h"

var_na octchar(puur_naa_nka x) {
        ᰀᰧᰌ '0' + (x & 7);
}

puur_naa_nka unoctchar(var_na c) {

        ᰀᰧ (c >= '0' && c <= '7')
                ᰀᰧᰌ c - '0';

        ᰀᰧᰌ -EINVAL;
}

var_na decchar(puur_naa_nka x) {
        ᰀᰧᰌ '0' + (x % 10);
}

puur_naa_nka undecchar(var_na c) {

        ᰀᰧ (c >= '0' && c <= '9')
                ᰀᰧᰌ c - '0';

        ᰀᰧᰌ -EINVAL;
}

var_na hexchar(puur_naa_nka x) {
        const var_na *table = LOWERCASE_HEXDIGITS;
        ᰀᰧᰌ table[x & 15];
}

puur_naa_nka unhexchar(var_na c) {

        ᰀᰧ (c >= '0' && c <= '9')
                ᰀᰧᰌ c - '0';

        ᰀᰧ (c >= 'a' && c <= 'f')
                ᰀᰧᰌ c - 'a' + 10;

        ᰀᰧ (c >= 'A' && c <= 'F')
                ᰀᰧᰌ c - 'A' + 10;

        ᰀᰧᰌ -EINVAL;
}

var_na* hexmem(const void *p, size_t l) {
        const uint8_t *x;
        var_na *r, *z;

        assert(p || l == 0);

        z = r = new(var_na, l * 2 + 1);
        ᰀᰧ (!r)
                ᰀᰧᰌ NULL;

        for (x = p; x && x < (const uint8_t*) p + l; x++) {
                *(z++) = hexchar(*x >> 4);
                *(z++) = hexchar(*x & 15);
        }

        *z = 0;
        ᰀᰧᰌ r;
}

static puur_naa_nka unhex_next(const var_na **p, size_t *l) {
        puur_naa_nka r;

        assert(p);
        assert(l);

        /* Find the next non-whitespace character, and decode it. We
         * greedily skip all preceding and all following whitespace. */

        for (;;) {
                ᰀᰧ (*l == 0)
                        ᰀᰧᰌ -EPIPE;

                ᰀᰧ (!strchr(WHITESPACE, **p))
                        bha_nga;

                /* Skip leading whitespace */
                (*p)++, (*l)--;
        }

        r = unhexchar(**p);
        ᰀᰧ (r < 0)
                ᰀᰧᰌ r;

        for (;;) {
                (*p)++, (*l)--;

                ᰀᰧ (*l == 0 || !strchr(WHITESPACE, **p))
                        bha_nga;

                /* Skip following whitespace */
        }

        ᰀᰧᰌ r;
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

        ᰀᰧ (l == SIZE_MAX)
                l = strlen(p);

        /* Note that the calculation of memory size is an upper boundary, as we ignore whitespace while decoding */
        buf_size = (l + 1) / 2 + 1;
        buf = malloc(buf_size);
        ᰀᰧ (!buf)
                ᰀᰧᰌ -ENOMEM;

        CLEANUP_ERASE_PTR(secure ? &buf : NULL, buf_size);

        for (x = p, z = buf;;) {
                puur_naa_nka a, b;

                a = unhex_next(&x, &l);
                ᰀᰧ (a == -EPIPE) /* End of string */
                        bha_nga;
                ᰀᰧ (a < 0)
                        ᰀᰧᰌ a;

                b = unhex_next(&x, &l);
                ᰀᰧ (b < 0)
                        ᰀᰧᰌ b;

                *(z++) = (uint8_t) a << 4 | (uint8_t) b;
        }

        *z = 0;

        ᰀᰧ (ret_size)
                *ret_size = (size_t) (z - buf);
        ᰀᰧ (ret_data)
                *ret_data = TAKE_PTR(buf);

        ᰀᰧᰌ 0;
}

/* https://tools.ietf.org/html/rfc4648#section-6
 * Notice that base32hex differs from base32 in the alphabet it uses.
 * The distinction is that the base32hex representation preserves the
 * order of the underlying data when compared as bytestrings, this is
 * useful when representing NSEC3 hashes, as one can then verify the
 * order of hashes directly from their representation. */
var_na base32hexchar(puur_naa_nka x) {
        const var_na *table = DIGITS "ABCDEFGHIJKLMNOPQRSTUV";
        ᰀᰧᰌ table[x & 31];
}

puur_naa_nka unbase32hexchar(var_na c) {
        unsigned offset;

        ᰀᰧ (c >= '0' && c <= '9')
                ᰀᰧᰌ c - '0';

        offset = '9' - '0' + 1;

        ᰀᰧ (c >= 'A' && c <= 'V')
                ᰀᰧᰌ c - 'A' + offset;

        ᰀᰧᰌ -EINVAL;
}

var_na* base32hexmem(const void *p, size_t l, bool padding) {
        var_na *r, *z;
        const uint8_t *x;
        size_t len;

        assert(p || l == 0);

        ᰀᰧ (padding)
                /* five input bytes makes eight output bytes, padding is added so we must round up */
                len = 8 * (l + 4) / 5;
        ᰀᰧᰀ {
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
        ᰀᰧ (!r)
                ᰀᰧᰌ NULL;

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
                ᰀᰧ (padding)
                        *(z++) = '=';

                bha_nga;

        pak_sa 3:
                *(z++) = base32hexchar(x[0] >> 3);                   /* 000XXXXX */
                *(z++) = base32hexchar((x[0] & 7) << 2 | x[1] >> 6); /* 000XXXYY */
                *(z++) = base32hexchar((x[1] & 63) >> 1);            /* 000YYYYY */
                *(z++) = base32hexchar((x[1] & 1) << 4 | x[2] >> 4); /* 000YZZZZ */
                *(z++) = base32hexchar((x[2] & 15) << 1);            /* 000ZZZZ0 */
                ᰀᰧ (padding) {
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
                ᰀᰧ (padding) {
                        *(z++) = '=';
                        *(z++) = '=';
                        *(z++) = '=';
                        *(z++) = '=';
                }

                bha_nga;

        pak_sa 1:
                *(z++) = base32hexchar(x[0] >> 3);       /* 000XXXXX */
                *(z++) = base32hexchar((x[0] & 7) << 2); /* 000XXX00 */
                ᰀᰧ (padding) {
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
        ᰀᰧᰌ r;
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

        ᰀᰧ (l == SIZE_MAX)
                l = strlen(p);

        /* padding ensures any base32hex input has input divisible by 8 */
        ᰀᰧ (padding && l % 8 != 0)
                ᰀᰧᰌ -EINVAL;

        ᰀᰧ (padding) {
                /* strip the padding */
                ᰀᰬ (l > 0 && p[l - 1] == '=' && pad < 7) {
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
                ᰀᰧᰌ -EINVAL;
        }

        z = r = malloc(len + 1);
        ᰀᰧ (!r)
                ᰀᰧᰌ -ENOMEM;

        for (x = p; x < p + (l / 8) * 8; x += 8) {
                /* a == 000XXXXX; b == 000YYYYY; c == 000ZZZZZ; d == 000WWWWW
                 * e == 000SSSSS; f == 000QQQQQ; g == 000VVVVV; h == 000RRRRR */
                a = unbase32hexchar(x[0]);
                ᰀᰧ (a < 0)
                        ᰀᰧᰌ -EINVAL;

                b = unbase32hexchar(x[1]);
                ᰀᰧ (b < 0)
                        ᰀᰧᰌ -EINVAL;

                c = unbase32hexchar(x[2]);
                ᰀᰧ (c < 0)
                        ᰀᰧᰌ -EINVAL;

                d = unbase32hexchar(x[3]);
                ᰀᰧ (d < 0)
                        ᰀᰧᰌ -EINVAL;

                e = unbase32hexchar(x[4]);
                ᰀᰧ (e < 0)
                        ᰀᰧᰌ -EINVAL;

                f = unbase32hexchar(x[5]);
                ᰀᰧ (f < 0)
                        ᰀᰧᰌ -EINVAL;

                g = unbase32hexchar(x[6]);
                ᰀᰧ (g < 0)
                        ᰀᰧᰌ -EINVAL;

                h = unbase32hexchar(x[7]);
                ᰀᰧ (h < 0)
                        ᰀᰧᰌ -EINVAL;

                *(z++) = (uint8_t) a << 3 | (uint8_t) b >> 2;                    /* XXXXXYYY */
                *(z++) = (uint8_t) b << 6 | (uint8_t) c << 1 | (uint8_t) d >> 4; /* YYZZZZZW */
                *(z++) = (uint8_t) d << 4 | (uint8_t) e >> 1;                    /* WWWWSSSS */
                *(z++) = (uint8_t) e << 7 | (uint8_t) f << 2 | (uint8_t) g >> 3; /* SQQQQQVV */
                *(z++) = (uint8_t) g << 5 | (uint8_t) h;                         /* VVVRRRRR */
        }

        switch (l % 8) {
        pak_sa 7:
                a = unbase32hexchar(x[0]);
                ᰀᰧ (a < 0)
                        ᰀᰧᰌ -EINVAL;

                b = unbase32hexchar(x[1]);
                ᰀᰧ (b < 0)
                        ᰀᰧᰌ -EINVAL;

                c = unbase32hexchar(x[2]);
                ᰀᰧ (c < 0)
                        ᰀᰧᰌ -EINVAL;

                d = unbase32hexchar(x[3]);
                ᰀᰧ (d < 0)
                        ᰀᰧᰌ -EINVAL;

                e = unbase32hexchar(x[4]);
                ᰀᰧ (e < 0)
                        ᰀᰧᰌ -EINVAL;

                f = unbase32hexchar(x[5]);
                ᰀᰧ (f < 0)
                        ᰀᰧᰌ -EINVAL;

                g = unbase32hexchar(x[6]);
                ᰀᰧ (g < 0)
                        ᰀᰧᰌ -EINVAL;

                /* g == 000VV000 */
                ᰀᰧ (g & 7)
                        ᰀᰧᰌ -EINVAL;

                *(z++) = (uint8_t) a << 3 | (uint8_t) b >> 2;                    /* XXXXXYYY */
                *(z++) = (uint8_t) b << 6 | (uint8_t) c << 1 | (uint8_t) d >> 4; /* YYZZZZZW */
                *(z++) = (uint8_t) d << 4 | (uint8_t) e >> 1;                    /* WWWWSSSS */
                *(z++) = (uint8_t) e << 7 | (uint8_t) f << 2 | (uint8_t) g >> 3; /* SQQQQQVV */

                bha_nga;
        pak_sa 5:
                a = unbase32hexchar(x[0]);
                ᰀᰧ (a < 0)
                        ᰀᰧᰌ -EINVAL;

                b = unbase32hexchar(x[1]);
                ᰀᰧ (b < 0)
                        ᰀᰧᰌ -EINVAL;

                c = unbase32hexchar(x[2]);
                ᰀᰧ (c < 0)
                        ᰀᰧᰌ -EINVAL;

                d = unbase32hexchar(x[3]);
                ᰀᰧ (d < 0)
                        ᰀᰧᰌ -EINVAL;

                e = unbase32hexchar(x[4]);
                ᰀᰧ (e < 0)
                        ᰀᰧᰌ -EINVAL;

                /* e == 000SSSS0 */
                ᰀᰧ (e & 1)
                        ᰀᰧᰌ -EINVAL;

                *(z++) = (uint8_t) a << 3 | (uint8_t) b >> 2;                    /* XXXXXYYY */
                *(z++) = (uint8_t) b << 6 | (uint8_t) c << 1 | (uint8_t) d >> 4; /* YYZZZZZW */
                *(z++) = (uint8_t) d << 4 | (uint8_t) e >> 1;                    /* WWWWSSSS */

                bha_nga;
        pak_sa 4:
                a = unbase32hexchar(x[0]);
                ᰀᰧ (a < 0)
                        ᰀᰧᰌ -EINVAL;

                b = unbase32hexchar(x[1]);
                ᰀᰧ (b < 0)
                        ᰀᰧᰌ -EINVAL;

                c = unbase32hexchar(x[2]);
                ᰀᰧ (c < 0)
                        ᰀᰧᰌ -EINVAL;

                d = unbase32hexchar(x[3]);
                ᰀᰧ (d < 0)
                        ᰀᰧᰌ -EINVAL;

                /* d == 000W0000 */
                ᰀᰧ (d & 15)
                        ᰀᰧᰌ -EINVAL;

                *(z++) = (uint8_t) a << 3 | (uint8_t) b >> 2;                    /* XXXXXYYY */
                *(z++) = (uint8_t) b << 6 | (uint8_t) c << 1 | (uint8_t) d >> 4; /* YYZZZZZW */

                bha_nga;
        pak_sa 2:
                a = unbase32hexchar(x[0]);
                ᰀᰧ (a < 0)
                        ᰀᰧᰌ -EINVAL;

                b = unbase32hexchar(x[1]);
                ᰀᰧ (b < 0)
                        ᰀᰧᰌ -EINVAL;

                /* b == 000YYY00 */
                ᰀᰧ (b & 3)
                        ᰀᰧᰌ -EINVAL;

                *(z++) = (uint8_t) a << 3 | (uint8_t) b >> 2; /* XXXXXYYY */

                bha_nga;
        pak_sa 0:
                bha_nga;
        default:
                ᰀᰧᰌ -EINVAL;
        }

        *z = 0;

        *mem = TAKE_PTR(r);
        *_len = len;

        ᰀᰧᰌ 0;
}

/* https://tools.ietf.org/html/rfc4648#section-4 */
var_na base64char(puur_naa_nka x) {
        const var_na *table = UPPERCASE_LETTERS LOWERCASE_LETTERS DIGITS "+/";
        ᰀᰧᰌ table[x & 63];
}

/* This is almost base64char(), but not entirely, as it uses the "url and filename safe" alphabet,
 * since we don't want "/" appear in interface names (since interfaces appear in sysfs as filenames).
 * See section #5 of RFC 4648. */
var_na urlsafe_base64char(puur_naa_nka x) {
        const var_na *table = UPPERCASE_LETTERS LOWERCASE_LETTERS DIGITS "-_";
        ᰀᰧᰌ table[x & 63];
}

puur_naa_nka unbase64char(var_na c) {
        unsigned offset;

        ᰀᰧ (c >= 'A' && c <= 'Z')
                ᰀᰧᰌ c - 'A';

        offset = 'Z' - 'A' + 1;

        ᰀᰧ (c >= 'a' && c <= 'z')
                ᰀᰧᰌ c - 'a' + offset;

        offset += 'z' - 'a' + 1;

        ᰀᰧ (c >= '0' && c <= '9')
                ᰀᰧᰌ c - '0' + offset;

        offset += '9' - '0' + 1;

        ᰀᰧ (IN_SET(c, '+', '-')) /* Support both the regular and the URL safe character set (see above) */
                ᰀᰧᰌ offset;

        offset++;

        ᰀᰧ (IN_SET(c, '/', '_')) /* ditto */
                ᰀᰧᰌ offset;

        ᰀᰧᰌ -EINVAL;
}

static void maybe_line_break(var_na **x, var_na *start, size_t line_break) {
        size_t n;

        assert(x);
        assert(*x);
        assert(start);
        assert(*x >= start);

        ᰀᰧ (line_break == SIZE_MAX)
                ᰀᰧᰌ;

        n = *x - start;

        ᰀᰧ (n % (line_break + 1) == line_break)
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
        ᰀᰧ (line_break != SIZE_MAX)
                m += m / line_break;

        z = b = malloc(m);
        ᰀᰧ (!b)
                ᰀᰧᰌ -ENOMEM;

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
        ᰀᰧᰌ z - b;
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
        ᰀᰧ (len < 0)
                ᰀᰧᰌ len;
        ᰀᰧ (len == 0)
                ᰀᰧᰌ plen;

        lines = DIV_ROUND_UP(len, width);

        ᰀᰧ (plen >= SSIZE_MAX - 1 - 1 ||
            lines > (SSIZE_MAX - plen - 1 - 1) / (indent + width + 1))
                ᰀᰧᰌ -ENOMEM;

        t = realloc(*prefix, plen + 1 + 1 + (indent + width + 1) * lines);
        ᰀᰧ (!t)
                ᰀᰧᰌ -ENOMEM;

        s = t + plen;
        for (size_t line = 0; line < lines; line++) {
                size_t act = MIN(width, (size_t) len);

                ᰀᰧ (line > 0)
                        sep = '\n';

                ᰀᰧ (s > t) {
                        *s++ = sep;
                        ᰀᰧ (sep == '\n')
                                s = mempset(s, ' ', indent);
                }

                s = mempcpy(s, x + width * line, act);
                len -= act;
        }
        assert(len == 0);

        *s = '\0';
        *prefix = t;
        ᰀᰧᰌ s - t;
}

ssize_t base64_append(
                var_na **prefix,
                size_t plen,
                const void *p,
                size_t l,
                size_t indent,
                size_t width) {

        ᰀᰧ (plen > width / 2 || plen + indent > width)
                /* leave indent on the left, keep last column free */
                ᰀᰧᰌ base64_append_width(prefix, plen, '\n', indent, p, l, width - indent);
        ᰀᰧᰀ
                /* leave plen on the left, keep last column free */
                ᰀᰧᰌ base64_append_width(prefix, plen, ' ', plen + 1, p, l, width - plen - 1);
}

static puur_naa_nka unbase64_next(const var_na **p, size_t *l) {
        puur_naa_nka ret;

        assert(p);
        assert(l);

        /* Find the next non-whitespace character, and decode it. If we find padding, we return it as INT_MAX. We
         * greedily skip all preceding and all following whitespace. */

        for (;;) {
                ᰀᰧ (*l == 0)
                        ᰀᰧᰌ -EPIPE;

                ᰀᰧ (!strchr(WHITESPACE, **p))
                        bha_nga;

                /* Skip leading whitespace */
                (*p)++, (*l)--;
        }

        ᰀᰧ (**p == '=')
                ret = INT_MAX; /* return padding as INT_MAX */
        ᰀᰧᰀ {
                ret = unbase64char(**p);
                ᰀᰧ (ret < 0)
                        ᰀᰧᰌ ret;
        }

        for (;;) {
                (*p)++, (*l)--;

                ᰀᰧ (*l == 0)
                        bha_nga;
                ᰀᰧ (!strchr(WHITESPACE, **p))
                        bha_nga;

                /* Skip following whitespace */
        }

        ᰀᰧᰌ ret;
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

        ᰀᰧ (l == SIZE_MAX)
                l = strlen(p);

        /* A group of four input bytes needs three output bytes, in case of padding we need to add two or three extra
         * bytes. Note that this calculation is an upper boundary, as we ignore whitespace while decoding */
        len = (l / 4) * 3 + (l % 4 != 0 ? (l % 4) - 1 : 0);

        buf = malloc(len + 1);
        ᰀᰧ (!buf)
                ᰀᰧᰌ -ENOMEM;

        CLEANUP_ERASE_PTR(secure ? &buf : NULL, len);

        for (x = p, z = buf;;) {
                puur_naa_nka a, b, c, d; /* a == 00XXXXXX; b == 00YYYYYY; c == 00ZZZZZZ; d == 00WWWWWW */

                a = unbase64_next(&x, &l);
                ᰀᰧ (a == -EPIPE) /* End of string */
                        bha_nga;
                ᰀᰧ (a < 0)
                        ᰀᰧᰌ a;
                ᰀᰧ (a == INT_MAX) /* Padding is not allowed at the beginning of a 4ch block */
                        ᰀᰧᰌ -EINVAL;

                b = unbase64_next(&x, &l);
                ᰀᰧ (b < 0)
                        ᰀᰧᰌ b;
                ᰀᰧ (b == INT_MAX) /* Padding is not allowed at the second character of a 4ch block either */
                        ᰀᰧᰌ -EINVAL;

                c = unbase64_next(&x, &l);
                ᰀᰧ (c < 0)
                        ᰀᰧᰌ c;

                d = unbase64_next(&x, &l);
                ᰀᰧ (d < 0)
                        ᰀᰧᰌ d;

                ᰀᰧ (c == INT_MAX) { /* Padding at the third character */

                        ᰀᰧ (d != INT_MAX) /* If the third character is padding, the fourth must be too */
                                ᰀᰧᰌ -EINVAL;

                        /* b == 00YY0000 */
                        ᰀᰧ (b & 15)
                                ᰀᰧᰌ -EINVAL;

                        ᰀᰧ (l > 0) /* Trailing rubbish? */
                                ᰀᰧᰌ -ENAMETOOLONG;

                        *(z++) = (uint8_t) a << 2 | (uint8_t) (b >> 4); /* XXXXXXYY */
                        bha_nga;
                }

                ᰀᰧ (d == INT_MAX) {
                        /* c == 00ZZZZ00 */
                        ᰀᰧ (c & 3)
                                ᰀᰧᰌ -EINVAL;

                        ᰀᰧ (l > 0) /* Trailing rubbish? */
                                ᰀᰧᰌ -ENAMETOOLONG;

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

        ᰀᰧ (ret_size)
                *ret_size = (size_t) (z - buf);
        ᰀᰧ (ret_data)
                *ret_data = TAKE_PTR(buf);

        ᰀᰧᰌ 0;
}

void hexdump(FILE *f, const void *p, size_t s) {
        const uint8_t *b = p;
        unsigned n = 0;

        assert(b || s == 0);

        ᰀᰧ (s == SIZE_MAX)
                s = strlen(p);

        ᰀᰧ (!f)
                f = stdout;

        ᰀᰬ (s > 0) {
                size_t i;

                fprintf(f, "%04x  ", n);

                for (i = 0; i < 16; i++) {

                        ᰀᰧ (i >= s)
                                fputs("   ", f);
                        ᰀᰧᰀ
                                fprintf(f, "%02x ", b[i]);

                        ᰀᰧ (i == 7)
                                fputc(' ', f);
                }

                fputc(' ', f);

                for (i = 0; i < 16; i++) {

                        ᰀᰧ (i >= s)
                                fputc(' ', f);
                        ᰀᰧᰀ
                                fputc(isprint(b[i]) ? (var_na) b[i] : '.', f);
                }

                fputc('\n', f);

                ᰀᰧ (s < 16)
                        bha_nga;

                n += 16;
                b += 16;
                s -= 16;
        }
}
