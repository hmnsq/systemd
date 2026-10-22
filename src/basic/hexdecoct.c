/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव <ctype.h>
#अन्तर्भाव <stdio.h>

#अन्तर्भाव "alloc-util.h"
#अन्तर्भाव "hexdecoct.h"
#अन्तर्भाव "memory-util.h"
#अन्तर्भाव "string-util.h"

वर्ण octchar(पूर्णाङ्क x) {
        प्रत्यागम '0' + (x & 7);
}

पूर्णाङ्क unoctchar(वर्ण c) {

        यदि (c >= '0' && c <= '7')
                प्रत्यागम c - '0';

        प्रत्यागम -EINVAL;
}

वर्ण decchar(पूर्णाङ्क x) {
        प्रत्यागम '0' + (x % 10);
}

पूर्णाङ्क undecchar(वर्ण c) {

        यदि (c >= '0' && c <= '9')
                प्रत्यागम c - '0';

        प्रत्यागम -EINVAL;
}

वर्ण hexchar(पूर्णाङ्क x) {
        ध्रुव वर्ण *सारणी = LOWERCASE_HEXDIGITS;
        प्रत्यागम सारणी[x & 15];
}

पूर्णाङ्क unhexchar(वर्ण c) {

        यदि (c >= '0' && c <= '9')
                प्रत्यागम c - '0';

        यदि (c >= 'a' && c <= 'f')
                प्रत्यागम c - 'a' + 10;

        यदि (c >= 'A' && c <= 'F')
                प्रत्यागम c - 'A' + 10;

        प्रत्यागम -EINVAL;
}

वर्ण* hexmem(ध्रुव शून्य *p, size_t l) {
        ध्रुव uint8_t *x;
        वर्ण *r, *z;

        प्रतिज्ञा(p || l == 0);

        z = r = new(वर्ण, l * 2 + 1);
        यदि (!r)
                प्रत्यागम रिक्त;

        प्रति (x = p; x && x < (ध्रुव uint8_t*) p + l; x++) {
                *(z++) = hexchar(*x >> 4);
                *(z++) = hexchar(*x & 15);
        }

        *z = 0;
        प्रत्यागम r;
}

स्थिर पूर्णाङ्क unhex_अग्र(ध्रुव वर्ण **p, size_t *l) {
        पूर्णाङ्क r;

        प्रतिज्ञा(p);
        प्रतिज्ञा(l);

        /* Find the next non-whitespace character, and decode it. We
         * greedily skip all preceding and all following whitespace. */

        प्रति (;;) {
                यदि (*l == 0)
                        प्रत्यागम -EPIPE;

                यदि (!strchr(WHITESPACE, **p))
                        भङ्ग;

                /* Skip leading whitespace */
                (*p)++, (*l)--;
        }

        r = unhexchar(**p);
        यदि (r < 0)
                प्रत्यागम r;

        प्रति (;;) {
                (*p)++, (*l)--;

                यदि (*l == 0 || !strchr(WHITESPACE, **p))
                        भङ्ग;

                /* Skip following whitespace */
        }

        प्रत्यागम r;
}

पूर्णाङ्क unhexmem_पूर्णम्(
                ध्रुव वर्ण *p,
                size_t l,
                bool secure,
                शून्य **ret_दत्त,
                size_t *ret_आकार) {

        _cleanup_free_ uint8_t *buf = रिक्त;
        size_t buf_size;
        ध्रुव वर्ण *x;
        uint8_t *z;

        प्रतिज्ञा(p || l == 0);

        यदि (l == SIZE_MAX)
                l = वाक्यदैर्घ्यम्(p);

        /* Note that the calculation of memory size is an upper boundary, as we ignore whitespace while decoding */
        buf_size = (l + 1) / 2 + 1;
        buf = स्मृतिं_देहि(buf_size);
        यदि (!buf)
                प्रत्यागम -ENOMEM;

        CLEANUP_ERASE_PTR(secure ? &buf : रिक्त, buf_size);

        प्रति (x = p, z = buf;;) {
                पूर्णाङ्क a, b;

                a = unhex_अग्र(&x, &l);
                यदि (a == -EPIPE) /* End of string */
                        भङ्ग;
                यदि (a < 0)
                        प्रत्यागम a;

                b = unhex_अग्र(&x, &l);
                यदि (b < 0)
                        प्रत्यागम b;

                *(z++) = (uint8_t) a << 4 | (uint8_t) b;
        }

        *z = 0;

        यदि (ret_आकार)
                *ret_आकार = (size_t) (z - buf);
        यदि (ret_दत्त)
                *ret_दत्त = TAKE_PTR(buf);

        प्रत्यागम 0;
}

/* https://tools.ietf.org/html/rfc4648#section-6
 * Notice that base32hex differs from base32 in the alphabet it uses.
 * The distinction is that the base32hex representation preserves the
 * order of the underlying data when compared as bytestrings, this is
 * useful when representing NSEC3 hashes, as one can then verify the
 * order of hashes directly from their representation. */
वर्ण base32hexchar(पूर्णाङ्क x) {
        ध्रुव वर्ण *सारणी = DIGITS "ABCDEFGHIJKLMNOPQRSTUV";
        प्रत्यागम सारणी[x & 31];
}

पूर्णाङ्क unbase32hexchar(वर्ण c) {
        अचिह्नित विस्थापनम्;

        यदि (c >= '0' && c <= '9')
                प्रत्यागम c - '0';

        विस्थापनम् = '9' - '0' + 1;

        यदि (c >= 'A' && c <= 'V')
                प्रत्यागम c - 'A' + विस्थापनम्;

        प्रत्यागम -EINVAL;
}

वर्ण* base32hexmem(ध्रुव शून्य *p, size_t l, bool padding) {
        वर्ण *r, *z;
        ध्रुव uint8_t *x;
        size_t len;

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
                        भङ्ग;
                पक्ष 3:
                        len += 5;
                        भङ्ग;
                पक्ष 2:
                        len += 4;
                        भङ्ग;
                पक्ष 1:
                        len += 2;
                        भङ्ग;
                }
        }

        z = r = स्मृतिं_देहि(len + 1);
        यदि (!r)
                प्रत्यागम रिक्त;

        प्रति (x = p; x < (ध्रुव uint8_t*) p + (l / 5) * 5; x += 5) {
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

                भङ्ग;

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

                भङ्ग;

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

                भङ्ग;

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

                भङ्ग;
        }

        *z = 0;
        प्रत्यागम r;
}

पूर्णाङ्क unbase32hexmem(ध्रुव वर्ण *p, size_t l, bool padding, शून्य **mem, size_t *_len) {
        _cleanup_free_ uint8_t *r = रिक्त;
        पूर्णाङ्क a, b, c, d, e, f, g, h;
        uint8_t *z;
        ध्रुव वर्ण *x;
        size_t len;
        अचिह्नित pad = 0;

        प्रतिज्ञा(p || l == 0);
        प्रतिज्ञा(mem);
        प्रतिज्ञा(_len);

        यदि (l == SIZE_MAX)
                l = वाक्यदैर्घ्यम्(p);

        /* padding ensures any base32hex input has input divisible by 8 */
        यदि (padding && l % 8 != 0)
                प्रत्यागम -EINVAL;

        यदि (padding) {
                /* strip the padding */
                यावत् (l > 0 && p[l - 1] == '=' && pad < 7) {
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
                भङ्ग;
        पक्ष 5:
                len += 3;
                भङ्ग;
        पक्ष 4:
                len += 2;
                भङ्ग;
        पक्ष 2:
                len += 1;
                भङ्ग;
        पक्ष 0:
                भङ्ग;
        स्वभाव:
                प्रत्यागम -EINVAL;
        }

        z = r = स्मृतिं_देहि(len + 1);
        यदि (!r)
                प्रत्यागम -ENOMEM;

        प्रति (x = p; x < p + (l / 8) * 8; x += 8) {
                /* a == 000XXXXX; b == 000YYYYY; c == 000ZZZZZ; d == 000WWWWW
                 * e == 000SSSSS; f == 000QQQQQ; g == 000VVVVV; h == 000RRRRR */
                a = unbase32hexchar(x[0]);
                यदि (a < 0)
                        प्रत्यागम -EINVAL;

                b = unbase32hexchar(x[1]);
                यदि (b < 0)
                        प्रत्यागम -EINVAL;

                c = unbase32hexchar(x[2]);
                यदि (c < 0)
                        प्रत्यागम -EINVAL;

                d = unbase32hexchar(x[3]);
                यदि (d < 0)
                        प्रत्यागम -EINVAL;

                e = unbase32hexchar(x[4]);
                यदि (e < 0)
                        प्रत्यागम -EINVAL;

                f = unbase32hexchar(x[5]);
                यदि (f < 0)
                        प्रत्यागम -EINVAL;

                g = unbase32hexchar(x[6]);
                यदि (g < 0)
                        प्रत्यागम -EINVAL;

                h = unbase32hexchar(x[7]);
                यदि (h < 0)
                        प्रत्यागम -EINVAL;

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
                        प्रत्यागम -EINVAL;

                b = unbase32hexchar(x[1]);
                यदि (b < 0)
                        प्रत्यागम -EINVAL;

                c = unbase32hexchar(x[2]);
                यदि (c < 0)
                        प्रत्यागम -EINVAL;

                d = unbase32hexchar(x[3]);
                यदि (d < 0)
                        प्रत्यागम -EINVAL;

                e = unbase32hexchar(x[4]);
                यदि (e < 0)
                        प्रत्यागम -EINVAL;

                f = unbase32hexchar(x[5]);
                यदि (f < 0)
                        प्रत्यागम -EINVAL;

                g = unbase32hexchar(x[6]);
                यदि (g < 0)
                        प्रत्यागम -EINVAL;

                /* g == 000VV000 */
                यदि (g & 7)
                        प्रत्यागम -EINVAL;

                *(z++) = (uint8_t) a << 3 | (uint8_t) b >> 2;                    /* XXXXXYYY */
                *(z++) = (uint8_t) b << 6 | (uint8_t) c << 1 | (uint8_t) d >> 4; /* YYZZZZZW */
                *(z++) = (uint8_t) d << 4 | (uint8_t) e >> 1;                    /* WWWWSSSS */
                *(z++) = (uint8_t) e << 7 | (uint8_t) f << 2 | (uint8_t) g >> 3; /* SQQQQQVV */

                भङ्ग;
        पक्ष 5:
                a = unbase32hexchar(x[0]);
                यदि (a < 0)
                        प्रत्यागम -EINVAL;

                b = unbase32hexchar(x[1]);
                यदि (b < 0)
                        प्रत्यागम -EINVAL;

                c = unbase32hexchar(x[2]);
                यदि (c < 0)
                        प्रत्यागम -EINVAL;

                d = unbase32hexchar(x[3]);
                यदि (d < 0)
                        प्रत्यागम -EINVAL;

                e = unbase32hexchar(x[4]);
                यदि (e < 0)
                        प्रत्यागम -EINVAL;

                /* e == 000SSSS0 */
                यदि (e & 1)
                        प्रत्यागम -EINVAL;

                *(z++) = (uint8_t) a << 3 | (uint8_t) b >> 2;                    /* XXXXXYYY */
                *(z++) = (uint8_t) b << 6 | (uint8_t) c << 1 | (uint8_t) d >> 4; /* YYZZZZZW */
                *(z++) = (uint8_t) d << 4 | (uint8_t) e >> 1;                    /* WWWWSSSS */

                भङ्ग;
        पक्ष 4:
                a = unbase32hexchar(x[0]);
                यदि (a < 0)
                        प्रत्यागम -EINVAL;

                b = unbase32hexchar(x[1]);
                यदि (b < 0)
                        प्रत्यागम -EINVAL;

                c = unbase32hexchar(x[2]);
                यदि (c < 0)
                        प्रत्यागम -EINVAL;

                d = unbase32hexchar(x[3]);
                यदि (d < 0)
                        प्रत्यागम -EINVAL;

                /* d == 000W0000 */
                यदि (d & 15)
                        प्रत्यागम -EINVAL;

                *(z++) = (uint8_t) a << 3 | (uint8_t) b >> 2;                    /* XXXXXYYY */
                *(z++) = (uint8_t) b << 6 | (uint8_t) c << 1 | (uint8_t) d >> 4; /* YYZZZZZW */

                भङ्ग;
        पक्ष 2:
                a = unbase32hexchar(x[0]);
                यदि (a < 0)
                        प्रत्यागम -EINVAL;

                b = unbase32hexchar(x[1]);
                यदि (b < 0)
                        प्रत्यागम -EINVAL;

                /* b == 000YYY00 */
                यदि (b & 3)
                        प्रत्यागम -EINVAL;

                *(z++) = (uint8_t) a << 3 | (uint8_t) b >> 2; /* XXXXXYYY */

                भङ्ग;
        पक्ष 0:
                भङ्ग;
        स्वभाव:
                प्रत्यागम -EINVAL;
        }

        *z = 0;

        *mem = TAKE_PTR(r);
        *_len = len;

        प्रत्यागम 0;
}

/* https://tools.ietf.org/html/rfc4648#section-4 */
वर्ण base64char(पूर्णाङ्क x) {
        ध्रुव वर्ण *सारणी = UPPERCASE_LETTERS LOWERCASE_LETTERS DIGITS "+/";
        प्रत्यागम सारणी[x & 63];
}

/* This is almost base64char(), but not entirely, as it uses the "url and filename safe" alphabet,
 * since we don't want "/" appear in interface names (since interfaces appear in sysfs as filenames).
 * See section #5 of RFC 4648. */
वर्ण urlsafe_base64char(पूर्णाङ्क x) {
        ध्रुव वर्ण *सारणी = UPPERCASE_LETTERS LOWERCASE_LETTERS DIGITS "-_";
        प्रत्यागम सारणी[x & 63];
}

पूर्णाङ्क unbase64char(वर्ण c) {
        अचिह्नित विस्थापनम्;

        यदि (c >= 'A' && c <= 'Z')
                प्रत्यागम c - 'A';

        विस्थापनम् = 'Z' - 'A' + 1;

        यदि (c >= 'a' && c <= 'z')
                प्रत्यागम c - 'a' + विस्थापनम्;

        विस्थापनम् += 'z' - 'a' + 1;

        यदि (c >= '0' && c <= '9')
                प्रत्यागम c - '0' + विस्थापनम्;

        विस्थापनम् += '9' - '0' + 1;

        यदि (अन्तर्_समुच्चयः(c, '+', '-')) /* Support both the regular and the URL safe character set (see above) */
                प्रत्यागम विस्थापनम्;

        विस्थापनम्++;

        यदि (अन्तर्_समुच्चयः(c, '/', '_')) /* ditto */
                प्रत्यागम विस्थापनम्;

        प्रत्यागम -EINVAL;
}

स्थिर शून्य maybe_पङ्क्तिः_break(वर्ण **x, वर्ण *start, size_t पङ्क्तिः_break) {
        size_t n;

        प्रतिज्ञा(x);
        प्रतिज्ञा(*x);
        प्रतिज्ञा(start);
        प्रतिज्ञा(*x >= start);

        यदि (पङ्क्तिः_break == SIZE_MAX)
                प्रत्यागम;

        n = *x - start;

        यदि (n % (पङ्क्तिः_break + 1) == पङ्क्तिः_break)
                *((*x)++) = '\n';
}

ssize_t base64mem_पूर्णम्(
                ध्रुव शून्य *p,
                size_t l,
                size_t पङ्क्तिः_break,
                वर्ण **ret) {

        ध्रुव uint8_t *x;
        वर्ण *b, *z;
        size_t m;

        प्रतिज्ञा(p || l == 0);
        प्रतिज्ञा(पङ्क्तिः_break > 0);
        प्रतिज्ञा(ret);

        /* three input bytes makes four output bytes, padding is added so we must round up */
        m = 4 * (l + 2) / 3 + 1;
        यदि (पङ्क्तिः_break != SIZE_MAX)
                m += m / पङ्क्तिः_break;

        z = b = स्मृतिं_देहि(m);
        यदि (!b)
                प्रत्यागम -ENOMEM;

        प्रति (x = p; x && x < (ध्रुव uint8_t*) p + (l / 3) * 3; x += 3) {
                /* x[0] == XXXXXXXX; x[1] == YYYYYYYY; x[2] == ZZZZZZZZ */
                maybe_पङ्क्तिः_break(&z, b, पङ्क्तिः_break);
                *(z++) = base64char(x[0] >> 2);                    /* 00XXXXXX */
                maybe_पङ्क्तिः_break(&z, b, पङ्क्तिः_break);
                *(z++) = base64char((x[0] & 3) << 4 | x[1] >> 4);  /* 00XXYYYY */
                maybe_पङ्क्तिः_break(&z, b, पङ्क्तिः_break);
                *(z++) = base64char((x[1] & 15) << 2 | x[2] >> 6); /* 00YYYYZZ */
                maybe_पङ्क्तिः_break(&z, b, पङ्क्तिः_break);
                *(z++) = base64char(x[2] & 63);                    /* 00ZZZZZZ */
        }

        विकल्प (l % 3) {
        पक्ष 2:
                maybe_पङ्क्तिः_break(&z, b, पङ्क्तिः_break);
                *(z++) = base64char(x[0] >> 2);                   /* 00XXXXXX */
                maybe_पङ्क्तिः_break(&z, b, पङ्क्तिः_break);
                *(z++) = base64char((x[0] & 3) << 4 | x[1] >> 4); /* 00XXYYYY */
                maybe_पङ्क्तिः_break(&z, b, पङ्क्तिः_break);
                *(z++) = base64char((x[1] & 15) << 2);            /* 00YYYY00 */
                maybe_पङ्क्तिः_break(&z, b, पङ्क्तिः_break);
                *(z++) = '=';
                भङ्ग;

        पक्ष 1:
                maybe_पङ्क्तिः_break(&z, b, पङ्क्तिः_break);
                *(z++) = base64char(x[0] >> 2);        /* 00XXXXXX */
                maybe_पङ्क्तिः_break(&z, b, पङ्क्तिः_break);
                *(z++) = base64char((x[0] & 3) << 4);  /* 00XX0000 */
                maybe_पङ्क्तिः_break(&z, b, पङ्क्तिः_break);
                *(z++) = '=';
                maybe_पङ्क्तिः_break(&z, b, पङ्क्तिः_break);
                *(z++) = '=';
                भङ्ग;
        }

        *z = 0;
        *ret = b;

        प्रतिज्ञा(z >= b); /* Let static analyzers know that the answer is non-negative. */
        प्रत्यागम z - b;
}

स्थिर ssize_t base64_append_विस्तारः(
                वर्ण **उपसर्गः,
                size_t plen,
                वर्ण sep,
                size_t indent,
                ध्रुव शून्य *p,
                size_t l,
                size_t width) {

        _cleanup_free_ वर्ण *x = रिक्त;
        वर्ण *t, *s;
        size_t पङ्क्तयः;
        ssize_t len;

        प्रतिज्ञा(उपसर्गः);
        प्रतिज्ञा(*उपसर्गः || plen == 0);
        प्रतिज्ञा(p || l == 0);

        len = base64mem(p, l, &x);
        यदि (len < 0)
                प्रत्यागम len;
        यदि (len == 0)
                प्रत्यागम plen;

        पङ्क्तयः = DIV_आवर्तनम्_ऊर्ध्वम्(len, width);

        यदि (plen >= SSIZE_MAX - 1 - 1 ||
            पङ्क्तयः > (SSIZE_MAX - plen - 1 - 1) / (indent + width + 1))
                प्रत्यागम -ENOMEM;

        t = realloc(*उपसर्गः, plen + 1 + 1 + (indent + width + 1) * पङ्क्तयः);
        यदि (!t)
                प्रत्यागम -ENOMEM;

        s = t + plen;
        प्रति (size_t पङ्क्तिः = 0; पङ्क्तिः < पङ्क्तयः; पङ्क्तिः++) {
                size_t act = MIN(width, (size_t) len);

                यदि (पङ्क्तिः > 0)
                        sep = '\n';

                यदि (s > t) {
                        *s++ = sep;
                        यदि (sep == '\n')
                                s = mempset(s, ' ', indent);
                }

                s = mempcpy(s, x + width * पङ्क्तिः, act);
                len -= act;
        }
        प्रतिज्ञा(len == 0);

        *s = '\0';
        *उपसर्गः = t;
        प्रत्यागम s - t;
}

ssize_t base64_append(
                वर्ण **उपसर्गः,
                size_t plen,
                ध्रुव शून्य *p,
                size_t l,
                size_t indent,
                size_t width) {

        यदि (plen > width / 2 || plen + indent > width)
                /* leave indent on the left, keep last column free */
                प्रत्यागम base64_append_विस्तारः(उपसर्गः, plen, '\n', indent, p, l, width - indent);
        अन्यथा
                /* leave plen on the left, keep last column free */
                प्रत्यागम base64_append_विस्तारः(उपसर्गः, plen, ' ', plen + 1, p, l, width - plen - 1);
}

स्थिर पूर्णाङ्क unbase64_अग्र(ध्रुव वर्ण **p, size_t *l) {
        पूर्णाङ्क ret;

        प्रतिज्ञा(p);
        प्रतिज्ञा(l);

        /* Find the next non-whitespace character, and decode it. If we find padding, we return it as INT_MAX. We
         * greedily skip all preceding and all following whitespace. */

        प्रति (;;) {
                यदि (*l == 0)
                        प्रत्यागम -EPIPE;

                यदि (!strchr(WHITESPACE, **p))
                        भङ्ग;

                /* Skip leading whitespace */
                (*p)++, (*l)--;
        }

        यदि (**p == '=')
                ret = अधिकतम_पूर्णाङ्कः; /* return padding as INT_MAX */
        अन्यथा {
                ret = unbase64char(**p);
                यदि (ret < 0)
                        प्रत्यागम ret;
        }

        प्रति (;;) {
                (*p)++, (*l)--;

                यदि (*l == 0)
                        भङ्ग;
                यदि (!strchr(WHITESPACE, **p))
                        भङ्ग;

                /* Skip following whitespace */
        }

        प्रत्यागम ret;
}

पूर्णाङ्क unbase64mem_पूर्णम्(
                ध्रुव वर्ण *p,
                size_t l,
                bool secure,
                शून्य **ret_दत्त,
                size_t *ret_आकार) {

        _cleanup_free_ uint8_t *buf = रिक्त;
        ध्रुव वर्ण *x;
        uint8_t *z;
        size_t len;

        प्रतिज्ञा(p || l == 0);

        यदि (l == SIZE_MAX)
                l = वाक्यदैर्घ्यम्(p);

        /* A group of four input bytes needs three output bytes, in case of padding we need to add two or three extra
         * bytes. Note that this calculation is an upper boundary, as we ignore whitespace while decoding */
        len = (l / 4) * 3 + (l % 4 != 0 ? (l % 4) - 1 : 0);

        buf = स्मृतिं_देहि(len + 1);
        यदि (!buf)
                प्रत्यागम -ENOMEM;

        CLEANUP_ERASE_PTR(secure ? &buf : रिक्त, len);

        प्रति (x = p, z = buf;;) {
                पूर्णाङ्क a, b, c, d; /* a == 00XXXXXX; b == 00YYYYYY; c == 00ZZZZZZ; d == 00WWWWWW */

                a = unbase64_अग्र(&x, &l);
                यदि (a == -EPIPE) /* End of string */
                        भङ्ग;
                यदि (a < 0)
                        प्रत्यागम a;
                यदि (a == अधिकतम_पूर्णाङ्कः) /* Padding is not allowed at the beginning of a 4ch block */
                        प्रत्यागम -EINVAL;

                b = unbase64_अग्र(&x, &l);
                यदि (b < 0)
                        प्रत्यागम b;
                यदि (b == अधिकतम_पूर्णाङ्कः) /* Padding is not allowed at the second character of a 4ch block either */
                        प्रत्यागम -EINVAL;

                c = unbase64_अग्र(&x, &l);
                यदि (c < 0)
                        प्रत्यागम c;

                d = unbase64_अग्र(&x, &l);
                यदि (d < 0)
                        प्रत्यागम d;

                यदि (c == अधिकतम_पूर्णाङ्कः) { /* Padding at the third character */

                        यदि (d != अधिकतम_पूर्णाङ्कः) /* If the third character is padding, the fourth must be too */
                                प्रत्यागम -EINVAL;

                        /* b == 00YY0000 */
                        यदि (b & 15)
                                प्रत्यागम -EINVAL;

                        यदि (l > 0) /* Trailing rubbish? */
                                प्रत्यागम -ENAMETOOLONG;

                        *(z++) = (uint8_t) a << 2 | (uint8_t) (b >> 4); /* XXXXXXYY */
                        भङ्ग;
                }

                यदि (d == अधिकतम_पूर्णाङ्कः) {
                        /* c == 00ZZZZ00 */
                        यदि (c & 3)
                                प्रत्यागम -EINVAL;

                        यदि (l > 0) /* Trailing rubbish? */
                                प्रत्यागम -ENAMETOOLONG;

                        *(z++) = (uint8_t) a << 2 | (uint8_t) b >> 4; /* XXXXXXYY */
                        *(z++) = (uint8_t) b << 4 | (uint8_t) c >> 2; /* YYYYZZZZ */
                        भङ्ग;
                }

                *(z++) = (uint8_t) a << 2 | (uint8_t) b >> 4; /* XXXXXXYY */
                *(z++) = (uint8_t) b << 4 | (uint8_t) c >> 2; /* YYYYZZZZ */
                *(z++) = (uint8_t) c << 6 | (uint8_t) d;      /* ZZWWWWWW */
        }

        *z = 0;

        प्रतिज्ञा((size_t) (z - buf) <= len);

        यदि (ret_आकार)
                *ret_आकार = (size_t) (z - buf);
        यदि (ret_दत्त)
                *ret_दत्त = TAKE_PTR(buf);

        प्रत्यागम 0;
}

शून्य hexdump(FILE *f, ध्रुव शून्य *p, size_t s) {
        ध्रुव uint8_t *b = p;
        अचिह्नित n = 0;

        प्रतिज्ञा(b || s == 0);

        यदि (s == SIZE_MAX)
                s = वाक्यदैर्घ्यम्(p);

        यदि (!f)
                f = stdout;

        यावत् (s > 0) {
                size_t i;

                fprintf(f, "%04x  ", n);

                प्रति (i = 0; i < 16; i++) {

                        यदि (i >= s)
                                fputs("   ", f);
                        अन्यथा
                                fprintf(f, "%02x ", b[i]);

                        यदि (i == 7)
                                fputc(' ', f);
                }

                fputc(' ', f);

                प्रति (i = 0; i < 16; i++) {

                        यदि (i >= s)
                                fputc(' ', f);
                        अन्यथा
                                fputc(isprint(b[i]) ? (वर्ण) b[i] : '.', f);
                }

                fputc('\n', f);

                यदि (s < 16)
                        भङ्ग;

                n += 16;
                b += 16;
                s -= 16;
        }
}
