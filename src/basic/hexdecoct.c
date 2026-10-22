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

        إذا (c >= '0' && c <= '7')
                أرجع c - '0';

        أرجع -EINVAL;
}

حرف decchar(صحيح x) {
        أرجع '0' + (x % 10);
}

صحيح undecchar(حرف c) {

        إذا (c >= '0' && c <= '9')
                أرجع c - '0';

        أرجع -EINVAL;
}

حرف hexchar(صحيح x) {
        ثابت حرف *جدول = LOWERCASE_HEXDIGITS;
        أرجع جدول[x & 15];
}

صحيح unhexchar(حرف c) {

        إذا (c >= '0' && c <= '9')
                أرجع c - '0';

        إذا (c >= 'a' && c <= 'f')
                أرجع c - 'a' + 10;

        إذا (c >= 'A' && c <= 'F')
                أرجع c - 'A' + 10;

        أرجع -EINVAL;
}

حرف* hexmem(ثابت فراغ *p, size_t l) {
        ثابت uint8_t *x;
        حرف *r, *z;

        أكد(p || l == 0);

        z = r = new(حرف, l * 2 + 1);
        إذا (!r)
                أرجع عدم;

        لكل (x = p; x && x < (ثابت uint8_t*) p + l; x++) {
                *(z++) = hexchar(*x >> 4);
                *(z++) = hexchar(*x & 15);
        }

        *z = 0;
        أرجع r;
}

ساكن صحيح unhex_بل(ثابت حرف **p, size_t *l) {
        صحيح r;

        أكد(p);
        أكد(l);

        /* Find the next non-whitespace character, and decode it. We
         * greedily skip all preceding and all following whitespace. */

        لكل (;;) {
                إذا (*l == 0)
                        أرجع -EPIPE;

                إذا (!strchr(WHITESPACE, **p))
                        اكسر;

                /* Skip leading whitespace */
                (*p)++, (*l)--;
        }

        r = unhexchar(**p);
        إذا (r < 0)
                أرجع r;

        لكل (;;) {
                (*p)++, (*l)--;

                إذا (*l == 0 || !strchr(WHITESPACE, **p))
                        اكسر;

                /* Skip following whitespace */
        }

        أرجع r;
}

صحيح unhexmem_ډک(
                ثابت حرف *p,
                size_t l,
                bool secure,
                فراغ **ret_معلومات,
                size_t *ret_اندازه) {

        _cleanup_free_ uint8_t *buf = عدم;
        size_t buf_size;
        ثابت حرف *x;
        uint8_t *z;

        أكد(p || l == 0);

        إذا (l == SIZE_MAX)
                l = مزي_اوږدوالی(p);

        /* Note that the calculation of memory size is an upper boundary, as we ignore whitespace while decoding */
        buf_size = (l + 1) / 2 + 1;
        buf = حافظه_ورکړه(buf_size);
        إذا (!buf)
                أرجع -ENOMEM;

        CLEANUP_ERASE_PTR(secure ? &buf : عدم, buf_size);

        لكل (x = p, z = buf;;) {
                صحيح a, b;

                a = unhex_بل(&x, &l);
                إذا (a == -EPIPE) /* End of string */
                        اكسر;
                إذا (a < 0)
                        أرجع a;

                b = unhex_بل(&x, &l);
                إذا (b < 0)
                        أرجع b;

                *(z++) = (uint8_t) a << 4 | (uint8_t) b;
        }

        *z = 0;

        إذا (ret_اندازه)
                *ret_اندازه = (size_t) (z - buf);
        إذا (ret_معلومات)
                *ret_معلومات = TAKE_PTR(buf);

        أرجع 0;
}

/* https://tools.ietf.org/html/rfc4648#section-6
 * Notice that base32hex differs from base32 in the alphabet it uses.
 * The distinction is that the base32hex representation preserves the
 * order of the underlying data when compared as bytestrings, this is
 * useful when representing NSEC3 hashes, as one can then verify the
 * order of hashes directly from their representation. */
حرف base32hexchar(صحيح x) {
        ثابت حرف *جدول = DIGITS "ABCDEFGHIJKLMNOPQRSTUV";
        أرجع جدول[x & 31];
}

صحيح unbase32hexchar(حرف c) {
        بینښه دننه_واټن;

        إذا (c >= '0' && c <= '9')
                أرجع c - '0';

        دننه_واټن = '9' - '0' + 1;

        إذا (c >= 'A' && c <= 'V')
                أرجع c - 'A' + دننه_واټن;

        أرجع -EINVAL;
}

حرف* base32hexmem(ثابت فراغ *p, size_t l, bool padding) {
        حرف *r, *z;
        ثابت uint8_t *x;
        size_t len;

        أكد(p || l == 0);

        إذا (padding)
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

        z = r = حافظه_ورکړه(len + 1);
        إذا (!r)
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
                إذا (padding)
                        *(z++) = '=';

                اكسر;

        حالة 3:
                *(z++) = base32hexchar(x[0] >> 3);                   /* 000XXXXX */
                *(z++) = base32hexchar((x[0] & 7) << 2 | x[1] >> 6); /* 000XXXYY */
                *(z++) = base32hexchar((x[1] & 63) >> 1);            /* 000YYYYY */
                *(z++) = base32hexchar((x[1] & 1) << 4 | x[2] >> 4); /* 000YZZZZ */
                *(z++) = base32hexchar((x[2] & 15) << 1);            /* 000ZZZZ0 */
                إذا (padding) {
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
                إذا (padding) {
                        *(z++) = '=';
                        *(z++) = '=';
                        *(z++) = '=';
                        *(z++) = '=';
                }

                اكسر;

        حالة 1:
                *(z++) = base32hexchar(x[0] >> 3);       /* 000XXXXX */
                *(z++) = base32hexchar((x[0] & 7) << 2); /* 000XXX00 */
                إذا (padding) {
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
        بینښه pad = 0;

        أكد(p || l == 0);
        أكد(mem);
        أكد(_len);

        إذا (l == SIZE_MAX)
                l = مزي_اوږدوالی(p);

        /* padding ensures any base32hex input has input divisible by 8 */
        إذا (padding && l % 8 != 0)
                أرجع -EINVAL;

        إذا (padding) {
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

        z = r = حافظه_ورکړه(len + 1);
        إذا (!r)
                أرجع -ENOMEM;

        لكل (x = p; x < p + (l / 8) * 8; x += 8) {
                /* a == 000XXXXX; b == 000YYYYY; c == 000ZZZZZ; d == 000WWWWW
                 * e == 000SSSSS; f == 000QQQQQ; g == 000VVVVV; h == 000RRRRR */
                a = unbase32hexchar(x[0]);
                إذا (a < 0)
                        أرجع -EINVAL;

                b = unbase32hexchar(x[1]);
                إذا (b < 0)
                        أرجع -EINVAL;

                c = unbase32hexchar(x[2]);
                إذا (c < 0)
                        أرجع -EINVAL;

                d = unbase32hexchar(x[3]);
                إذا (d < 0)
                        أرجع -EINVAL;

                e = unbase32hexchar(x[4]);
                إذا (e < 0)
                        أرجع -EINVAL;

                f = unbase32hexchar(x[5]);
                إذا (f < 0)
                        أرجع -EINVAL;

                g = unbase32hexchar(x[6]);
                إذا (g < 0)
                        أرجع -EINVAL;

                h = unbase32hexchar(x[7]);
                إذا (h < 0)
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
                إذا (a < 0)
                        أرجع -EINVAL;

                b = unbase32hexchar(x[1]);
                إذا (b < 0)
                        أرجع -EINVAL;

                c = unbase32hexchar(x[2]);
                إذا (c < 0)
                        أرجع -EINVAL;

                d = unbase32hexchar(x[3]);
                إذا (d < 0)
                        أرجع -EINVAL;

                e = unbase32hexchar(x[4]);
                إذا (e < 0)
                        أرجع -EINVAL;

                f = unbase32hexchar(x[5]);
                إذا (f < 0)
                        أرجع -EINVAL;

                g = unbase32hexchar(x[6]);
                إذا (g < 0)
                        أرجع -EINVAL;

                /* g == 000VV000 */
                إذا (g & 7)
                        أرجع -EINVAL;

                *(z++) = (uint8_t) a << 3 | (uint8_t) b >> 2;                    /* XXXXXYYY */
                *(z++) = (uint8_t) b << 6 | (uint8_t) c << 1 | (uint8_t) d >> 4; /* YYZZZZZW */
                *(z++) = (uint8_t) d << 4 | (uint8_t) e >> 1;                    /* WWWWSSSS */
                *(z++) = (uint8_t) e << 7 | (uint8_t) f << 2 | (uint8_t) g >> 3; /* SQQQQQVV */

                اكسر;
        حالة 5:
                a = unbase32hexchar(x[0]);
                إذا (a < 0)
                        أرجع -EINVAL;

                b = unbase32hexchar(x[1]);
                إذا (b < 0)
                        أرجع -EINVAL;

                c = unbase32hexchar(x[2]);
                إذا (c < 0)
                        أرجع -EINVAL;

                d = unbase32hexchar(x[3]);
                إذا (d < 0)
                        أرجع -EINVAL;

                e = unbase32hexchar(x[4]);
                إذا (e < 0)
                        أرجع -EINVAL;

                /* e == 000SSSS0 */
                إذا (e & 1)
                        أرجع -EINVAL;

                *(z++) = (uint8_t) a << 3 | (uint8_t) b >> 2;                    /* XXXXXYYY */
                *(z++) = (uint8_t) b << 6 | (uint8_t) c << 1 | (uint8_t) d >> 4; /* YYZZZZZW */
                *(z++) = (uint8_t) d << 4 | (uint8_t) e >> 1;                    /* WWWWSSSS */

                اكسر;
        حالة 4:
                a = unbase32hexchar(x[0]);
                إذا (a < 0)
                        أرجع -EINVAL;

                b = unbase32hexchar(x[1]);
                إذا (b < 0)
                        أرجع -EINVAL;

                c = unbase32hexchar(x[2]);
                إذا (c < 0)
                        أرجع -EINVAL;

                d = unbase32hexchar(x[3]);
                إذا (d < 0)
                        أرجع -EINVAL;

                /* d == 000W0000 */
                إذا (d & 15)
                        أرجع -EINVAL;

                *(z++) = (uint8_t) a << 3 | (uint8_t) b >> 2;                    /* XXXXXYYY */
                *(z++) = (uint8_t) b << 6 | (uint8_t) c << 1 | (uint8_t) d >> 4; /* YYZZZZZW */

                اكسر;
        حالة 2:
                a = unbase32hexchar(x[0]);
                إذا (a < 0)
                        أرجع -EINVAL;

                b = unbase32hexchar(x[1]);
                إذا (b < 0)
                        أرجع -EINVAL;

                /* b == 000YYY00 */
                إذا (b & 3)
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
        ثابت حرف *جدول = UPPERCASE_LETTERS LOWERCASE_LETTERS DIGITS "+/";
        أرجع جدول[x & 63];
}

/* This is almost base64char(), but not entirely, as it uses the "url and filename safe" alphabet,
 * since we don't want "/" appear in interface names (since interfaces appear in sysfs as filenames).
 * See section #5 of RFC 4648. */
حرف urlsafe_base64char(صحيح x) {
        ثابت حرف *جدول = UPPERCASE_LETTERS LOWERCASE_LETTERS DIGITS "-_";
        أرجع جدول[x & 63];
}

صحيح unbase64char(حرف c) {
        بینښه دننه_واټن;

        إذا (c >= 'A' && c <= 'Z')
                أرجع c - 'A';

        دننه_واټن = 'Z' - 'A' + 1;

        إذا (c >= 'a' && c <= 'z')
                أرجع c - 'a' + دننه_واټن;

        دننه_واټن += 'z' - 'a' + 1;

        إذا (c >= '0' && c <= '9')
                أرجع c - '0' + دننه_واټن;

        دننه_واټن += '9' - '0' + 1;

        إذا (دننه_سېټ(c, '+', '-')) /* Support both the regular and the URL safe character set (see above) */
                أرجع دننه_واټن;

        دننه_واټن++;

        إذا (دننه_سېټ(c, '/', '_')) /* ditto */
                أرجع دننه_واټن;

        أرجع -EINVAL;
}

ساكن فراغ maybe_کرښه_break(حرف **x, حرف *پیل, size_t کرښه_break) {
        size_t n;

        أكد(x);
        أكد(*x);
        أكد(پیل);
        أكد(*x >= پیل);

        إذا (کرښه_break == SIZE_MAX)
                أرجع;

        n = *x - پیل;

        إذا (n % (کرښه_break + 1) == کرښه_break)
                *((*x)++) = '\n';
}

ssize_t base64mem_ډک(
                ثابت فراغ *p,
                size_t l,
                size_t کرښه_break,
                حرف **ret) {

        ثابت uint8_t *x;
        حرف *b, *z;
        size_t m;

        أكد(p || l == 0);
        أكد(کرښه_break > 0);
        أكد(ret);

        /* three input bytes makes four output bytes, padding is added so we must round up */
        m = 4 * (l + 2) / 3 + 1;
        إذا (کرښه_break != SIZE_MAX)
                m += m / کرښه_break;

        z = b = حافظه_ورکړه(m);
        إذا (!b)
                أرجع -ENOMEM;

        لكل (x = p; x && x < (ثابت uint8_t*) p + (l / 3) * 3; x += 3) {
                /* x[0] == XXXXXXXX; x[1] == YYYYYYYY; x[2] == ZZZZZZZZ */
                maybe_کرښه_break(&z, b, کرښه_break);
                *(z++) = base64char(x[0] >> 2);                    /* 00XXXXXX */
                maybe_کرښه_break(&z, b, کرښه_break);
                *(z++) = base64char((x[0] & 3) << 4 | x[1] >> 4);  /* 00XXYYYY */
                maybe_کرښه_break(&z, b, کرښه_break);
                *(z++) = base64char((x[1] & 15) << 2 | x[2] >> 6); /* 00YYYYZZ */
                maybe_کرښه_break(&z, b, کرښه_break);
                *(z++) = base64char(x[2] & 63);                    /* 00ZZZZZZ */
        }

        بدل (l % 3) {
        حالة 2:
                maybe_کرښه_break(&z, b, کرښه_break);
                *(z++) = base64char(x[0] >> 2);                   /* 00XXXXXX */
                maybe_کرښه_break(&z, b, کرښه_break);
                *(z++) = base64char((x[0] & 3) << 4 | x[1] >> 4); /* 00XXYYYY */
                maybe_کرښه_break(&z, b, کرښه_break);
                *(z++) = base64char((x[1] & 15) << 2);            /* 00YYYY00 */
                maybe_کرښه_break(&z, b, کرښه_break);
                *(z++) = '=';
                اكسر;

        حالة 1:
                maybe_کرښه_break(&z, b, کرښه_break);
                *(z++) = base64char(x[0] >> 2);        /* 00XXXXXX */
                maybe_کرښه_break(&z, b, کرښه_break);
                *(z++) = base64char((x[0] & 3) << 4);  /* 00XX0000 */
                maybe_کرښه_break(&z, b, کرښه_break);
                *(z++) = '=';
                maybe_کرښه_break(&z, b, کرښه_break);
                *(z++) = '=';
                اكسر;
        }

        *z = 0;
        *ret = b;

        أكد(z >= b); /* Let static analyzers know that the answer is non-negative. */
        أرجع z - b;
}

ساكن ssize_t base64_append_پلنوالی(
                حرف **مختاړی,
                size_t plen,
                حرف sep,
                size_t indent,
                ثابت فراغ *p,
                size_t l,
                size_t width) {

        _cleanup_free_ حرف *x = عدم;
        حرف *t, *s;
        size_t کرښې;
        ssize_t len;

        أكد(مختاړی);
        أكد(*مختاړی || plen == 0);
        أكد(p || l == 0);

        len = base64mem(p, l, &x);
        إذا (len < 0)
                أرجع len;
        إذا (len == 0)
                أرجع plen;

        کرښې = DIV_پړاو_پورته(len, width);

        إذا (plen >= SSIZE_MAX - 1 - 1 ||
            کرښې > (SSIZE_MAX - plen - 1 - 1) / (indent + width + 1))
                أرجع -ENOMEM;

        t = realloc(*مختاړی, plen + 1 + 1 + (indent + width + 1) * کرښې);
        إذا (!t)
                أرجع -ENOMEM;

        s = t + plen;
        لكل (size_t کرښه = 0; کرښه < کرښې; کرښه++) {
                size_t act = MIN(width, (size_t) len);

                إذا (کرښه > 0)
                        sep = '\n';

                إذا (s > t) {
                        *s++ = sep;
                        إذا (sep == '\n')
                                s = mempset(s, ' ', indent);
                }

                s = mempcpy(s, x + width * کرښه, act);
                len -= act;
        }
        أكد(len == 0);

        *s = '\0';
        *مختاړی = t;
        أرجع s - t;
}

ssize_t base64_append(
                حرف **مختاړی,
                size_t plen,
                ثابت فراغ *p,
                size_t l,
                size_t indent,
                size_t width) {

        إذا (plen > width / 2 || plen + indent > width)
                /* leave indent on the left, keep last column free */
                أرجع base64_append_پلنوالی(مختاړی, plen, '\n', indent, p, l, width - indent);
        وإلا
                /* leave plen on the left, keep last column free */
                أرجع base64_append_پلنوالی(مختاړی, plen, ' ', plen + 1, p, l, width - plen - 1);
}

ساكن صحيح unbase64_بل(ثابت حرف **p, size_t *l) {
        صحيح ret;

        أكد(p);
        أكد(l);

        /* Find the next non-whitespace character, and decode it. If we find padding, we return it as INT_MAX. We
         * greedily skip all preceding and all following whitespace. */

        لكل (;;) {
                إذا (*l == 0)
                        أرجع -EPIPE;

                إذا (!strchr(WHITESPACE, **p))
                        اكسر;

                /* Skip leading whitespace */
                (*p)++, (*l)--;
        }

        إذا (**p == '=')
                ret = لوی_عدد; /* return padding as INT_MAX */
        وإلا {
                ret = unbase64char(**p);
                إذا (ret < 0)
                        أرجع ret;
        }

        لكل (;;) {
                (*p)++, (*l)--;

                إذا (*l == 0)
                        اكسر;
                إذا (!strchr(WHITESPACE, **p))
                        اكسر;

                /* Skip following whitespace */
        }

        أرجع ret;
}

صحيح unbase64mem_ډک(
                ثابت حرف *p,
                size_t l,
                bool secure,
                فراغ **ret_معلومات,
                size_t *ret_اندازه) {

        _cleanup_free_ uint8_t *buf = عدم;
        ثابت حرف *x;
        uint8_t *z;
        size_t len;

        أكد(p || l == 0);

        إذا (l == SIZE_MAX)
                l = مزي_اوږدوالی(p);

        /* A group of four input bytes needs three output bytes, in case of padding we need to add two or three extra
         * bytes. Note that this calculation is an upper boundary, as we ignore whitespace while decoding */
        len = (l / 4) * 3 + (l % 4 != 0 ? (l % 4) - 1 : 0);

        buf = حافظه_ورکړه(len + 1);
        إذا (!buf)
                أرجع -ENOMEM;

        CLEANUP_ERASE_PTR(secure ? &buf : عدم, len);

        لكل (x = p, z = buf;;) {
                صحيح a, b, c, d; /* a == 00XXXXXX; b == 00YYYYYY; c == 00ZZZZZZ; d == 00WWWWWW */

                a = unbase64_بل(&x, &l);
                إذا (a == -EPIPE) /* End of string */
                        اكسر;
                إذا (a < 0)
                        أرجع a;
                إذا (a == لوی_عدد) /* Padding is not allowed at the beginning of a 4ch block */
                        أرجع -EINVAL;

                b = unbase64_بل(&x, &l);
                إذا (b < 0)
                        أرجع b;
                إذا (b == لوی_عدد) /* Padding is not allowed at the second character of a 4ch block either */
                        أرجع -EINVAL;

                c = unbase64_بل(&x, &l);
                إذا (c < 0)
                        أرجع c;

                d = unbase64_بل(&x, &l);
                إذا (d < 0)
                        أرجع d;

                إذا (c == لوی_عدد) { /* Padding at the third character */

                        إذا (d != لوی_عدد) /* If the third character is padding, the fourth must be too */
                                أرجع -EINVAL;

                        /* b == 00YY0000 */
                        إذا (b & 15)
                                أرجع -EINVAL;

                        إذا (l > 0) /* Trailing rubbish? */
                                أرجع -ENAMETOOLONG;

                        *(z++) = (uint8_t) a << 2 | (uint8_t) (b >> 4); /* XXXXXXYY */
                        اكسر;
                }

                إذا (d == لوی_عدد) {
                        /* c == 00ZZZZ00 */
                        إذا (c & 3)
                                أرجع -EINVAL;

                        إذا (l > 0) /* Trailing rubbish? */
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

        إذا (ret_اندازه)
                *ret_اندازه = (size_t) (z - buf);
        إذا (ret_معلومات)
                *ret_معلومات = TAKE_PTR(buf);

        أرجع 0;
}

فراغ hexdump(FILE *f, ثابت فراغ *p, size_t s) {
        ثابت uint8_t *b = p;
        بینښه n = 0;

        أكد(b || s == 0);

        إذا (s == SIZE_MAX)
                s = مزي_اوږدوالی(p);

        إذا (!f)
                f = stdout;

        طالما (s > 0) {
                size_t i;

                fprintf(f, "%04x  ", n);

                لكل (i = 0; i < 16; i++) {

                        إذا (i >= s)
                                fputs("   ", f);
                        وإلا
                                fprintf(f, "%02x ", b[i]);

                        إذا (i == 7)
                                fputc(' ', f);
                }

                fputc(' ', f);

                لكل (i = 0; i < 16; i++) {

                        إذا (i >= s)
                                fputc(' ', f);
                        وإلا
                                fputc(isprint(b[i]) ? (حرف) b[i] : '.', f);
                }

                fputc('\n', f);

                إذا (s < 16)
                        اكسر;

                n += 16;
                b += 16;
                s -= 16;
        }
}
