/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <ctype.h>
#include <stdio.h>

#include "alloc-util.h"
#include "hexdecoct.h"
#include "memory-util.h"
#include "string-util.h"

char octchar(целое x) {
        вернуть '0' + (x & 7);
}

целое unoctchar(char c) {

        если (c >= '0' && c <= '7')
                вернуть c - '0';

        вернуть -EINVAL;
}

char decchar(целое x) {
        вернуть '0' + (x % 10);
}

целое undecchar(char c) {

        если (c >= '0' && c <= '9')
                вернуть c - '0';

        вернуть -EINVAL;
}

char hexchar(целое x) {
        const char *table = LOWERCASE_HEXDIGITS;
        вернуть table[x & 15];
}

целое unhexchar(char c) {

        если (c >= '0' && c <= '9')
                вернуть c - '0';

        если (c >= 'a' && c <= 'f')
                вернуть c - 'a' + 10;

        если (c >= 'A' && c <= 'F')
                вернуть c - 'A' + 10;

        вернуть -EINVAL;
}

char* hexmem(const void *p, size_t l) {
        const uint8_t *x;
        char *r, *z;

        assert(p || l == 0);

        z = r = new(char, l * 2 + 1);
        если (!r)
                вернуть NULL;

        для (x = p; x && x < (const uint8_t*) p + l; x++) {
                *(z++) = hexchar(*x >> 4);
                *(z++) = hexchar(*x & 15);
        }

        *z = 0;
        вернуть r;
}

static целое unhex_next(const char **p, size_t *l) {
        целое r;

        assert(p);
        assert(l);

        /* Find the next non-whitespace character, and decode it. We
         * greedily skip all preceding and all following whitespace. */

        для (;;) {
                если (*l == 0)
                        вернуть -EPIPE;

                если (!strchr(WHITESPACE, **p))
                        break;

                /* Skip leading whitespace */
                (*p)++, (*l)--;
        }

        r = unhexchar(**p);
        если (r < 0)
                вернуть r;

        для (;;) {
                (*p)++, (*l)--;

                если (*l == 0 || !strchr(WHITESPACE, **p))
                        break;

                /* Skip following whitespace */
        }

        вернуть r;
}

целое unhexmem_full(
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

        если (l == SIZE_MAX)
                l = strlen(p);

        /* Note that the calculation of memory size is an upper boundary, as we ignore whitespace while decoding */
        buf_size = (l + 1) / 2 + 1;
        buf = malloc(buf_size);
        если (!buf)
                вернуть -ENOMEM;

        CLEANUP_ERASE_PTR(secure ? &buf : NULL, buf_size);

        для (x = p, z = buf;;) {
                целое a, b;

                a = unhex_next(&x, &l);
                если (a == -EPIPE) /* End of string */
                        break;
                если (a < 0)
                        вернуть a;

                b = unhex_next(&x, &l);
                если (b < 0)
                        вернуть b;

                *(z++) = (uint8_t) a << 4 | (uint8_t) b;
        }

        *z = 0;

        если (ret_size)
                *ret_size = (size_t) (z - buf);
        если (ret_data)
                *ret_data = TAKE_PTR(buf);

        вернуть 0;
}

/* https://tools.ietf.org/html/rfc4648#section-6
 * Notice that base32hex differs from base32 in the alphabet it uses.
 * The distinction is that the base32hex representation preserves the
 * order of the underlying data when compared as bytestrings, this is
 * useful when representing NSEC3 hashes, as one can then verify the
 * order of hashes directly from their representation. */
char base32hexchar(целое x) {
        const char *table = DIGITS "ABCDEFGHIJKLMNOPQRSTUV";
        вернуть table[x & 31];
}

целое unbase32hexchar(char c) {
        unsigned offset;

        если (c >= '0' && c <= '9')
                вернуть c - '0';

        offset = '9' - '0' + 1;

        если (c >= 'A' && c <= 'V')
                вернуть c - 'A' + offset;

        вернуть -EINVAL;
}

char* base32hexmem(const void *p, size_t l, bool padding) {
        char *r, *z;
        const uint8_t *x;
        size_t len;

        assert(p || l == 0);

        если (padding)
                /* five input bytes makes eight output bytes, padding is added so we must round up */
                len = 8 * (l + 4) / 5;
        иначе {
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
        если (!r)
                вернуть NULL;

        для (x = p; x < (const uint8_t*) p + (l / 5) * 5; x += 5) {
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
                если (padding)
                        *(z++) = '=';

                break;

        case 3:
                *(z++) = base32hexchar(x[0] >> 3);                   /* 000XXXXX */
                *(z++) = base32hexchar((x[0] & 7) << 2 | x[1] >> 6); /* 000XXXYY */
                *(z++) = base32hexchar((x[1] & 63) >> 1);            /* 000YYYYY */
                *(z++) = base32hexchar((x[1] & 1) << 4 | x[2] >> 4); /* 000YZZZZ */
                *(z++) = base32hexchar((x[2] & 15) << 1);            /* 000ZZZZ0 */
                если (padding) {
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
                если (padding) {
                        *(z++) = '=';
                        *(z++) = '=';
                        *(z++) = '=';
                        *(z++) = '=';
                }

                break;

        case 1:
                *(z++) = base32hexchar(x[0] >> 3);       /* 000XXXXX */
                *(z++) = base32hexchar((x[0] & 7) << 2); /* 000XXX00 */
                если (padding) {
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
        вернуть r;
}

целое unbase32hexmem(const char *p, size_t l, bool padding, void **mem, size_t *_len) {
        _cleanup_free_ uint8_t *r = NULL;
        целое a, b, c, d, e, f, g, h;
        uint8_t *z;
        const char *x;
        size_t len;
        unsigned pad = 0;

        assert(p || l == 0);
        assert(mem);
        assert(_len);

        если (l == SIZE_MAX)
                l = strlen(p);

        /* padding ensures any base32hex input has input divisible by 8 */
        если (padding && l % 8 != 0)
                вернуть -EINVAL;

        если (padding) {
                /* strip the padding */
                пока (l > 0 && p[l - 1] == '=' && pad < 7) {
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
                вернуть -EINVAL;
        }

        z = r = malloc(len + 1);
        если (!r)
                вернуть -ENOMEM;

        для (x = p; x < p + (l / 8) * 8; x += 8) {
                /* a == 000XXXXX; b == 000YYYYY; c == 000ZZZZZ; d == 000WWWWW
                 * e == 000SSSSS; f == 000QQQQQ; g == 000VVVVV; h == 000RRRRR */
                a = unbase32hexchar(x[0]);
                если (a < 0)
                        вернуть -EINVAL;

                b = unbase32hexchar(x[1]);
                если (b < 0)
                        вернуть -EINVAL;

                c = unbase32hexchar(x[2]);
                если (c < 0)
                        вернуть -EINVAL;

                d = unbase32hexchar(x[3]);
                если (d < 0)
                        вернуть -EINVAL;

                e = unbase32hexchar(x[4]);
                если (e < 0)
                        вернуть -EINVAL;

                f = unbase32hexchar(x[5]);
                если (f < 0)
                        вернуть -EINVAL;

                g = unbase32hexchar(x[6]);
                если (g < 0)
                        вернуть -EINVAL;

                h = unbase32hexchar(x[7]);
                если (h < 0)
                        вернуть -EINVAL;

                *(z++) = (uint8_t) a << 3 | (uint8_t) b >> 2;                    /* XXXXXYYY */
                *(z++) = (uint8_t) b << 6 | (uint8_t) c << 1 | (uint8_t) d >> 4; /* YYZZZZZW */
                *(z++) = (uint8_t) d << 4 | (uint8_t) e >> 1;                    /* WWWWSSSS */
                *(z++) = (uint8_t) e << 7 | (uint8_t) f << 2 | (uint8_t) g >> 3; /* SQQQQQVV */
                *(z++) = (uint8_t) g << 5 | (uint8_t) h;                         /* VVVRRRRR */
        }

        switch (l % 8) {
        case 7:
                a = unbase32hexchar(x[0]);
                если (a < 0)
                        вернуть -EINVAL;

                b = unbase32hexchar(x[1]);
                если (b < 0)
                        вернуть -EINVAL;

                c = unbase32hexchar(x[2]);
                если (c < 0)
                        вернуть -EINVAL;

                d = unbase32hexchar(x[3]);
                если (d < 0)
                        вернуть -EINVAL;

                e = unbase32hexchar(x[4]);
                если (e < 0)
                        вернуть -EINVAL;

                f = unbase32hexchar(x[5]);
                если (f < 0)
                        вернуть -EINVAL;

                g = unbase32hexchar(x[6]);
                если (g < 0)
                        вернуть -EINVAL;

                /* g == 000VV000 */
                если (g & 7)
                        вернуть -EINVAL;

                *(z++) = (uint8_t) a << 3 | (uint8_t) b >> 2;                    /* XXXXXYYY */
                *(z++) = (uint8_t) b << 6 | (uint8_t) c << 1 | (uint8_t) d >> 4; /* YYZZZZZW */
                *(z++) = (uint8_t) d << 4 | (uint8_t) e >> 1;                    /* WWWWSSSS */
                *(z++) = (uint8_t) e << 7 | (uint8_t) f << 2 | (uint8_t) g >> 3; /* SQQQQQVV */

                break;
        case 5:
                a = unbase32hexchar(x[0]);
                если (a < 0)
                        вернуть -EINVAL;

                b = unbase32hexchar(x[1]);
                если (b < 0)
                        вернуть -EINVAL;

                c = unbase32hexchar(x[2]);
                если (c < 0)
                        вернуть -EINVAL;

                d = unbase32hexchar(x[3]);
                если (d < 0)
                        вернуть -EINVAL;

                e = unbase32hexchar(x[4]);
                если (e < 0)
                        вернуть -EINVAL;

                /* e == 000SSSS0 */
                если (e & 1)
                        вернуть -EINVAL;

                *(z++) = (uint8_t) a << 3 | (uint8_t) b >> 2;                    /* XXXXXYYY */
                *(z++) = (uint8_t) b << 6 | (uint8_t) c << 1 | (uint8_t) d >> 4; /* YYZZZZZW */
                *(z++) = (uint8_t) d << 4 | (uint8_t) e >> 1;                    /* WWWWSSSS */

                break;
        case 4:
                a = unbase32hexchar(x[0]);
                если (a < 0)
                        вернуть -EINVAL;

                b = unbase32hexchar(x[1]);
                если (b < 0)
                        вернуть -EINVAL;

                c = unbase32hexchar(x[2]);
                если (c < 0)
                        вернуть -EINVAL;

                d = unbase32hexchar(x[3]);
                если (d < 0)
                        вернуть -EINVAL;

                /* d == 000W0000 */
                если (d & 15)
                        вернуть -EINVAL;

                *(z++) = (uint8_t) a << 3 | (uint8_t) b >> 2;                    /* XXXXXYYY */
                *(z++) = (uint8_t) b << 6 | (uint8_t) c << 1 | (uint8_t) d >> 4; /* YYZZZZZW */

                break;
        case 2:
                a = unbase32hexchar(x[0]);
                если (a < 0)
                        вернуть -EINVAL;

                b = unbase32hexchar(x[1]);
                если (b < 0)
                        вернуть -EINVAL;

                /* b == 000YYY00 */
                если (b & 3)
                        вернуть -EINVAL;

                *(z++) = (uint8_t) a << 3 | (uint8_t) b >> 2; /* XXXXXYYY */

                break;
        case 0:
                break;
        default:
                вернуть -EINVAL;
        }

        *z = 0;

        *mem = TAKE_PTR(r);
        *_len = len;

        вернуть 0;
}

/* https://tools.ietf.org/html/rfc4648#section-4 */
char base64char(целое x) {
        const char *table = UPPERCASE_LETTERS LOWERCASE_LETTERS DIGITS "+/";
        вернуть table[x & 63];
}

/* This is almost base64char(), but not entirely, as it uses the "url and filename safe" alphabet,
 * since we don't want "/" appear in interface names (since interfaces appear in sysfs as filenames).
 * See section #5 of RFC 4648. */
char urlsafe_base64char(целое x) {
        const char *table = UPPERCASE_LETTERS LOWERCASE_LETTERS DIGITS "-_";
        вернуть table[x & 63];
}

целое unbase64char(char c) {
        unsigned offset;

        если (c >= 'A' && c <= 'Z')
                вернуть c - 'A';

        offset = 'Z' - 'A' + 1;

        если (c >= 'a' && c <= 'z')
                вернуть c - 'a' + offset;

        offset += 'z' - 'a' + 1;

        если (c >= '0' && c <= '9')
                вернуть c - '0' + offset;

        offset += '9' - '0' + 1;

        если (IN_SET(c, '+', '-')) /* Support both the regular and the URL safe character set (see above) */
                вернуть offset;

        offset++;

        если (IN_SET(c, '/', '_')) /* ditto */
                вернуть offset;

        вернуть -EINVAL;
}

static void maybe_line_break(char **x, char *start, size_t line_break) {
        size_t n;

        assert(x);
        assert(*x);
        assert(start);
        assert(*x >= start);

        если (line_break == SIZE_MAX)
                вернуть;

        n = *x - start;

        если (n % (line_break + 1) == line_break)
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
        если (line_break != SIZE_MAX)
                m += m / line_break;

        z = b = malloc(m);
        если (!b)
                вернуть -ENOMEM;

        для (x = p; x && x < (const uint8_t*) p + (l / 3) * 3; x += 3) {
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
        вернуть z - b;
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
        если (len < 0)
                вернуть len;
        если (len == 0)
                вернуть plen;

        lines = DIV_ROUND_UP(len, width);

        если (plen >= SSIZE_MAX - 1 - 1 ||
            lines > (SSIZE_MAX - plen - 1 - 1) / (indent + width + 1))
                вернуть -ENOMEM;

        t = realloc(*prefix, plen + 1 + 1 + (indent + width + 1) * lines);
        если (!t)
                вернуть -ENOMEM;

        s = t + plen;
        для (size_t line = 0; line < lines; line++) {
                size_t act = MIN(width, (size_t) len);

                если (line > 0)
                        sep = '\n';

                если (s > t) {
                        *s++ = sep;
                        если (sep == '\n')
                                s = mempset(s, ' ', indent);
                }

                s = mempcpy(s, x + width * line, act);
                len -= act;
        }
        assert(len == 0);

        *s = '\0';
        *prefix = t;
        вернуть s - t;
}

ssize_t base64_append(
                char **prefix,
                size_t plen,
                const void *p,
                size_t l,
                size_t indent,
                size_t width) {

        если (plen > width / 2 || plen + indent > width)
                /* leave indent on the left, keep last column free */
                вернуть base64_append_width(prefix, plen, '\n', indent, p, l, width - indent);
        иначе
                /* leave plen on the left, keep last column free */
                вернуть base64_append_width(prefix, plen, ' ', plen + 1, p, l, width - plen - 1);
}

static целое unbase64_next(const char **p, size_t *l) {
        целое ret;

        assert(p);
        assert(l);

        /* Find the next non-whitespace character, and decode it. If we find padding, we return it as INT_MAX. We
         * greedily skip all preceding and all following whitespace. */

        для (;;) {
                если (*l == 0)
                        вернуть -EPIPE;

                если (!strchr(WHITESPACE, **p))
                        break;

                /* Skip leading whitespace */
                (*p)++, (*l)--;
        }

        если (**p == '=')
                ret = INT_MAX; /* return padding as INT_MAX */
        иначе {
                ret = unbase64char(**p);
                если (ret < 0)
                        вернуть ret;
        }

        для (;;) {
                (*p)++, (*l)--;

                если (*l == 0)
                        break;
                если (!strchr(WHITESPACE, **p))
                        break;

                /* Skip following whitespace */
        }

        вернуть ret;
}

целое unbase64mem_full(
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

        если (l == SIZE_MAX)
                l = strlen(p);

        /* A group of four input bytes needs three output bytes, in case of padding we need to add two or three extra
         * bytes. Note that this calculation is an upper boundary, as we ignore whitespace while decoding */
        len = (l / 4) * 3 + (l % 4 != 0 ? (l % 4) - 1 : 0);

        buf = malloc(len + 1);
        если (!buf)
                вернуть -ENOMEM;

        CLEANUP_ERASE_PTR(secure ? &buf : NULL, len);

        для (x = p, z = buf;;) {
                целое a, b, c, d; /* a == 00XXXXXX; b == 00YYYYYY; c == 00ZZZZZZ; d == 00WWWWWW */

                a = unbase64_next(&x, &l);
                если (a == -EPIPE) /* End of string */
                        break;
                если (a < 0)
                        вернуть a;
                если (a == INT_MAX) /* Padding is not allowed at the beginning of a 4ch block */
                        вернуть -EINVAL;

                b = unbase64_next(&x, &l);
                если (b < 0)
                        вернуть b;
                если (b == INT_MAX) /* Padding is not allowed at the second character of a 4ch block either */
                        вернуть -EINVAL;

                c = unbase64_next(&x, &l);
                если (c < 0)
                        вернуть c;

                d = unbase64_next(&x, &l);
                если (d < 0)
                        вернуть d;

                если (c == INT_MAX) { /* Padding at the third character */

                        если (d != INT_MAX) /* If the third character is padding, the fourth must be too */
                                вернуть -EINVAL;

                        /* b == 00YY0000 */
                        если (b & 15)
                                вернуть -EINVAL;

                        если (l > 0) /* Trailing rubbish? */
                                вернуть -ENAMETOOLONG;

                        *(z++) = (uint8_t) a << 2 | (uint8_t) (b >> 4); /* XXXXXXYY */
                        break;
                }

                если (d == INT_MAX) {
                        /* c == 00ZZZZ00 */
                        если (c & 3)
                                вернуть -EINVAL;

                        если (l > 0) /* Trailing rubbish? */
                                вернуть -ENAMETOOLONG;

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

        если (ret_size)
                *ret_size = (size_t) (z - buf);
        если (ret_data)
                *ret_data = TAKE_PTR(buf);

        вернуть 0;
}

void hexdump(FILE *f, const void *p, size_t s) {
        const uint8_t *b = p;
        unsigned n = 0;

        assert(b || s == 0);

        если (s == SIZE_MAX)
                s = strlen(p);

        если (!f)
                f = stdout;

        пока (s > 0) {
                size_t i;

                fprintf(f, "%04x  ", n);

                для (i = 0; i < 16; i++) {

                        если (i >= s)
                                fputs("   ", f);
                        иначе
                                fprintf(f, "%02x ", b[i]);

                        если (i == 7)
                                fputc(' ', f);
                }

                fputc(' ', f);

                для (i = 0; i < 16; i++) {

                        если (i >= s)
                                fputc(' ', f);
                        иначе
                                fputc(isprint(b[i]) ? (char) b[i] : '.', f);
                }

                fputc('\n', f);

                если (s < 16)
                        break;

                n += 16;
                b += 16;
                s -= 16;
        }
}
