/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <ctype.h>
#include <stdio.h>

#include "alloc-util.h"
#include "hexdecoct.h"
#include "memory-util.h"
#include "string-util.h"

char octchar(số_nguyên x) {
        trả_về '0' + (x & 7);
}

số_nguyên unoctchar(char c) {

        nếu (c >= '0' && c <= '7')
                trả_về c - '0';

        trả_về -EINVAL;
}

char decchar(số_nguyên x) {
        trả_về '0' + (x % 10);
}

số_nguyên undecchar(char c) {

        nếu (c >= '0' && c <= '9')
                trả_về c - '0';

        trả_về -EINVAL;
}

char hexchar(số_nguyên x) {
        const char *table = LOWERCASE_HEXDIGITS;
        trả_về table[x & 15];
}

số_nguyên unhexchar(char c) {

        nếu (c >= '0' && c <= '9')
                trả_về c - '0';

        nếu (c >= 'a' && c <= 'f')
                trả_về c - 'a' + 10;

        nếu (c >= 'A' && c <= 'F')
                trả_về c - 'A' + 10;

        trả_về -EINVAL;
}

char* hexmem(const void *p, size_t l) {
        const uint8_t *x;
        char *r, *z;

        assert(p || l == 0);

        z = r = new(char, l * 2 + 1);
        nếu (!r)
                trả_về NULL;

        cho (x = p; x && x < (const uint8_t*) p + l; x++) {
                *(z++) = hexchar(*x >> 4);
                *(z++) = hexchar(*x & 15);
        }

        *z = 0;
        trả_về r;
}

static số_nguyên unhex_next(const char **p, size_t *l) {
        số_nguyên r;

        assert(p);
        assert(l);

        /* Find the next non-whitespace character, and decode it. We
         * greedily skip all preceding and all following whitespace. */

        cho (;;) {
                nếu (*l == 0)
                        trả_về -EPIPE;

                nếu (!strchr(WHITESPACE, **p))
                        break;

                /* Skip leading whitespace */
                (*p)++, (*l)--;
        }

        r = unhexchar(**p);
        nếu (r < 0)
                trả_về r;

        cho (;;) {
                (*p)++, (*l)--;

                nếu (*l == 0 || !strchr(WHITESPACE, **p))
                        break;

                /* Skip following whitespace */
        }

        trả_về r;
}

số_nguyên unhexmem_full(
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

        nếu (l == SIZE_MAX)
                l = strlen(p);

        /* Note that the calculation of memory size is an upper boundary, as we ignore whitespace while decoding */
        buf_size = (l + 1) / 2 + 1;
        buf = malloc(buf_size);
        nếu (!buf)
                trả_về -ENOMEM;

        CLEANUP_ERASE_PTR(secure ? &buf : NULL, buf_size);

        cho (x = p, z = buf;;) {
                số_nguyên a, b;

                a = unhex_next(&x, &l);
                nếu (a == -EPIPE) /* End of string */
                        break;
                nếu (a < 0)
                        trả_về a;

                b = unhex_next(&x, &l);
                nếu (b < 0)
                        trả_về b;

                *(z++) = (uint8_t) a << 4 | (uint8_t) b;
        }

        *z = 0;

        nếu (ret_size)
                *ret_size = (size_t) (z - buf);
        nếu (ret_data)
                *ret_data = TAKE_PTR(buf);

        trả_về 0;
}

/* https://tools.ietf.org/html/rfc4648#section-6
 * Notice that base32hex differs from base32 in the alphabet it uses.
 * The distinction is that the base32hex representation preserves the
 * order of the underlying data when compared as bytestrings, this is
 * useful when representing NSEC3 hashes, as one can then verify the
 * order of hashes directly from their representation. */
char base32hexchar(số_nguyên x) {
        const char *table = DIGITS "ABCDEFGHIJKLMNOPQRSTUV";
        trả_về table[x & 31];
}

số_nguyên unbase32hexchar(char c) {
        unsigned offset;

        nếu (c >= '0' && c <= '9')
                trả_về c - '0';

        offset = '9' - '0' + 1;

        nếu (c >= 'A' && c <= 'V')
                trả_về c - 'A' + offset;

        trả_về -EINVAL;
}

char* base32hexmem(const void *p, size_t l, bool padding) {
        char *r, *z;
        const uint8_t *x;
        size_t len;

        assert(p || l == 0);

        nếu (padding)
                /* five input bytes makes eight output bytes, padding is added so we must round up */
                len = 8 * (l + 4) / 5;
        không_thì {
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
        nếu (!r)
                trả_về NULL;

        cho (x = p; x < (const uint8_t*) p + (l / 5) * 5; x += 5) {
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
                nếu (padding)
                        *(z++) = '=';

                break;

        case 3:
                *(z++) = base32hexchar(x[0] >> 3);                   /* 000XXXXX */
                *(z++) = base32hexchar((x[0] & 7) << 2 | x[1] >> 6); /* 000XXXYY */
                *(z++) = base32hexchar((x[1] & 63) >> 1);            /* 000YYYYY */
                *(z++) = base32hexchar((x[1] & 1) << 4 | x[2] >> 4); /* 000YZZZZ */
                *(z++) = base32hexchar((x[2] & 15) << 1);            /* 000ZZZZ0 */
                nếu (padding) {
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
                nếu (padding) {
                        *(z++) = '=';
                        *(z++) = '=';
                        *(z++) = '=';
                        *(z++) = '=';
                }

                break;

        case 1:
                *(z++) = base32hexchar(x[0] >> 3);       /* 000XXXXX */
                *(z++) = base32hexchar((x[0] & 7) << 2); /* 000XXX00 */
                nếu (padding) {
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
        trả_về r;
}

số_nguyên unbase32hexmem(const char *p, size_t l, bool padding, void **mem, size_t *_len) {
        _cleanup_free_ uint8_t *r = NULL;
        số_nguyên a, b, c, d, e, f, g, h;
        uint8_t *z;
        const char *x;
        size_t len;
        unsigned pad = 0;

        assert(p || l == 0);
        assert(mem);
        assert(_len);

        nếu (l == SIZE_MAX)
                l = strlen(p);

        /* padding ensures any base32hex input has input divisible by 8 */
        nếu (padding && l % 8 != 0)
                trả_về -EINVAL;

        nếu (padding) {
                /* strip the padding */
                trong_khi (l > 0 && p[l - 1] == '=' && pad < 7) {
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
                trả_về -EINVAL;
        }

        z = r = malloc(len + 1);
        nếu (!r)
                trả_về -ENOMEM;

        cho (x = p; x < p + (l / 8) * 8; x += 8) {
                /* a == 000XXXXX; b == 000YYYYY; c == 000ZZZZZ; d == 000WWWWW
                 * e == 000SSSSS; f == 000QQQQQ; g == 000VVVVV; h == 000RRRRR */
                a = unbase32hexchar(x[0]);
                nếu (a < 0)
                        trả_về -EINVAL;

                b = unbase32hexchar(x[1]);
                nếu (b < 0)
                        trả_về -EINVAL;

                c = unbase32hexchar(x[2]);
                nếu (c < 0)
                        trả_về -EINVAL;

                d = unbase32hexchar(x[3]);
                nếu (d < 0)
                        trả_về -EINVAL;

                e = unbase32hexchar(x[4]);
                nếu (e < 0)
                        trả_về -EINVAL;

                f = unbase32hexchar(x[5]);
                nếu (f < 0)
                        trả_về -EINVAL;

                g = unbase32hexchar(x[6]);
                nếu (g < 0)
                        trả_về -EINVAL;

                h = unbase32hexchar(x[7]);
                nếu (h < 0)
                        trả_về -EINVAL;

                *(z++) = (uint8_t) a << 3 | (uint8_t) b >> 2;                    /* XXXXXYYY */
                *(z++) = (uint8_t) b << 6 | (uint8_t) c << 1 | (uint8_t) d >> 4; /* YYZZZZZW */
                *(z++) = (uint8_t) d << 4 | (uint8_t) e >> 1;                    /* WWWWSSSS */
                *(z++) = (uint8_t) e << 7 | (uint8_t) f << 2 | (uint8_t) g >> 3; /* SQQQQQVV */
                *(z++) = (uint8_t) g << 5 | (uint8_t) h;                         /* VVVRRRRR */
        }

        switch (l % 8) {
        case 7:
                a = unbase32hexchar(x[0]);
                nếu (a < 0)
                        trả_về -EINVAL;

                b = unbase32hexchar(x[1]);
                nếu (b < 0)
                        trả_về -EINVAL;

                c = unbase32hexchar(x[2]);
                nếu (c < 0)
                        trả_về -EINVAL;

                d = unbase32hexchar(x[3]);
                nếu (d < 0)
                        trả_về -EINVAL;

                e = unbase32hexchar(x[4]);
                nếu (e < 0)
                        trả_về -EINVAL;

                f = unbase32hexchar(x[5]);
                nếu (f < 0)
                        trả_về -EINVAL;

                g = unbase32hexchar(x[6]);
                nếu (g < 0)
                        trả_về -EINVAL;

                /* g == 000VV000 */
                nếu (g & 7)
                        trả_về -EINVAL;

                *(z++) = (uint8_t) a << 3 | (uint8_t) b >> 2;                    /* XXXXXYYY */
                *(z++) = (uint8_t) b << 6 | (uint8_t) c << 1 | (uint8_t) d >> 4; /* YYZZZZZW */
                *(z++) = (uint8_t) d << 4 | (uint8_t) e >> 1;                    /* WWWWSSSS */
                *(z++) = (uint8_t) e << 7 | (uint8_t) f << 2 | (uint8_t) g >> 3; /* SQQQQQVV */

                break;
        case 5:
                a = unbase32hexchar(x[0]);
                nếu (a < 0)
                        trả_về -EINVAL;

                b = unbase32hexchar(x[1]);
                nếu (b < 0)
                        trả_về -EINVAL;

                c = unbase32hexchar(x[2]);
                nếu (c < 0)
                        trả_về -EINVAL;

                d = unbase32hexchar(x[3]);
                nếu (d < 0)
                        trả_về -EINVAL;

                e = unbase32hexchar(x[4]);
                nếu (e < 0)
                        trả_về -EINVAL;

                /* e == 000SSSS0 */
                nếu (e & 1)
                        trả_về -EINVAL;

                *(z++) = (uint8_t) a << 3 | (uint8_t) b >> 2;                    /* XXXXXYYY */
                *(z++) = (uint8_t) b << 6 | (uint8_t) c << 1 | (uint8_t) d >> 4; /* YYZZZZZW */
                *(z++) = (uint8_t) d << 4 | (uint8_t) e >> 1;                    /* WWWWSSSS */

                break;
        case 4:
                a = unbase32hexchar(x[0]);
                nếu (a < 0)
                        trả_về -EINVAL;

                b = unbase32hexchar(x[1]);
                nếu (b < 0)
                        trả_về -EINVAL;

                c = unbase32hexchar(x[2]);
                nếu (c < 0)
                        trả_về -EINVAL;

                d = unbase32hexchar(x[3]);
                nếu (d < 0)
                        trả_về -EINVAL;

                /* d == 000W0000 */
                nếu (d & 15)
                        trả_về -EINVAL;

                *(z++) = (uint8_t) a << 3 | (uint8_t) b >> 2;                    /* XXXXXYYY */
                *(z++) = (uint8_t) b << 6 | (uint8_t) c << 1 | (uint8_t) d >> 4; /* YYZZZZZW */

                break;
        case 2:
                a = unbase32hexchar(x[0]);
                nếu (a < 0)
                        trả_về -EINVAL;

                b = unbase32hexchar(x[1]);
                nếu (b < 0)
                        trả_về -EINVAL;

                /* b == 000YYY00 */
                nếu (b & 3)
                        trả_về -EINVAL;

                *(z++) = (uint8_t) a << 3 | (uint8_t) b >> 2; /* XXXXXYYY */

                break;
        case 0:
                break;
        default:
                trả_về -EINVAL;
        }

        *z = 0;

        *mem = TAKE_PTR(r);
        *_len = len;

        trả_về 0;
}

/* https://tools.ietf.org/html/rfc4648#section-4 */
char base64char(số_nguyên x) {
        const char *table = UPPERCASE_LETTERS LOWERCASE_LETTERS DIGITS "+/";
        trả_về table[x & 63];
}

/* This is almost base64char(), but not entirely, as it uses the "url and filename safe" alphabet,
 * since we don't want "/" appear in interface names (since interfaces appear in sysfs as filenames).
 * See section #5 of RFC 4648. */
char urlsafe_base64char(số_nguyên x) {
        const char *table = UPPERCASE_LETTERS LOWERCASE_LETTERS DIGITS "-_";
        trả_về table[x & 63];
}

số_nguyên unbase64char(char c) {
        unsigned offset;

        nếu (c >= 'A' && c <= 'Z')
                trả_về c - 'A';

        offset = 'Z' - 'A' + 1;

        nếu (c >= 'a' && c <= 'z')
                trả_về c - 'a' + offset;

        offset += 'z' - 'a' + 1;

        nếu (c >= '0' && c <= '9')
                trả_về c - '0' + offset;

        offset += '9' - '0' + 1;

        nếu (IN_SET(c, '+', '-')) /* Support both the regular and the URL safe character set (see above) */
                trả_về offset;

        offset++;

        nếu (IN_SET(c, '/', '_')) /* ditto */
                trả_về offset;

        trả_về -EINVAL;
}

static void maybe_line_break(char **x, char *start, size_t line_break) {
        size_t n;

        assert(x);
        assert(*x);
        assert(start);
        assert(*x >= start);

        nếu (line_break == SIZE_MAX)
                trả_về;

        n = *x - start;

        nếu (n % (line_break + 1) == line_break)
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
        nếu (line_break != SIZE_MAX)
                m += m / line_break;

        z = b = malloc(m);
        nếu (!b)
                trả_về -ENOMEM;

        cho (x = p; x && x < (const uint8_t*) p + (l / 3) * 3; x += 3) {
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
        trả_về z - b;
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
        nếu (len < 0)
                trả_về len;
        nếu (len == 0)
                trả_về plen;

        lines = DIV_ROUND_UP(len, width);

        nếu (plen >= SSIZE_MAX - 1 - 1 ||
            lines > (SSIZE_MAX - plen - 1 - 1) / (indent + width + 1))
                trả_về -ENOMEM;

        t = realloc(*prefix, plen + 1 + 1 + (indent + width + 1) * lines);
        nếu (!t)
                trả_về -ENOMEM;

        s = t + plen;
        cho (size_t line = 0; line < lines; line++) {
                size_t act = MIN(width, (size_t) len);

                nếu (line > 0)
                        sep = '\n';

                nếu (s > t) {
                        *s++ = sep;
                        nếu (sep == '\n')
                                s = mempset(s, ' ', indent);
                }

                s = mempcpy(s, x + width * line, act);
                len -= act;
        }
        assert(len == 0);

        *s = '\0';
        *prefix = t;
        trả_về s - t;
}

ssize_t base64_append(
                char **prefix,
                size_t plen,
                const void *p,
                size_t l,
                size_t indent,
                size_t width) {

        nếu (plen > width / 2 || plen + indent > width)
                /* leave indent on the left, keep last column free */
                trả_về base64_append_width(prefix, plen, '\n', indent, p, l, width - indent);
        không_thì
                /* leave plen on the left, keep last column free */
                trả_về base64_append_width(prefix, plen, ' ', plen + 1, p, l, width - plen - 1);
}

static số_nguyên unbase64_next(const char **p, size_t *l) {
        số_nguyên ret;

        assert(p);
        assert(l);

        /* Find the next non-whitespace character, and decode it. If we find padding, we return it as INT_MAX. We
         * greedily skip all preceding and all following whitespace. */

        cho (;;) {
                nếu (*l == 0)
                        trả_về -EPIPE;

                nếu (!strchr(WHITESPACE, **p))
                        break;

                /* Skip leading whitespace */
                (*p)++, (*l)--;
        }

        nếu (**p == '=')
                ret = INT_MAX; /* return padding as INT_MAX */
        không_thì {
                ret = unbase64char(**p);
                nếu (ret < 0)
                        trả_về ret;
        }

        cho (;;) {
                (*p)++, (*l)--;

                nếu (*l == 0)
                        break;
                nếu (!strchr(WHITESPACE, **p))
                        break;

                /* Skip following whitespace */
        }

        trả_về ret;
}

số_nguyên unbase64mem_full(
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

        nếu (l == SIZE_MAX)
                l = strlen(p);

        /* A group of four input bytes needs three output bytes, in case of padding we need to add two or three extra
         * bytes. Note that this calculation is an upper boundary, as we ignore whitespace while decoding */
        len = (l / 4) * 3 + (l % 4 != 0 ? (l % 4) - 1 : 0);

        buf = malloc(len + 1);
        nếu (!buf)
                trả_về -ENOMEM;

        CLEANUP_ERASE_PTR(secure ? &buf : NULL, len);

        cho (x = p, z = buf;;) {
                số_nguyên a, b, c, d; /* a == 00XXXXXX; b == 00YYYYYY; c == 00ZZZZZZ; d == 00WWWWWW */

                a = unbase64_next(&x, &l);
                nếu (a == -EPIPE) /* End of string */
                        break;
                nếu (a < 0)
                        trả_về a;
                nếu (a == INT_MAX) /* Padding is not allowed at the beginning of a 4ch block */
                        trả_về -EINVAL;

                b = unbase64_next(&x, &l);
                nếu (b < 0)
                        trả_về b;
                nếu (b == INT_MAX) /* Padding is not allowed at the second character of a 4ch block either */
                        trả_về -EINVAL;

                c = unbase64_next(&x, &l);
                nếu (c < 0)
                        trả_về c;

                d = unbase64_next(&x, &l);
                nếu (d < 0)
                        trả_về d;

                nếu (c == INT_MAX) { /* Padding at the third character */

                        nếu (d != INT_MAX) /* If the third character is padding, the fourth must be too */
                                trả_về -EINVAL;

                        /* b == 00YY0000 */
                        nếu (b & 15)
                                trả_về -EINVAL;

                        nếu (l > 0) /* Trailing rubbish? */
                                trả_về -ENAMETOOLONG;

                        *(z++) = (uint8_t) a << 2 | (uint8_t) (b >> 4); /* XXXXXXYY */
                        break;
                }

                nếu (d == INT_MAX) {
                        /* c == 00ZZZZ00 */
                        nếu (c & 3)
                                trả_về -EINVAL;

                        nếu (l > 0) /* Trailing rubbish? */
                                trả_về -ENAMETOOLONG;

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

        nếu (ret_size)
                *ret_size = (size_t) (z - buf);
        nếu (ret_data)
                *ret_data = TAKE_PTR(buf);

        trả_về 0;
}

void hexdump(FILE *f, const void *p, size_t s) {
        const uint8_t *b = p;
        unsigned n = 0;

        assert(b || s == 0);

        nếu (s == SIZE_MAX)
                s = strlen(p);

        nếu (!f)
                f = stdout;

        trong_khi (s > 0) {
                size_t i;

                fprintf(f, "%04x  ", n);

                cho (i = 0; i < 16; i++) {

                        nếu (i >= s)
                                fputs("   ", f);
                        không_thì
                                fprintf(f, "%02x ", b[i]);

                        nếu (i == 7)
                                fputc(' ', f);
                }

                fputc(' ', f);

                cho (i = 0; i < 16; i++) {

                        nếu (i >= s)
                                fputc(' ', f);
                        không_thì
                                fputc(isprint(b[i]) ? (char) b[i] : '.', f);
                }

                fputc('\n', f);

                nếu (s < 16)
                        break;

                n += 16;
                b += 16;
                s -= 16;
        }
}
