/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাব <ctype.h>
#অন্তর্ভাব <stdio.h>

#অন্তর্ভাব "alloc-util.h"
#অন্তর্ভাব "hexdecoct.h"
#অন্তর্ভাব "memory-util.h"
#অন্তর্ভাব "string-util.h"

অক্ষর octchar(পূর্ণসংখ্যা x) {
        ফেরত '0' + (x & 7);
}

পূর্ণসংখ্যা unoctchar(অক্ষর c) {

        যদি (c >= '0' && c <= '7')
                ফেরত c - '0';

        ফেরত -EINVAL;
}

অক্ষর decchar(পূর্ণসংখ্যা x) {
        ফেরত '0' + (x % 10);
}

পূর্ণসংখ্যা undecchar(অক্ষর c) {

        যদি (c >= '0' && c <= '9')
                ফেরত c - '0';

        ফেরত -EINVAL;
}

অক্ষর hexchar(পূর্ণসংখ্যা x) {
        ধ্রুবক অক্ষর *table = LOWERCASE_HEXDIGITS;
        ফেরত table[x & 15];
}

পূর্ণসংখ্যা unhexchar(অক্ষর c) {

        যদি (c >= '0' && c <= '9')
                ফেরত c - '0';

        যদি (c >= 'a' && c <= 'f')
                ফেরত c - 'a' + 10;

        যদি (c >= 'A' && c <= 'F')
                ফেরত c - 'A' + 10;

        ফেরত -EINVAL;
}

অক্ষর* hexmem(ধ্রুবক শূন্য *p, size_t l) {
        ধ্রুবক uint8_t *x;
        অক্ষর *r, *z;

        প্রতিজ্ঞা(p || l == 0);

        z = r = new(অক্ষর, l * 2 + 1);
        যদি (!r)
                ফেরত ফাঁকা;

        জন্য (x = p; x && x < (ধ্রুবক uint8_t*) p + l; x++) {
                *(z++) = hexchar(*x >> 4);
                *(z++) = hexchar(*x & 15);
        }

        *z = 0;
        ফেরত r;
}

স্থির পূর্ণসংখ্যা unhex_পরবর্তী(ধ্রুবক অক্ষর **p, size_t *l) {
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(p);
        প্রতিজ্ঞা(l);

        /* Find the next non-whitespace character, and decode it. We
         * greedily skip all preceding and all following whitespace. */

        জন্য (;;) {
                যদি (*l == 0)
                        ফেরত -EPIPE;

                যদি (!strchr(WHITESPACE, **p))
                        বিরতি;

                /* Skip leading whitespace */
                (*p)++, (*l)--;
        }

        r = unhexchar(**p);
        যদি (r < 0)
                ফেরত r;

        জন্য (;;) {
                (*p)++, (*l)--;

                যদি (*l == 0 || !strchr(WHITESPACE, **p))
                        বিরতি;

                /* Skip following whitespace */
        }

        ফেরত r;
}

পূর্ণসংখ্যা unhexmem_full(
                ধ্রুবক অক্ষর *p,
                size_t l,
                bool secure,
                শূন্য **ret_তথ্য,
                size_t *ret_আকার) {

        _cleanup_free_ uint8_t *buf = ফাঁকা;
        size_t buf_size;
        ধ্রুবক অক্ষর *x;
        uint8_t *z;

        প্রতিজ্ঞা(p || l == 0);

        যদি (l == SIZE_MAX)
                l = strlen(p);

        /* Note that the calculation of memory size is an upper boundary, as we ignore whitespace while decoding */
        buf_size = (l + 1) / 2 + 1;
        buf = malloc(buf_size);
        যদি (!buf)
                ফেরত -ENOMEM;

        CLEANUP_ERASE_PTR(secure ? &buf : ফাঁকা, buf_size);

        জন্য (x = p, z = buf;;) {
                পূর্ণসংখ্যা a, b;

                a = unhex_পরবর্তী(&x, &l);
                যদি (a == -EPIPE) /* End of string */
                        বিরতি;
                যদি (a < 0)
                        ফেরত a;

                b = unhex_পরবর্তী(&x, &l);
                যদি (b < 0)
                        ফেরত b;

                *(z++) = (uint8_t) a << 4 | (uint8_t) b;
        }

        *z = 0;

        যদি (ret_আকার)
                *ret_আকার = (size_t) (z - buf);
        যদি (ret_তথ্য)
                *ret_তথ্য = TAKE_PTR(buf);

        ফেরত 0;
}

/* https://tools.ietf.org/html/rfc4648#section-6
 * Notice that base32hex differs from base32 in the alphabet it uses.
 * The distinction is that the base32hex representation preserves the
 * order of the underlying data when compared as bytestrings, this is
 * useful when representing NSEC3 hashes, as one can then verify the
 * order of hashes directly from their representation. */
অক্ষর base32hexchar(পূর্ণসংখ্যা x) {
        ধ্রুবক অক্ষর *table = DIGITS "ABCDEFGHIJKLMNOPQRSTUV";
        ফেরত table[x & 31];
}

পূর্ণসংখ্যা unbase32hexchar(অক্ষর c) {
        unsigned offset;

        যদি (c >= '0' && c <= '9')
                ফেরত c - '0';

        offset = '9' - '0' + 1;

        যদি (c >= 'A' && c <= 'V')
                ফেরত c - 'A' + offset;

        ফেরত -EINVAL;
}

অক্ষর* base32hexmem(ধ্রুবক শূন্য *p, size_t l, bool padding) {
        অক্ষর *r, *z;
        ধ্রুবক uint8_t *x;
        size_t len;

        প্রতিজ্ঞা(p || l == 0);

        যদি (padding)
                /* five input bytes makes eight output bytes, padding is added so we must round up */
                len = 8 * (l + 4) / 5;
        অন্যথা {
                /* same, but round down as there is no padding */
                len = 8 * l / 5;

                নির্বাচন (l % 5) {
                ক্ষেত্র 4:
                        len += 7;
                        বিরতি;
                ক্ষেত্র 3:
                        len += 5;
                        বিরতি;
                ক্ষেত্র 2:
                        len += 4;
                        বিরতি;
                ক্ষেত্র 1:
                        len += 2;
                        বিরতি;
                }
        }

        z = r = malloc(len + 1);
        যদি (!r)
                ফেরত ফাঁকা;

        জন্য (x = p; x < (ধ্রুবক uint8_t*) p + (l / 5) * 5; x += 5) {
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

        নির্বাচন (l % 5) {
        ক্ষেত্র 4:
                *(z++) = base32hexchar(x[0] >> 3);                    /* 000XXXXX */
                *(z++) = base32hexchar((x[0] & 7) << 2 | x[1] >> 6);  /* 000XXXYY */
                *(z++) = base32hexchar((x[1] & 63) >> 1);             /* 000YYYYY */
                *(z++) = base32hexchar((x[1] & 1) << 4 | x[2] >> 4);   /* 000YZZZZ */
                *(z++) = base32hexchar((x[2] & 15) << 1 | x[3] >> 7); /* 000ZZZZQ */
                *(z++) = base32hexchar((x[3] & 127) >> 2);            /* 000QQQQQ */
                *(z++) = base32hexchar((x[3] & 3) << 3);              /* 000QQ000 */
                যদি (padding)
                        *(z++) = '=';

                বিরতি;

        ক্ষেত্র 3:
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

                বিরতি;

        ক্ষেত্র 2:
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

                বিরতি;

        ক্ষেত্র 1:
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

                বিরতি;
        }

        *z = 0;
        ফেরত r;
}

পূর্ণসংখ্যা unbase32hexmem(ধ্রুবক অক্ষর *p, size_t l, bool padding, শূন্য **mem, size_t *_len) {
        _cleanup_free_ uint8_t *r = ফাঁকা;
        পূর্ণসংখ্যা a, b, c, d, e, f, g, h;
        uint8_t *z;
        ধ্রুবক অক্ষর *x;
        size_t len;
        unsigned pad = 0;

        প্রতিজ্ঞা(p || l == 0);
        প্রতিজ্ঞা(mem);
        প্রতিজ্ঞা(_len);

        যদি (l == SIZE_MAX)
                l = strlen(p);

        /* padding ensures any base32hex input has input divisible by 8 */
        যদি (padding && l % 8 != 0)
                ফেরত -EINVAL;

        যদি (padding) {
                /* strip the padding */
                যখন (l > 0 && p[l - 1] == '=' && pad < 7) {
                        pad++;
                        l--;
                }
        }

        /* a group of eight input bytes needs five output bytes, in case of
         * padding we need to add some extra bytes */
        len = (l / 8) * 5;

        নির্বাচন (l % 8) {
        ক্ষেত্র 7:
                len += 4;
                বিরতি;
        ক্ষেত্র 5:
                len += 3;
                বিরতি;
        ক্ষেত্র 4:
                len += 2;
                বিরতি;
        ক্ষেত্র 2:
                len += 1;
                বিরতি;
        ক্ষেত্র 0:
                বিরতি;
        পূর্বনির্ধারিত:
                ফেরত -EINVAL;
        }

        z = r = malloc(len + 1);
        যদি (!r)
                ফেরত -ENOMEM;

        জন্য (x = p; x < p + (l / 8) * 8; x += 8) {
                /* a == 000XXXXX; b == 000YYYYY; c == 000ZZZZZ; d == 000WWWWW
                 * e == 000SSSSS; f == 000QQQQQ; g == 000VVVVV; h == 000RRRRR */
                a = unbase32hexchar(x[0]);
                যদি (a < 0)
                        ফেরত -EINVAL;

                b = unbase32hexchar(x[1]);
                যদি (b < 0)
                        ফেরত -EINVAL;

                c = unbase32hexchar(x[2]);
                যদি (c < 0)
                        ফেরত -EINVAL;

                d = unbase32hexchar(x[3]);
                যদি (d < 0)
                        ফেরত -EINVAL;

                e = unbase32hexchar(x[4]);
                যদি (e < 0)
                        ফেরত -EINVAL;

                f = unbase32hexchar(x[5]);
                যদি (f < 0)
                        ফেরত -EINVAL;

                g = unbase32hexchar(x[6]);
                যদি (g < 0)
                        ফেরত -EINVAL;

                h = unbase32hexchar(x[7]);
                যদি (h < 0)
                        ফেরত -EINVAL;

                *(z++) = (uint8_t) a << 3 | (uint8_t) b >> 2;                    /* XXXXXYYY */
                *(z++) = (uint8_t) b << 6 | (uint8_t) c << 1 | (uint8_t) d >> 4; /* YYZZZZZW */
                *(z++) = (uint8_t) d << 4 | (uint8_t) e >> 1;                    /* WWWWSSSS */
                *(z++) = (uint8_t) e << 7 | (uint8_t) f << 2 | (uint8_t) g >> 3; /* SQQQQQVV */
                *(z++) = (uint8_t) g << 5 | (uint8_t) h;                         /* VVVRRRRR */
        }

        নির্বাচন (l % 8) {
        ক্ষেত্র 7:
                a = unbase32hexchar(x[0]);
                যদি (a < 0)
                        ফেরত -EINVAL;

                b = unbase32hexchar(x[1]);
                যদি (b < 0)
                        ফেরত -EINVAL;

                c = unbase32hexchar(x[2]);
                যদি (c < 0)
                        ফেরত -EINVAL;

                d = unbase32hexchar(x[3]);
                যদি (d < 0)
                        ফেরত -EINVAL;

                e = unbase32hexchar(x[4]);
                যদি (e < 0)
                        ফেরত -EINVAL;

                f = unbase32hexchar(x[5]);
                যদি (f < 0)
                        ফেরত -EINVAL;

                g = unbase32hexchar(x[6]);
                যদি (g < 0)
                        ফেরত -EINVAL;

                /* g == 000VV000 */
                যদি (g & 7)
                        ফেরত -EINVAL;

                *(z++) = (uint8_t) a << 3 | (uint8_t) b >> 2;                    /* XXXXXYYY */
                *(z++) = (uint8_t) b << 6 | (uint8_t) c << 1 | (uint8_t) d >> 4; /* YYZZZZZW */
                *(z++) = (uint8_t) d << 4 | (uint8_t) e >> 1;                    /* WWWWSSSS */
                *(z++) = (uint8_t) e << 7 | (uint8_t) f << 2 | (uint8_t) g >> 3; /* SQQQQQVV */

                বিরতি;
        ক্ষেত্র 5:
                a = unbase32hexchar(x[0]);
                যদি (a < 0)
                        ফেরত -EINVAL;

                b = unbase32hexchar(x[1]);
                যদি (b < 0)
                        ফেরত -EINVAL;

                c = unbase32hexchar(x[2]);
                যদি (c < 0)
                        ফেরত -EINVAL;

                d = unbase32hexchar(x[3]);
                যদি (d < 0)
                        ফেরত -EINVAL;

                e = unbase32hexchar(x[4]);
                যদি (e < 0)
                        ফেরত -EINVAL;

                /* e == 000SSSS0 */
                যদি (e & 1)
                        ফেরত -EINVAL;

                *(z++) = (uint8_t) a << 3 | (uint8_t) b >> 2;                    /* XXXXXYYY */
                *(z++) = (uint8_t) b << 6 | (uint8_t) c << 1 | (uint8_t) d >> 4; /* YYZZZZZW */
                *(z++) = (uint8_t) d << 4 | (uint8_t) e >> 1;                    /* WWWWSSSS */

                বিরতি;
        ক্ষেত্র 4:
                a = unbase32hexchar(x[0]);
                যদি (a < 0)
                        ফেরত -EINVAL;

                b = unbase32hexchar(x[1]);
                যদি (b < 0)
                        ফেরত -EINVAL;

                c = unbase32hexchar(x[2]);
                যদি (c < 0)
                        ফেরত -EINVAL;

                d = unbase32hexchar(x[3]);
                যদি (d < 0)
                        ফেরত -EINVAL;

                /* d == 000W0000 */
                যদি (d & 15)
                        ফেরত -EINVAL;

                *(z++) = (uint8_t) a << 3 | (uint8_t) b >> 2;                    /* XXXXXYYY */
                *(z++) = (uint8_t) b << 6 | (uint8_t) c << 1 | (uint8_t) d >> 4; /* YYZZZZZW */

                বিরতি;
        ক্ষেত্র 2:
                a = unbase32hexchar(x[0]);
                যদি (a < 0)
                        ফেরত -EINVAL;

                b = unbase32hexchar(x[1]);
                যদি (b < 0)
                        ফেরত -EINVAL;

                /* b == 000YYY00 */
                যদি (b & 3)
                        ফেরত -EINVAL;

                *(z++) = (uint8_t) a << 3 | (uint8_t) b >> 2; /* XXXXXYYY */

                বিরতি;
        ক্ষেত্র 0:
                বিরতি;
        পূর্বনির্ধারিত:
                ফেরত -EINVAL;
        }

        *z = 0;

        *mem = TAKE_PTR(r);
        *_len = len;

        ফেরত 0;
}

/* https://tools.ietf.org/html/rfc4648#section-4 */
অক্ষর base64char(পূর্ণসংখ্যা x) {
        ধ্রুবক অক্ষর *table = UPPERCASE_LETTERS LOWERCASE_LETTERS DIGITS "+/";
        ফেরত table[x & 63];
}

/* This is almost base64char(), but not entirely, as it uses the "url and filename safe" alphabet,
 * since we don't want "/" appear in interface names (since interfaces appear in sysfs as filenames).
 * See section #5 of RFC 4648. */
অক্ষর urlsafe_base64char(পূর্ণসংখ্যা x) {
        ধ্রুবক অক্ষর *table = UPPERCASE_LETTERS LOWERCASE_LETTERS DIGITS "-_";
        ফেরত table[x & 63];
}

পূর্ণসংখ্যা unbase64char(অক্ষর c) {
        unsigned offset;

        যদি (c >= 'A' && c <= 'Z')
                ফেরত c - 'A';

        offset = 'Z' - 'A' + 1;

        যদি (c >= 'a' && c <= 'z')
                ফেরত c - 'a' + offset;

        offset += 'z' - 'a' + 1;

        যদি (c >= '0' && c <= '9')
                ফেরত c - '0' + offset;

        offset += '9' - '0' + 1;

        যদি (IN_SET(c, '+', '-')) /* Support both the regular and the URL safe character set (see above) */
                ফেরত offset;

        offset++;

        যদি (IN_SET(c, '/', '_')) /* ditto */
                ফেরত offset;

        ফেরত -EINVAL;
}

স্থির শূন্য maybe_সারি_break(অক্ষর **x, অক্ষর *start, size_t সারি_break) {
        size_t n;

        প্রতিজ্ঞা(x);
        প্রতিজ্ঞা(*x);
        প্রতিজ্ঞা(start);
        প্রতিজ্ঞা(*x >= start);

        যদি (সারি_break == SIZE_MAX)
                ফেরত;

        n = *x - start;

        যদি (n % (সারি_break + 1) == সারি_break)
                *((*x)++) = '\n';
}

ssize_t base64mem_full(
                ধ্রুবক শূন্য *p,
                size_t l,
                size_t সারি_break,
                অক্ষর **ret) {

        ধ্রুবক uint8_t *x;
        অক্ষর *b, *z;
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
                ফেরত -ENOMEM;

        জন্য (x = p; x && x < (ধ্রুবক uint8_t*) p + (l / 3) * 3; x += 3) {
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

        নির্বাচন (l % 3) {
        ক্ষেত্র 2:
                maybe_সারি_break(&z, b, সারি_break);
                *(z++) = base64char(x[0] >> 2);                   /* 00XXXXXX */
                maybe_সারি_break(&z, b, সারি_break);
                *(z++) = base64char((x[0] & 3) << 4 | x[1] >> 4); /* 00XXYYYY */
                maybe_সারি_break(&z, b, সারি_break);
                *(z++) = base64char((x[1] & 15) << 2);            /* 00YYYY00 */
                maybe_সারি_break(&z, b, সারি_break);
                *(z++) = '=';
                বিরতি;

        ক্ষেত্র 1:
                maybe_সারি_break(&z, b, সারি_break);
                *(z++) = base64char(x[0] >> 2);        /* 00XXXXXX */
                maybe_সারি_break(&z, b, সারি_break);
                *(z++) = base64char((x[0] & 3) << 4);  /* 00XX0000 */
                maybe_সারি_break(&z, b, সারি_break);
                *(z++) = '=';
                maybe_সারি_break(&z, b, সারি_break);
                *(z++) = '=';
                বিরতি;
        }

        *z = 0;
        *ret = b;

        প্রতিজ্ঞা(z >= b); /* Let static analyzers know that the answer is non-negative. */
        ফেরত z - b;
}

স্থির ssize_t base64_append_width(
                অক্ষর **prefix,
                size_t plen,
                অক্ষর sep,
                size_t indent,
                ধ্রুবক শূন্য *p,
                size_t l,
                size_t width) {

        _cleanup_free_ অক্ষর *x = ফাঁকা;
        অক্ষর *t, *s;
        size_t সারিগুলি;
        ssize_t len;

        প্রতিজ্ঞা(prefix);
        প্রতিজ্ঞা(*prefix || plen == 0);
        প্রতিজ্ঞা(p || l == 0);

        len = base64mem(p, l, &x);
        যদি (len < 0)
                ফেরত len;
        যদি (len == 0)
                ফেরত plen;

        সারিগুলি = DIV_ROUND_UP(len, width);

        যদি (plen >= SSIZE_MAX - 1 - 1 ||
            সারিগুলি > (SSIZE_MAX - plen - 1 - 1) / (indent + width + 1))
                ফেরত -ENOMEM;

        t = realloc(*prefix, plen + 1 + 1 + (indent + width + 1) * সারিগুলি);
        যদি (!t)
                ফেরত -ENOMEM;

        s = t + plen;
        জন্য (size_t সারি = 0; সারি < সারিগুলি; সারি++) {
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
        ফেরত s - t;
}

ssize_t base64_append(
                অক্ষর **prefix,
                size_t plen,
                ধ্রুবক শূন্য *p,
                size_t l,
                size_t indent,
                size_t width) {

        যদি (plen > width / 2 || plen + indent > width)
                /* leave indent on the left, keep last column free */
                ফেরত base64_append_width(prefix, plen, '\n', indent, p, l, width - indent);
        অন্যথা
                /* leave plen on the left, keep last column free */
                ফেরত base64_append_width(prefix, plen, ' ', plen + 1, p, l, width - plen - 1);
}

স্থির পূর্ণসংখ্যা unbase64_পরবর্তী(ধ্রুবক অক্ষর **p, size_t *l) {
        পূর্ণসংখ্যা ret;

        প্রতিজ্ঞা(p);
        প্রতিজ্ঞা(l);

        /* Find the next non-whitespace character, and decode it. If we find padding, we return it as INT_MAX. We
         * greedily skip all preceding and all following whitespace. */

        জন্য (;;) {
                যদি (*l == 0)
                        ফেরত -EPIPE;

                যদি (!strchr(WHITESPACE, **p))
                        বিরতি;

                /* Skip leading whitespace */
                (*p)++, (*l)--;
        }

        যদি (**p == '=')
                ret = INT_MAX; /* return padding as INT_MAX */
        অন্যথা {
                ret = unbase64char(**p);
                যদি (ret < 0)
                        ফেরত ret;
        }

        জন্য (;;) {
                (*p)++, (*l)--;

                যদি (*l == 0)
                        বিরতি;
                যদি (!strchr(WHITESPACE, **p))
                        বিরতি;

                /* Skip following whitespace */
        }

        ফেরত ret;
}

পূর্ণসংখ্যা unbase64mem_full(
                ধ্রুবক অক্ষর *p,
                size_t l,
                bool secure,
                শূন্য **ret_তথ্য,
                size_t *ret_আকার) {

        _cleanup_free_ uint8_t *buf = ফাঁকা;
        ধ্রুবক অক্ষর *x;
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
                ফেরত -ENOMEM;

        CLEANUP_ERASE_PTR(secure ? &buf : ফাঁকা, len);

        জন্য (x = p, z = buf;;) {
                পূর্ণসংখ্যা a, b, c, d; /* a == 00XXXXXX; b == 00YYYYYY; c == 00ZZZZZZ; d == 00WWWWWW */

                a = unbase64_পরবর্তী(&x, &l);
                যদি (a == -EPIPE) /* End of string */
                        বিরতি;
                যদি (a < 0)
                        ফেরত a;
                যদি (a == INT_MAX) /* Padding is not allowed at the beginning of a 4ch block */
                        ফেরত -EINVAL;

                b = unbase64_পরবর্তী(&x, &l);
                যদি (b < 0)
                        ফেরত b;
                যদি (b == INT_MAX) /* Padding is not allowed at the second character of a 4ch block either */
                        ফেরত -EINVAL;

                c = unbase64_পরবর্তী(&x, &l);
                যদি (c < 0)
                        ফেরত c;

                d = unbase64_পরবর্তী(&x, &l);
                যদি (d < 0)
                        ফেরত d;

                যদি (c == INT_MAX) { /* Padding at the third character */

                        যদি (d != INT_MAX) /* If the third character is padding, the fourth must be too */
                                ফেরত -EINVAL;

                        /* b == 00YY0000 */
                        যদি (b & 15)
                                ফেরত -EINVAL;

                        যদি (l > 0) /* Trailing rubbish? */
                                ফেরত -ENAMETOOLONG;

                        *(z++) = (uint8_t) a << 2 | (uint8_t) (b >> 4); /* XXXXXXYY */
                        বিরতি;
                }

                যদি (d == INT_MAX) {
                        /* c == 00ZZZZ00 */
                        যদি (c & 3)
                                ফেরত -EINVAL;

                        যদি (l > 0) /* Trailing rubbish? */
                                ফেরত -ENAMETOOLONG;

                        *(z++) = (uint8_t) a << 2 | (uint8_t) b >> 4; /* XXXXXXYY */
                        *(z++) = (uint8_t) b << 4 | (uint8_t) c >> 2; /* YYYYZZZZ */
                        বিরতি;
                }

                *(z++) = (uint8_t) a << 2 | (uint8_t) b >> 4; /* XXXXXXYY */
                *(z++) = (uint8_t) b << 4 | (uint8_t) c >> 2; /* YYYYZZZZ */
                *(z++) = (uint8_t) c << 6 | (uint8_t) d;      /* ZZWWWWWW */
        }

        *z = 0;

        প্রতিজ্ঞা((size_t) (z - buf) <= len);

        যদি (ret_আকার)
                *ret_আকার = (size_t) (z - buf);
        যদি (ret_তথ্য)
                *ret_তথ্য = TAKE_PTR(buf);

        ফেরত 0;
}

শূন্য hexdump(FILE *f, ধ্রুবক শূন্য *p, size_t s) {
        ধ্রুবক uint8_t *b = p;
        unsigned n = 0;

        প্রতিজ্ঞা(b || s == 0);

        যদি (s == SIZE_MAX)
                s = strlen(p);

        যদি (!f)
                f = stdout;

        যখন (s > 0) {
                size_t i;

                fprintf(f, "%04x  ", n);

                জন্য (i = 0; i < 16; i++) {

                        যদি (i >= s)
                                fputs("   ", f);
                        অন্যথা
                                fprintf(f, "%02x ", b[i]);

                        যদি (i == 7)
                                fputc(' ', f);
                }

                fputc(' ', f);

                জন্য (i = 0; i < 16; i++) {

                        যদি (i >= s)
                                fputc(' ', f);
                        অন্যথা
                                fputc(isprint(b[i]) ? (অক্ষর) b[i] : '.', f);
                }

                fputc('\n', f);

                যদি (s < 16)
                        বিরতি;

                n += 16;
                b += 16;
                s -= 16;
        }
}
