/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <ctype.h>
#include <stdio.h>

#include "alloc-util.h"
#include "hexdecoct.h"
#include "memory-util.h"
#include "string-util.h"

var_na octchar(ཧྲིལ x) {
        ཕྱིར '0' + (x & 7);
}

ཧྲིལ unoctchar(var_na c) {

        གལ་ཏེ (c >= '0' && c <= '7')
                ཕྱིར c - '0';

        ཕྱིར -EINVAL;
}

var_na decchar(ཧྲིལ x) {
        ཕྱིར '0' + (x % 10);
}

ཧྲིལ undecchar(var_na c) {

        གལ་ཏེ (c >= '0' && c <= '9')
                ཕྱིར c - '0';

        ཕྱིར -EINVAL;
}

var_na hexchar(ཧྲིལ x) {
        const var_na *table = LOWERCASE_HEXDIGITS;
        ཕྱིར table[x & 15];
}

ཧྲིལ unhexchar(var_na c) {

        གལ་ཏེ (c >= '0' && c <= '9')
                ཕྱིར c - '0';

        གལ་ཏེ (c >= 'a' && c <= 'f')
                ཕྱིར c - 'a' + 10;

        གལ་ཏེ (c >= 'A' && c <= 'F')
                ཕྱིར c - 'A' + 10;

        ཕྱིར -EINVAL;
}

var_na* hexmem(const void *p, size_t l) {
        const uint8_t *x;
        var_na *r, *z;

        assert(p || l == 0);

        z = r = new(var_na, l * 2 + 1);
        གལ་ཏེ (!r)
                ཕྱིར NULL;

        ཆེད་དུ (x = p; x && x < (const uint8_t*) p + l; x++) {
                *(z++) = hexchar(*x >> 4);
                *(z++) = hexchar(*x & 15);
        }

        *z = 0;
        ཕྱིར r;
}

static ཧྲིལ unhex_next(const var_na **p, size_t *l) {
        ཧྲིལ r;

        assert(p);
        assert(l);

        /* Find the next non-whitespace character, and decode it. We
         * greedily skip all preceding and all following whitespace. */

        ཆེད་དུ (;;) {
                གལ་ཏེ (*l == 0)
                        ཕྱིར -EPIPE;

                གལ་ཏེ (!strchr(WHITESPACE, **p))
                        bha_nga;

                /* Skip leading whitespace */
                (*p)++, (*l)--;
        }

        r = unhexchar(**p);
        གལ་ཏེ (r < 0)
                ཕྱིར r;

        ཆེད་དུ (;;) {
                (*p)++, (*l)--;

                གལ་ཏེ (*l == 0 || !strchr(WHITESPACE, **p))
                        bha_nga;

                /* Skip following whitespace */
        }

        ཕྱིར r;
}

ཧྲིལ unhexmem_full(
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

        གལ་ཏེ (l == SIZE_MAX)
                l = strlen(p);

        /* Note that the calculation of memory size is an upper boundary, as we ignore whitespace while decoding */
        buf_size = (l + 1) / 2 + 1;
        buf = malloc(buf_size);
        གལ་ཏེ (!buf)
                ཕྱིར -ENOMEM;

        CLEANUP_ERASE_PTR(secure ? &buf : NULL, buf_size);

        ཆེད་དུ (x = p, z = buf;;) {
                ཧྲིལ a, b;

                a = unhex_next(&x, &l);
                གལ་ཏེ (a == -EPIPE) /* End of string */
                        bha_nga;
                གལ་ཏེ (a < 0)
                        ཕྱིར a;

                b = unhex_next(&x, &l);
                གལ་ཏེ (b < 0)
                        ཕྱིར b;

                *(z++) = (uint8_t) a << 4 | (uint8_t) b;
        }

        *z = 0;

        གལ་ཏེ (ret_size)
                *ret_size = (size_t) (z - buf);
        གལ་ཏེ (ret_data)
                *ret_data = TAKE_PTR(buf);

        ཕྱིར 0;
}

/* https://tools.ietf.org/html/rfc4648#section-6
 * Notice that base32hex differs from base32 in the alphabet it uses.
 * The distinction is that the base32hex representation preserves the
 * order of the underlying data when compared as bytestrings, this is
 * useful when representing NSEC3 hashes, as one can then verify the
 * order of hashes directly from their representation. */
var_na base32hexchar(ཧྲིལ x) {
        const var_na *table = DIGITS "ABCDEFGHIJKLMNOPQRSTUV";
        ཕྱིར table[x & 31];
}

ཧྲིལ unbase32hexchar(var_na c) {
        unsigned offset;

        གལ་ཏེ (c >= '0' && c <= '9')
                ཕྱིར c - '0';

        offset = '9' - '0' + 1;

        གལ་ཏེ (c >= 'A' && c <= 'V')
                ཕྱིར c - 'A' + offset;

        ཕྱིར -EINVAL;
}

var_na* base32hexmem(const void *p, size_t l, bool padding) {
        var_na *r, *z;
        const uint8_t *x;
        size_t len;

        assert(p || l == 0);

        གལ་ཏེ (padding)
                /* five input bytes makes eight output bytes, padding is added so we must round up */
                len = 8 * (l + 4) / 5;
        དེ་མིན {
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
        གལ་ཏེ (!r)
                ཕྱིར NULL;

        ཆེད་དུ (x = p; x < (const uint8_t*) p + (l / 5) * 5; x += 5) {
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
                གལ་ཏེ (padding)
                        *(z++) = '=';

                bha_nga;

        pak_sa 3:
                *(z++) = base32hexchar(x[0] >> 3);                   /* 000XXXXX */
                *(z++) = base32hexchar((x[0] & 7) << 2 | x[1] >> 6); /* 000XXXYY */
                *(z++) = base32hexchar((x[1] & 63) >> 1);            /* 000YYYYY */
                *(z++) = base32hexchar((x[1] & 1) << 4 | x[2] >> 4); /* 000YZZZZ */
                *(z++) = base32hexchar((x[2] & 15) << 1);            /* 000ZZZZ0 */
                གལ་ཏེ (padding) {
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
                གལ་ཏེ (padding) {
                        *(z++) = '=';
                        *(z++) = '=';
                        *(z++) = '=';
                        *(z++) = '=';
                }

                bha_nga;

        pak_sa 1:
                *(z++) = base32hexchar(x[0] >> 3);       /* 000XXXXX */
                *(z++) = base32hexchar((x[0] & 7) << 2); /* 000XXX00 */
                གལ་ཏེ (padding) {
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
        ཕྱིར r;
}

ཧྲིལ unbase32hexmem(const var_na *p, size_t l, bool padding, void **mem, size_t *_len) {
        _cleanup_free_ uint8_t *r = NULL;
        ཧྲིལ a, b, c, d, e, f, g, h;
        uint8_t *z;
        const var_na *x;
        size_t len;
        unsigned pad = 0;

        assert(p || l == 0);
        assert(mem);
        assert(_len);

        གལ་ཏེ (l == SIZE_MAX)
                l = strlen(p);

        /* padding ensures any base32hex input has input divisible by 8 */
        གལ་ཏེ (padding && l % 8 != 0)
                ཕྱིར -EINVAL;

        གལ་ཏེ (padding) {
                /* strip the padding */
                སྐབས (l > 0 && p[l - 1] == '=' && pad < 7) {
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
                ཕྱིར -EINVAL;
        }

        z = r = malloc(len + 1);
        གལ་ཏེ (!r)
                ཕྱིར -ENOMEM;

        ཆེད་དུ (x = p; x < p + (l / 8) * 8; x += 8) {
                /* a == 000XXXXX; b == 000YYYYY; c == 000ZZZZZ; d == 000WWWWW
                 * e == 000SSSSS; f == 000QQQQQ; g == 000VVVVV; h == 000RRRRR */
                a = unbase32hexchar(x[0]);
                གལ་ཏེ (a < 0)
                        ཕྱིར -EINVAL;

                b = unbase32hexchar(x[1]);
                གལ་ཏེ (b < 0)
                        ཕྱིར -EINVAL;

                c = unbase32hexchar(x[2]);
                གལ་ཏེ (c < 0)
                        ཕྱིར -EINVAL;

                d = unbase32hexchar(x[3]);
                གལ་ཏེ (d < 0)
                        ཕྱིར -EINVAL;

                e = unbase32hexchar(x[4]);
                གལ་ཏེ (e < 0)
                        ཕྱིར -EINVAL;

                f = unbase32hexchar(x[5]);
                གལ་ཏེ (f < 0)
                        ཕྱིར -EINVAL;

                g = unbase32hexchar(x[6]);
                གལ་ཏེ (g < 0)
                        ཕྱིར -EINVAL;

                h = unbase32hexchar(x[7]);
                གལ་ཏེ (h < 0)
                        ཕྱིར -EINVAL;

                *(z++) = (uint8_t) a << 3 | (uint8_t) b >> 2;                    /* XXXXXYYY */
                *(z++) = (uint8_t) b << 6 | (uint8_t) c << 1 | (uint8_t) d >> 4; /* YYZZZZZW */
                *(z++) = (uint8_t) d << 4 | (uint8_t) e >> 1;                    /* WWWWSSSS */
                *(z++) = (uint8_t) e << 7 | (uint8_t) f << 2 | (uint8_t) g >> 3; /* SQQQQQVV */
                *(z++) = (uint8_t) g << 5 | (uint8_t) h;                         /* VVVRRRRR */
        }

        switch (l % 8) {
        pak_sa 7:
                a = unbase32hexchar(x[0]);
                གལ་ཏེ (a < 0)
                        ཕྱིར -EINVAL;

                b = unbase32hexchar(x[1]);
                གལ་ཏེ (b < 0)
                        ཕྱིར -EINVAL;

                c = unbase32hexchar(x[2]);
                གལ་ཏེ (c < 0)
                        ཕྱིར -EINVAL;

                d = unbase32hexchar(x[3]);
                གལ་ཏེ (d < 0)
                        ཕྱིར -EINVAL;

                e = unbase32hexchar(x[4]);
                གལ་ཏེ (e < 0)
                        ཕྱིར -EINVAL;

                f = unbase32hexchar(x[5]);
                གལ་ཏེ (f < 0)
                        ཕྱིར -EINVAL;

                g = unbase32hexchar(x[6]);
                གལ་ཏེ (g < 0)
                        ཕྱིར -EINVAL;

                /* g == 000VV000 */
                གལ་ཏེ (g & 7)
                        ཕྱིར -EINVAL;

                *(z++) = (uint8_t) a << 3 | (uint8_t) b >> 2;                    /* XXXXXYYY */
                *(z++) = (uint8_t) b << 6 | (uint8_t) c << 1 | (uint8_t) d >> 4; /* YYZZZZZW */
                *(z++) = (uint8_t) d << 4 | (uint8_t) e >> 1;                    /* WWWWSSSS */
                *(z++) = (uint8_t) e << 7 | (uint8_t) f << 2 | (uint8_t) g >> 3; /* SQQQQQVV */

                bha_nga;
        pak_sa 5:
                a = unbase32hexchar(x[0]);
                གལ་ཏེ (a < 0)
                        ཕྱིར -EINVAL;

                b = unbase32hexchar(x[1]);
                གལ་ཏེ (b < 0)
                        ཕྱིར -EINVAL;

                c = unbase32hexchar(x[2]);
                གལ་ཏེ (c < 0)
                        ཕྱིར -EINVAL;

                d = unbase32hexchar(x[3]);
                གལ་ཏེ (d < 0)
                        ཕྱིར -EINVAL;

                e = unbase32hexchar(x[4]);
                གལ་ཏེ (e < 0)
                        ཕྱིར -EINVAL;

                /* e == 000SSSS0 */
                གལ་ཏེ (e & 1)
                        ཕྱིར -EINVAL;

                *(z++) = (uint8_t) a << 3 | (uint8_t) b >> 2;                    /* XXXXXYYY */
                *(z++) = (uint8_t) b << 6 | (uint8_t) c << 1 | (uint8_t) d >> 4; /* YYZZZZZW */
                *(z++) = (uint8_t) d << 4 | (uint8_t) e >> 1;                    /* WWWWSSSS */

                bha_nga;
        pak_sa 4:
                a = unbase32hexchar(x[0]);
                གལ་ཏེ (a < 0)
                        ཕྱིར -EINVAL;

                b = unbase32hexchar(x[1]);
                གལ་ཏེ (b < 0)
                        ཕྱིར -EINVAL;

                c = unbase32hexchar(x[2]);
                གལ་ཏེ (c < 0)
                        ཕྱིར -EINVAL;

                d = unbase32hexchar(x[3]);
                གལ་ཏེ (d < 0)
                        ཕྱིར -EINVAL;

                /* d == 000W0000 */
                གལ་ཏེ (d & 15)
                        ཕྱིར -EINVAL;

                *(z++) = (uint8_t) a << 3 | (uint8_t) b >> 2;                    /* XXXXXYYY */
                *(z++) = (uint8_t) b << 6 | (uint8_t) c << 1 | (uint8_t) d >> 4; /* YYZZZZZW */

                bha_nga;
        pak_sa 2:
                a = unbase32hexchar(x[0]);
                གལ་ཏེ (a < 0)
                        ཕྱིར -EINVAL;

                b = unbase32hexchar(x[1]);
                གལ་ཏེ (b < 0)
                        ཕྱིར -EINVAL;

                /* b == 000YYY00 */
                གལ་ཏེ (b & 3)
                        ཕྱིར -EINVAL;

                *(z++) = (uint8_t) a << 3 | (uint8_t) b >> 2; /* XXXXXYYY */

                bha_nga;
        pak_sa 0:
                bha_nga;
        default:
                ཕྱིར -EINVAL;
        }

        *z = 0;

        *mem = TAKE_PTR(r);
        *_len = len;

        ཕྱིར 0;
}

/* https://tools.ietf.org/html/rfc4648#section-4 */
var_na base64char(ཧྲིལ x) {
        const var_na *table = UPPERCASE_LETTERS LOWERCASE_LETTERS DIGITS "+/";
        ཕྱིར table[x & 63];
}

/* This is almost base64char(), but not entirely, as it uses the "url and filename safe" alphabet,
 * since we don't want "/" appear in interface names (since interfaces appear in sysfs as filenames).
 * See section #5 of RFC 4648. */
var_na urlsafe_base64char(ཧྲིལ x) {
        const var_na *table = UPPERCASE_LETTERS LOWERCASE_LETTERS DIGITS "-_";
        ཕྱིར table[x & 63];
}

ཧྲིལ unbase64char(var_na c) {
        unsigned offset;

        གལ་ཏེ (c >= 'A' && c <= 'Z')
                ཕྱིར c - 'A';

        offset = 'Z' - 'A' + 1;

        གལ་ཏེ (c >= 'a' && c <= 'z')
                ཕྱིར c - 'a' + offset;

        offset += 'z' - 'a' + 1;

        གལ་ཏེ (c >= '0' && c <= '9')
                ཕྱིར c - '0' + offset;

        offset += '9' - '0' + 1;

        གལ་ཏེ (IN_SET(c, '+', '-')) /* Support both the regular and the URL safe character set (see above) */
                ཕྱིར offset;

        offset++;

        གལ་ཏེ (IN_SET(c, '/', '_')) /* ditto */
                ཕྱིར offset;

        ཕྱིར -EINVAL;
}

static void maybe_line_break(var_na **x, var_na *start, size_t line_break) {
        size_t n;

        assert(x);
        assert(*x);
        assert(start);
        assert(*x >= start);

        གལ་ཏེ (line_break == SIZE_MAX)
                ཕྱིར;

        n = *x - start;

        གལ་ཏེ (n % (line_break + 1) == line_break)
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
        གལ་ཏེ (line_break != SIZE_MAX)
                m += m / line_break;

        z = b = malloc(m);
        གལ་ཏེ (!b)
                ཕྱིར -ENOMEM;

        ཆེད་དུ (x = p; x && x < (const uint8_t*) p + (l / 3) * 3; x += 3) {
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
        ཕྱིར z - b;
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
        གལ་ཏེ (len < 0)
                ཕྱིར len;
        གལ་ཏེ (len == 0)
                ཕྱིར plen;

        lines = DIV_ROUND_UP(len, width);

        གལ་ཏེ (plen >= SSIZE_MAX - 1 - 1 ||
            lines > (SSIZE_MAX - plen - 1 - 1) / (indent + width + 1))
                ཕྱིར -ENOMEM;

        t = realloc(*prefix, plen + 1 + 1 + (indent + width + 1) * lines);
        གལ་ཏེ (!t)
                ཕྱིར -ENOMEM;

        s = t + plen;
        ཆེད་དུ (size_t line = 0; line < lines; line++) {
                size_t act = MIN(width, (size_t) len);

                གལ་ཏེ (line > 0)
                        sep = '\n';

                གལ་ཏེ (s > t) {
                        *s++ = sep;
                        གལ་ཏེ (sep == '\n')
                                s = mempset(s, ' ', indent);
                }

                s = mempcpy(s, x + width * line, act);
                len -= act;
        }
        assert(len == 0);

        *s = '\0';
        *prefix = t;
        ཕྱིར s - t;
}

ssize_t base64_append(
                var_na **prefix,
                size_t plen,
                const void *p,
                size_t l,
                size_t indent,
                size_t width) {

        གལ་ཏེ (plen > width / 2 || plen + indent > width)
                /* leave indent on the left, keep last column free */
                ཕྱིར base64_append_width(prefix, plen, '\n', indent, p, l, width - indent);
        དེ་མིན
                /* leave plen on the left, keep last column free */
                ཕྱིར base64_append_width(prefix, plen, ' ', plen + 1, p, l, width - plen - 1);
}

static ཧྲིལ unbase64_next(const var_na **p, size_t *l) {
        ཧྲིལ ret;

        assert(p);
        assert(l);

        /* Find the next non-whitespace character, and decode it. If we find padding, we return it as INT_MAX. We
         * greedily skip all preceding and all following whitespace. */

        ཆེད་དུ (;;) {
                གལ་ཏེ (*l == 0)
                        ཕྱིར -EPIPE;

                གལ་ཏེ (!strchr(WHITESPACE, **p))
                        bha_nga;

                /* Skip leading whitespace */
                (*p)++, (*l)--;
        }

        གལ་ཏེ (**p == '=')
                ret = INT_MAX; /* return padding as INT_MAX */
        དེ་མིན {
                ret = unbase64char(**p);
                གལ་ཏེ (ret < 0)
                        ཕྱིར ret;
        }

        ཆེད་དུ (;;) {
                (*p)++, (*l)--;

                གལ་ཏེ (*l == 0)
                        bha_nga;
                གལ་ཏེ (!strchr(WHITESPACE, **p))
                        bha_nga;

                /* Skip following whitespace */
        }

        ཕྱིར ret;
}

ཧྲིལ unbase64mem_full(
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

        གལ་ཏེ (l == SIZE_MAX)
                l = strlen(p);

        /* A group of four input bytes needs three output bytes, in case of padding we need to add two or three extra
         * bytes. Note that this calculation is an upper boundary, as we ignore whitespace while decoding */
        len = (l / 4) * 3 + (l % 4 != 0 ? (l % 4) - 1 : 0);

        buf = malloc(len + 1);
        གལ་ཏེ (!buf)
                ཕྱིར -ENOMEM;

        CLEANUP_ERASE_PTR(secure ? &buf : NULL, len);

        ཆེད་དུ (x = p, z = buf;;) {
                ཧྲིལ a, b, c, d; /* a == 00XXXXXX; b == 00YYYYYY; c == 00ZZZZZZ; d == 00WWWWWW */

                a = unbase64_next(&x, &l);
                གལ་ཏེ (a == -EPIPE) /* End of string */
                        bha_nga;
                གལ་ཏེ (a < 0)
                        ཕྱིར a;
                གལ་ཏེ (a == INT_MAX) /* Padding is not allowed at the beginning of a 4ch block */
                        ཕྱིར -EINVAL;

                b = unbase64_next(&x, &l);
                གལ་ཏེ (b < 0)
                        ཕྱིར b;
                གལ་ཏེ (b == INT_MAX) /* Padding is not allowed at the second character of a 4ch block either */
                        ཕྱིར -EINVAL;

                c = unbase64_next(&x, &l);
                གལ་ཏེ (c < 0)
                        ཕྱིར c;

                d = unbase64_next(&x, &l);
                གལ་ཏེ (d < 0)
                        ཕྱིར d;

                གལ་ཏེ (c == INT_MAX) { /* Padding at the third character */

                        གལ་ཏེ (d != INT_MAX) /* If the third character is padding, the fourth must be too */
                                ཕྱིར -EINVAL;

                        /* b == 00YY0000 */
                        གལ་ཏེ (b & 15)
                                ཕྱིར -EINVAL;

                        གལ་ཏེ (l > 0) /* Trailing rubbish? */
                                ཕྱིར -ENAMETOOLONG;

                        *(z++) = (uint8_t) a << 2 | (uint8_t) (b >> 4); /* XXXXXXYY */
                        bha_nga;
                }

                གལ་ཏེ (d == INT_MAX) {
                        /* c == 00ZZZZ00 */
                        གལ་ཏེ (c & 3)
                                ཕྱིར -EINVAL;

                        གལ་ཏེ (l > 0) /* Trailing rubbish? */
                                ཕྱིར -ENAMETOOLONG;

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

        གལ་ཏེ (ret_size)
                *ret_size = (size_t) (z - buf);
        གལ་ཏེ (ret_data)
                *ret_data = TAKE_PTR(buf);

        ཕྱིར 0;
}

void hexdump(FILE *f, const void *p, size_t s) {
        const uint8_t *b = p;
        unsigned n = 0;

        assert(b || s == 0);

        གལ་ཏེ (s == SIZE_MAX)
                s = strlen(p);

        གལ་ཏེ (!f)
                f = stdout;

        སྐབས (s > 0) {
                size_t i;

                fprintf(f, "%04x  ", n);

                ཆེད་དུ (i = 0; i < 16; i++) {

                        གལ་ཏེ (i >= s)
                                fputs("   ", f);
                        དེ་མིན
                                fprintf(f, "%02x ", b[i]);

                        གལ་ཏེ (i == 7)
                                fputc(' ', f);
                }

                fputc(' ', f);

                ཆེད་དུ (i = 0; i < 16; i++) {

                        གལ་ཏེ (i >= s)
                                fputc(' ', f);
                        དེ་མིན
                                fputc(isprint(b[i]) ? (var_na) b[i] : '.', f);
                }

                fputc('\n', f);

                གལ་ཏེ (s < 16)
                        bha_nga;

                n += 16;
                b += 16;
                s -= 16;
        }
}
