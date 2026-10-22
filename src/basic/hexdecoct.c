/* SPDX-License-Identifier: LGPL-2.1-or-later */

#شامل <ctype.h>
#شامل <stdio.h>

#شامل "alloc-util.h"
#شامل "hexdecoct.h"
#شامل "memory-util.h"
#شامل "string-util.h"

حرف octchar(صحیح x) {
        واپس '0' + (x & 7);
}

صحیح unoctchar(حرف c) {

        اگر (c >= '0' && c <= '7')
                واپس c - '0';

        واپس -EINVAL;
}

حرف decchar(صحیح x) {
        واپس '0' + (x % 10);
}

صحیح undecchar(حرف c) {

        اگر (c >= '0' && c <= '9')
                واپس c - '0';

        واپس -EINVAL;
}

حرف hexchar(صحیح x) {
        ثابت حرف *جدول = LOWERCASE_HEXDIGITS;
        واپس جدول[x & 15];
}

صحیح unhexchar(حرف c) {

        اگر (c >= '0' && c <= '9')
                واپس c - '0';

        اگر (c >= 'a' && c <= 'f')
                واپس c - 'a' + 10;

        اگر (c >= 'A' && c <= 'F')
                واپس c - 'A' + 10;

        واپس -EINVAL;
}

حرف* hexmem(ثابت خالی *p, size_t l) {
        ثابت uint8_t *x;
        حرف *r, *z;

        أكد(p || l == 0);

        z = r = new(حرف, l * 2 + 1);
        اگر (!r)
                واپس تہی;

        برائے (x = p; x && x < (ثابت uint8_t*) p + l; x++) {
                *(z++) = hexchar(*x >> 4);
                *(z++) = hexchar(*x & 15);
        }

        *z = 0;
        واپس r;
}

ساکن صحیح unhex_اگلا(ثابت حرف **p, size_t *l) {
        صحیح r;

        أكد(p);
        أكد(l);

        /* Find the next non-whitespace character, and decode it. We
         * greedily skip all preceding and all following whitespace. */

        برائے (;;) {
                اگر (*l == 0)
                        واپس -EPIPE;

                اگر (!strchr(WHITESPACE, **p))
                        توڑو;

                /* Skip leading whitespace */
                (*p)++, (*l)--;
        }

        r = unhexchar(**p);
        اگر (r < 0)
                واپس r;

        برائے (;;) {
                (*p)++, (*l)--;

                اگر (*l == 0 || !strchr(WHITESPACE, **p))
                        توڑو;

                /* Skip following whitespace */
        }

        واپس r;
}

صحیح unhexmem_بھرا(
                ثابت حرف *p,
                size_t l,
                bool secure,
                خالی **ret_ڈیٹا,
                size_t *ret_سائز) {

        _cleanup_free_ uint8_t *buf = تہی;
        size_t buf_size;
        ثابت حرف *x;
        uint8_t *z;

        أكد(p || l == 0);

        اگر (l == SIZE_MAX)
                l = لڑی_لمبائی(p);

        /* Note that the calculation of memory size is an upper boundary, as we ignore whitespace while decoding */
        buf_size = (l + 1) / 2 + 1;
        buf = حافظہ_دو(buf_size);
        اگر (!buf)
                واپس -ENOMEM;

        CLEANUP_ERASE_PTR(secure ? &buf : تہی, buf_size);

        برائے (x = p, z = buf;;) {
                صحیح a, b;

                a = unhex_اگلا(&x, &l);
                اگر (a == -EPIPE) /* End of string */
                        توڑو;
                اگر (a < 0)
                        واپس a;

                b = unhex_اگلا(&x, &l);
                اگر (b < 0)
                        واپس b;

                *(z++) = (uint8_t) a << 4 | (uint8_t) b;
        }

        *z = 0;

        اگر (ret_سائز)
                *ret_سائز = (size_t) (z - buf);
        اگر (ret_ڈیٹا)
                *ret_ڈیٹا = TAKE_PTR(buf);

        واپس 0;
}

/* https://tools.ietf.org/html/rfc4648#section-6
 * Notice that base32hex differs from base32 in the alphabet it uses.
 * The distinction is that the base32hex representation preserves the
 * order of the underlying data when compared as bytestrings, this is
 * useful when representing NSEC3 hashes, as one can then verify the
 * order of hashes directly from their representation. */
حرف base32hexchar(صحیح x) {
        ثابت حرف *جدول = DIGITS "ABCDEFGHIJKLMNOPQRSTUV";
        واپس جدول[x & 31];
}

صحیح unbase32hexchar(حرف c) {
        غیرعلامتی انحراف;

        اگر (c >= '0' && c <= '9')
                واپس c - '0';

        انحراف = '9' - '0' + 1;

        اگر (c >= 'A' && c <= 'V')
                واپس c - 'A' + انحراف;

        واپس -EINVAL;
}

حرف* base32hexmem(ثابت خالی *p, size_t l, bool padding) {
        حرف *r, *z;
        ثابت uint8_t *x;
        size_t len;

        أكد(p || l == 0);

        اگر (padding)
                /* five input bytes makes eight output bytes, padding is added so we must round up */
                len = 8 * (l + 4) / 5;
        ورنہ {
                /* same, but round down as there is no padding */
                len = 8 * l / 5;

                انتخاب (l % 5) {
                حالت 4:
                        len += 7;
                        توڑو;
                حالت 3:
                        len += 5;
                        توڑو;
                حالت 2:
                        len += 4;
                        توڑو;
                حالت 1:
                        len += 2;
                        توڑو;
                }
        }

        z = r = حافظہ_دو(len + 1);
        اگر (!r)
                واپس تہی;

        برائے (x = p; x < (ثابت uint8_t*) p + (l / 5) * 5; x += 5) {
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

        انتخاب (l % 5) {
        حالت 4:
                *(z++) = base32hexchar(x[0] >> 3);                    /* 000XXXXX */
                *(z++) = base32hexchar((x[0] & 7) << 2 | x[1] >> 6);  /* 000XXXYY */
                *(z++) = base32hexchar((x[1] & 63) >> 1);             /* 000YYYYY */
                *(z++) = base32hexchar((x[1] & 1) << 4 | x[2] >> 4);   /* 000YZZZZ */
                *(z++) = base32hexchar((x[2] & 15) << 1 | x[3] >> 7); /* 000ZZZZQ */
                *(z++) = base32hexchar((x[3] & 127) >> 2);            /* 000QQQQQ */
                *(z++) = base32hexchar((x[3] & 3) << 3);              /* 000QQ000 */
                اگر (padding)
                        *(z++) = '=';

                توڑو;

        حالت 3:
                *(z++) = base32hexchar(x[0] >> 3);                   /* 000XXXXX */
                *(z++) = base32hexchar((x[0] & 7) << 2 | x[1] >> 6); /* 000XXXYY */
                *(z++) = base32hexchar((x[1] & 63) >> 1);            /* 000YYYYY */
                *(z++) = base32hexchar((x[1] & 1) << 4 | x[2] >> 4); /* 000YZZZZ */
                *(z++) = base32hexchar((x[2] & 15) << 1);            /* 000ZZZZ0 */
                اگر (padding) {
                        *(z++) = '=';
                        *(z++) = '=';
                        *(z++) = '=';
                }

                توڑو;

        حالت 2:
                *(z++) = base32hexchar(x[0] >> 3);                   /* 000XXXXX */
                *(z++) = base32hexchar((x[0] & 7) << 2 | x[1] >> 6); /* 000XXXYY */
                *(z++) = base32hexchar((x[1] & 63) >> 1);            /* 000YYYYY */
                *(z++) = base32hexchar((x[1] & 1) << 4);             /* 000Y0000 */
                اگر (padding) {
                        *(z++) = '=';
                        *(z++) = '=';
                        *(z++) = '=';
                        *(z++) = '=';
                }

                توڑو;

        حالت 1:
                *(z++) = base32hexchar(x[0] >> 3);       /* 000XXXXX */
                *(z++) = base32hexchar((x[0] & 7) << 2); /* 000XXX00 */
                اگر (padding) {
                        *(z++) = '=';
                        *(z++) = '=';
                        *(z++) = '=';
                        *(z++) = '=';
                        *(z++) = '=';
                        *(z++) = '=';
                }

                توڑو;
        }

        *z = 0;
        واپس r;
}

صحیح unbase32hexmem(ثابت حرف *p, size_t l, bool padding, خالی **mem, size_t *_len) {
        _cleanup_free_ uint8_t *r = تہی;
        صحیح a, b, c, d, e, f, g, h;
        uint8_t *z;
        ثابت حرف *x;
        size_t len;
        غیرعلامتی pad = 0;

        أكد(p || l == 0);
        أكد(mem);
        أكد(_len);

        اگر (l == SIZE_MAX)
                l = لڑی_لمبائی(p);

        /* padding ensures any base32hex input has input divisible by 8 */
        اگر (padding && l % 8 != 0)
                واپس -EINVAL;

        اگر (padding) {
                /* strip the padding */
                جب (l > 0 && p[l - 1] == '=' && pad < 7) {
                        pad++;
                        l--;
                }
        }

        /* a group of eight input bytes needs five output bytes, in case of
         * padding we need to add some extra bytes */
        len = (l / 8) * 5;

        انتخاب (l % 8) {
        حالت 7:
                len += 4;
                توڑو;
        حالت 5:
                len += 3;
                توڑو;
        حالت 4:
                len += 2;
                توڑو;
        حالت 2:
                len += 1;
                توڑو;
        حالت 0:
                توڑو;
        طےشدہ:
                واپس -EINVAL;
        }

        z = r = حافظہ_دو(len + 1);
        اگر (!r)
                واپس -ENOMEM;

        برائے (x = p; x < p + (l / 8) * 8; x += 8) {
                /* a == 000XXXXX; b == 000YYYYY; c == 000ZZZZZ; d == 000WWWWW
                 * e == 000SSSSS; f == 000QQQQQ; g == 000VVVVV; h == 000RRRRR */
                a = unbase32hexchar(x[0]);
                اگر (a < 0)
                        واپس -EINVAL;

                b = unbase32hexchar(x[1]);
                اگر (b < 0)
                        واپس -EINVAL;

                c = unbase32hexchar(x[2]);
                اگر (c < 0)
                        واپس -EINVAL;

                d = unbase32hexchar(x[3]);
                اگر (d < 0)
                        واپس -EINVAL;

                e = unbase32hexchar(x[4]);
                اگر (e < 0)
                        واپس -EINVAL;

                f = unbase32hexchar(x[5]);
                اگر (f < 0)
                        واپس -EINVAL;

                g = unbase32hexchar(x[6]);
                اگر (g < 0)
                        واپس -EINVAL;

                h = unbase32hexchar(x[7]);
                اگر (h < 0)
                        واپس -EINVAL;

                *(z++) = (uint8_t) a << 3 | (uint8_t) b >> 2;                    /* XXXXXYYY */
                *(z++) = (uint8_t) b << 6 | (uint8_t) c << 1 | (uint8_t) d >> 4; /* YYZZZZZW */
                *(z++) = (uint8_t) d << 4 | (uint8_t) e >> 1;                    /* WWWWSSSS */
                *(z++) = (uint8_t) e << 7 | (uint8_t) f << 2 | (uint8_t) g >> 3; /* SQQQQQVV */
                *(z++) = (uint8_t) g << 5 | (uint8_t) h;                         /* VVVRRRRR */
        }

        انتخاب (l % 8) {
        حالت 7:
                a = unbase32hexchar(x[0]);
                اگر (a < 0)
                        واپس -EINVAL;

                b = unbase32hexchar(x[1]);
                اگر (b < 0)
                        واپس -EINVAL;

                c = unbase32hexchar(x[2]);
                اگر (c < 0)
                        واپس -EINVAL;

                d = unbase32hexchar(x[3]);
                اگر (d < 0)
                        واپس -EINVAL;

                e = unbase32hexchar(x[4]);
                اگر (e < 0)
                        واپس -EINVAL;

                f = unbase32hexchar(x[5]);
                اگر (f < 0)
                        واپس -EINVAL;

                g = unbase32hexchar(x[6]);
                اگر (g < 0)
                        واپس -EINVAL;

                /* g == 000VV000 */
                اگر (g & 7)
                        واپس -EINVAL;

                *(z++) = (uint8_t) a << 3 | (uint8_t) b >> 2;                    /* XXXXXYYY */
                *(z++) = (uint8_t) b << 6 | (uint8_t) c << 1 | (uint8_t) d >> 4; /* YYZZZZZW */
                *(z++) = (uint8_t) d << 4 | (uint8_t) e >> 1;                    /* WWWWSSSS */
                *(z++) = (uint8_t) e << 7 | (uint8_t) f << 2 | (uint8_t) g >> 3; /* SQQQQQVV */

                توڑو;
        حالت 5:
                a = unbase32hexchar(x[0]);
                اگر (a < 0)
                        واپس -EINVAL;

                b = unbase32hexchar(x[1]);
                اگر (b < 0)
                        واپس -EINVAL;

                c = unbase32hexchar(x[2]);
                اگر (c < 0)
                        واپس -EINVAL;

                d = unbase32hexchar(x[3]);
                اگر (d < 0)
                        واپس -EINVAL;

                e = unbase32hexchar(x[4]);
                اگر (e < 0)
                        واپس -EINVAL;

                /* e == 000SSSS0 */
                اگر (e & 1)
                        واپس -EINVAL;

                *(z++) = (uint8_t) a << 3 | (uint8_t) b >> 2;                    /* XXXXXYYY */
                *(z++) = (uint8_t) b << 6 | (uint8_t) c << 1 | (uint8_t) d >> 4; /* YYZZZZZW */
                *(z++) = (uint8_t) d << 4 | (uint8_t) e >> 1;                    /* WWWWSSSS */

                توڑو;
        حالت 4:
                a = unbase32hexchar(x[0]);
                اگر (a < 0)
                        واپس -EINVAL;

                b = unbase32hexchar(x[1]);
                اگر (b < 0)
                        واپس -EINVAL;

                c = unbase32hexchar(x[2]);
                اگر (c < 0)
                        واپس -EINVAL;

                d = unbase32hexchar(x[3]);
                اگر (d < 0)
                        واپس -EINVAL;

                /* d == 000W0000 */
                اگر (d & 15)
                        واپس -EINVAL;

                *(z++) = (uint8_t) a << 3 | (uint8_t) b >> 2;                    /* XXXXXYYY */
                *(z++) = (uint8_t) b << 6 | (uint8_t) c << 1 | (uint8_t) d >> 4; /* YYZZZZZW */

                توڑو;
        حالت 2:
                a = unbase32hexchar(x[0]);
                اگر (a < 0)
                        واپس -EINVAL;

                b = unbase32hexchar(x[1]);
                اگر (b < 0)
                        واپس -EINVAL;

                /* b == 000YYY00 */
                اگر (b & 3)
                        واپس -EINVAL;

                *(z++) = (uint8_t) a << 3 | (uint8_t) b >> 2; /* XXXXXYYY */

                توڑو;
        حالت 0:
                توڑو;
        طےشدہ:
                واپس -EINVAL;
        }

        *z = 0;

        *mem = TAKE_PTR(r);
        *_len = len;

        واپس 0;
}

/* https://tools.ietf.org/html/rfc4648#section-4 */
حرف base64char(صحیح x) {
        ثابت حرف *جدول = UPPERCASE_LETTERS LOWERCASE_LETTERS DIGITS "+/";
        واپس جدول[x & 63];
}

/* This is almost base64char(), but not entirely, as it uses the "url and filename safe" alphabet,
 * since we don't want "/" appear in interface names (since interfaces appear in sysfs as filenames).
 * See section #5 of RFC 4648. */
حرف urlsafe_base64char(صحیح x) {
        ثابت حرف *جدول = UPPERCASE_LETTERS LOWERCASE_LETTERS DIGITS "-_";
        واپس جدول[x & 63];
}

صحیح unbase64char(حرف c) {
        غیرعلامتی انحراف;

        اگر (c >= 'A' && c <= 'Z')
                واپس c - 'A';

        انحراف = 'Z' - 'A' + 1;

        اگر (c >= 'a' && c <= 'z')
                واپس c - 'a' + انحراف;

        انحراف += 'z' - 'a' + 1;

        اگر (c >= '0' && c <= '9')
                واپس c - '0' + انحراف;

        انحراف += '9' - '0' + 1;

        اگر (اندر_سیٹ(c, '+', '-')) /* Support both the regular and the URL safe character set (see above) */
                واپس انحراف;

        انحراف++;

        اگر (اندر_سیٹ(c, '/', '_')) /* ditto */
                واپس انحراف;

        واپس -EINVAL;
}

ساکن خالی maybe_سطر_break(حرف **x, حرف *آغاز, size_t سطر_break) {
        size_t n;

        أكد(x);
        أكد(*x);
        أكد(آغاز);
        أكد(*x >= آغاز);

        اگر (سطر_break == SIZE_MAX)
                واپس;

        n = *x - آغاز;

        اگر (n % (سطر_break + 1) == سطر_break)
                *((*x)++) = '\n';
}

ssize_t base64mem_بھرا(
                ثابت خالی *p,
                size_t l,
                size_t سطر_break,
                حرف **ret) {

        ثابت uint8_t *x;
        حرف *b, *z;
        size_t m;

        أكد(p || l == 0);
        أكد(سطر_break > 0);
        أكد(ret);

        /* three input bytes makes four output bytes, padding is added so we must round up */
        m = 4 * (l + 2) / 3 + 1;
        اگر (سطر_break != SIZE_MAX)
                m += m / سطر_break;

        z = b = حافظہ_دو(m);
        اگر (!b)
                واپس -ENOMEM;

        برائے (x = p; x && x < (ثابت uint8_t*) p + (l / 3) * 3; x += 3) {
                /* x[0] == XXXXXXXX; x[1] == YYYYYYYY; x[2] == ZZZZZZZZ */
                maybe_سطر_break(&z, b, سطر_break);
                *(z++) = base64char(x[0] >> 2);                    /* 00XXXXXX */
                maybe_سطر_break(&z, b, سطر_break);
                *(z++) = base64char((x[0] & 3) << 4 | x[1] >> 4);  /* 00XXYYYY */
                maybe_سطر_break(&z, b, سطر_break);
                *(z++) = base64char((x[1] & 15) << 2 | x[2] >> 6); /* 00YYYYZZ */
                maybe_سطر_break(&z, b, سطر_break);
                *(z++) = base64char(x[2] & 63);                    /* 00ZZZZZZ */
        }

        انتخاب (l % 3) {
        حالت 2:
                maybe_سطر_break(&z, b, سطر_break);
                *(z++) = base64char(x[0] >> 2);                   /* 00XXXXXX */
                maybe_سطر_break(&z, b, سطر_break);
                *(z++) = base64char((x[0] & 3) << 4 | x[1] >> 4); /* 00XXYYYY */
                maybe_سطر_break(&z, b, سطر_break);
                *(z++) = base64char((x[1] & 15) << 2);            /* 00YYYY00 */
                maybe_سطر_break(&z, b, سطر_break);
                *(z++) = '=';
                توڑو;

        حالت 1:
                maybe_سطر_break(&z, b, سطر_break);
                *(z++) = base64char(x[0] >> 2);        /* 00XXXXXX */
                maybe_سطر_break(&z, b, سطر_break);
                *(z++) = base64char((x[0] & 3) << 4);  /* 00XX0000 */
                maybe_سطر_break(&z, b, سطر_break);
                *(z++) = '=';
                maybe_سطر_break(&z, b, سطر_break);
                *(z++) = '=';
                توڑو;
        }

        *z = 0;
        *ret = b;

        أكد(z >= b); /* Let static analyzers know that the answer is non-negative. */
        واپس z - b;
}

ساکن ssize_t base64_append_چوڑائی(
                حرف **سابقہ,
                size_t plen,
                حرف sep,
                size_t indent,
                ثابت خالی *p,
                size_t l,
                size_t width) {

        _cleanup_free_ حرف *x = تہی;
        حرف *t, *s;
        size_t سطریں;
        ssize_t len;

        أكد(سابقہ);
        أكد(*سابقہ || plen == 0);
        أكد(p || l == 0);

        len = base64mem(p, l, &x);
        اگر (len < 0)
                واپس len;
        اگر (len == 0)
                واپس plen;

        سطریں = DIV_باری_اوپر(len, width);

        اگر (plen >= SSIZE_MAX - 1 - 1 ||
            سطریں > (SSIZE_MAX - plen - 1 - 1) / (indent + width + 1))
                واپس -ENOMEM;

        t = realloc(*سابقہ, plen + 1 + 1 + (indent + width + 1) * سطریں);
        اگر (!t)
                واپس -ENOMEM;

        s = t + plen;
        برائے (size_t سطر = 0; سطر < سطریں; سطر++) {
                size_t act = MIN(width, (size_t) len);

                اگر (سطر > 0)
                        sep = '\n';

                اگر (s > t) {
                        *s++ = sep;
                        اگر (sep == '\n')
                                s = mempset(s, ' ', indent);
                }

                s = mempcpy(s, x + width * سطر, act);
                len -= act;
        }
        أكد(len == 0);

        *s = '\0';
        *سابقہ = t;
        واپس s - t;
}

ssize_t base64_append(
                حرف **سابقہ,
                size_t plen,
                ثابت خالی *p,
                size_t l,
                size_t indent,
                size_t width) {

        اگر (plen > width / 2 || plen + indent > width)
                /* leave indent on the left, keep last column free */
                واپس base64_append_چوڑائی(سابقہ, plen, '\n', indent, p, l, width - indent);
        ورنہ
                /* leave plen on the left, keep last column free */
                واپس base64_append_چوڑائی(سابقہ, plen, ' ', plen + 1, p, l, width - plen - 1);
}

ساکن صحیح unbase64_اگلا(ثابت حرف **p, size_t *l) {
        صحیح ret;

        أكد(p);
        أكد(l);

        /* Find the next non-whitespace character, and decode it. If we find padding, we return it as INT_MAX. We
         * greedily skip all preceding and all following whitespace. */

        برائے (;;) {
                اگر (*l == 0)
                        واپس -EPIPE;

                اگر (!strchr(WHITESPACE, **p))
                        توڑو;

                /* Skip leading whitespace */
                (*p)++, (*l)--;
        }

        اگر (**p == '=')
                ret = زیادہ_سے_زیادہ_عدد; /* return padding as INT_MAX */
        ورنہ {
                ret = unbase64char(**p);
                اگر (ret < 0)
                        واپس ret;
        }

        برائے (;;) {
                (*p)++, (*l)--;

                اگر (*l == 0)
                        توڑو;
                اگر (!strchr(WHITESPACE, **p))
                        توڑو;

                /* Skip following whitespace */
        }

        واپس ret;
}

صحیح unbase64mem_بھرا(
                ثابت حرف *p,
                size_t l,
                bool secure,
                خالی **ret_ڈیٹا,
                size_t *ret_سائز) {

        _cleanup_free_ uint8_t *buf = تہی;
        ثابت حرف *x;
        uint8_t *z;
        size_t len;

        أكد(p || l == 0);

        اگر (l == SIZE_MAX)
                l = لڑی_لمبائی(p);

        /* A group of four input bytes needs three output bytes, in case of padding we need to add two or three extra
         * bytes. Note that this calculation is an upper boundary, as we ignore whitespace while decoding */
        len = (l / 4) * 3 + (l % 4 != 0 ? (l % 4) - 1 : 0);

        buf = حافظہ_دو(len + 1);
        اگر (!buf)
                واپس -ENOMEM;

        CLEANUP_ERASE_PTR(secure ? &buf : تہی, len);

        برائے (x = p, z = buf;;) {
                صحیح a, b, c, d; /* a == 00XXXXXX; b == 00YYYYYY; c == 00ZZZZZZ; d == 00WWWWWW */

                a = unbase64_اگلا(&x, &l);
                اگر (a == -EPIPE) /* End of string */
                        توڑو;
                اگر (a < 0)
                        واپس a;
                اگر (a == زیادہ_سے_زیادہ_عدد) /* Padding is not allowed at the beginning of a 4ch block */
                        واپس -EINVAL;

                b = unbase64_اگلا(&x, &l);
                اگر (b < 0)
                        واپس b;
                اگر (b == زیادہ_سے_زیادہ_عدد) /* Padding is not allowed at the second character of a 4ch block either */
                        واپس -EINVAL;

                c = unbase64_اگلا(&x, &l);
                اگر (c < 0)
                        واپس c;

                d = unbase64_اگلا(&x, &l);
                اگر (d < 0)
                        واپس d;

                اگر (c == زیادہ_سے_زیادہ_عدد) { /* Padding at the third character */

                        اگر (d != زیادہ_سے_زیادہ_عدد) /* If the third character is padding, the fourth must be too */
                                واپس -EINVAL;

                        /* b == 00YY0000 */
                        اگر (b & 15)
                                واپس -EINVAL;

                        اگر (l > 0) /* Trailing rubbish? */
                                واپس -ENAMETOOLONG;

                        *(z++) = (uint8_t) a << 2 | (uint8_t) (b >> 4); /* XXXXXXYY */
                        توڑو;
                }

                اگر (d == زیادہ_سے_زیادہ_عدد) {
                        /* c == 00ZZZZ00 */
                        اگر (c & 3)
                                واپس -EINVAL;

                        اگر (l > 0) /* Trailing rubbish? */
                                واپس -ENAMETOOLONG;

                        *(z++) = (uint8_t) a << 2 | (uint8_t) b >> 4; /* XXXXXXYY */
                        *(z++) = (uint8_t) b << 4 | (uint8_t) c >> 2; /* YYYYZZZZ */
                        توڑو;
                }

                *(z++) = (uint8_t) a << 2 | (uint8_t) b >> 4; /* XXXXXXYY */
                *(z++) = (uint8_t) b << 4 | (uint8_t) c >> 2; /* YYYYZZZZ */
                *(z++) = (uint8_t) c << 6 | (uint8_t) d;      /* ZZWWWWWW */
        }

        *z = 0;

        أكد((size_t) (z - buf) <= len);

        اگر (ret_سائز)
                *ret_سائز = (size_t) (z - buf);
        اگر (ret_ڈیٹا)
                *ret_ڈیٹا = TAKE_PTR(buf);

        واپس 0;
}

خالی hexdump(FILE *f, ثابت خالی *p, size_t s) {
        ثابت uint8_t *b = p;
        غیرعلامتی n = 0;

        أكد(b || s == 0);

        اگر (s == SIZE_MAX)
                s = لڑی_لمبائی(p);

        اگر (!f)
                f = stdout;

        جب (s > 0) {
                size_t i;

                fprintf(f, "%04x  ", n);

                برائے (i = 0; i < 16; i++) {

                        اگر (i >= s)
                                fputs("   ", f);
                        ورنہ
                                fprintf(f, "%02x ", b[i]);

                        اگر (i == 7)
                                fputc(' ', f);
                }

                fputc(' ', f);

                برائے (i = 0; i < 16; i++) {

                        اگر (i >= s)
                                fputc(' ', f);
                        ورنہ
                                fputc(isprint(b[i]) ? (حرف) b[i] : '.', f);
                }

                fputc('\n', f);

                اگر (s < 16)
                        توڑو;

                n += 16;
                b += 16;
                s -= 16;
        }
}
