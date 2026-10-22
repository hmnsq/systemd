/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव <ctype.h>
#अन्तर्भाव <stdio.h>

#अन्तर्भाव "alloc-util.h"
#अन्तर्भाव "hexdecoct.h"
#अन्तर्भाव "memory-util.h"
#अन्तर्भाव "string-util.h"

अक्षर octchar(पूर्णांक x) {
        परत '0' + (x & 7);
}

पूर्णांक unoctchar(अक्षर c) {

        जर (c >= '0' && c <= '7')
                परत c - '0';

        परत -EINVAL;
}

अक्षर decchar(पूर्णांक x) {
        परत '0' + (x % 10);
}

पूर्णांक undecchar(अक्षर c) {

        जर (c >= '0' && c <= '9')
                परत c - '0';

        परत -EINVAL;
}

अक्षर hexchar(पूर्णांक x) {
        स्थिरांक अक्षर *table = LOWERCASE_HEXDIGITS;
        परत table[x & 15];
}

पूर्णांक unhexchar(अक्षर c) {

        जर (c >= '0' && c <= '9')
                परत c - '0';

        जर (c >= 'a' && c <= 'f')
                परत c - 'a' + 10;

        जर (c >= 'A' && c <= 'F')
                परत c - 'A' + 10;

        परत -EINVAL;
}

अक्षर* hexmem(स्थिरांक शून्य *p, माप_प्रकार l) {
        स्थिरांक uint8_t *x;
        अक्षर *r, *z;

        प्रतिज्ञा(p || l == 0);

        z = r = new(अक्षर, l * 2 + 1);
        जर (!r)
                परत रिक्त;

        साठी (x = p; x && x < (स्थिरांक uint8_t*) p + l; x++) {
                *(z++) = hexchar(*x >> 4);
                *(z++) = hexchar(*x & 15);
        }

        *z = 0;
        परत r;
}

स्थिर पूर्णांक unhex_next(स्थिरांक अक्षर **p, माप_प्रकार *l) {
        पूर्णांक r;

        प्रतिज्ञा(p);
        प्रतिज्ञा(l);

        /* Find the next non-whitespace character, and decode it. We
         * greedily skip all preceding and all following whitespace. */

        साठी (;;) {
                जर (*l == 0)
                        परत -EPIPE;

                जर (!म_अक्षर(WHITESPACE, **p))
                        थांबा;

                /* Skip leading whitespace */
                (*p)++, (*l)--;
        }

        r = unhexchar(**p);
        जर (r < 0)
                परत r;

        साठी (;;) {
                (*p)++, (*l)--;

                जर (*l == 0 || !म_अक्षर(WHITESPACE, **p))
                        थांबा;

                /* Skip following whitespace */
        }

        परत r;
}

पूर्णांक unhexmem_full(
                स्थिरांक अक्षर *p,
                माप_प्रकार l,
                bool secure,
                शून्य **ret_data,
                माप_प्रकार *ret_size) {

        _cleanup_free_ uint8_t *buf = रिक्त;
        माप_प्रकार buf_size;
        स्थिरांक अक्षर *x;
        uint8_t *z;

        प्रतिज्ञा(p || l == 0);

        जर (l == SIZE_MAX)
                l = म_माप(p);

        /* Note that the calculation of memory size is an upper boundary, as we ignore whitespace while decoding */
        buf_size = (l + 1) / 2 + 1;
        buf = दो_स्मृति(buf_size);
        जर (!buf)
                परत -ENOMEM;

        CLEANUP_ERASE_PTR(secure ? &buf : रिक्त, buf_size);

        साठी (x = p, z = buf;;) {
                पूर्णांक a, b;

                a = unhex_next(&x, &l);
                जर (a == -EPIPE) /* End of string */
                        थांबा;
                जर (a < 0)
                        परत a;

                b = unhex_next(&x, &l);
                जर (b < 0)
                        परत b;

                *(z++) = (uint8_t) a << 4 | (uint8_t) b;
        }

        *z = 0;

        जर (ret_size)
                *ret_size = (माप_प्रकार) (z - buf);
        जर (ret_data)
                *ret_data = TAKE_PTR(buf);

        परत 0;
}

/* https://tools.ietf.org/html/rfc4648#section-6
 * Notice that base32hex differs from base32 in the alphabet it uses.
 * The distinction is that the base32hex representation preserves the
 * order of the underlying data when compared as bytestrings, this is
 * useful when representing NSEC3 hashes, as one can then verify the
 * order of hashes directly from their representation. */
अक्षर base32hexchar(पूर्णांक x) {
        स्थिरांक अक्षर *table = DIGITS "ABCDEFGHIJKLMNOPQRSTUV";
        परत table[x & 31];
}

पूर्णांक unbase32hexchar(अक्षर c) {
        अचिन्हित offset;

        जर (c >= '0' && c <= '9')
                परत c - '0';

        offset = '9' - '0' + 1;

        जर (c >= 'A' && c <= 'V')
                परत c - 'A' + offset;

        परत -EINVAL;
}

अक्षर* base32hexmem(स्थिरांक शून्य *p, माप_प्रकार l, bool padding) {
        अक्षर *r, *z;
        स्थिरांक uint8_t *x;
        माप_प्रकार len;

        प्रतिज्ञा(p || l == 0);

        जर (padding)
                /* five input bytes makes eight output bytes, padding is added so we must round up */
                len = 8 * (l + 4) / 5;
        अन्यथा {
                /* same, but round down as there is no padding */
                len = 8 * l / 5;

                निवड (l % 5) {
                स्थिती 4:
                        len += 7;
                        थांबा;
                स्थिती 3:
                        len += 5;
                        थांबा;
                स्थिती 2:
                        len += 4;
                        थांबा;
                स्थिती 1:
                        len += 2;
                        थांबा;
                }
        }

        z = r = दो_स्मृति(len + 1);
        जर (!r)
                परत रिक्त;

        साठी (x = p; x < (स्थिरांक uint8_t*) p + (l / 5) * 5; x += 5) {
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

        निवड (l % 5) {
        स्थिती 4:
                *(z++) = base32hexchar(x[0] >> 3);                    /* 000XXXXX */
                *(z++) = base32hexchar((x[0] & 7) << 2 | x[1] >> 6);  /* 000XXXYY */
                *(z++) = base32hexchar((x[1] & 63) >> 1);             /* 000YYYYY */
                *(z++) = base32hexchar((x[1] & 1) << 4 | x[2] >> 4);   /* 000YZZZZ */
                *(z++) = base32hexchar((x[2] & 15) << 1 | x[3] >> 7); /* 000ZZZZQ */
                *(z++) = base32hexchar((x[3] & 127) >> 2);            /* 000QQQQQ */
                *(z++) = base32hexchar((x[3] & 3) << 3);              /* 000QQ000 */
                जर (padding)
                        *(z++) = '=';

                थांबा;

        स्थिती 3:
                *(z++) = base32hexchar(x[0] >> 3);                   /* 000XXXXX */
                *(z++) = base32hexchar((x[0] & 7) << 2 | x[1] >> 6); /* 000XXXYY */
                *(z++) = base32hexchar((x[1] & 63) >> 1);            /* 000YYYYY */
                *(z++) = base32hexchar((x[1] & 1) << 4 | x[2] >> 4); /* 000YZZZZ */
                *(z++) = base32hexchar((x[2] & 15) << 1);            /* 000ZZZZ0 */
                जर (padding) {
                        *(z++) = '=';
                        *(z++) = '=';
                        *(z++) = '=';
                }

                थांबा;

        स्थिती 2:
                *(z++) = base32hexchar(x[0] >> 3);                   /* 000XXXXX */
                *(z++) = base32hexchar((x[0] & 7) << 2 | x[1] >> 6); /* 000XXXYY */
                *(z++) = base32hexchar((x[1] & 63) >> 1);            /* 000YYYYY */
                *(z++) = base32hexchar((x[1] & 1) << 4);             /* 000Y0000 */
                जर (padding) {
                        *(z++) = '=';
                        *(z++) = '=';
                        *(z++) = '=';
                        *(z++) = '=';
                }

                थांबा;

        स्थिती 1:
                *(z++) = base32hexchar(x[0] >> 3);       /* 000XXXXX */
                *(z++) = base32hexchar((x[0] & 7) << 2); /* 000XXX00 */
                जर (padding) {
                        *(z++) = '=';
                        *(z++) = '=';
                        *(z++) = '=';
                        *(z++) = '=';
                        *(z++) = '=';
                        *(z++) = '=';
                }

                थांबा;
        }

        *z = 0;
        परत r;
}

पूर्णांक unbase32hexmem(स्थिरांक अक्षर *p, माप_प्रकार l, bool padding, शून्य **mem, माप_प्रकार *_len) {
        _cleanup_free_ uint8_t *r = रिक्त;
        पूर्णांक a, b, c, d, e, f, g, h;
        uint8_t *z;
        स्थिरांक अक्षर *x;
        माप_प्रकार len;
        अचिन्हित pad = 0;

        प्रतिज्ञा(p || l == 0);
        प्रतिज्ञा(mem);
        प्रतिज्ञा(_len);

        जर (l == SIZE_MAX)
                l = म_माप(p);

        /* padding ensures any base32hex input has input divisible by 8 */
        जर (padding && l % 8 != 0)
                परत -EINVAL;

        जर (padding) {
                /* strip the padding */
                पर्यंत (l > 0 && p[l - 1] == '=' && pad < 7) {
                        pad++;
                        l--;
                }
        }

        /* a group of eight input bytes needs five output bytes, in case of
         * padding we need to add some extra bytes */
        len = (l / 8) * 5;

        निवड (l % 8) {
        स्थिती 7:
                len += 4;
                थांबा;
        स्थिती 5:
                len += 3;
                थांबा;
        स्थिती 4:
                len += 2;
                थांबा;
        स्थिती 2:
                len += 1;
                थांबा;
        स्थिती 0:
                थांबा;
        पूर्वनिर्धारित:
                परत -EINVAL;
        }

        z = r = दो_स्मृति(len + 1);
        जर (!r)
                परत -ENOMEM;

        साठी (x = p; x < p + (l / 8) * 8; x += 8) {
                /* a == 000XXXXX; b == 000YYYYY; c == 000ZZZZZ; d == 000WWWWW
                 * e == 000SSSSS; f == 000QQQQQ; g == 000VVVVV; h == 000RRRRR */
                a = unbase32hexchar(x[0]);
                जर (a < 0)
                        परत -EINVAL;

                b = unbase32hexchar(x[1]);
                जर (b < 0)
                        परत -EINVAL;

                c = unbase32hexchar(x[2]);
                जर (c < 0)
                        परत -EINVAL;

                d = unbase32hexchar(x[3]);
                जर (d < 0)
                        परत -EINVAL;

                e = unbase32hexchar(x[4]);
                जर (e < 0)
                        परत -EINVAL;

                f = unbase32hexchar(x[5]);
                जर (f < 0)
                        परत -EINVAL;

                g = unbase32hexchar(x[6]);
                जर (g < 0)
                        परत -EINVAL;

                h = unbase32hexchar(x[7]);
                जर (h < 0)
                        परत -EINVAL;

                *(z++) = (uint8_t) a << 3 | (uint8_t) b >> 2;                    /* XXXXXYYY */
                *(z++) = (uint8_t) b << 6 | (uint8_t) c << 1 | (uint8_t) d >> 4; /* YYZZZZZW */
                *(z++) = (uint8_t) d << 4 | (uint8_t) e >> 1;                    /* WWWWSSSS */
                *(z++) = (uint8_t) e << 7 | (uint8_t) f << 2 | (uint8_t) g >> 3; /* SQQQQQVV */
                *(z++) = (uint8_t) g << 5 | (uint8_t) h;                         /* VVVRRRRR */
        }

        निवड (l % 8) {
        स्थिती 7:
                a = unbase32hexchar(x[0]);
                जर (a < 0)
                        परत -EINVAL;

                b = unbase32hexchar(x[1]);
                जर (b < 0)
                        परत -EINVAL;

                c = unbase32hexchar(x[2]);
                जर (c < 0)
                        परत -EINVAL;

                d = unbase32hexchar(x[3]);
                जर (d < 0)
                        परत -EINVAL;

                e = unbase32hexchar(x[4]);
                जर (e < 0)
                        परत -EINVAL;

                f = unbase32hexchar(x[5]);
                जर (f < 0)
                        परत -EINVAL;

                g = unbase32hexchar(x[6]);
                जर (g < 0)
                        परत -EINVAL;

                /* g == 000VV000 */
                जर (g & 7)
                        परत -EINVAL;

                *(z++) = (uint8_t) a << 3 | (uint8_t) b >> 2;                    /* XXXXXYYY */
                *(z++) = (uint8_t) b << 6 | (uint8_t) c << 1 | (uint8_t) d >> 4; /* YYZZZZZW */
                *(z++) = (uint8_t) d << 4 | (uint8_t) e >> 1;                    /* WWWWSSSS */
                *(z++) = (uint8_t) e << 7 | (uint8_t) f << 2 | (uint8_t) g >> 3; /* SQQQQQVV */

                थांबा;
        स्थिती 5:
                a = unbase32hexchar(x[0]);
                जर (a < 0)
                        परत -EINVAL;

                b = unbase32hexchar(x[1]);
                जर (b < 0)
                        परत -EINVAL;

                c = unbase32hexchar(x[2]);
                जर (c < 0)
                        परत -EINVAL;

                d = unbase32hexchar(x[3]);
                जर (d < 0)
                        परत -EINVAL;

                e = unbase32hexchar(x[4]);
                जर (e < 0)
                        परत -EINVAL;

                /* e == 000SSSS0 */
                जर (e & 1)
                        परत -EINVAL;

                *(z++) = (uint8_t) a << 3 | (uint8_t) b >> 2;                    /* XXXXXYYY */
                *(z++) = (uint8_t) b << 6 | (uint8_t) c << 1 | (uint8_t) d >> 4; /* YYZZZZZW */
                *(z++) = (uint8_t) d << 4 | (uint8_t) e >> 1;                    /* WWWWSSSS */

                थांबा;
        स्थिती 4:
                a = unbase32hexchar(x[0]);
                जर (a < 0)
                        परत -EINVAL;

                b = unbase32hexchar(x[1]);
                जर (b < 0)
                        परत -EINVAL;

                c = unbase32hexchar(x[2]);
                जर (c < 0)
                        परत -EINVAL;

                d = unbase32hexchar(x[3]);
                जर (d < 0)
                        परत -EINVAL;

                /* d == 000W0000 */
                जर (d & 15)
                        परत -EINVAL;

                *(z++) = (uint8_t) a << 3 | (uint8_t) b >> 2;                    /* XXXXXYYY */
                *(z++) = (uint8_t) b << 6 | (uint8_t) c << 1 | (uint8_t) d >> 4; /* YYZZZZZW */

                थांबा;
        स्थिती 2:
                a = unbase32hexchar(x[0]);
                जर (a < 0)
                        परत -EINVAL;

                b = unbase32hexchar(x[1]);
                जर (b < 0)
                        परत -EINVAL;

                /* b == 000YYY00 */
                जर (b & 3)
                        परत -EINVAL;

                *(z++) = (uint8_t) a << 3 | (uint8_t) b >> 2; /* XXXXXYYY */

                थांबा;
        स्थिती 0:
                थांबा;
        पूर्वनिर्धारित:
                परत -EINVAL;
        }

        *z = 0;

        *mem = TAKE_PTR(r);
        *_len = len;

        परत 0;
}

/* https://tools.ietf.org/html/rfc4648#section-4 */
अक्षर base64char(पूर्णांक x) {
        स्थिरांक अक्षर *table = UPPERCASE_LETTERS LOWERCASE_LETTERS DIGITS "+/";
        परत table[x & 63];
}

/* This is almost base64char(), but not entirely, as it uses the "url and filename safe" alphabet,
 * since we don't want "/" appear in interface names (since interfaces appear in sysfs as filenames).
 * See section #5 of RFC 4648. */
अक्षर urlsafe_base64char(पूर्णांक x) {
        स्थिरांक अक्षर *table = UPPERCASE_LETTERS LOWERCASE_LETTERS DIGITS "-_";
        परत table[x & 63];
}

पूर्णांक unbase64char(अक्षर c) {
        अचिन्हित offset;

        जर (c >= 'A' && c <= 'Z')
                परत c - 'A';

        offset = 'Z' - 'A' + 1;

        जर (c >= 'a' && c <= 'z')
                परत c - 'a' + offset;

        offset += 'z' - 'a' + 1;

        जर (c >= '0' && c <= '9')
                परत c - '0' + offset;

        offset += '9' - '0' + 1;

        जर (IN_SET(c, '+', '-')) /* Support both the regular and the URL safe character set (see above) */
                परत offset;

        offset++;

        जर (IN_SET(c, '/', '_')) /* ditto */
                परत offset;

        परत -EINVAL;
}

स्थिर शून्य maybe_ओळ_break(अक्षर **x, अक्षर *start, माप_प्रकार ओळ_break) {
        माप_प्रकार n;

        प्रतिज्ञा(x);
        प्रतिज्ञा(*x);
        प्रतिज्ञा(start);
        प्रतिज्ञा(*x >= start);

        जर (ओळ_break == SIZE_MAX)
                परत;

        n = *x - start;

        जर (n % (ओळ_break + 1) == ओळ_break)
                *((*x)++) = '\n';
}

ssize_t base64mem_full(
                स्थिरांक शून्य *p,
                माप_प्रकार l,
                माप_प्रकार ओळ_break,
                अक्षर **ret) {

        स्थिरांक uint8_t *x;
        अक्षर *b, *z;
        माप_प्रकार m;

        प्रतिज्ञा(p || l == 0);
        प्रतिज्ञा(ओळ_break > 0);
        प्रतिज्ञा(ret);

        /* three input bytes makes four output bytes, padding is added so we must round up */
        m = 4 * (l + 2) / 3 + 1;
        जर (ओळ_break != SIZE_MAX)
                m += m / ओळ_break;

        z = b = दो_स्मृति(m);
        जर (!b)
                परत -ENOMEM;

        साठी (x = p; x && x < (स्थिरांक uint8_t*) p + (l / 3) * 3; x += 3) {
                /* x[0] == XXXXXXXX; x[1] == YYYYYYYY; x[2] == ZZZZZZZZ */
                maybe_ओळ_break(&z, b, ओळ_break);
                *(z++) = base64char(x[0] >> 2);                    /* 00XXXXXX */
                maybe_ओळ_break(&z, b, ओळ_break);
                *(z++) = base64char((x[0] & 3) << 4 | x[1] >> 4);  /* 00XXYYYY */
                maybe_ओळ_break(&z, b, ओळ_break);
                *(z++) = base64char((x[1] & 15) << 2 | x[2] >> 6); /* 00YYYYZZ */
                maybe_ओळ_break(&z, b, ओळ_break);
                *(z++) = base64char(x[2] & 63);                    /* 00ZZZZZZ */
        }

        निवड (l % 3) {
        स्थिती 2:
                maybe_ओळ_break(&z, b, ओळ_break);
                *(z++) = base64char(x[0] >> 2);                   /* 00XXXXXX */
                maybe_ओळ_break(&z, b, ओळ_break);
                *(z++) = base64char((x[0] & 3) << 4 | x[1] >> 4); /* 00XXYYYY */
                maybe_ओळ_break(&z, b, ओळ_break);
                *(z++) = base64char((x[1] & 15) << 2);            /* 00YYYY00 */
                maybe_ओळ_break(&z, b, ओळ_break);
                *(z++) = '=';
                थांबा;

        स्थिती 1:
                maybe_ओळ_break(&z, b, ओळ_break);
                *(z++) = base64char(x[0] >> 2);        /* 00XXXXXX */
                maybe_ओळ_break(&z, b, ओळ_break);
                *(z++) = base64char((x[0] & 3) << 4);  /* 00XX0000 */
                maybe_ओळ_break(&z, b, ओळ_break);
                *(z++) = '=';
                maybe_ओळ_break(&z, b, ओळ_break);
                *(z++) = '=';
                थांबा;
        }

        *z = 0;
        *ret = b;

        प्रतिज्ञा(z >= b); /* Let static analyzers know that the answer is non-negative. */
        परत z - b;
}

स्थिर ssize_t base64_append_width(
                अक्षर **prefix,
                माप_प्रकार plen,
                अक्षर sep,
                माप_प्रकार indent,
                स्थिरांक शून्य *p,
                माप_प्रकार l,
                माप_प्रकार width) {

        _cleanup_free_ अक्षर *x = रिक्त;
        अक्षर *t, *s;
        माप_प्रकार ओळी;
        ssize_t len;

        प्रतिज्ञा(prefix);
        प्रतिज्ञा(*prefix || plen == 0);
        प्रतिज्ञा(p || l == 0);

        len = base64mem(p, l, &x);
        जर (len < 0)
                परत len;
        जर (len == 0)
                परत plen;

        ओळी = DIV_ROUND_UP(len, width);

        जर (plen >= SSIZE_MAX - 1 - 1 ||
            ओळी > (SSIZE_MAX - plen - 1 - 1) / (indent + width + 1))
                परत -ENOMEM;

        t = पुनः_स्मृति(*prefix, plen + 1 + 1 + (indent + width + 1) * ओळी);
        जर (!t)
                परत -ENOMEM;

        s = t + plen;
        साठी (माप_प्रकार ओळ = 0; ओळ < ओळी; ओळ++) {
                माप_प्रकार act = MIN(width, (माप_प्रकार) len);

                जर (ओळ > 0)
                        sep = '\n';

                जर (s > t) {
                        *s++ = sep;
                        जर (sep == '\n')
                                s = mempset(s, ' ', indent);
                }

                s = mempcpy(s, x + width * ओळ, act);
                len -= act;
        }
        प्रतिज्ञा(len == 0);

        *s = '\0';
        *prefix = t;
        परत s - t;
}

ssize_t base64_append(
                अक्षर **prefix,
                माप_प्रकार plen,
                स्थिरांक शून्य *p,
                माप_प्रकार l,
                माप_प्रकार indent,
                माप_प्रकार width) {

        जर (plen > width / 2 || plen + indent > width)
                /* leave indent on the left, keep last column free */
                परत base64_append_width(prefix, plen, '\n', indent, p, l, width - indent);
        अन्यथा
                /* leave plen on the left, keep last column free */
                परत base64_append_width(prefix, plen, ' ', plen + 1, p, l, width - plen - 1);
}

स्थिर पूर्णांक unbase64_next(स्थिरांक अक्षर **p, माप_प्रकार *l) {
        पूर्णांक ret;

        प्रतिज्ञा(p);
        प्रतिज्ञा(l);

        /* Find the next non-whitespace character, and decode it. If we find padding, we return it as INT_MAX. We
         * greedily skip all preceding and all following whitespace. */

        साठी (;;) {
                जर (*l == 0)
                        परत -EPIPE;

                जर (!म_अक्षर(WHITESPACE, **p))
                        थांबा;

                /* Skip leading whitespace */
                (*p)++, (*l)--;
        }

        जर (**p == '=')
                ret = पूर्णांक_उच्च; /* return padding as INT_MAX */
        अन्यथा {
                ret = unbase64char(**p);
                जर (ret < 0)
                        परत ret;
        }

        साठी (;;) {
                (*p)++, (*l)--;

                जर (*l == 0)
                        थांबा;
                जर (!म_अक्षर(WHITESPACE, **p))
                        थांबा;

                /* Skip following whitespace */
        }

        परत ret;
}

पूर्णांक unbase64mem_full(
                स्थिरांक अक्षर *p,
                माप_प्रकार l,
                bool secure,
                शून्य **ret_data,
                माप_प्रकार *ret_size) {

        _cleanup_free_ uint8_t *buf = रिक्त;
        स्थिरांक अक्षर *x;
        uint8_t *z;
        माप_प्रकार len;

        प्रतिज्ञा(p || l == 0);

        जर (l == SIZE_MAX)
                l = म_माप(p);

        /* A group of four input bytes needs three output bytes, in case of padding we need to add two or three extra
         * bytes. Note that this calculation is an upper boundary, as we ignore whitespace while decoding */
        len = (l / 4) * 3 + (l % 4 != 0 ? (l % 4) - 1 : 0);

        buf = दो_स्मृति(len + 1);
        जर (!buf)
                परत -ENOMEM;

        CLEANUP_ERASE_PTR(secure ? &buf : रिक्त, len);

        साठी (x = p, z = buf;;) {
                पूर्णांक a, b, c, d; /* a == 00XXXXXX; b == 00YYYYYY; c == 00ZZZZZZ; d == 00WWWWWW */

                a = unbase64_next(&x, &l);
                जर (a == -EPIPE) /* End of string */
                        थांबा;
                जर (a < 0)
                        परत a;
                जर (a == पूर्णांक_उच्च) /* Padding is not allowed at the beginning of a 4ch block */
                        परत -EINVAL;

                b = unbase64_next(&x, &l);
                जर (b < 0)
                        परत b;
                जर (b == पूर्णांक_उच्च) /* Padding is not allowed at the second character of a 4ch block either */
                        परत -EINVAL;

                c = unbase64_next(&x, &l);
                जर (c < 0)
                        परत c;

                d = unbase64_next(&x, &l);
                जर (d < 0)
                        परत d;

                जर (c == पूर्णांक_उच्च) { /* Padding at the third character */

                        जर (d != पूर्णांक_उच्च) /* If the third character is padding, the fourth must be too */
                                परत -EINVAL;

                        /* b == 00YY0000 */
                        जर (b & 15)
                                परत -EINVAL;

                        जर (l > 0) /* Trailing rubbish? */
                                परत -ENAMETOOLONG;

                        *(z++) = (uint8_t) a << 2 | (uint8_t) (b >> 4); /* XXXXXXYY */
                        थांबा;
                }

                जर (d == पूर्णांक_उच्च) {
                        /* c == 00ZZZZ00 */
                        जर (c & 3)
                                परत -EINVAL;

                        जर (l > 0) /* Trailing rubbish? */
                                परत -ENAMETOOLONG;

                        *(z++) = (uint8_t) a << 2 | (uint8_t) b >> 4; /* XXXXXXYY */
                        *(z++) = (uint8_t) b << 4 | (uint8_t) c >> 2; /* YYYYZZZZ */
                        थांबा;
                }

                *(z++) = (uint8_t) a << 2 | (uint8_t) b >> 4; /* XXXXXXYY */
                *(z++) = (uint8_t) b << 4 | (uint8_t) c >> 2; /* YYYYZZZZ */
                *(z++) = (uint8_t) c << 6 | (uint8_t) d;      /* ZZWWWWWW */
        }

        *z = 0;

        प्रतिज्ञा((माप_प्रकार) (z - buf) <= len);

        जर (ret_size)
                *ret_size = (माप_प्रकार) (z - buf);
        जर (ret_data)
                *ret_data = TAKE_PTR(buf);

        परत 0;
}

शून्य hexdump(खाता *f, स्थिरांक शून्य *p, माप_प्रकार s) {
        स्थिरांक uint8_t *b = p;
        अचिन्हित n = 0;

        प्रतिज्ञा(b || s == 0);

        जर (s == SIZE_MAX)
                s = म_माप(p);

        जर (!f)
                f = मानक_निकास;

        पर्यंत (s > 0) {
                माप_प्रकार i;

                ख_लिखो(f, "%04x  ", n);

                साठी (i = 0; i < 16; i++) {

                        जर (i >= s)
                                ख_माला_दो("   ", f);
                        अन्यथा
                                ख_लिखो(f, "%02x ", b[i]);

                        जर (i == 7)
                                ख_अक्षर_दो(' ', f);
                }

                ख_अक्षर_दो(' ', f);

                साठी (i = 0; i < 16; i++) {

                        जर (i >= s)
                                ख_अक्षर_दो(' ', f);
                        अन्यथा
                                ख_अक्षर_दो(है_छाप(b[i]) ? (अक्षर) b[i] : '.', f);
                }

                ख_अक्षर_दो('\n', f);

                जर (s < 16)
                        थांबा;

                n += 16;
                b += 16;
                s -= 16;
        }
}
