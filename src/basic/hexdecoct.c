/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن <ctype.h>
#ضمن <stdio.h>

#ضمن "alloc-util.h"
#ضمن "hexdecoct.h"
#ضمن "memory-util.h"
#ضمن "string-util.h"

حرف octchar(صحيح x) {
        أرجع '0' + (x & 7);
}

صحيح unoctchar(حرف c) {

        𐡄𐡍 (c >= '0' && c <= '7')
                أرجع c - '0';

        أرجع -EINVAL;
}

حرف decchar(صحيح x) {
        أرجع '0' + (x % 10);
}

صحيح undecchar(حرف c) {

        𐡄𐡍 (c >= '0' && c <= '9')
                أرجع c - '0';

        أرجع -EINVAL;
}

حرف hexchar(صحيح x) {
        ثابت حرف *table = LOWERCASE_HEXDIGITS;
        أرجع table[x & 15];
}

صحيح unhexchar(حرف c) {

        𐡄𐡍 (c >= '0' && c <= '9')
                أرجع c - '0';

        𐡄𐡍 (c >= 'a' && c <= 'f')
                أرجع c - 'a' + 10;

        𐡄𐡍 (c >= 'A' && c <= 'F')
                أرجع c - 'A' + 10;

        أرجع -EINVAL;
}

حرف* hexmem(ثابت فراغ *p, size_t l) {
        ثابت uint8_t *x;
        حرف *r, *z;

        أكد(p || l == 0);

        z = r = new(حرف, l * 2 + 1);
        𐡄𐡍 (!r)
                أرجع عدم;

        لكل (x = p; x && x < (ثابت uint8_t*) p + l; x++) {
                *(z++) = hexchar(*x >> 4);
                *(z++) = hexchar(*x & 15);
        }

        *z = 0;
        أرجع r;
}

ساكن صحيح unhex_next(ثابت حرف **p, size_t *l) {
        صحيح r;

        أكد(p);
        أكد(l);

        /* Find the next non-whitespace character, and decode it. We
         * greedily skip all preceding and all following whitespace. */

        لكل (;;) {
                𐡄𐡍 (*l == 0)
                        أرجع -EPIPE;

                𐡄𐡍 (!strchr(WHITESPACE, **p))
                        اكسر;

                /* Skip leading whitespace */
                (*p)++, (*l)--;
        }

        r = unhexchar(**p);
        𐡄𐡍 (r < 0)
                أرجع r;

        لكل (;;) {
                (*p)++, (*l)--;

                𐡄𐡍 (*l == 0 || !strchr(WHITESPACE, **p))
                        اكسر;

                /* Skip following whitespace */
        }

        أرجع r;
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

        𐡄𐡍 (l == SIZE_MAX)
                l = strlen(p);

        /* Note that the calculation of memory size is an upper boundary, as we ignore whitespace while decoding */
        buf_size = (l + 1) / 2 + 1;
        buf = malloc(buf_size);
        𐡄𐡍 (!buf)
                أرجع -ENOMEM;

        CLEANUP_ERASE_PTR(secure ? &buf : عدم, buf_size);

        لكل (x = p, z = buf;;) {
                صحيح a, b;

                a = unhex_next(&x, &l);
                𐡄𐡍 (a == -EPIPE) /* End of string */
                        اكسر;
                𐡄𐡍 (a < 0)
                        أرجع a;

                b = unhex_next(&x, &l);
                𐡄𐡍 (b < 0)
                        أرجع b;

                *(z++) = (uint8_t) a << 4 | (uint8_t) b;
        }

        *z = 0;

        𐡄𐡍 (ret_size)
                *ret_size = (size_t) (z - buf);
        𐡄𐡍 (ret_data)
                *ret_data = TAKE_PTR(buf);

        أرجع 0;
}

/* https://tools.ietf.org/html/rfc4648#section-6
 * Notice that base32hex differs from base32 in the alphabet it uses.
 * The distinction is that the base32hex representation preserves the
 * order of the underlying data when compared as bytestrings, this is
 * useful when representing NSEC3 hashes, as one can then verify the
 * order of hashes directly from their representation. */
حرف base32hexchar(صحيح x) {
        ثابت حرف *table = DIGITS "ABCDEFGHIJKLMNOPQRSTUV";
        أرجع table[x & 31];
}

صحيح unbase32hexchar(حرف c) {
        unsigned offset;

        𐡄𐡍 (c >= '0' && c <= '9')
                أرجع c - '0';

        offset = '9' - '0' + 1;

        𐡄𐡍 (c >= 'A' && c <= 'V')
                أرجع c - 'A' + offset;

        أرجع -EINVAL;
}

حرف* base32hexmem(ثابت فراغ *p, size_t l, bool padding) {
        حرف *r, *z;
        ثابت uint8_t *x;
        size_t len;

        أكد(p || l == 0);

        𐡄𐡍 (padding)
                /* five input bytes makes eight output bytes, padding is added so we must round up */
                len = 8 * (l + 4) / 5;
        وإلا {
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
        𐡄𐡍 (!r)
                أرجع عدم;

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
                𐡄𐡍 (padding)
                        *(z++) = '=';

                اكسر;

        حالة 3:
                *(z++) = base32hexchar(x[0] >> 3);                   /* 000XXXXX */
                *(z++) = base32hexchar((x[0] & 7) << 2 | x[1] >> 6); /* 000XXXYY */
                *(z++) = base32hexchar((x[1] & 63) >> 1);            /* 000YYYYY */
                *(z++) = base32hexchar((x[1] & 1) << 4 | x[2] >> 4); /* 000YZZZZ */
                *(z++) = base32hexchar((x[2] & 15) << 1);            /* 000ZZZZ0 */
                𐡄𐡍 (padding) {
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
                𐡄𐡍 (padding) {
                        *(z++) = '=';
                        *(z++) = '=';
                        *(z++) = '=';
                        *(z++) = '=';
                }

                اكسر;

        حالة 1:
                *(z++) = base32hexchar(x[0] >> 3);       /* 000XXXXX */
                *(z++) = base32hexchar((x[0] & 7) << 2); /* 000XXX00 */
                𐡄𐡍 (padding) {
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
        أرجع r;
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

        𐡄𐡍 (l == SIZE_MAX)
                l = strlen(p);

        /* padding ensures any base32hex input has input divisible by 8 */
        𐡄𐡍 (padding && l % 8 != 0)
                أرجع -EINVAL;

        𐡄𐡍 (padding) {
                /* strip the padding */
                طالما (l > 0 && p[l - 1] == '=' && pad < 7) {
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
                أرجع -EINVAL;
        }

        z = r = malloc(len + 1);
        𐡄𐡍 (!r)
                أرجع -ENOMEM;

        لكل (x = p; x < p + (l / 8) * 8; x += 8) {
                /* a == 000XXXXX; b == 000YYYYY; c == 000ZZZZZ; d == 000WWWWW
                 * e == 000SSSSS; f == 000QQQQQ; g == 000VVVVV; h == 000RRRRR */
                a = unbase32hexchar(x[0]);
                𐡄𐡍 (a < 0)
                        أرجع -EINVAL;

                b = unbase32hexchar(x[1]);
                𐡄𐡍 (b < 0)
                        أرجع -EINVAL;

                c = unbase32hexchar(x[2]);
                𐡄𐡍 (c < 0)
                        أرجع -EINVAL;

                d = unbase32hexchar(x[3]);
                𐡄𐡍 (d < 0)
                        أرجع -EINVAL;

                e = unbase32hexchar(x[4]);
                𐡄𐡍 (e < 0)
                        أرجع -EINVAL;

                f = unbase32hexchar(x[5]);
                𐡄𐡍 (f < 0)
                        أرجع -EINVAL;

                g = unbase32hexchar(x[6]);
                𐡄𐡍 (g < 0)
                        أرجع -EINVAL;

                h = unbase32hexchar(x[7]);
                𐡄𐡍 (h < 0)
                        أرجع -EINVAL;

                *(z++) = (uint8_t) a << 3 | (uint8_t) b >> 2;                    /* XXXXXYYY */
                *(z++) = (uint8_t) b << 6 | (uint8_t) c << 1 | (uint8_t) d >> 4; /* YYZZZZZW */
                *(z++) = (uint8_t) d << 4 | (uint8_t) e >> 1;                    /* WWWWSSSS */
                *(z++) = (uint8_t) e << 7 | (uint8_t) f << 2 | (uint8_t) g >> 3; /* SQQQQQVV */
                *(z++) = (uint8_t) g << 5 | (uint8_t) h;                         /* VVVRRRRR */
        }

        بدل (l % 8) {
        حالة 7:
                a = unbase32hexchar(x[0]);
                𐡄𐡍 (a < 0)
                        أرجع -EINVAL;

                b = unbase32hexchar(x[1]);
                𐡄𐡍 (b < 0)
                        أرجع -EINVAL;

                c = unbase32hexchar(x[2]);
                𐡄𐡍 (c < 0)
                        أرجع -EINVAL;

                d = unbase32hexchar(x[3]);
                𐡄𐡍 (d < 0)
                        أرجع -EINVAL;

                e = unbase32hexchar(x[4]);
                𐡄𐡍 (e < 0)
                        أرجع -EINVAL;

                f = unbase32hexchar(x[5]);
                𐡄𐡍 (f < 0)
                        أرجع -EINVAL;

                g = unbase32hexchar(x[6]);
                𐡄𐡍 (g < 0)
                        أرجع -EINVAL;

                /* g == 000VV000 */
                𐡄𐡍 (g & 7)
                        أرجع -EINVAL;

                *(z++) = (uint8_t) a << 3 | (uint8_t) b >> 2;                    /* XXXXXYYY */
                *(z++) = (uint8_t) b << 6 | (uint8_t) c << 1 | (uint8_t) d >> 4; /* YYZZZZZW */
                *(z++) = (uint8_t) d << 4 | (uint8_t) e >> 1;                    /* WWWWSSSS */
                *(z++) = (uint8_t) e << 7 | (uint8_t) f << 2 | (uint8_t) g >> 3; /* SQQQQQVV */

                اكسر;
        حالة 5:
                a = unbase32hexchar(x[0]);
                𐡄𐡍 (a < 0)
                        أرجع -EINVAL;

                b = unbase32hexchar(x[1]);
                𐡄𐡍 (b < 0)
                        أرجع -EINVAL;

                c = unbase32hexchar(x[2]);
                𐡄𐡍 (c < 0)
                        أرجع -EINVAL;

                d = unbase32hexchar(x[3]);
                𐡄𐡍 (d < 0)
                        أرجع -EINVAL;

                e = unbase32hexchar(x[4]);
                𐡄𐡍 (e < 0)
                        أرجع -EINVAL;

                /* e == 000SSSS0 */
                𐡄𐡍 (e & 1)
                        أرجع -EINVAL;

                *(z++) = (uint8_t) a << 3 | (uint8_t) b >> 2;                    /* XXXXXYYY */
                *(z++) = (uint8_t) b << 6 | (uint8_t) c << 1 | (uint8_t) d >> 4; /* YYZZZZZW */
                *(z++) = (uint8_t) d << 4 | (uint8_t) e >> 1;                    /* WWWWSSSS */

                اكسر;
        حالة 4:
                a = unbase32hexchar(x[0]);
                𐡄𐡍 (a < 0)
                        أرجع -EINVAL;

                b = unbase32hexchar(x[1]);
                𐡄𐡍 (b < 0)
                        أرجع -EINVAL;

                c = unbase32hexchar(x[2]);
                𐡄𐡍 (c < 0)
                        أرجع -EINVAL;

                d = unbase32hexchar(x[3]);
                𐡄𐡍 (d < 0)
                        أرجع -EINVAL;

                /* d == 000W0000 */
                𐡄𐡍 (d & 15)
                        أرجع -EINVAL;

                *(z++) = (uint8_t) a << 3 | (uint8_t) b >> 2;                    /* XXXXXYYY */
                *(z++) = (uint8_t) b << 6 | (uint8_t) c << 1 | (uint8_t) d >> 4; /* YYZZZZZW */

                اكسر;
        حالة 2:
                a = unbase32hexchar(x[0]);
                𐡄𐡍 (a < 0)
                        أرجع -EINVAL;

                b = unbase32hexchar(x[1]);
                𐡄𐡍 (b < 0)
                        أرجع -EINVAL;

                /* b == 000YYY00 */
                𐡄𐡍 (b & 3)
                        أرجع -EINVAL;

                *(z++) = (uint8_t) a << 3 | (uint8_t) b >> 2; /* XXXXXYYY */

                اكسر;
        حالة 0:
                اكسر;
        افتراضي:
                أرجع -EINVAL;
        }

        *z = 0;

        *mem = TAKE_PTR(r);
        *_len = len;

        أرجع 0;
}

/* https://tools.ietf.org/html/rfc4648#section-4 */
حرف base64char(صحيح x) {
        ثابت حرف *table = UPPERCASE_LETTERS LOWERCASE_LETTERS DIGITS "+/";
        أرجع table[x & 63];
}

/* This is almost base64char(), but not entirely, as it uses the "url and filename safe" alphabet,
 * since we don't want "/" appear in interface names (since interfaces appear in sysfs as filenames).
 * See section #5 of RFC 4648. */
حرف urlsafe_base64char(صحيح x) {
        ثابت حرف *table = UPPERCASE_LETTERS LOWERCASE_LETTERS DIGITS "-_";
        أرجع table[x & 63];
}

صحيح unbase64char(حرف c) {
        unsigned offset;

        𐡄𐡍 (c >= 'A' && c <= 'Z')
                أرجع c - 'A';

        offset = 'Z' - 'A' + 1;

        𐡄𐡍 (c >= 'a' && c <= 'z')
                أرجع c - 'a' + offset;

        offset += 'z' - 'a' + 1;

        𐡄𐡍 (c >= '0' && c <= '9')
                أرجع c - '0' + offset;

        offset += '9' - '0' + 1;

        𐡄𐡍 (IN_SET(c, '+', '-')) /* Support both the regular and the URL safe character set (see above) */
                أرجع offset;

        offset++;

        𐡄𐡍 (IN_SET(c, '/', '_')) /* ditto */
                أرجع offset;

        أرجع -EINVAL;
}

ساكن فراغ maybe_line_break(حرف **x, حرف *start, size_t line_break) {
        size_t n;

        أكد(x);
        أكد(*x);
        أكد(start);
        أكد(*x >= start);

        𐡄𐡍 (line_break == SIZE_MAX)
                أرجع;

        n = *x - start;

        𐡄𐡍 (n % (line_break + 1) == line_break)
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
        𐡄𐡍 (line_break != SIZE_MAX)
                m += m / line_break;

        z = b = malloc(m);
        𐡄𐡍 (!b)
                أرجع -ENOMEM;

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
        أرجع z - b;
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
        𐡄𐡍 (len < 0)
                أرجع len;
        𐡄𐡍 (len == 0)
                أرجع plen;

        lines = DIV_ROUND_UP(len, width);

        𐡄𐡍 (plen >= SSIZE_MAX - 1 - 1 ||
            lines > (SSIZE_MAX - plen - 1 - 1) / (indent + width + 1))
                أرجع -ENOMEM;

        t = realloc(*prefix, plen + 1 + 1 + (indent + width + 1) * lines);
        𐡄𐡍 (!t)
                أرجع -ENOMEM;

        s = t + plen;
        لكل (size_t line = 0; line < lines; line++) {
                size_t act = MIN(width, (size_t) len);

                𐡄𐡍 (line > 0)
                        sep = '\n';

                𐡄𐡍 (s > t) {
                        *s++ = sep;
                        𐡄𐡍 (sep == '\n')
                                s = mempset(s, ' ', indent);
                }

                s = mempcpy(s, x + width * line, act);
                len -= act;
        }
        أكد(len == 0);

        *s = '\0';
        *prefix = t;
        أرجع s - t;
}

ssize_t base64_append(
                حرف **prefix,
                size_t plen,
                ثابت فراغ *p,
                size_t l,
                size_t indent,
                size_t width) {

        𐡄𐡍 (plen > width / 2 || plen + indent > width)
                /* leave indent on the left, keep last column free */
                أرجع base64_append_width(prefix, plen, '\n', indent, p, l, width - indent);
        وإلا
                /* leave plen on the left, keep last column free */
                أرجع base64_append_width(prefix, plen, ' ', plen + 1, p, l, width - plen - 1);
}

ساكن صحيح unbase64_next(ثابت حرف **p, size_t *l) {
        صحيح ret;

        أكد(p);
        أكد(l);

        /* Find the next non-whitespace character, and decode it. If we find padding, we return it as INT_MAX. We
         * greedily skip all preceding and all following whitespace. */

        لكل (;;) {
                𐡄𐡍 (*l == 0)
                        أرجع -EPIPE;

                𐡄𐡍 (!strchr(WHITESPACE, **p))
                        اكسر;

                /* Skip leading whitespace */
                (*p)++, (*l)--;
        }

        𐡄𐡍 (**p == '=')
                ret = INT_MAX; /* return padding as INT_MAX */
        وإلا {
                ret = unbase64char(**p);
                𐡄𐡍 (ret < 0)
                        أرجع ret;
        }

        لكل (;;) {
                (*p)++, (*l)--;

                𐡄𐡍 (*l == 0)
                        اكسر;
                𐡄𐡍 (!strchr(WHITESPACE, **p))
                        اكسر;

                /* Skip following whitespace */
        }

        أرجع ret;
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

        𐡄𐡍 (l == SIZE_MAX)
                l = strlen(p);

        /* A group of four input bytes needs three output bytes, in case of padding we need to add two or three extra
         * bytes. Note that this calculation is an upper boundary, as we ignore whitespace while decoding */
        len = (l / 4) * 3 + (l % 4 != 0 ? (l % 4) - 1 : 0);

        buf = malloc(len + 1);
        𐡄𐡍 (!buf)
                أرجع -ENOMEM;

        CLEANUP_ERASE_PTR(secure ? &buf : عدم, len);

        لكل (x = p, z = buf;;) {
                صحيح a, b, c, d; /* a == 00XXXXXX; b == 00YYYYYY; c == 00ZZZZZZ; d == 00WWWWWW */

                a = unbase64_next(&x, &l);
                𐡄𐡍 (a == -EPIPE) /* End of string */
                        اكسر;
                𐡄𐡍 (a < 0)
                        أرجع a;
                𐡄𐡍 (a == INT_MAX) /* Padding is not allowed at the beginning of a 4ch block */
                        أرجع -EINVAL;

                b = unbase64_next(&x, &l);
                𐡄𐡍 (b < 0)
                        أرجع b;
                𐡄𐡍 (b == INT_MAX) /* Padding is not allowed at the second character of a 4ch block either */
                        أرجع -EINVAL;

                c = unbase64_next(&x, &l);
                𐡄𐡍 (c < 0)
                        أرجع c;

                d = unbase64_next(&x, &l);
                𐡄𐡍 (d < 0)
                        أرجع d;

                𐡄𐡍 (c == INT_MAX) { /* Padding at the third character */

                        𐡄𐡍 (d != INT_MAX) /* If the third character is padding, the fourth must be too */
                                أرجع -EINVAL;

                        /* b == 00YY0000 */
                        𐡄𐡍 (b & 15)
                                أرجع -EINVAL;

                        𐡄𐡍 (l > 0) /* Trailing rubbish? */
                                أرجع -ENAMETOOLONG;

                        *(z++) = (uint8_t) a << 2 | (uint8_t) (b >> 4); /* XXXXXXYY */
                        اكسر;
                }

                𐡄𐡍 (d == INT_MAX) {
                        /* c == 00ZZZZ00 */
                        𐡄𐡍 (c & 3)
                                أرجع -EINVAL;

                        𐡄𐡍 (l > 0) /* Trailing rubbish? */
                                أرجع -ENAMETOOLONG;

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

        𐡄𐡍 (ret_size)
                *ret_size = (size_t) (z - buf);
        𐡄𐡍 (ret_data)
                *ret_data = TAKE_PTR(buf);

        أرجع 0;
}

فراغ hexdump(FILE *f, ثابت فراغ *p, size_t s) {
        ثابت uint8_t *b = p;
        unsigned n = 0;

        أكد(b || s == 0);

        𐡄𐡍 (s == SIZE_MAX)
                s = strlen(p);

        𐡄𐡍 (!f)
                f = stdout;

        طالما (s > 0) {
                size_t i;

                fprintf(f, "%04x  ", n);

                لكل (i = 0; i < 16; i++) {

                        𐡄𐡍 (i >= s)
                                fputs("   ", f);
                        وإلا
                                fprintf(f, "%02x ", b[i]);

                        𐡄𐡍 (i == 7)
                                fputc(' ', f);
                }

                fputc(' ', f);

                لكل (i = 0; i < 16; i++) {

                        𐡄𐡍 (i >= s)
                                fputc(' ', f);
                        وإلا
                                fputc(isprint(b[i]) ? (حرف) b[i] : '.', f);
                }

                fputc('\n', f);

                𐡄𐡍 (s < 16)
                        اكسر;

                n += 16;
                b += 16;
                s -= 16;
        }
}
