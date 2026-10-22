/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ਅਨ੍ਤਰ੍ਭਾਵ <ctype.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <stdio.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "alloc-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hexdecoct.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "memory-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "string-util.h"

ਅੱਖਰ octchar(ਪੂਰਨ x) {
        ਵਾਪਸ '0' + (x & 7);
}

ਪੂਰਨ unoctchar(ਅੱਖਰ c) {

        ਜੇ (c >= '0' && c <= '7')
                ਵਾਪਸ c - '0';

        ਵਾਪਸ -EINVAL;
}

ਅੱਖਰ decchar(ਪੂਰਨ x) {
        ਵਾਪਸ '0' + (x % 10);
}

ਪੂਰਨ undecchar(ਅੱਖਰ c) {

        ਜੇ (c >= '0' && c <= '9')
                ਵਾਪਸ c - '0';

        ਵਾਪਸ -EINVAL;
}

ਅੱਖਰ hexchar(ਪੂਰਨ x) {
        ਅਚਲ ਅੱਖਰ *ਸਾਰਣੀ = LOWERCASE_HEXDIGITS;
        ਵਾਪਸ ਸਾਰਣੀ[x & 15];
}

ਪੂਰਨ unhexchar(ਅੱਖਰ c) {

        ਜੇ (c >= '0' && c <= '9')
                ਵਾਪਸ c - '0';

        ਜੇ (c >= 'a' && c <= 'f')
                ਵਾਪਸ c - 'a' + 10;

        ਜੇ (c >= 'A' && c <= 'F')
                ਵਾਪਸ c - 'A' + 10;

        ਵਾਪਸ -EINVAL;
}

ਅੱਖਰ* hexmem(ਅਚਲ ਖਾਲੀ *p, size_t l) {
        ਅਚਲ uint8_t *x;
        ਅੱਖਰ *r, *z;

        ਪ੍ਰਤਿਜ੍ਞਾ(p || l == 0);

        z = r = new(ਅੱਖਰ, l * 2 + 1);
        ਜੇ (!r)
                ਵਾਪਸ ਨਿਰਾ;

        ਲਈ (x = p; x && x < (ਅਚਲ uint8_t*) p + l; x++) {
                *(z++) = hexchar(*x >> 4);
                *(z++) = hexchar(*x & 15);
        }

        *z = 0;
        ਵਾਪਸ r;
}

ਸਥਿਰ ਪੂਰਨ unhex_ਅਗਲਾ(ਅਚਲ ਅੱਖਰ **p, size_t *l) {
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(p);
        ਪ੍ਰਤਿਜ੍ਞਾ(l);

        /* Find the next non-whitespace character, and decode it. We
         * greedily skip all preceding and all following whitespace. */

        ਲਈ (;;) {
                ਜੇ (*l == 0)
                        ਵਾਪਸ -EPIPE;

                ਜੇ (!strchr(WHITESPACE, **p))
                        ਰੋਕੋ;

                /* Skip leading whitespace */
                (*p)++, (*l)--;
        }

        r = unhexchar(**p);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਲਈ (;;) {
                (*p)++, (*l)--;

                ਜੇ (*l == 0 || !strchr(WHITESPACE, **p))
                        ਰੋਕੋ;

                /* Skip following whitespace */
        }

        ਵਾਪਸ r;
}

ਪੂਰਨ unhexmem_ਭਰਿਆ(
                ਅਚਲ ਅੱਖਰ *p,
                size_t l,
                bool secure,
                ਖਾਲੀ **ret_ਅੰਕੜੇ,
                size_t *ret_ਮਾਪ) {

        _cleanup_free_ uint8_t *buf = ਨਿਰਾ;
        size_t buf_size;
        ਅਚਲ ਅੱਖਰ *x;
        uint8_t *z;

        ਪ੍ਰਤਿਜ੍ਞਾ(p || l == 0);

        ਜੇ (l == SIZE_MAX)
                l = ਲੜੀ_ਲੰਬਾਈ(p);

        /* Note that the calculation of memory size is an upper boundary, as we ignore whitespace while decoding */
        buf_size = (l + 1) / 2 + 1;
        buf = ਸਮ੍ਰਿਤੀ_ਦਿਓ(buf_size);
        ਜੇ (!buf)
                ਵਾਪਸ -ENOMEM;

        CLEANUP_ERASE_PTR(secure ? &buf : ਨਿਰਾ, buf_size);

        ਲਈ (x = p, z = buf;;) {
                ਪੂਰਨ a, b;

                a = unhex_ਅਗਲਾ(&x, &l);
                ਜੇ (a == -EPIPE) /* End of string */
                        ਰੋਕੋ;
                ਜੇ (a < 0)
                        ਵਾਪਸ a;

                b = unhex_ਅਗਲਾ(&x, &l);
                ਜੇ (b < 0)
                        ਵਾਪਸ b;

                *(z++) = (uint8_t) a << 4 | (uint8_t) b;
        }

        *z = 0;

        ਜੇ (ret_ਮਾਪ)
                *ret_ਮਾਪ = (size_t) (z - buf);
        ਜੇ (ret_ਅੰਕੜੇ)
                *ret_ਅੰਕੜੇ = TAKE_PTR(buf);

        ਵਾਪਸ 0;
}

/* https://tools.ietf.org/html/rfc4648#section-6
 * Notice that base32hex differs from base32 in the alphabet it uses.
 * The distinction is that the base32hex representation preserves the
 * order of the underlying data when compared as bytestrings, this is
 * useful when representing NSEC3 hashes, as one can then verify the
 * order of hashes directly from their representation. */
ਅੱਖਰ base32hexchar(ਪੂਰਨ x) {
        ਅਚਲ ਅੱਖਰ *ਸਾਰਣੀ = DIGITS "ABCDEFGHIJKLMNOPQRSTUV";
        ਵਾਪਸ ਸਾਰਣੀ[x & 31];
}

ਪੂਰਨ unbase32hexchar(ਅੱਖਰ c) {
        ਬਿਨਾਨਿਸ਼ਾਨੀ ਵਿੱਥ;

        ਜੇ (c >= '0' && c <= '9')
                ਵਾਪਸ c - '0';

        ਵਿੱਥ = '9' - '0' + 1;

        ਜੇ (c >= 'A' && c <= 'V')
                ਵਾਪਸ c - 'A' + ਵਿੱਥ;

        ਵਾਪਸ -EINVAL;
}

ਅੱਖਰ* base32hexmem(ਅਚਲ ਖਾਲੀ *p, size_t l, bool padding) {
        ਅੱਖਰ *r, *z;
        ਅਚਲ uint8_t *x;
        size_t len;

        ਪ੍ਰਤਿਜ੍ਞਾ(p || l == 0);

        ਜੇ (padding)
                /* five input bytes makes eight output bytes, padding is added so we must round up */
                len = 8 * (l + 4) / 5;
        ਨਹੀਂਤਾਂ {
                /* same, but round down as there is no padding */
                len = 8 * l / 5;

                ਚੋਣ (l % 5) {
                ਹਾਲਤ 4:
                        len += 7;
                        ਰੋਕੋ;
                ਹਾਲਤ 3:
                        len += 5;
                        ਰੋਕੋ;
                ਹਾਲਤ 2:
                        len += 4;
                        ਰੋਕੋ;
                ਹਾਲਤ 1:
                        len += 2;
                        ਰੋਕੋ;
                }
        }

        z = r = ਸਮ੍ਰਿਤੀ_ਦਿਓ(len + 1);
        ਜੇ (!r)
                ਵਾਪਸ ਨਿਰਾ;

        ਲਈ (x = p; x < (ਅਚਲ uint8_t*) p + (l / 5) * 5; x += 5) {
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

        ਚੋਣ (l % 5) {
        ਹਾਲਤ 4:
                *(z++) = base32hexchar(x[0] >> 3);                    /* 000XXXXX */
                *(z++) = base32hexchar((x[0] & 7) << 2 | x[1] >> 6);  /* 000XXXYY */
                *(z++) = base32hexchar((x[1] & 63) >> 1);             /* 000YYYYY */
                *(z++) = base32hexchar((x[1] & 1) << 4 | x[2] >> 4);   /* 000YZZZZ */
                *(z++) = base32hexchar((x[2] & 15) << 1 | x[3] >> 7); /* 000ZZZZQ */
                *(z++) = base32hexchar((x[3] & 127) >> 2);            /* 000QQQQQ */
                *(z++) = base32hexchar((x[3] & 3) << 3);              /* 000QQ000 */
                ਜੇ (padding)
                        *(z++) = '=';

                ਰੋਕੋ;

        ਹਾਲਤ 3:
                *(z++) = base32hexchar(x[0] >> 3);                   /* 000XXXXX */
                *(z++) = base32hexchar((x[0] & 7) << 2 | x[1] >> 6); /* 000XXXYY */
                *(z++) = base32hexchar((x[1] & 63) >> 1);            /* 000YYYYY */
                *(z++) = base32hexchar((x[1] & 1) << 4 | x[2] >> 4); /* 000YZZZZ */
                *(z++) = base32hexchar((x[2] & 15) << 1);            /* 000ZZZZ0 */
                ਜੇ (padding) {
                        *(z++) = '=';
                        *(z++) = '=';
                        *(z++) = '=';
                }

                ਰੋਕੋ;

        ਹਾਲਤ 2:
                *(z++) = base32hexchar(x[0] >> 3);                   /* 000XXXXX */
                *(z++) = base32hexchar((x[0] & 7) << 2 | x[1] >> 6); /* 000XXXYY */
                *(z++) = base32hexchar((x[1] & 63) >> 1);            /* 000YYYYY */
                *(z++) = base32hexchar((x[1] & 1) << 4);             /* 000Y0000 */
                ਜੇ (padding) {
                        *(z++) = '=';
                        *(z++) = '=';
                        *(z++) = '=';
                        *(z++) = '=';
                }

                ਰੋਕੋ;

        ਹਾਲਤ 1:
                *(z++) = base32hexchar(x[0] >> 3);       /* 000XXXXX */
                *(z++) = base32hexchar((x[0] & 7) << 2); /* 000XXX00 */
                ਜੇ (padding) {
                        *(z++) = '=';
                        *(z++) = '=';
                        *(z++) = '=';
                        *(z++) = '=';
                        *(z++) = '=';
                        *(z++) = '=';
                }

                ਰੋਕੋ;
        }

        *z = 0;
        ਵਾਪਸ r;
}

ਪੂਰਨ unbase32hexmem(ਅਚਲ ਅੱਖਰ *p, size_t l, bool padding, ਖਾਲੀ **mem, size_t *_len) {
        _cleanup_free_ uint8_t *r = ਨਿਰਾ;
        ਪੂਰਨ a, b, c, d, e, f, g, h;
        uint8_t *z;
        ਅਚਲ ਅੱਖਰ *x;
        size_t len;
        ਬਿਨਾਨਿਸ਼ਾਨੀ pad = 0;

        ਪ੍ਰਤਿਜ੍ਞਾ(p || l == 0);
        ਪ੍ਰਤਿਜ੍ਞਾ(mem);
        ਪ੍ਰਤਿਜ੍ਞਾ(_len);

        ਜੇ (l == SIZE_MAX)
                l = ਲੜੀ_ਲੰਬਾਈ(p);

        /* padding ensures any base32hex input has input divisible by 8 */
        ਜੇ (padding && l % 8 != 0)
                ਵਾਪਸ -EINVAL;

        ਜੇ (padding) {
                /* strip the padding */
                ਜਦੋਂ (l > 0 && p[l - 1] == '=' && pad < 7) {
                        pad++;
                        l--;
                }
        }

        /* a group of eight input bytes needs five output bytes, in case of
         * padding we need to add some extra bytes */
        len = (l / 8) * 5;

        ਚੋਣ (l % 8) {
        ਹਾਲਤ 7:
                len += 4;
                ਰੋਕੋ;
        ਹਾਲਤ 5:
                len += 3;
                ਰੋਕੋ;
        ਹਾਲਤ 4:
                len += 2;
                ਰੋਕੋ;
        ਹਾਲਤ 2:
                len += 1;
                ਰੋਕੋ;
        ਹਾਲਤ 0:
                ਰੋਕੋ;
        ਮੂਲ:
                ਵਾਪਸ -EINVAL;
        }

        z = r = ਸਮ੍ਰਿਤੀ_ਦਿਓ(len + 1);
        ਜੇ (!r)
                ਵਾਪਸ -ENOMEM;

        ਲਈ (x = p; x < p + (l / 8) * 8; x += 8) {
                /* a == 000XXXXX; b == 000YYYYY; c == 000ZZZZZ; d == 000WWWWW
                 * e == 000SSSSS; f == 000QQQQQ; g == 000VVVVV; h == 000RRRRR */
                a = unbase32hexchar(x[0]);
                ਜੇ (a < 0)
                        ਵਾਪਸ -EINVAL;

                b = unbase32hexchar(x[1]);
                ਜੇ (b < 0)
                        ਵਾਪਸ -EINVAL;

                c = unbase32hexchar(x[2]);
                ਜੇ (c < 0)
                        ਵਾਪਸ -EINVAL;

                d = unbase32hexchar(x[3]);
                ਜੇ (d < 0)
                        ਵਾਪਸ -EINVAL;

                e = unbase32hexchar(x[4]);
                ਜੇ (e < 0)
                        ਵਾਪਸ -EINVAL;

                f = unbase32hexchar(x[5]);
                ਜੇ (f < 0)
                        ਵਾਪਸ -EINVAL;

                g = unbase32hexchar(x[6]);
                ਜੇ (g < 0)
                        ਵਾਪਸ -EINVAL;

                h = unbase32hexchar(x[7]);
                ਜੇ (h < 0)
                        ਵਾਪਸ -EINVAL;

                *(z++) = (uint8_t) a << 3 | (uint8_t) b >> 2;                    /* XXXXXYYY */
                *(z++) = (uint8_t) b << 6 | (uint8_t) c << 1 | (uint8_t) d >> 4; /* YYZZZZZW */
                *(z++) = (uint8_t) d << 4 | (uint8_t) e >> 1;                    /* WWWWSSSS */
                *(z++) = (uint8_t) e << 7 | (uint8_t) f << 2 | (uint8_t) g >> 3; /* SQQQQQVV */
                *(z++) = (uint8_t) g << 5 | (uint8_t) h;                         /* VVVRRRRR */
        }

        ਚੋਣ (l % 8) {
        ਹਾਲਤ 7:
                a = unbase32hexchar(x[0]);
                ਜੇ (a < 0)
                        ਵਾਪਸ -EINVAL;

                b = unbase32hexchar(x[1]);
                ਜੇ (b < 0)
                        ਵਾਪਸ -EINVAL;

                c = unbase32hexchar(x[2]);
                ਜੇ (c < 0)
                        ਵਾਪਸ -EINVAL;

                d = unbase32hexchar(x[3]);
                ਜੇ (d < 0)
                        ਵਾਪਸ -EINVAL;

                e = unbase32hexchar(x[4]);
                ਜੇ (e < 0)
                        ਵਾਪਸ -EINVAL;

                f = unbase32hexchar(x[5]);
                ਜੇ (f < 0)
                        ਵਾਪਸ -EINVAL;

                g = unbase32hexchar(x[6]);
                ਜੇ (g < 0)
                        ਵਾਪਸ -EINVAL;

                /* g == 000VV000 */
                ਜੇ (g & 7)
                        ਵਾਪਸ -EINVAL;

                *(z++) = (uint8_t) a << 3 | (uint8_t) b >> 2;                    /* XXXXXYYY */
                *(z++) = (uint8_t) b << 6 | (uint8_t) c << 1 | (uint8_t) d >> 4; /* YYZZZZZW */
                *(z++) = (uint8_t) d << 4 | (uint8_t) e >> 1;                    /* WWWWSSSS */
                *(z++) = (uint8_t) e << 7 | (uint8_t) f << 2 | (uint8_t) g >> 3; /* SQQQQQVV */

                ਰੋਕੋ;
        ਹਾਲਤ 5:
                a = unbase32hexchar(x[0]);
                ਜੇ (a < 0)
                        ਵਾਪਸ -EINVAL;

                b = unbase32hexchar(x[1]);
                ਜੇ (b < 0)
                        ਵਾਪਸ -EINVAL;

                c = unbase32hexchar(x[2]);
                ਜੇ (c < 0)
                        ਵਾਪਸ -EINVAL;

                d = unbase32hexchar(x[3]);
                ਜੇ (d < 0)
                        ਵਾਪਸ -EINVAL;

                e = unbase32hexchar(x[4]);
                ਜੇ (e < 0)
                        ਵਾਪਸ -EINVAL;

                /* e == 000SSSS0 */
                ਜੇ (e & 1)
                        ਵਾਪਸ -EINVAL;

                *(z++) = (uint8_t) a << 3 | (uint8_t) b >> 2;                    /* XXXXXYYY */
                *(z++) = (uint8_t) b << 6 | (uint8_t) c << 1 | (uint8_t) d >> 4; /* YYZZZZZW */
                *(z++) = (uint8_t) d << 4 | (uint8_t) e >> 1;                    /* WWWWSSSS */

                ਰੋਕੋ;
        ਹਾਲਤ 4:
                a = unbase32hexchar(x[0]);
                ਜੇ (a < 0)
                        ਵਾਪਸ -EINVAL;

                b = unbase32hexchar(x[1]);
                ਜੇ (b < 0)
                        ਵਾਪਸ -EINVAL;

                c = unbase32hexchar(x[2]);
                ਜੇ (c < 0)
                        ਵਾਪਸ -EINVAL;

                d = unbase32hexchar(x[3]);
                ਜੇ (d < 0)
                        ਵਾਪਸ -EINVAL;

                /* d == 000W0000 */
                ਜੇ (d & 15)
                        ਵਾਪਸ -EINVAL;

                *(z++) = (uint8_t) a << 3 | (uint8_t) b >> 2;                    /* XXXXXYYY */
                *(z++) = (uint8_t) b << 6 | (uint8_t) c << 1 | (uint8_t) d >> 4; /* YYZZZZZW */

                ਰੋਕੋ;
        ਹਾਲਤ 2:
                a = unbase32hexchar(x[0]);
                ਜੇ (a < 0)
                        ਵਾਪਸ -EINVAL;

                b = unbase32hexchar(x[1]);
                ਜੇ (b < 0)
                        ਵਾਪਸ -EINVAL;

                /* b == 000YYY00 */
                ਜੇ (b & 3)
                        ਵਾਪਸ -EINVAL;

                *(z++) = (uint8_t) a << 3 | (uint8_t) b >> 2; /* XXXXXYYY */

                ਰੋਕੋ;
        ਹਾਲਤ 0:
                ਰੋਕੋ;
        ਮੂਲ:
                ਵਾਪਸ -EINVAL;
        }

        *z = 0;

        *mem = TAKE_PTR(r);
        *_len = len;

        ਵਾਪਸ 0;
}

/* https://tools.ietf.org/html/rfc4648#section-4 */
ਅੱਖਰ base64char(ਪੂਰਨ x) {
        ਅਚਲ ਅੱਖਰ *ਸਾਰਣੀ = UPPERCASE_LETTERS LOWERCASE_LETTERS DIGITS "+/";
        ਵਾਪਸ ਸਾਰਣੀ[x & 63];
}

/* This is almost base64char(), but not entirely, as it uses the "url and filename safe" alphabet,
 * since we don't want "/" appear in interface names (since interfaces appear in sysfs as filenames).
 * See section #5 of RFC 4648. */
ਅੱਖਰ urlsafe_base64char(ਪੂਰਨ x) {
        ਅਚਲ ਅੱਖਰ *ਸਾਰਣੀ = UPPERCASE_LETTERS LOWERCASE_LETTERS DIGITS "-_";
        ਵਾਪਸ ਸਾਰਣੀ[x & 63];
}

ਪੂਰਨ unbase64char(ਅੱਖਰ c) {
        ਬਿਨਾਨਿਸ਼ਾਨੀ ਵਿੱਥ;

        ਜੇ (c >= 'A' && c <= 'Z')
                ਵਾਪਸ c - 'A';

        ਵਿੱਥ = 'Z' - 'A' + 1;

        ਜੇ (c >= 'a' && c <= 'z')
                ਵਾਪਸ c - 'a' + ਵਿੱਥ;

        ਵਿੱਥ += 'z' - 'a' + 1;

        ਜੇ (c >= '0' && c <= '9')
                ਵਾਪਸ c - '0' + ਵਿੱਥ;

        ਵਿੱਥ += '9' - '0' + 1;

        ਜੇ (ਵਿੱਚ_ਸੈੱਟ(c, '+', '-')) /* Support both the regular and the URL safe character set (see above) */
                ਵਾਪਸ ਵਿੱਥ;

        ਵਿੱਥ++;

        ਜੇ (ਵਿੱਚ_ਸੈੱਟ(c, '/', '_')) /* ditto */
                ਵਾਪਸ ਵਿੱਥ;

        ਵਾਪਸ -EINVAL;
}

ਸਥਿਰ ਖਾਲੀ maybe_ਸਤਰ_break(ਅੱਖਰ **x, ਅੱਖਰ *ਸ਼ੁਰੂ, size_t ਸਤਰ_break) {
        size_t n;

        ਪ੍ਰਤਿਜ੍ਞਾ(x);
        ਪ੍ਰਤਿਜ੍ਞਾ(*x);
        ਪ੍ਰਤਿਜ੍ਞਾ(ਸ਼ੁਰੂ);
        ਪ੍ਰਤਿਜ੍ਞਾ(*x >= ਸ਼ੁਰੂ);

        ਜੇ (ਸਤਰ_break == SIZE_MAX)
                ਵਾਪਸ;

        n = *x - ਸ਼ੁਰੂ;

        ਜੇ (n % (ਸਤਰ_break + 1) == ਸਤਰ_break)
                *((*x)++) = '\n';
}

ssize_t base64mem_ਭਰਿਆ(
                ਅਚਲ ਖਾਲੀ *p,
                size_t l,
                size_t ਸਤਰ_break,
                ਅੱਖਰ **ret) {

        ਅਚਲ uint8_t *x;
        ਅੱਖਰ *b, *z;
        size_t m;

        ਪ੍ਰਤਿਜ੍ਞਾ(p || l == 0);
        ਪ੍ਰਤਿਜ੍ਞਾ(ਸਤਰ_break > 0);
        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        /* three input bytes makes four output bytes, padding is added so we must round up */
        m = 4 * (l + 2) / 3 + 1;
        ਜੇ (ਸਤਰ_break != SIZE_MAX)
                m += m / ਸਤਰ_break;

        z = b = ਸਮ੍ਰਿਤੀ_ਦਿਓ(m);
        ਜੇ (!b)
                ਵਾਪਸ -ENOMEM;

        ਲਈ (x = p; x && x < (ਅਚਲ uint8_t*) p + (l / 3) * 3; x += 3) {
                /* x[0] == XXXXXXXX; x[1] == YYYYYYYY; x[2] == ZZZZZZZZ */
                maybe_ਸਤਰ_break(&z, b, ਸਤਰ_break);
                *(z++) = base64char(x[0] >> 2);                    /* 00XXXXXX */
                maybe_ਸਤਰ_break(&z, b, ਸਤਰ_break);
                *(z++) = base64char((x[0] & 3) << 4 | x[1] >> 4);  /* 00XXYYYY */
                maybe_ਸਤਰ_break(&z, b, ਸਤਰ_break);
                *(z++) = base64char((x[1] & 15) << 2 | x[2] >> 6); /* 00YYYYZZ */
                maybe_ਸਤਰ_break(&z, b, ਸਤਰ_break);
                *(z++) = base64char(x[2] & 63);                    /* 00ZZZZZZ */
        }

        ਚੋਣ (l % 3) {
        ਹਾਲਤ 2:
                maybe_ਸਤਰ_break(&z, b, ਸਤਰ_break);
                *(z++) = base64char(x[0] >> 2);                   /* 00XXXXXX */
                maybe_ਸਤਰ_break(&z, b, ਸਤਰ_break);
                *(z++) = base64char((x[0] & 3) << 4 | x[1] >> 4); /* 00XXYYYY */
                maybe_ਸਤਰ_break(&z, b, ਸਤਰ_break);
                *(z++) = base64char((x[1] & 15) << 2);            /* 00YYYY00 */
                maybe_ਸਤਰ_break(&z, b, ਸਤਰ_break);
                *(z++) = '=';
                ਰੋਕੋ;

        ਹਾਲਤ 1:
                maybe_ਸਤਰ_break(&z, b, ਸਤਰ_break);
                *(z++) = base64char(x[0] >> 2);        /* 00XXXXXX */
                maybe_ਸਤਰ_break(&z, b, ਸਤਰ_break);
                *(z++) = base64char((x[0] & 3) << 4);  /* 00XX0000 */
                maybe_ਸਤਰ_break(&z, b, ਸਤਰ_break);
                *(z++) = '=';
                maybe_ਸਤਰ_break(&z, b, ਸਤਰ_break);
                *(z++) = '=';
                ਰੋਕੋ;
        }

        *z = 0;
        *ret = b;

        ਪ੍ਰਤਿਜ੍ਞਾ(z >= b); /* Let static analyzers know that the answer is non-negative. */
        ਵਾਪਸ z - b;
}

ਸਥਿਰ ssize_t base64_append_ਚੌੜਾਈ(
                ਅੱਖਰ **ਅਗੇਤਰ,
                size_t plen,
                ਅੱਖਰ sep,
                size_t indent,
                ਅਚਲ ਖਾਲੀ *p,
                size_t l,
                size_t width) {

        _cleanup_free_ ਅੱਖਰ *x = ਨਿਰਾ;
        ਅੱਖਰ *t, *s;
        size_t ਸਤਰਾਂ;
        ssize_t len;

        ਪ੍ਰਤਿਜ੍ਞਾ(ਅਗੇਤਰ);
        ਪ੍ਰਤਿਜ੍ਞਾ(*ਅਗੇਤਰ || plen == 0);
        ਪ੍ਰਤਿਜ੍ਞਾ(p || l == 0);

        len = base64mem(p, l, &x);
        ਜੇ (len < 0)
                ਵਾਪਸ len;
        ਜੇ (len == 0)
                ਵਾਪਸ plen;

        ਸਤਰਾਂ = DIV_ਗੇੜਾ_ਉੱਪਰ(len, width);

        ਜੇ (plen >= SSIZE_MAX - 1 - 1 ||
            ਸਤਰਾਂ > (SSIZE_MAX - plen - 1 - 1) / (indent + width + 1))
                ਵਾਪਸ -ENOMEM;

        t = realloc(*ਅਗੇਤਰ, plen + 1 + 1 + (indent + width + 1) * ਸਤਰਾਂ);
        ਜੇ (!t)
                ਵਾਪਸ -ENOMEM;

        s = t + plen;
        ਲਈ (size_t ਸਤਰ = 0; ਸਤਰ < ਸਤਰਾਂ; ਸਤਰ++) {
                size_t act = MIN(width, (size_t) len);

                ਜੇ (ਸਤਰ > 0)
                        sep = '\n';

                ਜੇ (s > t) {
                        *s++ = sep;
                        ਜੇ (sep == '\n')
                                s = mempset(s, ' ', indent);
                }

                s = mempcpy(s, x + width * ਸਤਰ, act);
                len -= act;
        }
        ਪ੍ਰਤਿਜ੍ਞਾ(len == 0);

        *s = '\0';
        *ਅਗੇਤਰ = t;
        ਵਾਪਸ s - t;
}

ssize_t base64_append(
                ਅੱਖਰ **ਅਗੇਤਰ,
                size_t plen,
                ਅਚਲ ਖਾਲੀ *p,
                size_t l,
                size_t indent,
                size_t width) {

        ਜੇ (plen > width / 2 || plen + indent > width)
                /* leave indent on the left, keep last column free */
                ਵਾਪਸ base64_append_ਚੌੜਾਈ(ਅਗੇਤਰ, plen, '\n', indent, p, l, width - indent);
        ਨਹੀਂਤਾਂ
                /* leave plen on the left, keep last column free */
                ਵਾਪਸ base64_append_ਚੌੜਾਈ(ਅਗੇਤਰ, plen, ' ', plen + 1, p, l, width - plen - 1);
}

ਸਥਿਰ ਪੂਰਨ unbase64_ਅਗਲਾ(ਅਚਲ ਅੱਖਰ **p, size_t *l) {
        ਪੂਰਨ ret;

        ਪ੍ਰਤਿਜ੍ਞਾ(p);
        ਪ੍ਰਤਿਜ੍ਞਾ(l);

        /* Find the next non-whitespace character, and decode it. If we find padding, we return it as INT_MAX. We
         * greedily skip all preceding and all following whitespace. */

        ਲਈ (;;) {
                ਜੇ (*l == 0)
                        ਵਾਪਸ -EPIPE;

                ਜੇ (!strchr(WHITESPACE, **p))
                        ਰੋਕੋ;

                /* Skip leading whitespace */
                (*p)++, (*l)--;
        }

        ਜੇ (**p == '=')
                ret = ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ; /* return padding as INT_MAX */
        ਨਹੀਂਤਾਂ {
                ret = unbase64char(**p);
                ਜੇ (ret < 0)
                        ਵਾਪਸ ret;
        }

        ਲਈ (;;) {
                (*p)++, (*l)--;

                ਜੇ (*l == 0)
                        ਰੋਕੋ;
                ਜੇ (!strchr(WHITESPACE, **p))
                        ਰੋਕੋ;

                /* Skip following whitespace */
        }

        ਵਾਪਸ ret;
}

ਪੂਰਨ unbase64mem_ਭਰਿਆ(
                ਅਚਲ ਅੱਖਰ *p,
                size_t l,
                bool secure,
                ਖਾਲੀ **ret_ਅੰਕੜੇ,
                size_t *ret_ਮਾਪ) {

        _cleanup_free_ uint8_t *buf = ਨਿਰਾ;
        ਅਚਲ ਅੱਖਰ *x;
        uint8_t *z;
        size_t len;

        ਪ੍ਰਤਿਜ੍ਞਾ(p || l == 0);

        ਜੇ (l == SIZE_MAX)
                l = ਲੜੀ_ਲੰਬਾਈ(p);

        /* A group of four input bytes needs three output bytes, in case of padding we need to add two or three extra
         * bytes. Note that this calculation is an upper boundary, as we ignore whitespace while decoding */
        len = (l / 4) * 3 + (l % 4 != 0 ? (l % 4) - 1 : 0);

        buf = ਸਮ੍ਰਿਤੀ_ਦਿਓ(len + 1);
        ਜੇ (!buf)
                ਵਾਪਸ -ENOMEM;

        CLEANUP_ERASE_PTR(secure ? &buf : ਨਿਰਾ, len);

        ਲਈ (x = p, z = buf;;) {
                ਪੂਰਨ a, b, c, d; /* a == 00XXXXXX; b == 00YYYYYY; c == 00ZZZZZZ; d == 00WWWWWW */

                a = unbase64_ਅਗਲਾ(&x, &l);
                ਜੇ (a == -EPIPE) /* End of string */
                        ਰੋਕੋ;
                ਜੇ (a < 0)
                        ਵਾਪਸ a;
                ਜੇ (a == ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ) /* Padding is not allowed at the beginning of a 4ch block */
                        ਵਾਪਸ -EINVAL;

                b = unbase64_ਅਗਲਾ(&x, &l);
                ਜੇ (b < 0)
                        ਵਾਪਸ b;
                ਜੇ (b == ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ) /* Padding is not allowed at the second character of a 4ch block either */
                        ਵਾਪਸ -EINVAL;

                c = unbase64_ਅਗਲਾ(&x, &l);
                ਜੇ (c < 0)
                        ਵਾਪਸ c;

                d = unbase64_ਅਗਲਾ(&x, &l);
                ਜੇ (d < 0)
                        ਵਾਪਸ d;

                ਜੇ (c == ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ) { /* Padding at the third character */

                        ਜੇ (d != ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ) /* If the third character is padding, the fourth must be too */
                                ਵਾਪਸ -EINVAL;

                        /* b == 00YY0000 */
                        ਜੇ (b & 15)
                                ਵਾਪਸ -EINVAL;

                        ਜੇ (l > 0) /* Trailing rubbish? */
                                ਵਾਪਸ -ENAMETOOLONG;

                        *(z++) = (uint8_t) a << 2 | (uint8_t) (b >> 4); /* XXXXXXYY */
                        ਰੋਕੋ;
                }

                ਜੇ (d == ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ) {
                        /* c == 00ZZZZ00 */
                        ਜੇ (c & 3)
                                ਵਾਪਸ -EINVAL;

                        ਜੇ (l > 0) /* Trailing rubbish? */
                                ਵਾਪਸ -ENAMETOOLONG;

                        *(z++) = (uint8_t) a << 2 | (uint8_t) b >> 4; /* XXXXXXYY */
                        *(z++) = (uint8_t) b << 4 | (uint8_t) c >> 2; /* YYYYZZZZ */
                        ਰੋਕੋ;
                }

                *(z++) = (uint8_t) a << 2 | (uint8_t) b >> 4; /* XXXXXXYY */
                *(z++) = (uint8_t) b << 4 | (uint8_t) c >> 2; /* YYYYZZZZ */
                *(z++) = (uint8_t) c << 6 | (uint8_t) d;      /* ZZWWWWWW */
        }

        *z = 0;

        ਪ੍ਰਤਿਜ੍ਞਾ((size_t) (z - buf) <= len);

        ਜੇ (ret_ਮਾਪ)
                *ret_ਮਾਪ = (size_t) (z - buf);
        ਜੇ (ret_ਅੰਕੜੇ)
                *ret_ਅੰਕੜੇ = TAKE_PTR(buf);

        ਵਾਪਸ 0;
}

ਖਾਲੀ hexdump(FILE *f, ਅਚਲ ਖਾਲੀ *p, size_t s) {
        ਅਚਲ uint8_t *b = p;
        ਬਿਨਾਨਿਸ਼ਾਨੀ n = 0;

        ਪ੍ਰਤਿਜ੍ਞਾ(b || s == 0);

        ਜੇ (s == SIZE_MAX)
                s = ਲੜੀ_ਲੰਬਾਈ(p);

        ਜੇ (!f)
                f = stdout;

        ਜਦੋਂ (s > 0) {
                size_t i;

                fprintf(f, "%04x  ", n);

                ਲਈ (i = 0; i < 16; i++) {

                        ਜੇ (i >= s)
                                fputs("   ", f);
                        ਨਹੀਂਤਾਂ
                                fprintf(f, "%02x ", b[i]);

                        ਜੇ (i == 7)
                                fputc(' ', f);
                }

                fputc(' ', f);

                ਲਈ (i = 0; i < 16; i++) {

                        ਜੇ (i >= s)
                                fputc(' ', f);
                        ਨਹੀਂਤਾਂ
                                fputc(isprint(b[i]) ? (ਅੱਖਰ) b[i] : '.', f);
                }

                fputc('\n', f);

                ਜੇ (s < 16)
                        ਰੋਕੋ;

                n += 16;
                b += 16;
                s -= 16;
        }
}
