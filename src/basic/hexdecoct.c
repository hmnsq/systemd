/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাব <ctype.h>
#অন্তর্ভাব <stdio.h>

#অন্তর্ভাব "alloc-util.h"
#অন্তর্ভাব "hexdecoct.h"
#অন্তর্ভাব "memory-util.h"
#অন্তর্ভাব "string-util.h"

বর্ণ octchar(পূর্ণাঙ্ক x) {
        প্রত্যাগম '0' + (x & 7);
}

পূর্ণাঙ্ক unoctchar(বর্ণ c) {

        যদি (c >= '0' && c <= '7')
                প্রত্যাগম c - '0';

        প্রত্যাগম -EINVAL;
}

বর্ণ decchar(পূর্ণাঙ্ক x) {
        প্রত্যাগম '0' + (x % 10);
}

পূর্ণাঙ্ক undecchar(বর্ণ c) {

        যদি (c >= '0' && c <= '9')
                প্রত্যাগম c - '0';

        প্রত্যাগম -EINVAL;
}

বর্ণ hexchar(পূর্ণাঙ্ক x) {
        ধ্রুব বর্ণ *table = LOWERCASE_HEXDIGITS;
        প্রত্যাগম table[x & 15];
}

পূর্ণাঙ্ক unhexchar(বর্ণ c) {

        যদি (c >= '0' && c <= '9')
                প্রত্যাগম c - '0';

        যদি (c >= 'a' && c <= 'f')
                প্রত্যাগম c - 'a' + 10;

        যদি (c >= 'A' && c <= 'F')
                প্রত্যাগম c - 'A' + 10;

        প্রত্যাগম -EINVAL;
}

বর্ণ* hexmem(ধ্রুব শূন্য *p, size_t l) {
        ধ্রুব uint8_t *x;
        বর্ণ *r, *z;

        প্রতিজ্ঞা(p || l == 0);

        z = r = new(বর্ণ, l * 2 + 1);
        যদি (!r)
                প্রত্যাগম রিক্ত;

        প্রতি (x = p; x && x < (ধ্রুব uint8_t*) p + l; x++) {
                *(z++) = hexchar(*x >> 4);
                *(z++) = hexchar(*x & 15);
        }

        *z = 0;
        প্রত্যাগম r;
}

স্থির পূর্ণাঙ্ক unhex_next(ধ্রুব বর্ণ **p, size_t *l) {
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(p);
        প্রতিজ্ঞা(l);

        /* Find the next non-whitespace character, and decode it. We
         * greedily skip all preceding and all following whitespace. */

        প্রতি (;;) {
                যদি (*l == 0)
                        প্রত্যাগম -EPIPE;

                যদি (!strchr(WHITESPACE, **p))
                        ভঙ্গ;

                /* Skip leading whitespace */
                (*p)++, (*l)--;
        }

        r = unhexchar(**p);
        যদি (r < 0)
                প্রত্যাগম r;

        প্রতি (;;) {
                (*p)++, (*l)--;

                যদি (*l == 0 || !strchr(WHITESPACE, **p))
                        ভঙ্গ;

                /* Skip following whitespace */
        }

        প্রত্যাগম r;
}

পূর্ণাঙ্ক unhexmem_full(
                ধ্রুব বর্ণ *p,
                size_t l,
                bool secure,
                শূন্য **ret_data,
                size_t *ret_size) {

        _cleanup_free_ uint8_t *buf = রিক্ত;
        size_t buf_size;
        ধ্রুব বর্ণ *x;
        uint8_t *z;

        প্রতিজ্ঞা(p || l == 0);

        যদি (l == SIZE_MAX)
                l = strlen(p);

        /* Note that the calculation of memory size is an upper boundary, as we ignore whitespace while decoding */
        buf_size = (l + 1) / 2 + 1;
        buf = malloc(buf_size);
        যদি (!buf)
                প্রত্যাগম -ENOMEM;

        CLEANUP_ERASE_PTR(secure ? &buf : রিক্ত, buf_size);

        প্রতি (x = p, z = buf;;) {
                পূর্ণাঙ্ক a, b;

                a = unhex_next(&x, &l);
                যদি (a == -EPIPE) /* End of string */
                        ভঙ্গ;
                যদি (a < 0)
                        প্রত্যাগম a;

                b = unhex_next(&x, &l);
                যদি (b < 0)
                        প্রত্যাগম b;

                *(z++) = (uint8_t) a << 4 | (uint8_t) b;
        }

        *z = 0;

        যদি (ret_size)
                *ret_size = (size_t) (z - buf);
        যদি (ret_data)
                *ret_data = TAKE_PTR(buf);

        প্রত্যাগম 0;
}

/* https://tools.ietf.org/html/rfc4648#section-6
 * Notice that base32hex differs from base32 in the alphabet it uses.
 * The distinction is that the base32hex representation preserves the
 * order of the underlying data when compared as bytestrings, this is
 * useful when representing NSEC3 hashes, as one can then verify the
 * order of hashes directly from their representation. */
বর্ণ base32hexchar(পূর্ণাঙ্ক x) {
        ধ্রুব বর্ণ *table = DIGITS "ABCDEFGHIJKLMNOPQRSTUV";
        প্রত্যাগম table[x & 31];
}

পূর্ণাঙ্ক unbase32hexchar(বর্ণ c) {
        unsigned offset;

        যদি (c >= '0' && c <= '9')
                প্রত্যাগম c - '0';

        offset = '9' - '0' + 1;

        যদি (c >= 'A' && c <= 'V')
                প্রত্যাগম c - 'A' + offset;

        প্রত্যাগম -EINVAL;
}

বর্ণ* base32hexmem(ধ্রুব শূন্য *p, size_t l, bool padding) {
        বর্ণ *r, *z;
        ধ্রুব uint8_t *x;
        size_t len;

        প্রতিজ্ঞা(p || l == 0);

        যদি (padding)
                /* five input bytes makes eight output bytes, padding is added so we must round up */
                len = 8 * (l + 4) / 5;
        অন্যথা {
                /* same, but round down as there is no padding */
                len = 8 * l / 5;

                বিকল্প (l % 5) {
                পক্ষ 4:
                        len += 7;
                        ভঙ্গ;
                পক্ষ 3:
                        len += 5;
                        ভঙ্গ;
                পক্ষ 2:
                        len += 4;
                        ভঙ্গ;
                পক্ষ 1:
                        len += 2;
                        ভঙ্গ;
                }
        }

        z = r = malloc(len + 1);
        যদি (!r)
                প্রত্যাগম রিক্ত;

        প্রতি (x = p; x < (ধ্রুব uint8_t*) p + (l / 5) * 5; x += 5) {
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

        বিকল্প (l % 5) {
        পক্ষ 4:
                *(z++) = base32hexchar(x[0] >> 3);                    /* 000XXXXX */
                *(z++) = base32hexchar((x[0] & 7) << 2 | x[1] >> 6);  /* 000XXXYY */
                *(z++) = base32hexchar((x[1] & 63) >> 1);             /* 000YYYYY */
                *(z++) = base32hexchar((x[1] & 1) << 4 | x[2] >> 4);   /* 000YZZZZ */
                *(z++) = base32hexchar((x[2] & 15) << 1 | x[3] >> 7); /* 000ZZZZQ */
                *(z++) = base32hexchar((x[3] & 127) >> 2);            /* 000QQQQQ */
                *(z++) = base32hexchar((x[3] & 3) << 3);              /* 000QQ000 */
                যদি (padding)
                        *(z++) = '=';

                ভঙ্গ;

        পক্ষ 3:
                *(z++) = base32hexchar(x[0] >> 3);                   /* 000XXXXX */
                *(z++) = base32hexchar((x[0] & 7) << 2 | x[1] >> 6); /* 000XXXYY */
                *(z++) = base32hexchar((x[1] & 63) >> 1);            /* 000YYYYY */
                *(z++) = base32hexchar((x[1] & 1) << 4 | x[2] >> 4); /* 000YZZZZ */
                *(z++) = base32hexchar((x[2] & 15) << 1);            /* 000ZZZZ0 */
                যদি (padding) {
                        *(z++) = '=';
                        *(z++) = '=';
                        *(z++) = '=';
                }

                ভঙ্গ;

        পক্ষ 2:
                *(z++) = base32hexchar(x[0] >> 3);                   /* 000XXXXX */
                *(z++) = base32hexchar((x[0] & 7) << 2 | x[1] >> 6); /* 000XXXYY */
                *(z++) = base32hexchar((x[1] & 63) >> 1);            /* 000YYYYY */
                *(z++) = base32hexchar((x[1] & 1) << 4);             /* 000Y0000 */
                যদি (padding) {
                        *(z++) = '=';
                        *(z++) = '=';
                        *(z++) = '=';
                        *(z++) = '=';
                }

                ভঙ্গ;

        পক্ষ 1:
                *(z++) = base32hexchar(x[0] >> 3);       /* 000XXXXX */
                *(z++) = base32hexchar((x[0] & 7) << 2); /* 000XXX00 */
                যদি (padding) {
                        *(z++) = '=';
                        *(z++) = '=';
                        *(z++) = '=';
                        *(z++) = '=';
                        *(z++) = '=';
                        *(z++) = '=';
                }

                ভঙ্গ;
        }

        *z = 0;
        প্রত্যাগম r;
}

পূর্ণাঙ্ক unbase32hexmem(ধ্রুব বর্ণ *p, size_t l, bool padding, শূন্য **mem, size_t *_len) {
        _cleanup_free_ uint8_t *r = রিক্ত;
        পূর্ণাঙ্ক a, b, c, d, e, f, g, h;
        uint8_t *z;
        ধ্রুব বর্ণ *x;
        size_t len;
        unsigned pad = 0;

        প্রতিজ্ঞা(p || l == 0);
        প্রতিজ্ঞা(mem);
        প্রতিজ্ঞা(_len);

        যদি (l == SIZE_MAX)
                l = strlen(p);

        /* padding ensures any base32hex input has input divisible by 8 */
        যদি (padding && l % 8 != 0)
                প্রত্যাগম -EINVAL;

        যদি (padding) {
                /* strip the padding */
                যাবত্ (l > 0 && p[l - 1] == '=' && pad < 7) {
                        pad++;
                        l--;
                }
        }

        /* a group of eight input bytes needs five output bytes, in case of
         * padding we need to add some extra bytes */
        len = (l / 8) * 5;

        বিকল্প (l % 8) {
        পক্ষ 7:
                len += 4;
                ভঙ্গ;
        পক্ষ 5:
                len += 3;
                ভঙ্গ;
        পক্ষ 4:
                len += 2;
                ভঙ্গ;
        পক্ষ 2:
                len += 1;
                ভঙ্গ;
        পক্ষ 0:
                ভঙ্গ;
        স্বভাব:
                প্রত্যাগম -EINVAL;
        }

        z = r = malloc(len + 1);
        যদি (!r)
                প্রত্যাগম -ENOMEM;

        প্রতি (x = p; x < p + (l / 8) * 8; x += 8) {
                /* a == 000XXXXX; b == 000YYYYY; c == 000ZZZZZ; d == 000WWWWW
                 * e == 000SSSSS; f == 000QQQQQ; g == 000VVVVV; h == 000RRRRR */
                a = unbase32hexchar(x[0]);
                যদি (a < 0)
                        প্রত্যাগম -EINVAL;

                b = unbase32hexchar(x[1]);
                যদি (b < 0)
                        প্রত্যাগম -EINVAL;

                c = unbase32hexchar(x[2]);
                যদি (c < 0)
                        প্রত্যাগম -EINVAL;

                d = unbase32hexchar(x[3]);
                যদি (d < 0)
                        প্রত্যাগম -EINVAL;

                e = unbase32hexchar(x[4]);
                যদি (e < 0)
                        প্রত্যাগম -EINVAL;

                f = unbase32hexchar(x[5]);
                যদি (f < 0)
                        প্রত্যাগম -EINVAL;

                g = unbase32hexchar(x[6]);
                যদি (g < 0)
                        প্রত্যাগম -EINVAL;

                h = unbase32hexchar(x[7]);
                যদি (h < 0)
                        প্রত্যাগম -EINVAL;

                *(z++) = (uint8_t) a << 3 | (uint8_t) b >> 2;                    /* XXXXXYYY */
                *(z++) = (uint8_t) b << 6 | (uint8_t) c << 1 | (uint8_t) d >> 4; /* YYZZZZZW */
                *(z++) = (uint8_t) d << 4 | (uint8_t) e >> 1;                    /* WWWWSSSS */
                *(z++) = (uint8_t) e << 7 | (uint8_t) f << 2 | (uint8_t) g >> 3; /* SQQQQQVV */
                *(z++) = (uint8_t) g << 5 | (uint8_t) h;                         /* VVVRRRRR */
        }

        বিকল্প (l % 8) {
        পক্ষ 7:
                a = unbase32hexchar(x[0]);
                যদি (a < 0)
                        প্রত্যাগম -EINVAL;

                b = unbase32hexchar(x[1]);
                যদি (b < 0)
                        প্রত্যাগম -EINVAL;

                c = unbase32hexchar(x[2]);
                যদি (c < 0)
                        প্রত্যাগম -EINVAL;

                d = unbase32hexchar(x[3]);
                যদি (d < 0)
                        প্রত্যাগম -EINVAL;

                e = unbase32hexchar(x[4]);
                যদি (e < 0)
                        প্রত্যাগম -EINVAL;

                f = unbase32hexchar(x[5]);
                যদি (f < 0)
                        প্রত্যাগম -EINVAL;

                g = unbase32hexchar(x[6]);
                যদি (g < 0)
                        প্রত্যাগম -EINVAL;

                /* g == 000VV000 */
                যদি (g & 7)
                        প্রত্যাগম -EINVAL;

                *(z++) = (uint8_t) a << 3 | (uint8_t) b >> 2;                    /* XXXXXYYY */
                *(z++) = (uint8_t) b << 6 | (uint8_t) c << 1 | (uint8_t) d >> 4; /* YYZZZZZW */
                *(z++) = (uint8_t) d << 4 | (uint8_t) e >> 1;                    /* WWWWSSSS */
                *(z++) = (uint8_t) e << 7 | (uint8_t) f << 2 | (uint8_t) g >> 3; /* SQQQQQVV */

                ভঙ্গ;
        পক্ষ 5:
                a = unbase32hexchar(x[0]);
                যদি (a < 0)
                        প্রত্যাগম -EINVAL;

                b = unbase32hexchar(x[1]);
                যদি (b < 0)
                        প্রত্যাগম -EINVAL;

                c = unbase32hexchar(x[2]);
                যদি (c < 0)
                        প্রত্যাগম -EINVAL;

                d = unbase32hexchar(x[3]);
                যদি (d < 0)
                        প্রত্যাগম -EINVAL;

                e = unbase32hexchar(x[4]);
                যদি (e < 0)
                        প্রত্যাগম -EINVAL;

                /* e == 000SSSS0 */
                যদি (e & 1)
                        প্রত্যাগম -EINVAL;

                *(z++) = (uint8_t) a << 3 | (uint8_t) b >> 2;                    /* XXXXXYYY */
                *(z++) = (uint8_t) b << 6 | (uint8_t) c << 1 | (uint8_t) d >> 4; /* YYZZZZZW */
                *(z++) = (uint8_t) d << 4 | (uint8_t) e >> 1;                    /* WWWWSSSS */

                ভঙ্গ;
        পক্ষ 4:
                a = unbase32hexchar(x[0]);
                যদি (a < 0)
                        প্রত্যাগম -EINVAL;

                b = unbase32hexchar(x[1]);
                যদি (b < 0)
                        প্রত্যাগম -EINVAL;

                c = unbase32hexchar(x[2]);
                যদি (c < 0)
                        প্রত্যাগম -EINVAL;

                d = unbase32hexchar(x[3]);
                যদি (d < 0)
                        প্রত্যাগম -EINVAL;

                /* d == 000W0000 */
                যদি (d & 15)
                        প্রত্যাগম -EINVAL;

                *(z++) = (uint8_t) a << 3 | (uint8_t) b >> 2;                    /* XXXXXYYY */
                *(z++) = (uint8_t) b << 6 | (uint8_t) c << 1 | (uint8_t) d >> 4; /* YYZZZZZW */

                ভঙ্গ;
        পক্ষ 2:
                a = unbase32hexchar(x[0]);
                যদি (a < 0)
                        প্রত্যাগম -EINVAL;

                b = unbase32hexchar(x[1]);
                যদি (b < 0)
                        প্রত্যাগম -EINVAL;

                /* b == 000YYY00 */
                যদি (b & 3)
                        প্রত্যাগম -EINVAL;

                *(z++) = (uint8_t) a << 3 | (uint8_t) b >> 2; /* XXXXXYYY */

                ভঙ্গ;
        পক্ষ 0:
                ভঙ্গ;
        স্বভাব:
                প্রত্যাগম -EINVAL;
        }

        *z = 0;

        *mem = TAKE_PTR(r);
        *_len = len;

        প্রত্যাগম 0;
}

/* https://tools.ietf.org/html/rfc4648#section-4 */
বর্ণ base64char(পূর্ণাঙ্ক x) {
        ধ্রুব বর্ণ *table = UPPERCASE_LETTERS LOWERCASE_LETTERS DIGITS "+/";
        প্রত্যাগম table[x & 63];
}

/* This is almost base64char(), but not entirely, as it uses the "url and filename safe" alphabet,
 * since we don't want "/" appear in interface names (since interfaces appear in sysfs as filenames).
 * See section #5 of RFC 4648. */
বর্ণ urlsafe_base64char(পূর্ণাঙ্ক x) {
        ধ্রুব বর্ণ *table = UPPERCASE_LETTERS LOWERCASE_LETTERS DIGITS "-_";
        প্রত্যাগম table[x & 63];
}

পূর্ণাঙ্ক unbase64char(বর্ণ c) {
        unsigned offset;

        যদি (c >= 'A' && c <= 'Z')
                প্রত্যাগম c - 'A';

        offset = 'Z' - 'A' + 1;

        যদি (c >= 'a' && c <= 'z')
                প্রত্যাগম c - 'a' + offset;

        offset += 'z' - 'a' + 1;

        যদি (c >= '0' && c <= '9')
                প্রত্যাগম c - '0' + offset;

        offset += '9' - '0' + 1;

        যদি (IN_SET(c, '+', '-')) /* Support both the regular and the URL safe character set (see above) */
                প্রত্যাগম offset;

        offset++;

        যদি (IN_SET(c, '/', '_')) /* ditto */
                প্রত্যাগম offset;

        প্রত্যাগম -EINVAL;
}

স্থির শূন্য maybe_সারি_break(বর্ণ **x, বর্ণ *start, size_t সারি_break) {
        size_t n;

        প্রতিজ্ঞা(x);
        প্রতিজ্ঞা(*x);
        প্রতিজ্ঞা(start);
        প্রতিজ্ঞা(*x >= start);

        যদি (সারি_break == SIZE_MAX)
                প্রত্যাগম;

        n = *x - start;

        যদি (n % (সারি_break + 1) == সারি_break)
                *((*x)++) = '\n';
}

ssize_t base64mem_full(
                ধ্রুব শূন্য *p,
                size_t l,
                size_t সারি_break,
                বর্ণ **ret) {

        ধ্রুব uint8_t *x;
        বর্ণ *b, *z;
        size_t m;

        প্রতিজ্ঞা(p || l == 0);
        প্রতিজ্ঞা(সারি_break > 0);
        প্রতিজ্ঞা(ret);

        /* three input bytes makes four output bytes, padding is added so we must round up */
        m = 4 * (l + 2) / 3 + 1;
        যদি (সারি_break != SIZE_MAX)
                m += m / সারি_break;

        z = b = malloc(m);
        যদি (!b)
                প্রত্যাগম -ENOMEM;

        প্রতি (x = p; x && x < (ধ্রুব uint8_t*) p + (l / 3) * 3; x += 3) {
                /* x[0] == XXXXXXXX; x[1] == YYYYYYYY; x[2] == ZZZZZZZZ */
                maybe_সারি_break(&z, b, সারি_break);
                *(z++) = base64char(x[0] >> 2);                    /* 00XXXXXX */
                maybe_সারি_break(&z, b, সারি_break);
                *(z++) = base64char((x[0] & 3) << 4 | x[1] >> 4);  /* 00XXYYYY */
                maybe_সারি_break(&z, b, সারি_break);
                *(z++) = base64char((x[1] & 15) << 2 | x[2] >> 6); /* 00YYYYZZ */
                maybe_সারি_break(&z, b, সারি_break);
                *(z++) = base64char(x[2] & 63);                    /* 00ZZZZZZ */
        }

        বিকল্প (l % 3) {
        পক্ষ 2:
                maybe_সারি_break(&z, b, সারি_break);
                *(z++) = base64char(x[0] >> 2);                   /* 00XXXXXX */
                maybe_সারি_break(&z, b, সারি_break);
                *(z++) = base64char((x[0] & 3) << 4 | x[1] >> 4); /* 00XXYYYY */
                maybe_সারি_break(&z, b, সারি_break);
                *(z++) = base64char((x[1] & 15) << 2);            /* 00YYYY00 */
                maybe_সারি_break(&z, b, সারি_break);
                *(z++) = '=';
                ভঙ্গ;

        পক্ষ 1:
                maybe_সারি_break(&z, b, সারি_break);
                *(z++) = base64char(x[0] >> 2);        /* 00XXXXXX */
                maybe_সারি_break(&z, b, সারি_break);
                *(z++) = base64char((x[0] & 3) << 4);  /* 00XX0000 */
                maybe_সারি_break(&z, b, সারি_break);
                *(z++) = '=';
                maybe_সারি_break(&z, b, সারি_break);
                *(z++) = '=';
                ভঙ্গ;
        }

        *z = 0;
        *ret = b;

        প্রতিজ্ঞা(z >= b); /* Let static analyzers know that the answer is non-negative. */
        প্রত্যাগম z - b;
}

স্থির ssize_t base64_append_width(
                বর্ণ **prefix,
                size_t plen,
                বর্ণ sep,
                size_t indent,
                ধ্রুব শূন্য *p,
                size_t l,
                size_t width) {

        _cleanup_free_ বর্ণ *x = রিক্ত;
        বর্ণ *t, *s;
        size_t সারিগুলি;
        ssize_t len;

        প্রতিজ্ঞা(prefix);
        প্রতিজ্ঞা(*prefix || plen == 0);
        প্রতিজ্ঞা(p || l == 0);

        len = base64mem(p, l, &x);
        যদি (len < 0)
                প্রত্যাগম len;
        যদি (len == 0)
                প্রত্যাগম plen;

        সারিগুলি = DIV_ROUND_UP(len, width);

        যদি (plen >= SSIZE_MAX - 1 - 1 ||
            সারিগুলি > (SSIZE_MAX - plen - 1 - 1) / (indent + width + 1))
                প্রত্যাগম -ENOMEM;

        t = realloc(*prefix, plen + 1 + 1 + (indent + width + 1) * সারিগুলি);
        যদি (!t)
                প্রত্যাগম -ENOMEM;

        s = t + plen;
        প্রতি (size_t সারি = 0; সারি < সারিগুলি; সারি++) {
                size_t act = MIN(width, (size_t) len);

                যদি (সারি > 0)
                        sep = '\n';

                যদি (s > t) {
                        *s++ = sep;
                        যদি (sep == '\n')
                                s = mempset(s, ' ', indent);
                }

                s = mempcpy(s, x + width * সারি, act);
                len -= act;
        }
        প্রতিজ্ঞা(len == 0);

        *s = '\0';
        *prefix = t;
        প্রত্যাগম s - t;
}

ssize_t base64_append(
                বর্ণ **prefix,
                size_t plen,
                ধ্রুব শূন্য *p,
                size_t l,
                size_t indent,
                size_t width) {

        যদি (plen > width / 2 || plen + indent > width)
                /* leave indent on the left, keep last column free */
                প্রত্যাগম base64_append_width(prefix, plen, '\n', indent, p, l, width - indent);
        অন্যথা
                /* leave plen on the left, keep last column free */
                প্রত্যাগম base64_append_width(prefix, plen, ' ', plen + 1, p, l, width - plen - 1);
}

স্থির পূর্ণাঙ্ক unbase64_next(ধ্রুব বর্ণ **p, size_t *l) {
        পূর্ণাঙ্ক ret;

        প্রতিজ্ঞা(p);
        প্রতিজ্ঞা(l);

        /* Find the next non-whitespace character, and decode it. If we find padding, we return it as INT_MAX. We
         * greedily skip all preceding and all following whitespace. */

        প্রতি (;;) {
                যদি (*l == 0)
                        প্রত্যাগম -EPIPE;

                যদি (!strchr(WHITESPACE, **p))
                        ভঙ্গ;

                /* Skip leading whitespace */
                (*p)++, (*l)--;
        }

        যদি (**p == '=')
                ret = INT_MAX; /* return padding as INT_MAX */
        অন্যথা {
                ret = unbase64char(**p);
                যদি (ret < 0)
                        প্রত্যাগম ret;
        }

        প্রতি (;;) {
                (*p)++, (*l)--;

                যদি (*l == 0)
                        ভঙ্গ;
                যদি (!strchr(WHITESPACE, **p))
                        ভঙ্গ;

                /* Skip following whitespace */
        }

        প্রত্যাগম ret;
}

পূর্ণাঙ্ক unbase64mem_full(
                ধ্রুব বর্ণ *p,
                size_t l,
                bool secure,
                শূন্য **ret_data,
                size_t *ret_size) {

        _cleanup_free_ uint8_t *buf = রিক্ত;
        ধ্রুব বর্ণ *x;
        uint8_t *z;
        size_t len;

        প্রতিজ্ঞা(p || l == 0);

        যদি (l == SIZE_MAX)
                l = strlen(p);

        /* A group of four input bytes needs three output bytes, in case of padding we need to add two or three extra
         * bytes. Note that this calculation is an upper boundary, as we ignore whitespace while decoding */
        len = (l / 4) * 3 + (l % 4 != 0 ? (l % 4) - 1 : 0);

        buf = malloc(len + 1);
        যদি (!buf)
                প্রত্যাগম -ENOMEM;

        CLEANUP_ERASE_PTR(secure ? &buf : রিক্ত, len);

        প্রতি (x = p, z = buf;;) {
                পূর্ণাঙ্ক a, b, c, d; /* a == 00XXXXXX; b == 00YYYYYY; c == 00ZZZZZZ; d == 00WWWWWW */

                a = unbase64_next(&x, &l);
                যদি (a == -EPIPE) /* End of string */
                        ভঙ্গ;
                যদি (a < 0)
                        প্রত্যাগম a;
                যদি (a == INT_MAX) /* Padding is not allowed at the beginning of a 4ch block */
                        প্রত্যাগম -EINVAL;

                b = unbase64_next(&x, &l);
                যদি (b < 0)
                        প্রত্যাগম b;
                যদি (b == INT_MAX) /* Padding is not allowed at the second character of a 4ch block either */
                        প্রত্যাগম -EINVAL;

                c = unbase64_next(&x, &l);
                যদি (c < 0)
                        প্রত্যাগম c;

                d = unbase64_next(&x, &l);
                যদি (d < 0)
                        প্রত্যাগম d;

                যদি (c == INT_MAX) { /* Padding at the third character */

                        যদি (d != INT_MAX) /* If the third character is padding, the fourth must be too */
                                প্রত্যাগম -EINVAL;

                        /* b == 00YY0000 */
                        যদি (b & 15)
                                প্রত্যাগম -EINVAL;

                        যদি (l > 0) /* Trailing rubbish? */
                                প্রত্যাগম -ENAMETOOLONG;

                        *(z++) = (uint8_t) a << 2 | (uint8_t) (b >> 4); /* XXXXXXYY */
                        ভঙ্গ;
                }

                যদি (d == INT_MAX) {
                        /* c == 00ZZZZ00 */
                        যদি (c & 3)
                                প্রত্যাগম -EINVAL;

                        যদি (l > 0) /* Trailing rubbish? */
                                প্রত্যাগম -ENAMETOOLONG;

                        *(z++) = (uint8_t) a << 2 | (uint8_t) b >> 4; /* XXXXXXYY */
                        *(z++) = (uint8_t) b << 4 | (uint8_t) c >> 2; /* YYYYZZZZ */
                        ভঙ্গ;
                }

                *(z++) = (uint8_t) a << 2 | (uint8_t) b >> 4; /* XXXXXXYY */
                *(z++) = (uint8_t) b << 4 | (uint8_t) c >> 2; /* YYYYZZZZ */
                *(z++) = (uint8_t) c << 6 | (uint8_t) d;      /* ZZWWWWWW */
        }

        *z = 0;

        প্রতিজ্ঞা((size_t) (z - buf) <= len);

        যদি (ret_size)
                *ret_size = (size_t) (z - buf);
        যদি (ret_data)
                *ret_data = TAKE_PTR(buf);

        প্রত্যাগম 0;
}

শূন্য hexdump(FILE *f, ধ্রুব শূন্য *p, size_t s) {
        ধ্রুব uint8_t *b = p;
        unsigned n = 0;

        প্রতিজ্ঞা(b || s == 0);

        যদি (s == SIZE_MAX)
                s = strlen(p);

        যদি (!f)
                f = stdout;

        যাবত্ (s > 0) {
                size_t i;

                fprintf(f, "%04x  ", n);

                প্রতি (i = 0; i < 16; i++) {

                        যদি (i >= s)
                                fputs("   ", f);
                        অন্যথা
                                fprintf(f, "%02x ", b[i]);

                        যদি (i == 7)
                                fputc(' ', f);
                }

                fputc(' ', f);

                প্রতি (i = 0; i < 16; i++) {

                        যদি (i >= s)
                                fputc(' ', f);
                        অন্যথা
                                fputc(isprint(b[i]) ? (বর্ণ) b[i] : '.', f);
                }

                fputc('\n', f);

                যদি (s < 16)
                        ভঙ্গ;

                n += 16;
                b += 16;
                s -= 16;
        }
}
