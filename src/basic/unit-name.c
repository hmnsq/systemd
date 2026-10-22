/* SPDX-License-Identifier: LGPL-2.1-or-later */

#అన్తర్భావ "sd-id128.h"

#అన్తర్భావ "alloc-util.h"
#అన్తర్భావ "glob-util.h"
#అన్తర్భావ "hexdecoct.h"
#అన్తర్భావ "log.h"
#అన్తర్భావ "memory-util.h"
#అన్తర్భావ "path-util.h"
#అన్తర్భావ "sparse-endian.h"
#అన్తర్భావ "special.h"
#అన్తర్భావ "siphash24.h"
#అన్తర్భావ "string-util.h"
#అన్తర్భావ "unit-def.h"
#అన్తర్భావ "unit-name.h"

/* Characters valid in a unit name. */
#నిర్వచించు VALID_CHARS                             \
        ALPHANUMERICAL                          \
        ":-_.\\"

/* The same, but also permits the single @ character that may appear */
#నిర్వచించు VALID_CHARS_WITH_AT                     \
        "@"                                     \
        VALID_CHARS

/* All chars valid in a unit name glob */
#నిర్వచించు VALID_CHARS_GLOB                        \
        VALID_CHARS_WITH_AT                     \
        "[]!-*?"

#నిర్వచించు LONG_UNIT_NAME_HASH_కీలకం SD_ID128_MAKE(ec,f2,37,fb,58,32,4a,32,84,9f,06,9b,0d,21,eb,9a)
#నిర్వచించు UNIT_NAME_HASH_నిడివి_CHARS 16

bool unit_name_is_valid(స్థిరరాశి అక్షరం *n, UnitNameFlags flags) {
        స్థిరరాశి అక్షరం *e, *i, *at;

        ప్రతిజ్ఞా((flags & ~(UNIT_NAME_PLAIN|UNIT_NAME_INSTANCE|UNIT_NAME_TEMPLATE)) == 0);

        ఒకవేళ (_unlikely_(flags == 0))
                తిరిగి false;

        ఒకవేళ (isempty(n))
                తిరిగి false;

        ఒకవేళ (అక్షరశ్రేణి_పొడవు(n) >= UNIT_NAME_MAX)
                తిరిగి false;

        e = strrchr(n, '.');
        ఒకవేళ (!e || e == n)
                తిరిగి false;

        ఒకవేళ (unit_type_from_string(e + 1) < 0)
                తిరిగి false;

        కొరకు (i = n, at = ఖాళీ; i < e; i++) {

                ఒకవేళ (*i == '@' && !at)
                        at = i;

                ఒకవేళ (!strchr(VALID_CHARS_WITH_AT, *i))
                        తిరిగి false;
        }

        ఒకవేళ (at == n)
                తిరిగి false;

        ఒకవేళ (flags & UNIT_NAME_PLAIN)
                ఒకవేళ (!at)
                        తిరిగి true;

        ఒకవేళ (flags & UNIT_NAME_INSTANCE)
                ఒకవేళ (at && e > at + 1)
                        తిరిగి true;

        ఒకవేళ (flags & UNIT_NAME_TEMPLATE)
                ఒకవేళ (at && e == at + 1)
                        తిరిగి true;

        తిరిగి false;
}

bool unit_ఉపసర్గ_is_valid(స్థిరరాశి అక్షరం *p) {

        /* We don't allow additional @ in the prefix string */

        ఒకవేళ (isempty(p))
                తిరిగి false;

        తిరిగి లోపల_charset(p, VALID_CHARS);
}

bool unit_instance_is_valid(స్థిరరాశి అక్షరం *i) {

        /* The max length depends on the length of the string, so we
         * don't really check this here. */

        ఒకవేళ (isempty(i))
                తిరిగి false;

        /* We allow additional @ in the instance string, we do not
         * allow them in the prefix! */

        తిరిగి లోపల_charset(i, "@" VALID_CHARS);
}

bool unit_suffix_is_valid(స్థిరరాశి అక్షరం *s) {
        ఒకవేళ (isempty(s))
                తిరిగి false;

        ఒకవేళ (s[0] != '.')
                తిరిగి false;

        ఒకవేళ (unit_type_from_string(s + 1) < 0)
                తిరిగి false;

        తిరిగి true;
}

పూర్ణాంకం unit_name_గమ్యం_ఉపసర్గ(స్థిరరాశి అక్షరం *n, అక్షరం **ret) {
        స్థిరరాశి అక్షరం *p;
        అక్షరం *s;

        ప్రతిజ్ఞా(n);
        ప్రతిజ్ఞా(ret);

        ఒకవేళ (!unit_name_is_valid(n, UNIT_NAME_ANY))
                తిరిగి -EINVAL;

        p = strchr(n, '@');
        ఒకవేళ (!p)
                p = strrchr(n, '.');

        assert_se(p);

        s = strndup(n, p - n);
        ఒకవేళ (!s)
                తిరిగి -ENOMEM;

        *ret = s;
        తిరిగి 0;
}

UnitNameFlags unit_name_గమ్యం_instance(స్థిరరాశి అక్షరం *n, అక్షరం **ret) {
        స్థిరరాశి అక్షరం *p, *d;

        ప్రతిజ్ఞా(n);

        ఒకవేళ (!unit_name_is_valid(n, UNIT_NAME_ANY))
                తిరిగి -EINVAL;

        /* Everything past the first @ and before the last . is the instance */
        p = strchr(n, '@');
        ఒకవేళ (!p) {
                ఒకవేళ (ret)
                        *ret = ఖాళీ;
                తిరిగి UNIT_NAME_PLAIN;
        }

        p++;

        d = strrchr(p, '.');
        ఒకవేళ (!d)
                తిరిగి -EINVAL;

        ఒకవేళ (ret) {
                అక్షరం *i = strndup(p, d-p);
                ఒకవేళ (!i)
                        తిరిగి -ENOMEM;

                *ret = i;
        }
        తిరిగి d > p ? UNIT_NAME_INSTANCE : UNIT_NAME_TEMPLATE;
}

పూర్ణాంకం unit_name_గమ్యం_ఉపసర్గ_and_instance(స్థిరరాశి అక్షరం *n, అక్షరం **ret) {
        స్థిరరాశి అక్షరం *d;
        అక్షరం *s;

        ప్రతిజ్ఞా(n);
        ప్రతిజ్ఞా(ret);

        ఒకవేళ (!unit_name_is_valid(n, UNIT_NAME_ANY))
                తిరిగి -EINVAL;

        d = strrchr(n, '.');
        ఒకవేళ (!d)
                తిరిగి -EINVAL;

        s = strndup(n, d - n);
        ఒకవేళ (!s)
                తిరిగి -ENOMEM;

        *ret = s;
        తిరిగి 0;
}

UnitType unit_name_గమ్యం_type(స్థిరరాశి అక్షరం *n) {
        స్థిరరాశి అక్షరం *e;

        ప్రతిజ్ఞా(n);

        ఒకవేళ (!unit_name_is_valid(n, UNIT_NAME_ANY))
                తిరిగి _UNIT_TYPE_INVALID;

        assert_se(e = strrchr(n, '.'));

        తిరిగి unit_type_from_string(e + 1);
}

పూర్ణాంకం unit_name_change_suffix(స్థిరరాశి అక్షరం *n, స్థిరరాశి అక్షరం *suffix, అక్షరం **ret) {
        _cleanup_free_ అక్షరం *s = ఖాళీ;
        size_t a, b;
        స్థిరరాశి అక్షరం *e;

        ప్రతిజ్ఞా(n);
        ప్రతిజ్ఞా(suffix);
        ప్రతిజ్ఞా(ret);

        ఒకవేళ (!unit_name_is_valid(n, UNIT_NAME_ANY))
                తిరిగి -EINVAL;

        ఒకవేళ (!unit_suffix_is_valid(suffix))
                తిరిగి -EINVAL;

        assert_se(e = strrchr(n, '.'));

        a = e - n;
        b = అక్షరశ్రేణి_పొడవు(suffix);

        s = new(అక్షరం, a + b + 1);
        ఒకవేళ (!s)
                తిరిగి -ENOMEM;

        strcpy(mempcpy(s, n, a), suffix);

        /* Make sure the name is still valid (i.e. didn't grow too large due to longer suffix) */
        ఒకవేళ (!unit_name_is_valid(s, UNIT_NAME_ANY))
                తిరిగి -EINVAL;

        *ret = TAKE_PTR(s);
        తిరిగి 0;
}

పూర్ణాంకం unit_name_build(స్థిరరాశి అక్షరం *ఉపసర్గ, స్థిరరాశి అక్షరం *instance, స్థిరరాశి అక్షరం *suffix, అక్షరం **ret) {
        UnitType type;

        ప్రతిజ్ఞా(ఉపసర్గ);
        ప్రతిజ్ఞా(suffix);
        ప్రతిజ్ఞా(ret);

        ఒకవేళ (suffix[0] != '.')
                తిరిగి -EINVAL;

        type = unit_type_from_string(suffix + 1);
        ఒకవేళ (type < 0)
                తిరిగి type;

        తిరిగి unit_name_build_from_type(ఉపసర్గ, instance, type, ret);
}

పూర్ణాంకం unit_name_build_from_type(స్థిరరాశి అక్షరం *ఉపసర్గ, స్థిరరాశి అక్షరం *instance, UnitType type, అక్షరం **ret) {
        _cleanup_free_ అక్షరం *s = ఖాళీ;
        స్థిరరాశి అక్షరం *ut;

        ప్రతిజ్ఞా(ఉపసర్గ);
        ప్రతిజ్ఞా(type >= 0);
        ప్రతిజ్ఞా(type < _UNIT_TYPE_MAX);
        ప్రతిజ్ఞా(ret);

        ఒకవేళ (!unit_ఉపసర్గ_is_valid(ఉపసర్గ))
                తిరిగి -EINVAL;

        ut = unit_type_గమ్యం_string(type);

        ఒకవేళ (instance) {
                ఒకవేళ (!unit_instance_is_valid(instance))
                        తిరిగి -EINVAL;

                s = strjoin(ఉపసర్గ, "@", instance, ".", ut);
        } లేకపోతే
                s = strjoin(ఉపసర్గ, ".", ut);
        ఒకవేళ (!s)
                తిరిగి -ENOMEM;

        /* Verify that this didn't grow too large (or otherwise is invalid) */
        ఒకవేళ (!unit_name_is_valid(s, instance ? UNIT_NAME_INSTANCE : UNIT_NAME_PLAIN))
                తిరిగి -EINVAL;

        *ret = TAKE_PTR(s);
        తిరిగి 0;
}

స్థిర అక్షరం *do_escape_char(అక్షరం c, అక్షరం *t) {
        ప్రతిజ్ఞా(t);

        *(t++) = '\\';
        *(t++) = 'x';
        *(t++) = hexchar(c >> 4);
        *(t++) = hexchar(c);

        తిరిగి t;
}

స్థిర అక్షరం *do_escape(స్థిరరాశి అక్షరం *f, అక్షరం *t) {
        ప్రతిజ్ఞా(f);
        ప్రతిజ్ఞా(t);

        /* do not create units with a leading '.', like for "/.dotdir" mount points */
        ఒకవేళ (*f == '.') {
                t = do_escape_char(*f, t);
                f++;
        }

        కొరకు (; *f; f++) {
                ఒకవేళ (*f == '/')
                        *(t++) = '-';
                లేకపోతే ఒకవేళ (లోపల_సమితి(*f, '-', '\\') || !strchr(VALID_CHARS, *f))
                        t = do_escape_char(*f, t);
                లేకపోతే
                        *(t++) = *f;
        }

        తిరిగి t;
}

అక్షరం* unit_name_escape(స్థిరరాశి అక్షరం *f) {
        అక్షరం *r, *t;

        ప్రతిజ్ఞా(f);

        r = new(అక్షరం, అక్షరశ్రేణి_పొడవు(f)*4+1);
        ఒకవేళ (!r)
                తిరిగి ఖాళీ;

        t = do_escape(f, r);
        *t = 0;

        తిరిగి r;
}

పూర్ణాంకం unit_name_unescape(స్థిరరాశి అక్షరం *f, అక్షరం **ret) {
        _cleanup_free_ అక్షరం *r = ఖాళీ;
        అక్షరం *t;

        ప్రతిజ్ఞా(f);
        ప్రతిజ్ఞా(ret);

        r = strdup(f);
        ఒకవేళ (!r)
                తిరిగి -ENOMEM;

        కొరకు (t = r; *f; f++) {
                ఒకవేళ (*f == '-')
                        *(t++) = '/';
                లేకపోతే ఒకవేళ (*f == '\\') {
                        పూర్ణాంకం a, b;

                        ఒకవేళ (f[1] != 'x')
                                తిరిగి -EINVAL;

                        a = unhexchar(f[2]);
                        ఒకవేళ (a < 0)
                                తిరిగి -EINVAL;

                        b = unhexchar(f[3]);
                        ఒకవేళ (b < 0)
                                తిరిగి -EINVAL;

                        *(t++) = (అక్షరం) (((uint8_t) a << 4U) | (uint8_t) b);
                        f += 3;
                } లేకపోతే
                        *(t++) = *f;
        }

        *t = 0;

        *ret = TAKE_PTR(r);

        తిరిగి 0;
}

పూర్ణాంకం unit_name_మార్గం_escape(స్థిరరాశి అక్షరం *f, అక్షరం **ret) {
        _cleanup_free_ అక్షరం *p = ఖాళీ;
        అక్షరం *s;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(f);
        ప్రతిజ్ఞా(ret);

        r = మార్గం_simplify_alloc(f, &p);
        ఒకవేళ (r < 0)
                తిరిగి r;

        ఒకవేళ (ఖాళీలు_or_మూలం(p))
                s = strdup("-");
        లేకపోతే {
                ఒకవేళ (!మార్గం_is_normalized(p))
                        తిరిగి -EINVAL;

                /* Truncate trailing slashes and skip leading slashes */
                delete_trailing_chars(p, "/");
                s = unit_name_escape(skip_leading_chars(p, "/"));
        }
        ఒకవేళ (!s)
                తిరిగి -ENOMEM;

        *ret = s;
        తిరిగి 0;
}

పూర్ణాంకం unit_name_మార్గం_unescape(స్థిరరాశి అక్షరం *f, అక్షరం **ret) {
        _cleanup_free_ అక్షరం *s = ఖాళీ;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(f);

        ఒకవేళ (isempty(f))
                తిరిగి -EINVAL;

        ఒకవేళ (streq(f, "-")) {
                s = strdup("/");
                ఒకవేళ (!s)
                        తిరిగి -ENOMEM;
        } లేకపోతే {
                _cleanup_free_ అక్షరం *w = ఖాళీ;

                r = unit_name_unescape(f, &w);
                ఒకవేళ (r < 0)
                        తిరిగి r;

                /* Don't accept trailing or leading slashes */
                ఒకవేళ (startswith(w, "/") || endswith(w, "/"))
                        తిరిగి -EINVAL;

                /* Prefix a slash again */
                s = strjoin("/", w);
                ఒకవేళ (!s)
                        తిరిగి -ENOMEM;

                ఒకవేళ (!మార్గం_is_normalized(s))
                        తిరిగి -EINVAL;
        }

        ఒకవేళ (ret)
                *ret = TAKE_PTR(s);

        తిరిగి 0;
}

పూర్ణాంకం unit_name_replace_instance_నిండిన(
                స్థిరరాశి అక్షరం *original,
                స్థిరరాశి అక్షరం *instance,
                bool accept_glob,
                అక్షరం **ret) {

        _cleanup_free_ అక్షరం *s = ఖాళీ;
        స్థిరరాశి అక్షరం *ఉపసర్గ, *suffix;
        size_t pl;

        ప్రతిజ్ఞా(original);
        ప్రతిజ్ఞా(instance);
        ప్రతిజ్ఞా(ret);

        ఒకవేళ (!unit_name_is_valid(original, UNIT_NAME_INSTANCE|UNIT_NAME_TEMPLATE))
                తిరిగి -EINVAL;
        ఒకవేళ (!unit_instance_is_valid(instance) && !(accept_glob && లోపల_charset(instance, VALID_CHARS_GLOB)))
                తిరిగి -EINVAL;

        ఉపసర్గ = ASSERT_PTR(strchr(original, '@'));
        suffix = ASSERT_PTR(strrchr(original, '.'));
        ప్రతిజ్ఞా(ఉపసర్గ < suffix);

        pl = ఉపసర్గ - original + 1; /* include '@' */

        s = new(అక్షరం, pl + అక్షరశ్రేణి_పొడవు(instance) + అక్షరశ్రేణి_పొడవు(suffix) + 1);
        ఒకవేళ (!s)
                తిరిగి -ENOMEM;

#ఒకవేళ HAS_FEATURE_జ్ఞాపకం_SANITIZER
        /* MSan doesn't like stpncpy... See also https://github.com/google/sanitizers/issues/926 */
        memzero(s, pl + అక్షరశ్రేణి_పొడవు(instance) + అక్షరశ్రేణి_పొడవు(suffix) + 1);
#అయితే_ముగింపు

        strcpy(stpcpy(stpncpy(s, original, pl), instance), suffix);

        /* Make sure the resulting name still is valid, i.e. didn't grow too large. Globs will be expanded
         * by clients when used, so the check is pointless. */
        ఒకవేళ (!accept_glob && !unit_name_is_valid(s, UNIT_NAME_INSTANCE))
                తిరిగి -EINVAL;

        *ret = TAKE_PTR(s);
        తిరిగి 0;
}

పూర్ణాంకం unit_name_template(స్థిరరాశి అక్షరం *f, అక్షరం **ret) {
        స్థిరరాశి అక్షరం *p, *e;
        అక్షరం *s;
        size_t a;

        ప్రతిజ్ఞా(f);
        ప్రతిజ్ఞా(ret);

        ఒకవేళ (!unit_name_is_valid(f, UNIT_NAME_INSTANCE|UNIT_NAME_TEMPLATE))
                తిరిగి -EINVAL;

        assert_se(p = strchr(f, '@'));
        assert_se(e = strrchr(f, '.'));

        a = p - f;

        s = new(అక్షరం, a + 1 + అక్షరశ్రేణి_పొడవు(e) + 1);
        ఒకవేళ (!s)
                తిరిగి -ENOMEM;

        strcpy(mempcpy(s, f, a + 1), e);

        *ret = s;
        తిరిగి 0;
}

bool unit_name_is_hashed(స్థిరరాశి అక్షరం *name) {
        స్థిరరాశి అక్షరం *s;

        ఒకవేళ (!unit_name_is_valid(name, UNIT_NAME_PLAIN))
                తిరిగి false;

        assert_se(s = strrchr(name, '.'));

        ఒకవేళ (s - name < UNIT_NAME_HASH_నిడివి_CHARS + 1)
                తిరిగి false;

        s -= UNIT_NAME_HASH_నిడివి_CHARS;
        ఒకవేళ (s[-1] != '_')
                తిరిగి false;

        కొరకు (size_t i = 0; i < UNIT_NAME_HASH_నిడివి_CHARS; i++)
                ఒకవేళ (!strchr(LOWERCASE_HEXDIGITS, s[i]))
                        తిరిగి false;

        తిరిగి true;
}

పూర్ణాంకం unit_name_hash_long(స్థిరరాశి అక్షరం *name, అక్షరం **ret) {
        _cleanup_free_ అక్షరం *n = ఖాళీ, *hash = ఖాళీ;
        స్థిరరాశి అక్షరం *suffix;
        le64_t h;
        size_t len;

        ప్రతిజ్ఞా(ret);

        ఒకవేళ (అక్షరశ్రేణి_పొడవు(name) < UNIT_NAME_MAX)
                తిరిగి -EMSGSIZE;

        suffix = strrchr(name, '.');
        ఒకవేళ (!suffix)
                తిరిగి -EINVAL;

        ఒకవేళ (unit_type_from_string(suffix+1) < 0)
                తిరిగి -EINVAL;

        h = htole64(siphash24_string(name, LONG_UNIT_NAME_HASH_కీలకం.బైట్లు));

        hash = hexmem(&h, పరిమాణం(h));
        ఒకవేళ (!hash)
                తిరిగి -ENOMEM;

        assert_se(అక్షరశ్రేణి_పొడవు(hash) == UNIT_NAME_HASH_నిడివి_CHARS);

        len = UNIT_NAME_MAX - 1 - అక్షరశ్రేణి_పొడవు(suffix+1) - UNIT_NAME_HASH_నిడివి_CHARS - 2;
        ప్రతిజ్ఞా(len > 0 && len < UNIT_NAME_MAX);

        n = strndup(name, len);
        ఒకవేళ (!n)
                తిరిగి -ENOMEM;

        ఒకవేళ (!strextend(&n, "_", hash, suffix))
                తిరిగి -ENOMEM;
        assert_se(unit_name_is_valid(n, UNIT_NAME_PLAIN));

        *ret = TAKE_PTR(n);

        తిరిగి 0;
}

పూర్ణాంకం unit_name_from_మార్గం(స్థిరరాశి అక్షరం *మార్గం, స్థిరరాశి అక్షరం *suffix, అక్షరం **ret) {
        _cleanup_free_ అక్షరం *p = ఖాళీ, *s = ఖాళీ;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(మార్గం);
        ప్రతిజ్ఞా(suffix);
        ప్రతిజ్ఞా(ret);

        ఒకవేళ (!unit_suffix_is_valid(suffix))
                తిరిగి -EINVAL;

        r = unit_name_మార్గం_escape(మార్గం, &p);
        ఒకవేళ (r < 0)
                తిరిగి r;

        s = strjoin(p, suffix);
        ఒకవేళ (!s)
                తిరిగి -ENOMEM;

        ఒకవేళ (అక్షరశ్రేణి_పొడవు(s) >= UNIT_NAME_MAX) {
                _cleanup_free_ అక్షరం *n = ఖాళీ;

                log_debug("Unit name \"%s\" too long, falling back to hashed unit name.", s);

                r = unit_name_hash_long(s, &n);
                ఒకవేళ (r < 0)
                        తిరిగి r;

                free_and_replace(s, n);
        }

        /* Refuse if this for some other reason didn't result in a valid name */
        ఒకవేళ (!unit_name_is_valid(s, UNIT_NAME_PLAIN))
                తిరిగి -EINVAL;

        *ret = TAKE_PTR(s);
        తిరిగి 0;
}

పూర్ణాంకం unit_name_from_మార్గం_instance(స్థిరరాశి అక్షరం *ఉపసర్గ, స్థిరరాశి అక్షరం *మార్గం, స్థిరరాశి అక్షరం *suffix, అక్షరం **ret) {
        _cleanup_free_ అక్షరం *p = ఖాళీ, *s = ఖాళీ;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(ఉపసర్గ);
        ప్రతిజ్ఞా(మార్గం);
        ప్రతిజ్ఞా(suffix);
        ప్రతిజ్ఞా(ret);

        ఒకవేళ (!unit_ఉపసర్గ_is_valid(ఉపసర్గ))
                తిరిగి -EINVAL;

        ఒకవేళ (!unit_suffix_is_valid(suffix))
                తిరిగి -EINVAL;

        r = unit_name_మార్గం_escape(మార్గం, &p);
        ఒకవేళ (r < 0)
                తిరిగి r;

        s = strjoin(ఉపసర్గ, "@", p, suffix);
        ఒకవేళ (!s)
                తిరిగి -ENOMEM;

        ఒకవేళ (అక్షరశ్రేణి_పొడవు(s) >= UNIT_NAME_MAX) /* Return a slightly more descriptive error for this specific condition */
                తిరిగి -ENAMETOOLONG;

        /* Refuse if this for some other reason didn't result in a valid name */
        ఒకవేళ (!unit_name_is_valid(s, UNIT_NAME_INSTANCE))
                తిరిగి -EINVAL;

        *ret = TAKE_PTR(s);
        తిరిగి 0;
}

పూర్ణాంకం unit_name_గమ్యం_మార్గం(స్థిరరాశి అక్షరం *name, అక్షరం **ret) {
        _cleanup_free_ అక్షరం *ఉపసర్గ = ఖాళీ;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(name);

        r = unit_name_గమ్యం_ఉపసర్గ(name, &ఉపసర్గ);
        ఒకవేళ (r < 0)
                తిరిగి r;

        ఒకవేళ (unit_name_is_hashed(name))
                తిరిగి -ENAMETOOLONG;

        తిరిగి unit_name_మార్గం_unescape(ఉపసర్గ, ret);
}

స్థిర bool do_escape_mangle(స్థిరరాశి అక్షరం *f, bool allow_globs, అక్షరం *t) {
        స్థిరరాశి అక్షరం *valid_chars;
        bool mangled = false;

        ప్రతిజ్ఞా(f);
        ప్రతిజ్ఞా(t);

        /* We'll only escape the obvious characters here, to play safe.
         *
         * Returns true if any characters were mangled, false otherwise.
         */

        valid_chars = allow_globs ? VALID_CHARS_GLOB : VALID_CHARS_WITH_AT;

        కొరకు (; *f; f++)
                ఒకవేళ (*f == '/') {
                        *(t++) = '-';
                        mangled = true;
                } లేకపోతే ఒకవేళ (!strchr(valid_chars, *f)) {
                        t = do_escape_char(*f, t);
                        mangled = true;
                } లేకపోతే
                        *(t++) = *f;
        *t = 0;

        తిరిగి mangled;
}

/**
 *  Convert a string to a unit name. /dev/blah is converted to dev-blah.device,
 *  /blah/blah is converted to blah-blah.mount, anything else is left alone,
 *  except that @suffix is appended if a valid unit suffix is not present.
 *
 *  If @allow_globs, globs characters are preserved. Otherwise, they are escaped.
 */
పూర్ణాంకం unit_name_mangle_with_suffix(
                స్థిరరాశి అక్షరం *name,
                స్థిరరాశి అక్షరం *operation,
                UnitNameMangle flags,
                స్థిరరాశి అక్షరం *suffix,
                అక్షరం **ret) {

        _cleanup_free_ అక్షరం *s = ఖాళీ;
        bool mangled, suggest_escape = true, warn = FLAGS_సమితి(flags, UNIT_NAME_MANGLE_WARN);
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(name);
        ప్రతిజ్ఞా(suffix);
        ప్రతిజ్ఞా(ret);

        ఒకవేళ (isempty(name)) /* We cannot mangle empty unit names to become valid, sorry. */
                తిరిగి -EINVAL;

        ఒకవేళ (!unit_suffix_is_valid(suffix))
                తిరిగి -EINVAL;

        /* Already a fully valid unit name? If so, no mangling is necessary... */
        ఒకవేళ (unit_name_is_valid(name, UNIT_NAME_ANY)) {
                ఒకవేళ (FLAGS_సమితి(flags, UNIT_NAME_MANGLE_STRICT) && !endswith(name, suffix)) {
                        స్థిరరాశి అక్షరం *e = ASSERT_PTR(strrchr(name, '.'));

                        తిరిగి log_నిండిన_errno(warn ? LOG_NOTICE : LOG_DEBUG,
                                              SYNTHETIC_ERRNO(EINVAL),
                                              "Unit name \"%s\" has unit type \"%s\", but \"%s\" is expected%s%s.",
                                              name, e + 1, suffix + 1,
                                              operation ? " " : "", strempty(operation));
                }

                వెళ్ళు good;
        }

        /* Already a fully valid globbing expression? If so, no mangling is necessary either... */
        ఒకవేళ (string_is_glob(name) && లోపల_charset(name, VALID_CHARS_GLOB)) {
                ఒకవేళ (FLAGS_సమితి(flags, UNIT_NAME_MANGLE_GLOB))
                        వెళ్ళు good;
                log_నిండిన(warn ? LOG_NOTICE : LOG_DEBUG,
                         "Glob pattern passed%s%s, but globs are not supported for this.",
                         operation ? " " : "", strempty(operation));
                suggest_escape = false;
        }

        ఒకవేళ (మార్గం_is_absolute(name)) {
                _cleanup_free_ అక్షరం *n = ఖాళీ, *u = ఖాళీ;

                r = మార్గం_simplify_alloc(name, &n);
                ఒకవేళ (r < 0)
                        తిరిగి r;

                ఒకవేళ (is_device_మార్గం(n)) {
                        r = unit_name_from_మార్గం(n, ".device", &u);
                        ఒకవేళ (r >= 0) {
                                ఒకవేళ (FLAGS_సమితి(flags, UNIT_NAME_MANGLE_STRICT) && !streq(suffix, ".device"))
                                        తిరిగి log_నిండిన_errno(warn ? LOG_NOTICE : LOG_DEBUG,
                                                              SYNTHETIC_ERRNO(EINVAL),
                                                              "Path \"%s\" resolves to unit type \"device\", but \"%s\" is expected%s%s.",
                                                              name, suffix + 1,
                                                              operation ? " " : "", strempty(operation));

                                *ret = TAKE_PTR(u);
                                తిరిగి 1;
                        }
                        ఒకవేళ (r != -EINVAL)
                                తిరిగి r;
                }

                r = unit_name_from_మార్గం(n, ".mount", &u);
                ఒకవేళ (r >= 0) {
                        ఒకవేళ (FLAGS_సమితి(flags, UNIT_NAME_MANGLE_STRICT) && !streq(suffix, ".mount"))
                                తిరిగి log_నిండిన_errno(warn ? LOG_NOTICE : LOG_DEBUG,
                                                      SYNTHETIC_ERRNO(EINVAL),
                                                      "Path \"%s\" resolves to unit type \"mount\", but \"%s\" is expected%s%s.",
                                                      name, suffix + 1,
                                                      operation ? " " : "", strempty(operation));

                        *ret = TAKE_PTR(u);
                        తిరిగి 1;
                }
                ఒకవేళ (r != -EINVAL)
                        తిరిగి r;
        }

        s = new(అక్షరం, అక్షరశ్రేణి_పొడవు(name) * 4 + అక్షరశ్రేణి_పొడవు(suffix) + 1);
        ఒకవేళ (!s)
                తిరిగి -ENOMEM;

        mangled = do_escape_mangle(name, FLAGS_సమితి(flags, UNIT_NAME_MANGLE_GLOB), s);
        ఒకవేళ (mangled)
                log_నిండిన(warn ? LOG_NOTICE : LOG_DEBUG,
                         "Invalid unit name \"%s\" escaped as \"%s\"%s.",
                         name, s,
                         suggest_escape ? " (maybe you should use systemd-escape?)" : "");

        /* Append a suffix if it doesn't have any, but only if this is not a glob, so that we can allow
         * "foo.*" as a valid glob. */
        ఒకవేళ ((!FLAGS_సమితి(flags, UNIT_NAME_MANGLE_GLOB) || !string_is_glob(s)) && unit_name_గమ్యం_type(s) < 0)
                strcat(s, suffix);

        /* Make sure mangling didn't grow this too large (but don't do this check if globbing is allowed,
         * since globs generally do not qualify as valid unit names) */
        ఒకవేళ (!FLAGS_సమితి(flags, UNIT_NAME_MANGLE_GLOB) && !unit_name_is_valid(s, UNIT_NAME_ANY))
                తిరిగి -EINVAL;

        *ret = TAKE_PTR(s);
        తిరిగి 1;

good:
        తిరిగి strdup_గమ్యం(ret, name);
}

పూర్ణాంకం ముక్క_build_మాతృ_ముక్క(స్థిరరాశి అక్షరం *ముక్క, అక్షరం **ret) {
        ప్రతిజ్ఞా(ముక్క);
        ప్రతిజ్ఞా(ret);

        ఒకవేళ (!ముక్క_name_is_valid(ముక్క))
                తిరిగి -EINVAL;

        ఒకవేళ (streq(ముక్క, SPECIAL_మూలం_ముక్క)) {
                *ret = ఖాళీ;
                తిరిగి 0;
        }

        _cleanup_free_ అక్షరం *s = strdup(ముక్క);
        ఒకవేళ (!s)
                తిరిగి -ENOMEM;

        అక్షరం *dash = strrchr(s, '-');
        ఒకవేళ (!dash)
                తిరిగి strdup_గమ్యం_నిండిన(ret, SPECIAL_మూలం_ముక్క);

        /* We know that s ended with .slice before truncation, so we have enough space. */
        strcpy(dash, ".slice");

        *ret = TAKE_PTR(s);
        తిరిగి 1;
}

పూర్ణాంకం ముక్క_build_subslice(స్థిరరాశి అక్షరం *ముక్క, స్థిరరాశి అక్షరం *name, అక్షరం **ret) {
        అక్షరం *subslice;

        ప్రతిజ్ఞా(ముక్క);
        ప్రతిజ్ఞా(name);
        ప్రతిజ్ఞా(ret);

        ఒకవేళ (!ముక్క_name_is_valid(ముక్క))
                తిరిగి -EINVAL;

        ఒకవేళ (!unit_ఉపసర్గ_is_valid(name))
                తిరిగి -EINVAL;

        ఒకవేళ (streq(ముక్క, SPECIAL_మూలం_ముక్క))
                subslice = strjoin(name, ".slice");
        లేకపోతే {
                స్థిరరాశి అక్షరం *e;

                assert_se(e = endswith(ముక్క, ".slice"));

                subslice = new(అక్షరం, (e - ముక్క) + 1 + అక్షరశ్రేణి_పొడవు(name) + 6 + 1);
                ఒకవేళ (!subslice)
                        తిరిగి -ENOMEM;

                stpcpy(stpcpy(stpcpy(mempcpy(subslice, ముక్క, e - ముక్క), "-"), name), ".slice");
        }

        *ret = subslice;
        తిరిగి 0;
}

bool ముక్క_name_is_valid(స్థిరరాశి అక్షరం *name) {
        స్థిరరాశి అక్షరం *p, *e;
        bool dash = false;

        ఒకవేళ (!unit_name_is_valid(name, UNIT_NAME_PLAIN))
                తిరిగి false;

        ఒకవేళ (streq(name, SPECIAL_మూలం_ముక్క))
                తిరిగి true;

        e = endswith(name, ".slice");
        ఒకవేళ (!e)
                తిరిగి false;

        కొరకు (p = name; p < e; p++) {

                ఒకవేళ (*p == '-') {

                        /* Don't allow initial dash */
                        ఒకవేళ (p == name)
                                తిరిగి false;

                        /* Don't allow multiple dashes */
                        ఒకవేళ (dash)
                                తిరిగి false;

                        dash = true;
                } లేకపోతే
                        dash = false;
        }

        /* Don't allow trailing hash */
        ఒకవేళ (dash)
                తిరిగి false;

        తిరిగి true;
}

bool unit_name_ఉపసర్గ_equal(స్థిరరాశి అక్షరం *a, స్థిరరాశి అక్షరం *b) {
        స్థిరరాశి అక్షరం *p, *q;

        ప్రతిజ్ఞా(a);
        ప్రతిజ్ఞా(b);

        ఒకవేళ (!unit_name_is_valid(a, UNIT_NAME_ANY) || !unit_name_is_valid(b, UNIT_NAME_ANY))
                తిరిగి false;

        p = strchr(a, '@');
        ఒకవేళ (!p)
                p = strrchr(a, '.');

        q = strchr(b, '@');
        ఒకవేళ (!q)
                q = strrchr(b, '.');

        ప్రతిజ్ఞా(p);
        ప్రతిజ్ఞా(q);

        తిరిగి memcmp_nn(a, p - a, b, q - b) == 0;
}
