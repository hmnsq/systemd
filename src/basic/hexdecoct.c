/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن <ctype.h>
#ضمن <stdio.h>

#ضمن "alloc-util.h"
#ضمن "hexdecoct.h"
#ضمن "memory-util.h"
#ضمن "string-util.h"

حرف octchar(صحيح x) {
        ܦܢܐ '0' + (x & 7);
}

صحيح unoctchar(حرف c) {

        ܐܢ (c >= '0' && c <= '7')
                ܦܢܐ c - '0';

        ܦܢܐ -EINVAL;
}

حرف decchar(صحيح x) {
        ܦܢܐ '0' + (x % 10);
}

صحيح undecchar(حرف c) {

        ܐܢ (c >= '0' && c <= '9')
                ܦܢܐ c - '0';

        ܦܢܐ -EINVAL;
}

حرف hexchar(صحيح x) {
        ثابت حرف *table = LOWERCASE_HEXDIGITS;
        ܦܢܐ table[x & 15];
}

صحيح unhexchar(حرف c) {

        ܐܢ (c >= '0' && c <= '9')
                ܦܢܐ c - '0';

        ܐܢ (c >= 'a' && c <= 'f')
                ܦܢܐ c - 'a' + 10;

        ܐܢ (c >= 'A' && c <= 'F')
                ܦܢܐ c - 'A' + 10;

        ܦܢܐ -EINVAL;
}

حرف* hexmem(ثابت فراغ *p, size_t l) {
        ثابت uint8_t *x;
        حرف *r, *z;

        أكد(p || l == 0);

        z = r = new(حرف, l * 2 + 1);
        ܐܢ (!r)
                ܦܢܐ عدم;

        لكل (x = p; x && x < (ثابت uint8_t*) p + l; x++) {
                *(z++) = hexchar(*x >> 4);
                *(z++) = hexchar(*x & 15);
        }

        *z = 0;
        ܦܢܐ r;
}

ساكن صحيح unhex_next(ثابت حرف **p, size_t *l) {
        صحيح r;

        أكد(p);
        أكد(l);

        /* Find the next non-whitespace character, and decode it. We
         * greedily skip all preceding and all following whitespace. */

        لكل (;;) {
                ܐܢ (*l == 0)
                        ܦܢܐ -EPIPE;

                ܐܢ (!strchr(WHITESPACE, **p))
                        اكسر;

                /* Skip leading whitespace */
                (*p)++, (*l)--;
        }

        r = unhexchar(**p);
        ܐܢ (r < 0)
                ܦܢܐ r;

        لكل (;;) {
                (*p)++, (*l)--;

                ܐܢ (*l == 0 || !strchr(WHITESPACE, **p))
                        اكسر;

                /* Skip following whitespace */
        }

        ܦܢܐ r;
}

صحيح unhexmem_full(
                ثابت حرف *p,
                size_t l,
                bool secure,
                فراغ **ret_data,
                size_t *ret_size) {

        _cleanup_free_ uint8_t *buf = عدم;
        size_t buf_size;
        ثابت حرف *x;
        uint8_t *z;

        أكد(p || l == 0);

        ܐܢ (l == SIZE_MAX)
                l = strlen(p);

        /* Note that the calculation of memory size is an upper boundary, as we ignore whitespace while decoding */
        buf_size = (l + 1) / 2 + 1;
        buf = malloc(buf_size);
        ܐܢ (!buf)
                ܦܢܐ -ENOMEM;

        CLEANUP_ERASE_PTR(secure ? &buf : عدم, buf_size);

        لكل (x = p, z = buf;;) {
                صحيح a, b;

                a = unhex_next(&x, &l);
                ܐܢ (a == -EPIPE) /* End of string */
                        اكسر;
                ܐܢ (a < 0)
                        ܦܢܐ a;

                b = unhex_next(&x, &l);
                ܐܢ (b < 0)
                        ܦܢܐ b;

                *(z++) = (uint8_t) a << 4 | (uint8_t) b;
        }

        *z = 0;

        ܐܢ (ret_size)
                *ret_size = (size_t) (z - buf);
        ܐܢ (ret_data)
                *ret_data = TAKE_PTR(buf);

        ܦܢܐ 0;
}

/* https://tools.ietf.org/html/rfc4648#section-6
 * Notice that base32hex differs from base32 in the alphabet it uses.
 * The distinction is that the base32hex representation preserves the
 * order of the underlying data when compared as bytestrings, this is
 * useful when representing NSEC3 hashes, as one can then verify the
 * order of hashes directly from their representation. */
حرف base32hexchar(صحيح x) {
        ثابت حرف *table = DIGITS "ABCDEFGHIJKLMNOPQRSTUV";
        ܦܢܐ table[x & 31];
}

صحيح unbase32hexchar(حرف c) {
        unsigned offset;

        ܐܢ (c >= '0' && c <= '9')
                ܦܢܐ c - '0';

        offset = '9' - '0' + 1;

        ܐܢ (c >= 'A' && c <= 'V')
                ܦܢܐ c - 'A' + offset;

        ܦܢܐ -EINVAL;
}

حرف* base32hexmem(ثابت فراغ *p, size_t l, bool padding) {
        حرف *r, *z;
        ثابت uint8_t *x;
        size_t len;

        أكد(p || l == 0);

        ܐܢ (padding)
                /* five input bytes makes eight output bytes, padding is added so we must round up */
                len = 8 * (l + 4) / 5;
        ܐܚܪܢܐ {
                /* same, but round down as there is no padding */
                len = 8 * l / 5;

                بدل (l % 5) {
                حالة 4:
                        len += 7;
                        اكسر;
                حالة 3:
                        len += 5;
                        اكسر;
                حالة 2:
                        len += 4;
                        اكسر;
                حالة 1:
                        len += 2;
                        اكسر;
                }
        }

        z = r = malloc(len + 1);
        ܐܢ (!r)
                ܦܢܐ عدم;

        لكل (x = p; x < (ثابت uint8_t*) p + (l / 5) * 5; x += 5) {
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

        بدل (l % 5) {
        حالة 4:
                *(z++) = base32hexchar(x[0] >> 3);                    /* 000XXXXX */
                *(z++) = base32hexchar((x[0] & 7) << 2 | x[1] >> 6);  /* 000XXXYY */
                *(z++) = base32hexchar((x[1] & 63) >> 1);             /* 000YYYYY */
                *(z++) = base32hexchar((x[1] & 1) << 4 | x[2] >> 4);   /* 000YZZZZ */
                *(z++) = base32hexchar((x[2] & 15) << 1 | x[3] >> 7); /* 000ZZZZQ */
                *(z++) = base32hexchar((x[3] & 127) >> 2);            /* 000QQQQQ */
                *(z++) = base32hexchar((x[3] & 3) << 3);              /* 000QQ000 */
                ܐܢ (padding)
                        *(z++) = '=';

                اكسر;

        حالة 3:
                *(z++) = base32hexchar(x[0] >> 3);                   /* 000XXXXX */
                *(z++) = base32hexchar((x[0] & 7) << 2 | x[1] >> 6); /* 000XXXYY */
                *(z++) = base32hexchar((x[1] & 63) >> 1);            /* 000YYYYY */
                *(z++) = base32hexchar((x[1] & 1) << 4 | x[2] >> 4); /* 000YZZZZ */
                *(z++) = base32hexchar((x[2] & 15) << 1);            /* 000ZZZZ0 */
                ܐܢ (padding) {
                        *(z++) = '=';
                        *(z++) = '=';
                        *(z++) = '=';
                }

                اكسر;

        حالة 2:
                *(z++) = base32hexchar(x[0] >> 3);                   /* 000XXXXX */
                *(z++) = base32hexchar((x[0] & 7) << 2 | x[1] >> 6); /* 000XXXYY */
                *(z++) = base32hexchar((x[1] & 63) >> 1);            /* 000YYYYY */
                *(z++) = base32hexchar((x[1] & 1) << 4);             /* 000Y0000 */
                ܐܢ (padding) {
                        *(z++) = '=';
                        *(z++) = '=';
                        *(z++) = '=';
                        *(z++) = '=';
                }

                اكسر;

        حالة 1:
                *(z++) = base32hexchar(x[0] >> 3);       /* 000XXXXX */
                *(z++) = base32hexchar((x[0] & 7) << 2); /* 000XXX00 */
                ܐܢ (padding) {
                        *(z++) = '=';
                        *(z++) = '=';
                        *(z++) = '=';
                        *(z++) = '=';
                        *(z++) = '=';
                        *(z++) = '=';
                }

                اكسر;
        }

        *z = 0;
        ܦܢܐ r;
}

صحيح unbase32hexmem(ثابت حرف *p, size_t l, bool padding, فراغ **mem, size_t *_len) {
        _cleanup_free_ uint8_t *r = عدم;
        صحيح a, b, c, d, e, f, g, h;
        uint8_t *z;
        ثابت حرف *x;
        size_t len;
        unsigned pad = 0;

        أكد(p || l == 0);
        أكد(mem);
        أكد(_len);

        ܐܢ (l == SIZE_MAX)
                l = strlen(p);

        /* padding ensures any base32hex input has input divisible by 8 */
        ܐܢ (padding && l % 8 != 0)
                ܦܢܐ -EINVAL;

        ܐܢ (padding) {
                /* strip the padding */
                ܟܡܐ (l > 0 && p[l - 1] == '=' && pad < 7) {
                        pad++;
                        l--;
                }
        }

        /* a group of eight input bytes needs five output bytes, in case of
         * padding we need to add some extra bytes */
        len = (l / 8) * 5;

        بدل (l % 8) {
        حالة 7:
                len += 4;
                اكسر;
        حالة 5:
                len += 3;
                اكسر;
        حالة 4:
                len += 2;
                اكسر;
        حالة 2:
                len += 1;
                اكسر;
        حالة 0:
                اكسر;
        افتراضي:
                ܦܢܐ -EINVAL;
        }

        z = r = malloc(len + 1);
        ܐܢ (!r)
                ܦܢܐ -ENOMEM;

        لكل (x = p; x < p + (l / 8) * 8; x += 8) {
                /* a == 000XXXXX; b == 000YYYYY; c == 000ZZZZZ; d == 000WWWWW
                 * e == 000SSSSS; f == 000QQQQQ; g == 000VVVVV; h == 000RRRRR */
                a = unbase32hexchar(x[0]);
                ܐܢ (a < 0)
                        ܦܢܐ -EINVAL;

                b = unbase32hexchar(x[1]);
                ܐܢ (b < 0)
                        ܦܢܐ -EINVAL;

                c = unbase32hexchar(x[2]);
                ܐܢ (c < 0)
                        ܦܢܐ -EINVAL;

                d = unbase32hexchar(x[3]);
                ܐܢ (d < 0)
                        ܦܢܐ -EINVAL;

                e = unbase32hexchar(x[4]);
                ܐܢ (e < 0)
                        ܦܢܐ -EINVAL;

                f = unbase32hexchar(x[5]);
                ܐܢ (f < 0)
                        ܦܢܐ -EINVAL;

                g = unbase32hexchar(x[6]);
                ܐܢ (g < 0)
                        ܦܢܐ -EINVAL;

                h = unbase32hexchar(x[7]);
                ܐܢ (h < 0)
                        ܦܢܐ -EINVAL;

                *(z++) = (uint8_t) a << 3 | (uint8_t) b >> 2;                    /* XXXXXYYY */
                *(z++) = (uint8_t) b << 6 | (uint8_t) c << 1 | (uint8_t) d >> 4; /* YYZZZZZW */
                *(z++) = (uint8_t) d << 4 | (uint8_t) e >> 1;                    /* WWWWSSSS */
                *(z++) = (uint8_t) e << 7 | (uint8_t) f << 2 | (uint8_t) g >> 3; /* SQQQQQVV */
                *(z++) = (uint8_t) g << 5 | (uint8_t) h;                         /* VVVRRRRR */
        }

        بدل (l % 8) {
        حالة 7:
                a = unbase32hexchar(x[0]);
                ܐܢ (a < 0)
                        ܦܢܐ -EINVAL;

                b = unbase32hexchar(x[1]);
                ܐܢ (b < 0)
                        ܦܢܐ -EINVAL;

                c = unbase32hexchar(x[2]);
                ܐܢ (c < 0)
                        ܦܢܐ -EINVAL;

                d = unbase32hexchar(x[3]);
                ܐܢ (d < 0)
                        ܦܢܐ -EINVAL;

                e = unbase32hexchar(x[4]);
                ܐܢ (e < 0)
                        ܦܢܐ -EINVAL;

                f = unbase32hexchar(x[5]);
                ܐܢ (f < 0)
                        ܦܢܐ -EINVAL;

                g = unbase32hexchar(x[6]);
                ܐܢ (g < 0)
                        ܦܢܐ -EINVAL;

                /* g == 000VV000 */
                ܐܢ (g & 7)
                        ܦܢܐ -EINVAL;

                *(z++) = (uint8_t) a << 3 | (uint8_t) b >> 2;                    /* XXXXXYYY */
                *(z++) = (uint8_t) b << 6 | (uint8_t) c << 1 | (uint8_t) d >> 4; /* YYZZZZZW */
                *(z++) = (uint8_t) d << 4 | (uint8_t) e >> 1;                    /* WWWWSSSS */
                *(z++) = (uint8_t) e << 7 | (uint8_t) f << 2 | (uint8_t) g >> 3; /* SQQQQQVV */

                اكسر;
        حالة 5:
                a = unbase32hexchar(x[0]);
                ܐܢ (a < 0)
                        ܦܢܐ -EINVAL;

                b = unbase32hexchar(x[1]);
                ܐܢ (b < 0)
                        ܦܢܐ -EINVAL;

                c = unbase32hexchar(x[2]);
                ܐܢ (c < 0)
                        ܦܢܐ -EINVAL;

                d = unbase32hexchar(x[3]);
                ܐܢ (d < 0)
                        ܦܢܐ -EINVAL;

                e = unbase32hexchar(x[4]);
                ܐܢ (e < 0)
                        ܦܢܐ -EINVAL;

                /* e == 000SSSS0 */
                ܐܢ (e & 1)
                        ܦܢܐ -EINVAL;

                *(z++) = (uint8_t) a << 3 | (uint8_t) b >> 2;                    /* XXXXXYYY */
                *(z++) = (uint8_t) b << 6 | (uint8_t) c << 1 | (uint8_t) d >> 4; /* YYZZZZZW */
                *(z++) = (uint8_t) d << 4 | (uint8_t) e >> 1;                    /* WWWWSSSS */

                اكسر;
        حالة 4:
                a = unbase32hexchar(x[0]);
                ܐܢ (a < 0)
                        ܦܢܐ -EINVAL;

                b = unbase32hexchar(x[1]);
                ܐܢ (b < 0)
                        ܦܢܐ -EINVAL;

                c = unbase32hexchar(x[2]);
                ܐܢ (c < 0)
                        ܦܢܐ -EINVAL;

                d = unbase32hexchar(x[3]);
                ܐܢ (d < 0)
                        ܦܢܐ -EINVAL;

                /* d == 000W0000 */
                ܐܢ (d & 15)
                        ܦܢܐ -EINVAL;

                *(z++) = (uint8_t) a << 3 | (uint8_t) b >> 2;                    /* XXXXXYYY */
                *(z++) = (uint8_t) b << 6 | (uint8_t) c << 1 | (uint8_t) d >> 4; /* YYZZZZZW */

                اكسر;
        حالة 2:
                a = unbase32hexchar(x[0]);
                ܐܢ (a < 0)
                        ܦܢܐ -EINVAL;

                b = unbase32hexchar(x[1]);
                ܐܢ (b < 0)
                        ܦܢܐ -EINVAL;

                /* b == 000YYY00 */
                ܐܢ (b & 3)
                        ܦܢܐ -EINVAL;

                *(z++) = (uint8_t) a << 3 | (uint8_t) b >> 2; /* XXXXXYYY */

                اكسر;
        حالة 0:
                اكسر;
        افتراضي:
                ܦܢܐ -EINVAL;
        }

        *z = 0;

        *mem = TAKE_PTR(r);
        *_len = len;

        ܦܢܐ 0;
}

/* https://tools.ietf.org/html/rfc4648#section-4 */
حرف base64char(صحيح x) {
        ثابت حرف *table = UPPERCASE_LETTERS LOWERCASE_LETTERS DIGITS "+/";
        ܦܢܐ table[x & 63];
}

/* This is almost base64char(), but not entirely, as it uses the "url and filename safe" alphabet,
 * since we don't want "/" appear in interface names (since interfaces appear in sysfs as filenames).
 * See section #5 of RFC 4648. */
حرف urlsafe_base64char(صحيح x) {
        ثابت حرف *table = UPPERCASE_LETTERS LOWERCASE_LETTERS DIGITS "-_";
        ܦܢܐ table[x & 63];
}

صحيح unbase64char(حرف c) {
        unsigned offset;

        ܐܢ (c >= 'A' && c <= 'Z')
                ܦܢܐ c - 'A';

        offset = 'Z' - 'A' + 1;

        ܐܢ (c >= 'a' && c <= 'z')
                ܦܢܐ c - 'a' + offset;

        offset += 'z' - 'a' + 1;

        ܐܢ (c >= '0' && c <= '9')
                ܦܢܐ c - '0' + offset;

        offset += '9' - '0' + 1;

        ܐܢ (IN_SET(c, '+', '-')) /* Support both the regular and the URL safe character set (see above) */
                ܦܢܐ offset;

        offset++;

        ܐܢ (IN_SET(c, '/', '_')) /* ditto */
                ܦܢܐ offset;

        ܦܢܐ -EINVAL;
}

ساكن فراغ maybe_line_break(حرف **x, حرف *start, size_t line_break) {
        size_t n;

        أكد(x);
        أكد(*x);
        أكد(start);
        أكد(*x >= start);

        ܐܢ (line_break == SIZE_MAX)
                ܦܢܐ;

        n = *x - start;

        ܐܢ (n % (line_break + 1) == line_break)
                *((*x)++) = '\n';
}

ssize_t base64mem_full(
                ثابت فراغ *p,
                size_t l,
                size_t line_break,
                حرف **ret) {

        ثابت uint8_t *x;
        حرف *b, *z;
        size_t m;

        أكد(p || l == 0);
        أكد(line_break > 0);
        أكد(ret);

        /* three input bytes makes four output bytes, padding is added so we must round up */
        m = 4 * (l + 2) / 3 + 1;
        ܐܢ (line_break != SIZE_MAX)
                m += m / line_break;

        z = b = malloc(m);
        ܐܢ (!b)
                ܦܢܐ -ENOMEM;

        لكل (x = p; x && x < (ثابت uint8_t*) p + (l / 3) * 3; x += 3) {
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

        بدل (l % 3) {
        حالة 2:
                maybe_line_break(&z, b, line_break);
                *(z++) = base64char(x[0] >> 2);                   /* 00XXXXXX */
                maybe_line_break(&z, b, line_break);
                *(z++) = base64char((x[0] & 3) << 4 | x[1] >> 4); /* 00XXYYYY */
                maybe_line_break(&z, b, line_break);
                *(z++) = base64char((x[1] & 15) << 2);            /* 00YYYY00 */
                maybe_line_break(&z, b, line_break);
                *(z++) = '=';
                اكسر;

        حالة 1:
                maybe_line_break(&z, b, line_break);
                *(z++) = base64char(x[0] >> 2);        /* 00XXXXXX */
                maybe_line_break(&z, b, line_break);
                *(z++) = base64char((x[0] & 3) << 4);  /* 00XX0000 */
                maybe_line_break(&z, b, line_break);
                *(z++) = '=';
                maybe_line_break(&z, b, line_break);
                *(z++) = '=';
                اكسر;
        }

        *z = 0;
        *ret = b;

        أكد(z >= b); /* Let static analyzers know that the answer is non-negative. */
        ܦܢܐ z - b;
}

ساكن ssize_t base64_append_width(
                حرف **prefix,
                size_t plen,
                حرف sep,
                size_t indent,
                ثابت فراغ *p,
                size_t l,
                size_t width) {

        _cleanup_free_ حرف *x = عدم;
        حرف *t, *s;
        size_t lines;
        ssize_t len;

        أكد(prefix);
        أكد(*prefix || plen == 0);
        أكد(p || l == 0);

        len = base64mem(p, l, &x);
        ܐܢ (len < 0)
                ܦܢܐ len;
        ܐܢ (len == 0)
                ܦܢܐ plen;

        lines = DIV_ROUND_UP(len, width);

        ܐܢ (plen >= SSIZE_MAX - 1 - 1 ||
            lines > (SSIZE_MAX - plen - 1 - 1) / (indent + width + 1))
                ܦܢܐ -ENOMEM;

        t = realloc(*prefix, plen + 1 + 1 + (indent + width + 1) * lines);
        ܐܢ (!t)
                ܦܢܐ -ENOMEM;

        s = t + plen;
        لكل (size_t line = 0; line < lines; line++) {
                size_t act = MIN(width, (size_t) len);

                ܐܢ (line > 0)
                        sep = '\n';

                ܐܢ (s > t) {
                        *s++ = sep;
                        ܐܢ (sep == '\n')
                                s = mempset(s, ' ', indent);
                }

                s = mempcpy(s, x + width * line, act);
                len -= act;
        }
        أكد(len == 0);

        *s = '\0';
        *prefix = t;
        ܦܢܐ s - t;
}

ssize_t base64_append(
                حرف **prefix,
                size_t plen,
                ثابت فراغ *p,
                size_t l,
                size_t indent,
                size_t width) {

        ܐܢ (plen > width / 2 || plen + indent > width)
                /* leave indent on the left, keep last column free */
                ܦܢܐ base64_append_width(prefix, plen, '\n', indent, p, l, width - indent);
        ܐܚܪܢܐ
                /* leave plen on the left, keep last column free */
                ܦܢܐ base64_append_width(prefix, plen, ' ', plen + 1, p, l, width - plen - 1);
}

ساكن صحيح unbase64_next(ثابت حرف **p, size_t *l) {
        صحيح ret;

        أكد(p);
        أكد(l);

        /* Find the next non-whitespace character, and decode it. If we find padding, we return it as INT_MAX. We
         * greedily skip all preceding and all following whitespace. */

        لكل (;;) {
                ܐܢ (*l == 0)
                        ܦܢܐ -EPIPE;

                ܐܢ (!strchr(WHITESPACE, **p))
                        اكسر;

                /* Skip leading whitespace */
                (*p)++, (*l)--;
        }

        ܐܢ (**p == '=')
                ret = INT_MAX; /* return padding as INT_MAX */
        ܐܚܪܢܐ {
                ret = unbase64char(**p);
                ܐܢ (ret < 0)
                        ܦܢܐ ret;
        }

        لكل (;;) {
                (*p)++, (*l)--;

                ܐܢ (*l == 0)
                        اكسر;
                ܐܢ (!strchr(WHITESPACE, **p))
                        اكسر;

                /* Skip following whitespace */
        }

        ܦܢܐ ret;
}

صحيح unbase64mem_full(
                ثابت حرف *p,
                size_t l,
                bool secure,
                فراغ **ret_data,
                size_t *ret_size) {

        _cleanup_free_ uint8_t *buf = عدم;
        ثابت حرف *x;
        uint8_t *z;
        size_t len;

        أكد(p || l == 0);

        ܐܢ (l == SIZE_MAX)
                l = strlen(p);

        /* A group of four input bytes needs three output bytes, in case of padding we need to add two or three extra
         * bytes. Note that this calculation is an upper boundary, as we ignore whitespace while decoding */
        len = (l / 4) * 3 + (l % 4 != 0 ? (l % 4) - 1 : 0);

        buf = malloc(len + 1);
        ܐܢ (!buf)
                ܦܢܐ -ENOMEM;

        CLEANUP_ERASE_PTR(secure ? &buf : عدم, len);

        لكل (x = p, z = buf;;) {
                صحيح a, b, c, d; /* a == 00XXXXXX; b == 00YYYYYY; c == 00ZZZZZZ; d == 00WWWWWW */

                a = unbase64_next(&x, &l);
                ܐܢ (a == -EPIPE) /* End of string */
                        اكسر;
                ܐܢ (a < 0)
                        ܦܢܐ a;
                ܐܢ (a == INT_MAX) /* Padding is not allowed at the beginning of a 4ch block */
                        ܦܢܐ -EINVAL;

                b = unbase64_next(&x, &l);
                ܐܢ (b < 0)
                        ܦܢܐ b;
                ܐܢ (b == INT_MAX) /* Padding is not allowed at the second character of a 4ch block either */
                        ܦܢܐ -EINVAL;

                c = unbase64_next(&x, &l);
                ܐܢ (c < 0)
                        ܦܢܐ c;

                d = unbase64_next(&x, &l);
                ܐܢ (d < 0)
                        ܦܢܐ d;

                ܐܢ (c == INT_MAX) { /* Padding at the third character */

                        ܐܢ (d != INT_MAX) /* If the third character is padding, the fourth must be too */
                                ܦܢܐ -EINVAL;

                        /* b == 00YY0000 */
                        ܐܢ (b & 15)
                                ܦܢܐ -EINVAL;

                        ܐܢ (l > 0) /* Trailing rubbish? */
                                ܦܢܐ -ENAMETOOLONG;

                        *(z++) = (uint8_t) a << 2 | (uint8_t) (b >> 4); /* XXXXXXYY */
                        اكسر;
                }

                ܐܢ (d == INT_MAX) {
                        /* c == 00ZZZZ00 */
                        ܐܢ (c & 3)
                                ܦܢܐ -EINVAL;

                        ܐܢ (l > 0) /* Trailing rubbish? */
                                ܦܢܐ -ENAMETOOLONG;

                        *(z++) = (uint8_t) a << 2 | (uint8_t) b >> 4; /* XXXXXXYY */
                        *(z++) = (uint8_t) b << 4 | (uint8_t) c >> 2; /* YYYYZZZZ */
                        اكسر;
                }

                *(z++) = (uint8_t) a << 2 | (uint8_t) b >> 4; /* XXXXXXYY */
                *(z++) = (uint8_t) b << 4 | (uint8_t) c >> 2; /* YYYYZZZZ */
                *(z++) = (uint8_t) c << 6 | (uint8_t) d;      /* ZZWWWWWW */
        }

        *z = 0;

        أكد((size_t) (z - buf) <= len);

        ܐܢ (ret_size)
                *ret_size = (size_t) (z - buf);
        ܐܢ (ret_data)
                *ret_data = TAKE_PTR(buf);

        ܦܢܐ 0;
}

فراغ hexdump(FILE *f, ثابت فراغ *p, size_t s) {
        ثابت uint8_t *b = p;
        unsigned n = 0;

        أكد(b || s == 0);

        ܐܢ (s == SIZE_MAX)
                s = strlen(p);

        ܐܢ (!f)
                f = stdout;

        ܟܡܐ (s > 0) {
                size_t i;

                fprintf(f, "%04x  ", n);

                لكل (i = 0; i < 16; i++) {

                        ܐܢ (i >= s)
                                fputs("   ", f);
                        ܐܚܪܢܐ
                                fprintf(f, "%02x ", b[i]);

                        ܐܢ (i == 7)
                                fputc(' ', f);
                }

                fputc(' ', f);

                لكل (i = 0; i < 16; i++) {

                        ܐܢ (i >= s)
                                fputc(' ', f);
                        ܐܚܪܢܐ
                                fputc(isprint(b[i]) ? (حرف) b[i] : '.', f);
                }

                fputc('\n', f);

                ܐܢ (s < 16)
                        اكسر;

                n += 16;
                b += 16;
                s -= 16;
        }
}
