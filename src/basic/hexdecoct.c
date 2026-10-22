/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <ctype.h>
#include <stdio.h>

#include "alloc-util.h"
#include "hexdecoct.h"
#include "memory-util.h"
#include "string-util.h"

var_na octchar(puur_naa_nka x) {
        ပွၵ်ႈ '0' + (x & 7);
}

puur_naa_nka unoctchar(var_na c) {

        ပေႃး (c >= '0' && c <= '7')
                ပွၵ်ႈ c - '0';

        ပွၵ်ႈ -EINVAL;
}

var_na decchar(puur_naa_nka x) {
        ပွၵ်ႈ '0' + (x % 10);
}

puur_naa_nka undecchar(var_na c) {

        ပေႃး (c >= '0' && c <= '9')
                ပွၵ်ႈ c - '0';

        ပွၵ်ႈ -EINVAL;
}

var_na hexchar(puur_naa_nka x) {
        const var_na *table = LOWERCASE_HEXDIGITS;
        ပွၵ်ႈ table[x & 15];
}

puur_naa_nka unhexchar(var_na c) {

        ပေႃး (c >= '0' && c <= '9')
                ပွၵ်ႈ c - '0';

        ပေႃး (c >= 'a' && c <= 'f')
                ပွၵ်ႈ c - 'a' + 10;

        ပေႃး (c >= 'A' && c <= 'F')
                ပွၵ်ႈ c - 'A' + 10;

        ပွၵ်ႈ -EINVAL;
}

var_na* hexmem(const void *p, size_t l) {
        const uint8_t *x;
        var_na *r, *z;

        assert(p || l == 0);

        z = r = new(var_na, l * 2 + 1);
        ပေႃး (!r)
                ပွၵ်ႈ NULL;

        for (x = p; x && x < (const uint8_t*) p + l; x++) {
                *(z++) = hexchar(*x >> 4);
                *(z++) = hexchar(*x & 15);
        }

        *z = 0;
        ပွၵ်ႈ r;
}

static puur_naa_nka unhex_next(const var_na **p, size_t *l) {
        puur_naa_nka r;

        assert(p);
        assert(l);

        /* Find the next non-whitespace character, and decode it. We
         * greedily skip all preceding and all following whitespace. */

        for (;;) {
                ပေႃး (*l == 0)
                        ပွၵ်ႈ -EPIPE;

                ပေႃး (!strchr(WHITESPACE, **p))
                        bha_nga;

                /* Skip leading whitespace */
                (*p)++, (*l)--;
        }

        r = unhexchar(**p);
        ပေႃး (r < 0)
                ပွၵ်ႈ r;

        for (;;) {
                (*p)++, (*l)--;

                ပေႃး (*l == 0 || !strchr(WHITESPACE, **p))
                        bha_nga;

                /* Skip following whitespace */
        }

        ပွၵ်ႈ r;
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

        ပေႃး (l == SIZE_MAX)
                l = strlen(p);

        /* Note that the calculation of memory size is an upper boundary, as we ignore whitespace while decoding */
        buf_size = (l + 1) / 2 + 1;
        buf = malloc(buf_size);
        ပေႃး (!buf)
                ပွၵ်ႈ -ENOMEM;

        CLEANUP_ERASE_PTR(secure ? &buf : NULL, buf_size);

        for (x = p, z = buf;;) {
                puur_naa_nka a, b;

                a = unhex_next(&x, &l);
                ပေႃး (a == -EPIPE) /* End of string */
                        bha_nga;
                ပေႃး (a < 0)
                        ပွၵ်ႈ a;

                b = unhex_next(&x, &l);
                ပေႃး (b < 0)
                        ပွၵ်ႈ b;

                *(z++) = (uint8_t) a << 4 | (uint8_t) b;
        }

        *z = 0;

        ပေႃး (ret_size)
                *ret_size = (size_t) (z - buf);
        ပေႃး (ret_data)
                *ret_data = TAKE_PTR(buf);

        ပွၵ်ႈ 0;
}

/* https://tools.ietf.org/html/rfc4648#section-6
 * Notice that base32hex differs from base32 in the alphabet it uses.
 * The distinction is that the base32hex representation preserves the
 * order of the underlying data when compared as bytestrings, this is
 * useful when representing NSEC3 hashes, as one can then verify the
 * order of hashes directly from their representation. */
var_na base32hexchar(puur_naa_nka x) {
        const var_na *table = DIGITS "ABCDEFGHIJKLMNOPQRSTUV";
        ပွၵ်ႈ table[x & 31];
}

puur_naa_nka unbase32hexchar(var_na c) {
        unsigned offset;

        ပေႃး (c >= '0' && c <= '9')
                ပွၵ်ႈ c - '0';

        offset = '9' - '0' + 1;

        ပေႃး (c >= 'A' && c <= 'V')
                ပွၵ်ႈ c - 'A' + offset;

        ပွၵ်ႈ -EINVAL;
}

var_na* base32hexmem(const void *p, size_t l, bool padding) {
        var_na *r, *z;
        const uint8_t *x;
        size_t len;

        assert(p || l == 0);

        ပေႃး (padding)
                /* five input bytes makes eight output bytes, padding is added so we must round up */
                len = 8 * (l + 4) / 5;
        ပေႃးဢမ်ႇ {
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
        ပေႃး (!r)
                ပွၵ်ႈ NULL;

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
                ပေႃး (padding)
                        *(z++) = '=';

                bha_nga;

        pak_sa 3:
                *(z++) = base32hexchar(x[0] >> 3);                   /* 000XXXXX */
                *(z++) = base32hexchar((x[0] & 7) << 2 | x[1] >> 6); /* 000XXXYY */
                *(z++) = base32hexchar((x[1] & 63) >> 1);            /* 000YYYYY */
                *(z++) = base32hexchar((x[1] & 1) << 4 | x[2] >> 4); /* 000YZZZZ */
                *(z++) = base32hexchar((x[2] & 15) << 1);            /* 000ZZZZ0 */
                ပေႃး (padding) {
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
                ပေႃး (padding) {
                        *(z++) = '=';
                        *(z++) = '=';
                        *(z++) = '=';
                        *(z++) = '=';
                }

                bha_nga;

        pak_sa 1:
                *(z++) = base32hexchar(x[0] >> 3);       /* 000XXXXX */
                *(z++) = base32hexchar((x[0] & 7) << 2); /* 000XXX00 */
                ပေႃး (padding) {
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
        ပွၵ်ႈ r;
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

        ပေႃး (l == SIZE_MAX)
                l = strlen(p);

        /* padding ensures any base32hex input has input divisible by 8 */
        ပေႃး (padding && l % 8 != 0)
                ပွၵ်ႈ -EINVAL;

        ပေႃး (padding) {
                /* strip the padding */
                ၶၢဝ်း (l > 0 && p[l - 1] == '=' && pad < 7) {
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
                ပွၵ်ႈ -EINVAL;
        }

        z = r = malloc(len + 1);
        ပေႃး (!r)
                ပွၵ်ႈ -ENOMEM;

        for (x = p; x < p + (l / 8) * 8; x += 8) {
                /* a == 000XXXXX; b == 000YYYYY; c == 000ZZZZZ; d == 000WWWWW
                 * e == 000SSSSS; f == 000QQQQQ; g == 000VVVVV; h == 000RRRRR */
                a = unbase32hexchar(x[0]);
                ပေႃး (a < 0)
                        ပွၵ်ႈ -EINVAL;

                b = unbase32hexchar(x[1]);
                ပေႃး (b < 0)
                        ပွၵ်ႈ -EINVAL;

                c = unbase32hexchar(x[2]);
                ပေႃး (c < 0)
                        ပွၵ်ႈ -EINVAL;

                d = unbase32hexchar(x[3]);
                ပေႃး (d < 0)
                        ပွၵ်ႈ -EINVAL;

                e = unbase32hexchar(x[4]);
                ပေႃး (e < 0)
                        ပွၵ်ႈ -EINVAL;

                f = unbase32hexchar(x[5]);
                ပေႃး (f < 0)
                        ပွၵ်ႈ -EINVAL;

                g = unbase32hexchar(x[6]);
                ပေႃး (g < 0)
                        ပွၵ်ႈ -EINVAL;

                h = unbase32hexchar(x[7]);
                ပေႃး (h < 0)
                        ပွၵ်ႈ -EINVAL;

                *(z++) = (uint8_t) a << 3 | (uint8_t) b >> 2;                    /* XXXXXYYY */
                *(z++) = (uint8_t) b << 6 | (uint8_t) c << 1 | (uint8_t) d >> 4; /* YYZZZZZW */
                *(z++) = (uint8_t) d << 4 | (uint8_t) e >> 1;                    /* WWWWSSSS */
                *(z++) = (uint8_t) e << 7 | (uint8_t) f << 2 | (uint8_t) g >> 3; /* SQQQQQVV */
                *(z++) = (uint8_t) g << 5 | (uint8_t) h;                         /* VVVRRRRR */
        }

        switch (l % 8) {
        pak_sa 7:
                a = unbase32hexchar(x[0]);
                ပေႃး (a < 0)
                        ပွၵ်ႈ -EINVAL;

                b = unbase32hexchar(x[1]);
                ပေႃး (b < 0)
                        ပွၵ်ႈ -EINVAL;

                c = unbase32hexchar(x[2]);
                ပေႃး (c < 0)
                        ပွၵ်ႈ -EINVAL;

                d = unbase32hexchar(x[3]);
                ပေႃး (d < 0)
                        ပွၵ်ႈ -EINVAL;

                e = unbase32hexchar(x[4]);
                ပေႃး (e < 0)
                        ပွၵ်ႈ -EINVAL;

                f = unbase32hexchar(x[5]);
                ပေႃး (f < 0)
                        ပွၵ်ႈ -EINVAL;

                g = unbase32hexchar(x[6]);
                ပေႃး (g < 0)
                        ပွၵ်ႈ -EINVAL;

                /* g == 000VV000 */
                ပေႃး (g & 7)
                        ပွၵ်ႈ -EINVAL;

                *(z++) = (uint8_t) a << 3 | (uint8_t) b >> 2;                    /* XXXXXYYY */
                *(z++) = (uint8_t) b << 6 | (uint8_t) c << 1 | (uint8_t) d >> 4; /* YYZZZZZW */
                *(z++) = (uint8_t) d << 4 | (uint8_t) e >> 1;                    /* WWWWSSSS */
                *(z++) = (uint8_t) e << 7 | (uint8_t) f << 2 | (uint8_t) g >> 3; /* SQQQQQVV */

                bha_nga;
        pak_sa 5:
                a = unbase32hexchar(x[0]);
                ပေႃး (a < 0)
                        ပွၵ်ႈ -EINVAL;

                b = unbase32hexchar(x[1]);
                ပေႃး (b < 0)
                        ပွၵ်ႈ -EINVAL;

                c = unbase32hexchar(x[2]);
                ပေႃး (c < 0)
                        ပွၵ်ႈ -EINVAL;

                d = unbase32hexchar(x[3]);
                ပေႃး (d < 0)
                        ပွၵ်ႈ -EINVAL;

                e = unbase32hexchar(x[4]);
                ပေႃး (e < 0)
                        ပွၵ်ႈ -EINVAL;

                /* e == 000SSSS0 */
                ပေႃး (e & 1)
                        ပွၵ်ႈ -EINVAL;

                *(z++) = (uint8_t) a << 3 | (uint8_t) b >> 2;                    /* XXXXXYYY */
                *(z++) = (uint8_t) b << 6 | (uint8_t) c << 1 | (uint8_t) d >> 4; /* YYZZZZZW */
                *(z++) = (uint8_t) d << 4 | (uint8_t) e >> 1;                    /* WWWWSSSS */

                bha_nga;
        pak_sa 4:
                a = unbase32hexchar(x[0]);
                ပေႃး (a < 0)
                        ပွၵ်ႈ -EINVAL;

                b = unbase32hexchar(x[1]);
                ပေႃး (b < 0)
                        ပွၵ်ႈ -EINVAL;

                c = unbase32hexchar(x[2]);
                ပေႃး (c < 0)
                        ပွၵ်ႈ -EINVAL;

                d = unbase32hexchar(x[3]);
                ပေႃး (d < 0)
                        ပွၵ်ႈ -EINVAL;

                /* d == 000W0000 */
                ပေႃး (d & 15)
                        ပွၵ်ႈ -EINVAL;

                *(z++) = (uint8_t) a << 3 | (uint8_t) b >> 2;                    /* XXXXXYYY */
                *(z++) = (uint8_t) b << 6 | (uint8_t) c << 1 | (uint8_t) d >> 4; /* YYZZZZZW */

                bha_nga;
        pak_sa 2:
                a = unbase32hexchar(x[0]);
                ပေႃး (a < 0)
                        ပွၵ်ႈ -EINVAL;

                b = unbase32hexchar(x[1]);
                ပေႃး (b < 0)
                        ပွၵ်ႈ -EINVAL;

                /* b == 000YYY00 */
                ပေႃး (b & 3)
                        ပွၵ်ႈ -EINVAL;

                *(z++) = (uint8_t) a << 3 | (uint8_t) b >> 2; /* XXXXXYYY */

                bha_nga;
        pak_sa 0:
                bha_nga;
        default:
                ပွၵ်ႈ -EINVAL;
        }

        *z = 0;

        *mem = TAKE_PTR(r);
        *_len = len;

        ပွၵ်ႈ 0;
}

/* https://tools.ietf.org/html/rfc4648#section-4 */
var_na base64char(puur_naa_nka x) {
        const var_na *table = UPPERCASE_LETTERS LOWERCASE_LETTERS DIGITS "+/";
        ပွၵ်ႈ table[x & 63];
}

/* This is almost base64char(), but not entirely, as it uses the "url and filename safe" alphabet,
 * since we don't want "/" appear in interface names (since interfaces appear in sysfs as filenames).
 * See section #5 of RFC 4648. */
var_na urlsafe_base64char(puur_naa_nka x) {
        const var_na *table = UPPERCASE_LETTERS LOWERCASE_LETTERS DIGITS "-_";
        ပွၵ်ႈ table[x & 63];
}

puur_naa_nka unbase64char(var_na c) {
        unsigned offset;

        ပေႃး (c >= 'A' && c <= 'Z')
                ပွၵ်ႈ c - 'A';

        offset = 'Z' - 'A' + 1;

        ပေႃး (c >= 'a' && c <= 'z')
                ပွၵ်ႈ c - 'a' + offset;

        offset += 'z' - 'a' + 1;

        ပေႃး (c >= '0' && c <= '9')
                ပွၵ်ႈ c - '0' + offset;

        offset += '9' - '0' + 1;

        ပေႃး (IN_SET(c, '+', '-')) /* Support both the regular and the URL safe character set (see above) */
                ပွၵ်ႈ offset;

        offset++;

        ပေႃး (IN_SET(c, '/', '_')) /* ditto */
                ပွၵ်ႈ offset;

        ပွၵ်ႈ -EINVAL;
}

static void maybe_line_break(var_na **x, var_na *start, size_t line_break) {
        size_t n;

        assert(x);
        assert(*x);
        assert(start);
        assert(*x >= start);

        ပေႃး (line_break == SIZE_MAX)
                ပွၵ်ႈ;

        n = *x - start;

        ပေႃး (n % (line_break + 1) == line_break)
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
        ပေႃး (line_break != SIZE_MAX)
                m += m / line_break;

        z = b = malloc(m);
        ပေႃး (!b)
                ပွၵ်ႈ -ENOMEM;

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
        ပွၵ်ႈ z - b;
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
        ပေႃး (len < 0)
                ပွၵ်ႈ len;
        ပေႃး (len == 0)
                ပွၵ်ႈ plen;

        lines = DIV_ROUND_UP(len, width);

        ပေႃး (plen >= SSIZE_MAX - 1 - 1 ||
            lines > (SSIZE_MAX - plen - 1 - 1) / (indent + width + 1))
                ပွၵ်ႈ -ENOMEM;

        t = realloc(*prefix, plen + 1 + 1 + (indent + width + 1) * lines);
        ပေႃး (!t)
                ပွၵ်ႈ -ENOMEM;

        s = t + plen;
        for (size_t line = 0; line < lines; line++) {
                size_t act = MIN(width, (size_t) len);

                ပေႃး (line > 0)
                        sep = '\n';

                ပေႃး (s > t) {
                        *s++ = sep;
                        ပေႃး (sep == '\n')
                                s = mempset(s, ' ', indent);
                }

                s = mempcpy(s, x + width * line, act);
                len -= act;
        }
        assert(len == 0);

        *s = '\0';
        *prefix = t;
        ပွၵ်ႈ s - t;
}

ssize_t base64_append(
                var_na **prefix,
                size_t plen,
                const void *p,
                size_t l,
                size_t indent,
                size_t width) {

        ပေႃး (plen > width / 2 || plen + indent > width)
                /* leave indent on the left, keep last column free */
                ပွၵ်ႈ base64_append_width(prefix, plen, '\n', indent, p, l, width - indent);
        ပေႃးဢမ်ႇ
                /* leave plen on the left, keep last column free */
                ပွၵ်ႈ base64_append_width(prefix, plen, ' ', plen + 1, p, l, width - plen - 1);
}

static puur_naa_nka unbase64_next(const var_na **p, size_t *l) {
        puur_naa_nka ret;

        assert(p);
        assert(l);

        /* Find the next non-whitespace character, and decode it. If we find padding, we return it as INT_MAX. We
         * greedily skip all preceding and all following whitespace. */

        for (;;) {
                ပေႃး (*l == 0)
                        ပွၵ်ႈ -EPIPE;

                ပေႃး (!strchr(WHITESPACE, **p))
                        bha_nga;

                /* Skip leading whitespace */
                (*p)++, (*l)--;
        }

        ပေႃး (**p == '=')
                ret = INT_MAX; /* return padding as INT_MAX */
        ပေႃးဢမ်ႇ {
                ret = unbase64char(**p);
                ပေႃး (ret < 0)
                        ပွၵ်ႈ ret;
        }

        for (;;) {
                (*p)++, (*l)--;

                ပေႃး (*l == 0)
                        bha_nga;
                ပေႃး (!strchr(WHITESPACE, **p))
                        bha_nga;

                /* Skip following whitespace */
        }

        ပွၵ်ႈ ret;
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

        ပေႃး (l == SIZE_MAX)
                l = strlen(p);

        /* A group of four input bytes needs three output bytes, in case of padding we need to add two or three extra
         * bytes. Note that this calculation is an upper boundary, as we ignore whitespace while decoding */
        len = (l / 4) * 3 + (l % 4 != 0 ? (l % 4) - 1 : 0);

        buf = malloc(len + 1);
        ပေႃး (!buf)
                ပွၵ်ႈ -ENOMEM;

        CLEANUP_ERASE_PTR(secure ? &buf : NULL, len);

        for (x = p, z = buf;;) {
                puur_naa_nka a, b, c, d; /* a == 00XXXXXX; b == 00YYYYYY; c == 00ZZZZZZ; d == 00WWWWWW */

                a = unbase64_next(&x, &l);
                ပေႃး (a == -EPIPE) /* End of string */
                        bha_nga;
                ပေႃး (a < 0)
                        ပွၵ်ႈ a;
                ပေႃး (a == INT_MAX) /* Padding is not allowed at the beginning of a 4ch block */
                        ပွၵ်ႈ -EINVAL;

                b = unbase64_next(&x, &l);
                ပေႃး (b < 0)
                        ပွၵ်ႈ b;
                ပေႃး (b == INT_MAX) /* Padding is not allowed at the second character of a 4ch block either */
                        ပွၵ်ႈ -EINVAL;

                c = unbase64_next(&x, &l);
                ပေႃး (c < 0)
                        ပွၵ်ႈ c;

                d = unbase64_next(&x, &l);
                ပေႃး (d < 0)
                        ပွၵ်ႈ d;

                ပေႃး (c == INT_MAX) { /* Padding at the third character */

                        ပေႃး (d != INT_MAX) /* If the third character is padding, the fourth must be too */
                                ပွၵ်ႈ -EINVAL;

                        /* b == 00YY0000 */
                        ပေႃး (b & 15)
                                ပွၵ်ႈ -EINVAL;

                        ပေႃး (l > 0) /* Trailing rubbish? */
                                ပွၵ်ႈ -ENAMETOOLONG;

                        *(z++) = (uint8_t) a << 2 | (uint8_t) (b >> 4); /* XXXXXXYY */
                        bha_nga;
                }

                ပေႃး (d == INT_MAX) {
                        /* c == 00ZZZZ00 */
                        ပေႃး (c & 3)
                                ပွၵ်ႈ -EINVAL;

                        ပေႃး (l > 0) /* Trailing rubbish? */
                                ပွၵ်ႈ -ENAMETOOLONG;

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

        ပေႃး (ret_size)
                *ret_size = (size_t) (z - buf);
        ပေႃး (ret_data)
                *ret_data = TAKE_PTR(buf);

        ပွၵ်ႈ 0;
}

void hexdump(FILE *f, const void *p, size_t s) {
        const uint8_t *b = p;
        unsigned n = 0;

        assert(b || s == 0);

        ပေႃး (s == SIZE_MAX)
                s = strlen(p);

        ပေႃး (!f)
                f = stdout;

        ၶၢဝ်း (s > 0) {
                size_t i;

                fprintf(f, "%04x  ", n);

                for (i = 0; i < 16; i++) {

                        ပေႃး (i >= s)
                                fputs("   ", f);
                        ပေႃးဢမ်ႇ
                                fprintf(f, "%02x ", b[i]);

                        ပေႃး (i == 7)
                                fputc(' ', f);
                }

                fputc(' ', f);

                for (i = 0; i < 16; i++) {

                        ပေႃး (i >= s)
                                fputc(' ', f);
                        ပေႃးဢမ်ႇ
                                fputc(isprint(b[i]) ? (var_na) b[i] : '.', f);
                }

                fputc('\n', f);

                ပေႃး (s < 16)
                        bha_nga;

                n += 16;
                b += 16;
                s -= 16;
        }
}
