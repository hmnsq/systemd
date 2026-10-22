/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ଅନ୍ତର୍ଭାଵ <ctype.h>
#ଅନ୍ତର୍ଭାଵ <stdio.h>

#ଅନ୍ତର୍ଭାଵ "alloc-util.h"
#ଅନ୍ତର୍ଭାଵ "hexdecoct.h"
#ଅନ୍ତର୍ଭାଵ "memory-util.h"
#ଅନ୍ତର୍ଭାଵ "string-util.h"

ଅକ୍ଷର octchar(ପୂର୍ଣ୍ଣସଂଖ୍ୟା x) {
        ଫେରନ୍ତୁ '0' + (x & 7);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା unoctchar(ଅକ୍ଷର c) {

        ଯଦି (c >= '0' && c <= '7')
                ଫେରନ୍ତୁ c - '0';

        ଫେରନ୍ତୁ -EINVAL;
}

ଅକ୍ଷର decchar(ପୂର୍ଣ୍ଣସଂଖ୍ୟା x) {
        ଫେରନ୍ତୁ '0' + (x % 10);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା undecchar(ଅକ୍ଷର c) {

        ଯଦି (c >= '0' && c <= '9')
                ଫେରନ୍ତୁ c - '0';

        ଫେରନ୍ତୁ -EINVAL;
}

ଅକ୍ଷର hexchar(ପୂର୍ଣ୍ଣସଂଖ୍ୟା x) {
        ଅଚଳ ଅକ୍ଷର *table = LOWERCASE_HEXDIGITS;
        ଫେରନ୍ତୁ table[x & 15];
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା unhexchar(ଅକ୍ଷର c) {

        ଯଦି (c >= '0' && c <= '9')
                ଫେରନ୍ତୁ c - '0';

        ଯଦି (c >= 'a' && c <= 'f')
                ଫେରନ୍ତୁ c - 'a' + 10;

        ଯଦି (c >= 'A' && c <= 'F')
                ଫେରନ୍ତୁ c - 'A' + 10;

        ଫେରନ୍ତୁ -EINVAL;
}

ଅକ୍ଷର* hexmem(ଅଚଳ ଶୂନ୍ୟ *p, size_t l) {
        ଅଚଳ uint8_t *x;
        ଅକ୍ଷର *r, *z;

        ପ୍ରତିଜ୍ଞା(p || l == 0);

        z = r = new(ଅକ୍ଷର, l * 2 + 1);
        ଯଦି (!r)
                ଫେରନ୍ତୁ ଖାଲି;

        ପାଇଁ (x = p; x && x < (ଅଚଳ uint8_t*) p + l; x++) {
                *(z++) = hexchar(*x >> 4);
                *(z++) = hexchar(*x & 15);
        }

        *z = 0;
        ଫେରନ୍ତୁ r;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା unhex_next(ଅଚଳ ଅକ୍ଷର **p, size_t *l) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(p);
        ପ୍ରତିଜ୍ଞା(l);

        /* Find the next non-whitespace character, and decode it. We
         * greedily skip all preceding and all following whitespace. */

        ପାଇଁ (;;) {
                ଯଦି (*l == 0)
                        ଫେରନ୍ତୁ -EPIPE;

                ଯଦି (!strchr(WHITESPACE, **p))
                        ଭାଙ୍ଗ;

                /* Skip leading whitespace */
                (*p)++, (*l)--;
        }

        r = unhexchar(**p);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ପାଇଁ (;;) {
                (*p)++, (*l)--;

                ଯଦି (*l == 0 || !strchr(WHITESPACE, **p))
                        ଭାଙ୍ଗ;

                /* Skip following whitespace */
        }

        ଫେରନ୍ତୁ r;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା unhexmem_full(
                ଅଚଳ ଅକ୍ଷର *p,
                size_t l,
                bool secure,
                ଶୂନ୍ୟ **ret_data,
                size_t *ret_size) {

        _cleanup_free_ uint8_t *buf = ଖାଲି;
        size_t buf_size;
        ଅଚଳ ଅକ୍ଷର *x;
        uint8_t *z;

        ପ୍ରତିଜ୍ଞା(p || l == 0);

        ଯଦି (l == SIZE_MAX)
                l = strlen(p);

        /* Note that the calculation of memory size is an upper boundary, as we ignore whitespace while decoding */
        buf_size = (l + 1) / 2 + 1;
        buf = malloc(buf_size);
        ଯଦି (!buf)
                ଫେରନ୍ତୁ -ENOMEM;

        CLEANUP_ERASE_PTR(secure ? &buf : ଖାଲି, buf_size);

        ପାଇଁ (x = p, z = buf;;) {
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା a, b;

                a = unhex_next(&x, &l);
                ଯଦି (a == -EPIPE) /* End of string */
                        ଭାଙ୍ଗ;
                ଯଦି (a < 0)
                        ଫେରନ୍ତୁ a;

                b = unhex_next(&x, &l);
                ଯଦି (b < 0)
                        ଫେରନ୍ତୁ b;

                *(z++) = (uint8_t) a << 4 | (uint8_t) b;
        }

        *z = 0;

        ଯଦି (ret_size)
                *ret_size = (size_t) (z - buf);
        ଯଦି (ret_data)
                *ret_data = TAKE_PTR(buf);

        ଫେରନ୍ତୁ 0;
}

/* https://tools.ietf.org/html/rfc4648#section-6
 * Notice that base32hex differs from base32 in the alphabet it uses.
 * The distinction is that the base32hex representation preserves the
 * order of the underlying data when compared as bytestrings, this is
 * useful when representing NSEC3 hashes, as one can then verify the
 * order of hashes directly from their representation. */
ଅକ୍ଷର base32hexchar(ପୂର୍ଣ୍ଣସଂଖ୍ୟା x) {
        ଅଚଳ ଅକ୍ଷର *table = DIGITS "ABCDEFGHIJKLMNOPQRSTUV";
        ଫେରନ୍ତୁ table[x & 31];
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା unbase32hexchar(ଅକ୍ଷର c) {
        unsigned offset;

        ଯଦି (c >= '0' && c <= '9')
                ଫେରନ୍ତୁ c - '0';

        offset = '9' - '0' + 1;

        ଯଦି (c >= 'A' && c <= 'V')
                ଫେରନ୍ତୁ c - 'A' + offset;

        ଫେରନ୍ତୁ -EINVAL;
}

ଅକ୍ଷର* base32hexmem(ଅଚଳ ଶୂନ୍ୟ *p, size_t l, bool padding) {
        ଅକ୍ଷର *r, *z;
        ଅଚଳ uint8_t *x;
        size_t len;

        ପ୍ରତିଜ୍ଞା(p || l == 0);

        ଯଦି (padding)
                /* five input bytes makes eight output bytes, padding is added so we must round up */
                len = 8 * (l + 4) / 5;
        ଅନ୍ୟଥା {
                /* same, but round down as there is no padding */
                len = 8 * l / 5;

                ବାଛ (l % 5) {
                ସ୍ଥିତି 4:
                        len += 7;
                        ଭାଙ୍ଗ;
                ସ୍ଥିତି 3:
                        len += 5;
                        ଭାଙ୍ଗ;
                ସ୍ଥିତି 2:
                        len += 4;
                        ଭାଙ୍ଗ;
                ସ୍ଥିତି 1:
                        len += 2;
                        ଭାଙ୍ଗ;
                }
        }

        z = r = malloc(len + 1);
        ଯଦି (!r)
                ଫେରନ୍ତୁ ଖାଲି;

        ପାଇଁ (x = p; x < (ଅଚଳ uint8_t*) p + (l / 5) * 5; x += 5) {
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

        ବାଛ (l % 5) {
        ସ୍ଥିତି 4:
                *(z++) = base32hexchar(x[0] >> 3);                    /* 000XXXXX */
                *(z++) = base32hexchar((x[0] & 7) << 2 | x[1] >> 6);  /* 000XXXYY */
                *(z++) = base32hexchar((x[1] & 63) >> 1);             /* 000YYYYY */
                *(z++) = base32hexchar((x[1] & 1) << 4 | x[2] >> 4);   /* 000YZZZZ */
                *(z++) = base32hexchar((x[2] & 15) << 1 | x[3] >> 7); /* 000ZZZZQ */
                *(z++) = base32hexchar((x[3] & 127) >> 2);            /* 000QQQQQ */
                *(z++) = base32hexchar((x[3] & 3) << 3);              /* 000QQ000 */
                ଯଦି (padding)
                        *(z++) = '=';

                ଭାଙ୍ଗ;

        ସ୍ଥିତି 3:
                *(z++) = base32hexchar(x[0] >> 3);                   /* 000XXXXX */
                *(z++) = base32hexchar((x[0] & 7) << 2 | x[1] >> 6); /* 000XXXYY */
                *(z++) = base32hexchar((x[1] & 63) >> 1);            /* 000YYYYY */
                *(z++) = base32hexchar((x[1] & 1) << 4 | x[2] >> 4); /* 000YZZZZ */
                *(z++) = base32hexchar((x[2] & 15) << 1);            /* 000ZZZZ0 */
                ଯଦି (padding) {
                        *(z++) = '=';
                        *(z++) = '=';
                        *(z++) = '=';
                }

                ଭାଙ୍ଗ;

        ସ୍ଥିତି 2:
                *(z++) = base32hexchar(x[0] >> 3);                   /* 000XXXXX */
                *(z++) = base32hexchar((x[0] & 7) << 2 | x[1] >> 6); /* 000XXXYY */
                *(z++) = base32hexchar((x[1] & 63) >> 1);            /* 000YYYYY */
                *(z++) = base32hexchar((x[1] & 1) << 4);             /* 000Y0000 */
                ଯଦି (padding) {
                        *(z++) = '=';
                        *(z++) = '=';
                        *(z++) = '=';
                        *(z++) = '=';
                }

                ଭାଙ୍ଗ;

        ସ୍ଥିତି 1:
                *(z++) = base32hexchar(x[0] >> 3);       /* 000XXXXX */
                *(z++) = base32hexchar((x[0] & 7) << 2); /* 000XXX00 */
                ଯଦି (padding) {
                        *(z++) = '=';
                        *(z++) = '=';
                        *(z++) = '=';
                        *(z++) = '=';
                        *(z++) = '=';
                        *(z++) = '=';
                }

                ଭାଙ୍ଗ;
        }

        *z = 0;
        ଫେରନ୍ତୁ r;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା unbase32hexmem(ଅଚଳ ଅକ୍ଷର *p, size_t l, bool padding, ଶୂନ୍ୟ **mem, size_t *_len) {
        _cleanup_free_ uint8_t *r = ଖାଲି;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା a, b, c, d, e, f, g, h;
        uint8_t *z;
        ଅଚଳ ଅକ୍ଷର *x;
        size_t len;
        unsigned pad = 0;

        ପ୍ରତିଜ୍ଞା(p || l == 0);
        ପ୍ରତିଜ୍ଞା(mem);
        ପ୍ରତିଜ୍ଞା(_len);

        ଯଦି (l == SIZE_MAX)
                l = strlen(p);

        /* padding ensures any base32hex input has input divisible by 8 */
        ଯଦି (padding && l % 8 != 0)
                ଫେରନ୍ତୁ -EINVAL;

        ଯଦି (padding) {
                /* strip the padding */
                ଯେତେବେଳେ (l > 0 && p[l - 1] == '=' && pad < 7) {
                        pad++;
                        l--;
                }
        }

        /* a group of eight input bytes needs five output bytes, in case of
         * padding we need to add some extra bytes */
        len = (l / 8) * 5;

        ବାଛ (l % 8) {
        ସ୍ଥିତି 7:
                len += 4;
                ଭାଙ୍ଗ;
        ସ୍ଥିତି 5:
                len += 3;
                ଭାଙ୍ଗ;
        ସ୍ଥିତି 4:
                len += 2;
                ଭାଙ୍ଗ;
        ସ୍ଥିତି 2:
                len += 1;
                ଭାଙ୍ଗ;
        ସ୍ଥିତି 0:
                ଭାଙ୍ଗ;
        ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
                ଫେରନ୍ତୁ -EINVAL;
        }

        z = r = malloc(len + 1);
        ଯଦି (!r)
                ଫେରନ୍ତୁ -ENOMEM;

        ପାଇଁ (x = p; x < p + (l / 8) * 8; x += 8) {
                /* a == 000XXXXX; b == 000YYYYY; c == 000ZZZZZ; d == 000WWWWW
                 * e == 000SSSSS; f == 000QQQQQ; g == 000VVVVV; h == 000RRRRR */
                a = unbase32hexchar(x[0]);
                ଯଦି (a < 0)
                        ଫେରନ୍ତୁ -EINVAL;

                b = unbase32hexchar(x[1]);
                ଯଦି (b < 0)
                        ଫେରନ୍ତୁ -EINVAL;

                c = unbase32hexchar(x[2]);
                ଯଦି (c < 0)
                        ଫେରନ୍ତୁ -EINVAL;

                d = unbase32hexchar(x[3]);
                ଯଦି (d < 0)
                        ଫେରନ୍ତୁ -EINVAL;

                e = unbase32hexchar(x[4]);
                ଯଦି (e < 0)
                        ଫେରନ୍ତୁ -EINVAL;

                f = unbase32hexchar(x[5]);
                ଯଦି (f < 0)
                        ଫେରନ୍ତୁ -EINVAL;

                g = unbase32hexchar(x[6]);
                ଯଦି (g < 0)
                        ଫେରନ୍ତୁ -EINVAL;

                h = unbase32hexchar(x[7]);
                ଯଦି (h < 0)
                        ଫେରନ୍ତୁ -EINVAL;

                *(z++) = (uint8_t) a << 3 | (uint8_t) b >> 2;                    /* XXXXXYYY */
                *(z++) = (uint8_t) b << 6 | (uint8_t) c << 1 | (uint8_t) d >> 4; /* YYZZZZZW */
                *(z++) = (uint8_t) d << 4 | (uint8_t) e >> 1;                    /* WWWWSSSS */
                *(z++) = (uint8_t) e << 7 | (uint8_t) f << 2 | (uint8_t) g >> 3; /* SQQQQQVV */
                *(z++) = (uint8_t) g << 5 | (uint8_t) h;                         /* VVVRRRRR */
        }

        ବାଛ (l % 8) {
        ସ୍ଥିତି 7:
                a = unbase32hexchar(x[0]);
                ଯଦି (a < 0)
                        ଫେରନ୍ତୁ -EINVAL;

                b = unbase32hexchar(x[1]);
                ଯଦି (b < 0)
                        ଫେରନ୍ତୁ -EINVAL;

                c = unbase32hexchar(x[2]);
                ଯଦି (c < 0)
                        ଫେରନ୍ତୁ -EINVAL;

                d = unbase32hexchar(x[3]);
                ଯଦି (d < 0)
                        ଫେରନ୍ତୁ -EINVAL;

                e = unbase32hexchar(x[4]);
                ଯଦି (e < 0)
                        ଫେରନ୍ତୁ -EINVAL;

                f = unbase32hexchar(x[5]);
                ଯଦି (f < 0)
                        ଫେରନ୍ତୁ -EINVAL;

                g = unbase32hexchar(x[6]);
                ଯଦି (g < 0)
                        ଫେରନ୍ତୁ -EINVAL;

                /* g == 000VV000 */
                ଯଦି (g & 7)
                        ଫେରନ୍ତୁ -EINVAL;

                *(z++) = (uint8_t) a << 3 | (uint8_t) b >> 2;                    /* XXXXXYYY */
                *(z++) = (uint8_t) b << 6 | (uint8_t) c << 1 | (uint8_t) d >> 4; /* YYZZZZZW */
                *(z++) = (uint8_t) d << 4 | (uint8_t) e >> 1;                    /* WWWWSSSS */
                *(z++) = (uint8_t) e << 7 | (uint8_t) f << 2 | (uint8_t) g >> 3; /* SQQQQQVV */

                ଭାଙ୍ଗ;
        ସ୍ଥିତି 5:
                a = unbase32hexchar(x[0]);
                ଯଦି (a < 0)
                        ଫେରନ୍ତୁ -EINVAL;

                b = unbase32hexchar(x[1]);
                ଯଦି (b < 0)
                        ଫେରନ୍ତୁ -EINVAL;

                c = unbase32hexchar(x[2]);
                ଯଦି (c < 0)
                        ଫେରନ୍ତୁ -EINVAL;

                d = unbase32hexchar(x[3]);
                ଯଦି (d < 0)
                        ଫେରନ୍ତୁ -EINVAL;

                e = unbase32hexchar(x[4]);
                ଯଦି (e < 0)
                        ଫେରନ୍ତୁ -EINVAL;

                /* e == 000SSSS0 */
                ଯଦି (e & 1)
                        ଫେରନ୍ତୁ -EINVAL;

                *(z++) = (uint8_t) a << 3 | (uint8_t) b >> 2;                    /* XXXXXYYY */
                *(z++) = (uint8_t) b << 6 | (uint8_t) c << 1 | (uint8_t) d >> 4; /* YYZZZZZW */
                *(z++) = (uint8_t) d << 4 | (uint8_t) e >> 1;                    /* WWWWSSSS */

                ଭାଙ୍ଗ;
        ସ୍ଥିତି 4:
                a = unbase32hexchar(x[0]);
                ଯଦି (a < 0)
                        ଫେରନ୍ତୁ -EINVAL;

                b = unbase32hexchar(x[1]);
                ଯଦି (b < 0)
                        ଫେରନ୍ତୁ -EINVAL;

                c = unbase32hexchar(x[2]);
                ଯଦି (c < 0)
                        ଫେରନ୍ତୁ -EINVAL;

                d = unbase32hexchar(x[3]);
                ଯଦି (d < 0)
                        ଫେରନ୍ତୁ -EINVAL;

                /* d == 000W0000 */
                ଯଦି (d & 15)
                        ଫେରନ୍ତୁ -EINVAL;

                *(z++) = (uint8_t) a << 3 | (uint8_t) b >> 2;                    /* XXXXXYYY */
                *(z++) = (uint8_t) b << 6 | (uint8_t) c << 1 | (uint8_t) d >> 4; /* YYZZZZZW */

                ଭାଙ୍ଗ;
        ସ୍ଥିତି 2:
                a = unbase32hexchar(x[0]);
                ଯଦି (a < 0)
                        ଫେରନ୍ତୁ -EINVAL;

                b = unbase32hexchar(x[1]);
                ଯଦି (b < 0)
                        ଫେରନ୍ତୁ -EINVAL;

                /* b == 000YYY00 */
                ଯଦି (b & 3)
                        ଫେରନ୍ତୁ -EINVAL;

                *(z++) = (uint8_t) a << 3 | (uint8_t) b >> 2; /* XXXXXYYY */

                ଭାଙ୍ଗ;
        ସ୍ଥିତି 0:
                ଭାଙ୍ଗ;
        ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
                ଫେରନ୍ତୁ -EINVAL;
        }

        *z = 0;

        *mem = TAKE_PTR(r);
        *_len = len;

        ଫେରନ୍ତୁ 0;
}

/* https://tools.ietf.org/html/rfc4648#section-4 */
ଅକ୍ଷର base64char(ପୂର୍ଣ୍ଣସଂଖ୍ୟା x) {
        ଅଚଳ ଅକ୍ଷର *table = UPPERCASE_LETTERS LOWERCASE_LETTERS DIGITS "+/";
        ଫେରନ୍ତୁ table[x & 63];
}

/* This is almost base64char(), but not entirely, as it uses the "url and filename safe" alphabet,
 * since we don't want "/" appear in interface names (since interfaces appear in sysfs as filenames).
 * See section #5 of RFC 4648. */
ଅକ୍ଷର urlsafe_base64char(ପୂର୍ଣ୍ଣସଂଖ୍ୟା x) {
        ଅଚଳ ଅକ୍ଷର *table = UPPERCASE_LETTERS LOWERCASE_LETTERS DIGITS "-_";
        ଫେରନ୍ତୁ table[x & 63];
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା unbase64char(ଅକ୍ଷର c) {
        unsigned offset;

        ଯଦି (c >= 'A' && c <= 'Z')
                ଫେରନ୍ତୁ c - 'A';

        offset = 'Z' - 'A' + 1;

        ଯଦି (c >= 'a' && c <= 'z')
                ଫେରନ୍ତୁ c - 'a' + offset;

        offset += 'z' - 'a' + 1;

        ଯଦି (c >= '0' && c <= '9')
                ଫେରନ୍ତୁ c - '0' + offset;

        offset += '9' - '0' + 1;

        ଯଦି (IN_SET(c, '+', '-')) /* Support both the regular and the URL safe character set (see above) */
                ଫେରନ୍ତୁ offset;

        offset++;

        ଯଦି (IN_SET(c, '/', '_')) /* ditto */
                ଫେରନ୍ତୁ offset;

        ଫେରନ୍ତୁ -EINVAL;
}

ସ୍ଥିର ଶୂନ୍ୟ maybe_ଧାଡ଼ି_break(ଅକ୍ଷର **x, ଅକ୍ଷର *start, size_t ଧାଡ଼ି_break) {
        size_t n;

        ପ୍ରତିଜ୍ଞା(x);
        ପ୍ରତିଜ୍ଞା(*x);
        ପ୍ରତିଜ୍ଞା(start);
        ପ୍ରତିଜ୍ଞା(*x >= start);

        ଯଦି (ଧାଡ଼ି_break == SIZE_MAX)
                ଫେରନ୍ତୁ;

        n = *x - start;

        ଯଦି (n % (ଧାଡ଼ି_break + 1) == ଧାଡ଼ି_break)
                *((*x)++) = '\n';
}

ssize_t base64mem_full(
                ଅଚଳ ଶୂନ୍ୟ *p,
                size_t l,
                size_t ଧାଡ଼ି_break,
                ଅକ୍ଷର **ret) {

        ଅଚଳ uint8_t *x;
        ଅକ୍ଷର *b, *z;
        size_t m;

        ପ୍ରତିଜ୍ଞା(p || l == 0);
        ପ୍ରତିଜ୍ଞା(ଧାଡ଼ି_break > 0);
        ପ୍ରତିଜ୍ଞା(ret);

        /* three input bytes makes four output bytes, padding is added so we must round up */
        m = 4 * (l + 2) / 3 + 1;
        ଯଦି (ଧାଡ଼ି_break != SIZE_MAX)
                m += m / ଧାଡ଼ି_break;

        z = b = malloc(m);
        ଯଦି (!b)
                ଫେରନ୍ତୁ -ENOMEM;

        ପାଇଁ (x = p; x && x < (ଅଚଳ uint8_t*) p + (l / 3) * 3; x += 3) {
                /* x[0] == XXXXXXXX; x[1] == YYYYYYYY; x[2] == ZZZZZZZZ */
                maybe_ଧାଡ଼ି_break(&z, b, ଧାଡ଼ି_break);
                *(z++) = base64char(x[0] >> 2);                    /* 00XXXXXX */
                maybe_ଧାଡ଼ି_break(&z, b, ଧାଡ଼ି_break);
                *(z++) = base64char((x[0] & 3) << 4 | x[1] >> 4);  /* 00XXYYYY */
                maybe_ଧାଡ଼ି_break(&z, b, ଧାଡ଼ି_break);
                *(z++) = base64char((x[1] & 15) << 2 | x[2] >> 6); /* 00YYYYZZ */
                maybe_ଧାଡ଼ି_break(&z, b, ଧାଡ଼ି_break);
                *(z++) = base64char(x[2] & 63);                    /* 00ZZZZZZ */
        }

        ବାଛ (l % 3) {
        ସ୍ଥିତି 2:
                maybe_ଧାଡ଼ି_break(&z, b, ଧାଡ଼ି_break);
                *(z++) = base64char(x[0] >> 2);                   /* 00XXXXXX */
                maybe_ଧାଡ଼ି_break(&z, b, ଧାଡ଼ି_break);
                *(z++) = base64char((x[0] & 3) << 4 | x[1] >> 4); /* 00XXYYYY */
                maybe_ଧାଡ଼ି_break(&z, b, ଧାଡ଼ି_break);
                *(z++) = base64char((x[1] & 15) << 2);            /* 00YYYY00 */
                maybe_ଧାଡ଼ି_break(&z, b, ଧାଡ଼ି_break);
                *(z++) = '=';
                ଭାଙ୍ଗ;

        ସ୍ଥିତି 1:
                maybe_ଧାଡ଼ି_break(&z, b, ଧାଡ଼ି_break);
                *(z++) = base64char(x[0] >> 2);        /* 00XXXXXX */
                maybe_ଧାଡ଼ି_break(&z, b, ଧାଡ଼ି_break);
                *(z++) = base64char((x[0] & 3) << 4);  /* 00XX0000 */
                maybe_ଧାଡ଼ି_break(&z, b, ଧାଡ଼ି_break);
                *(z++) = '=';
                maybe_ଧାଡ଼ି_break(&z, b, ଧାଡ଼ି_break);
                *(z++) = '=';
                ଭାଙ୍ଗ;
        }

        *z = 0;
        *ret = b;

        ପ୍ରତିଜ୍ଞା(z >= b); /* Let static analyzers know that the answer is non-negative. */
        ଫେରନ୍ତୁ z - b;
}

ସ୍ଥିର ssize_t base64_append_width(
                ଅକ୍ଷର **prefix,
                size_t plen,
                ଅକ୍ଷର sep,
                size_t indent,
                ଅଚଳ ଶୂନ୍ୟ *p,
                size_t l,
                size_t width) {

        _cleanup_free_ ଅକ୍ଷର *x = ଖାଲି;
        ଅକ୍ଷର *t, *s;
        size_t ଧାଡ଼ିଗୁଡ଼ିକ;
        ssize_t len;

        ପ୍ରତିଜ୍ଞା(prefix);
        ପ୍ରତିଜ୍ଞା(*prefix || plen == 0);
        ପ୍ରତିଜ୍ଞା(p || l == 0);

        len = base64mem(p, l, &x);
        ଯଦି (len < 0)
                ଫେରନ୍ତୁ len;
        ଯଦି (len == 0)
                ଫେରନ୍ତୁ plen;

        ଧାଡ଼ିଗୁଡ଼ିକ = DIV_ROUND_UP(len, width);

        ଯଦି (plen >= SSIZE_MAX - 1 - 1 ||
            ଧାଡ଼ିଗୁଡ଼ିକ > (SSIZE_MAX - plen - 1 - 1) / (indent + width + 1))
                ଫେରନ୍ତୁ -ENOMEM;

        t = realloc(*prefix, plen + 1 + 1 + (indent + width + 1) * ଧାଡ଼ିଗୁଡ଼ିକ);
        ଯଦି (!t)
                ଫେରନ୍ତୁ -ENOMEM;

        s = t + plen;
        ପାଇଁ (size_t ଧାଡ଼ି = 0; ଧାଡ଼ି < ଧାଡ଼ିଗୁଡ଼ିକ; ଧାଡ଼ି++) {
                size_t act = MIN(width, (size_t) len);

                ଯଦି (ଧାଡ଼ି > 0)
                        sep = '\n';

                ଯଦି (s > t) {
                        *s++ = sep;
                        ଯଦି (sep == '\n')
                                s = mempset(s, ' ', indent);
                }

                s = mempcpy(s, x + width * ଧାଡ଼ି, act);
                len -= act;
        }
        ପ୍ରତିଜ୍ଞା(len == 0);

        *s = '\0';
        *prefix = t;
        ଫେରନ୍ତୁ s - t;
}

ssize_t base64_append(
                ଅକ୍ଷର **prefix,
                size_t plen,
                ଅଚଳ ଶୂନ୍ୟ *p,
                size_t l,
                size_t indent,
                size_t width) {

        ଯଦି (plen > width / 2 || plen + indent > width)
                /* leave indent on the left, keep last column free */
                ଫେରନ୍ତୁ base64_append_width(prefix, plen, '\n', indent, p, l, width - indent);
        ଅନ୍ୟଥା
                /* leave plen on the left, keep last column free */
                ଫେରନ୍ତୁ base64_append_width(prefix, plen, ' ', plen + 1, p, l, width - plen - 1);
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା unbase64_next(ଅଚଳ ଅକ୍ଷର **p, size_t *l) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret;

        ପ୍ରତିଜ୍ଞା(p);
        ପ୍ରତିଜ୍ଞା(l);

        /* Find the next non-whitespace character, and decode it. If we find padding, we return it as INT_MAX. We
         * greedily skip all preceding and all following whitespace. */

        ପାଇଁ (;;) {
                ଯଦି (*l == 0)
                        ଫେରନ୍ତୁ -EPIPE;

                ଯଦି (!strchr(WHITESPACE, **p))
                        ଭାଙ୍ଗ;

                /* Skip leading whitespace */
                (*p)++, (*l)--;
        }

        ଯଦି (**p == '=')
                ret = INT_MAX; /* return padding as INT_MAX */
        ଅନ୍ୟଥା {
                ret = unbase64char(**p);
                ଯଦି (ret < 0)
                        ଫେରନ୍ତୁ ret;
        }

        ପାଇଁ (;;) {
                (*p)++, (*l)--;

                ଯଦି (*l == 0)
                        ଭାଙ୍ଗ;
                ଯଦି (!strchr(WHITESPACE, **p))
                        ଭାଙ୍ଗ;

                /* Skip following whitespace */
        }

        ଫେରନ୍ତୁ ret;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା unbase64mem_full(
                ଅଚଳ ଅକ୍ଷର *p,
                size_t l,
                bool secure,
                ଶୂନ୍ୟ **ret_data,
                size_t *ret_size) {

        _cleanup_free_ uint8_t *buf = ଖାଲି;
        ଅଚଳ ଅକ୍ଷର *x;
        uint8_t *z;
        size_t len;

        ପ୍ରତିଜ୍ଞା(p || l == 0);

        ଯଦି (l == SIZE_MAX)
                l = strlen(p);

        /* A group of four input bytes needs three output bytes, in case of padding we need to add two or three extra
         * bytes. Note that this calculation is an upper boundary, as we ignore whitespace while decoding */
        len = (l / 4) * 3 + (l % 4 != 0 ? (l % 4) - 1 : 0);

        buf = malloc(len + 1);
        ଯଦି (!buf)
                ଫେରନ୍ତୁ -ENOMEM;

        CLEANUP_ERASE_PTR(secure ? &buf : ଖାଲି, len);

        ପାଇଁ (x = p, z = buf;;) {
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା a, b, c, d; /* a == 00XXXXXX; b == 00YYYYYY; c == 00ZZZZZZ; d == 00WWWWWW */

                a = unbase64_next(&x, &l);
                ଯଦି (a == -EPIPE) /* End of string */
                        ଭାଙ୍ଗ;
                ଯଦି (a < 0)
                        ଫେରନ୍ତୁ a;
                ଯଦି (a == INT_MAX) /* Padding is not allowed at the beginning of a 4ch block */
                        ଫେରନ୍ତୁ -EINVAL;

                b = unbase64_next(&x, &l);
                ଯଦି (b < 0)
                        ଫେରନ୍ତୁ b;
                ଯଦି (b == INT_MAX) /* Padding is not allowed at the second character of a 4ch block either */
                        ଫେରନ୍ତୁ -EINVAL;

                c = unbase64_next(&x, &l);
                ଯଦି (c < 0)
                        ଫେରନ୍ତୁ c;

                d = unbase64_next(&x, &l);
                ଯଦି (d < 0)
                        ଫେରନ୍ତୁ d;

                ଯଦି (c == INT_MAX) { /* Padding at the third character */

                        ଯଦି (d != INT_MAX) /* If the third character is padding, the fourth must be too */
                                ଫେରନ୍ତୁ -EINVAL;

                        /* b == 00YY0000 */
                        ଯଦି (b & 15)
                                ଫେରନ୍ତୁ -EINVAL;

                        ଯଦି (l > 0) /* Trailing rubbish? */
                                ଫେରନ୍ତୁ -ENAMETOOLONG;

                        *(z++) = (uint8_t) a << 2 | (uint8_t) (b >> 4); /* XXXXXXYY */
                        ଭାଙ୍ଗ;
                }

                ଯଦି (d == INT_MAX) {
                        /* c == 00ZZZZ00 */
                        ଯଦି (c & 3)
                                ଫେରନ୍ତୁ -EINVAL;

                        ଯଦି (l > 0) /* Trailing rubbish? */
                                ଫେରନ୍ତୁ -ENAMETOOLONG;

                        *(z++) = (uint8_t) a << 2 | (uint8_t) b >> 4; /* XXXXXXYY */
                        *(z++) = (uint8_t) b << 4 | (uint8_t) c >> 2; /* YYYYZZZZ */
                        ଭାଙ୍ଗ;
                }

                *(z++) = (uint8_t) a << 2 | (uint8_t) b >> 4; /* XXXXXXYY */
                *(z++) = (uint8_t) b << 4 | (uint8_t) c >> 2; /* YYYYZZZZ */
                *(z++) = (uint8_t) c << 6 | (uint8_t) d;      /* ZZWWWWWW */
        }

        *z = 0;

        ପ୍ରତିଜ୍ଞା((size_t) (z - buf) <= len);

        ଯଦି (ret_size)
                *ret_size = (size_t) (z - buf);
        ଯଦି (ret_data)
                *ret_data = TAKE_PTR(buf);

        ଫେରନ୍ତୁ 0;
}

ଶୂନ୍ୟ hexdump(FILE *f, ଅଚଳ ଶୂନ୍ୟ *p, size_t s) {
        ଅଚଳ uint8_t *b = p;
        unsigned n = 0;

        ପ୍ରତିଜ୍ଞା(b || s == 0);

        ଯଦି (s == SIZE_MAX)
                s = strlen(p);

        ଯଦି (!f)
                f = stdout;

        ଯେତେବେଳେ (s > 0) {
                size_t i;

                fprintf(f, "%04x  ", n);

                ପାଇଁ (i = 0; i < 16; i++) {

                        ଯଦି (i >= s)
                                fputs("   ", f);
                        ଅନ୍ୟଥା
                                fprintf(f, "%02x ", b[i]);

                        ଯଦି (i == 7)
                                fputc(' ', f);
                }

                fputc(' ', f);

                ପାଇଁ (i = 0; i < 16; i++) {

                        ଯଦି (i >= s)
                                fputc(' ', f);
                        ଅନ୍ୟଥା
                                fputc(isprint(b[i]) ? (ଅକ୍ଷର) b[i] : '.', f);
                }

                fputc('\n', f);

                ଯଦି (s < 16)
                        ଭାଙ୍ଗ;

                n += 16;
                b += 16;
                s -= 16;
        }
}
