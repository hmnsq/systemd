/* SPDX-License-Identifier: LGPL-2.1-or-later */

#అన్తర్భావ <ctype.h>
#అన్తర్భావ <stdio.h>

#అన్తర్భావ "alloc-util.h"
#అన్తర్భావ "hexdecoct.h"
#అన్తర్భావ "memory-util.h"
#అన్తర్భావ "string-util.h"

అక్షరం octchar(పూర్ణాంకం x) {
        తిరిగి '0' + (x & 7);
}

పూర్ణాంకం unoctchar(అక్షరం c) {

        ఒకవేళ (c >= '0' && c <= '7')
                తిరిగి c - '0';

        తిరిగి -EINVAL;
}

అక్షరం decchar(పూర్ణాంకం x) {
        తిరిగి '0' + (x % 10);
}

పూర్ణాంకం undecchar(అక్షరం c) {

        ఒకవేళ (c >= '0' && c <= '9')
                తిరిగి c - '0';

        తిరిగి -EINVAL;
}

అక్షరం hexchar(పూర్ణాంకం x) {
        స్థిరరాశి అక్షరం *పట్టిక = LOWERCASE_HEXDIGITS;
        తిరిగి పట్టిక[x & 15];
}

పూర్ణాంకం unhexchar(అక్షరం c) {

        ఒకవేళ (c >= '0' && c <= '9')
                తిరిగి c - '0';

        ఒకవేళ (c >= 'a' && c <= 'f')
                తిరిగి c - 'a' + 10;

        ఒకవేళ (c >= 'A' && c <= 'F')
                తిరిగి c - 'A' + 10;

        తిరిగి -EINVAL;
}

అక్షరం* hexmem(స్థిరరాశి శూన్య *p, size_t l) {
        స్థిరరాశి uint8_t *x;
        అక్షరం *r, *z;

        ప్రతిజ్ఞా(p || l == 0);

        z = r = new(అక్షరం, l * 2 + 1);
        ఒకవేళ (!r)
                తిరిగి ఖాళీ;

        కొరకు (x = p; x && x < (స్థిరరాశి uint8_t*) p + l; x++) {
                *(z++) = hexchar(*x >> 4);
                *(z++) = hexchar(*x & 15);
        }

        *z = 0;
        తిరిగి r;
}

స్థిర పూర్ణాంకం unhex_తదుపరి(స్థిరరాశి అక్షరం **p, size_t *l) {
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(p);
        ప్రతిజ్ఞా(l);

        /* Find the next non-whitespace character, and decode it. We
         * greedily skip all preceding and all following whitespace. */

        కొరకు (;;) {
                ఒకవేళ (*l == 0)
                        తిరిగి -EPIPE;

                ఒకవేళ (!strchr(WHITESPACE, **p))
                        ఆపు;

                /* Skip leading whitespace */
                (*p)++, (*l)--;
        }

        r = unhexchar(**p);
        ఒకవేళ (r < 0)
                తిరిగి r;

        కొరకు (;;) {
                (*p)++, (*l)--;

                ఒకవేళ (*l == 0 || !strchr(WHITESPACE, **p))
                        ఆపు;

                /* Skip following whitespace */
        }

        తిరిగి r;
}

పూర్ణాంకం unhexmem_నిండిన(
                స్థిరరాశి అక్షరం *p,
                size_t l,
                bool secure,
                శూన్య **ret_దత్తాంశం,
                size_t *ret_కొలత) {

        _cleanup_free_ uint8_t *buf = ఖాళీ;
        size_t buf_size;
        స్థిరరాశి అక్షరం *x;
        uint8_t *z;

        ప్రతిజ్ఞా(p || l == 0);

        ఒకవేళ (l == SIZE_MAX)
                l = అక్షరశ్రేణి_పొడవు(p);

        /* Note that the calculation of memory size is an upper boundary, as we ignore whitespace while decoding */
        buf_size = (l + 1) / 2 + 1;
        buf = జ్ఞాపకం_ఇవ్వు(buf_size);
        ఒకవేళ (!buf)
                తిరిగి -ENOMEM;

        CLEANUP_ERASE_PTR(secure ? &buf : ఖాళీ, buf_size);

        కొరకు (x = p, z = buf;;) {
                పూర్ణాంకం a, b;

                a = unhex_తదుపరి(&x, &l);
                ఒకవేళ (a == -EPIPE) /* End of string */
                        ఆపు;
                ఒకవేళ (a < 0)
                        తిరిగి a;

                b = unhex_తదుపరి(&x, &l);
                ఒకవేళ (b < 0)
                        తిరిగి b;

                *(z++) = (uint8_t) a << 4 | (uint8_t) b;
        }

        *z = 0;

        ఒకవేళ (ret_కొలత)
                *ret_కొలత = (size_t) (z - buf);
        ఒకవేళ (ret_దత్తాంశం)
                *ret_దత్తాంశం = TAKE_PTR(buf);

        తిరిగి 0;
}

/* https://tools.ietf.org/html/rfc4648#section-6
 * Notice that base32hex differs from base32 in the alphabet it uses.
 * The distinction is that the base32hex representation preserves the
 * order of the underlying data when compared as bytestrings, this is
 * useful when representing NSEC3 hashes, as one can then verify the
 * order of hashes directly from their representation. */
అక్షరం base32hexchar(పూర్ణాంకం x) {
        స్థిరరాశి అక్షరం *పట్టిక = DIGITS "ABCDEFGHIJKLMNOPQRSTUV";
        తిరిగి పట్టిక[x & 31];
}

పూర్ణాంకం unbase32hexchar(అక్షరం c) {
        గుర్తులేని స్థానభ్రంశం;

        ఒకవేళ (c >= '0' && c <= '9')
                తిరిగి c - '0';

        స్థానభ్రంశం = '9' - '0' + 1;

        ఒకవేళ (c >= 'A' && c <= 'V')
                తిరిగి c - 'A' + స్థానభ్రంశం;

        తిరిగి -EINVAL;
}

అక్షరం* base32hexmem(స్థిరరాశి శూన్య *p, size_t l, bool padding) {
        అక్షరం *r, *z;
        స్థిరరాశి uint8_t *x;
        size_t len;

        ప్రతిజ్ఞా(p || l == 0);

        ఒకవేళ (padding)
                /* five input bytes makes eight output bytes, padding is added so we must round up */
                len = 8 * (l + 4) / 5;
        లేకపోతే {
                /* same, but round down as there is no padding */
                len = 8 * l / 5;

                ఎంపిక (l % 5) {
                స్థితి 4:
                        len += 7;
                        ఆపు;
                స్థితి 3:
                        len += 5;
                        ఆపు;
                స్థితి 2:
                        len += 4;
                        ఆపు;
                స్థితి 1:
                        len += 2;
                        ఆపు;
                }
        }

        z = r = జ్ఞాపకం_ఇవ్వు(len + 1);
        ఒకవేళ (!r)
                తిరిగి ఖాళీ;

        కొరకు (x = p; x < (స్థిరరాశి uint8_t*) p + (l / 5) * 5; x += 5) {
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

        ఎంపిక (l % 5) {
        స్థితి 4:
                *(z++) = base32hexchar(x[0] >> 3);                    /* 000XXXXX */
                *(z++) = base32hexchar((x[0] & 7) << 2 | x[1] >> 6);  /* 000XXXYY */
                *(z++) = base32hexchar((x[1] & 63) >> 1);             /* 000YYYYY */
                *(z++) = base32hexchar((x[1] & 1) << 4 | x[2] >> 4);   /* 000YZZZZ */
                *(z++) = base32hexchar((x[2] & 15) << 1 | x[3] >> 7); /* 000ZZZZQ */
                *(z++) = base32hexchar((x[3] & 127) >> 2);            /* 000QQQQQ */
                *(z++) = base32hexchar((x[3] & 3) << 3);              /* 000QQ000 */
                ఒకవేళ (padding)
                        *(z++) = '=';

                ఆపు;

        స్థితి 3:
                *(z++) = base32hexchar(x[0] >> 3);                   /* 000XXXXX */
                *(z++) = base32hexchar((x[0] & 7) << 2 | x[1] >> 6); /* 000XXXYY */
                *(z++) = base32hexchar((x[1] & 63) >> 1);            /* 000YYYYY */
                *(z++) = base32hexchar((x[1] & 1) << 4 | x[2] >> 4); /* 000YZZZZ */
                *(z++) = base32hexchar((x[2] & 15) << 1);            /* 000ZZZZ0 */
                ఒకవేళ (padding) {
                        *(z++) = '=';
                        *(z++) = '=';
                        *(z++) = '=';
                }

                ఆపు;

        స్థితి 2:
                *(z++) = base32hexchar(x[0] >> 3);                   /* 000XXXXX */
                *(z++) = base32hexchar((x[0] & 7) << 2 | x[1] >> 6); /* 000XXXYY */
                *(z++) = base32hexchar((x[1] & 63) >> 1);            /* 000YYYYY */
                *(z++) = base32hexchar((x[1] & 1) << 4);             /* 000Y0000 */
                ఒకవేళ (padding) {
                        *(z++) = '=';
                        *(z++) = '=';
                        *(z++) = '=';
                        *(z++) = '=';
                }

                ఆపు;

        స్థితి 1:
                *(z++) = base32hexchar(x[0] >> 3);       /* 000XXXXX */
                *(z++) = base32hexchar((x[0] & 7) << 2); /* 000XXX00 */
                ఒకవేళ (padding) {
                        *(z++) = '=';
                        *(z++) = '=';
                        *(z++) = '=';
                        *(z++) = '=';
                        *(z++) = '=';
                        *(z++) = '=';
                }

                ఆపు;
        }

        *z = 0;
        తిరిగి r;
}

పూర్ణాంకం unbase32hexmem(స్థిరరాశి అక్షరం *p, size_t l, bool padding, శూన్య **mem, size_t *_len) {
        _cleanup_free_ uint8_t *r = ఖాళీ;
        పూర్ణాంకం a, b, c, d, e, f, g, h;
        uint8_t *z;
        స్థిరరాశి అక్షరం *x;
        size_t len;
        గుర్తులేని pad = 0;

        ప్రతిజ్ఞా(p || l == 0);
        ప్రతిజ్ఞా(mem);
        ప్రతిజ్ఞా(_len);

        ఒకవేళ (l == SIZE_MAX)
                l = అక్షరశ్రేణి_పొడవు(p);

        /* padding ensures any base32hex input has input divisible by 8 */
        ఒకవేళ (padding && l % 8 != 0)
                తిరిగి -EINVAL;

        ఒకవేళ (padding) {
                /* strip the padding */
                అయితే (l > 0 && p[l - 1] == '=' && pad < 7) {
                        pad++;
                        l--;
                }
        }

        /* a group of eight input bytes needs five output bytes, in case of
         * padding we need to add some extra bytes */
        len = (l / 8) * 5;

        ఎంపిక (l % 8) {
        స్థితి 7:
                len += 4;
                ఆపు;
        స్థితి 5:
                len += 3;
                ఆపు;
        స్థితి 4:
                len += 2;
                ఆపు;
        స్థితి 2:
                len += 1;
                ఆపు;
        స్థితి 0:
                ఆపు;
        అప్రమేయ:
                తిరిగి -EINVAL;
        }

        z = r = జ్ఞాపకం_ఇవ్వు(len + 1);
        ఒకవేళ (!r)
                తిరిగి -ENOMEM;

        కొరకు (x = p; x < p + (l / 8) * 8; x += 8) {
                /* a == 000XXXXX; b == 000YYYYY; c == 000ZZZZZ; d == 000WWWWW
                 * e == 000SSSSS; f == 000QQQQQ; g == 000VVVVV; h == 000RRRRR */
                a = unbase32hexchar(x[0]);
                ఒకవేళ (a < 0)
                        తిరిగి -EINVAL;

                b = unbase32hexchar(x[1]);
                ఒకవేళ (b < 0)
                        తిరిగి -EINVAL;

                c = unbase32hexchar(x[2]);
                ఒకవేళ (c < 0)
                        తిరిగి -EINVAL;

                d = unbase32hexchar(x[3]);
                ఒకవేళ (d < 0)
                        తిరిగి -EINVAL;

                e = unbase32hexchar(x[4]);
                ఒకవేళ (e < 0)
                        తిరిగి -EINVAL;

                f = unbase32hexchar(x[5]);
                ఒకవేళ (f < 0)
                        తిరిగి -EINVAL;

                g = unbase32hexchar(x[6]);
                ఒకవేళ (g < 0)
                        తిరిగి -EINVAL;

                h = unbase32hexchar(x[7]);
                ఒకవేళ (h < 0)
                        తిరిగి -EINVAL;

                *(z++) = (uint8_t) a << 3 | (uint8_t) b >> 2;                    /* XXXXXYYY */
                *(z++) = (uint8_t) b << 6 | (uint8_t) c << 1 | (uint8_t) d >> 4; /* YYZZZZZW */
                *(z++) = (uint8_t) d << 4 | (uint8_t) e >> 1;                    /* WWWWSSSS */
                *(z++) = (uint8_t) e << 7 | (uint8_t) f << 2 | (uint8_t) g >> 3; /* SQQQQQVV */
                *(z++) = (uint8_t) g << 5 | (uint8_t) h;                         /* VVVRRRRR */
        }

        ఎంపిక (l % 8) {
        స్థితి 7:
                a = unbase32hexchar(x[0]);
                ఒకవేళ (a < 0)
                        తిరిగి -EINVAL;

                b = unbase32hexchar(x[1]);
                ఒకవేళ (b < 0)
                        తిరిగి -EINVAL;

                c = unbase32hexchar(x[2]);
                ఒకవేళ (c < 0)
                        తిరిగి -EINVAL;

                d = unbase32hexchar(x[3]);
                ఒకవేళ (d < 0)
                        తిరిగి -EINVAL;

                e = unbase32hexchar(x[4]);
                ఒకవేళ (e < 0)
                        తిరిగి -EINVAL;

                f = unbase32hexchar(x[5]);
                ఒకవేళ (f < 0)
                        తిరిగి -EINVAL;

                g = unbase32hexchar(x[6]);
                ఒకవేళ (g < 0)
                        తిరిగి -EINVAL;

                /* g == 000VV000 */
                ఒకవేళ (g & 7)
                        తిరిగి -EINVAL;

                *(z++) = (uint8_t) a << 3 | (uint8_t) b >> 2;                    /* XXXXXYYY */
                *(z++) = (uint8_t) b << 6 | (uint8_t) c << 1 | (uint8_t) d >> 4; /* YYZZZZZW */
                *(z++) = (uint8_t) d << 4 | (uint8_t) e >> 1;                    /* WWWWSSSS */
                *(z++) = (uint8_t) e << 7 | (uint8_t) f << 2 | (uint8_t) g >> 3; /* SQQQQQVV */

                ఆపు;
        స్థితి 5:
                a = unbase32hexchar(x[0]);
                ఒకవేళ (a < 0)
                        తిరిగి -EINVAL;

                b = unbase32hexchar(x[1]);
                ఒకవేళ (b < 0)
                        తిరిగి -EINVAL;

                c = unbase32hexchar(x[2]);
                ఒకవేళ (c < 0)
                        తిరిగి -EINVAL;

                d = unbase32hexchar(x[3]);
                ఒకవేళ (d < 0)
                        తిరిగి -EINVAL;

                e = unbase32hexchar(x[4]);
                ఒకవేళ (e < 0)
                        తిరిగి -EINVAL;

                /* e == 000SSSS0 */
                ఒకవేళ (e & 1)
                        తిరిగి -EINVAL;

                *(z++) = (uint8_t) a << 3 | (uint8_t) b >> 2;                    /* XXXXXYYY */
                *(z++) = (uint8_t) b << 6 | (uint8_t) c << 1 | (uint8_t) d >> 4; /* YYZZZZZW */
                *(z++) = (uint8_t) d << 4 | (uint8_t) e >> 1;                    /* WWWWSSSS */

                ఆపు;
        స్థితి 4:
                a = unbase32hexchar(x[0]);
                ఒకవేళ (a < 0)
                        తిరిగి -EINVAL;

                b = unbase32hexchar(x[1]);
                ఒకవేళ (b < 0)
                        తిరిగి -EINVAL;

                c = unbase32hexchar(x[2]);
                ఒకవేళ (c < 0)
                        తిరిగి -EINVAL;

                d = unbase32hexchar(x[3]);
                ఒకవేళ (d < 0)
                        తిరిగి -EINVAL;

                /* d == 000W0000 */
                ఒకవేళ (d & 15)
                        తిరిగి -EINVAL;

                *(z++) = (uint8_t) a << 3 | (uint8_t) b >> 2;                    /* XXXXXYYY */
                *(z++) = (uint8_t) b << 6 | (uint8_t) c << 1 | (uint8_t) d >> 4; /* YYZZZZZW */

                ఆపు;
        స్థితి 2:
                a = unbase32hexchar(x[0]);
                ఒకవేళ (a < 0)
                        తిరిగి -EINVAL;

                b = unbase32hexchar(x[1]);
                ఒకవేళ (b < 0)
                        తిరిగి -EINVAL;

                /* b == 000YYY00 */
                ఒకవేళ (b & 3)
                        తిరిగి -EINVAL;

                *(z++) = (uint8_t) a << 3 | (uint8_t) b >> 2; /* XXXXXYYY */

                ఆపు;
        స్థితి 0:
                ఆపు;
        అప్రమేయ:
                తిరిగి -EINVAL;
        }

        *z = 0;

        *mem = TAKE_PTR(r);
        *_len = len;

        తిరిగి 0;
}

/* https://tools.ietf.org/html/rfc4648#section-4 */
అక్షరం base64char(పూర్ణాంకం x) {
        స్థిరరాశి అక్షరం *పట్టిక = UPPERCASE_LETTERS LOWERCASE_LETTERS DIGITS "+/";
        తిరిగి పట్టిక[x & 63];
}

/* This is almost base64char(), but not entirely, as it uses the "url and filename safe" alphabet,
 * since we don't want "/" appear in interface names (since interfaces appear in sysfs as filenames).
 * See section #5 of RFC 4648. */
అక్షరం urlsafe_base64char(పూర్ణాంకం x) {
        స్థిరరాశి అక్షరం *పట్టిక = UPPERCASE_LETTERS LOWERCASE_LETTERS DIGITS "-_";
        తిరిగి పట్టిక[x & 63];
}

పూర్ణాంకం unbase64char(అక్షరం c) {
        గుర్తులేని స్థానభ్రంశం;

        ఒకవేళ (c >= 'A' && c <= 'Z')
                తిరిగి c - 'A';

        స్థానభ్రంశం = 'Z' - 'A' + 1;

        ఒకవేళ (c >= 'a' && c <= 'z')
                తిరిగి c - 'a' + స్థానభ్రంశం;

        స్థానభ్రంశం += 'z' - 'a' + 1;

        ఒకవేళ (c >= '0' && c <= '9')
                తిరిగి c - '0' + స్థానభ్రంశం;

        స్థానభ్రంశం += '9' - '0' + 1;

        ఒకవేళ (లోపల_సమితి(c, '+', '-')) /* Support both the regular and the URL safe character set (see above) */
                తిరిగి స్థానభ్రంశం;

        స్థానభ్రంశం++;

        ఒకవేళ (లోపల_సమితి(c, '/', '_')) /* ditto */
                తిరిగి స్థానభ్రంశం;

        తిరిగి -EINVAL;
}

స్థిర శూన్య maybe_పంక్తి_break(అక్షరం **x, అక్షరం *ప్రారంభం, size_t పంక్తి_break) {
        size_t n;

        ప్రతిజ్ఞా(x);
        ప్రతిజ్ఞా(*x);
        ప్రతిజ్ఞా(ప్రారంభం);
        ప్రతిజ్ఞా(*x >= ప్రారంభం);

        ఒకవేళ (పంక్తి_break == SIZE_MAX)
                తిరిగి;

        n = *x - ప్రారంభం;

        ఒకవేళ (n % (పంక్తి_break + 1) == పంక్తి_break)
                *((*x)++) = '\n';
}

ssize_t base64mem_నిండిన(
                స్థిరరాశి శూన్య *p,
                size_t l,
                size_t పంక్తి_break,
                అక్షరం **ret) {

        స్థిరరాశి uint8_t *x;
        అక్షరం *b, *z;
        size_t m;

        ప్రతిజ్ఞా(p || l == 0);
        ప్రతిజ్ఞా(పంక్తి_break > 0);
        ప్రతిజ్ఞా(ret);

        /* three input bytes makes four output bytes, padding is added so we must round up */
        m = 4 * (l + 2) / 3 + 1;
        ఒకవేళ (పంక్తి_break != SIZE_MAX)
                m += m / పంక్తి_break;

        z = b = జ్ఞాపకం_ఇవ్వు(m);
        ఒకవేళ (!b)
                తిరిగి -ENOMEM;

        కొరకు (x = p; x && x < (స్థిరరాశి uint8_t*) p + (l / 3) * 3; x += 3) {
                /* x[0] == XXXXXXXX; x[1] == YYYYYYYY; x[2] == ZZZZZZZZ */
                maybe_పంక్తి_break(&z, b, పంక్తి_break);
                *(z++) = base64char(x[0] >> 2);                    /* 00XXXXXX */
                maybe_పంక్తి_break(&z, b, పంక్తి_break);
                *(z++) = base64char((x[0] & 3) << 4 | x[1] >> 4);  /* 00XXYYYY */
                maybe_పంక్తి_break(&z, b, పంక్తి_break);
                *(z++) = base64char((x[1] & 15) << 2 | x[2] >> 6); /* 00YYYYZZ */
                maybe_పంక్తి_break(&z, b, పంక్తి_break);
                *(z++) = base64char(x[2] & 63);                    /* 00ZZZZZZ */
        }

        ఎంపిక (l % 3) {
        స్థితి 2:
                maybe_పంక్తి_break(&z, b, పంక్తి_break);
                *(z++) = base64char(x[0] >> 2);                   /* 00XXXXXX */
                maybe_పంక్తి_break(&z, b, పంక్తి_break);
                *(z++) = base64char((x[0] & 3) << 4 | x[1] >> 4); /* 00XXYYYY */
                maybe_పంక్తి_break(&z, b, పంక్తి_break);
                *(z++) = base64char((x[1] & 15) << 2);            /* 00YYYY00 */
                maybe_పంక్తి_break(&z, b, పంక్తి_break);
                *(z++) = '=';
                ఆపు;

        స్థితి 1:
                maybe_పంక్తి_break(&z, b, పంక్తి_break);
                *(z++) = base64char(x[0] >> 2);        /* 00XXXXXX */
                maybe_పంక్తి_break(&z, b, పంక్తి_break);
                *(z++) = base64char((x[0] & 3) << 4);  /* 00XX0000 */
                maybe_పంక్తి_break(&z, b, పంక్తి_break);
                *(z++) = '=';
                maybe_పంక్తి_break(&z, b, పంక్తి_break);
                *(z++) = '=';
                ఆపు;
        }

        *z = 0;
        *ret = b;

        ప్రతిజ్ఞా(z >= b); /* Let static analyzers know that the answer is non-negative. */
        తిరిగి z - b;
}

స్థిర ssize_t base64_append_వెడల్పు(
                అక్షరం **ఉపసర్గ,
                size_t plen,
                అక్షరం sep,
                size_t indent,
                స్థిరరాశి శూన్య *p,
                size_t l,
                size_t width) {

        _cleanup_free_ అక్షరం *x = ఖాళీ;
        అక్షరం *t, *s;
        size_t పంక్తులు;
        ssize_t len;

        ప్రతిజ్ఞా(ఉపసర్గ);
        ప్రతిజ్ఞా(*ఉపసర్గ || plen == 0);
        ప్రతిజ్ఞా(p || l == 0);

        len = base64mem(p, l, &x);
        ఒకవేళ (len < 0)
                తిరిగి len;
        ఒకవేళ (len == 0)
                తిరిగి plen;

        పంక్తులు = DIV_విడత_పైకి(len, width);

        ఒకవేళ (plen >= SSIZE_MAX - 1 - 1 ||
            పంక్తులు > (SSIZE_MAX - plen - 1 - 1) / (indent + width + 1))
                తిరిగి -ENOMEM;

        t = realloc(*ఉపసర్గ, plen + 1 + 1 + (indent + width + 1) * పంక్తులు);
        ఒకవేళ (!t)
                తిరిగి -ENOMEM;

        s = t + plen;
        కొరకు (size_t పంక్తి = 0; పంక్తి < పంక్తులు; పంక్తి++) {
                size_t act = MIN(width, (size_t) len);

                ఒకవేళ (పంక్తి > 0)
                        sep = '\n';

                ఒకవేళ (s > t) {
                        *s++ = sep;
                        ఒకవేళ (sep == '\n')
                                s = mempset(s, ' ', indent);
                }

                s = mempcpy(s, x + width * పంక్తి, act);
                len -= act;
        }
        ప్రతిజ్ఞా(len == 0);

        *s = '\0';
        *ఉపసర్గ = t;
        తిరిగి s - t;
}

ssize_t base64_append(
                అక్షరం **ఉపసర్గ,
                size_t plen,
                స్థిరరాశి శూన్య *p,
                size_t l,
                size_t indent,
                size_t width) {

        ఒకవేళ (plen > width / 2 || plen + indent > width)
                /* leave indent on the left, keep last column free */
                తిరిగి base64_append_వెడల్పు(ఉపసర్గ, plen, '\n', indent, p, l, width - indent);
        లేకపోతే
                /* leave plen on the left, keep last column free */
                తిరిగి base64_append_వెడల్పు(ఉపసర్గ, plen, ' ', plen + 1, p, l, width - plen - 1);
}

స్థిర పూర్ణాంకం unbase64_తదుపరి(స్థిరరాశి అక్షరం **p, size_t *l) {
        పూర్ణాంకం ret;

        ప్రతిజ్ఞా(p);
        ప్రతిజ్ఞా(l);

        /* Find the next non-whitespace character, and decode it. If we find padding, we return it as INT_MAX. We
         * greedily skip all preceding and all following whitespace. */

        కొరకు (;;) {
                ఒకవేళ (*l == 0)
                        తిరిగి -EPIPE;

                ఒకవేళ (!strchr(WHITESPACE, **p))
                        ఆపు;

                /* Skip leading whitespace */
                (*p)++, (*l)--;
        }

        ఒకవేళ (**p == '=')
                ret = గరిష్ఠ_పూర్ణాంకం; /* return padding as INT_MAX */
        లేకపోతే {
                ret = unbase64char(**p);
                ఒకవేళ (ret < 0)
                        తిరిగి ret;
        }

        కొరకు (;;) {
                (*p)++, (*l)--;

                ఒకవేళ (*l == 0)
                        ఆపు;
                ఒకవేళ (!strchr(WHITESPACE, **p))
                        ఆపు;

                /* Skip following whitespace */
        }

        తిరిగి ret;
}

పూర్ణాంకం unbase64mem_నిండిన(
                స్థిరరాశి అక్షరం *p,
                size_t l,
                bool secure,
                శూన్య **ret_దత్తాంశం,
                size_t *ret_కొలత) {

        _cleanup_free_ uint8_t *buf = ఖాళీ;
        స్థిరరాశి అక్షరం *x;
        uint8_t *z;
        size_t len;

        ప్రతిజ్ఞా(p || l == 0);

        ఒకవేళ (l == SIZE_MAX)
                l = అక్షరశ్రేణి_పొడవు(p);

        /* A group of four input bytes needs three output bytes, in case of padding we need to add two or three extra
         * bytes. Note that this calculation is an upper boundary, as we ignore whitespace while decoding */
        len = (l / 4) * 3 + (l % 4 != 0 ? (l % 4) - 1 : 0);

        buf = జ్ఞాపకం_ఇవ్వు(len + 1);
        ఒకవేళ (!buf)
                తిరిగి -ENOMEM;

        CLEANUP_ERASE_PTR(secure ? &buf : ఖాళీ, len);

        కొరకు (x = p, z = buf;;) {
                పూర్ణాంకం a, b, c, d; /* a == 00XXXXXX; b == 00YYYYYY; c == 00ZZZZZZ; d == 00WWWWWW */

                a = unbase64_తదుపరి(&x, &l);
                ఒకవేళ (a == -EPIPE) /* End of string */
                        ఆపు;
                ఒకవేళ (a < 0)
                        తిరిగి a;
                ఒకవేళ (a == గరిష్ఠ_పూర్ణాంకం) /* Padding is not allowed at the beginning of a 4ch block */
                        తిరిగి -EINVAL;

                b = unbase64_తదుపరి(&x, &l);
                ఒకవేళ (b < 0)
                        తిరిగి b;
                ఒకవేళ (b == గరిష్ఠ_పూర్ణాంకం) /* Padding is not allowed at the second character of a 4ch block either */
                        తిరిగి -EINVAL;

                c = unbase64_తదుపరి(&x, &l);
                ఒకవేళ (c < 0)
                        తిరిగి c;

                d = unbase64_తదుపరి(&x, &l);
                ఒకవేళ (d < 0)
                        తిరిగి d;

                ఒకవేళ (c == గరిష్ఠ_పూర్ణాంకం) { /* Padding at the third character */

                        ఒకవేళ (d != గరిష్ఠ_పూర్ణాంకం) /* If the third character is padding, the fourth must be too */
                                తిరిగి -EINVAL;

                        /* b == 00YY0000 */
                        ఒకవేళ (b & 15)
                                తిరిగి -EINVAL;

                        ఒకవేళ (l > 0) /* Trailing rubbish? */
                                తిరిగి -ENAMETOOLONG;

                        *(z++) = (uint8_t) a << 2 | (uint8_t) (b >> 4); /* XXXXXXYY */
                        ఆపు;
                }

                ఒకవేళ (d == గరిష్ఠ_పూర్ణాంకం) {
                        /* c == 00ZZZZ00 */
                        ఒకవేళ (c & 3)
                                తిరిగి -EINVAL;

                        ఒకవేళ (l > 0) /* Trailing rubbish? */
                                తిరిగి -ENAMETOOLONG;

                        *(z++) = (uint8_t) a << 2 | (uint8_t) b >> 4; /* XXXXXXYY */
                        *(z++) = (uint8_t) b << 4 | (uint8_t) c >> 2; /* YYYYZZZZ */
                        ఆపు;
                }

                *(z++) = (uint8_t) a << 2 | (uint8_t) b >> 4; /* XXXXXXYY */
                *(z++) = (uint8_t) b << 4 | (uint8_t) c >> 2; /* YYYYZZZZ */
                *(z++) = (uint8_t) c << 6 | (uint8_t) d;      /* ZZWWWWWW */
        }

        *z = 0;

        ప్రతిజ్ఞా((size_t) (z - buf) <= len);

        ఒకవేళ (ret_కొలత)
                *ret_కొలత = (size_t) (z - buf);
        ఒకవేళ (ret_దత్తాంశం)
                *ret_దత్తాంశం = TAKE_PTR(buf);

        తిరిగి 0;
}

శూన్య hexdump(FILE *f, స్థిరరాశి శూన్య *p, size_t s) {
        స్థిరరాశి uint8_t *b = p;
        గుర్తులేని n = 0;

        ప్రతిజ్ఞా(b || s == 0);

        ఒకవేళ (s == SIZE_MAX)
                s = అక్షరశ్రేణి_పొడవు(p);

        ఒకవేళ (!f)
                f = stdout;

        అయితే (s > 0) {
                size_t i;

                fprintf(f, "%04x  ", n);

                కొరకు (i = 0; i < 16; i++) {

                        ఒకవేళ (i >= s)
                                fputs("   ", f);
                        లేకపోతే
                                fprintf(f, "%02x ", b[i]);

                        ఒకవేళ (i == 7)
                                fputc(' ', f);
                }

                fputc(' ', f);

                కొరకు (i = 0; i < 16; i++) {

                        ఒకవేళ (i >= s)
                                fputc(' ', f);
                        లేకపోతే
                                fputc(isprint(b[i]) ? (అక్షరం) b[i] : '.', f);
                }

                fputc('\n', f);

                ఒకవేళ (s < 16)
                        ఆపు;

                n += 16;
                b += 16;
                s -= 16;
        }
}
