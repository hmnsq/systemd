/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव <ctype.h>
#अन्तर्भाव <stdio.h>

#अन्तर्भाव "alloc-util.h"
#अन्तर्भाव "hexdecoct.h"
#अन्तर्भाव "memory-util.h"
#अन्तर्भाव "string-util.h"

वर्ण octchar(पूर्णाङ्क x) {
        फर्काऊ '0' + (x & 7);
}

पूर्णाङ्क unoctchar(वर्ण c) {

        यदि (c >= '0' && c <= '7')
                फर्काऊ c - '0';

        फर्काऊ -EINVAL;
}

वर्ण decchar(पूर्णाङ्क x) {
        फर्काऊ '0' + (x % 10);
}

पूर्णाङ्क undecchar(वर्ण c) {

        यदि (c >= '0' && c <= '9')
                फर्काऊ c - '0';

        फर्काऊ -EINVAL;
}

वर्ण hexchar(पूर्णाङ्क x) {
        अचर वर्ण *table = LOWERCASE_HEXDIGITS;
        फर्काऊ table[x & 15];
}

पूर्णाङ्क unhexchar(वर्ण c) {

        यदि (c >= '0' && c <= '9')
                फर्काऊ c - '0';

        यदि (c >= 'a' && c <= 'f')
                फर्काऊ c - 'a' + 10;

        यदि (c >= 'A' && c <= 'F')
                फर्काऊ c - 'A' + 10;

        फर्काऊ -EINVAL;
}

वर्ण* hexmem(अचर शून्य *p, माप_प्रकार l) {
        अचर uint8_t *x;
        वर्ण *r, *z;

        प्रतिज्ञा(p || l == 0);

        z = r = new(वर्ण, l * 2 + 1);
        यदि (!r)
                फर्काऊ रिक्त;

        लागि (x = p; x && x < (अचर uint8_t*) p + l; x++) {
                *(z++) = hexchar(*x >> 4);
                *(z++) = hexchar(*x & 15);
        }

        *z = 0;
        फर्काऊ r;
}

स्थिर पूर्णाङ्क unhex_next(अचर वर्ण **p, माप_प्रकार *l) {
        पूर्णाङ्क r;

        प्रतिज्ञा(p);
        प्रतिज्ञा(l);

        /* Find the next non-whitespace character, and decode it. We
         * greedily skip all preceding and all following whitespace. */

        लागि (;;) {
                यदि (*l == 0)
                        फर्काऊ -EPIPE;

                यदि (!म_अक्षर(WHITESPACE, **p))
                        रोक;

                /* Skip leading whitespace */
                (*p)++, (*l)--;
        }

        r = unhexchar(**p);
        यदि (r < 0)
                फर्काऊ r;

        लागि (;;) {
                (*p)++, (*l)--;

                यदि (*l == 0 || !म_अक्षर(WHITESPACE, **p))
                        रोक;

                /* Skip following whitespace */
        }

        फर्काऊ r;
}

पूर्णाङ्क unhexmem_full(
                अचर वर्ण *p,
                माप_प्रकार l,
                bool secure,
                शून्य **ret_data,
                माप_प्रकार *ret_size) {

        _cleanup_free_ uint8_t *buf = रिक्त;
        माप_प्रकार buf_size;
        अचर वर्ण *x;
        uint8_t *z;

        प्रतिज्ञा(p || l == 0);

        यदि (l == SIZE_MAX)
                l = म_माप(p);

        /* Note that the calculation of memory size is an upper boundary, as we ignore whitespace while decoding */
        buf_size = (l + 1) / 2 + 1;
        buf = दो_स्मृति(buf_size);
        यदि (!buf)
                फर्काऊ -ENOMEM;

        CLEANUP_ERASE_PTR(secure ? &buf : रिक्त, buf_size);

        लागि (x = p, z = buf;;) {
                पूर्णाङ्क a, b;

                a = unhex_next(&x, &l);
                यदि (a == -EPIPE) /* End of string */
                        रोक;
                यदि (a < 0)
                        फर्काऊ a;

                b = unhex_next(&x, &l);
                यदि (b < 0)
                        फर्काऊ b;

                *(z++) = (uint8_t) a << 4 | (uint8_t) b;
        }

        *z = 0;

        यदि (ret_size)
                *ret_size = (माप_प्रकार) (z - buf);
        यदि (ret_data)
                *ret_data = TAKE_PTR(buf);

        फर्काऊ 0;
}

/* https://tools.ietf.org/html/rfc4648#section-6
 * Notice that base32hex differs from base32 in the alphabet it uses.
 * The distinction is that the base32hex representation preserves the
 * order of the underlying data when compared as bytestrings, this is
 * useful when representing NSEC3 hashes, as one can then verify the
 * order of hashes directly from their representation. */
वर्ण base32hexchar(पूर्णाङ्क x) {
        अचर वर्ण *table = DIGITS "ABCDEFGHIJKLMNOPQRSTUV";
        फर्काऊ table[x & 31];
}

पूर्णाङ्क unbase32hexchar(वर्ण c) {
        अचिन्हित offset;

        यदि (c >= '0' && c <= '9')
                फर्काऊ c - '0';

        offset = '9' - '0' + 1;

        यदि (c >= 'A' && c <= 'V')
                फर्काऊ c - 'A' + offset;

        फर्काऊ -EINVAL;
}

वर्ण* base32hexmem(अचर शून्य *p, माप_प्रकार l, bool padding) {
        वर्ण *r, *z;
        अचर uint8_t *x;
        माप_प्रकार len;

        प्रतिज्ञा(p || l == 0);

        यदि (padding)
                /* five input bytes makes eight output bytes, padding is added so we must round up */
                len = 8 * (l + 4) / 5;
        अन्यथा {
                /* same, but round down as there is no padding */
                len = 8 * l / 5;

                विकल्प (l % 5) {
                पक्ष 4:
                        len += 7;
                        रोक;
                पक्ष 3:
                        len += 5;
                        रोक;
                पक्ष 2:
                        len += 4;
                        रोक;
                पक्ष 1:
                        len += 2;
                        रोक;
                }
        }

        z = r = दो_स्मृति(len + 1);
        यदि (!r)
                फर्काऊ रिक्त;

        लागि (x = p; x < (अचर uint8_t*) p + (l / 5) * 5; x += 5) {
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

        विकल्प (l % 5) {
        पक्ष 4:
                *(z++) = base32hexchar(x[0] >> 3);                    /* 000XXXXX */
                *(z++) = base32hexchar((x[0] & 7) << 2 | x[1] >> 6);  /* 000XXXYY */
                *(z++) = base32hexchar((x[1] & 63) >> 1);             /* 000YYYYY */
                *(z++) = base32hexchar((x[1] & 1) << 4 | x[2] >> 4);   /* 000YZZZZ */
                *(z++) = base32hexchar((x[2] & 15) << 1 | x[3] >> 7); /* 000ZZZZQ */
                *(z++) = base32hexchar((x[3] & 127) >> 2);            /* 000QQQQQ */
                *(z++) = base32hexchar((x[3] & 3) << 3);              /* 000QQ000 */
                यदि (padding)
                        *(z++) = '=';

                रोक;

        पक्ष 3:
                *(z++) = base32hexchar(x[0] >> 3);                   /* 000XXXXX */
                *(z++) = base32hexchar((x[0] & 7) << 2 | x[1] >> 6); /* 000XXXYY */
                *(z++) = base32hexchar((x[1] & 63) >> 1);            /* 000YYYYY */
                *(z++) = base32hexchar((x[1] & 1) << 4 | x[2] >> 4); /* 000YZZZZ */
                *(z++) = base32hexchar((x[2] & 15) << 1);            /* 000ZZZZ0 */
                यदि (padding) {
                        *(z++) = '=';
                        *(z++) = '=';
                        *(z++) = '=';
                }

                रोक;

        पक्ष 2:
                *(z++) = base32hexchar(x[0] >> 3);                   /* 000XXXXX */
                *(z++) = base32hexchar((x[0] & 7) << 2 | x[1] >> 6); /* 000XXXYY */
                *(z++) = base32hexchar((x[1] & 63) >> 1);            /* 000YYYYY */
                *(z++) = base32hexchar((x[1] & 1) << 4);             /* 000Y0000 */
                यदि (padding) {
                        *(z++) = '=';
                        *(z++) = '=';
                        *(z++) = '=';
                        *(z++) = '=';
                }

                रोक;

        पक्ष 1:
                *(z++) = base32hexchar(x[0] >> 3);       /* 000XXXXX */
                *(z++) = base32hexchar((x[0] & 7) << 2); /* 000XXX00 */
                यदि (padding) {
                        *(z++) = '=';
                        *(z++) = '=';
                        *(z++) = '=';
                        *(z++) = '=';
                        *(z++) = '=';
                        *(z++) = '=';
                }

                रोक;
        }

        *z = 0;
        फर्काऊ r;
}

पूर्णाङ्क unbase32hexmem(अचर वर्ण *p, माप_प्रकार l, bool padding, शून्य **mem, माप_प्रकार *_len) {
        _cleanup_free_ uint8_t *r = रिक्त;
        पूर्णाङ्क a, b, c, d, e, f, g, h;
        uint8_t *z;
        अचर वर्ण *x;
        माप_प्रकार len;
        अचिन्हित pad = 0;

        प्रतिज्ञा(p || l == 0);
        प्रतिज्ञा(mem);
        प्रतिज्ञा(_len);

        यदि (l == SIZE_MAX)
                l = म_माप(p);

        /* padding ensures any base32hex input has input divisible by 8 */
        यदि (padding && l % 8 != 0)
                फर्काऊ -EINVAL;

        यदि (padding) {
                /* strip the padding */
                जबसम्म (l > 0 && p[l - 1] == '=' && pad < 7) {
                        pad++;
                        l--;
                }
        }

        /* a group of eight input bytes needs five output bytes, in case of
         * padding we need to add some extra bytes */
        len = (l / 8) * 5;

        विकल्प (l % 8) {
        पक्ष 7:
                len += 4;
                रोक;
        पक्ष 5:
                len += 3;
                रोक;
        पक्ष 4:
                len += 2;
                रोक;
        पक्ष 2:
                len += 1;
                रोक;
        पक्ष 0:
                रोक;
        स्वभाव:
                फर्काऊ -EINVAL;
        }

        z = r = दो_स्मृति(len + 1);
        यदि (!r)
                फर्काऊ -ENOMEM;

        लागि (x = p; x < p + (l / 8) * 8; x += 8) {
                /* a == 000XXXXX; b == 000YYYYY; c == 000ZZZZZ; d == 000WWWWW
                 * e == 000SSSSS; f == 000QQQQQ; g == 000VVVVV; h == 000RRRRR */
                a = unbase32hexchar(x[0]);
                यदि (a < 0)
                        फर्काऊ -EINVAL;

                b = unbase32hexchar(x[1]);
                यदि (b < 0)
                        फर्काऊ -EINVAL;

                c = unbase32hexchar(x[2]);
                यदि (c < 0)
                        फर्काऊ -EINVAL;

                d = unbase32hexchar(x[3]);
                यदि (d < 0)
                        फर्काऊ -EINVAL;

                e = unbase32hexchar(x[4]);
                यदि (e < 0)
                        फर्काऊ -EINVAL;

                f = unbase32hexchar(x[5]);
                यदि (f < 0)
                        फर्काऊ -EINVAL;

                g = unbase32hexchar(x[6]);
                यदि (g < 0)
                        फर्काऊ -EINVAL;

                h = unbase32hexchar(x[7]);
                यदि (h < 0)
                        फर्काऊ -EINVAL;

                *(z++) = (uint8_t) a << 3 | (uint8_t) b >> 2;                    /* XXXXXYYY */
                *(z++) = (uint8_t) b << 6 | (uint8_t) c << 1 | (uint8_t) d >> 4; /* YYZZZZZW */
                *(z++) = (uint8_t) d << 4 | (uint8_t) e >> 1;                    /* WWWWSSSS */
                *(z++) = (uint8_t) e << 7 | (uint8_t) f << 2 | (uint8_t) g >> 3; /* SQQQQQVV */
                *(z++) = (uint8_t) g << 5 | (uint8_t) h;                         /* VVVRRRRR */
        }

        विकल्प (l % 8) {
        पक्ष 7:
                a = unbase32hexchar(x[0]);
                यदि (a < 0)
                        फर्काऊ -EINVAL;

                b = unbase32hexchar(x[1]);
                यदि (b < 0)
                        फर्काऊ -EINVAL;

                c = unbase32hexchar(x[2]);
                यदि (c < 0)
                        फर्काऊ -EINVAL;

                d = unbase32hexchar(x[3]);
                यदि (d < 0)
                        फर्काऊ -EINVAL;

                e = unbase32hexchar(x[4]);
                यदि (e < 0)
                        फर्काऊ -EINVAL;

                f = unbase32hexchar(x[5]);
                यदि (f < 0)
                        फर्काऊ -EINVAL;

                g = unbase32hexchar(x[6]);
                यदि (g < 0)
                        फर्काऊ -EINVAL;

                /* g == 000VV000 */
                यदि (g & 7)
                        फर्काऊ -EINVAL;

                *(z++) = (uint8_t) a << 3 | (uint8_t) b >> 2;                    /* XXXXXYYY */
                *(z++) = (uint8_t) b << 6 | (uint8_t) c << 1 | (uint8_t) d >> 4; /* YYZZZZZW */
                *(z++) = (uint8_t) d << 4 | (uint8_t) e >> 1;                    /* WWWWSSSS */
                *(z++) = (uint8_t) e << 7 | (uint8_t) f << 2 | (uint8_t) g >> 3; /* SQQQQQVV */

                रोक;
        पक्ष 5:
                a = unbase32hexchar(x[0]);
                यदि (a < 0)
                        फर्काऊ -EINVAL;

                b = unbase32hexchar(x[1]);
                यदि (b < 0)
                        फर्काऊ -EINVAL;

                c = unbase32hexchar(x[2]);
                यदि (c < 0)
                        फर्काऊ -EINVAL;

                d = unbase32hexchar(x[3]);
                यदि (d < 0)
                        फर्काऊ -EINVAL;

                e = unbase32hexchar(x[4]);
                यदि (e < 0)
                        फर्काऊ -EINVAL;

                /* e == 000SSSS0 */
                यदि (e & 1)
                        फर्काऊ -EINVAL;

                *(z++) = (uint8_t) a << 3 | (uint8_t) b >> 2;                    /* XXXXXYYY */
                *(z++) = (uint8_t) b << 6 | (uint8_t) c << 1 | (uint8_t) d >> 4; /* YYZZZZZW */
                *(z++) = (uint8_t) d << 4 | (uint8_t) e >> 1;                    /* WWWWSSSS */

                रोक;
        पक्ष 4:
                a = unbase32hexchar(x[0]);
                यदि (a < 0)
                        फर्काऊ -EINVAL;

                b = unbase32hexchar(x[1]);
                यदि (b < 0)
                        फर्काऊ -EINVAL;

                c = unbase32hexchar(x[2]);
                यदि (c < 0)
                        फर्काऊ -EINVAL;

                d = unbase32hexchar(x[3]);
                यदि (d < 0)
                        फर्काऊ -EINVAL;

                /* d == 000W0000 */
                यदि (d & 15)
                        फर्काऊ -EINVAL;

                *(z++) = (uint8_t) a << 3 | (uint8_t) b >> 2;                    /* XXXXXYYY */
                *(z++) = (uint8_t) b << 6 | (uint8_t) c << 1 | (uint8_t) d >> 4; /* YYZZZZZW */

                रोक;
        पक्ष 2:
                a = unbase32hexchar(x[0]);
                यदि (a < 0)
                        फर्काऊ -EINVAL;

                b = unbase32hexchar(x[1]);
                यदि (b < 0)
                        फर्काऊ -EINVAL;

                /* b == 000YYY00 */
                यदि (b & 3)
                        फर्काऊ -EINVAL;

                *(z++) = (uint8_t) a << 3 | (uint8_t) b >> 2; /* XXXXXYYY */

                रोक;
        पक्ष 0:
                रोक;
        स्वभाव:
                फर्काऊ -EINVAL;
        }

        *z = 0;

        *mem = TAKE_PTR(r);
        *_len = len;

        फर्काऊ 0;
}

/* https://tools.ietf.org/html/rfc4648#section-4 */
वर्ण base64char(पूर्णाङ्क x) {
        अचर वर्ण *table = UPPERCASE_LETTERS LOWERCASE_LETTERS DIGITS "+/";
        फर्काऊ table[x & 63];
}

/* This is almost base64char(), but not entirely, as it uses the "url and filename safe" alphabet,
 * since we don't want "/" appear in interface names (since interfaces appear in sysfs as filenames).
 * See section #5 of RFC 4648. */
वर्ण urlsafe_base64char(पूर्णाङ्क x) {
        अचर वर्ण *table = UPPERCASE_LETTERS LOWERCASE_LETTERS DIGITS "-_";
        फर्काऊ table[x & 63];
}

पूर्णाङ्क unbase64char(वर्ण c) {
        अचिन्हित offset;

        यदि (c >= 'A' && c <= 'Z')
                फर्काऊ c - 'A';

        offset = 'Z' - 'A' + 1;

        यदि (c >= 'a' && c <= 'z')
                फर्काऊ c - 'a' + offset;

        offset += 'z' - 'a' + 1;

        यदि (c >= '0' && c <= '9')
                फर्काऊ c - '0' + offset;

        offset += '9' - '0' + 1;

        यदि (IN_SET(c, '+', '-')) /* Support both the regular and the URL safe character set (see above) */
                फर्काऊ offset;

        offset++;

        यदि (IN_SET(c, '/', '_')) /* ditto */
                फर्काऊ offset;

        फर्काऊ -EINVAL;
}

स्थिर शून्य maybe_पङ्क्ति_break(वर्ण **x, वर्ण *start, माप_प्रकार पङ्क्ति_break) {
        माप_प्रकार n;

        प्रतिज्ञा(x);
        प्रतिज्ञा(*x);
        प्रतिज्ञा(start);
        प्रतिज्ञा(*x >= start);

        यदि (पङ्क्ति_break == SIZE_MAX)
                फर्काऊ;

        n = *x - start;

        यदि (n % (पङ्क्ति_break + 1) == पङ्क्ति_break)
                *((*x)++) = '\n';
}

ssize_t base64mem_full(
                अचर शून्य *p,
                माप_प्रकार l,
                माप_प्रकार पङ्क्ति_break,
                वर्ण **ret) {

        अचर uint8_t *x;
        वर्ण *b, *z;
        माप_प्रकार m;

        प्रतिज्ञा(p || l == 0);
        प्रतिज्ञा(पङ्क्ति_break > 0);
        प्रतिज्ञा(ret);

        /* three input bytes makes four output bytes, padding is added so we must round up */
        m = 4 * (l + 2) / 3 + 1;
        यदि (पङ्क्ति_break != SIZE_MAX)
                m += m / पङ्क्ति_break;

        z = b = दो_स्मृति(m);
        यदि (!b)
                फर्काऊ -ENOMEM;

        लागि (x = p; x && x < (अचर uint8_t*) p + (l / 3) * 3; x += 3) {
                /* x[0] == XXXXXXXX; x[1] == YYYYYYYY; x[2] == ZZZZZZZZ */
                maybe_पङ्क्ति_break(&z, b, पङ्क्ति_break);
                *(z++) = base64char(x[0] >> 2);                    /* 00XXXXXX */
                maybe_पङ्क्ति_break(&z, b, पङ्क्ति_break);
                *(z++) = base64char((x[0] & 3) << 4 | x[1] >> 4);  /* 00XXYYYY */
                maybe_पङ्क्ति_break(&z, b, पङ्क्ति_break);
                *(z++) = base64char((x[1] & 15) << 2 | x[2] >> 6); /* 00YYYYZZ */
                maybe_पङ्क्ति_break(&z, b, पङ्क्ति_break);
                *(z++) = base64char(x[2] & 63);                    /* 00ZZZZZZ */
        }

        विकल्प (l % 3) {
        पक्ष 2:
                maybe_पङ्क्ति_break(&z, b, पङ्क्ति_break);
                *(z++) = base64char(x[0] >> 2);                   /* 00XXXXXX */
                maybe_पङ्क्ति_break(&z, b, पङ्क्ति_break);
                *(z++) = base64char((x[0] & 3) << 4 | x[1] >> 4); /* 00XXYYYY */
                maybe_पङ्क्ति_break(&z, b, पङ्क्ति_break);
                *(z++) = base64char((x[1] & 15) << 2);            /* 00YYYY00 */
                maybe_पङ्क्ति_break(&z, b, पङ्क्ति_break);
                *(z++) = '=';
                रोक;

        पक्ष 1:
                maybe_पङ्क्ति_break(&z, b, पङ्क्ति_break);
                *(z++) = base64char(x[0] >> 2);        /* 00XXXXXX */
                maybe_पङ्क्ति_break(&z, b, पङ्क्ति_break);
                *(z++) = base64char((x[0] & 3) << 4);  /* 00XX0000 */
                maybe_पङ्क्ति_break(&z, b, पङ्क्ति_break);
                *(z++) = '=';
                maybe_पङ्क्ति_break(&z, b, पङ्क्ति_break);
                *(z++) = '=';
                रोक;
        }

        *z = 0;
        *ret = b;

        प्रतिज्ञा(z >= b); /* Let static analyzers know that the answer is non-negative. */
        फर्काऊ z - b;
}

स्थिर ssize_t base64_append_width(
                वर्ण **prefix,
                माप_प्रकार plen,
                वर्ण sep,
                माप_प्रकार indent,
                अचर शून्य *p,
                माप_प्रकार l,
                माप_प्रकार width) {

        _cleanup_free_ वर्ण *x = रिक्त;
        वर्ण *t, *s;
        माप_प्रकार पङ्क्तिहरू;
        ssize_t len;

        प्रतिज्ञा(prefix);
        प्रतिज्ञा(*prefix || plen == 0);
        प्रतिज्ञा(p || l == 0);

        len = base64mem(p, l, &x);
        यदि (len < 0)
                फर्काऊ len;
        यदि (len == 0)
                फर्काऊ plen;

        पङ्क्तिहरू = DIV_ROUND_UP(len, width);

        यदि (plen >= SSIZE_MAX - 1 - 1 ||
            पङ्क्तिहरू > (SSIZE_MAX - plen - 1 - 1) / (indent + width + 1))
                फर्काऊ -ENOMEM;

        t = पुनः_स्मृति(*prefix, plen + 1 + 1 + (indent + width + 1) * पङ्क्तिहरू);
        यदि (!t)
                फर्काऊ -ENOMEM;

        s = t + plen;
        लागि (माप_प्रकार पङ्क्ति = 0; पङ्क्ति < पङ्क्तिहरू; पङ्क्ति++) {
                माप_प्रकार act = MIN(width, (माप_प्रकार) len);

                यदि (पङ्क्ति > 0)
                        sep = '\n';

                यदि (s > t) {
                        *s++ = sep;
                        यदि (sep == '\n')
                                s = mempset(s, ' ', indent);
                }

                s = mempcpy(s, x + width * पङ्क्ति, act);
                len -= act;
        }
        प्रतिज्ञा(len == 0);

        *s = '\0';
        *prefix = t;
        फर्काऊ s - t;
}

ssize_t base64_append(
                वर्ण **prefix,
                माप_प्रकार plen,
                अचर शून्य *p,
                माप_प्रकार l,
                माप_प्रकार indent,
                माप_प्रकार width) {

        यदि (plen > width / 2 || plen + indent > width)
                /* leave indent on the left, keep last column free */
                फर्काऊ base64_append_width(prefix, plen, '\n', indent, p, l, width - indent);
        अन्यथा
                /* leave plen on the left, keep last column free */
                फर्काऊ base64_append_width(prefix, plen, ' ', plen + 1, p, l, width - plen - 1);
}

स्थिर पूर्णाङ्क unbase64_next(अचर वर्ण **p, माप_प्रकार *l) {
        पूर्णाङ्क ret;

        प्रतिज्ञा(p);
        प्रतिज्ञा(l);

        /* Find the next non-whitespace character, and decode it. If we find padding, we return it as INT_MAX. We
         * greedily skip all preceding and all following whitespace. */

        लागि (;;) {
                यदि (*l == 0)
                        फर्काऊ -EPIPE;

                यदि (!म_अक्षर(WHITESPACE, **p))
                        रोक;

                /* Skip leading whitespace */
                (*p)++, (*l)--;
        }

        यदि (**p == '=')
                ret = पूर्णांक_उच्च; /* return padding as INT_MAX */
        अन्यथा {
                ret = unbase64char(**p);
                यदि (ret < 0)
                        फर्काऊ ret;
        }

        लागि (;;) {
                (*p)++, (*l)--;

                यदि (*l == 0)
                        रोक;
                यदि (!म_अक्षर(WHITESPACE, **p))
                        रोक;

                /* Skip following whitespace */
        }

        फर्काऊ ret;
}

पूर्णाङ्क unbase64mem_full(
                अचर वर्ण *p,
                माप_प्रकार l,
                bool secure,
                शून्य **ret_data,
                माप_प्रकार *ret_size) {

        _cleanup_free_ uint8_t *buf = रिक्त;
        अचर वर्ण *x;
        uint8_t *z;
        माप_प्रकार len;

        प्रतिज्ञा(p || l == 0);

        यदि (l == SIZE_MAX)
                l = म_माप(p);

        /* A group of four input bytes needs three output bytes, in case of padding we need to add two or three extra
         * bytes. Note that this calculation is an upper boundary, as we ignore whitespace while decoding */
        len = (l / 4) * 3 + (l % 4 != 0 ? (l % 4) - 1 : 0);

        buf = दो_स्मृति(len + 1);
        यदि (!buf)
                फर्काऊ -ENOMEM;

        CLEANUP_ERASE_PTR(secure ? &buf : रिक्त, len);

        लागि (x = p, z = buf;;) {
                पूर्णाङ्क a, b, c, d; /* a == 00XXXXXX; b == 00YYYYYY; c == 00ZZZZZZ; d == 00WWWWWW */

                a = unbase64_next(&x, &l);
                यदि (a == -EPIPE) /* End of string */
                        रोक;
                यदि (a < 0)
                        फर्काऊ a;
                यदि (a == पूर्णांक_उच्च) /* Padding is not allowed at the beginning of a 4ch block */
                        फर्काऊ -EINVAL;

                b = unbase64_next(&x, &l);
                यदि (b < 0)
                        फर्काऊ b;
                यदि (b == पूर्णांक_उच्च) /* Padding is not allowed at the second character of a 4ch block either */
                        फर्काऊ -EINVAL;

                c = unbase64_next(&x, &l);
                यदि (c < 0)
                        फर्काऊ c;

                d = unbase64_next(&x, &l);
                यदि (d < 0)
                        फर्काऊ d;

                यदि (c == पूर्णांक_उच्च) { /* Padding at the third character */

                        यदि (d != पूर्णांक_उच्च) /* If the third character is padding, the fourth must be too */
                                फर्काऊ -EINVAL;

                        /* b == 00YY0000 */
                        यदि (b & 15)
                                फर्काऊ -EINVAL;

                        यदि (l > 0) /* Trailing rubbish? */
                                फर्काऊ -ENAMETOOLONG;

                        *(z++) = (uint8_t) a << 2 | (uint8_t) (b >> 4); /* XXXXXXYY */
                        रोक;
                }

                यदि (d == पूर्णांक_उच्च) {
                        /* c == 00ZZZZ00 */
                        यदि (c & 3)
                                फर्काऊ -EINVAL;

                        यदि (l > 0) /* Trailing rubbish? */
                                फर्काऊ -ENAMETOOLONG;

                        *(z++) = (uint8_t) a << 2 | (uint8_t) b >> 4; /* XXXXXXYY */
                        *(z++) = (uint8_t) b << 4 | (uint8_t) c >> 2; /* YYYYZZZZ */
                        रोक;
                }

                *(z++) = (uint8_t) a << 2 | (uint8_t) b >> 4; /* XXXXXXYY */
                *(z++) = (uint8_t) b << 4 | (uint8_t) c >> 2; /* YYYYZZZZ */
                *(z++) = (uint8_t) c << 6 | (uint8_t) d;      /* ZZWWWWWW */
        }

        *z = 0;

        प्रतिज्ञा((माप_प्रकार) (z - buf) <= len);

        यदि (ret_size)
                *ret_size = (माप_प्रकार) (z - buf);
        यदि (ret_data)
                *ret_data = TAKE_PTR(buf);

        फर्काऊ 0;
}

शून्य hexdump(खाता *f, अचर शून्य *p, माप_प्रकार s) {
        अचर uint8_t *b = p;
        अचिन्हित n = 0;

        प्रतिज्ञा(b || s == 0);

        यदि (s == SIZE_MAX)
                s = म_माप(p);

        यदि (!f)
                f = मानक_निकास;

        जबसम्म (s > 0) {
                माप_प्रकार i;

                ख_लिखो(f, "%04x  ", n);

                लागि (i = 0; i < 16; i++) {

                        यदि (i >= s)
                                ख_माला_दो("   ", f);
                        अन्यथा
                                ख_लिखो(f, "%02x ", b[i]);

                        यदि (i == 7)
                                ख_अक्षर_दो(' ', f);
                }

                ख_अक्षर_दो(' ', f);

                लागि (i = 0; i < 16; i++) {

                        यदि (i >= s)
                                ख_अक्षर_दो(' ', f);
                        अन्यथा
                                ख_अक्षर_दो(है_छाप(b[i]) ? (वर्ण) b[i] : '.', f);
                }

                ख_अक्षर_दो('\n', f);

                यदि (s < 16)
                        रोक;

                n += 16;
                b += 16;
                s -= 16;
        }
}
