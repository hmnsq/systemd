/* SPDX-License-Identifier: LGPL-2.1-or-later */

#அந்தர்பாவ <ctype.h>
#அந்தர்பாவ <stdio.h>

#அந்தர்பாவ "alloc-util.h"
#அந்தர்பாவ "hexdecoct.h"
#அந்தர்பாவ "memory-util.h"
#அந்தர்பாவ "string-util.h"

எழுத்து octchar(முழு x) {
        திரும்பு '0' + (x & 7);
}

முழு unoctchar(எழுத்து c) {

        எனில் (c >= '0' && c <= '7')
                திரும்பு c - '0';

        திரும்பு -EINVAL;
}

எழுத்து decchar(முழு x) {
        திரும்பு '0' + (x % 10);
}

முழு undecchar(எழுத்து c) {

        எனில் (c >= '0' && c <= '9')
                திரும்பு c - '0';

        திரும்பு -EINVAL;
}

எழுத்து hexchar(முழு x) {
        மாறிலி எழுத்து *table = LOWERCASE_HEXDIGITS;
        திரும்பு table[x & 15];
}

முழு unhexchar(எழுத்து c) {

        எனில் (c >= '0' && c <= '9')
                திரும்பு c - '0';

        எனில் (c >= 'a' && c <= 'f')
                திரும்பு c - 'a' + 10;

        எனில் (c >= 'A' && c <= 'F')
                திரும்பு c - 'A' + 10;

        திரும்பு -EINVAL;
}

எழுத்து* hexmem(மாறிலி வெறுமை *p, size_t l) {
        மாறிலி uint8_t *x;
        எழுத்து *r, *z;

        ப்ரதிஜ்ஞா(p || l == 0);

        z = r = new(எழுத்து, l * 2 + 1);
        எனில் (!r)
                திரும்பு வெற்று;

        க்கு (x = p; x && x < (மாறிலி uint8_t*) p + l; x++) {
                *(z++) = hexchar(*x >> 4);
                *(z++) = hexchar(*x & 15);
        }

        *z = 0;
        திரும்பு r;
}

நிலையான முழு unhex_next(மாறிலி எழுத்து **p, size_t *l) {
        முழு r;

        ப்ரதிஜ்ஞா(p);
        ப்ரதிஜ்ஞா(l);

        /* Find the next non-whitespace character, and decode it. We
         * greedily skip all preceding and all following whitespace. */

        க்கு (;;) {
                எனில் (*l == 0)
                        திரும்பு -EPIPE;

                எனில் (!strchr(WHITESPACE, **p))
                        நிறுத்து;

                /* Skip leading whitespace */
                (*p)++, (*l)--;
        }

        r = unhexchar(**p);
        எனில் (r < 0)
                திரும்பு r;

        க்கு (;;) {
                (*p)++, (*l)--;

                எனில் (*l == 0 || !strchr(WHITESPACE, **p))
                        நிறுத்து;

                /* Skip following whitespace */
        }

        திரும்பு r;
}

முழு unhexmem_full(
                மாறிலி எழுத்து *p,
                size_t l,
                bool secure,
                வெறுமை **ret_data,
                size_t *ret_size) {

        _cleanup_free_ uint8_t *buf = வெற்று;
        size_t buf_size;
        மாறிலி எழுத்து *x;
        uint8_t *z;

        ப்ரதிஜ்ஞா(p || l == 0);

        எனில் (l == SIZE_MAX)
                l = strlen(p);

        /* Note that the calculation of memory size is an upper boundary, as we ignore whitespace while decoding */
        buf_size = (l + 1) / 2 + 1;
        buf = malloc(buf_size);
        எனில் (!buf)
                திரும்பு -ENOMEM;

        CLEANUP_ERASE_PTR(secure ? &buf : வெற்று, buf_size);

        க்கு (x = p, z = buf;;) {
                முழு a, b;

                a = unhex_next(&x, &l);
                எனில் (a == -EPIPE) /* End of string */
                        நிறுத்து;
                எனில் (a < 0)
                        திரும்பு a;

                b = unhex_next(&x, &l);
                எனில் (b < 0)
                        திரும்பு b;

                *(z++) = (uint8_t) a << 4 | (uint8_t) b;
        }

        *z = 0;

        எனில் (ret_size)
                *ret_size = (size_t) (z - buf);
        எனில் (ret_data)
                *ret_data = TAKE_PTR(buf);

        திரும்பு 0;
}

/* https://tools.ietf.org/html/rfc4648#section-6
 * Notice that base32hex differs from base32 in the alphabet it uses.
 * The distinction is that the base32hex representation preserves the
 * order of the underlying data when compared as bytestrings, this is
 * useful when representing NSEC3 hashes, as one can then verify the
 * order of hashes directly from their representation. */
எழுத்து base32hexchar(முழு x) {
        மாறிலி எழுத்து *table = DIGITS "ABCDEFGHIJKLMNOPQRSTUV";
        திரும்பு table[x & 31];
}

முழு unbase32hexchar(எழுத்து c) {
        unsigned offset;

        எனில் (c >= '0' && c <= '9')
                திரும்பு c - '0';

        offset = '9' - '0' + 1;

        எனில் (c >= 'A' && c <= 'V')
                திரும்பு c - 'A' + offset;

        திரும்பு -EINVAL;
}

எழுத்து* base32hexmem(மாறிலி வெறுமை *p, size_t l, bool padding) {
        எழுத்து *r, *z;
        மாறிலி uint8_t *x;
        size_t len;

        ப்ரதிஜ்ஞா(p || l == 0);

        எனில் (padding)
                /* five input bytes makes eight output bytes, padding is added so we must round up */
                len = 8 * (l + 4) / 5;
        இல்லையெனில் {
                /* same, but round down as there is no padding */
                len = 8 * l / 5;

                தேர்வு (l % 5) {
                நிலை 4:
                        len += 7;
                        நிறுத்து;
                நிலை 3:
                        len += 5;
                        நிறுத்து;
                நிலை 2:
                        len += 4;
                        நிறுத்து;
                நிலை 1:
                        len += 2;
                        நிறுத்து;
                }
        }

        z = r = malloc(len + 1);
        எனில் (!r)
                திரும்பு வெற்று;

        க்கு (x = p; x < (மாறிலி uint8_t*) p + (l / 5) * 5; x += 5) {
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

        தேர்வு (l % 5) {
        நிலை 4:
                *(z++) = base32hexchar(x[0] >> 3);                    /* 000XXXXX */
                *(z++) = base32hexchar((x[0] & 7) << 2 | x[1] >> 6);  /* 000XXXYY */
                *(z++) = base32hexchar((x[1] & 63) >> 1);             /* 000YYYYY */
                *(z++) = base32hexchar((x[1] & 1) << 4 | x[2] >> 4);   /* 000YZZZZ */
                *(z++) = base32hexchar((x[2] & 15) << 1 | x[3] >> 7); /* 000ZZZZQ */
                *(z++) = base32hexchar((x[3] & 127) >> 2);            /* 000QQQQQ */
                *(z++) = base32hexchar((x[3] & 3) << 3);              /* 000QQ000 */
                எனில் (padding)
                        *(z++) = '=';

                நிறுத்து;

        நிலை 3:
                *(z++) = base32hexchar(x[0] >> 3);                   /* 000XXXXX */
                *(z++) = base32hexchar((x[0] & 7) << 2 | x[1] >> 6); /* 000XXXYY */
                *(z++) = base32hexchar((x[1] & 63) >> 1);            /* 000YYYYY */
                *(z++) = base32hexchar((x[1] & 1) << 4 | x[2] >> 4); /* 000YZZZZ */
                *(z++) = base32hexchar((x[2] & 15) << 1);            /* 000ZZZZ0 */
                எனில் (padding) {
                        *(z++) = '=';
                        *(z++) = '=';
                        *(z++) = '=';
                }

                நிறுத்து;

        நிலை 2:
                *(z++) = base32hexchar(x[0] >> 3);                   /* 000XXXXX */
                *(z++) = base32hexchar((x[0] & 7) << 2 | x[1] >> 6); /* 000XXXYY */
                *(z++) = base32hexchar((x[1] & 63) >> 1);            /* 000YYYYY */
                *(z++) = base32hexchar((x[1] & 1) << 4);             /* 000Y0000 */
                எனில் (padding) {
                        *(z++) = '=';
                        *(z++) = '=';
                        *(z++) = '=';
                        *(z++) = '=';
                }

                நிறுத்து;

        நிலை 1:
                *(z++) = base32hexchar(x[0] >> 3);       /* 000XXXXX */
                *(z++) = base32hexchar((x[0] & 7) << 2); /* 000XXX00 */
                எனில் (padding) {
                        *(z++) = '=';
                        *(z++) = '=';
                        *(z++) = '=';
                        *(z++) = '=';
                        *(z++) = '=';
                        *(z++) = '=';
                }

                நிறுத்து;
        }

        *z = 0;
        திரும்பு r;
}

முழு unbase32hexmem(மாறிலி எழுத்து *p, size_t l, bool padding, வெறுமை **mem, size_t *_len) {
        _cleanup_free_ uint8_t *r = வெற்று;
        முழு a, b, c, d, e, f, g, h;
        uint8_t *z;
        மாறிலி எழுத்து *x;
        size_t len;
        unsigned pad = 0;

        ப்ரதிஜ்ஞா(p || l == 0);
        ப்ரதிஜ்ஞா(mem);
        ப்ரதிஜ்ஞா(_len);

        எனில் (l == SIZE_MAX)
                l = strlen(p);

        /* padding ensures any base32hex input has input divisible by 8 */
        எனில் (padding && l % 8 != 0)
                திரும்பு -EINVAL;

        எனில் (padding) {
                /* strip the padding */
                போது (l > 0 && p[l - 1] == '=' && pad < 7) {
                        pad++;
                        l--;
                }
        }

        /* a group of eight input bytes needs five output bytes, in case of
         * padding we need to add some extra bytes */
        len = (l / 8) * 5;

        தேர்வு (l % 8) {
        நிலை 7:
                len += 4;
                நிறுத்து;
        நிலை 5:
                len += 3;
                நிறுத்து;
        நிலை 4:
                len += 2;
                நிறுத்து;
        நிலை 2:
                len += 1;
                நிறுத்து;
        நிலை 0:
                நிறுத்து;
        இயல்பு:
                திரும்பு -EINVAL;
        }

        z = r = malloc(len + 1);
        எனில் (!r)
                திரும்பு -ENOMEM;

        க்கு (x = p; x < p + (l / 8) * 8; x += 8) {
                /* a == 000XXXXX; b == 000YYYYY; c == 000ZZZZZ; d == 000WWWWW
                 * e == 000SSSSS; f == 000QQQQQ; g == 000VVVVV; h == 000RRRRR */
                a = unbase32hexchar(x[0]);
                எனில் (a < 0)
                        திரும்பு -EINVAL;

                b = unbase32hexchar(x[1]);
                எனில் (b < 0)
                        திரும்பு -EINVAL;

                c = unbase32hexchar(x[2]);
                எனில் (c < 0)
                        திரும்பு -EINVAL;

                d = unbase32hexchar(x[3]);
                எனில் (d < 0)
                        திரும்பு -EINVAL;

                e = unbase32hexchar(x[4]);
                எனில் (e < 0)
                        திரும்பு -EINVAL;

                f = unbase32hexchar(x[5]);
                எனில் (f < 0)
                        திரும்பு -EINVAL;

                g = unbase32hexchar(x[6]);
                எனில் (g < 0)
                        திரும்பு -EINVAL;

                h = unbase32hexchar(x[7]);
                எனில் (h < 0)
                        திரும்பு -EINVAL;

                *(z++) = (uint8_t) a << 3 | (uint8_t) b >> 2;                    /* XXXXXYYY */
                *(z++) = (uint8_t) b << 6 | (uint8_t) c << 1 | (uint8_t) d >> 4; /* YYZZZZZW */
                *(z++) = (uint8_t) d << 4 | (uint8_t) e >> 1;                    /* WWWWSSSS */
                *(z++) = (uint8_t) e << 7 | (uint8_t) f << 2 | (uint8_t) g >> 3; /* SQQQQQVV */
                *(z++) = (uint8_t) g << 5 | (uint8_t) h;                         /* VVVRRRRR */
        }

        தேர்வு (l % 8) {
        நிலை 7:
                a = unbase32hexchar(x[0]);
                எனில் (a < 0)
                        திரும்பு -EINVAL;

                b = unbase32hexchar(x[1]);
                எனில் (b < 0)
                        திரும்பு -EINVAL;

                c = unbase32hexchar(x[2]);
                எனில் (c < 0)
                        திரும்பு -EINVAL;

                d = unbase32hexchar(x[3]);
                எனில் (d < 0)
                        திரும்பு -EINVAL;

                e = unbase32hexchar(x[4]);
                எனில் (e < 0)
                        திரும்பு -EINVAL;

                f = unbase32hexchar(x[5]);
                எனில் (f < 0)
                        திரும்பு -EINVAL;

                g = unbase32hexchar(x[6]);
                எனில் (g < 0)
                        திரும்பு -EINVAL;

                /* g == 000VV000 */
                எனில் (g & 7)
                        திரும்பு -EINVAL;

                *(z++) = (uint8_t) a << 3 | (uint8_t) b >> 2;                    /* XXXXXYYY */
                *(z++) = (uint8_t) b << 6 | (uint8_t) c << 1 | (uint8_t) d >> 4; /* YYZZZZZW */
                *(z++) = (uint8_t) d << 4 | (uint8_t) e >> 1;                    /* WWWWSSSS */
                *(z++) = (uint8_t) e << 7 | (uint8_t) f << 2 | (uint8_t) g >> 3; /* SQQQQQVV */

                நிறுத்து;
        நிலை 5:
                a = unbase32hexchar(x[0]);
                எனில் (a < 0)
                        திரும்பு -EINVAL;

                b = unbase32hexchar(x[1]);
                எனில் (b < 0)
                        திரும்பு -EINVAL;

                c = unbase32hexchar(x[2]);
                எனில் (c < 0)
                        திரும்பு -EINVAL;

                d = unbase32hexchar(x[3]);
                எனில் (d < 0)
                        திரும்பு -EINVAL;

                e = unbase32hexchar(x[4]);
                எனில் (e < 0)
                        திரும்பு -EINVAL;

                /* e == 000SSSS0 */
                எனில் (e & 1)
                        திரும்பு -EINVAL;

                *(z++) = (uint8_t) a << 3 | (uint8_t) b >> 2;                    /* XXXXXYYY */
                *(z++) = (uint8_t) b << 6 | (uint8_t) c << 1 | (uint8_t) d >> 4; /* YYZZZZZW */
                *(z++) = (uint8_t) d << 4 | (uint8_t) e >> 1;                    /* WWWWSSSS */

                நிறுத்து;
        நிலை 4:
                a = unbase32hexchar(x[0]);
                எனில் (a < 0)
                        திரும்பு -EINVAL;

                b = unbase32hexchar(x[1]);
                எனில் (b < 0)
                        திரும்பு -EINVAL;

                c = unbase32hexchar(x[2]);
                எனில் (c < 0)
                        திரும்பு -EINVAL;

                d = unbase32hexchar(x[3]);
                எனில் (d < 0)
                        திரும்பு -EINVAL;

                /* d == 000W0000 */
                எனில் (d & 15)
                        திரும்பு -EINVAL;

                *(z++) = (uint8_t) a << 3 | (uint8_t) b >> 2;                    /* XXXXXYYY */
                *(z++) = (uint8_t) b << 6 | (uint8_t) c << 1 | (uint8_t) d >> 4; /* YYZZZZZW */

                நிறுத்து;
        நிலை 2:
                a = unbase32hexchar(x[0]);
                எனில் (a < 0)
                        திரும்பு -EINVAL;

                b = unbase32hexchar(x[1]);
                எனில் (b < 0)
                        திரும்பு -EINVAL;

                /* b == 000YYY00 */
                எனில் (b & 3)
                        திரும்பு -EINVAL;

                *(z++) = (uint8_t) a << 3 | (uint8_t) b >> 2; /* XXXXXYYY */

                நிறுத்து;
        நிலை 0:
                நிறுத்து;
        இயல்பு:
                திரும்பு -EINVAL;
        }

        *z = 0;

        *mem = TAKE_PTR(r);
        *_len = len;

        திரும்பு 0;
}

/* https://tools.ietf.org/html/rfc4648#section-4 */
எழுத்து base64char(முழு x) {
        மாறிலி எழுத்து *table = UPPERCASE_LETTERS LOWERCASE_LETTERS DIGITS "+/";
        திரும்பு table[x & 63];
}

/* This is almost base64char(), but not entirely, as it uses the "url and filename safe" alphabet,
 * since we don't want "/" appear in interface names (since interfaces appear in sysfs as filenames).
 * See section #5 of RFC 4648. */
எழுத்து urlsafe_base64char(முழு x) {
        மாறிலி எழுத்து *table = UPPERCASE_LETTERS LOWERCASE_LETTERS DIGITS "-_";
        திரும்பு table[x & 63];
}

முழு unbase64char(எழுத்து c) {
        unsigned offset;

        எனில் (c >= 'A' && c <= 'Z')
                திரும்பு c - 'A';

        offset = 'Z' - 'A' + 1;

        எனில் (c >= 'a' && c <= 'z')
                திரும்பு c - 'a' + offset;

        offset += 'z' - 'a' + 1;

        எனில் (c >= '0' && c <= '9')
                திரும்பு c - '0' + offset;

        offset += '9' - '0' + 1;

        எனில் (IN_SET(c, '+', '-')) /* Support both the regular and the URL safe character set (see above) */
                திரும்பு offset;

        offset++;

        எனில் (IN_SET(c, '/', '_')) /* ditto */
                திரும்பு offset;

        திரும்பு -EINVAL;
}

நிலையான வெறுமை maybe_வரி_break(எழுத்து **x, எழுத்து *start, size_t வரி_break) {
        size_t n;

        ப்ரதிஜ்ஞா(x);
        ப்ரதிஜ்ஞா(*x);
        ப்ரதிஜ்ஞா(start);
        ப்ரதிஜ்ஞா(*x >= start);

        எனில் (வரி_break == SIZE_MAX)
                திரும்பு;

        n = *x - start;

        எனில் (n % (வரி_break + 1) == வரி_break)
                *((*x)++) = '\n';
}

ssize_t base64mem_full(
                மாறிலி வெறுமை *p,
                size_t l,
                size_t வரி_break,
                எழுத்து **ret) {

        மாறிலி uint8_t *x;
        எழுத்து *b, *z;
        size_t m;

        ப்ரதிஜ்ஞா(p || l == 0);
        ப்ரதிஜ்ஞா(வரி_break > 0);
        ப்ரதிஜ்ஞா(ret);

        /* three input bytes makes four output bytes, padding is added so we must round up */
        m = 4 * (l + 2) / 3 + 1;
        எனில் (வரி_break != SIZE_MAX)
                m += m / வரி_break;

        z = b = malloc(m);
        எனில் (!b)
                திரும்பு -ENOMEM;

        க்கு (x = p; x && x < (மாறிலி uint8_t*) p + (l / 3) * 3; x += 3) {
                /* x[0] == XXXXXXXX; x[1] == YYYYYYYY; x[2] == ZZZZZZZZ */
                maybe_வரி_break(&z, b, வரி_break);
                *(z++) = base64char(x[0] >> 2);                    /* 00XXXXXX */
                maybe_வரி_break(&z, b, வரி_break);
                *(z++) = base64char((x[0] & 3) << 4 | x[1] >> 4);  /* 00XXYYYY */
                maybe_வரி_break(&z, b, வரி_break);
                *(z++) = base64char((x[1] & 15) << 2 | x[2] >> 6); /* 00YYYYZZ */
                maybe_வரி_break(&z, b, வரி_break);
                *(z++) = base64char(x[2] & 63);                    /* 00ZZZZZZ */
        }

        தேர்வு (l % 3) {
        நிலை 2:
                maybe_வரி_break(&z, b, வரி_break);
                *(z++) = base64char(x[0] >> 2);                   /* 00XXXXXX */
                maybe_வரி_break(&z, b, வரி_break);
                *(z++) = base64char((x[0] & 3) << 4 | x[1] >> 4); /* 00XXYYYY */
                maybe_வரி_break(&z, b, வரி_break);
                *(z++) = base64char((x[1] & 15) << 2);            /* 00YYYY00 */
                maybe_வரி_break(&z, b, வரி_break);
                *(z++) = '=';
                நிறுத்து;

        நிலை 1:
                maybe_வரி_break(&z, b, வரி_break);
                *(z++) = base64char(x[0] >> 2);        /* 00XXXXXX */
                maybe_வரி_break(&z, b, வரி_break);
                *(z++) = base64char((x[0] & 3) << 4);  /* 00XX0000 */
                maybe_வரி_break(&z, b, வரி_break);
                *(z++) = '=';
                maybe_வரி_break(&z, b, வரி_break);
                *(z++) = '=';
                நிறுத்து;
        }

        *z = 0;
        *ret = b;

        ப்ரதிஜ்ஞா(z >= b); /* Let static analyzers know that the answer is non-negative. */
        திரும்பு z - b;
}

நிலையான ssize_t base64_append_width(
                எழுத்து **prefix,
                size_t plen,
                எழுத்து sep,
                size_t indent,
                மாறிலி வெறுமை *p,
                size_t l,
                size_t width) {

        _cleanup_free_ எழுத்து *x = வெற்று;
        எழுத்து *t, *s;
        size_t வரிகள்;
        ssize_t len;

        ப்ரதிஜ்ஞா(prefix);
        ப்ரதிஜ்ஞா(*prefix || plen == 0);
        ப்ரதிஜ்ஞா(p || l == 0);

        len = base64mem(p, l, &x);
        எனில் (len < 0)
                திரும்பு len;
        எனில் (len == 0)
                திரும்பு plen;

        வரிகள் = DIV_ROUND_UP(len, width);

        எனில் (plen >= SSIZE_MAX - 1 - 1 ||
            வரிகள் > (SSIZE_MAX - plen - 1 - 1) / (indent + width + 1))
                திரும்பு -ENOMEM;

        t = realloc(*prefix, plen + 1 + 1 + (indent + width + 1) * வரிகள்);
        எனில் (!t)
                திரும்பு -ENOMEM;

        s = t + plen;
        க்கு (size_t வரி = 0; வரி < வரிகள்; வரி++) {
                size_t act = MIN(width, (size_t) len);

                எனில் (வரி > 0)
                        sep = '\n';

                எனில் (s > t) {
                        *s++ = sep;
                        எனில் (sep == '\n')
                                s = mempset(s, ' ', indent);
                }

                s = mempcpy(s, x + width * வரி, act);
                len -= act;
        }
        ப்ரதிஜ்ஞா(len == 0);

        *s = '\0';
        *prefix = t;
        திரும்பு s - t;
}

ssize_t base64_append(
                எழுத்து **prefix,
                size_t plen,
                மாறிலி வெறுமை *p,
                size_t l,
                size_t indent,
                size_t width) {

        எனில் (plen > width / 2 || plen + indent > width)
                /* leave indent on the left, keep last column free */
                திரும்பு base64_append_width(prefix, plen, '\n', indent, p, l, width - indent);
        இல்லையெனில்
                /* leave plen on the left, keep last column free */
                திரும்பு base64_append_width(prefix, plen, ' ', plen + 1, p, l, width - plen - 1);
}

நிலையான முழு unbase64_next(மாறிலி எழுத்து **p, size_t *l) {
        முழு ret;

        ப்ரதிஜ்ஞா(p);
        ப்ரதிஜ்ஞா(l);

        /* Find the next non-whitespace character, and decode it. If we find padding, we return it as INT_MAX. We
         * greedily skip all preceding and all following whitespace. */

        க்கு (;;) {
                எனில் (*l == 0)
                        திரும்பு -EPIPE;

                எனில் (!strchr(WHITESPACE, **p))
                        நிறுத்து;

                /* Skip leading whitespace */
                (*p)++, (*l)--;
        }

        எனில் (**p == '=')
                ret = INT_MAX; /* return padding as INT_MAX */
        இல்லையெனில் {
                ret = unbase64char(**p);
                எனில் (ret < 0)
                        திரும்பு ret;
        }

        க்கு (;;) {
                (*p)++, (*l)--;

                எனில் (*l == 0)
                        நிறுத்து;
                எனில் (!strchr(WHITESPACE, **p))
                        நிறுத்து;

                /* Skip following whitespace */
        }

        திரும்பு ret;
}

முழு unbase64mem_full(
                மாறிலி எழுத்து *p,
                size_t l,
                bool secure,
                வெறுமை **ret_data,
                size_t *ret_size) {

        _cleanup_free_ uint8_t *buf = வெற்று;
        மாறிலி எழுத்து *x;
        uint8_t *z;
        size_t len;

        ப்ரதிஜ்ஞா(p || l == 0);

        எனில் (l == SIZE_MAX)
                l = strlen(p);

        /* A group of four input bytes needs three output bytes, in case of padding we need to add two or three extra
         * bytes. Note that this calculation is an upper boundary, as we ignore whitespace while decoding */
        len = (l / 4) * 3 + (l % 4 != 0 ? (l % 4) - 1 : 0);

        buf = malloc(len + 1);
        எனில் (!buf)
                திரும்பு -ENOMEM;

        CLEANUP_ERASE_PTR(secure ? &buf : வெற்று, len);

        க்கு (x = p, z = buf;;) {
                முழு a, b, c, d; /* a == 00XXXXXX; b == 00YYYYYY; c == 00ZZZZZZ; d == 00WWWWWW */

                a = unbase64_next(&x, &l);
                எனில் (a == -EPIPE) /* End of string */
                        நிறுத்து;
                எனில் (a < 0)
                        திரும்பு a;
                எனில் (a == INT_MAX) /* Padding is not allowed at the beginning of a 4ch block */
                        திரும்பு -EINVAL;

                b = unbase64_next(&x, &l);
                எனில் (b < 0)
                        திரும்பு b;
                எனில் (b == INT_MAX) /* Padding is not allowed at the second character of a 4ch block either */
                        திரும்பு -EINVAL;

                c = unbase64_next(&x, &l);
                எனில் (c < 0)
                        திரும்பு c;

                d = unbase64_next(&x, &l);
                எனில் (d < 0)
                        திரும்பு d;

                எனில் (c == INT_MAX) { /* Padding at the third character */

                        எனில் (d != INT_MAX) /* If the third character is padding, the fourth must be too */
                                திரும்பு -EINVAL;

                        /* b == 00YY0000 */
                        எனில் (b & 15)
                                திரும்பு -EINVAL;

                        எனில் (l > 0) /* Trailing rubbish? */
                                திரும்பு -ENAMETOOLONG;

                        *(z++) = (uint8_t) a << 2 | (uint8_t) (b >> 4); /* XXXXXXYY */
                        நிறுத்து;
                }

                எனில் (d == INT_MAX) {
                        /* c == 00ZZZZ00 */
                        எனில் (c & 3)
                                திரும்பு -EINVAL;

                        எனில் (l > 0) /* Trailing rubbish? */
                                திரும்பு -ENAMETOOLONG;

                        *(z++) = (uint8_t) a << 2 | (uint8_t) b >> 4; /* XXXXXXYY */
                        *(z++) = (uint8_t) b << 4 | (uint8_t) c >> 2; /* YYYYZZZZ */
                        நிறுத்து;
                }

                *(z++) = (uint8_t) a << 2 | (uint8_t) b >> 4; /* XXXXXXYY */
                *(z++) = (uint8_t) b << 4 | (uint8_t) c >> 2; /* YYYYZZZZ */
                *(z++) = (uint8_t) c << 6 | (uint8_t) d;      /* ZZWWWWWW */
        }

        *z = 0;

        ப்ரதிஜ்ஞா((size_t) (z - buf) <= len);

        எனில் (ret_size)
                *ret_size = (size_t) (z - buf);
        எனில் (ret_data)
                *ret_data = TAKE_PTR(buf);

        திரும்பு 0;
}

வெறுமை hexdump(FILE *f, மாறிலி வெறுமை *p, size_t s) {
        மாறிலி uint8_t *b = p;
        unsigned n = 0;

        ப்ரதிஜ்ஞா(b || s == 0);

        எனில் (s == SIZE_MAX)
                s = strlen(p);

        எனில் (!f)
                f = stdout;

        போது (s > 0) {
                size_t i;

                fprintf(f, "%04x  ", n);

                க்கு (i = 0; i < 16; i++) {

                        எனில் (i >= s)
                                fputs("   ", f);
                        இல்லையெனில்
                                fprintf(f, "%02x ", b[i]);

                        எனில் (i == 7)
                                fputc(' ', f);
                }

                fputc(' ', f);

                க்கு (i = 0; i < 16; i++) {

                        எனில் (i >= s)
                                fputc(' ', f);
                        இல்லையெனில்
                                fputc(isprint(b[i]) ? (எழுத்து) b[i] : '.', f);
                }

                fputc('\n', f);

                எனில் (s < 16)
                        நிறுத்து;

                n += 16;
                b += 16;
                s -= 16;
        }
}
