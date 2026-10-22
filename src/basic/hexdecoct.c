/* SPDX-License-Identifier: LGPL-2.1-or-later */

#समावेश <ctype.h>
#समावेश <stdio.h>

#समावेश "alloc-util.h"
#समावेश "hexdecoct.h"
#समावेश "memory-util.h"
#समावेश "string-util.h"

अक्षर octchar(पूर्णांक x) {
        वापस '0' + (x & 7);
}

पूर्णांक unoctchar(अक्षर c) {

        अगर (c >= '0' && c <= '7')
                वापस c - '0';

        वापस -EINVAL;
}

अक्षर decchar(पूर्णांक x) {
        वापस '0' + (x % 10);
}

पूर्णांक undecchar(अक्षर c) {

        अगर (c >= '0' && c <= '9')
                वापस c - '0';

        वापस -EINVAL;
}

अक्षर hexchar(पूर्णांक x) {
        स्थिर अक्षर *table = LOWERCASE_HEXDIGITS;
        वापस table[x & 15];
}

पूर्णांक unhexchar(अक्षर c) {

        अगर (c >= '0' && c <= '9')
                वापस c - '0';

        अगर (c >= 'a' && c <= 'f')
                वापस c - 'a' + 10;

        अगर (c >= 'A' && c <= 'F')
                वापस c - 'A' + 10;

        वापस -EINVAL;
}

अक्षर* hexmem(स्थिर व्योम *p, माप_प्रकार l) {
        स्थिर uint8_t *x;
        अक्षर *r, *z;

        निश्चित(p || l == 0);

        z = r = new(अक्षर, l * 2 + 1);
        अगर (!r)
                वापस शून्य;

        क्रम (x = p; x && x < (स्थिर uint8_t*) p + l; x++) {
                *(z++) = hexchar(*x >> 4);
                *(z++) = hexchar(*x & 15);
        }

        *z = 0;
        वापस r;
}

अटल पूर्णांक unhex_अग्र(स्थिर अक्षर **p, माप_प्रकार *l) {
        पूर्णांक r;

        निश्चित(p);
        निश्चित(l);

        /* Find the next non-whitespace character, and decode it. We
         * greedily skip all preceding and all following whitespace. */

        क्रम (;;) {
                अगर (*l == 0)
                        वापस -EPIPE;

                अगर (!म_अक्षर(WHITESPACE, **p))
                        अवरोध;

                /* Skip leading whitespace */
                (*p)++, (*l)--;
        }

        r = unhexchar(**p);
        अगर (r < 0)
                वापस r;

        क्रम (;;) {
                (*p)++, (*l)--;

                अगर (*l == 0 || !म_अक्षर(WHITESPACE, **p))
                        अवरोध;

                /* Skip following whitespace */
        }

        वापस r;
}

पूर्णांक unhexmem_full(
                स्थिर अक्षर *p,
                माप_प्रकार l,
                bool secure,
                व्योम **ret_तथ्य,
                माप_प्रकार *ret_आकार) {

        _cleanup_free_ uint8_t *buf = शून्य;
        माप_प्रकार buf_size;
        स्थिर अक्षर *x;
        uint8_t *z;

        निश्चित(p || l == 0);

        अगर (l == SIZE_MAX)
                l = म_माप(p);

        /* Note that the calculation of memory size is an upper boundary, as we ignore whitespace while decoding */
        buf_size = (l + 1) / 2 + 1;
        buf = दो_स्मृति(buf_size);
        अगर (!buf)
                वापस -ENOMEM;

        CLEANUP_ERASE_PTR(secure ? &buf : शून्य, buf_size);

        क्रम (x = p, z = buf;;) {
                पूर्णांक a, b;

                a = unhex_अग्र(&x, &l);
                अगर (a == -EPIPE) /* End of string */
                        अवरोध;
                अगर (a < 0)
                        वापस a;

                b = unhex_अग्र(&x, &l);
                अगर (b < 0)
                        वापस b;

                *(z++) = (uint8_t) a << 4 | (uint8_t) b;
        }

        *z = 0;

        अगर (ret_आकार)
                *ret_आकार = (माप_प्रकार) (z - buf);
        अगर (ret_तथ्य)
                *ret_तथ्य = TAKE_PTR(buf);

        वापस 0;
}

/* https://tools.ietf.org/html/rfc4648#section-6
 * Notice that base32hex differs from base32 in the alphabet it uses.
 * The distinction is that the base32hex representation preserves the
 * order of the underlying data when compared as bytestrings, this is
 * useful when representing NSEC3 hashes, as one can then verify the
 * order of hashes directly from their representation. */
अक्षर base32hexchar(पूर्णांक x) {
        स्थिर अक्षर *table = DIGITS "ABCDEFGHIJKLMNOPQRSTUV";
        वापस table[x & 31];
}

पूर्णांक unbase32hexchar(अक्षर c) {
        अचिन्हित offset;

        अगर (c >= '0' && c <= '9')
                वापस c - '0';

        offset = '9' - '0' + 1;

        अगर (c >= 'A' && c <= 'V')
                वापस c - 'A' + offset;

        वापस -EINVAL;
}

अक्षर* base32hexmem(स्थिर व्योम *p, माप_प्रकार l, bool padding) {
        अक्षर *r, *z;
        स्थिर uint8_t *x;
        माप_प्रकार len;

        निश्चित(p || l == 0);

        अगर (padding)
                /* five input bytes makes eight output bytes, padding is added so we must round up */
                len = 8 * (l + 4) / 5;
        अन्यथा {
                /* same, but round down as there is no padding */
                len = 8 * l / 5;

                चयन (l % 5) {
                हाल 4:
                        len += 7;
                        अवरोध;
                हाल 3:
                        len += 5;
                        अवरोध;
                हाल 2:
                        len += 4;
                        अवरोध;
                हाल 1:
                        len += 2;
                        अवरोध;
                }
        }

        z = r = दो_स्मृति(len + 1);
        अगर (!r)
                वापस शून्य;

        क्रम (x = p; x < (स्थिर uint8_t*) p + (l / 5) * 5; x += 5) {
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

        चयन (l % 5) {
        हाल 4:
                *(z++) = base32hexchar(x[0] >> 3);                    /* 000XXXXX */
                *(z++) = base32hexchar((x[0] & 7) << 2 | x[1] >> 6);  /* 000XXXYY */
                *(z++) = base32hexchar((x[1] & 63) >> 1);             /* 000YYYYY */
                *(z++) = base32hexchar((x[1] & 1) << 4 | x[2] >> 4);   /* 000YZZZZ */
                *(z++) = base32hexchar((x[2] & 15) << 1 | x[3] >> 7); /* 000ZZZZQ */
                *(z++) = base32hexchar((x[3] & 127) >> 2);            /* 000QQQQQ */
                *(z++) = base32hexchar((x[3] & 3) << 3);              /* 000QQ000 */
                अगर (padding)
                        *(z++) = '=';

                अवरोध;

        हाल 3:
                *(z++) = base32hexchar(x[0] >> 3);                   /* 000XXXXX */
                *(z++) = base32hexchar((x[0] & 7) << 2 | x[1] >> 6); /* 000XXXYY */
                *(z++) = base32hexchar((x[1] & 63) >> 1);            /* 000YYYYY */
                *(z++) = base32hexchar((x[1] & 1) << 4 | x[2] >> 4); /* 000YZZZZ */
                *(z++) = base32hexchar((x[2] & 15) << 1);            /* 000ZZZZ0 */
                अगर (padding) {
                        *(z++) = '=';
                        *(z++) = '=';
                        *(z++) = '=';
                }

                अवरोध;

        हाल 2:
                *(z++) = base32hexchar(x[0] >> 3);                   /* 000XXXXX */
                *(z++) = base32hexchar((x[0] & 7) << 2 | x[1] >> 6); /* 000XXXYY */
                *(z++) = base32hexchar((x[1] & 63) >> 1);            /* 000YYYYY */
                *(z++) = base32hexchar((x[1] & 1) << 4);             /* 000Y0000 */
                अगर (padding) {
                        *(z++) = '=';
                        *(z++) = '=';
                        *(z++) = '=';
                        *(z++) = '=';
                }

                अवरोध;

        हाल 1:
                *(z++) = base32hexchar(x[0] >> 3);       /* 000XXXXX */
                *(z++) = base32hexchar((x[0] & 7) << 2); /* 000XXX00 */
                अगर (padding) {
                        *(z++) = '=';
                        *(z++) = '=';
                        *(z++) = '=';
                        *(z++) = '=';
                        *(z++) = '=';
                        *(z++) = '=';
                }

                अवरोध;
        }

        *z = 0;
        वापस r;
}

पूर्णांक unbase32hexmem(स्थिर अक्षर *p, माप_प्रकार l, bool padding, व्योम **mem, माप_प्रकार *_len) {
        _cleanup_free_ uint8_t *r = शून्य;
        पूर्णांक a, b, c, d, e, f, g, h;
        uint8_t *z;
        स्थिर अक्षर *x;
        माप_प्रकार len;
        अचिन्हित pad = 0;

        निश्चित(p || l == 0);
        निश्चित(mem);
        निश्चित(_len);

        अगर (l == SIZE_MAX)
                l = म_माप(p);

        /* padding ensures any base32hex input has input divisible by 8 */
        अगर (padding && l % 8 != 0)
                वापस -EINVAL;

        अगर (padding) {
                /* strip the padding */
                जबतक (l > 0 && p[l - 1] == '=' && pad < 7) {
                        pad++;
                        l--;
                }
        }

        /* a group of eight input bytes needs five output bytes, in case of
         * padding we need to add some extra bytes */
        len = (l / 8) * 5;

        चयन (l % 8) {
        हाल 7:
                len += 4;
                अवरोध;
        हाल 5:
                len += 3;
                अवरोध;
        हाल 4:
                len += 2;
                अवरोध;
        हाल 2:
                len += 1;
                अवरोध;
        हाल 0:
                अवरोध;
        शेष:
                वापस -EINVAL;
        }

        z = r = दो_स्मृति(len + 1);
        अगर (!r)
                वापस -ENOMEM;

        क्रम (x = p; x < p + (l / 8) * 8; x += 8) {
                /* a == 000XXXXX; b == 000YYYYY; c == 000ZZZZZ; d == 000WWWWW
                 * e == 000SSSSS; f == 000QQQQQ; g == 000VVVVV; h == 000RRRRR */
                a = unbase32hexchar(x[0]);
                अगर (a < 0)
                        वापस -EINVAL;

                b = unbase32hexchar(x[1]);
                अगर (b < 0)
                        वापस -EINVAL;

                c = unbase32hexchar(x[2]);
                अगर (c < 0)
                        वापस -EINVAL;

                d = unbase32hexchar(x[3]);
                अगर (d < 0)
                        वापस -EINVAL;

                e = unbase32hexchar(x[4]);
                अगर (e < 0)
                        वापस -EINVAL;

                f = unbase32hexchar(x[5]);
                अगर (f < 0)
                        वापस -EINVAL;

                g = unbase32hexchar(x[6]);
                अगर (g < 0)
                        वापस -EINVAL;

                h = unbase32hexchar(x[7]);
                अगर (h < 0)
                        वापस -EINVAL;

                *(z++) = (uint8_t) a << 3 | (uint8_t) b >> 2;                    /* XXXXXYYY */
                *(z++) = (uint8_t) b << 6 | (uint8_t) c << 1 | (uint8_t) d >> 4; /* YYZZZZZW */
                *(z++) = (uint8_t) d << 4 | (uint8_t) e >> 1;                    /* WWWWSSSS */
                *(z++) = (uint8_t) e << 7 | (uint8_t) f << 2 | (uint8_t) g >> 3; /* SQQQQQVV */
                *(z++) = (uint8_t) g << 5 | (uint8_t) h;                         /* VVVRRRRR */
        }

        चयन (l % 8) {
        हाल 7:
                a = unbase32hexchar(x[0]);
                अगर (a < 0)
                        वापस -EINVAL;

                b = unbase32hexchar(x[1]);
                अगर (b < 0)
                        वापस -EINVAL;

                c = unbase32hexchar(x[2]);
                अगर (c < 0)
                        वापस -EINVAL;

                d = unbase32hexchar(x[3]);
                अगर (d < 0)
                        वापस -EINVAL;

                e = unbase32hexchar(x[4]);
                अगर (e < 0)
                        वापस -EINVAL;

                f = unbase32hexchar(x[5]);
                अगर (f < 0)
                        वापस -EINVAL;

                g = unbase32hexchar(x[6]);
                अगर (g < 0)
                        वापस -EINVAL;

                /* g == 000VV000 */
                अगर (g & 7)
                        वापस -EINVAL;

                *(z++) = (uint8_t) a << 3 | (uint8_t) b >> 2;                    /* XXXXXYYY */
                *(z++) = (uint8_t) b << 6 | (uint8_t) c << 1 | (uint8_t) d >> 4; /* YYZZZZZW */
                *(z++) = (uint8_t) d << 4 | (uint8_t) e >> 1;                    /* WWWWSSSS */
                *(z++) = (uint8_t) e << 7 | (uint8_t) f << 2 | (uint8_t) g >> 3; /* SQQQQQVV */

                अवरोध;
        हाल 5:
                a = unbase32hexchar(x[0]);
                अगर (a < 0)
                        वापस -EINVAL;

                b = unbase32hexchar(x[1]);
                अगर (b < 0)
                        वापस -EINVAL;

                c = unbase32hexchar(x[2]);
                अगर (c < 0)
                        वापस -EINVAL;

                d = unbase32hexchar(x[3]);
                अगर (d < 0)
                        वापस -EINVAL;

                e = unbase32hexchar(x[4]);
                अगर (e < 0)
                        वापस -EINVAL;

                /* e == 000SSSS0 */
                अगर (e & 1)
                        वापस -EINVAL;

                *(z++) = (uint8_t) a << 3 | (uint8_t) b >> 2;                    /* XXXXXYYY */
                *(z++) = (uint8_t) b << 6 | (uint8_t) c << 1 | (uint8_t) d >> 4; /* YYZZZZZW */
                *(z++) = (uint8_t) d << 4 | (uint8_t) e >> 1;                    /* WWWWSSSS */

                अवरोध;
        हाल 4:
                a = unbase32hexchar(x[0]);
                अगर (a < 0)
                        वापस -EINVAL;

                b = unbase32hexchar(x[1]);
                अगर (b < 0)
                        वापस -EINVAL;

                c = unbase32hexchar(x[2]);
                अगर (c < 0)
                        वापस -EINVAL;

                d = unbase32hexchar(x[3]);
                अगर (d < 0)
                        वापस -EINVAL;

                /* d == 000W0000 */
                अगर (d & 15)
                        वापस -EINVAL;

                *(z++) = (uint8_t) a << 3 | (uint8_t) b >> 2;                    /* XXXXXYYY */
                *(z++) = (uint8_t) b << 6 | (uint8_t) c << 1 | (uint8_t) d >> 4; /* YYZZZZZW */

                अवरोध;
        हाल 2:
                a = unbase32hexchar(x[0]);
                अगर (a < 0)
                        वापस -EINVAL;

                b = unbase32hexchar(x[1]);
                अगर (b < 0)
                        वापस -EINVAL;

                /* b == 000YYY00 */
                अगर (b & 3)
                        वापस -EINVAL;

                *(z++) = (uint8_t) a << 3 | (uint8_t) b >> 2; /* XXXXXYYY */

                अवरोध;
        हाल 0:
                अवरोध;
        शेष:
                वापस -EINVAL;
        }

        *z = 0;

        *mem = TAKE_PTR(r);
        *_len = len;

        वापस 0;
}

/* https://tools.ietf.org/html/rfc4648#section-4 */
अक्षर base64char(पूर्णांक x) {
        स्थिर अक्षर *table = UPPERCASE_LETTERS LOWERCASE_LETTERS DIGITS "+/";
        वापस table[x & 63];
}

/* This is almost base64char(), but not entirely, as it uses the "url and filename safe" alphabet,
 * since we don't want "/" appear in interface names (since interfaces appear in sysfs as filenames).
 * See section #5 of RFC 4648. */
अक्षर urlsafe_base64char(पूर्णांक x) {
        स्थिर अक्षर *table = UPPERCASE_LETTERS LOWERCASE_LETTERS DIGITS "-_";
        वापस table[x & 63];
}

पूर्णांक unbase64char(अक्षर c) {
        अचिन्हित offset;

        अगर (c >= 'A' && c <= 'Z')
                वापस c - 'A';

        offset = 'Z' - 'A' + 1;

        अगर (c >= 'a' && c <= 'z')
                वापस c - 'a' + offset;

        offset += 'z' - 'a' + 1;

        अगर (c >= '0' && c <= '9')
                वापस c - '0' + offset;

        offset += '9' - '0' + 1;

        अगर (IN_SET(c, '+', '-')) /* Support both the regular and the URL safe character set (see above) */
                वापस offset;

        offset++;

        अगर (IN_SET(c, '/', '_')) /* ditto */
                वापस offset;

        वापस -EINVAL;
}

अटल व्योम maybe_पंक्ति_break(अक्षर **x, अक्षर *start, माप_प्रकार पंक्ति_break) {
        माप_प्रकार n;

        निश्चित(x);
        निश्चित(*x);
        निश्चित(start);
        निश्चित(*x >= start);

        अगर (पंक्ति_break == SIZE_MAX)
                वापस;

        n = *x - start;

        अगर (n % (पंक्ति_break + 1) == पंक्ति_break)
                *((*x)++) = '\n';
}

ssize_t base64mem_full(
                स्थिर व्योम *p,
                माप_प्रकार l,
                माप_प्रकार पंक्ति_break,
                अक्षर **ret) {

        स्थिर uint8_t *x;
        अक्षर *b, *z;
        माप_प्रकार m;

        निश्चित(p || l == 0);
        निश्चित(पंक्ति_break > 0);
        निश्चित(ret);

        /* three input bytes makes four output bytes, padding is added so we must round up */
        m = 4 * (l + 2) / 3 + 1;
        अगर (पंक्ति_break != SIZE_MAX)
                m += m / पंक्ति_break;

        z = b = दो_स्मृति(m);
        अगर (!b)
                वापस -ENOMEM;

        क्रम (x = p; x && x < (स्थिर uint8_t*) p + (l / 3) * 3; x += 3) {
                /* x[0] == XXXXXXXX; x[1] == YYYYYYYY; x[2] == ZZZZZZZZ */
                maybe_पंक्ति_break(&z, b, पंक्ति_break);
                *(z++) = base64char(x[0] >> 2);                    /* 00XXXXXX */
                maybe_पंक्ति_break(&z, b, पंक्ति_break);
                *(z++) = base64char((x[0] & 3) << 4 | x[1] >> 4);  /* 00XXYYYY */
                maybe_पंक्ति_break(&z, b, पंक्ति_break);
                *(z++) = base64char((x[1] & 15) << 2 | x[2] >> 6); /* 00YYYYZZ */
                maybe_पंक्ति_break(&z, b, पंक्ति_break);
                *(z++) = base64char(x[2] & 63);                    /* 00ZZZZZZ */
        }

        चयन (l % 3) {
        हाल 2:
                maybe_पंक्ति_break(&z, b, पंक्ति_break);
                *(z++) = base64char(x[0] >> 2);                   /* 00XXXXXX */
                maybe_पंक्ति_break(&z, b, पंक्ति_break);
                *(z++) = base64char((x[0] & 3) << 4 | x[1] >> 4); /* 00XXYYYY */
                maybe_पंक्ति_break(&z, b, पंक्ति_break);
                *(z++) = base64char((x[1] & 15) << 2);            /* 00YYYY00 */
                maybe_पंक्ति_break(&z, b, पंक्ति_break);
                *(z++) = '=';
                अवरोध;

        हाल 1:
                maybe_पंक्ति_break(&z, b, पंक्ति_break);
                *(z++) = base64char(x[0] >> 2);        /* 00XXXXXX */
                maybe_पंक्ति_break(&z, b, पंक्ति_break);
                *(z++) = base64char((x[0] & 3) << 4);  /* 00XX0000 */
                maybe_पंक्ति_break(&z, b, पंक्ति_break);
                *(z++) = '=';
                maybe_पंक्ति_break(&z, b, पंक्ति_break);
                *(z++) = '=';
                अवरोध;
        }

        *z = 0;
        *ret = b;

        निश्चित(z >= b); /* Let static analyzers know that the answer is non-negative. */
        वापस z - b;
}

अटल ssize_t base64_append_width(
                अक्षर **prefix,
                माप_प्रकार plen,
                अक्षर sep,
                माप_प्रकार indent,
                स्थिर व्योम *p,
                माप_प्रकार l,
                माप_प्रकार width) {

        _cleanup_free_ अक्षर *x = शून्य;
        अक्षर *t, *s;
        माप_प्रकार पंक्तियाँ;
        ssize_t len;

        निश्चित(prefix);
        निश्चित(*prefix || plen == 0);
        निश्चित(p || l == 0);

        len = base64mem(p, l, &x);
        अगर (len < 0)
                वापस len;
        अगर (len == 0)
                वापस plen;

        पंक्तियाँ = DIV_ROUND_UP(len, width);

        अगर (plen >= SSIZE_MAX - 1 - 1 ||
            पंक्तियाँ > (SSIZE_MAX - plen - 1 - 1) / (indent + width + 1))
                वापस -ENOMEM;

        t = पुनः_स्मृति(*prefix, plen + 1 + 1 + (indent + width + 1) * पंक्तियाँ);
        अगर (!t)
                वापस -ENOMEM;

        s = t + plen;
        क्रम (माप_प्रकार पंक्ति = 0; पंक्ति < पंक्तियाँ; पंक्ति++) {
                माप_प्रकार act = MIN(width, (माप_प्रकार) len);

                अगर (पंक्ति > 0)
                        sep = '\n';

                अगर (s > t) {
                        *s++ = sep;
                        अगर (sep == '\n')
                                s = mempset(s, ' ', indent);
                }

                s = mempcpy(s, x + width * पंक्ति, act);
                len -= act;
        }
        निश्चित(len == 0);

        *s = '\0';
        *prefix = t;
        वापस s - t;
}

ssize_t base64_append(
                अक्षर **prefix,
                माप_प्रकार plen,
                स्थिर व्योम *p,
                माप_प्रकार l,
                माप_प्रकार indent,
                माप_प्रकार width) {

        अगर (plen > width / 2 || plen + indent > width)
                /* leave indent on the left, keep last column free */
                वापस base64_append_width(prefix, plen, '\n', indent, p, l, width - indent);
        अन्यथा
                /* leave plen on the left, keep last column free */
                वापस base64_append_width(prefix, plen, ' ', plen + 1, p, l, width - plen - 1);
}

अटल पूर्णांक unbase64_अग्र(स्थिर अक्षर **p, माप_प्रकार *l) {
        पूर्णांक ret;

        निश्चित(p);
        निश्चित(l);

        /* Find the next non-whitespace character, and decode it. If we find padding, we return it as INT_MAX. We
         * greedily skip all preceding and all following whitespace. */

        क्रम (;;) {
                अगर (*l == 0)
                        वापस -EPIPE;

                अगर (!म_अक्षर(WHITESPACE, **p))
                        अवरोध;

                /* Skip leading whitespace */
                (*p)++, (*l)--;
        }

        अगर (**p == '=')
                ret = पूर्णांक_उच्च; /* return padding as INT_MAX */
        अन्यथा {
                ret = unbase64char(**p);
                अगर (ret < 0)
                        वापस ret;
        }

        क्रम (;;) {
                (*p)++, (*l)--;

                अगर (*l == 0)
                        अवरोध;
                अगर (!म_अक्षर(WHITESPACE, **p))
                        अवरोध;

                /* Skip following whitespace */
        }

        वापस ret;
}

पूर्णांक unbase64mem_full(
                स्थिर अक्षर *p,
                माप_प्रकार l,
                bool secure,
                व्योम **ret_तथ्य,
                माप_प्रकार *ret_आकार) {

        _cleanup_free_ uint8_t *buf = शून्य;
        स्थिर अक्षर *x;
        uint8_t *z;
        माप_प्रकार len;

        निश्चित(p || l == 0);

        अगर (l == SIZE_MAX)
                l = म_माप(p);

        /* A group of four input bytes needs three output bytes, in case of padding we need to add two or three extra
         * bytes. Note that this calculation is an upper boundary, as we ignore whitespace while decoding */
        len = (l / 4) * 3 + (l % 4 != 0 ? (l % 4) - 1 : 0);

        buf = दो_स्मृति(len + 1);
        अगर (!buf)
                वापस -ENOMEM;

        CLEANUP_ERASE_PTR(secure ? &buf : शून्य, len);

        क्रम (x = p, z = buf;;) {
                पूर्णांक a, b, c, d; /* a == 00XXXXXX; b == 00YYYYYY; c == 00ZZZZZZ; d == 00WWWWWW */

                a = unbase64_अग्र(&x, &l);
                अगर (a == -EPIPE) /* End of string */
                        अवरोध;
                अगर (a < 0)
                        वापस a;
                अगर (a == पूर्णांक_उच्च) /* Padding is not allowed at the beginning of a 4ch block */
                        वापस -EINVAL;

                b = unbase64_अग्र(&x, &l);
                अगर (b < 0)
                        वापस b;
                अगर (b == पूर्णांक_उच्च) /* Padding is not allowed at the second character of a 4ch block either */
                        वापस -EINVAL;

                c = unbase64_अग्र(&x, &l);
                अगर (c < 0)
                        वापस c;

                d = unbase64_अग्र(&x, &l);
                अगर (d < 0)
                        वापस d;

                अगर (c == पूर्णांक_उच्च) { /* Padding at the third character */

                        अगर (d != पूर्णांक_उच्च) /* If the third character is padding, the fourth must be too */
                                वापस -EINVAL;

                        /* b == 00YY0000 */
                        अगर (b & 15)
                                वापस -EINVAL;

                        अगर (l > 0) /* Trailing rubbish? */
                                वापस -ENAMETOOLONG;

                        *(z++) = (uint8_t) a << 2 | (uint8_t) (b >> 4); /* XXXXXXYY */
                        अवरोध;
                }

                अगर (d == पूर्णांक_उच्च) {
                        /* c == 00ZZZZ00 */
                        अगर (c & 3)
                                वापस -EINVAL;

                        अगर (l > 0) /* Trailing rubbish? */
                                वापस -ENAMETOOLONG;

                        *(z++) = (uint8_t) a << 2 | (uint8_t) b >> 4; /* XXXXXXYY */
                        *(z++) = (uint8_t) b << 4 | (uint8_t) c >> 2; /* YYYYZZZZ */
                        अवरोध;
                }

                *(z++) = (uint8_t) a << 2 | (uint8_t) b >> 4; /* XXXXXXYY */
                *(z++) = (uint8_t) b << 4 | (uint8_t) c >> 2; /* YYYYZZZZ */
                *(z++) = (uint8_t) c << 6 | (uint8_t) d;      /* ZZWWWWWW */
        }

        *z = 0;

        निश्चित((माप_प्रकार) (z - buf) <= len);

        अगर (ret_आकार)
                *ret_आकार = (माप_प्रकार) (z - buf);
        अगर (ret_तथ्य)
                *ret_तथ्य = TAKE_PTR(buf);

        वापस 0;
}

व्योम hexdump(खाता *f, स्थिर व्योम *p, माप_प्रकार s) {
        स्थिर uint8_t *b = p;
        अचिन्हित n = 0;

        निश्चित(b || s == 0);

        अगर (s == SIZE_MAX)
                s = म_माप(p);

        अगर (!f)
                f = मानक_निकास;

        जबतक (s > 0) {
                माप_प्रकार i;

                ख_लिखो(f, "%04x  ", n);

                क्रम (i = 0; i < 16; i++) {

                        अगर (i >= s)
                                ख_माला_दो("   ", f);
                        अन्यथा
                                ख_लिखो(f, "%02x ", b[i]);

                        अगर (i == 7)
                                ख_अक्षर_दो(' ', f);
                }

                ख_अक्षर_दो(' ', f);

                क्रम (i = 0; i < 16; i++) {

                        अगर (i >= s)
                                ख_अक्षर_दो(' ', f);
                        अन्यथा
                                ख_अक्षर_दो(है_छाप(b[i]) ? (अक्षर) b[i] : '.', f);
                }

                ख_अक्षर_दो('\n', f);

                अगर (s < 16)
                        अवरोध;

                n += 16;
                b += 16;
                s -= 16;
        }
}
