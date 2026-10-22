/* SPDX-License-Identifier: LGPL-2.1-or-later */

#અન્તર્ભાવ <ctype.h>
#અન્તર્ભાવ <stdio.h>

#અન્તર્ભાવ "alloc-util.h"
#અન્તર્ભાવ "hexdecoct.h"
#અન્તર્ભાવ "memory-util.h"
#અન્તર્ભાવ "string-util.h"

અક્ષર octchar(પૂર્ણાંક x) {
        પાછા '0' + (x & 7);
}

પૂર્ણાંક unoctchar(અક્ષર c) {

        જો (c >= '0' && c <= '7')
                પાછા c - '0';

        પાછા -EINVAL;
}

અક્ષર decchar(પૂર્ણાંક x) {
        પાછા '0' + (x % 10);
}

પૂર્ણાંક undecchar(અક્ષર c) {

        જો (c >= '0' && c <= '9')
                પાછા c - '0';

        પાછા -EINVAL;
}

અક્ષર hexchar(પૂર્ણાંક x) {
        અચલ અક્ષર *table = LOWERCASE_HEXDIGITS;
        પાછા table[x & 15];
}

પૂર્ણાંક unhexchar(અક્ષર c) {

        જો (c >= '0' && c <= '9')
                પાછા c - '0';

        જો (c >= 'a' && c <= 'f')
                પાછા c - 'a' + 10;

        જો (c >= 'A' && c <= 'F')
                પાછા c - 'A' + 10;

        પાછા -EINVAL;
}

અક્ષર* hexmem(અચલ શૂન્ય *p, size_t l) {
        અચલ uint8_t *x;
        અક્ષર *r, *z;

        પ્રતિજ્ઞા(p || l == 0);

        z = r = new(અક્ષર, l * 2 + 1);
        જો (!r)
                પાછા ખાલી;

        માટે (x = p; x && x < (અચલ uint8_t*) p + l; x++) {
                *(z++) = hexchar(*x >> 4);
                *(z++) = hexchar(*x & 15);
        }

        *z = 0;
        પાછા r;
}

સ્થિર પૂર્ણાંક unhex_next(અચલ અક્ષર **p, size_t *l) {
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(p);
        પ્રતિજ્ઞા(l);

        /* Find the next non-whitespace character, and decode it. We
         * greedily skip all preceding and all following whitespace. */

        માટે (;;) {
                જો (*l == 0)
                        પાછા -EPIPE;

                જો (!strchr(WHITESPACE, **p))
                        તોડો;

                /* Skip leading whitespace */
                (*p)++, (*l)--;
        }

        r = unhexchar(**p);
        જો (r < 0)
                પાછા r;

        માટે (;;) {
                (*p)++, (*l)--;

                જો (*l == 0 || !strchr(WHITESPACE, **p))
                        તોડો;

                /* Skip following whitespace */
        }

        પાછા r;
}

પૂર્ણાંક unhexmem_full(
                અચલ અક્ષર *p,
                size_t l,
                bool secure,
                શૂન્ય **ret_data,
                size_t *ret_size) {

        _cleanup_free_ uint8_t *buf = ખાલી;
        size_t buf_size;
        અચલ અક્ષર *x;
        uint8_t *z;

        પ્રતિજ્ઞા(p || l == 0);

        જો (l == SIZE_MAX)
                l = strlen(p);

        /* Note that the calculation of memory size is an upper boundary, as we ignore whitespace while decoding */
        buf_size = (l + 1) / 2 + 1;
        buf = malloc(buf_size);
        જો (!buf)
                પાછા -ENOMEM;

        CLEANUP_ERASE_PTR(secure ? &buf : ખાલી, buf_size);

        માટે (x = p, z = buf;;) {
                પૂર્ણાંક a, b;

                a = unhex_next(&x, &l);
                જો (a == -EPIPE) /* End of string */
                        તોડો;
                જો (a < 0)
                        પાછા a;

                b = unhex_next(&x, &l);
                જો (b < 0)
                        પાછા b;

                *(z++) = (uint8_t) a << 4 | (uint8_t) b;
        }

        *z = 0;

        જો (ret_size)
                *ret_size = (size_t) (z - buf);
        જો (ret_data)
                *ret_data = TAKE_PTR(buf);

        પાછા 0;
}

/* https://tools.ietf.org/html/rfc4648#section-6
 * Notice that base32hex differs from base32 in the alphabet it uses.
 * The distinction is that the base32hex representation preserves the
 * order of the underlying data when compared as bytestrings, this is
 * useful when representing NSEC3 hashes, as one can then verify the
 * order of hashes directly from their representation. */
અક્ષર base32hexchar(પૂર્ણાંક x) {
        અચલ અક્ષર *table = DIGITS "ABCDEFGHIJKLMNOPQRSTUV";
        પાછા table[x & 31];
}

પૂર્ણાંક unbase32hexchar(અક્ષર c) {
        unsigned offset;

        જો (c >= '0' && c <= '9')
                પાછા c - '0';

        offset = '9' - '0' + 1;

        જો (c >= 'A' && c <= 'V')
                પાછા c - 'A' + offset;

        પાછા -EINVAL;
}

અક્ષર* base32hexmem(અચલ શૂન્ય *p, size_t l, bool padding) {
        અક્ષર *r, *z;
        અચલ uint8_t *x;
        size_t len;

        પ્રતિજ્ઞા(p || l == 0);

        જો (padding)
                /* five input bytes makes eight output bytes, padding is added so we must round up */
                len = 8 * (l + 4) / 5;
        નહીંતર {
                /* same, but round down as there is no padding */
                len = 8 * l / 5;

                પસંદગી (l % 5) {
                સ્થિતિ 4:
                        len += 7;
                        તોડો;
                સ્થિતિ 3:
                        len += 5;
                        તોડો;
                સ્થિતિ 2:
                        len += 4;
                        તોડો;
                સ્થિતિ 1:
                        len += 2;
                        તોડો;
                }
        }

        z = r = malloc(len + 1);
        જો (!r)
                પાછા ખાલી;

        માટે (x = p; x < (અચલ uint8_t*) p + (l / 5) * 5; x += 5) {
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

        પસંદગી (l % 5) {
        સ્થિતિ 4:
                *(z++) = base32hexchar(x[0] >> 3);                    /* 000XXXXX */
                *(z++) = base32hexchar((x[0] & 7) << 2 | x[1] >> 6);  /* 000XXXYY */
                *(z++) = base32hexchar((x[1] & 63) >> 1);             /* 000YYYYY */
                *(z++) = base32hexchar((x[1] & 1) << 4 | x[2] >> 4);   /* 000YZZZZ */
                *(z++) = base32hexchar((x[2] & 15) << 1 | x[3] >> 7); /* 000ZZZZQ */
                *(z++) = base32hexchar((x[3] & 127) >> 2);            /* 000QQQQQ */
                *(z++) = base32hexchar((x[3] & 3) << 3);              /* 000QQ000 */
                જો (padding)
                        *(z++) = '=';

                તોડો;

        સ્થિતિ 3:
                *(z++) = base32hexchar(x[0] >> 3);                   /* 000XXXXX */
                *(z++) = base32hexchar((x[0] & 7) << 2 | x[1] >> 6); /* 000XXXYY */
                *(z++) = base32hexchar((x[1] & 63) >> 1);            /* 000YYYYY */
                *(z++) = base32hexchar((x[1] & 1) << 4 | x[2] >> 4); /* 000YZZZZ */
                *(z++) = base32hexchar((x[2] & 15) << 1);            /* 000ZZZZ0 */
                જો (padding) {
                        *(z++) = '=';
                        *(z++) = '=';
                        *(z++) = '=';
                }

                તોડો;

        સ્થિતિ 2:
                *(z++) = base32hexchar(x[0] >> 3);                   /* 000XXXXX */
                *(z++) = base32hexchar((x[0] & 7) << 2 | x[1] >> 6); /* 000XXXYY */
                *(z++) = base32hexchar((x[1] & 63) >> 1);            /* 000YYYYY */
                *(z++) = base32hexchar((x[1] & 1) << 4);             /* 000Y0000 */
                જો (padding) {
                        *(z++) = '=';
                        *(z++) = '=';
                        *(z++) = '=';
                        *(z++) = '=';
                }

                તોડો;

        સ્થિતિ 1:
                *(z++) = base32hexchar(x[0] >> 3);       /* 000XXXXX */
                *(z++) = base32hexchar((x[0] & 7) << 2); /* 000XXX00 */
                જો (padding) {
                        *(z++) = '=';
                        *(z++) = '=';
                        *(z++) = '=';
                        *(z++) = '=';
                        *(z++) = '=';
                        *(z++) = '=';
                }

                તોડો;
        }

        *z = 0;
        પાછા r;
}

પૂર્ણાંક unbase32hexmem(અચલ અક્ષર *p, size_t l, bool padding, શૂન્ય **mem, size_t *_len) {
        _cleanup_free_ uint8_t *r = ખાલી;
        પૂર્ણાંક a, b, c, d, e, f, g, h;
        uint8_t *z;
        અચલ અક્ષર *x;
        size_t len;
        unsigned pad = 0;

        પ્રતિજ્ઞા(p || l == 0);
        પ્રતિજ્ઞા(mem);
        પ્રતિજ્ઞા(_len);

        જો (l == SIZE_MAX)
                l = strlen(p);

        /* padding ensures any base32hex input has input divisible by 8 */
        જો (padding && l % 8 != 0)
                પાછા -EINVAL;

        જો (padding) {
                /* strip the padding */
                જ્યારે (l > 0 && p[l - 1] == '=' && pad < 7) {
                        pad++;
                        l--;
                }
        }

        /* a group of eight input bytes needs five output bytes, in case of
         * padding we need to add some extra bytes */
        len = (l / 8) * 5;

        પસંદગી (l % 8) {
        સ્થિતિ 7:
                len += 4;
                તોડો;
        સ્થિતિ 5:
                len += 3;
                તોડો;
        સ્થિતિ 4:
                len += 2;
                તોડો;
        સ્થિતિ 2:
                len += 1;
                તોડો;
        સ્થિતિ 0:
                તોડો;
        મૂળભૂત:
                પાછા -EINVAL;
        }

        z = r = malloc(len + 1);
        જો (!r)
                પાછા -ENOMEM;

        માટે (x = p; x < p + (l / 8) * 8; x += 8) {
                /* a == 000XXXXX; b == 000YYYYY; c == 000ZZZZZ; d == 000WWWWW
                 * e == 000SSSSS; f == 000QQQQQ; g == 000VVVVV; h == 000RRRRR */
                a = unbase32hexchar(x[0]);
                જો (a < 0)
                        પાછા -EINVAL;

                b = unbase32hexchar(x[1]);
                જો (b < 0)
                        પાછા -EINVAL;

                c = unbase32hexchar(x[2]);
                જો (c < 0)
                        પાછા -EINVAL;

                d = unbase32hexchar(x[3]);
                જો (d < 0)
                        પાછા -EINVAL;

                e = unbase32hexchar(x[4]);
                જો (e < 0)
                        પાછા -EINVAL;

                f = unbase32hexchar(x[5]);
                જો (f < 0)
                        પાછા -EINVAL;

                g = unbase32hexchar(x[6]);
                જો (g < 0)
                        પાછા -EINVAL;

                h = unbase32hexchar(x[7]);
                જો (h < 0)
                        પાછા -EINVAL;

                *(z++) = (uint8_t) a << 3 | (uint8_t) b >> 2;                    /* XXXXXYYY */
                *(z++) = (uint8_t) b << 6 | (uint8_t) c << 1 | (uint8_t) d >> 4; /* YYZZZZZW */
                *(z++) = (uint8_t) d << 4 | (uint8_t) e >> 1;                    /* WWWWSSSS */
                *(z++) = (uint8_t) e << 7 | (uint8_t) f << 2 | (uint8_t) g >> 3; /* SQQQQQVV */
                *(z++) = (uint8_t) g << 5 | (uint8_t) h;                         /* VVVRRRRR */
        }

        પસંદગી (l % 8) {
        સ્થિતિ 7:
                a = unbase32hexchar(x[0]);
                જો (a < 0)
                        પાછા -EINVAL;

                b = unbase32hexchar(x[1]);
                જો (b < 0)
                        પાછા -EINVAL;

                c = unbase32hexchar(x[2]);
                જો (c < 0)
                        પાછા -EINVAL;

                d = unbase32hexchar(x[3]);
                જો (d < 0)
                        પાછા -EINVAL;

                e = unbase32hexchar(x[4]);
                જો (e < 0)
                        પાછા -EINVAL;

                f = unbase32hexchar(x[5]);
                જો (f < 0)
                        પાછા -EINVAL;

                g = unbase32hexchar(x[6]);
                જો (g < 0)
                        પાછા -EINVAL;

                /* g == 000VV000 */
                જો (g & 7)
                        પાછા -EINVAL;

                *(z++) = (uint8_t) a << 3 | (uint8_t) b >> 2;                    /* XXXXXYYY */
                *(z++) = (uint8_t) b << 6 | (uint8_t) c << 1 | (uint8_t) d >> 4; /* YYZZZZZW */
                *(z++) = (uint8_t) d << 4 | (uint8_t) e >> 1;                    /* WWWWSSSS */
                *(z++) = (uint8_t) e << 7 | (uint8_t) f << 2 | (uint8_t) g >> 3; /* SQQQQQVV */

                તોડો;
        સ્થિતિ 5:
                a = unbase32hexchar(x[0]);
                જો (a < 0)
                        પાછા -EINVAL;

                b = unbase32hexchar(x[1]);
                જો (b < 0)
                        પાછા -EINVAL;

                c = unbase32hexchar(x[2]);
                જો (c < 0)
                        પાછા -EINVAL;

                d = unbase32hexchar(x[3]);
                જો (d < 0)
                        પાછા -EINVAL;

                e = unbase32hexchar(x[4]);
                જો (e < 0)
                        પાછા -EINVAL;

                /* e == 000SSSS0 */
                જો (e & 1)
                        પાછા -EINVAL;

                *(z++) = (uint8_t) a << 3 | (uint8_t) b >> 2;                    /* XXXXXYYY */
                *(z++) = (uint8_t) b << 6 | (uint8_t) c << 1 | (uint8_t) d >> 4; /* YYZZZZZW */
                *(z++) = (uint8_t) d << 4 | (uint8_t) e >> 1;                    /* WWWWSSSS */

                તોડો;
        સ્થિતિ 4:
                a = unbase32hexchar(x[0]);
                જો (a < 0)
                        પાછા -EINVAL;

                b = unbase32hexchar(x[1]);
                જો (b < 0)
                        પાછા -EINVAL;

                c = unbase32hexchar(x[2]);
                જો (c < 0)
                        પાછા -EINVAL;

                d = unbase32hexchar(x[3]);
                જો (d < 0)
                        પાછા -EINVAL;

                /* d == 000W0000 */
                જો (d & 15)
                        પાછા -EINVAL;

                *(z++) = (uint8_t) a << 3 | (uint8_t) b >> 2;                    /* XXXXXYYY */
                *(z++) = (uint8_t) b << 6 | (uint8_t) c << 1 | (uint8_t) d >> 4; /* YYZZZZZW */

                તોડો;
        સ્થિતિ 2:
                a = unbase32hexchar(x[0]);
                જો (a < 0)
                        પાછા -EINVAL;

                b = unbase32hexchar(x[1]);
                જો (b < 0)
                        પાછા -EINVAL;

                /* b == 000YYY00 */
                જો (b & 3)
                        પાછા -EINVAL;

                *(z++) = (uint8_t) a << 3 | (uint8_t) b >> 2; /* XXXXXYYY */

                તોડો;
        સ્થિતિ 0:
                તોડો;
        મૂળભૂત:
                પાછા -EINVAL;
        }

        *z = 0;

        *mem = TAKE_PTR(r);
        *_len = len;

        પાછા 0;
}

/* https://tools.ietf.org/html/rfc4648#section-4 */
અક્ષર base64char(પૂર્ણાંક x) {
        અચલ અક્ષર *table = UPPERCASE_LETTERS LOWERCASE_LETTERS DIGITS "+/";
        પાછા table[x & 63];
}

/* This is almost base64char(), but not entirely, as it uses the "url and filename safe" alphabet,
 * since we don't want "/" appear in interface names (since interfaces appear in sysfs as filenames).
 * See section #5 of RFC 4648. */
અક્ષર urlsafe_base64char(પૂર્ણાંક x) {
        અચલ અક્ષર *table = UPPERCASE_LETTERS LOWERCASE_LETTERS DIGITS "-_";
        પાછા table[x & 63];
}

પૂર્ણાંક unbase64char(અક્ષર c) {
        unsigned offset;

        જો (c >= 'A' && c <= 'Z')
                પાછા c - 'A';

        offset = 'Z' - 'A' + 1;

        જો (c >= 'a' && c <= 'z')
                પાછા c - 'a' + offset;

        offset += 'z' - 'a' + 1;

        જો (c >= '0' && c <= '9')
                પાછા c - '0' + offset;

        offset += '9' - '0' + 1;

        જો (IN_SET(c, '+', '-')) /* Support both the regular and the URL safe character set (see above) */
                પાછા offset;

        offset++;

        જો (IN_SET(c, '/', '_')) /* ditto */
                પાછા offset;

        પાછા -EINVAL;
}

સ્થિર શૂન્ય maybe_લીટી_break(અક્ષર **x, અક્ષર *start, size_t લીટી_break) {
        size_t n;

        પ્રતિજ્ઞા(x);
        પ્રતિજ્ઞા(*x);
        પ્રતિજ્ઞા(start);
        પ્રતિજ્ઞા(*x >= start);

        જો (લીટી_break == SIZE_MAX)
                પાછા;

        n = *x - start;

        જો (n % (લીટી_break + 1) == લીટી_break)
                *((*x)++) = '\n';
}

ssize_t base64mem_full(
                અચલ શૂન્ય *p,
                size_t l,
                size_t લીટી_break,
                અક્ષર **ret) {

        અચલ uint8_t *x;
        અક્ષર *b, *z;
        size_t m;

        પ્રતિજ્ઞા(p || l == 0);
        પ્રતિજ્ઞા(લીટી_break > 0);
        પ્રતિજ્ઞા(ret);

        /* three input bytes makes four output bytes, padding is added so we must round up */
        m = 4 * (l + 2) / 3 + 1;
        જો (લીટી_break != SIZE_MAX)
                m += m / લીટી_break;

        z = b = malloc(m);
        જો (!b)
                પાછા -ENOMEM;

        માટે (x = p; x && x < (અચલ uint8_t*) p + (l / 3) * 3; x += 3) {
                /* x[0] == XXXXXXXX; x[1] == YYYYYYYY; x[2] == ZZZZZZZZ */
                maybe_લીટી_break(&z, b, લીટી_break);
                *(z++) = base64char(x[0] >> 2);                    /* 00XXXXXX */
                maybe_લીટી_break(&z, b, લીટી_break);
                *(z++) = base64char((x[0] & 3) << 4 | x[1] >> 4);  /* 00XXYYYY */
                maybe_લીટી_break(&z, b, લીટી_break);
                *(z++) = base64char((x[1] & 15) << 2 | x[2] >> 6); /* 00YYYYZZ */
                maybe_લીટી_break(&z, b, લીટી_break);
                *(z++) = base64char(x[2] & 63);                    /* 00ZZZZZZ */
        }

        પસંદગી (l % 3) {
        સ્થિતિ 2:
                maybe_લીટી_break(&z, b, લીટી_break);
                *(z++) = base64char(x[0] >> 2);                   /* 00XXXXXX */
                maybe_લીટી_break(&z, b, લીટી_break);
                *(z++) = base64char((x[0] & 3) << 4 | x[1] >> 4); /* 00XXYYYY */
                maybe_લીટી_break(&z, b, લીટી_break);
                *(z++) = base64char((x[1] & 15) << 2);            /* 00YYYY00 */
                maybe_લીટી_break(&z, b, લીટી_break);
                *(z++) = '=';
                તોડો;

        સ્થિતિ 1:
                maybe_લીટી_break(&z, b, લીટી_break);
                *(z++) = base64char(x[0] >> 2);        /* 00XXXXXX */
                maybe_લીટી_break(&z, b, લીટી_break);
                *(z++) = base64char((x[0] & 3) << 4);  /* 00XX0000 */
                maybe_લીટી_break(&z, b, લીટી_break);
                *(z++) = '=';
                maybe_લીટી_break(&z, b, લીટી_break);
                *(z++) = '=';
                તોડો;
        }

        *z = 0;
        *ret = b;

        પ્રતિજ્ઞા(z >= b); /* Let static analyzers know that the answer is non-negative. */
        પાછા z - b;
}

સ્થિર ssize_t base64_append_width(
                અક્ષર **prefix,
                size_t plen,
                અક્ષર sep,
                size_t indent,
                અચલ શૂન્ય *p,
                size_t l,
                size_t width) {

        _cleanup_free_ અક્ષર *x = ખાલી;
        અક્ષર *t, *s;
        size_t લીટીઓ;
        ssize_t len;

        પ્રતિજ્ઞા(prefix);
        પ્રતિજ્ઞા(*prefix || plen == 0);
        પ્રતિજ્ઞા(p || l == 0);

        len = base64mem(p, l, &x);
        જો (len < 0)
                પાછા len;
        જો (len == 0)
                પાછા plen;

        લીટીઓ = DIV_ROUND_UP(len, width);

        જો (plen >= SSIZE_MAX - 1 - 1 ||
            લીટીઓ > (SSIZE_MAX - plen - 1 - 1) / (indent + width + 1))
                પાછા -ENOMEM;

        t = realloc(*prefix, plen + 1 + 1 + (indent + width + 1) * લીટીઓ);
        જો (!t)
                પાછા -ENOMEM;

        s = t + plen;
        માટે (size_t લીટી = 0; લીટી < લીટીઓ; લીટી++) {
                size_t act = MIN(width, (size_t) len);

                જો (લીટી > 0)
                        sep = '\n';

                જો (s > t) {
                        *s++ = sep;
                        જો (sep == '\n')
                                s = mempset(s, ' ', indent);
                }

                s = mempcpy(s, x + width * લીટી, act);
                len -= act;
        }
        પ્રતિજ્ઞા(len == 0);

        *s = '\0';
        *prefix = t;
        પાછા s - t;
}

ssize_t base64_append(
                અક્ષર **prefix,
                size_t plen,
                અચલ શૂન્ય *p,
                size_t l,
                size_t indent,
                size_t width) {

        જો (plen > width / 2 || plen + indent > width)
                /* leave indent on the left, keep last column free */
                પાછા base64_append_width(prefix, plen, '\n', indent, p, l, width - indent);
        નહીંતર
                /* leave plen on the left, keep last column free */
                પાછા base64_append_width(prefix, plen, ' ', plen + 1, p, l, width - plen - 1);
}

સ્થિર પૂર્ણાંક unbase64_next(અચલ અક્ષર **p, size_t *l) {
        પૂર્ણાંક ret;

        પ્રતિજ્ઞા(p);
        પ્રતિજ્ઞા(l);

        /* Find the next non-whitespace character, and decode it. If we find padding, we return it as INT_MAX. We
         * greedily skip all preceding and all following whitespace. */

        માટે (;;) {
                જો (*l == 0)
                        પાછા -EPIPE;

                જો (!strchr(WHITESPACE, **p))
                        તોડો;

                /* Skip leading whitespace */
                (*p)++, (*l)--;
        }

        જો (**p == '=')
                ret = INT_MAX; /* return padding as INT_MAX */
        નહીંતર {
                ret = unbase64char(**p);
                જો (ret < 0)
                        પાછા ret;
        }

        માટે (;;) {
                (*p)++, (*l)--;

                જો (*l == 0)
                        તોડો;
                જો (!strchr(WHITESPACE, **p))
                        તોડો;

                /* Skip following whitespace */
        }

        પાછા ret;
}

પૂર્ણાંક unbase64mem_full(
                અચલ અક્ષર *p,
                size_t l,
                bool secure,
                શૂન્ય **ret_data,
                size_t *ret_size) {

        _cleanup_free_ uint8_t *buf = ખાલી;
        અચલ અક્ષર *x;
        uint8_t *z;
        size_t len;

        પ્રતિજ્ઞા(p || l == 0);

        જો (l == SIZE_MAX)
                l = strlen(p);

        /* A group of four input bytes needs three output bytes, in case of padding we need to add two or three extra
         * bytes. Note that this calculation is an upper boundary, as we ignore whitespace while decoding */
        len = (l / 4) * 3 + (l % 4 != 0 ? (l % 4) - 1 : 0);

        buf = malloc(len + 1);
        જો (!buf)
                પાછા -ENOMEM;

        CLEANUP_ERASE_PTR(secure ? &buf : ખાલી, len);

        માટે (x = p, z = buf;;) {
                પૂર્ણાંક a, b, c, d; /* a == 00XXXXXX; b == 00YYYYYY; c == 00ZZZZZZ; d == 00WWWWWW */

                a = unbase64_next(&x, &l);
                જો (a == -EPIPE) /* End of string */
                        તોડો;
                જો (a < 0)
                        પાછા a;
                જો (a == INT_MAX) /* Padding is not allowed at the beginning of a 4ch block */
                        પાછા -EINVAL;

                b = unbase64_next(&x, &l);
                જો (b < 0)
                        પાછા b;
                જો (b == INT_MAX) /* Padding is not allowed at the second character of a 4ch block either */
                        પાછા -EINVAL;

                c = unbase64_next(&x, &l);
                જો (c < 0)
                        પાછા c;

                d = unbase64_next(&x, &l);
                જો (d < 0)
                        પાછા d;

                જો (c == INT_MAX) { /* Padding at the third character */

                        જો (d != INT_MAX) /* If the third character is padding, the fourth must be too */
                                પાછા -EINVAL;

                        /* b == 00YY0000 */
                        જો (b & 15)
                                પાછા -EINVAL;

                        જો (l > 0) /* Trailing rubbish? */
                                પાછા -ENAMETOOLONG;

                        *(z++) = (uint8_t) a << 2 | (uint8_t) (b >> 4); /* XXXXXXYY */
                        તોડો;
                }

                જો (d == INT_MAX) {
                        /* c == 00ZZZZ00 */
                        જો (c & 3)
                                પાછા -EINVAL;

                        જો (l > 0) /* Trailing rubbish? */
                                પાછા -ENAMETOOLONG;

                        *(z++) = (uint8_t) a << 2 | (uint8_t) b >> 4; /* XXXXXXYY */
                        *(z++) = (uint8_t) b << 4 | (uint8_t) c >> 2; /* YYYYZZZZ */
                        તોડો;
                }

                *(z++) = (uint8_t) a << 2 | (uint8_t) b >> 4; /* XXXXXXYY */
                *(z++) = (uint8_t) b << 4 | (uint8_t) c >> 2; /* YYYYZZZZ */
                *(z++) = (uint8_t) c << 6 | (uint8_t) d;      /* ZZWWWWWW */
        }

        *z = 0;

        પ્રતિજ્ઞા((size_t) (z - buf) <= len);

        જો (ret_size)
                *ret_size = (size_t) (z - buf);
        જો (ret_data)
                *ret_data = TAKE_PTR(buf);

        પાછા 0;
}

શૂન્ય hexdump(FILE *f, અચલ શૂન્ય *p, size_t s) {
        અચલ uint8_t *b = p;
        unsigned n = 0;

        પ્રતિજ્ઞા(b || s == 0);

        જો (s == SIZE_MAX)
                s = strlen(p);

        જો (!f)
                f = stdout;

        જ્યારે (s > 0) {
                size_t i;

                fprintf(f, "%04x  ", n);

                માટે (i = 0; i < 16; i++) {

                        જો (i >= s)
                                fputs("   ", f);
                        નહીંતર
                                fprintf(f, "%02x ", b[i]);

                        જો (i == 7)
                                fputc(' ', f);
                }

                fputc(' ', f);

                માટે (i = 0; i < 16; i++) {

                        જો (i >= s)
                                fputc(' ', f);
                        નહીંતર
                                fputc(isprint(b[i]) ? (અક્ષર) b[i] : '.', f);
                }

                fputc('\n', f);

                જો (s < 16)
                        તોડો;

                n += 16;
                b += 16;
                s -= 16;
        }
}
