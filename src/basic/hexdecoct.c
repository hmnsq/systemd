/* SPDX-License-Identifier: LGPL-2.1-or-later */

#شامل <ctype.h>
#شامل <stdio.h>

#شامل "alloc-util.h"
#شامل "hexdecoct.h"
#شامل "memory-util.h"
#شامل "string-util.h"

نویسه octchar(صحیح x) {
        بازگشت '0' + (x & 7);
}

صحیح unoctchar(نویسه c) {

        اگر (c >= '0' && c <= '7')
                بازگشت c - '0';

        بازگشت -EINVAL;
}

نویسه decchar(صحیح x) {
        بازگشت '0' + (x % 10);
}

صحیح undecchar(نویسه c) {

        اگر (c >= '0' && c <= '9')
                بازگشت c - '0';

        بازگشت -EINVAL;
}

نویسه hexchar(صحیح x) {
        ثابت نویسه *table = LOWERCASE_HEXDIGITS;
        بازگشت table[x & 15];
}

صحیح unhexchar(نویسه c) {

        اگر (c >= '0' && c <= '9')
                بازگشت c - '0';

        اگر (c >= 'a' && c <= 'f')
                بازگشت c - 'a' + 10;

        اگر (c >= 'A' && c <= 'F')
                بازگشت c - 'A' + 10;

        بازگشت -EINVAL;
}

نویسه* hexmem(ثابت تهی *p, size_t l) {
        ثابت uint8_t *x;
        نویسه *r, *z;

        أكد(p || l == 0);

        z = r = new(نویسه, l * 2 + 1);
        اگر (!r)
                بازگشت پوچ;

        برای (x = p; x && x < (ثابت uint8_t*) p + l; x++) {
                *(z++) = hexchar(*x >> 4);
                *(z++) = hexchar(*x & 15);
        }

        *z = 0;
        بازگشت r;
}

ایستا صحیح unhex_next(ثابت نویسه **p, size_t *l) {
        صحیح r;

        أكد(p);
        أكد(l);

        /* Find the next non-whitespace character, and decode it. We
         * greedily skip all preceding and all following whitespace. */

        برای (;;) {
                اگر (*l == 0)
                        بازگشت -EPIPE;

                اگر (!strchr(WHITESPACE, **p))
                        بشکن;

                /* Skip leading whitespace */
                (*p)++, (*l)--;
        }

        r = unhexchar(**p);
        اگر (r < 0)
                بازگشت r;

        برای (;;) {
                (*p)++, (*l)--;

                اگر (*l == 0 || !strchr(WHITESPACE, **p))
                        بشکن;

                /* Skip following whitespace */
        }

        بازگشت r;
}

صحیح unhexmem_full(
                ثابت نویسه *p,
                size_t l,
                bool secure,
                تهی **ret_data,
                size_t *ret_size) {

        _cleanup_free_ uint8_t *buf = پوچ;
        size_t buf_size;
        ثابت نویسه *x;
        uint8_t *z;

        أكد(p || l == 0);

        اگر (l == SIZE_MAX)
                l = strlen(p);

        /* Note that the calculation of memory size is an upper boundary, as we ignore whitespace while decoding */
        buf_size = (l + 1) / 2 + 1;
        buf = malloc(buf_size);
        اگر (!buf)
                بازگشت -ENOMEM;

        CLEANUP_ERASE_PTR(secure ? &buf : پوچ, buf_size);

        برای (x = p, z = buf;;) {
                صحیح a, b;

                a = unhex_next(&x, &l);
                اگر (a == -EPIPE) /* End of string */
                        بشکن;
                اگر (a < 0)
                        بازگشت a;

                b = unhex_next(&x, &l);
                اگر (b < 0)
                        بازگشت b;

                *(z++) = (uint8_t) a << 4 | (uint8_t) b;
        }

        *z = 0;

        اگر (ret_size)
                *ret_size = (size_t) (z - buf);
        اگر (ret_data)
                *ret_data = TAKE_PTR(buf);

        بازگشت 0;
}

/* https://tools.ietf.org/html/rfc4648#section-6
 * Notice that base32hex differs from base32 in the alphabet it uses.
 * The distinction is that the base32hex representation preserves the
 * order of the underlying data when compared as bytestrings, this is
 * useful when representing NSEC3 hashes, as one can then verify the
 * order of hashes directly from their representation. */
نویسه base32hexchar(صحیح x) {
        ثابت نویسه *table = DIGITS "ABCDEFGHIJKLMNOPQRSTUV";
        بازگشت table[x & 31];
}

صحیح unbase32hexchar(نویسه c) {
        unsigned offset;

        اگر (c >= '0' && c <= '9')
                بازگشت c - '0';

        offset = '9' - '0' + 1;

        اگر (c >= 'A' && c <= 'V')
                بازگشت c - 'A' + offset;

        بازگشت -EINVAL;
}

نویسه* base32hexmem(ثابت تهی *p, size_t l, bool padding) {
        نویسه *r, *z;
        ثابت uint8_t *x;
        size_t len;

        أكد(p || l == 0);

        اگر (padding)
                /* five input bytes makes eight output bytes, padding is added so we must round up */
                len = 8 * (l + 4) / 5;
        وگرنه {
                /* same, but round down as there is no padding */
                len = 8 * l / 5;

                انتخاب (l % 5) {
                حالت 4:
                        len += 7;
                        بشکن;
                حالت 3:
                        len += 5;
                        بشکن;
                حالت 2:
                        len += 4;
                        بشکن;
                حالت 1:
                        len += 2;
                        بشکن;
                }
        }

        z = r = malloc(len + 1);
        اگر (!r)
                بازگشت پوچ;

        برای (x = p; x < (ثابت uint8_t*) p + (l / 5) * 5; x += 5) {
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

                بشکن;

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

                بشکن;

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

                بشکن;

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

                بشکن;
        }

        *z = 0;
        بازگشت r;
}

صحیح unbase32hexmem(ثابت نویسه *p, size_t l, bool padding, تهی **mem, size_t *_len) {
        _cleanup_free_ uint8_t *r = پوچ;
        صحیح a, b, c, d, e, f, g, h;
        uint8_t *z;
        ثابت نویسه *x;
        size_t len;
        unsigned pad = 0;

        أكد(p || l == 0);
        أكد(mem);
        أكد(_len);

        اگر (l == SIZE_MAX)
                l = strlen(p);

        /* padding ensures any base32hex input has input divisible by 8 */
        اگر (padding && l % 8 != 0)
                بازگشت -EINVAL;

        اگر (padding) {
                /* strip the padding */
                وقتی (l > 0 && p[l - 1] == '=' && pad < 7) {
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
                بشکن;
        حالت 5:
                len += 3;
                بشکن;
        حالت 4:
                len += 2;
                بشکن;
        حالت 2:
                len += 1;
                بشکن;
        حالت 0:
                بشکن;
        پیشفرض:
                بازگشت -EINVAL;
        }

        z = r = malloc(len + 1);
        اگر (!r)
                بازگشت -ENOMEM;

        برای (x = p; x < p + (l / 8) * 8; x += 8) {
                /* a == 000XXXXX; b == 000YYYYY; c == 000ZZZZZ; d == 000WWWWW
                 * e == 000SSSSS; f == 000QQQQQ; g == 000VVVVV; h == 000RRRRR */
                a = unbase32hexchar(x[0]);
                اگر (a < 0)
                        بازگشت -EINVAL;

                b = unbase32hexchar(x[1]);
                اگر (b < 0)
                        بازگشت -EINVAL;

                c = unbase32hexchar(x[2]);
                اگر (c < 0)
                        بازگشت -EINVAL;

                d = unbase32hexchar(x[3]);
                اگر (d < 0)
                        بازگشت -EINVAL;

                e = unbase32hexchar(x[4]);
                اگر (e < 0)
                        بازگشت -EINVAL;

                f = unbase32hexchar(x[5]);
                اگر (f < 0)
                        بازگشت -EINVAL;

                g = unbase32hexchar(x[6]);
                اگر (g < 0)
                        بازگشت -EINVAL;

                h = unbase32hexchar(x[7]);
                اگر (h < 0)
                        بازگشت -EINVAL;

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
                        بازگشت -EINVAL;

                b = unbase32hexchar(x[1]);
                اگر (b < 0)
                        بازگشت -EINVAL;

                c = unbase32hexchar(x[2]);
                اگر (c < 0)
                        بازگشت -EINVAL;

                d = unbase32hexchar(x[3]);
                اگر (d < 0)
                        بازگشت -EINVAL;

                e = unbase32hexchar(x[4]);
                اگر (e < 0)
                        بازگشت -EINVAL;

                f = unbase32hexchar(x[5]);
                اگر (f < 0)
                        بازگشت -EINVAL;

                g = unbase32hexchar(x[6]);
                اگر (g < 0)
                        بازگشت -EINVAL;

                /* g == 000VV000 */
                اگر (g & 7)
                        بازگشت -EINVAL;

                *(z++) = (uint8_t) a << 3 | (uint8_t) b >> 2;                    /* XXXXXYYY */
                *(z++) = (uint8_t) b << 6 | (uint8_t) c << 1 | (uint8_t) d >> 4; /* YYZZZZZW */
                *(z++) = (uint8_t) d << 4 | (uint8_t) e >> 1;                    /* WWWWSSSS */
                *(z++) = (uint8_t) e << 7 | (uint8_t) f << 2 | (uint8_t) g >> 3; /* SQQQQQVV */

                بشکن;
        حالت 5:
                a = unbase32hexchar(x[0]);
                اگر (a < 0)
                        بازگشت -EINVAL;

                b = unbase32hexchar(x[1]);
                اگر (b < 0)
                        بازگشت -EINVAL;

                c = unbase32hexchar(x[2]);
                اگر (c < 0)
                        بازگشت -EINVAL;

                d = unbase32hexchar(x[3]);
                اگر (d < 0)
                        بازگشت -EINVAL;

                e = unbase32hexchar(x[4]);
                اگر (e < 0)
                        بازگشت -EINVAL;

                /* e == 000SSSS0 */
                اگر (e & 1)
                        بازگشت -EINVAL;

                *(z++) = (uint8_t) a << 3 | (uint8_t) b >> 2;                    /* XXXXXYYY */
                *(z++) = (uint8_t) b << 6 | (uint8_t) c << 1 | (uint8_t) d >> 4; /* YYZZZZZW */
                *(z++) = (uint8_t) d << 4 | (uint8_t) e >> 1;                    /* WWWWSSSS */

                بشکن;
        حالت 4:
                a = unbase32hexchar(x[0]);
                اگر (a < 0)
                        بازگشت -EINVAL;

                b = unbase32hexchar(x[1]);
                اگر (b < 0)
                        بازگشت -EINVAL;

                c = unbase32hexchar(x[2]);
                اگر (c < 0)
                        بازگشت -EINVAL;

                d = unbase32hexchar(x[3]);
                اگر (d < 0)
                        بازگشت -EINVAL;

                /* d == 000W0000 */
                اگر (d & 15)
                        بازگشت -EINVAL;

                *(z++) = (uint8_t) a << 3 | (uint8_t) b >> 2;                    /* XXXXXYYY */
                *(z++) = (uint8_t) b << 6 | (uint8_t) c << 1 | (uint8_t) d >> 4; /* YYZZZZZW */

                بشکن;
        حالت 2:
                a = unbase32hexchar(x[0]);
                اگر (a < 0)
                        بازگشت -EINVAL;

                b = unbase32hexchar(x[1]);
                اگر (b < 0)
                        بازگشت -EINVAL;

                /* b == 000YYY00 */
                اگر (b & 3)
                        بازگشت -EINVAL;

                *(z++) = (uint8_t) a << 3 | (uint8_t) b >> 2; /* XXXXXYYY */

                بشکن;
        حالت 0:
                بشکن;
        پیشفرض:
                بازگشت -EINVAL;
        }

        *z = 0;

        *mem = TAKE_PTR(r);
        *_len = len;

        بازگشت 0;
}

/* https://tools.ietf.org/html/rfc4648#section-4 */
نویسه base64char(صحیح x) {
        ثابت نویسه *table = UPPERCASE_LETTERS LOWERCASE_LETTERS DIGITS "+/";
        بازگشت table[x & 63];
}

/* This is almost base64char(), but not entirely, as it uses the "url and filename safe" alphabet,
 * since we don't want "/" appear in interface names (since interfaces appear in sysfs as filenames).
 * See section #5 of RFC 4648. */
نویسه urlsafe_base64char(صحیح x) {
        ثابت نویسه *table = UPPERCASE_LETTERS LOWERCASE_LETTERS DIGITS "-_";
        بازگشت table[x & 63];
}

صحیح unbase64char(نویسه c) {
        unsigned offset;

        اگر (c >= 'A' && c <= 'Z')
                بازگشت c - 'A';

        offset = 'Z' - 'A' + 1;

        اگر (c >= 'a' && c <= 'z')
                بازگشت c - 'a' + offset;

        offset += 'z' - 'a' + 1;

        اگر (c >= '0' && c <= '9')
                بازگشت c - '0' + offset;

        offset += '9' - '0' + 1;

        اگر (IN_SET(c, '+', '-')) /* Support both the regular and the URL safe character set (see above) */
                بازگشت offset;

        offset++;

        اگر (IN_SET(c, '/', '_')) /* ditto */
                بازگشت offset;

        بازگشت -EINVAL;
}

ایستا تهی maybe_سطر_break(نویسه **x, نویسه *start, size_t سطر_break) {
        size_t n;

        أكد(x);
        أكد(*x);
        أكد(start);
        أكد(*x >= start);

        اگر (سطر_break == SIZE_MAX)
                بازگشت;

        n = *x - start;

        اگر (n % (سطر_break + 1) == سطر_break)
                *((*x)++) = '\n';
}

ssize_t base64mem_full(
                ثابت تهی *p,
                size_t l,
                size_t سطر_break,
                نویسه **ret) {

        ثابت uint8_t *x;
        نویسه *b, *z;
        size_t m;

        أكد(p || l == 0);
        أكد(سطر_break > 0);
        أكد(ret);

        /* three input bytes makes four output bytes, padding is added so we must round up */
        m = 4 * (l + 2) / 3 + 1;
        اگر (سطر_break != SIZE_MAX)
                m += m / سطر_break;

        z = b = malloc(m);
        اگر (!b)
                بازگشت -ENOMEM;

        برای (x = p; x && x < (ثابت uint8_t*) p + (l / 3) * 3; x += 3) {
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
                بشکن;

        حالت 1:
                maybe_سطر_break(&z, b, سطر_break);
                *(z++) = base64char(x[0] >> 2);        /* 00XXXXXX */
                maybe_سطر_break(&z, b, سطر_break);
                *(z++) = base64char((x[0] & 3) << 4);  /* 00XX0000 */
                maybe_سطر_break(&z, b, سطر_break);
                *(z++) = '=';
                maybe_سطر_break(&z, b, سطر_break);
                *(z++) = '=';
                بشکن;
        }

        *z = 0;
        *ret = b;

        أكد(z >= b); /* Let static analyzers know that the answer is non-negative. */
        بازگشت z - b;
}

ایستا ssize_t base64_append_width(
                نویسه **prefix,
                size_t plen,
                نویسه sep,
                size_t indent,
                ثابت تهی *p,
                size_t l,
                size_t width) {

        _cleanup_free_ نویسه *x = پوچ;
        نویسه *t, *s;
        size_t سطرها;
        ssize_t len;

        أكد(prefix);
        أكد(*prefix || plen == 0);
        أكد(p || l == 0);

        len = base64mem(p, l, &x);
        اگر (len < 0)
                بازگشت len;
        اگر (len == 0)
                بازگشت plen;

        سطرها = DIV_ROUND_UP(len, width);

        اگر (plen >= SSIZE_MAX - 1 - 1 ||
            سطرها > (SSIZE_MAX - plen - 1 - 1) / (indent + width + 1))
                بازگشت -ENOMEM;

        t = realloc(*prefix, plen + 1 + 1 + (indent + width + 1) * سطرها);
        اگر (!t)
                بازگشت -ENOMEM;

        s = t + plen;
        برای (size_t سطر = 0; سطر < سطرها; سطر++) {
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
        *prefix = t;
        بازگشت s - t;
}

ssize_t base64_append(
                نویسه **prefix,
                size_t plen,
                ثابت تهی *p,
                size_t l,
                size_t indent,
                size_t width) {

        اگر (plen > width / 2 || plen + indent > width)
                /* leave indent on the left, keep last column free */
                بازگشت base64_append_width(prefix, plen, '\n', indent, p, l, width - indent);
        وگرنه
                /* leave plen on the left, keep last column free */
                بازگشت base64_append_width(prefix, plen, ' ', plen + 1, p, l, width - plen - 1);
}

ایستا صحیح unbase64_next(ثابت نویسه **p, size_t *l) {
        صحیح ret;

        أكد(p);
        أكد(l);

        /* Find the next non-whitespace character, and decode it. If we find padding, we return it as INT_MAX. We
         * greedily skip all preceding and all following whitespace. */

        برای (;;) {
                اگر (*l == 0)
                        بازگشت -EPIPE;

                اگر (!strchr(WHITESPACE, **p))
                        بشکن;

                /* Skip leading whitespace */
                (*p)++, (*l)--;
        }

        اگر (**p == '=')
                ret = INT_MAX; /* return padding as INT_MAX */
        وگرنه {
                ret = unbase64char(**p);
                اگر (ret < 0)
                        بازگشت ret;
        }

        برای (;;) {
                (*p)++, (*l)--;

                اگر (*l == 0)
                        بشکن;
                اگر (!strchr(WHITESPACE, **p))
                        بشکن;

                /* Skip following whitespace */
        }

        بازگشت ret;
}

صحیح unbase64mem_full(
                ثابت نویسه *p,
                size_t l,
                bool secure,
                تهی **ret_data,
                size_t *ret_size) {

        _cleanup_free_ uint8_t *buf = پوچ;
        ثابت نویسه *x;
        uint8_t *z;
        size_t len;

        أكد(p || l == 0);

        اگر (l == SIZE_MAX)
                l = strlen(p);

        /* A group of four input bytes needs three output bytes, in case of padding we need to add two or three extra
         * bytes. Note that this calculation is an upper boundary, as we ignore whitespace while decoding */
        len = (l / 4) * 3 + (l % 4 != 0 ? (l % 4) - 1 : 0);

        buf = malloc(len + 1);
        اگر (!buf)
                بازگشت -ENOMEM;

        CLEANUP_ERASE_PTR(secure ? &buf : پوچ, len);

        برای (x = p, z = buf;;) {
                صحیح a, b, c, d; /* a == 00XXXXXX; b == 00YYYYYY; c == 00ZZZZZZ; d == 00WWWWWW */

                a = unbase64_next(&x, &l);
                اگر (a == -EPIPE) /* End of string */
                        بشکن;
                اگر (a < 0)
                        بازگشت a;
                اگر (a == INT_MAX) /* Padding is not allowed at the beginning of a 4ch block */
                        بازگشت -EINVAL;

                b = unbase64_next(&x, &l);
                اگر (b < 0)
                        بازگشت b;
                اگر (b == INT_MAX) /* Padding is not allowed at the second character of a 4ch block either */
                        بازگشت -EINVAL;

                c = unbase64_next(&x, &l);
                اگر (c < 0)
                        بازگشت c;

                d = unbase64_next(&x, &l);
                اگر (d < 0)
                        بازگشت d;

                اگر (c == INT_MAX) { /* Padding at the third character */

                        اگر (d != INT_MAX) /* If the third character is padding, the fourth must be too */
                                بازگشت -EINVAL;

                        /* b == 00YY0000 */
                        اگر (b & 15)
                                بازگشت -EINVAL;

                        اگر (l > 0) /* Trailing rubbish? */
                                بازگشت -ENAMETOOLONG;

                        *(z++) = (uint8_t) a << 2 | (uint8_t) (b >> 4); /* XXXXXXYY */
                        بشکن;
                }

                اگر (d == INT_MAX) {
                        /* c == 00ZZZZ00 */
                        اگر (c & 3)
                                بازگشت -EINVAL;

                        اگر (l > 0) /* Trailing rubbish? */
                                بازگشت -ENAMETOOLONG;

                        *(z++) = (uint8_t) a << 2 | (uint8_t) b >> 4; /* XXXXXXYY */
                        *(z++) = (uint8_t) b << 4 | (uint8_t) c >> 2; /* YYYYZZZZ */
                        بشکن;
                }

                *(z++) = (uint8_t) a << 2 | (uint8_t) b >> 4; /* XXXXXXYY */
                *(z++) = (uint8_t) b << 4 | (uint8_t) c >> 2; /* YYYYZZZZ */
                *(z++) = (uint8_t) c << 6 | (uint8_t) d;      /* ZZWWWWWW */
        }

        *z = 0;

        أكد((size_t) (z - buf) <= len);

        اگر (ret_size)
                *ret_size = (size_t) (z - buf);
        اگر (ret_data)
                *ret_data = TAKE_PTR(buf);

        بازگشت 0;
}

تهی hexdump(FILE *f, ثابت تهی *p, size_t s) {
        ثابت uint8_t *b = p;
        unsigned n = 0;

        أكد(b || s == 0);

        اگر (s == SIZE_MAX)
                s = strlen(p);

        اگر (!f)
                f = stdout;

        وقتی (s > 0) {
                size_t i;

                fprintf(f, "%04x  ", n);

                برای (i = 0; i < 16; i++) {

                        اگر (i >= s)
                                fputs("   ", f);
                        وگرنه
                                fprintf(f, "%02x ", b[i]);

                        اگر (i == 7)
                                fputc(' ', f);
                }

                fputc(' ', f);

                برای (i = 0; i < 16; i++) {

                        اگر (i >= s)
                                fputc(' ', f);
                        وگرنه
                                fputc(isprint(b[i]) ? (نویسه) b[i] : '.', f);
                }

                fputc('\n', f);

                اگر (s < 16)
                        بشکن;

                n += 16;
                b += 16;
                s -= 16;
        }
}
