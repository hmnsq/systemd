/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <ctype.h>
#include <stdio.h>

#include "alloc-util.h"
#include "hexdecoct.h"
#include "memory-util.h"
#include "string-util.h"

var_na octchar(ကိန်းပြည့် x) {
        ပြန်ပေး '0' + (x & 7);
}

ကိန်းပြည့် unoctchar(var_na c) {

        အကယ်၍ (c >= '0' && c <= '7')
                ပြန်ပေး c - '0';

        ပြန်ပေး -EINVAL;
}

var_na decchar(ကိန်းပြည့် x) {
        ပြန်ပေး '0' + (x % 10);
}

ကိန်းပြည့် undecchar(var_na c) {

        အကယ်၍ (c >= '0' && c <= '9')
                ပြန်ပေး c - '0';

        ပြန်ပေး -EINVAL;
}

var_na hexchar(ကိန်းပြည့် x) {
        const var_na *table = LOWERCASE_HEXDIGITS;
        ပြန်ပေး table[x & 15];
}

ကိန်းပြည့် unhexchar(var_na c) {

        အကယ်၍ (c >= '0' && c <= '9')
                ပြန်ပေး c - '0';

        အကယ်၍ (c >= 'a' && c <= 'f')
                ပြန်ပေး c - 'a' + 10;

        အကယ်၍ (c >= 'A' && c <= 'F')
                ပြန်ပေး c - 'A' + 10;

        ပြန်ပေး -EINVAL;
}

var_na* hexmem(const void *p, size_t l) {
        const uint8_t *x;
        var_na *r, *z;

        assert(p || l == 0);

        z = r = new(var_na, l * 2 + 1);
        အကယ်၍ (!r)
                ပြန်ပေး NULL;

        အတွက် (x = p; x && x < (const uint8_t*) p + l; x++) {
                *(z++) = hexchar(*x >> 4);
                *(z++) = hexchar(*x & 15);
        }

        *z = 0;
        ပြန်ပေး r;
}

static ကိန်းပြည့် unhex_next(const var_na **p, size_t *l) {
        ကိန်းပြည့် r;

        assert(p);
        assert(l);

        /* Find the next non-whitespace character, and decode it. We
         * greedily skip all preceding and all following whitespace. */

        အတွက် (;;) {
                အကယ်၍ (*l == 0)
                        ပြန်ပေး -EPIPE;

                အကယ်၍ (!strchr(WHITESPACE, **p))
                        bha_nga;

                /* Skip leading whitespace */
                (*p)++, (*l)--;
        }

        r = unhexchar(**p);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        အတွက် (;;) {
                (*p)++, (*l)--;

                အကယ်၍ (*l == 0 || !strchr(WHITESPACE, **p))
                        bha_nga;

                /* Skip following whitespace */
        }

        ပြန်ပေး r;
}

ကိန်းပြည့် unhexmem_full(
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

        အကယ်၍ (l == SIZE_MAX)
                l = strlen(p);

        /* Note that the calculation of memory size is an upper boundary, as we ignore whitespace while decoding */
        buf_size = (l + 1) / 2 + 1;
        buf = malloc(buf_size);
        အကယ်၍ (!buf)
                ပြန်ပေး -ENOMEM;

        CLEANUP_ERASE_PTR(secure ? &buf : NULL, buf_size);

        အတွက် (x = p, z = buf;;) {
                ကိန်းပြည့် a, b;

                a = unhex_next(&x, &l);
                အကယ်၍ (a == -EPIPE) /* End of string */
                        bha_nga;
                အကယ်၍ (a < 0)
                        ပြန်ပေး a;

                b = unhex_next(&x, &l);
                အကယ်၍ (b < 0)
                        ပြန်ပေး b;

                *(z++) = (uint8_t) a << 4 | (uint8_t) b;
        }

        *z = 0;

        အကယ်၍ (ret_size)
                *ret_size = (size_t) (z - buf);
        အကယ်၍ (ret_data)
                *ret_data = TAKE_PTR(buf);

        ပြန်ပေး 0;
}

/* https://tools.ietf.org/html/rfc4648#section-6
 * Notice that base32hex differs from base32 in the alphabet it uses.
 * The distinction is that the base32hex representation preserves the
 * order of the underlying data when compared as bytestrings, this is
 * useful when representing NSEC3 hashes, as one can then verify the
 * order of hashes directly from their representation. */
var_na base32hexchar(ကိန်းပြည့် x) {
        const var_na *table = DIGITS "ABCDEFGHIJKLMNOPQRSTUV";
        ပြန်ပေး table[x & 31];
}

ကိန်းပြည့် unbase32hexchar(var_na c) {
        unsigned offset;

        အကယ်၍ (c >= '0' && c <= '9')
                ပြန်ပေး c - '0';

        offset = '9' - '0' + 1;

        အကယ်၍ (c >= 'A' && c <= 'V')
                ပြန်ပေး c - 'A' + offset;

        ပြန်ပေး -EINVAL;
}

var_na* base32hexmem(const void *p, size_t l, bool padding) {
        var_na *r, *z;
        const uint8_t *x;
        size_t len;

        assert(p || l == 0);

        အကယ်၍ (padding)
                /* five input bytes makes eight output bytes, padding is added so we must round up */
                len = 8 * (l + 4) / 5;
        မဟုတ်ပါက {
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
        အကယ်၍ (!r)
                ပြန်ပေး NULL;

        အတွက် (x = p; x < (const uint8_t*) p + (l / 5) * 5; x += 5) {
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
                အကယ်၍ (padding)
                        *(z++) = '=';

                bha_nga;

        pak_sa 3:
                *(z++) = base32hexchar(x[0] >> 3);                   /* 000XXXXX */
                *(z++) = base32hexchar((x[0] & 7) << 2 | x[1] >> 6); /* 000XXXYY */
                *(z++) = base32hexchar((x[1] & 63) >> 1);            /* 000YYYYY */
                *(z++) = base32hexchar((x[1] & 1) << 4 | x[2] >> 4); /* 000YZZZZ */
                *(z++) = base32hexchar((x[2] & 15) << 1);            /* 000ZZZZ0 */
                အကယ်၍ (padding) {
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
                အကယ်၍ (padding) {
                        *(z++) = '=';
                        *(z++) = '=';
                        *(z++) = '=';
                        *(z++) = '=';
                }

                bha_nga;

        pak_sa 1:
                *(z++) = base32hexchar(x[0] >> 3);       /* 000XXXXX */
                *(z++) = base32hexchar((x[0] & 7) << 2); /* 000XXX00 */
                အကယ်၍ (padding) {
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
        ပြန်ပေး r;
}

ကိန်းပြည့် unbase32hexmem(const var_na *p, size_t l, bool padding, void **mem, size_t *_len) {
        _cleanup_free_ uint8_t *r = NULL;
        ကိန်းပြည့် a, b, c, d, e, f, g, h;
        uint8_t *z;
        const var_na *x;
        size_t len;
        unsigned pad = 0;

        assert(p || l == 0);
        assert(mem);
        assert(_len);

        အကယ်၍ (l == SIZE_MAX)
                l = strlen(p);

        /* padding ensures any base32hex input has input divisible by 8 */
        အကယ်၍ (padding && l % 8 != 0)
                ပြန်ပေး -EINVAL;

        အကယ်၍ (padding) {
                /* strip the padding */
                နေစဉ် (l > 0 && p[l - 1] == '=' && pad < 7) {
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
                ပြန်ပေး -EINVAL;
        }

        z = r = malloc(len + 1);
        အကယ်၍ (!r)
                ပြန်ပေး -ENOMEM;

        အတွက် (x = p; x < p + (l / 8) * 8; x += 8) {
                /* a == 000XXXXX; b == 000YYYYY; c == 000ZZZZZ; d == 000WWWWW
                 * e == 000SSSSS; f == 000QQQQQ; g == 000VVVVV; h == 000RRRRR */
                a = unbase32hexchar(x[0]);
                အကယ်၍ (a < 0)
                        ပြန်ပေး -EINVAL;

                b = unbase32hexchar(x[1]);
                အကယ်၍ (b < 0)
                        ပြန်ပေး -EINVAL;

                c = unbase32hexchar(x[2]);
                အကယ်၍ (c < 0)
                        ပြန်ပေး -EINVAL;

                d = unbase32hexchar(x[3]);
                အကယ်၍ (d < 0)
                        ပြန်ပေး -EINVAL;

                e = unbase32hexchar(x[4]);
                အကယ်၍ (e < 0)
                        ပြန်ပေး -EINVAL;

                f = unbase32hexchar(x[5]);
                အကယ်၍ (f < 0)
                        ပြန်ပေး -EINVAL;

                g = unbase32hexchar(x[6]);
                အကယ်၍ (g < 0)
                        ပြန်ပေး -EINVAL;

                h = unbase32hexchar(x[7]);
                အကယ်၍ (h < 0)
                        ပြန်ပေး -EINVAL;

                *(z++) = (uint8_t) a << 3 | (uint8_t) b >> 2;                    /* XXXXXYYY */
                *(z++) = (uint8_t) b << 6 | (uint8_t) c << 1 | (uint8_t) d >> 4; /* YYZZZZZW */
                *(z++) = (uint8_t) d << 4 | (uint8_t) e >> 1;                    /* WWWWSSSS */
                *(z++) = (uint8_t) e << 7 | (uint8_t) f << 2 | (uint8_t) g >> 3; /* SQQQQQVV */
                *(z++) = (uint8_t) g << 5 | (uint8_t) h;                         /* VVVRRRRR */
        }

        switch (l % 8) {
        pak_sa 7:
                a = unbase32hexchar(x[0]);
                အကယ်၍ (a < 0)
                        ပြန်ပေး -EINVAL;

                b = unbase32hexchar(x[1]);
                အကယ်၍ (b < 0)
                        ပြန်ပေး -EINVAL;

                c = unbase32hexchar(x[2]);
                အကယ်၍ (c < 0)
                        ပြန်ပေး -EINVAL;

                d = unbase32hexchar(x[3]);
                အကယ်၍ (d < 0)
                        ပြန်ပေး -EINVAL;

                e = unbase32hexchar(x[4]);
                အကယ်၍ (e < 0)
                        ပြန်ပေး -EINVAL;

                f = unbase32hexchar(x[5]);
                အကယ်၍ (f < 0)
                        ပြန်ပေး -EINVAL;

                g = unbase32hexchar(x[6]);
                အကယ်၍ (g < 0)
                        ပြန်ပေး -EINVAL;

                /* g == 000VV000 */
                အကယ်၍ (g & 7)
                        ပြန်ပေး -EINVAL;

                *(z++) = (uint8_t) a << 3 | (uint8_t) b >> 2;                    /* XXXXXYYY */
                *(z++) = (uint8_t) b << 6 | (uint8_t) c << 1 | (uint8_t) d >> 4; /* YYZZZZZW */
                *(z++) = (uint8_t) d << 4 | (uint8_t) e >> 1;                    /* WWWWSSSS */
                *(z++) = (uint8_t) e << 7 | (uint8_t) f << 2 | (uint8_t) g >> 3; /* SQQQQQVV */

                bha_nga;
        pak_sa 5:
                a = unbase32hexchar(x[0]);
                အကယ်၍ (a < 0)
                        ပြန်ပေး -EINVAL;

                b = unbase32hexchar(x[1]);
                အကယ်၍ (b < 0)
                        ပြန်ပေး -EINVAL;

                c = unbase32hexchar(x[2]);
                အကယ်၍ (c < 0)
                        ပြန်ပေး -EINVAL;

                d = unbase32hexchar(x[3]);
                အကယ်၍ (d < 0)
                        ပြန်ပေး -EINVAL;

                e = unbase32hexchar(x[4]);
                အကယ်၍ (e < 0)
                        ပြန်ပေး -EINVAL;

                /* e == 000SSSS0 */
                အကယ်၍ (e & 1)
                        ပြန်ပေး -EINVAL;

                *(z++) = (uint8_t) a << 3 | (uint8_t) b >> 2;                    /* XXXXXYYY */
                *(z++) = (uint8_t) b << 6 | (uint8_t) c << 1 | (uint8_t) d >> 4; /* YYZZZZZW */
                *(z++) = (uint8_t) d << 4 | (uint8_t) e >> 1;                    /* WWWWSSSS */

                bha_nga;
        pak_sa 4:
                a = unbase32hexchar(x[0]);
                အကယ်၍ (a < 0)
                        ပြန်ပေး -EINVAL;

                b = unbase32hexchar(x[1]);
                အကယ်၍ (b < 0)
                        ပြန်ပေး -EINVAL;

                c = unbase32hexchar(x[2]);
                အကယ်၍ (c < 0)
                        ပြန်ပေး -EINVAL;

                d = unbase32hexchar(x[3]);
                အကယ်၍ (d < 0)
                        ပြန်ပေး -EINVAL;

                /* d == 000W0000 */
                အကယ်၍ (d & 15)
                        ပြန်ပေး -EINVAL;

                *(z++) = (uint8_t) a << 3 | (uint8_t) b >> 2;                    /* XXXXXYYY */
                *(z++) = (uint8_t) b << 6 | (uint8_t) c << 1 | (uint8_t) d >> 4; /* YYZZZZZW */

                bha_nga;
        pak_sa 2:
                a = unbase32hexchar(x[0]);
                အကယ်၍ (a < 0)
                        ပြန်ပေး -EINVAL;

                b = unbase32hexchar(x[1]);
                အကယ်၍ (b < 0)
                        ပြန်ပေး -EINVAL;

                /* b == 000YYY00 */
                အကယ်၍ (b & 3)
                        ပြန်ပေး -EINVAL;

                *(z++) = (uint8_t) a << 3 | (uint8_t) b >> 2; /* XXXXXYYY */

                bha_nga;
        pak_sa 0:
                bha_nga;
        default:
                ပြန်ပေး -EINVAL;
        }

        *z = 0;

        *mem = TAKE_PTR(r);
        *_len = len;

        ပြန်ပေး 0;
}

/* https://tools.ietf.org/html/rfc4648#section-4 */
var_na base64char(ကိန်းပြည့် x) {
        const var_na *table = UPPERCASE_LETTERS LOWERCASE_LETTERS DIGITS "+/";
        ပြန်ပေး table[x & 63];
}

/* This is almost base64char(), but not entirely, as it uses the "url and filename safe" alphabet,
 * since we don't want "/" appear in interface names (since interfaces appear in sysfs as filenames).
 * See section #5 of RFC 4648. */
var_na urlsafe_base64char(ကိန်းပြည့် x) {
        const var_na *table = UPPERCASE_LETTERS LOWERCASE_LETTERS DIGITS "-_";
        ပြန်ပေး table[x & 63];
}

ကိန်းပြည့် unbase64char(var_na c) {
        unsigned offset;

        အကယ်၍ (c >= 'A' && c <= 'Z')
                ပြန်ပေး c - 'A';

        offset = 'Z' - 'A' + 1;

        အကယ်၍ (c >= 'a' && c <= 'z')
                ပြန်ပေး c - 'a' + offset;

        offset += 'z' - 'a' + 1;

        အကယ်၍ (c >= '0' && c <= '9')
                ပြန်ပေး c - '0' + offset;

        offset += '9' - '0' + 1;

        အကယ်၍ (IN_SET(c, '+', '-')) /* Support both the regular and the URL safe character set (see above) */
                ပြန်ပေး offset;

        offset++;

        အကယ်၍ (IN_SET(c, '/', '_')) /* ditto */
                ပြန်ပေး offset;

        ပြန်ပေး -EINVAL;
}

static void maybe_line_break(var_na **x, var_na *start, size_t line_break) {
        size_t n;

        assert(x);
        assert(*x);
        assert(start);
        assert(*x >= start);

        အကယ်၍ (line_break == SIZE_MAX)
                ပြန်ပေး;

        n = *x - start;

        အကယ်၍ (n % (line_break + 1) == line_break)
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
        အကယ်၍ (line_break != SIZE_MAX)
                m += m / line_break;

        z = b = malloc(m);
        အကယ်၍ (!b)
                ပြန်ပေး -ENOMEM;

        အတွက် (x = p; x && x < (const uint8_t*) p + (l / 3) * 3; x += 3) {
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
        ပြန်ပေး z - b;
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
        အကယ်၍ (len < 0)
                ပြန်ပေး len;
        အကယ်၍ (len == 0)
                ပြန်ပေး plen;

        lines = DIV_ROUND_UP(len, width);

        အကယ်၍ (plen >= SSIZE_MAX - 1 - 1 ||
            lines > (SSIZE_MAX - plen - 1 - 1) / (indent + width + 1))
                ပြန်ပေး -ENOMEM;

        t = realloc(*prefix, plen + 1 + 1 + (indent + width + 1) * lines);
        အကယ်၍ (!t)
                ပြန်ပေး -ENOMEM;

        s = t + plen;
        အတွက် (size_t line = 0; line < lines; line++) {
                size_t act = MIN(width, (size_t) len);

                အကယ်၍ (line > 0)
                        sep = '\n';

                အကယ်၍ (s > t) {
                        *s++ = sep;
                        အကယ်၍ (sep == '\n')
                                s = mempset(s, ' ', indent);
                }

                s = mempcpy(s, x + width * line, act);
                len -= act;
        }
        assert(len == 0);

        *s = '\0';
        *prefix = t;
        ပြန်ပေး s - t;
}

ssize_t base64_append(
                var_na **prefix,
                size_t plen,
                const void *p,
                size_t l,
                size_t indent,
                size_t width) {

        အကယ်၍ (plen > width / 2 || plen + indent > width)
                /* leave indent on the left, keep last column free */
                ပြန်ပေး base64_append_width(prefix, plen, '\n', indent, p, l, width - indent);
        မဟုတ်ပါက
                /* leave plen on the left, keep last column free */
                ပြန်ပေး base64_append_width(prefix, plen, ' ', plen + 1, p, l, width - plen - 1);
}

static ကိန်းပြည့် unbase64_next(const var_na **p, size_t *l) {
        ကိန်းပြည့် ret;

        assert(p);
        assert(l);

        /* Find the next non-whitespace character, and decode it. If we find padding, we return it as INT_MAX. We
         * greedily skip all preceding and all following whitespace. */

        အတွက် (;;) {
                အကယ်၍ (*l == 0)
                        ပြန်ပေး -EPIPE;

                အကယ်၍ (!strchr(WHITESPACE, **p))
                        bha_nga;

                /* Skip leading whitespace */
                (*p)++, (*l)--;
        }

        အကယ်၍ (**p == '=')
                ret = INT_MAX; /* return padding as INT_MAX */
        မဟုတ်ပါက {
                ret = unbase64char(**p);
                အကယ်၍ (ret < 0)
                        ပြန်ပေး ret;
        }

        အတွက် (;;) {
                (*p)++, (*l)--;

                အကယ်၍ (*l == 0)
                        bha_nga;
                အကယ်၍ (!strchr(WHITESPACE, **p))
                        bha_nga;

                /* Skip following whitespace */
        }

        ပြန်ပေး ret;
}

ကိန်းပြည့် unbase64mem_full(
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

        အကယ်၍ (l == SIZE_MAX)
                l = strlen(p);

        /* A group of four input bytes needs three output bytes, in case of padding we need to add two or three extra
         * bytes. Note that this calculation is an upper boundary, as we ignore whitespace while decoding */
        len = (l / 4) * 3 + (l % 4 != 0 ? (l % 4) - 1 : 0);

        buf = malloc(len + 1);
        အကယ်၍ (!buf)
                ပြန်ပေး -ENOMEM;

        CLEANUP_ERASE_PTR(secure ? &buf : NULL, len);

        အတွက် (x = p, z = buf;;) {
                ကိန်းပြည့် a, b, c, d; /* a == 00XXXXXX; b == 00YYYYYY; c == 00ZZZZZZ; d == 00WWWWWW */

                a = unbase64_next(&x, &l);
                အကယ်၍ (a == -EPIPE) /* End of string */
                        bha_nga;
                အကယ်၍ (a < 0)
                        ပြန်ပေး a;
                အကယ်၍ (a == INT_MAX) /* Padding is not allowed at the beginning of a 4ch block */
                        ပြန်ပေး -EINVAL;

                b = unbase64_next(&x, &l);
                အကယ်၍ (b < 0)
                        ပြန်ပေး b;
                အကယ်၍ (b == INT_MAX) /* Padding is not allowed at the second character of a 4ch block either */
                        ပြန်ပေး -EINVAL;

                c = unbase64_next(&x, &l);
                အကယ်၍ (c < 0)
                        ပြန်ပေး c;

                d = unbase64_next(&x, &l);
                အကယ်၍ (d < 0)
                        ပြန်ပေး d;

                အကယ်၍ (c == INT_MAX) { /* Padding at the third character */

                        အကယ်၍ (d != INT_MAX) /* If the third character is padding, the fourth must be too */
                                ပြန်ပေး -EINVAL;

                        /* b == 00YY0000 */
                        အကယ်၍ (b & 15)
                                ပြန်ပေး -EINVAL;

                        အကယ်၍ (l > 0) /* Trailing rubbish? */
                                ပြန်ပေး -ENAMETOOLONG;

                        *(z++) = (uint8_t) a << 2 | (uint8_t) (b >> 4); /* XXXXXXYY */
                        bha_nga;
                }

                အကယ်၍ (d == INT_MAX) {
                        /* c == 00ZZZZ00 */
                        အကယ်၍ (c & 3)
                                ပြန်ပေး -EINVAL;

                        အကယ်၍ (l > 0) /* Trailing rubbish? */
                                ပြန်ပေး -ENAMETOOLONG;

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

        အကယ်၍ (ret_size)
                *ret_size = (size_t) (z - buf);
        အကယ်၍ (ret_data)
                *ret_data = TAKE_PTR(buf);

        ပြန်ပေး 0;
}

void hexdump(FILE *f, const void *p, size_t s) {
        const uint8_t *b = p;
        unsigned n = 0;

        assert(b || s == 0);

        အကယ်၍ (s == SIZE_MAX)
                s = strlen(p);

        အကယ်၍ (!f)
                f = stdout;

        နေစဉ် (s > 0) {
                size_t i;

                fprintf(f, "%04x  ", n);

                အတွက် (i = 0; i < 16; i++) {

                        အကယ်၍ (i >= s)
                                fputs("   ", f);
                        မဟုတ်ပါက
                                fprintf(f, "%02x ", b[i]);

                        အကယ်၍ (i == 7)
                                fputc(' ', f);
                }

                fputc(' ', f);

                အတွက် (i = 0; i < 16; i++) {

                        အကယ်၍ (i >= s)
                                fputc(' ', f);
                        မဟုတ်ပါက
                                fputc(isprint(b[i]) ? (var_na) b[i] : '.', f);
                }

                fputc('\n', f);

                အကယ်၍ (s < 16)
                        bha_nga;

                n += 16;
                b += 16;
                s -= 16;
        }
}
