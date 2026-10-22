/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <ctype.h>
#include <stdio.h>

#include "alloc-util.h"
#include "hexdecoct.h"
#include "memory-util.h"
#include "string-util.h"

char octchar(整数 x) {
        返回 '0' + (x & 7);
}

整数 unoctchar(char c) {

        如果 (c >= '0' && c <= '7')
                返回 c - '0';

        返回 -EINVAL;
}

char decchar(整数 x) {
        返回 '0' + (x % 10);
}

整数 undecchar(char c) {

        如果 (c >= '0' && c <= '9')
                返回 c - '0';

        返回 -EINVAL;
}

char hexchar(整数 x) {
        const char *table = LOWERCASE_HEXDIGITS;
        返回 table[x & 15];
}

整数 unhexchar(char c) {

        如果 (c >= '0' && c <= '9')
                返回 c - '0';

        如果 (c >= 'a' && c <= 'f')
                返回 c - 'a' + 10;

        如果 (c >= 'A' && c <= 'F')
                返回 c - 'A' + 10;

        返回 -EINVAL;
}

char* hexmem(const void *p, size_t l) {
        const uint8_t *x;
        char *r, *z;

        assert(p || l == 0);

        z = r = new(char, l * 2 + 1);
        如果 (!r)
                返回 NULL;

        为 (x = p; x && x < (const uint8_t*) p + l; x++) {
                *(z++) = hexchar(*x >> 4);
                *(z++) = hexchar(*x & 15);
        }

        *z = 0;
        返回 r;
}

static 整数 unhex_next(const char **p, size_t *l) {
        整数 r;

        assert(p);
        assert(l);

        /* Find the next non-whitespace character, and decode it. We
         * greedily skip all preceding and all following whitespace. */

        为 (;;) {
                如果 (*l == 0)
                        返回 -EPIPE;

                如果 (!strchr(WHITESPACE, **p))
                        break;

                /* Skip leading whitespace */
                (*p)++, (*l)--;
        }

        r = unhexchar(**p);
        如果 (r < 0)
                返回 r;

        为 (;;) {
                (*p)++, (*l)--;

                如果 (*l == 0 || !strchr(WHITESPACE, **p))
                        break;

                /* Skip following whitespace */
        }

        返回 r;
}

整数 unhexmem_full(
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

        如果 (l == SIZE_MAX)
                l = strlen(p);

        /* Note that the calculation of memory size is an upper boundary, as we ignore whitespace while decoding */
        buf_size = (l + 1) / 2 + 1;
        buf = malloc(buf_size);
        如果 (!buf)
                返回 -ENOMEM;

        CLEANUP_ERASE_PTR(secure ? &buf : NULL, buf_size);

        为 (x = p, z = buf;;) {
                整数 a, b;

                a = unhex_next(&x, &l);
                如果 (a == -EPIPE) /* End of string */
                        break;
                如果 (a < 0)
                        返回 a;

                b = unhex_next(&x, &l);
                如果 (b < 0)
                        返回 b;

                *(z++) = (uint8_t) a << 4 | (uint8_t) b;
        }

        *z = 0;

        如果 (ret_size)
                *ret_size = (size_t) (z - buf);
        如果 (ret_data)
                *ret_data = TAKE_PTR(buf);

        返回 0;
}

/* https://tools.ietf.org/html/rfc4648#section-6
 * Notice that base32hex differs from base32 in the alphabet it uses.
 * The distinction is that the base32hex representation preserves the
 * order of the underlying data when compared as bytestrings, this is
 * useful when representing NSEC3 hashes, as one can then verify the
 * order of hashes directly from their representation. */
char base32hexchar(整数 x) {
        const char *table = DIGITS "ABCDEFGHIJKLMNOPQRSTUV";
        返回 table[x & 31];
}

整数 unbase32hexchar(char c) {
        unsigned offset;

        如果 (c >= '0' && c <= '9')
                返回 c - '0';

        offset = '9' - '0' + 1;

        如果 (c >= 'A' && c <= 'V')
                返回 c - 'A' + offset;

        返回 -EINVAL;
}

char* base32hexmem(const void *p, size_t l, bool padding) {
        char *r, *z;
        const uint8_t *x;
        size_t len;

        assert(p || l == 0);

        如果 (padding)
                /* five input bytes makes eight output bytes, padding is added so we must round up */
                len = 8 * (l + 4) / 5;
        否则 {
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
        如果 (!r)
                返回 NULL;

        为 (x = p; x < (const uint8_t*) p + (l / 5) * 5; x += 5) {
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
                如果 (padding)
                        *(z++) = '=';

                break;

        case 3:
                *(z++) = base32hexchar(x[0] >> 3);                   /* 000XXXXX */
                *(z++) = base32hexchar((x[0] & 7) << 2 | x[1] >> 6); /* 000XXXYY */
                *(z++) = base32hexchar((x[1] & 63) >> 1);            /* 000YYYYY */
                *(z++) = base32hexchar((x[1] & 1) << 4 | x[2] >> 4); /* 000YZZZZ */
                *(z++) = base32hexchar((x[2] & 15) << 1);            /* 000ZZZZ0 */
                如果 (padding) {
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
                如果 (padding) {
                        *(z++) = '=';
                        *(z++) = '=';
                        *(z++) = '=';
                        *(z++) = '=';
                }

                break;

        case 1:
                *(z++) = base32hexchar(x[0] >> 3);       /* 000XXXXX */
                *(z++) = base32hexchar((x[0] & 7) << 2); /* 000XXX00 */
                如果 (padding) {
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
        返回 r;
}

整数 unbase32hexmem(const char *p, size_t l, bool padding, void **mem, size_t *_len) {
        _cleanup_free_ uint8_t *r = NULL;
        整数 a, b, c, d, e, f, g, h;
        uint8_t *z;
        const char *x;
        size_t len;
        unsigned pad = 0;

        assert(p || l == 0);
        assert(mem);
        assert(_len);

        如果 (l == SIZE_MAX)
                l = strlen(p);

        /* padding ensures any base32hex input has input divisible by 8 */
        如果 (padding && l % 8 != 0)
                返回 -EINVAL;

        如果 (padding) {
                /* strip the padding */
                当 (l > 0 && p[l - 1] == '=' && pad < 7) {
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
                返回 -EINVAL;
        }

        z = r = malloc(len + 1);
        如果 (!r)
                返回 -ENOMEM;

        为 (x = p; x < p + (l / 8) * 8; x += 8) {
                /* a == 000XXXXX; b == 000YYYYY; c == 000ZZZZZ; d == 000WWWWW
                 * e == 000SSSSS; f == 000QQQQQ; g == 000VVVVV; h == 000RRRRR */
                a = unbase32hexchar(x[0]);
                如果 (a < 0)
                        返回 -EINVAL;

                b = unbase32hexchar(x[1]);
                如果 (b < 0)
                        返回 -EINVAL;

                c = unbase32hexchar(x[2]);
                如果 (c < 0)
                        返回 -EINVAL;

                d = unbase32hexchar(x[3]);
                如果 (d < 0)
                        返回 -EINVAL;

                e = unbase32hexchar(x[4]);
                如果 (e < 0)
                        返回 -EINVAL;

                f = unbase32hexchar(x[5]);
                如果 (f < 0)
                        返回 -EINVAL;

                g = unbase32hexchar(x[6]);
                如果 (g < 0)
                        返回 -EINVAL;

                h = unbase32hexchar(x[7]);
                如果 (h < 0)
                        返回 -EINVAL;

                *(z++) = (uint8_t) a << 3 | (uint8_t) b >> 2;                    /* XXXXXYYY */
                *(z++) = (uint8_t) b << 6 | (uint8_t) c << 1 | (uint8_t) d >> 4; /* YYZZZZZW */
                *(z++) = (uint8_t) d << 4 | (uint8_t) e >> 1;                    /* WWWWSSSS */
                *(z++) = (uint8_t) e << 7 | (uint8_t) f << 2 | (uint8_t) g >> 3; /* SQQQQQVV */
                *(z++) = (uint8_t) g << 5 | (uint8_t) h;                         /* VVVRRRRR */
        }

        switch (l % 8) {
        case 7:
                a = unbase32hexchar(x[0]);
                如果 (a < 0)
                        返回 -EINVAL;

                b = unbase32hexchar(x[1]);
                如果 (b < 0)
                        返回 -EINVAL;

                c = unbase32hexchar(x[2]);
                如果 (c < 0)
                        返回 -EINVAL;

                d = unbase32hexchar(x[3]);
                如果 (d < 0)
                        返回 -EINVAL;

                e = unbase32hexchar(x[4]);
                如果 (e < 0)
                        返回 -EINVAL;

                f = unbase32hexchar(x[5]);
                如果 (f < 0)
                        返回 -EINVAL;

                g = unbase32hexchar(x[6]);
                如果 (g < 0)
                        返回 -EINVAL;

                /* g == 000VV000 */
                如果 (g & 7)
                        返回 -EINVAL;

                *(z++) = (uint8_t) a << 3 | (uint8_t) b >> 2;                    /* XXXXXYYY */
                *(z++) = (uint8_t) b << 6 | (uint8_t) c << 1 | (uint8_t) d >> 4; /* YYZZZZZW */
                *(z++) = (uint8_t) d << 4 | (uint8_t) e >> 1;                    /* WWWWSSSS */
                *(z++) = (uint8_t) e << 7 | (uint8_t) f << 2 | (uint8_t) g >> 3; /* SQQQQQVV */

                break;
        case 5:
                a = unbase32hexchar(x[0]);
                如果 (a < 0)
                        返回 -EINVAL;

                b = unbase32hexchar(x[1]);
                如果 (b < 0)
                        返回 -EINVAL;

                c = unbase32hexchar(x[2]);
                如果 (c < 0)
                        返回 -EINVAL;

                d = unbase32hexchar(x[3]);
                如果 (d < 0)
                        返回 -EINVAL;

                e = unbase32hexchar(x[4]);
                如果 (e < 0)
                        返回 -EINVAL;

                /* e == 000SSSS0 */
                如果 (e & 1)
                        返回 -EINVAL;

                *(z++) = (uint8_t) a << 3 | (uint8_t) b >> 2;                    /* XXXXXYYY */
                *(z++) = (uint8_t) b << 6 | (uint8_t) c << 1 | (uint8_t) d >> 4; /* YYZZZZZW */
                *(z++) = (uint8_t) d << 4 | (uint8_t) e >> 1;                    /* WWWWSSSS */

                break;
        case 4:
                a = unbase32hexchar(x[0]);
                如果 (a < 0)
                        返回 -EINVAL;

                b = unbase32hexchar(x[1]);
                如果 (b < 0)
                        返回 -EINVAL;

                c = unbase32hexchar(x[2]);
                如果 (c < 0)
                        返回 -EINVAL;

                d = unbase32hexchar(x[3]);
                如果 (d < 0)
                        返回 -EINVAL;

                /* d == 000W0000 */
                如果 (d & 15)
                        返回 -EINVAL;

                *(z++) = (uint8_t) a << 3 | (uint8_t) b >> 2;                    /* XXXXXYYY */
                *(z++) = (uint8_t) b << 6 | (uint8_t) c << 1 | (uint8_t) d >> 4; /* YYZZZZZW */

                break;
        case 2:
                a = unbase32hexchar(x[0]);
                如果 (a < 0)
                        返回 -EINVAL;

                b = unbase32hexchar(x[1]);
                如果 (b < 0)
                        返回 -EINVAL;

                /* b == 000YYY00 */
                如果 (b & 3)
                        返回 -EINVAL;

                *(z++) = (uint8_t) a << 3 | (uint8_t) b >> 2; /* XXXXXYYY */

                break;
        case 0:
                break;
        default:
                返回 -EINVAL;
        }

        *z = 0;

        *mem = TAKE_PTR(r);
        *_len = len;

        返回 0;
}

/* https://tools.ietf.org/html/rfc4648#section-4 */
char base64char(整数 x) {
        const char *table = UPPERCASE_LETTERS LOWERCASE_LETTERS DIGITS "+/";
        返回 table[x & 63];
}

/* This is almost base64char(), but not entirely, as it uses the "url and filename safe" alphabet,
 * since we don't want "/" appear in interface names (since interfaces appear in sysfs as filenames).
 * See section #5 of RFC 4648. */
char urlsafe_base64char(整数 x) {
        const char *table = UPPERCASE_LETTERS LOWERCASE_LETTERS DIGITS "-_";
        返回 table[x & 63];
}

整数 unbase64char(char c) {
        unsigned offset;

        如果 (c >= 'A' && c <= 'Z')
                返回 c - 'A';

        offset = 'Z' - 'A' + 1;

        如果 (c >= 'a' && c <= 'z')
                返回 c - 'a' + offset;

        offset += 'z' - 'a' + 1;

        如果 (c >= '0' && c <= '9')
                返回 c - '0' + offset;

        offset += '9' - '0' + 1;

        如果 (IN_SET(c, '+', '-')) /* Support both the regular and the URL safe character set (see above) */
                返回 offset;

        offset++;

        如果 (IN_SET(c, '/', '_')) /* ditto */
                返回 offset;

        返回 -EINVAL;
}

static void maybe_line_break(char **x, char *start, size_t line_break) {
        size_t n;

        assert(x);
        assert(*x);
        assert(start);
        assert(*x >= start);

        如果 (line_break == SIZE_MAX)
                返回;

        n = *x - start;

        如果 (n % (line_break + 1) == line_break)
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
        如果 (line_break != SIZE_MAX)
                m += m / line_break;

        z = b = malloc(m);
        如果 (!b)
                返回 -ENOMEM;

        为 (x = p; x && x < (const uint8_t*) p + (l / 3) * 3; x += 3) {
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
        返回 z - b;
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
        如果 (len < 0)
                返回 len;
        如果 (len == 0)
                返回 plen;

        lines = DIV_ROUND_UP(len, width);

        如果 (plen >= SSIZE_MAX - 1 - 1 ||
            lines > (SSIZE_MAX - plen - 1 - 1) / (indent + width + 1))
                返回 -ENOMEM;

        t = realloc(*prefix, plen + 1 + 1 + (indent + width + 1) * lines);
        如果 (!t)
                返回 -ENOMEM;

        s = t + plen;
        为 (size_t line = 0; line < lines; line++) {
                size_t act = MIN(width, (size_t) len);

                如果 (line > 0)
                        sep = '\n';

                如果 (s > t) {
                        *s++ = sep;
                        如果 (sep == '\n')
                                s = mempset(s, ' ', indent);
                }

                s = mempcpy(s, x + width * line, act);
                len -= act;
        }
        assert(len == 0);

        *s = '\0';
        *prefix = t;
        返回 s - t;
}

ssize_t base64_append(
                char **prefix,
                size_t plen,
                const void *p,
                size_t l,
                size_t indent,
                size_t width) {

        如果 (plen > width / 2 || plen + indent > width)
                /* leave indent on the left, keep last column free */
                返回 base64_append_width(prefix, plen, '\n', indent, p, l, width - indent);
        否则
                /* leave plen on the left, keep last column free */
                返回 base64_append_width(prefix, plen, ' ', plen + 1, p, l, width - plen - 1);
}

static 整数 unbase64_next(const char **p, size_t *l) {
        整数 ret;

        assert(p);
        assert(l);

        /* Find the next non-whitespace character, and decode it. If we find padding, we return it as INT_MAX. We
         * greedily skip all preceding and all following whitespace. */

        为 (;;) {
                如果 (*l == 0)
                        返回 -EPIPE;

                如果 (!strchr(WHITESPACE, **p))
                        break;

                /* Skip leading whitespace */
                (*p)++, (*l)--;
        }

        如果 (**p == '=')
                ret = INT_MAX; /* return padding as INT_MAX */
        否则 {
                ret = unbase64char(**p);
                如果 (ret < 0)
                        返回 ret;
        }

        为 (;;) {
                (*p)++, (*l)--;

                如果 (*l == 0)
                        break;
                如果 (!strchr(WHITESPACE, **p))
                        break;

                /* Skip following whitespace */
        }

        返回 ret;
}

整数 unbase64mem_full(
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

        如果 (l == SIZE_MAX)
                l = strlen(p);

        /* A group of four input bytes needs three output bytes, in case of padding we need to add two or three extra
         * bytes. Note that this calculation is an upper boundary, as we ignore whitespace while decoding */
        len = (l / 4) * 3 + (l % 4 != 0 ? (l % 4) - 1 : 0);

        buf = malloc(len + 1);
        如果 (!buf)
                返回 -ENOMEM;

        CLEANUP_ERASE_PTR(secure ? &buf : NULL, len);

        为 (x = p, z = buf;;) {
                整数 a, b, c, d; /* a == 00XXXXXX; b == 00YYYYYY; c == 00ZZZZZZ; d == 00WWWWWW */

                a = unbase64_next(&x, &l);
                如果 (a == -EPIPE) /* End of string */
                        break;
                如果 (a < 0)
                        返回 a;
                如果 (a == INT_MAX) /* Padding is not allowed at the beginning of a 4ch block */
                        返回 -EINVAL;

                b = unbase64_next(&x, &l);
                如果 (b < 0)
                        返回 b;
                如果 (b == INT_MAX) /* Padding is not allowed at the second character of a 4ch block either */
                        返回 -EINVAL;

                c = unbase64_next(&x, &l);
                如果 (c < 0)
                        返回 c;

                d = unbase64_next(&x, &l);
                如果 (d < 0)
                        返回 d;

                如果 (c == INT_MAX) { /* Padding at the third character */

                        如果 (d != INT_MAX) /* If the third character is padding, the fourth must be too */
                                返回 -EINVAL;

                        /* b == 00YY0000 */
                        如果 (b & 15)
                                返回 -EINVAL;

                        如果 (l > 0) /* Trailing rubbish? */
                                返回 -ENAMETOOLONG;

                        *(z++) = (uint8_t) a << 2 | (uint8_t) (b >> 4); /* XXXXXXYY */
                        break;
                }

                如果 (d == INT_MAX) {
                        /* c == 00ZZZZ00 */
                        如果 (c & 3)
                                返回 -EINVAL;

                        如果 (l > 0) /* Trailing rubbish? */
                                返回 -ENAMETOOLONG;

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

        如果 (ret_size)
                *ret_size = (size_t) (z - buf);
        如果 (ret_data)
                *ret_data = TAKE_PTR(buf);

        返回 0;
}

void hexdump(FILE *f, const void *p, size_t s) {
        const uint8_t *b = p;
        unsigned n = 0;

        assert(b || s == 0);

        如果 (s == SIZE_MAX)
                s = strlen(p);

        如果 (!f)
                f = stdout;

        当 (s > 0) {
                size_t i;

                fprintf(f, "%04x  ", n);

                为 (i = 0; i < 16; i++) {

                        如果 (i >= s)
                                fputs("   ", f);
                        否则
                                fprintf(f, "%02x ", b[i]);

                        如果 (i == 7)
                                fputc(' ', f);
                }

                fputc(' ', f);

                为 (i = 0; i < 16; i++) {

                        如果 (i >= s)
                                fputc(' ', f);
                        否则
                                fputc(isprint(b[i]) ? (char) b[i] : '.', f);
                }

                fputc('\n', f);

                如果 (s < 16)
                        break;

                n += 16;
                b += 16;
                s -= 16;
        }
}
