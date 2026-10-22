/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ਅਨ੍ਤਰ੍ਭਾਵ "sd-id128.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "alloc-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "glob-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hexdecoct.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "log.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "memory-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "path-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "sparse-endian.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "special.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "siphash24.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "string-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unit-def.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unit-name.h"

/* Characters valid in a unit name. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VALID_CHARS                             \
        ALPHANUMERICAL                          \
        ":-_.\\"

/* The same, but also permits the single @ character that may appear */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VALID_CHARS_WITH_AT                     \
        "@"                                     \
        VALID_CHARS

/* All chars valid in a unit name glob */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VALID_CHARS_GLOB                        \
        VALID_CHARS_WITH_AT                     \
        "[]!-*?"

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LONG_UNIT_NAME_HASH_ਕੁੰਜੀ SD_ID128_MAKE(ec,f2,37,fb,58,32,4a,32,84,9f,06,9b,0d,21,eb,9a)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UNIT_NAME_HASH_ਲੰਮਾਈ_CHARS 16

bool unit_name_is_valid(ਅਚਲ ਅੱਖਰ *n, UnitNameFlags flags) {
        ਅਚਲ ਅੱਖਰ *e, *i, *at;

        ਪ੍ਰਤਿਜ੍ਞਾ((flags & ~(UNIT_NAME_PLAIN|UNIT_NAME_INSTANCE|UNIT_NAME_TEMPLATE)) == 0);

        ਜੇ (_unlikely_(flags == 0))
                ਵਾਪਸ false;

        ਜੇ (isempty(n))
                ਵਾਪਸ false;

        ਜੇ (ਲੜੀ_ਲੰਬਾਈ(n) >= UNIT_NAME_MAX)
                ਵਾਪਸ false;

        e = strrchr(n, '.');
        ਜੇ (!e || e == n)
                ਵਾਪਸ false;

        ਜੇ (unit_type_from_string(e + 1) < 0)
                ਵਾਪਸ false;

        ਲਈ (i = n, at = ਨਿਰਾ; i < e; i++) {

                ਜੇ (*i == '@' && !at)
                        at = i;

                ਜੇ (!strchr(VALID_CHARS_WITH_AT, *i))
                        ਵਾਪਸ false;
        }

        ਜੇ (at == n)
                ਵਾਪਸ false;

        ਜੇ (flags & UNIT_NAME_PLAIN)
                ਜੇ (!at)
                        ਵਾਪਸ true;

        ਜੇ (flags & UNIT_NAME_INSTANCE)
                ਜੇ (at && e > at + 1)
                        ਵਾਪਸ true;

        ਜੇ (flags & UNIT_NAME_TEMPLATE)
                ਜੇ (at && e == at + 1)
                        ਵਾਪਸ true;

        ਵਾਪਸ false;
}

bool unit_ਅਗੇਤਰ_is_valid(ਅਚਲ ਅੱਖਰ *p) {

        /* We don't allow additional @ in the prefix string */

        ਜੇ (isempty(p))
                ਵਾਪਸ false;

        ਵਾਪਸ ਵਿੱਚ_charset(p, VALID_CHARS);
}

bool unit_instance_is_valid(ਅਚਲ ਅੱਖਰ *i) {

        /* The max length depends on the length of the string, so we
         * don't really check this here. */

        ਜੇ (isempty(i))
                ਵਾਪਸ false;

        /* We allow additional @ in the instance string, we do not
         * allow them in the prefix! */

        ਵਾਪਸ ਵਿੱਚ_charset(i, "@" VALID_CHARS);
}

bool unit_suffix_is_valid(ਅਚਲ ਅੱਖਰ *s) {
        ਜੇ (isempty(s))
                ਵਾਪਸ false;

        ਜੇ (s[0] != '.')
                ਵਾਪਸ false;

        ਜੇ (unit_type_from_string(s + 1) < 0)
                ਵਾਪਸ false;

        ਵਾਪਸ true;
}

ਪੂਰਨ unit_name_ਮੰਜ਼ਿਲ_ਅਗੇਤਰ(ਅਚਲ ਅੱਖਰ *n, ਅੱਖਰ **ret) {
        ਅਚਲ ਅੱਖਰ *p;
        ਅੱਖਰ *s;

        ਪ੍ਰਤਿਜ੍ਞਾ(n);
        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        ਜੇ (!unit_name_is_valid(n, UNIT_NAME_ANY))
                ਵਾਪਸ -EINVAL;

        p = strchr(n, '@');
        ਜੇ (!p)
                p = strrchr(n, '.');

        assert_se(p);

        s = strndup(n, p - n);
        ਜੇ (!s)
                ਵਾਪਸ -ENOMEM;

        *ret = s;
        ਵਾਪਸ 0;
}

UnitNameFlags unit_name_ਮੰਜ਼ਿਲ_instance(ਅਚਲ ਅੱਖਰ *n, ਅੱਖਰ **ret) {
        ਅਚਲ ਅੱਖਰ *p, *d;

        ਪ੍ਰਤਿਜ੍ਞਾ(n);

        ਜੇ (!unit_name_is_valid(n, UNIT_NAME_ANY))
                ਵਾਪਸ -EINVAL;

        /* Everything past the first @ and before the last . is the instance */
        p = strchr(n, '@');
        ਜੇ (!p) {
                ਜੇ (ret)
                        *ret = ਨਿਰਾ;
                ਵਾਪਸ UNIT_NAME_PLAIN;
        }

        p++;

        d = strrchr(p, '.');
        ਜੇ (!d)
                ਵਾਪਸ -EINVAL;

        ਜੇ (ret) {
                ਅੱਖਰ *i = strndup(p, d-p);
                ਜੇ (!i)
                        ਵਾਪਸ -ENOMEM;

                *ret = i;
        }
        ਵਾਪਸ d > p ? UNIT_NAME_INSTANCE : UNIT_NAME_TEMPLATE;
}

ਪੂਰਨ unit_name_ਮੰਜ਼ਿਲ_ਅਗੇਤਰ_and_instance(ਅਚਲ ਅੱਖਰ *n, ਅੱਖਰ **ret) {
        ਅਚਲ ਅੱਖਰ *d;
        ਅੱਖਰ *s;

        ਪ੍ਰਤਿਜ੍ਞਾ(n);
        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        ਜੇ (!unit_name_is_valid(n, UNIT_NAME_ANY))
                ਵਾਪਸ -EINVAL;

        d = strrchr(n, '.');
        ਜੇ (!d)
                ਵਾਪਸ -EINVAL;

        s = strndup(n, d - n);
        ਜੇ (!s)
                ਵਾਪਸ -ENOMEM;

        *ret = s;
        ਵਾਪਸ 0;
}

UnitType unit_name_ਮੰਜ਼ਿਲ_type(ਅਚਲ ਅੱਖਰ *n) {
        ਅਚਲ ਅੱਖਰ *e;

        ਪ੍ਰਤਿਜ੍ਞਾ(n);

        ਜੇ (!unit_name_is_valid(n, UNIT_NAME_ANY))
                ਵਾਪਸ _UNIT_TYPE_INVALID;

        assert_se(e = strrchr(n, '.'));

        ਵਾਪਸ unit_type_from_string(e + 1);
}

ਪੂਰਨ unit_name_change_suffix(ਅਚਲ ਅੱਖਰ *n, ਅਚਲ ਅੱਖਰ *suffix, ਅੱਖਰ **ret) {
        _cleanup_free_ ਅੱਖਰ *s = ਨਿਰਾ;
        size_t a, b;
        ਅਚਲ ਅੱਖਰ *e;

        ਪ੍ਰਤਿਜ੍ਞਾ(n);
        ਪ੍ਰਤਿਜ੍ਞਾ(suffix);
        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        ਜੇ (!unit_name_is_valid(n, UNIT_NAME_ANY))
                ਵਾਪਸ -EINVAL;

        ਜੇ (!unit_suffix_is_valid(suffix))
                ਵਾਪਸ -EINVAL;

        assert_se(e = strrchr(n, '.'));

        a = e - n;
        b = ਲੜੀ_ਲੰਬਾਈ(suffix);

        s = new(ਅੱਖਰ, a + b + 1);
        ਜੇ (!s)
                ਵਾਪਸ -ENOMEM;

        strcpy(mempcpy(s, n, a), suffix);

        /* Make sure the name is still valid (i.e. didn't grow too large due to longer suffix) */
        ਜੇ (!unit_name_is_valid(s, UNIT_NAME_ANY))
                ਵਾਪਸ -EINVAL;

        *ret = TAKE_PTR(s);
        ਵਾਪਸ 0;
}

ਪੂਰਨ unit_name_build(ਅਚਲ ਅੱਖਰ *ਅਗੇਤਰ, ਅਚਲ ਅੱਖਰ *instance, ਅਚਲ ਅੱਖਰ *suffix, ਅੱਖਰ **ret) {
        UnitType type;

        ਪ੍ਰਤਿਜ੍ਞਾ(ਅਗੇਤਰ);
        ਪ੍ਰਤਿਜ੍ਞਾ(suffix);
        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        ਜੇ (suffix[0] != '.')
                ਵਾਪਸ -EINVAL;

        type = unit_type_from_string(suffix + 1);
        ਜੇ (type < 0)
                ਵਾਪਸ type;

        ਵਾਪਸ unit_name_build_from_type(ਅਗੇਤਰ, instance, type, ret);
}

ਪੂਰਨ unit_name_build_from_type(ਅਚਲ ਅੱਖਰ *ਅਗੇਤਰ, ਅਚਲ ਅੱਖਰ *instance, UnitType type, ਅੱਖਰ **ret) {
        _cleanup_free_ ਅੱਖਰ *s = ਨਿਰਾ;
        ਅਚਲ ਅੱਖਰ *ut;

        ਪ੍ਰਤਿਜ੍ਞਾ(ਅਗੇਤਰ);
        ਪ੍ਰਤਿਜ੍ਞਾ(type >= 0);
        ਪ੍ਰਤਿਜ੍ਞਾ(type < _UNIT_TYPE_MAX);
        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        ਜੇ (!unit_ਅਗੇਤਰ_is_valid(ਅਗੇਤਰ))
                ਵਾਪਸ -EINVAL;

        ut = unit_type_ਮੰਜ਼ਿਲ_string(type);

        ਜੇ (instance) {
                ਜੇ (!unit_instance_is_valid(instance))
                        ਵਾਪਸ -EINVAL;

                s = strjoin(ਅਗੇਤਰ, "@", instance, ".", ut);
        } ਨਹੀਂਤਾਂ
                s = strjoin(ਅਗੇਤਰ, ".", ut);
        ਜੇ (!s)
                ਵਾਪਸ -ENOMEM;

        /* Verify that this didn't grow too large (or otherwise is invalid) */
        ਜੇ (!unit_name_is_valid(s, instance ? UNIT_NAME_INSTANCE : UNIT_NAME_PLAIN))
                ਵਾਪਸ -EINVAL;

        *ret = TAKE_PTR(s);
        ਵਾਪਸ 0;
}

ਸਥਿਰ ਅੱਖਰ *do_escape_char(ਅੱਖਰ c, ਅੱਖਰ *t) {
        ਪ੍ਰਤਿਜ੍ਞਾ(t);

        *(t++) = '\\';
        *(t++) = 'x';
        *(t++) = hexchar(c >> 4);
        *(t++) = hexchar(c);

        ਵਾਪਸ t;
}

ਸਥਿਰ ਅੱਖਰ *do_escape(ਅਚਲ ਅੱਖਰ *f, ਅੱਖਰ *t) {
        ਪ੍ਰਤਿਜ੍ਞਾ(f);
        ਪ੍ਰਤਿਜ੍ਞਾ(t);

        /* do not create units with a leading '.', like for "/.dotdir" mount points */
        ਜੇ (*f == '.') {
                t = do_escape_char(*f, t);
                f++;
        }

        ਲਈ (; *f; f++) {
                ਜੇ (*f == '/')
                        *(t++) = '-';
                ਨਹੀਂਤਾਂ ਜੇ (ਵਿੱਚ_ਸੈੱਟ(*f, '-', '\\') || !strchr(VALID_CHARS, *f))
                        t = do_escape_char(*f, t);
                ਨਹੀਂਤਾਂ
                        *(t++) = *f;
        }

        ਵਾਪਸ t;
}

ਅੱਖਰ* unit_name_escape(ਅਚਲ ਅੱਖਰ *f) {
        ਅੱਖਰ *r, *t;

        ਪ੍ਰਤਿਜ੍ਞਾ(f);

        r = new(ਅੱਖਰ, ਲੜੀ_ਲੰਬਾਈ(f)*4+1);
        ਜੇ (!r)
                ਵਾਪਸ ਨਿਰਾ;

        t = do_escape(f, r);
        *t = 0;

        ਵਾਪਸ r;
}

ਪੂਰਨ unit_name_unescape(ਅਚਲ ਅੱਖਰ *f, ਅੱਖਰ **ret) {
        _cleanup_free_ ਅੱਖਰ *r = ਨਿਰਾ;
        ਅੱਖਰ *t;

        ਪ੍ਰਤਿਜ੍ਞਾ(f);
        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        r = strdup(f);
        ਜੇ (!r)
                ਵਾਪਸ -ENOMEM;

        ਲਈ (t = r; *f; f++) {
                ਜੇ (*f == '-')
                        *(t++) = '/';
                ਨਹੀਂਤਾਂ ਜੇ (*f == '\\') {
                        ਪੂਰਨ a, b;

                        ਜੇ (f[1] != 'x')
                                ਵਾਪਸ -EINVAL;

                        a = unhexchar(f[2]);
                        ਜੇ (a < 0)
                                ਵਾਪਸ -EINVAL;

                        b = unhexchar(f[3]);
                        ਜੇ (b < 0)
                                ਵਾਪਸ -EINVAL;

                        *(t++) = (ਅੱਖਰ) (((uint8_t) a << 4U) | (uint8_t) b);
                        f += 3;
                } ਨਹੀਂਤਾਂ
                        *(t++) = *f;
        }

        *t = 0;

        *ret = TAKE_PTR(r);

        ਵਾਪਸ 0;
}

ਪੂਰਨ unit_name_ਰਸਤਾ_escape(ਅਚਲ ਅੱਖਰ *f, ਅੱਖਰ **ret) {
        _cleanup_free_ ਅੱਖਰ *p = ਨਿਰਾ;
        ਅੱਖਰ *s;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(f);
        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        r = ਰਸਤਾ_simplify_alloc(f, &p);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਜੇ (ਖਾਲੀ_ਥਾਵਾਂ_or_ਜੜ੍ਹ(p))
                s = strdup("-");
        ਨਹੀਂਤਾਂ {
                ਜੇ (!ਰਸਤਾ_is_normalized(p))
                        ਵਾਪਸ -EINVAL;

                /* Truncate trailing slashes and skip leading slashes */
                delete_trailing_chars(p, "/");
                s = unit_name_escape(skip_leading_chars(p, "/"));
        }
        ਜੇ (!s)
                ਵਾਪਸ -ENOMEM;

        *ret = s;
        ਵਾਪਸ 0;
}

ਪੂਰਨ unit_name_ਰਸਤਾ_unescape(ਅਚਲ ਅੱਖਰ *f, ਅੱਖਰ **ret) {
        _cleanup_free_ ਅੱਖਰ *s = ਨਿਰਾ;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(f);

        ਜੇ (isempty(f))
                ਵਾਪਸ -EINVAL;

        ਜੇ (streq(f, "-")) {
                s = strdup("/");
                ਜੇ (!s)
                        ਵਾਪਸ -ENOMEM;
        } ਨਹੀਂਤਾਂ {
                _cleanup_free_ ਅੱਖਰ *w = ਨਿਰਾ;

                r = unit_name_unescape(f, &w);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;

                /* Don't accept trailing or leading slashes */
                ਜੇ (startswith(w, "/") || endswith(w, "/"))
                        ਵਾਪਸ -EINVAL;

                /* Prefix a slash again */
                s = strjoin("/", w);
                ਜੇ (!s)
                        ਵਾਪਸ -ENOMEM;

                ਜੇ (!ਰਸਤਾ_is_normalized(s))
                        ਵਾਪਸ -EINVAL;
        }

        ਜੇ (ret)
                *ret = TAKE_PTR(s);

        ਵਾਪਸ 0;
}

ਪੂਰਨ unit_name_replace_instance_ਭਰਿਆ(
                ਅਚਲ ਅੱਖਰ *original,
                ਅਚਲ ਅੱਖਰ *instance,
                bool accept_glob,
                ਅੱਖਰ **ret) {

        _cleanup_free_ ਅੱਖਰ *s = ਨਿਰਾ;
        ਅਚਲ ਅੱਖਰ *ਅਗੇਤਰ, *suffix;
        size_t pl;

        ਪ੍ਰਤਿਜ੍ਞਾ(original);
        ਪ੍ਰਤਿਜ੍ਞਾ(instance);
        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        ਜੇ (!unit_name_is_valid(original, UNIT_NAME_INSTANCE|UNIT_NAME_TEMPLATE))
                ਵਾਪਸ -EINVAL;
        ਜੇ (!unit_instance_is_valid(instance) && !(accept_glob && ਵਿੱਚ_charset(instance, VALID_CHARS_GLOB)))
                ਵਾਪਸ -EINVAL;

        ਅਗੇਤਰ = ASSERT_PTR(strchr(original, '@'));
        suffix = ASSERT_PTR(strrchr(original, '.'));
        ਪ੍ਰਤਿਜ੍ਞਾ(ਅਗੇਤਰ < suffix);

        pl = ਅਗੇਤਰ - original + 1; /* include '@' */

        s = new(ਅੱਖਰ, pl + ਲੜੀ_ਲੰਬਾਈ(instance) + ਲੜੀ_ਲੰਬਾਈ(suffix) + 1);
        ਜੇ (!s)
                ਵਾਪਸ -ENOMEM;

#ਜੇ HAS_FEATURE_ਸਮ੍ਰਿਤੀ_SANITIZER
        /* MSan doesn't like stpncpy... See also https://github.com/google/sanitizers/issues/926 */
        memzero(s, pl + ਲੜੀ_ਲੰਬਾਈ(instance) + ਲੜੀ_ਲੰਬਾਈ(suffix) + 1);
#ਜੇ_ਅੰਤ

        strcpy(stpcpy(stpncpy(s, original, pl), instance), suffix);

        /* Make sure the resulting name still is valid, i.e. didn't grow too large. Globs will be expanded
         * by clients when used, so the check is pointless. */
        ਜੇ (!accept_glob && !unit_name_is_valid(s, UNIT_NAME_INSTANCE))
                ਵਾਪਸ -EINVAL;

        *ret = TAKE_PTR(s);
        ਵਾਪਸ 0;
}

ਪੂਰਨ unit_name_template(ਅਚਲ ਅੱਖਰ *f, ਅੱਖਰ **ret) {
        ਅਚਲ ਅੱਖਰ *p, *e;
        ਅੱਖਰ *s;
        size_t a;

        ਪ੍ਰਤਿਜ੍ਞਾ(f);
        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        ਜੇ (!unit_name_is_valid(f, UNIT_NAME_INSTANCE|UNIT_NAME_TEMPLATE))
                ਵਾਪਸ -EINVAL;

        assert_se(p = strchr(f, '@'));
        assert_se(e = strrchr(f, '.'));

        a = p - f;

        s = new(ਅੱਖਰ, a + 1 + ਲੜੀ_ਲੰਬਾਈ(e) + 1);
        ਜੇ (!s)
                ਵਾਪਸ -ENOMEM;

        strcpy(mempcpy(s, f, a + 1), e);

        *ret = s;
        ਵਾਪਸ 0;
}

bool unit_name_is_hashed(ਅਚਲ ਅੱਖਰ *name) {
        ਅਚਲ ਅੱਖਰ *s;

        ਜੇ (!unit_name_is_valid(name, UNIT_NAME_PLAIN))
                ਵਾਪਸ false;

        assert_se(s = strrchr(name, '.'));

        ਜੇ (s - name < UNIT_NAME_HASH_ਲੰਮਾਈ_CHARS + 1)
                ਵਾਪਸ false;

        s -= UNIT_NAME_HASH_ਲੰਮਾਈ_CHARS;
        ਜੇ (s[-1] != '_')
                ਵਾਪਸ false;

        ਲਈ (size_t i = 0; i < UNIT_NAME_HASH_ਲੰਮਾਈ_CHARS; i++)
                ਜੇ (!strchr(LOWERCASE_HEXDIGITS, s[i]))
                        ਵਾਪਸ false;

        ਵਾਪਸ true;
}

ਪੂਰਨ unit_name_hash_long(ਅਚਲ ਅੱਖਰ *name, ਅੱਖਰ **ret) {
        _cleanup_free_ ਅੱਖਰ *n = ਨਿਰਾ, *hash = ਨਿਰਾ;
        ਅਚਲ ਅੱਖਰ *suffix;
        le64_t h;
        size_t len;

        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        ਜੇ (ਲੜੀ_ਲੰਬਾਈ(name) < UNIT_NAME_MAX)
                ਵਾਪਸ -EMSGSIZE;

        suffix = strrchr(name, '.');
        ਜੇ (!suffix)
                ਵਾਪਸ -EINVAL;

        ਜੇ (unit_type_from_string(suffix+1) < 0)
                ਵਾਪਸ -EINVAL;

        h = htole64(siphash24_string(name, LONG_UNIT_NAME_HASH_ਕੁੰਜੀ.ਬਾਈਟਾਂ));

        hash = hexmem(&h, ਆਕਾਰ(h));
        ਜੇ (!hash)
                ਵਾਪਸ -ENOMEM;

        assert_se(ਲੜੀ_ਲੰਬਾਈ(hash) == UNIT_NAME_HASH_ਲੰਮਾਈ_CHARS);

        len = UNIT_NAME_MAX - 1 - ਲੜੀ_ਲੰਬਾਈ(suffix+1) - UNIT_NAME_HASH_ਲੰਮਾਈ_CHARS - 2;
        ਪ੍ਰਤਿਜ੍ਞਾ(len > 0 && len < UNIT_NAME_MAX);

        n = strndup(name, len);
        ਜੇ (!n)
                ਵਾਪਸ -ENOMEM;

        ਜੇ (!strextend(&n, "_", hash, suffix))
                ਵਾਪਸ -ENOMEM;
        assert_se(unit_name_is_valid(n, UNIT_NAME_PLAIN));

        *ret = TAKE_PTR(n);

        ਵਾਪਸ 0;
}

ਪੂਰਨ unit_name_from_ਰਸਤਾ(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਅਚਲ ਅੱਖਰ *suffix, ਅੱਖਰ **ret) {
        _cleanup_free_ ਅੱਖਰ *p = ਨਿਰਾ, *s = ਨਿਰਾ;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(ਰਸਤਾ);
        ਪ੍ਰਤਿਜ੍ਞਾ(suffix);
        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        ਜੇ (!unit_suffix_is_valid(suffix))
                ਵਾਪਸ -EINVAL;

        r = unit_name_ਰਸਤਾ_escape(ਰਸਤਾ, &p);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        s = strjoin(p, suffix);
        ਜੇ (!s)
                ਵਾਪਸ -ENOMEM;

        ਜੇ (ਲੜੀ_ਲੰਬਾਈ(s) >= UNIT_NAME_MAX) {
                _cleanup_free_ ਅੱਖਰ *n = ਨਿਰਾ;

                log_debug("Unit name \"%s\" too long, falling back to hashed unit name.", s);

                r = unit_name_hash_long(s, &n);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;

                free_and_replace(s, n);
        }

        /* Refuse if this for some other reason didn't result in a valid name */
        ਜੇ (!unit_name_is_valid(s, UNIT_NAME_PLAIN))
                ਵਾਪਸ -EINVAL;

        *ret = TAKE_PTR(s);
        ਵਾਪਸ 0;
}

ਪੂਰਨ unit_name_from_ਰਸਤਾ_instance(ਅਚਲ ਅੱਖਰ *ਅਗੇਤਰ, ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਅਚਲ ਅੱਖਰ *suffix, ਅੱਖਰ **ret) {
        _cleanup_free_ ਅੱਖਰ *p = ਨਿਰਾ, *s = ਨਿਰਾ;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(ਅਗੇਤਰ);
        ਪ੍ਰਤਿਜ੍ਞਾ(ਰਸਤਾ);
        ਪ੍ਰਤਿਜ੍ਞਾ(suffix);
        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        ਜੇ (!unit_ਅਗੇਤਰ_is_valid(ਅਗੇਤਰ))
                ਵਾਪਸ -EINVAL;

        ਜੇ (!unit_suffix_is_valid(suffix))
                ਵਾਪਸ -EINVAL;

        r = unit_name_ਰਸਤਾ_escape(ਰਸਤਾ, &p);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        s = strjoin(ਅਗੇਤਰ, "@", p, suffix);
        ਜੇ (!s)
                ਵਾਪਸ -ENOMEM;

        ਜੇ (ਲੜੀ_ਲੰਬਾਈ(s) >= UNIT_NAME_MAX) /* Return a slightly more descriptive error for this specific condition */
                ਵਾਪਸ -ENAMETOOLONG;

        /* Refuse if this for some other reason didn't result in a valid name */
        ਜੇ (!unit_name_is_valid(s, UNIT_NAME_INSTANCE))
                ਵਾਪਸ -EINVAL;

        *ret = TAKE_PTR(s);
        ਵਾਪਸ 0;
}

ਪੂਰਨ unit_name_ਮੰਜ਼ਿਲ_ਰਸਤਾ(ਅਚਲ ਅੱਖਰ *name, ਅੱਖਰ **ret) {
        _cleanup_free_ ਅੱਖਰ *ਅਗੇਤਰ = ਨਿਰਾ;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(name);

        r = unit_name_ਮੰਜ਼ਿਲ_ਅਗੇਤਰ(name, &ਅਗੇਤਰ);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਜੇ (unit_name_is_hashed(name))
                ਵਾਪਸ -ENAMETOOLONG;

        ਵਾਪਸ unit_name_ਰਸਤਾ_unescape(ਅਗੇਤਰ, ret);
}

ਸਥਿਰ bool do_escape_mangle(ਅਚਲ ਅੱਖਰ *f, bool allow_globs, ਅੱਖਰ *t) {
        ਅਚਲ ਅੱਖਰ *valid_chars;
        bool mangled = false;

        ਪ੍ਰਤਿਜ੍ਞਾ(f);
        ਪ੍ਰਤਿਜ੍ਞਾ(t);

        /* We'll only escape the obvious characters here, to play safe.
         *
         * Returns true if any characters were mangled, false otherwise.
         */

        valid_chars = allow_globs ? VALID_CHARS_GLOB : VALID_CHARS_WITH_AT;

        ਲਈ (; *f; f++)
                ਜੇ (*f == '/') {
                        *(t++) = '-';
                        mangled = true;
                } ਨਹੀਂਤਾਂ ਜੇ (!strchr(valid_chars, *f)) {
                        t = do_escape_char(*f, t);
                        mangled = true;
                } ਨਹੀਂਤਾਂ
                        *(t++) = *f;
        *t = 0;

        ਵਾਪਸ mangled;
}

/**
 *  Convert a string to a unit name. /dev/blah is converted to dev-blah.device,
 *  /blah/blah is converted to blah-blah.mount, anything else is left alone,
 *  except that @suffix is appended if a valid unit suffix is not present.
 *
 *  If @allow_globs, globs characters are preserved. Otherwise, they are escaped.
 */
ਪੂਰਨ unit_name_mangle_with_suffix(
                ਅਚਲ ਅੱਖਰ *name,
                ਅਚਲ ਅੱਖਰ *operation,
                UnitNameMangle flags,
                ਅਚਲ ਅੱਖਰ *suffix,
                ਅੱਖਰ **ret) {

        _cleanup_free_ ਅੱਖਰ *s = ਨਿਰਾ;
        bool mangled, suggest_escape = true, warn = FLAGS_ਸੈੱਟ(flags, UNIT_NAME_MANGLE_WARN);
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(name);
        ਪ੍ਰਤਿਜ੍ਞਾ(suffix);
        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        ਜੇ (isempty(name)) /* We cannot mangle empty unit names to become valid, sorry. */
                ਵਾਪਸ -EINVAL;

        ਜੇ (!unit_suffix_is_valid(suffix))
                ਵਾਪਸ -EINVAL;

        /* Already a fully valid unit name? If so, no mangling is necessary... */
        ਜੇ (unit_name_is_valid(name, UNIT_NAME_ANY)) {
                ਜੇ (FLAGS_ਸੈੱਟ(flags, UNIT_NAME_MANGLE_STRICT) && !endswith(name, suffix)) {
                        ਅਚਲ ਅੱਖਰ *e = ASSERT_PTR(strrchr(name, '.'));

                        ਵਾਪਸ log_ਭਰਿਆ_errno(warn ? LOG_NOTICE : LOG_DEBUG,
                                              SYNTHETIC_ERRNO(EINVAL),
                                              "Unit name \"%s\" has unit type \"%s\", but \"%s\" is expected%s%s.",
                                              name, e + 1, suffix + 1,
                                              operation ? " " : "", strempty(operation));
                }

                ਜਾਓ good;
        }

        /* Already a fully valid globbing expression? If so, no mangling is necessary either... */
        ਜੇ (string_is_glob(name) && ਵਿੱਚ_charset(name, VALID_CHARS_GLOB)) {
                ਜੇ (FLAGS_ਸੈੱਟ(flags, UNIT_NAME_MANGLE_GLOB))
                        ਜਾਓ good;
                log_ਭਰਿਆ(warn ? LOG_NOTICE : LOG_DEBUG,
                         "Glob pattern passed%s%s, but globs are not supported for this.",
                         operation ? " " : "", strempty(operation));
                suggest_escape = false;
        }

        ਜੇ (ਰਸਤਾ_is_absolute(name)) {
                _cleanup_free_ ਅੱਖਰ *n = ਨਿਰਾ, *u = ਨਿਰਾ;

                r = ਰਸਤਾ_simplify_alloc(name, &n);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;

                ਜੇ (is_device_ਰਸਤਾ(n)) {
                        r = unit_name_from_ਰਸਤਾ(n, ".device", &u);
                        ਜੇ (r >= 0) {
                                ਜੇ (FLAGS_ਸੈੱਟ(flags, UNIT_NAME_MANGLE_STRICT) && !streq(suffix, ".device"))
                                        ਵਾਪਸ log_ਭਰਿਆ_errno(warn ? LOG_NOTICE : LOG_DEBUG,
                                                              SYNTHETIC_ERRNO(EINVAL),
                                                              "Path \"%s\" resolves to unit type \"device\", but \"%s\" is expected%s%s.",
                                                              name, suffix + 1,
                                                              operation ? " " : "", strempty(operation));

                                *ret = TAKE_PTR(u);
                                ਵਾਪਸ 1;
                        }
                        ਜੇ (r != -EINVAL)
                                ਵਾਪਸ r;
                }

                r = unit_name_from_ਰਸਤਾ(n, ".mount", &u);
                ਜੇ (r >= 0) {
                        ਜੇ (FLAGS_ਸੈੱਟ(flags, UNIT_NAME_MANGLE_STRICT) && !streq(suffix, ".mount"))
                                ਵਾਪਸ log_ਭਰਿਆ_errno(warn ? LOG_NOTICE : LOG_DEBUG,
                                                      SYNTHETIC_ERRNO(EINVAL),
                                                      "Path \"%s\" resolves to unit type \"mount\", but \"%s\" is expected%s%s.",
                                                      name, suffix + 1,
                                                      operation ? " " : "", strempty(operation));

                        *ret = TAKE_PTR(u);
                        ਵਾਪਸ 1;
                }
                ਜੇ (r != -EINVAL)
                        ਵਾਪਸ r;
        }

        s = new(ਅੱਖਰ, ਲੜੀ_ਲੰਬਾਈ(name) * 4 + ਲੜੀ_ਲੰਬਾਈ(suffix) + 1);
        ਜੇ (!s)
                ਵਾਪਸ -ENOMEM;

        mangled = do_escape_mangle(name, FLAGS_ਸੈੱਟ(flags, UNIT_NAME_MANGLE_GLOB), s);
        ਜੇ (mangled)
                log_ਭਰਿਆ(warn ? LOG_NOTICE : LOG_DEBUG,
                         "Invalid unit name \"%s\" escaped as \"%s\"%s.",
                         name, s,
                         suggest_escape ? " (maybe you should use systemd-escape?)" : "");

        /* Append a suffix if it doesn't have any, but only if this is not a glob, so that we can allow
         * "foo.*" as a valid glob. */
        ਜੇ ((!FLAGS_ਸੈੱਟ(flags, UNIT_NAME_MANGLE_GLOB) || !string_is_glob(s)) && unit_name_ਮੰਜ਼ਿਲ_type(s) < 0)
                strcat(s, suffix);

        /* Make sure mangling didn't grow this too large (but don't do this check if globbing is allowed,
         * since globs generally do not qualify as valid unit names) */
        ਜੇ (!FLAGS_ਸੈੱਟ(flags, UNIT_NAME_MANGLE_GLOB) && !unit_name_is_valid(s, UNIT_NAME_ANY))
                ਵਾਪਸ -EINVAL;

        *ret = TAKE_PTR(s);
        ਵਾਪਸ 1;

good:
        ਵਾਪਸ strdup_ਮੰਜ਼ਿਲ(ret, name);
}

ਪੂਰਨ ਟੁਕੜਾ_build_ਮਾਪਾ_ਟੁਕੜਾ(ਅਚਲ ਅੱਖਰ *ਟੁਕੜਾ, ਅੱਖਰ **ret) {
        ਪ੍ਰਤਿਜ੍ਞਾ(ਟੁਕੜਾ);
        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        ਜੇ (!ਟੁਕੜਾ_name_is_valid(ਟੁਕੜਾ))
                ਵਾਪਸ -EINVAL;

        ਜੇ (streq(ਟੁਕੜਾ, SPECIAL_ਜੜ੍ਹ_ਟੁਕੜਾ)) {
                *ret = ਨਿਰਾ;
                ਵਾਪਸ 0;
        }

        _cleanup_free_ ਅੱਖਰ *s = strdup(ਟੁਕੜਾ);
        ਜੇ (!s)
                ਵਾਪਸ -ENOMEM;

        ਅੱਖਰ *dash = strrchr(s, '-');
        ਜੇ (!dash)
                ਵਾਪਸ strdup_ਮੰਜ਼ਿਲ_ਭਰਿਆ(ret, SPECIAL_ਜੜ੍ਹ_ਟੁਕੜਾ);

        /* We know that s ended with .slice before truncation, so we have enough space. */
        strcpy(dash, ".slice");

        *ret = TAKE_PTR(s);
        ਵਾਪਸ 1;
}

ਪੂਰਨ ਟੁਕੜਾ_build_subslice(ਅਚਲ ਅੱਖਰ *ਟੁਕੜਾ, ਅਚਲ ਅੱਖਰ *name, ਅੱਖਰ **ret) {
        ਅੱਖਰ *subslice;

        ਪ੍ਰਤਿਜ੍ਞਾ(ਟੁਕੜਾ);
        ਪ੍ਰਤਿਜ੍ਞਾ(name);
        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        ਜੇ (!ਟੁਕੜਾ_name_is_valid(ਟੁਕੜਾ))
                ਵਾਪਸ -EINVAL;

        ਜੇ (!unit_ਅਗੇਤਰ_is_valid(name))
                ਵਾਪਸ -EINVAL;

        ਜੇ (streq(ਟੁਕੜਾ, SPECIAL_ਜੜ੍ਹ_ਟੁਕੜਾ))
                subslice = strjoin(name, ".slice");
        ਨਹੀਂਤਾਂ {
                ਅਚਲ ਅੱਖਰ *e;

                assert_se(e = endswith(ਟੁਕੜਾ, ".slice"));

                subslice = new(ਅੱਖਰ, (e - ਟੁਕੜਾ) + 1 + ਲੜੀ_ਲੰਬਾਈ(name) + 6 + 1);
                ਜੇ (!subslice)
                        ਵਾਪਸ -ENOMEM;

                stpcpy(stpcpy(stpcpy(mempcpy(subslice, ਟੁਕੜਾ, e - ਟੁਕੜਾ), "-"), name), ".slice");
        }

        *ret = subslice;
        ਵਾਪਸ 0;
}

bool ਟੁਕੜਾ_name_is_valid(ਅਚਲ ਅੱਖਰ *name) {
        ਅਚਲ ਅੱਖਰ *p, *e;
        bool dash = false;

        ਜੇ (!unit_name_is_valid(name, UNIT_NAME_PLAIN))
                ਵਾਪਸ false;

        ਜੇ (streq(name, SPECIAL_ਜੜ੍ਹ_ਟੁਕੜਾ))
                ਵਾਪਸ true;

        e = endswith(name, ".slice");
        ਜੇ (!e)
                ਵਾਪਸ false;

        ਲਈ (p = name; p < e; p++) {

                ਜੇ (*p == '-') {

                        /* Don't allow initial dash */
                        ਜੇ (p == name)
                                ਵਾਪਸ false;

                        /* Don't allow multiple dashes */
                        ਜੇ (dash)
                                ਵਾਪਸ false;

                        dash = true;
                } ਨਹੀਂਤਾਂ
                        dash = false;
        }

        /* Don't allow trailing hash */
        ਜੇ (dash)
                ਵਾਪਸ false;

        ਵਾਪਸ true;
}

bool unit_name_ਅਗੇਤਰ_equal(ਅਚਲ ਅੱਖਰ *a, ਅਚਲ ਅੱਖਰ *b) {
        ਅਚਲ ਅੱਖਰ *p, *q;

        ਪ੍ਰਤਿਜ੍ਞਾ(a);
        ਪ੍ਰਤਿਜ੍ਞਾ(b);

        ਜੇ (!unit_name_is_valid(a, UNIT_NAME_ANY) || !unit_name_is_valid(b, UNIT_NAME_ANY))
                ਵਾਪਸ false;

        p = strchr(a, '@');
        ਜੇ (!p)
                p = strrchr(a, '.');

        q = strchr(b, '@');
        ਜੇ (!q)
                q = strrchr(b, '.');

        ਪ੍ਰਤਿਜ੍ਞਾ(p);
        ਪ੍ਰਤਿਜ੍ਞਾ(q);

        ਵਾਪਸ memcmp_nn(a, p - a, b, q - b) == 0;
}
