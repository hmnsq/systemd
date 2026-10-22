/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <ctype.h>
#include <stdio.h>

#include "alloc-util.h"
#include "hexdecoct.h"
#include "memory-util.h"
#include "string-util.h"

char octchar(цео x) {
        врати '0' + (x & 7);
}

цео unoctchar(char c) {

        ако (c >= '0' && c <= '7')
                врати c - '0';

        врати -EINVAL;
}

char decchar(цео x) {
        врати '0' + (x % 10);
}

цео undecchar(char c) {

        ако (c >= '0' && c <= '9')
                врати c - '0';

        врати -EINVAL;
}

char hexchar(цео x) {
        const char *table = LOWERCASE_HEXDIGITS;
        врати table[x & 15];
}

цео unhexchar(char c) {

        ако (c >= '0' && c <= '9')
                врати c - '0';

        ако (c >= 'a' && c <= 'f')
                врати c - 'a' + 10;

        ако (c >= 'A' && c <= 'F')
                врати c - 'A' + 10;

        врати -EINVAL;
}

char* hexmem(const void *p, size_t l) {
        const uint8_t *x;
        char *r, *z;

        assert(p || l == 0);

        z = r = new(char, l * 2 + 1);
        ако (!r)
                врати NULL;

        за (x = p; x && x < (const uint8_t*) p + l; x++) {
                *(z++) = hexchar(*x >> 4);
                *(z++) = hexchar(*x & 15);
        }

        *z = 0;
        врати r;
}

static цео unhex_next(const char **p, size_t *l) {
        цео r;

        assert(p);
        assert(l);

        /* Find the next non-whitespace character, and decode it. We
         * greedily skip all preceding and all following whitespace. */

        за (;;) {
                ако (*l == 0)
                        врати -EPIPE;

                ако (!strchr(WHITESPACE, **p))
                        break;

                /* Skip leading whitespace */
                (*p)++, (*l)--;
        }

        r = unhexchar(**p);
        ако (r < 0)
                врати r;

        за (;;) {
                (*p)++, (*l)--;

                ако (*l == 0 || !strchr(WHITESPACE, **p))
                        break;

                /* Skip following whitespace */
        }

        врати r;
}

цео unhexmem_full(
                const char *p,
                size_t l,
                bool secure,
                void **ret_data,
                size_t *ret_size) {

        _cleanup_free_ uint8_t *buf = NULL;
        size_t buf_size;
        const char *x;
        uint8_t *z;

        assert(p || l == 0);

        ако (l == SIZE_MAX)
                l = strlen(p);

        /* Note that the calculation of memory size is an upper boundary, as we ignore whitespace while decoding */
        buf_size = (l + 1) / 2 + 1;
        buf = malloc(buf_size);
        ако (!buf)
                врати -ENOMEM;

        CLEANUP_ERASE_PTR(secure ? &buf : NULL, buf_size);

        за (x = p, z = buf;;) {
                цео a, b;

                a = unhex_next(&x, &l);
                ако (a == -EPIPE) /* End of string */
                        break;
                ако (a < 0)
                        врати a;

                b = unhex_next(&x, &l);
                ако (b < 0)
                        врати b;

                *(z++) = (uint8_t) a << 4 | (uint8_t) b;
        }

        *z = 0;

        ако (ret_size)
                *ret_size = (size_t) (z - buf);
        ако (ret_data)
                *ret_data = TAKE_PTR(buf);

        врати 0;
}

/* https://tools.ietf.org/html/rfc4648#section-6
 * Notice that base32hex differs from base32 in the alphabet it uses.
 * The distinction is that the base32hex representation preserves the
 * order of the underlying data when compared as bytestrings, this is
 * useful when representing NSEC3 hashes, as one can then verify the
 * order of hashes directly from their representation. */
char base32hexchar(цео x) {
        const char *table = DIGITS "ABCDEFGHIJKLMNOPQRSTUV";
        врати table[x & 31];
}

цео unbase32hexchar(char c) {
        unsigned offset;

        ако (c >= '0' && c <= '9')
                врати c - '0';

        offset = '9' - '0' + 1;

        ако (c >= 'A' && c <= 'V')
                врати c - 'A' + offset;

        врати -EINVAL;
}

char* base32hexmem(const void *p, size_t l, bool padding) {
        char *r, *z;
        const uint8_t *x;
        size_t len;

        assert(p || l == 0);

        ако (padding)
                /* five input bytes makes eight output bytes, padding is added so we must round up */
                len = 8 * (l + 4) / 5;
        инако {
                /* same, but round down as there is no padding */
                len = 8 * l / 5;

                switch (l % 5) {
                case 4:
                        len += 7;
                        break;
                case 3:
                        len += 5;
                        break;
                case 2:
                        len += 4;
                        break;
                case 1:
                        len += 2;
                        break;
                }
        }

        z = r = malloc(len + 1);
        ако (!r)
                врати NULL;

        за (x = p; x < (const uint8_t*) p + (l / 5) * 5; x += 5) {
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
        case 4:
                *(z++) = base32hexchar(x[0] >> 3);                    /* 000XXXXX */
                *(z++) = base32hexchar((x[0] & 7) << 2 | x[1] >> 6);  /* 000XXXYY */
                *(z++) = base32hexchar((x[1] & 63) >> 1);             /* 000YYYYY */
                *(z++) = base32hexchar((x[1] & 1) << 4 | x[2] >> 4);   /* 000YZZZZ */
                *(z++) = base32hexchar((x[2] & 15) << 1 | x[3] >> 7); /* 000ZZZZQ */
                *(z++) = base32hexchar((x[3] & 127) >> 2);            /* 000QQQQQ */
                *(z++) = base32hexchar((x[3] & 3) << 3);              /* 000QQ000 */
                ако (padding)
                        *(z++) = '=';

                break;

        case 3:
                *(z++) = base32hexchar(x[0] >> 3);                   /* 000XXXXX */
                *(z++) = base32hexchar((x[0] & 7) << 2 | x[1] >> 6); /* 000XXXYY */
                *(z++) = base32hexchar((x[1] & 63) >> 1);            /* 000YYYYY */
                *(z++) = base32hexchar((x[1] & 1) << 4 | x[2] >> 4); /* 000YZZZZ */
                *(z++) = base32hexchar((x[2] & 15) << 1);            /* 000ZZZZ0 */
                ако (padding) {
                        *(z++) = '=';
                        *(z++) = '=';
                        *(z++) = '=';
                }

                break;

        case 2:
                *(z++) = base32hexchar(x[0] >> 3);                   /* 000XXXXX */
                *(z++) = base32hexchar((x[0] & 7) << 2 | x[1] >> 6); /* 000XXXYY */
                *(z++) = base32hexchar((x[1] & 63) >> 1);            /* 000YYYYY */
                *(z++) = base32hexchar((x[1] & 1) << 4);             /* 000Y0000 */
                ако (padding) {
                        *(z++) = '=';
                        *(z++) = '=';
                        *(z++) = '=';
                        *(z++) = '=';
                }

                break;

        case 1:
                *(z++) = base32hexchar(x[0] >> 3);       /* 000XXXXX */
                *(z++) = base32hexchar((x[0] & 7) << 2); /* 000XXX00 */
                ако (padding) {
                        *(z++) = '=';
                        *(z++) = '=';
                        *(z++) = '=';
                        *(z++) = '=';
                        *(z++) = '=';
                        *(z++) = '=';
                }

                break;
        }

        *z = 0;
        врати r;
}

цео unbase32hexmem(const char *p, size_t l, bool padding, void **mem, size_t *_len) {
        _cleanup_free_ uint8_t *r = NULL;
        цео a, b, c, d, e, f, g, h;
        uint8_t *z;
        const char *x;
        size_t len;
        unsigned pad = 0;

        assert(p || l == 0);
        assert(mem);
        assert(_len);

        ако (l == SIZE_MAX)
                l = strlen(p);

        /* padding ensures any base32hex input has input divisible by 8 */
        ако (padding && l % 8 != 0)
                врати -EINVAL;

        ако (padding) {
                /* strip the padding */
                док (l > 0 && p[l - 1] == '=' && pad < 7) {
                        pad++;
                        l--;
                }
        }

        /* a group of eight input bytes needs five output bytes, in case of
         * padding we need to add some extra bytes */
        len = (l / 8) * 5;

        switch (l % 8) {
        case 7:
                len += 4;
                break;
        case 5:
                len += 3;
                break;
        case 4:
                len += 2;
                break;
        case 2:
                len += 1;
                break;
        case 0:
                break;
        default:
                врати -EINVAL;
        }

        z = r = malloc(len + 1);
        ако (!r)
                врати -ENOMEM;

        за (x = p; x < p + (l / 8) * 8; x += 8) {
                /* a == 000XXXXX; b == 000YYYYY; c == 000ZZZZZ; d == 000WWWWW
                 * e == 000SSSSS; f == 000QQQQQ; g == 000VVVVV; h == 000RRRRR */
                a = unbase32hexchar(x[0]);
                ако (a < 0)
                        врати -EINVAL;

                b = unbase32hexchar(x[1]);
                ако (b < 0)
                        врати -EINVAL;

                c = unbase32hexchar(x[2]);
                ако (c < 0)
                        врати -EINVAL;

                d = unbase32hexchar(x[3]);
                ако (d < 0)
                        врати -EINVAL;

                e = unbase32hexchar(x[4]);
                ако (e < 0)
                        врати -EINVAL;

                f = unbase32hexchar(x[5]);
                ако (f < 0)
                        врати -EINVAL;

                g = unbase32hexchar(x[6]);
                ако (g < 0)
                        врати -EINVAL;

                h = unbase32hexchar(x[7]);
                ако (h < 0)
                        врати -EINVAL;

                *(z++) = (uint8_t) a << 3 | (uint8_t) b >> 2;                    /* XXXXXYYY */
                *(z++) = (uint8_t) b << 6 | (uint8_t) c << 1 | (uint8_t) d >> 4; /* YYZZZZZW */
                *(z++) = (uint8_t) d << 4 | (uint8_t) e >> 1;                    /* WWWWSSSS */
                *(z++) = (uint8_t) e << 7 | (uint8_t) f << 2 | (uint8_t) g >> 3; /* SQQQQQVV */
                *(z++) = (uint8_t) g << 5 | (uint8_t) h;                         /* VVVRRRRR */
        }

        switch (l % 8) {
        case 7:
                a = unbase32hexchar(x[0]);
                ако (a < 0)
                        врати -EINVAL;

                b = unbase32hexchar(x[1]);
                ако (b < 0)
                        врати -EINVAL;

                c = unbase32hexchar(x[2]);
                ако (c < 0)
                        врати -EINVAL;

                d = unbase32hexchar(x[3]);
                ако (d < 0)
                        врати -EINVAL;

                e = unbase32hexchar(x[4]);
                ако (e < 0)
                        врати -EINVAL;

                f = unbase32hexchar(x[5]);
                ако (f < 0)
                        врати -EINVAL;

                g = unbase32hexchar(x[6]);
                ако (g < 0)
                        врати -EINVAL;

                /* g == 000VV000 */
                ако (g & 7)
                        врати -EINVAL;

                *(z++) = (uint8_t) a << 3 | (uint8_t) b >> 2;                    /* XXXXXYYY */
                *(z++) = (uint8_t) b << 6 | (uint8_t) c << 1 | (uint8_t) d >> 4; /* YYZZZZZW */
                *(z++) = (uint8_t) d << 4 | (uint8_t) e >> 1;                    /* WWWWSSSS */
                *(z++) = (uint8_t) e << 7 | (uint8_t) f << 2 | (uint8_t) g >> 3; /* SQQQQQVV */

                break;
        case 5:
                a = unbase32hexchar(x[0]);
                ако (a < 0)
                        врати -EINVAL;

                b = unbase32hexchar(x[1]);
                ако (b < 0)
                        врати -EINVAL;

                c = unbase32hexchar(x[2]);
                ако (c < 0)
                        врати -EINVAL;

                d = unbase32hexchar(x[3]);
                ако (d < 0)
                        врати -EINVAL;

                e = unbase32hexchar(x[4]);
                ако (e < 0)
                        врати -EINVAL;

                /* e == 000SSSS0 */
                ако (e & 1)
                        врати -EINVAL;

                *(z++) = (uint8_t) a << 3 | (uint8_t) b >> 2;                    /* XXXXXYYY */
                *(z++) = (uint8_t) b << 6 | (uint8_t) c << 1 | (uint8_t) d >> 4; /* YYZZZZZW */
                *(z++) = (uint8_t) d << 4 | (uint8_t) e >> 1;                    /* WWWWSSSS */

                break;
        case 4:
                a = unbase32hexchar(x[0]);
                ако (a < 0)
                        врати -EINVAL;

                b = unbase32hexchar(x[1]);
                ако (b < 0)
                        врати -EINVAL;

                c = unbase32hexchar(x[2]);
                ако (c < 0)
                        врати -EINVAL;

                d = unbase32hexchar(x[3]);
                ако (d < 0)
                        врати -EINVAL;

                /* d == 000W0000 */
                ако (d & 15)
                        врати -EINVAL;

                *(z++) = (uint8_t) a << 3 | (uint8_t) b >> 2;                    /* XXXXXYYY */
                *(z++) = (uint8_t) b << 6 | (uint8_t) c << 1 | (uint8_t) d >> 4; /* YYZZZZZW */

                break;
        case 2:
                a = unbase32hexchar(x[0]);
                ако (a < 0)
                        врати -EINVAL;

                b = unbase32hexchar(x[1]);
                ако (b < 0)
                        врати -EINVAL;

                /* b == 000YYY00 */
                ако (b & 3)
                        врати -EINVAL;

                *(z++) = (uint8_t) a << 3 | (uint8_t) b >> 2; /* XXXXXYYY */

                break;
        case 0:
                break;
        default:
                врати -EINVAL;
        }

        *z = 0;

        *mem = TAKE_PTR(r);
        *_len = len;

        врати 0;
}

/* https://tools.ietf.org/html/rfc4648#section-4 */
char base64char(цео x) {
        const char *table = UPPERCASE_LETTERS LOWERCASE_LETTERS DIGITS "+/";
        врати table[x & 63];
}

/* This is almost base64char(), but not entirely, as it uses the "url and filename safe" alphabet,
 * since we don't want "/" appear in interface names (since interfaces appear in sysfs as filenames).
 * See section #5 of RFC 4648. */
char urlsafe_base64char(цео x) {
        const char *table = UPPERCASE_LETTERS LOWERCASE_LETTERS DIGITS "-_";
        врати table[x & 63];
}

цео unbase64char(char c) {
        unsigned offset;

        ако (c >= 'A' && c <= 'Z')
                врати c - 'A';

        offset = 'Z' - 'A' + 1;

        ако (c >= 'a' && c <= 'z')
                врати c - 'a' + offset;

        offset += 'z' - 'a' + 1;

        ако (c >= '0' && c <= '9')
                врати c - '0' + offset;

        offset += '9' - '0' + 1;

        ако (IN_SET(c, '+', '-')) /* Support both the regular and the URL safe character set (see above) */
                врати offset;

        offset++;

        ако (IN_SET(c, '/', '_')) /* ditto */
                врати offset;

        врати -EINVAL;
}

static void maybe_line_break(char **x, char *start, size_t line_break) {
        size_t n;

        assert(x);
        assert(*x);
        assert(start);
        assert(*x >= start);

        ако (line_break == SIZE_MAX)
                врати;

        n = *x - start;

        ако (n % (line_break + 1) == line_break)
                *((*x)++) = '\n';
}

ssize_t base64mem_full(
                const void *p,
                size_t l,
                size_t line_break,
                char **ret) {

        const uint8_t *x;
        char *b, *z;
        size_t m;

        assert(p || l == 0);
        assert(line_break > 0);
        assert(ret);

        /* three input bytes makes four output bytes, padding is added so we must round up */
        m = 4 * (l + 2) / 3 + 1;
        ако (line_break != SIZE_MAX)
                m += m / line_break;

        z = b = malloc(m);
        ако (!b)
                врати -ENOMEM;

        за (x = p; x && x < (const uint8_t*) p + (l / 3) * 3; x += 3) {
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
        case 2:
                maybe_line_break(&z, b, line_break);
                *(z++) = base64char(x[0] >> 2);                   /* 00XXXXXX */
                maybe_line_break(&z, b, line_break);
                *(z++) = base64char((x[0] & 3) << 4 | x[1] >> 4); /* 00XXYYYY */
                maybe_line_break(&z, b, line_break);
                *(z++) = base64char((x[1] & 15) << 2);            /* 00YYYY00 */
                maybe_line_break(&z, b, line_break);
                *(z++) = '=';
                break;

        case 1:
                maybe_line_break(&z, b, line_break);
                *(z++) = base64char(x[0] >> 2);        /* 00XXXXXX */
                maybe_line_break(&z, b, line_break);
                *(z++) = base64char((x[0] & 3) << 4);  /* 00XX0000 */
                maybe_line_break(&z, b, line_break);
                *(z++) = '=';
                maybe_line_break(&z, b, line_break);
                *(z++) = '=';
                break;
        }

        *z = 0;
        *ret = b;

        assert(z >= b); /* Let static analyzers know that the answer is non-negative. */
        врати z - b;
}

static ssize_t base64_append_width(
                char **prefix,
                size_t plen,
                char sep,
                size_t indent,
                const void *p,
                size_t l,
                size_t width) {

        _cleanup_free_ char *x = NULL;
        char *t, *s;
        size_t lines;
        ssize_t len;

        assert(prefix);
        assert(*prefix || plen == 0);
        assert(p || l == 0);

        len = base64mem(p, l, &x);
        ако (len < 0)
                врати len;
        ако (len == 0)
                врати plen;

        lines = DIV_ROUND_UP(len, width);

        ако (plen >= SSIZE_MAX - 1 - 1 ||
            lines > (SSIZE_MAX - plen - 1 - 1) / (indent + width + 1))
                врати -ENOMEM;

        t = realloc(*prefix, plen + 1 + 1 + (indent + width + 1) * lines);
        ако (!t)
                врати -ENOMEM;

        s = t + plen;
        за (size_t line = 0; line < lines; line++) {
                size_t act = MIN(width, (size_t) len);

                ако (line > 0)
                        sep = '\n';

                ако (s > t) {
                        *s++ = sep;
                        ако (sep == '\n')
                                s = mempset(s, ' ', indent);
                }

                s = mempcpy(s, x + width * line, act);
                len -= act;
        }
        assert(len == 0);

        *s = '\0';
        *prefix = t;
        врати s - t;
}

ssize_t base64_append(
                char **prefix,
                size_t plen,
                const void *p,
                size_t l,
                size_t indent,
                size_t width) {

        ако (plen > width / 2 || plen + indent > width)
                /* leave indent on the left, keep last column free */
                врати base64_append_width(prefix, plen, '\n', indent, p, l, width - indent);
        инако
                /* leave plen on the left, keep last column free */
                врати base64_append_width(prefix, plen, ' ', plen + 1, p, l, width - plen - 1);
}

static цео unbase64_next(const char **p, size_t *l) {
        цео ret;

        assert(p);
        assert(l);

        /* Find the next non-whitespace character, and decode it. If we find padding, we return it as INT_MAX. We
         * greedily skip all preceding and all following whitespace. */

        за (;;) {
                ако (*l == 0)
                        врати -EPIPE;

                ако (!strchr(WHITESPACE, **p))
                        break;

                /* Skip leading whitespace */
                (*p)++, (*l)--;
        }

        ако (**p == '=')
                ret = INT_MAX; /* return padding as INT_MAX */
        инако {
                ret = unbase64char(**p);
                ако (ret < 0)
                        врати ret;
        }

        за (;;) {
                (*p)++, (*l)--;

                ако (*l == 0)
                        break;
                ако (!strchr(WHITESPACE, **p))
                        break;

                /* Skip following whitespace */
        }

        врати ret;
}

цео unbase64mem_full(
                const char *p,
                size_t l,
                bool secure,
                void **ret_data,
                size_t *ret_size) {

        _cleanup_free_ uint8_t *buf = NULL;
        const char *x;
        uint8_t *z;
        size_t len;

        assert(p || l == 0);

        ако (l == SIZE_MAX)
                l = strlen(p);

        /* A group of four input bytes needs three output bytes, in case of padding we need to add two or three extra
         * bytes. Note that this calculation is an upper boundary, as we ignore whitespace while decoding */
        len = (l / 4) * 3 + (l % 4 != 0 ? (l % 4) - 1 : 0);

        buf = malloc(len + 1);
        ако (!buf)
                врати -ENOMEM;

        CLEANUP_ERASE_PTR(secure ? &buf : NULL, len);

        за (x = p, z = buf;;) {
                цео a, b, c, d; /* a == 00XXXXXX; b == 00YYYYYY; c == 00ZZZZZZ; d == 00WWWWWW */

                a = unbase64_next(&x, &l);
                ако (a == -EPIPE) /* End of string */
                        break;
                ако (a < 0)
                        врати a;
                ако (a == INT_MAX) /* Padding is not allowed at the beginning of a 4ch block */
                        врати -EINVAL;

                b = unbase64_next(&x, &l);
                ако (b < 0)
                        врати b;
                ако (b == INT_MAX) /* Padding is not allowed at the second character of a 4ch block either */
                        врати -EINVAL;

                c = unbase64_next(&x, &l);
                ако (c < 0)
                        врати c;

                d = unbase64_next(&x, &l);
                ако (d < 0)
                        врати d;

                ако (c == INT_MAX) { /* Padding at the third character */

                        ако (d != INT_MAX) /* If the third character is padding, the fourth must be too */
                                врати -EINVAL;

                        /* b == 00YY0000 */
                        ако (b & 15)
                                врати -EINVAL;

                        ако (l > 0) /* Trailing rubbish? */
                                врати -ENAMETOOLONG;

                        *(z++) = (uint8_t) a << 2 | (uint8_t) (b >> 4); /* XXXXXXYY */
                        break;
                }

                ако (d == INT_MAX) {
                        /* c == 00ZZZZ00 */
                        ако (c & 3)
                                врати -EINVAL;

                        ако (l > 0) /* Trailing rubbish? */
                                врати -ENAMETOOLONG;

                        *(z++) = (uint8_t) a << 2 | (uint8_t) b >> 4; /* XXXXXXYY */
                        *(z++) = (uint8_t) b << 4 | (uint8_t) c >> 2; /* YYYYZZZZ */
                        break;
                }

                *(z++) = (uint8_t) a << 2 | (uint8_t) b >> 4; /* XXXXXXYY */
                *(z++) = (uint8_t) b << 4 | (uint8_t) c >> 2; /* YYYYZZZZ */
                *(z++) = (uint8_t) c << 6 | (uint8_t) d;      /* ZZWWWWWW */
        }

        *z = 0;

        assert((size_t) (z - buf) <= len);

        ако (ret_size)
                *ret_size = (size_t) (z - buf);
        ако (ret_data)
                *ret_data = TAKE_PTR(buf);

        врати 0;
}

void hexdump(FILE *f, const void *p, size_t s) {
        const uint8_t *b = p;
        unsigned n = 0;

        assert(b || s == 0);

        ако (s == SIZE_MAX)
                s = strlen(p);

        ако (!f)
                f = stdout;

        док (s > 0) {
                size_t i;

                fprintf(f, "%04x  ", n);

                за (i = 0; i < 16; i++) {

                        ако (i >= s)
                                fputs("   ", f);
                        инако
                                fprintf(f, "%02x ", b[i]);

                        ако (i == 7)
                                fputc(' ', f);
                }

                fputc(' ', f);

                за (i = 0; i < 16; i++) {

                        ако (i >= s)
                                fputc(' ', f);
                        инако
                                fputc(isprint(b[i]) ? (char) b[i] : '.', f);
                }

                fputc('\n', f);

                ако (s < 16)
                        break;

                n += 16;
                b += 16;
                s -= 16;
        }
}
