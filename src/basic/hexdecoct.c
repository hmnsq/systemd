/* SPDX-License-Identifier: LGPL-2.1-or-later */

#അന്തര്ഭാവ <ctype.h>
#അന്തര്ഭാവ <stdio.h>

#അന്തര്ഭാവ "alloc-util.h"
#അന്തര്ഭാവ "hexdecoct.h"
#അന്തര്ഭാവ "memory-util.h"
#അന്തര്ഭാവ "string-util.h"

അക്ഷരം octchar(പൂർണ്ണസംഖ്യ x) {
        തിരികെ '0' + (x & 7);
}

പൂർണ്ണസംഖ്യ unoctchar(അക്ഷരം c) {

        എങ്കിൽ (c >= '0' && c <= '7')
                തിരികെ c - '0';

        തിരികെ -EINVAL;
}

അക്ഷരം decchar(പൂർണ്ണസംഖ്യ x) {
        തിരികെ '0' + (x % 10);
}

പൂർണ്ണസംഖ്യ undecchar(അക്ഷരം c) {

        എങ്കിൽ (c >= '0' && c <= '9')
                തിരികെ c - '0';

        തിരികെ -EINVAL;
}

അക്ഷരം hexchar(പൂർണ്ണസംഖ്യ x) {
        സ്ഥിരം അക്ഷരം *table = LOWERCASE_HEXDIGITS;
        തിരികെ table[x & 15];
}

പൂർണ്ണസംഖ്യ unhexchar(അക്ഷരം c) {

        എങ്കിൽ (c >= '0' && c <= '9')
                തിരികെ c - '0';

        എങ്കിൽ (c >= 'a' && c <= 'f')
                തിരികെ c - 'a' + 10;

        എങ്കിൽ (c >= 'A' && c <= 'F')
                തിരികെ c - 'A' + 10;

        തിരികെ -EINVAL;
}

അക്ഷരം* hexmem(സ്ഥിരം ശൂന്യം *p, size_t l) {
        സ്ഥിരം uint8_t *x;
        അക്ഷരം *r, *z;

        പ്രതിജ്ഞാ(p || l == 0);

        z = r = new(അക്ഷരം, l * 2 + 1);
        എങ്കിൽ (!r)
                തിരികെ ശൂന്യ;

        വേണ്ടി (x = p; x && x < (സ്ഥിരം uint8_t*) p + l; x++) {
                *(z++) = hexchar(*x >> 4);
                *(z++) = hexchar(*x & 15);
        }

        *z = 0;
        തിരികെ r;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ unhex_next(സ്ഥിരം അക്ഷരം **p, size_t *l) {
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(p);
        പ്രതിജ്ഞാ(l);

        /* Find the next non-whitespace character, and decode it. We
         * greedily skip all preceding and all following whitespace. */

        വേണ്ടി (;;) {
                എങ്കിൽ (*l == 0)
                        തിരികെ -EPIPE;

                എങ്കിൽ (!strchr(WHITESPACE, **p))
                        നിർത്തുക;

                /* Skip leading whitespace */
                (*p)++, (*l)--;
        }

        r = unhexchar(**p);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        വേണ്ടി (;;) {
                (*p)++, (*l)--;

                എങ്കിൽ (*l == 0 || !strchr(WHITESPACE, **p))
                        നിർത്തുക;

                /* Skip following whitespace */
        }

        തിരികെ r;
}

പൂർണ്ണസംഖ്യ unhexmem_full(
                സ്ഥിരം അക്ഷരം *p,
                size_t l,
                bool secure,
                ശൂന്യം **ret_data,
                size_t *ret_size) {

        _cleanup_free_ uint8_t *buf = ശൂന്യ;
        size_t buf_size;
        സ്ഥിരം അക്ഷരം *x;
        uint8_t *z;

        പ്രതിജ്ഞാ(p || l == 0);

        എങ്കിൽ (l == SIZE_MAX)
                l = strlen(p);

        /* Note that the calculation of memory size is an upper boundary, as we ignore whitespace while decoding */
        buf_size = (l + 1) / 2 + 1;
        buf = malloc(buf_size);
        എങ്കിൽ (!buf)
                തിരികെ -ENOMEM;

        CLEANUP_ERASE_PTR(secure ? &buf : ശൂന്യ, buf_size);

        വേണ്ടി (x = p, z = buf;;) {
                പൂർണ്ണസംഖ്യ a, b;

                a = unhex_next(&x, &l);
                എങ്കിൽ (a == -EPIPE) /* End of string */
                        നിർത്തുക;
                എങ്കിൽ (a < 0)
                        തിരികെ a;

                b = unhex_next(&x, &l);
                എങ്കിൽ (b < 0)
                        തിരികെ b;

                *(z++) = (uint8_t) a << 4 | (uint8_t) b;
        }

        *z = 0;

        എങ്കിൽ (ret_size)
                *ret_size = (size_t) (z - buf);
        എങ്കിൽ (ret_data)
                *ret_data = TAKE_PTR(buf);

        തിരികെ 0;
}

/* https://tools.ietf.org/html/rfc4648#section-6
 * Notice that base32hex differs from base32 in the alphabet it uses.
 * The distinction is that the base32hex representation preserves the
 * order of the underlying data when compared as bytestrings, this is
 * useful when representing NSEC3 hashes, as one can then verify the
 * order of hashes directly from their representation. */
അക്ഷരം base32hexchar(പൂർണ്ണസംഖ്യ x) {
        സ്ഥിരം അക്ഷരം *table = DIGITS "ABCDEFGHIJKLMNOPQRSTUV";
        തിരികെ table[x & 31];
}

പൂർണ്ണസംഖ്യ unbase32hexchar(അക്ഷരം c) {
        unsigned offset;

        എങ്കിൽ (c >= '0' && c <= '9')
                തിരികെ c - '0';

        offset = '9' - '0' + 1;

        എങ്കിൽ (c >= 'A' && c <= 'V')
                തിരികെ c - 'A' + offset;

        തിരികെ -EINVAL;
}

അക്ഷരം* base32hexmem(സ്ഥിരം ശൂന്യം *p, size_t l, bool padding) {
        അക്ഷരം *r, *z;
        സ്ഥിരം uint8_t *x;
        size_t len;

        പ്രതിജ്ഞാ(p || l == 0);

        എങ്കിൽ (padding)
                /* five input bytes makes eight output bytes, padding is added so we must round up */
                len = 8 * (l + 4) / 5;
        അല്ലെങ്കിൽ {
                /* same, but round down as there is no padding */
                len = 8 * l / 5;

                തിരഞ്ഞെടുപ്പ് (l % 5) {
                അവസ്ഥ 4:
                        len += 7;
                        നിർത്തുക;
                അവസ്ഥ 3:
                        len += 5;
                        നിർത്തുക;
                അവസ്ഥ 2:
                        len += 4;
                        നിർത്തുക;
                അവസ്ഥ 1:
                        len += 2;
                        നിർത്തുക;
                }
        }

        z = r = malloc(len + 1);
        എങ്കിൽ (!r)
                തിരികെ ശൂന്യ;

        വേണ്ടി (x = p; x < (സ്ഥിരം uint8_t*) p + (l / 5) * 5; x += 5) {
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

        തിരഞ്ഞെടുപ്പ് (l % 5) {
        അവസ്ഥ 4:
                *(z++) = base32hexchar(x[0] >> 3);                    /* 000XXXXX */
                *(z++) = base32hexchar((x[0] & 7) << 2 | x[1] >> 6);  /* 000XXXYY */
                *(z++) = base32hexchar((x[1] & 63) >> 1);             /* 000YYYYY */
                *(z++) = base32hexchar((x[1] & 1) << 4 | x[2] >> 4);   /* 000YZZZZ */
                *(z++) = base32hexchar((x[2] & 15) << 1 | x[3] >> 7); /* 000ZZZZQ */
                *(z++) = base32hexchar((x[3] & 127) >> 2);            /* 000QQQQQ */
                *(z++) = base32hexchar((x[3] & 3) << 3);              /* 000QQ000 */
                എങ്കിൽ (padding)
                        *(z++) = '=';

                നിർത്തുക;

        അവസ്ഥ 3:
                *(z++) = base32hexchar(x[0] >> 3);                   /* 000XXXXX */
                *(z++) = base32hexchar((x[0] & 7) << 2 | x[1] >> 6); /* 000XXXYY */
                *(z++) = base32hexchar((x[1] & 63) >> 1);            /* 000YYYYY */
                *(z++) = base32hexchar((x[1] & 1) << 4 | x[2] >> 4); /* 000YZZZZ */
                *(z++) = base32hexchar((x[2] & 15) << 1);            /* 000ZZZZ0 */
                എങ്കിൽ (padding) {
                        *(z++) = '=';
                        *(z++) = '=';
                        *(z++) = '=';
                }

                നിർത്തുക;

        അവസ്ഥ 2:
                *(z++) = base32hexchar(x[0] >> 3);                   /* 000XXXXX */
                *(z++) = base32hexchar((x[0] & 7) << 2 | x[1] >> 6); /* 000XXXYY */
                *(z++) = base32hexchar((x[1] & 63) >> 1);            /* 000YYYYY */
                *(z++) = base32hexchar((x[1] & 1) << 4);             /* 000Y0000 */
                എങ്കിൽ (padding) {
                        *(z++) = '=';
                        *(z++) = '=';
                        *(z++) = '=';
                        *(z++) = '=';
                }

                നിർത്തുക;

        അവസ്ഥ 1:
                *(z++) = base32hexchar(x[0] >> 3);       /* 000XXXXX */
                *(z++) = base32hexchar((x[0] & 7) << 2); /* 000XXX00 */
                എങ്കിൽ (padding) {
                        *(z++) = '=';
                        *(z++) = '=';
                        *(z++) = '=';
                        *(z++) = '=';
                        *(z++) = '=';
                        *(z++) = '=';
                }

                നിർത്തുക;
        }

        *z = 0;
        തിരികെ r;
}

പൂർണ്ണസംഖ്യ unbase32hexmem(സ്ഥിരം അക്ഷരം *p, size_t l, bool padding, ശൂന്യം **mem, size_t *_len) {
        _cleanup_free_ uint8_t *r = ശൂന്യ;
        പൂർണ്ണസംഖ്യ a, b, c, d, e, f, g, h;
        uint8_t *z;
        സ്ഥിരം അക്ഷരം *x;
        size_t len;
        unsigned pad = 0;

        പ്രതിജ്ഞാ(p || l == 0);
        പ്രതിജ്ഞാ(mem);
        പ്രതിജ്ഞാ(_len);

        എങ്കിൽ (l == SIZE_MAX)
                l = strlen(p);

        /* padding ensures any base32hex input has input divisible by 8 */
        എങ്കിൽ (padding && l % 8 != 0)
                തിരികെ -EINVAL;

        എങ്കിൽ (padding) {
                /* strip the padding */
                ആകുമ്പോൾ (l > 0 && p[l - 1] == '=' && pad < 7) {
                        pad++;
                        l--;
                }
        }

        /* a group of eight input bytes needs five output bytes, in case of
         * padding we need to add some extra bytes */
        len = (l / 8) * 5;

        തിരഞ്ഞെടുപ്പ് (l % 8) {
        അവസ്ഥ 7:
                len += 4;
                നിർത്തുക;
        അവസ്ഥ 5:
                len += 3;
                നിർത്തുക;
        അവസ്ഥ 4:
                len += 2;
                നിർത്തുക;
        അവസ്ഥ 2:
                len += 1;
                നിർത്തുക;
        അവസ്ഥ 0:
                നിർത്തുക;
        സ്ഥിരസ്ഥിതി:
                തിരികെ -EINVAL;
        }

        z = r = malloc(len + 1);
        എങ്കിൽ (!r)
                തിരികെ -ENOMEM;

        വേണ്ടി (x = p; x < p + (l / 8) * 8; x += 8) {
                /* a == 000XXXXX; b == 000YYYYY; c == 000ZZZZZ; d == 000WWWWW
                 * e == 000SSSSS; f == 000QQQQQ; g == 000VVVVV; h == 000RRRRR */
                a = unbase32hexchar(x[0]);
                എങ്കിൽ (a < 0)
                        തിരികെ -EINVAL;

                b = unbase32hexchar(x[1]);
                എങ്കിൽ (b < 0)
                        തിരികെ -EINVAL;

                c = unbase32hexchar(x[2]);
                എങ്കിൽ (c < 0)
                        തിരികെ -EINVAL;

                d = unbase32hexchar(x[3]);
                എങ്കിൽ (d < 0)
                        തിരികെ -EINVAL;

                e = unbase32hexchar(x[4]);
                എങ്കിൽ (e < 0)
                        തിരികെ -EINVAL;

                f = unbase32hexchar(x[5]);
                എങ്കിൽ (f < 0)
                        തിരികെ -EINVAL;

                g = unbase32hexchar(x[6]);
                എങ്കിൽ (g < 0)
                        തിരികെ -EINVAL;

                h = unbase32hexchar(x[7]);
                എങ്കിൽ (h < 0)
                        തിരികെ -EINVAL;

                *(z++) = (uint8_t) a << 3 | (uint8_t) b >> 2;                    /* XXXXXYYY */
                *(z++) = (uint8_t) b << 6 | (uint8_t) c << 1 | (uint8_t) d >> 4; /* YYZZZZZW */
                *(z++) = (uint8_t) d << 4 | (uint8_t) e >> 1;                    /* WWWWSSSS */
                *(z++) = (uint8_t) e << 7 | (uint8_t) f << 2 | (uint8_t) g >> 3; /* SQQQQQVV */
                *(z++) = (uint8_t) g << 5 | (uint8_t) h;                         /* VVVRRRRR */
        }

        തിരഞ്ഞെടുപ്പ് (l % 8) {
        അവസ്ഥ 7:
                a = unbase32hexchar(x[0]);
                എങ്കിൽ (a < 0)
                        തിരികെ -EINVAL;

                b = unbase32hexchar(x[1]);
                എങ്കിൽ (b < 0)
                        തിരികെ -EINVAL;

                c = unbase32hexchar(x[2]);
                എങ്കിൽ (c < 0)
                        തിരികെ -EINVAL;

                d = unbase32hexchar(x[3]);
                എങ്കിൽ (d < 0)
                        തിരികെ -EINVAL;

                e = unbase32hexchar(x[4]);
                എങ്കിൽ (e < 0)
                        തിരികെ -EINVAL;

                f = unbase32hexchar(x[5]);
                എങ്കിൽ (f < 0)
                        തിരികെ -EINVAL;

                g = unbase32hexchar(x[6]);
                എങ്കിൽ (g < 0)
                        തിരികെ -EINVAL;

                /* g == 000VV000 */
                എങ്കിൽ (g & 7)
                        തിരികെ -EINVAL;

                *(z++) = (uint8_t) a << 3 | (uint8_t) b >> 2;                    /* XXXXXYYY */
                *(z++) = (uint8_t) b << 6 | (uint8_t) c << 1 | (uint8_t) d >> 4; /* YYZZZZZW */
                *(z++) = (uint8_t) d << 4 | (uint8_t) e >> 1;                    /* WWWWSSSS */
                *(z++) = (uint8_t) e << 7 | (uint8_t) f << 2 | (uint8_t) g >> 3; /* SQQQQQVV */

                നിർത്തുക;
        അവസ്ഥ 5:
                a = unbase32hexchar(x[0]);
                എങ്കിൽ (a < 0)
                        തിരികെ -EINVAL;

                b = unbase32hexchar(x[1]);
                എങ്കിൽ (b < 0)
                        തിരികെ -EINVAL;

                c = unbase32hexchar(x[2]);
                എങ്കിൽ (c < 0)
                        തിരികെ -EINVAL;

                d = unbase32hexchar(x[3]);
                എങ്കിൽ (d < 0)
                        തിരികെ -EINVAL;

                e = unbase32hexchar(x[4]);
                എങ്കിൽ (e < 0)
                        തിരികെ -EINVAL;

                /* e == 000SSSS0 */
                എങ്കിൽ (e & 1)
                        തിരികെ -EINVAL;

                *(z++) = (uint8_t) a << 3 | (uint8_t) b >> 2;                    /* XXXXXYYY */
                *(z++) = (uint8_t) b << 6 | (uint8_t) c << 1 | (uint8_t) d >> 4; /* YYZZZZZW */
                *(z++) = (uint8_t) d << 4 | (uint8_t) e >> 1;                    /* WWWWSSSS */

                നിർത്തുക;
        അവസ്ഥ 4:
                a = unbase32hexchar(x[0]);
                എങ്കിൽ (a < 0)
                        തിരികെ -EINVAL;

                b = unbase32hexchar(x[1]);
                എങ്കിൽ (b < 0)
                        തിരികെ -EINVAL;

                c = unbase32hexchar(x[2]);
                എങ്കിൽ (c < 0)
                        തിരികെ -EINVAL;

                d = unbase32hexchar(x[3]);
                എങ്കിൽ (d < 0)
                        തിരികെ -EINVAL;

                /* d == 000W0000 */
                എങ്കിൽ (d & 15)
                        തിരികെ -EINVAL;

                *(z++) = (uint8_t) a << 3 | (uint8_t) b >> 2;                    /* XXXXXYYY */
                *(z++) = (uint8_t) b << 6 | (uint8_t) c << 1 | (uint8_t) d >> 4; /* YYZZZZZW */

                നിർത്തുക;
        അവസ്ഥ 2:
                a = unbase32hexchar(x[0]);
                എങ്കിൽ (a < 0)
                        തിരികെ -EINVAL;

                b = unbase32hexchar(x[1]);
                എങ്കിൽ (b < 0)
                        തിരികെ -EINVAL;

                /* b == 000YYY00 */
                എങ്കിൽ (b & 3)
                        തിരികെ -EINVAL;

                *(z++) = (uint8_t) a << 3 | (uint8_t) b >> 2; /* XXXXXYYY */

                നിർത്തുക;
        അവസ്ഥ 0:
                നിർത്തുക;
        സ്ഥിരസ്ഥിതി:
                തിരികെ -EINVAL;
        }

        *z = 0;

        *mem = TAKE_PTR(r);
        *_len = len;

        തിരികെ 0;
}

/* https://tools.ietf.org/html/rfc4648#section-4 */
അക്ഷരം base64char(പൂർണ്ണസംഖ്യ x) {
        സ്ഥിരം അക്ഷരം *table = UPPERCASE_LETTERS LOWERCASE_LETTERS DIGITS "+/";
        തിരികെ table[x & 63];
}

/* This is almost base64char(), but not entirely, as it uses the "url and filename safe" alphabet,
 * since we don't want "/" appear in interface names (since interfaces appear in sysfs as filenames).
 * See section #5 of RFC 4648. */
അക്ഷരം urlsafe_base64char(പൂർണ്ണസംഖ്യ x) {
        സ്ഥിരം അക്ഷരം *table = UPPERCASE_LETTERS LOWERCASE_LETTERS DIGITS "-_";
        തിരികെ table[x & 63];
}

പൂർണ്ണസംഖ്യ unbase64char(അക്ഷരം c) {
        unsigned offset;

        എങ്കിൽ (c >= 'A' && c <= 'Z')
                തിരികെ c - 'A';

        offset = 'Z' - 'A' + 1;

        എങ്കിൽ (c >= 'a' && c <= 'z')
                തിരികെ c - 'a' + offset;

        offset += 'z' - 'a' + 1;

        എങ്കിൽ (c >= '0' && c <= '9')
                തിരികെ c - '0' + offset;

        offset += '9' - '0' + 1;

        എങ്കിൽ (IN_SET(c, '+', '-')) /* Support both the regular and the URL safe character set (see above) */
                തിരികെ offset;

        offset++;

        എങ്കിൽ (IN_SET(c, '/', '_')) /* ditto */
                തിരികെ offset;

        തിരികെ -EINVAL;
}

സ്ഥിതം ശൂന്യം maybe_വരി_break(അക്ഷരം **x, അക്ഷരം *start, size_t വരി_break) {
        size_t n;

        പ്രതിജ്ഞാ(x);
        പ്രതിജ്ഞാ(*x);
        പ്രതിജ്ഞാ(start);
        പ്രതിജ്ഞാ(*x >= start);

        എങ്കിൽ (വരി_break == SIZE_MAX)
                തിരികെ;

        n = *x - start;

        എങ്കിൽ (n % (വരി_break + 1) == വരി_break)
                *((*x)++) = '\n';
}

ssize_t base64mem_full(
                സ്ഥിരം ശൂന്യം *p,
                size_t l,
                size_t വരി_break,
                അക്ഷരം **ret) {

        സ്ഥിരം uint8_t *x;
        അക്ഷരം *b, *z;
        size_t m;

        പ്രതിജ്ഞാ(p || l == 0);
        പ്രതിജ്ഞാ(വരി_break > 0);
        പ്രതിജ്ഞാ(ret);

        /* three input bytes makes four output bytes, padding is added so we must round up */
        m = 4 * (l + 2) / 3 + 1;
        എങ്കിൽ (വരി_break != SIZE_MAX)
                m += m / വരി_break;

        z = b = malloc(m);
        എങ്കിൽ (!b)
                തിരികെ -ENOMEM;

        വേണ്ടി (x = p; x && x < (സ്ഥിരം uint8_t*) p + (l / 3) * 3; x += 3) {
                /* x[0] == XXXXXXXX; x[1] == YYYYYYYY; x[2] == ZZZZZZZZ */
                maybe_വരി_break(&z, b, വരി_break);
                *(z++) = base64char(x[0] >> 2);                    /* 00XXXXXX */
                maybe_വരി_break(&z, b, വരി_break);
                *(z++) = base64char((x[0] & 3) << 4 | x[1] >> 4);  /* 00XXYYYY */
                maybe_വരി_break(&z, b, വരി_break);
                *(z++) = base64char((x[1] & 15) << 2 | x[2] >> 6); /* 00YYYYZZ */
                maybe_വരി_break(&z, b, വരി_break);
                *(z++) = base64char(x[2] & 63);                    /* 00ZZZZZZ */
        }

        തിരഞ്ഞെടുപ്പ് (l % 3) {
        അവസ്ഥ 2:
                maybe_വരി_break(&z, b, വരി_break);
                *(z++) = base64char(x[0] >> 2);                   /* 00XXXXXX */
                maybe_വരി_break(&z, b, വരി_break);
                *(z++) = base64char((x[0] & 3) << 4 | x[1] >> 4); /* 00XXYYYY */
                maybe_വരി_break(&z, b, വരി_break);
                *(z++) = base64char((x[1] & 15) << 2);            /* 00YYYY00 */
                maybe_വരി_break(&z, b, വരി_break);
                *(z++) = '=';
                നിർത്തുക;

        അവസ്ഥ 1:
                maybe_വരി_break(&z, b, വരി_break);
                *(z++) = base64char(x[0] >> 2);        /* 00XXXXXX */
                maybe_വരി_break(&z, b, വരി_break);
                *(z++) = base64char((x[0] & 3) << 4);  /* 00XX0000 */
                maybe_വരി_break(&z, b, വരി_break);
                *(z++) = '=';
                maybe_വരി_break(&z, b, വരി_break);
                *(z++) = '=';
                നിർത്തുക;
        }

        *z = 0;
        *ret = b;

        പ്രതിജ്ഞാ(z >= b); /* Let static analyzers know that the answer is non-negative. */
        തിരികെ z - b;
}

സ്ഥിതം ssize_t base64_append_width(
                അക്ഷരം **prefix,
                size_t plen,
                അക്ഷരം sep,
                size_t indent,
                സ്ഥിരം ശൂന്യം *p,
                size_t l,
                size_t width) {

        _cleanup_free_ അക്ഷരം *x = ശൂന്യ;
        അക്ഷരം *t, *s;
        size_t വരികൾ;
        ssize_t len;

        പ്രതിജ്ഞാ(prefix);
        പ്രതിജ്ഞാ(*prefix || plen == 0);
        പ്രതിജ്ഞാ(p || l == 0);

        len = base64mem(p, l, &x);
        എങ്കിൽ (len < 0)
                തിരികെ len;
        എങ്കിൽ (len == 0)
                തിരികെ plen;

        വരികൾ = DIV_ROUND_UP(len, width);

        എങ്കിൽ (plen >= SSIZE_MAX - 1 - 1 ||
            വരികൾ > (SSIZE_MAX - plen - 1 - 1) / (indent + width + 1))
                തിരികെ -ENOMEM;

        t = realloc(*prefix, plen + 1 + 1 + (indent + width + 1) * വരികൾ);
        എങ്കിൽ (!t)
                തിരികെ -ENOMEM;

        s = t + plen;
        വേണ്ടി (size_t വരി = 0; വരി < വരികൾ; വരി++) {
                size_t act = MIN(width, (size_t) len);

                എങ്കിൽ (വരി > 0)
                        sep = '\n';

                എങ്കിൽ (s > t) {
                        *s++ = sep;
                        എങ്കിൽ (sep == '\n')
                                s = mempset(s, ' ', indent);
                }

                s = mempcpy(s, x + width * വരി, act);
                len -= act;
        }
        പ്രതിജ്ഞാ(len == 0);

        *s = '\0';
        *prefix = t;
        തിരികെ s - t;
}

ssize_t base64_append(
                അക്ഷരം **prefix,
                size_t plen,
                സ്ഥിരം ശൂന്യം *p,
                size_t l,
                size_t indent,
                size_t width) {

        എങ്കിൽ (plen > width / 2 || plen + indent > width)
                /* leave indent on the left, keep last column free */
                തിരികെ base64_append_width(prefix, plen, '\n', indent, p, l, width - indent);
        അല്ലെങ്കിൽ
                /* leave plen on the left, keep last column free */
                തിരികെ base64_append_width(prefix, plen, ' ', plen + 1, p, l, width - plen - 1);
}

സ്ഥിതം പൂർണ്ണസംഖ്യ unbase64_next(സ്ഥിരം അക്ഷരം **p, size_t *l) {
        പൂർണ്ണസംഖ്യ ret;

        പ്രതിജ്ഞാ(p);
        പ്രതിജ്ഞാ(l);

        /* Find the next non-whitespace character, and decode it. If we find padding, we return it as INT_MAX. We
         * greedily skip all preceding and all following whitespace. */

        വേണ്ടി (;;) {
                എങ്കിൽ (*l == 0)
                        തിരികെ -EPIPE;

                എങ്കിൽ (!strchr(WHITESPACE, **p))
                        നിർത്തുക;

                /* Skip leading whitespace */
                (*p)++, (*l)--;
        }

        എങ്കിൽ (**p == '=')
                ret = INT_MAX; /* return padding as INT_MAX */
        അല്ലെങ്കിൽ {
                ret = unbase64char(**p);
                എങ്കിൽ (ret < 0)
                        തിരികെ ret;
        }

        വേണ്ടി (;;) {
                (*p)++, (*l)--;

                എങ്കിൽ (*l == 0)
                        നിർത്തുക;
                എങ്കിൽ (!strchr(WHITESPACE, **p))
                        നിർത്തുക;

                /* Skip following whitespace */
        }

        തിരികെ ret;
}

പൂർണ്ണസംഖ്യ unbase64mem_full(
                സ്ഥിരം അക്ഷരം *p,
                size_t l,
                bool secure,
                ശൂന്യം **ret_data,
                size_t *ret_size) {

        _cleanup_free_ uint8_t *buf = ശൂന്യ;
        സ്ഥിരം അക്ഷരം *x;
        uint8_t *z;
        size_t len;

        പ്രതിജ്ഞാ(p || l == 0);

        എങ്കിൽ (l == SIZE_MAX)
                l = strlen(p);

        /* A group of four input bytes needs three output bytes, in case of padding we need to add two or three extra
         * bytes. Note that this calculation is an upper boundary, as we ignore whitespace while decoding */
        len = (l / 4) * 3 + (l % 4 != 0 ? (l % 4) - 1 : 0);

        buf = malloc(len + 1);
        എങ്കിൽ (!buf)
                തിരികെ -ENOMEM;

        CLEANUP_ERASE_PTR(secure ? &buf : ശൂന്യ, len);

        വേണ്ടി (x = p, z = buf;;) {
                പൂർണ്ണസംഖ്യ a, b, c, d; /* a == 00XXXXXX; b == 00YYYYYY; c == 00ZZZZZZ; d == 00WWWWWW */

                a = unbase64_next(&x, &l);
                എങ്കിൽ (a == -EPIPE) /* End of string */
                        നിർത്തുക;
                എങ്കിൽ (a < 0)
                        തിരികെ a;
                എങ്കിൽ (a == INT_MAX) /* Padding is not allowed at the beginning of a 4ch block */
                        തിരികെ -EINVAL;

                b = unbase64_next(&x, &l);
                എങ്കിൽ (b < 0)
                        തിരികെ b;
                എങ്കിൽ (b == INT_MAX) /* Padding is not allowed at the second character of a 4ch block either */
                        തിരികെ -EINVAL;

                c = unbase64_next(&x, &l);
                എങ്കിൽ (c < 0)
                        തിരികെ c;

                d = unbase64_next(&x, &l);
                എങ്കിൽ (d < 0)
                        തിരികെ d;

                എങ്കിൽ (c == INT_MAX) { /* Padding at the third character */

                        എങ്കിൽ (d != INT_MAX) /* If the third character is padding, the fourth must be too */
                                തിരികെ -EINVAL;

                        /* b == 00YY0000 */
                        എങ്കിൽ (b & 15)
                                തിരികെ -EINVAL;

                        എങ്കിൽ (l > 0) /* Trailing rubbish? */
                                തിരികെ -ENAMETOOLONG;

                        *(z++) = (uint8_t) a << 2 | (uint8_t) (b >> 4); /* XXXXXXYY */
                        നിർത്തുക;
                }

                എങ്കിൽ (d == INT_MAX) {
                        /* c == 00ZZZZ00 */
                        എങ്കിൽ (c & 3)
                                തിരികെ -EINVAL;

                        എങ്കിൽ (l > 0) /* Trailing rubbish? */
                                തിരികെ -ENAMETOOLONG;

                        *(z++) = (uint8_t) a << 2 | (uint8_t) b >> 4; /* XXXXXXYY */
                        *(z++) = (uint8_t) b << 4 | (uint8_t) c >> 2; /* YYYYZZZZ */
                        നിർത്തുക;
                }

                *(z++) = (uint8_t) a << 2 | (uint8_t) b >> 4; /* XXXXXXYY */
                *(z++) = (uint8_t) b << 4 | (uint8_t) c >> 2; /* YYYYZZZZ */
                *(z++) = (uint8_t) c << 6 | (uint8_t) d;      /* ZZWWWWWW */
        }

        *z = 0;

        പ്രതിജ്ഞാ((size_t) (z - buf) <= len);

        എങ്കിൽ (ret_size)
                *ret_size = (size_t) (z - buf);
        എങ്കിൽ (ret_data)
                *ret_data = TAKE_PTR(buf);

        തിരികെ 0;
}

ശൂന്യം hexdump(FILE *f, സ്ഥിരം ശൂന്യം *p, size_t s) {
        സ്ഥിരം uint8_t *b = p;
        unsigned n = 0;

        പ്രതിജ്ഞാ(b || s == 0);

        എങ്കിൽ (s == SIZE_MAX)
                s = strlen(p);

        എങ്കിൽ (!f)
                f = stdout;

        ആകുമ്പോൾ (s > 0) {
                size_t i;

                fprintf(f, "%04x  ", n);

                വേണ്ടി (i = 0; i < 16; i++) {

                        എങ്കിൽ (i >= s)
                                fputs("   ", f);
                        അല്ലെങ്കിൽ
                                fprintf(f, "%02x ", b[i]);

                        എങ്കിൽ (i == 7)
                                fputc(' ', f);
                }

                fputc(' ', f);

                വേണ്ടി (i = 0; i < 16; i++) {

                        എങ്കിൽ (i >= s)
                                fputc(' ', f);
                        അല്ലെങ്കിൽ
                                fputc(isprint(b[i]) ? (അക്ഷരം) b[i] : '.', f);
                }

                fputc('\n', f);

                എങ്കിൽ (s < 16)
                        നിർത്തുക;

                n += 16;
                b += 16;
                s -= 16;
        }
}
