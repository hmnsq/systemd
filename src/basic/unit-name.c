/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ "sd-id128.h"

#ಅನ್ತರ್ಭಾವ "alloc-util.h"
#ಅನ್ತರ್ಭಾವ "glob-util.h"
#ಅನ್ತರ್ಭಾವ "hexdecoct.h"
#ಅನ್ತರ್ಭಾವ "log.h"
#ಅನ್ತರ್ಭಾವ "memory-util.h"
#ಅನ್ತರ್ಭಾವ "path-util.h"
#ಅನ್ತರ್ಭಾವ "sparse-endian.h"
#ಅನ್ತರ್ಭಾವ "special.h"
#ಅನ್ತರ್ಭಾವ "siphash24.h"
#ಅನ್ತರ್ಭಾವ "string-util.h"
#ಅನ್ತರ್ಭಾವ "unit-def.h"
#ಅನ್ತರ್ಭಾವ "unit-name.h"

/* Characters valid in a unit name. */
#ವ್ಯಾಖ್ಯಾನಿಸು VALID_CHARS                             \
        ALPHANUMERICAL                          \
        ":-_.\\"

/* The same, but also permits the single @ character that may appear */
#ವ್ಯಾಖ್ಯಾನಿಸು VALID_CHARS_WITH_AT                     \
        "@"                                     \
        VALID_CHARS

/* All chars valid in a unit name glob */
#ವ್ಯಾಖ್ಯಾನಿಸು VALID_CHARS_GLOB                        \
        VALID_CHARS_WITH_AT                     \
        "[]!-*?"

#ವ್ಯಾಖ್ಯಾನಿಸು LONG_UNIT_NAME_HASH_ಕೀಲಿ SD_ID128_MAKE(ec,f2,37,fb,58,32,4a,32,84,9f,06,9b,0d,21,eb,9a)
#ವ್ಯಾಖ್ಯಾನಿಸು UNIT_NAME_HASH_ಉದ್ದಳತೆ_CHARS 16

bool unit_name_is_valid(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *n, UnitNameFlags flags) {
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *e, *i, *at;

        ಪ್ರತಿಜ್ಞಾ((flags & ~(UNIT_NAME_PLAIN|UNIT_NAME_INSTANCE|UNIT_NAME_TEMPLATE)) == 0);

        ಆದರೆ (_unlikely_(flags == 0))
                ಹಿಂತಿರುಗಿ false;

        ಆದರೆ (isempty(n))
                ಹಿಂತಿರುಗಿ false;

        ಆದರೆ (ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(n) >= UNIT_NAME_MAX)
                ಹಿಂತಿರುಗಿ false;

        e = strrchr(n, '.');
        ಆದರೆ (!e || e == n)
                ಹಿಂತಿರುಗಿ false;

        ಆದರೆ (unit_type_from_string(e + 1) < 0)
                ಹಿಂತಿರುಗಿ false;

        ಗಾಗಿ (i = n, at = ಖಾಲಿ; i < e; i++) {

                ಆದರೆ (*i == '@' && !at)
                        at = i;

                ಆದರೆ (!strchr(VALID_CHARS_WITH_AT, *i))
                        ಹಿಂತಿರುಗಿ false;
        }

        ಆದರೆ (at == n)
                ಹಿಂತಿರುಗಿ false;

        ಆದರೆ (flags & UNIT_NAME_PLAIN)
                ಆದರೆ (!at)
                        ಹಿಂತಿರುಗಿ true;

        ಆದರೆ (flags & UNIT_NAME_INSTANCE)
                ಆದರೆ (at && e > at + 1)
                        ಹಿಂತಿರುಗಿ true;

        ಆದರೆ (flags & UNIT_NAME_TEMPLATE)
                ಆದರೆ (at && e == at + 1)
                        ಹಿಂತಿರುಗಿ true;

        ಹಿಂತಿರುಗಿ false;
}

bool unit_ಪೂರ್ವಪ್ರತ್ಯಯ_is_valid(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p) {

        /* We don't allow additional @ in the prefix string */

        ಆದರೆ (isempty(p))
                ಹಿಂತಿರುಗಿ false;

        ಹಿಂತಿರುಗಿ ಒಳ_charset(p, VALID_CHARS);
}

bool unit_instance_is_valid(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *i) {

        /* The max length depends on the length of the string, so we
         * don't really check this here. */

        ಆದರೆ (isempty(i))
                ಹಿಂತಿರುಗಿ false;

        /* We allow additional @ in the instance string, we do not
         * allow them in the prefix! */

        ಹಿಂತಿರುಗಿ ಒಳ_charset(i, "@" VALID_CHARS);
}

bool unit_suffix_is_valid(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s) {
        ಆದರೆ (isempty(s))
                ಹಿಂತಿರುಗಿ false;

        ಆದರೆ (s[0] != '.')
                ಹಿಂತಿರುಗಿ false;

        ಆದರೆ (unit_type_from_string(s + 1) < 0)
                ಹಿಂತಿರುಗಿ false;

        ಹಿಂತಿರುಗಿ true;
}

ಪೂರ್ಣಾಂಕ unit_name_ಗಮ್ಯ_ಪೂರ್ವಪ್ರತ್ಯಯ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *n, ಅಕ್ಷರ **ret) {
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p;
        ಅಕ್ಷರ *s;

        ಪ್ರತಿಜ್ಞಾ(n);
        ಪ್ರತಿಜ್ಞಾ(ret);

        ಆದರೆ (!unit_name_is_valid(n, UNIT_NAME_ANY))
                ಹಿಂತಿರುಗಿ -EINVAL;

        p = strchr(n, '@');
        ಆದರೆ (!p)
                p = strrchr(n, '.');

        assert_se(p);

        s = strndup(n, p - n);
        ಆದರೆ (!s)
                ಹಿಂತಿರುಗಿ -ENOMEM;

        *ret = s;
        ಹಿಂತಿರುಗಿ 0;
}

UnitNameFlags unit_name_ಗಮ್ಯ_instance(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *n, ಅಕ್ಷರ **ret) {
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p, *d;

        ಪ್ರತಿಜ್ಞಾ(n);

        ಆದರೆ (!unit_name_is_valid(n, UNIT_NAME_ANY))
                ಹಿಂತಿರುಗಿ -EINVAL;

        /* Everything past the first @ and before the last . is the instance */
        p = strchr(n, '@');
        ಆದರೆ (!p) {
                ಆದರೆ (ret)
                        *ret = ಖಾಲಿ;
                ಹಿಂತಿರುಗಿ UNIT_NAME_PLAIN;
        }

        p++;

        d = strrchr(p, '.');
        ಆದರೆ (!d)
                ಹಿಂತಿರುಗಿ -EINVAL;

        ಆದರೆ (ret) {
                ಅಕ್ಷರ *i = strndup(p, d-p);
                ಆದರೆ (!i)
                        ಹಿಂತಿರುಗಿ -ENOMEM;

                *ret = i;
        }
        ಹಿಂತಿರುಗಿ d > p ? UNIT_NAME_INSTANCE : UNIT_NAME_TEMPLATE;
}

ಪೂರ್ಣಾಂಕ unit_name_ಗಮ್ಯ_ಪೂರ್ವಪ್ರತ್ಯಯ_and_instance(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *n, ಅಕ್ಷರ **ret) {
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *d;
        ಅಕ್ಷರ *s;

        ಪ್ರತಿಜ್ಞಾ(n);
        ಪ್ರತಿಜ್ಞಾ(ret);

        ಆದರೆ (!unit_name_is_valid(n, UNIT_NAME_ANY))
                ಹಿಂತಿರುಗಿ -EINVAL;

        d = strrchr(n, '.');
        ಆದರೆ (!d)
                ಹಿಂತಿರುಗಿ -EINVAL;

        s = strndup(n, d - n);
        ಆದರೆ (!s)
                ಹಿಂತಿರುಗಿ -ENOMEM;

        *ret = s;
        ಹಿಂತಿರುಗಿ 0;
}

UnitType unit_name_ಗಮ್ಯ_type(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *n) {
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *e;

        ಪ್ರತಿಜ್ಞಾ(n);

        ಆದರೆ (!unit_name_is_valid(n, UNIT_NAME_ANY))
                ಹಿಂತಿರುಗಿ _UNIT_TYPE_INVALID;

        assert_se(e = strrchr(n, '.'));

        ಹಿಂತಿರುಗಿ unit_type_from_string(e + 1);
}

ಪೂರ್ಣಾಂಕ unit_name_change_suffix(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *n, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *suffix, ಅಕ್ಷರ **ret) {
        _cleanup_free_ ಅಕ್ಷರ *s = ಖಾಲಿ;
        size_t a, b;
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *e;

        ಪ್ರತಿಜ್ಞಾ(n);
        ಪ್ರತಿಜ್ಞಾ(suffix);
        ಪ್ರತಿಜ್ಞಾ(ret);

        ಆದರೆ (!unit_name_is_valid(n, UNIT_NAME_ANY))
                ಹಿಂತಿರುಗಿ -EINVAL;

        ಆದರೆ (!unit_suffix_is_valid(suffix))
                ಹಿಂತಿರುಗಿ -EINVAL;

        assert_se(e = strrchr(n, '.'));

        a = e - n;
        b = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(suffix);

        s = new(ಅಕ್ಷರ, a + b + 1);
        ಆದರೆ (!s)
                ಹಿಂತಿರುಗಿ -ENOMEM;

        strcpy(mempcpy(s, n, a), suffix);

        /* Make sure the name is still valid (i.e. didn't grow too large due to longer suffix) */
        ಆದರೆ (!unit_name_is_valid(s, UNIT_NAME_ANY))
                ಹಿಂತಿರುಗಿ -EINVAL;

        *ret = TAKE_PTR(s);
        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ unit_name_build(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಪೂರ್ವಪ್ರತ್ಯಯ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *instance, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *suffix, ಅಕ್ಷರ **ret) {
        UnitType type;

        ಪ್ರತಿಜ್ಞಾ(ಪೂರ್ವಪ್ರತ್ಯಯ);
        ಪ್ರತಿಜ್ಞಾ(suffix);
        ಪ್ರತಿಜ್ಞಾ(ret);

        ಆದರೆ (suffix[0] != '.')
                ಹಿಂತಿರುಗಿ -EINVAL;

        type = unit_type_from_string(suffix + 1);
        ಆದರೆ (type < 0)
                ಹಿಂತಿರುಗಿ type;

        ಹಿಂತಿರುಗಿ unit_name_build_from_type(ಪೂರ್ವಪ್ರತ್ಯಯ, instance, type, ret);
}

ಪೂರ್ಣಾಂಕ unit_name_build_from_type(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಪೂರ್ವಪ್ರತ್ಯಯ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *instance, UnitType type, ಅಕ್ಷರ **ret) {
        _cleanup_free_ ಅಕ್ಷರ *s = ಖಾಲಿ;
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ut;

        ಪ್ರತಿಜ್ಞಾ(ಪೂರ್ವಪ್ರತ್ಯಯ);
        ಪ್ರತಿಜ್ಞಾ(type >= 0);
        ಪ್ರತಿಜ್ಞಾ(type < _UNIT_TYPE_MAX);
        ಪ್ರತಿಜ್ಞಾ(ret);

        ಆದರೆ (!unit_ಪೂರ್ವಪ್ರತ್ಯಯ_is_valid(ಪೂರ್ವಪ್ರತ್ಯಯ))
                ಹಿಂತಿರುಗಿ -EINVAL;

        ut = unit_type_ಗಮ್ಯ_string(type);

        ಆದರೆ (instance) {
                ಆದರೆ (!unit_instance_is_valid(instance))
                        ಹಿಂತಿರುಗಿ -EINVAL;

                s = strjoin(ಪೂರ್ವಪ್ರತ್ಯಯ, "@", instance, ".", ut);
        } ಇಲ್ಲದಿದ್ದರೆ
                s = strjoin(ಪೂರ್ವಪ್ರತ್ಯಯ, ".", ut);
        ಆದರೆ (!s)
                ಹಿಂತಿರುಗಿ -ENOMEM;

        /* Verify that this didn't grow too large (or otherwise is invalid) */
        ಆದರೆ (!unit_name_is_valid(s, instance ? UNIT_NAME_INSTANCE : UNIT_NAME_PLAIN))
                ಹಿಂತಿರುಗಿ -EINVAL;

        *ret = TAKE_PTR(s);
        ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಅಕ್ಷರ *do_escape_char(ಅಕ್ಷರ c, ಅಕ್ಷರ *t) {
        ಪ್ರತಿಜ್ಞಾ(t);

        *(t++) = '\\';
        *(t++) = 'x';
        *(t++) = hexchar(c >> 4);
        *(t++) = hexchar(c);

        ಹಿಂತಿರುಗಿ t;
}

ಸ್ಥಿರ ಅಕ್ಷರ *do_escape(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *f, ಅಕ್ಷರ *t) {
        ಪ್ರತಿಜ್ಞಾ(f);
        ಪ್ರತಿಜ್ಞಾ(t);

        /* do not create units with a leading '.', like for "/.dotdir" mount points */
        ಆದರೆ (*f == '.') {
                t = do_escape_char(*f, t);
                f++;
        }

        ಗಾಗಿ (; *f; f++) {
                ಆದರೆ (*f == '/')
                        *(t++) = '-';
                ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಒಳ_ಗಣ(*f, '-', '\\') || !strchr(VALID_CHARS, *f))
                        t = do_escape_char(*f, t);
                ಇಲ್ಲದಿದ್ದರೆ
                        *(t++) = *f;
        }

        ಹಿಂತಿರುಗಿ t;
}

ಅಕ್ಷರ* unit_name_escape(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *f) {
        ಅಕ್ಷರ *r, *t;

        ಪ್ರತಿಜ್ಞಾ(f);

        r = new(ಅಕ್ಷರ, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(f)*4+1);
        ಆದರೆ (!r)
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        t = do_escape(f, r);
        *t = 0;

        ಹಿಂತಿರುಗಿ r;
}

ಪೂರ್ಣಾಂಕ unit_name_unescape(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *f, ಅಕ್ಷರ **ret) {
        _cleanup_free_ ಅಕ್ಷರ *r = ಖಾಲಿ;
        ಅಕ್ಷರ *t;

        ಪ್ರತಿಜ್ಞಾ(f);
        ಪ್ರತಿಜ್ಞಾ(ret);

        r = strdup(f);
        ಆದರೆ (!r)
                ಹಿಂತಿರುಗಿ -ENOMEM;

        ಗಾಗಿ (t = r; *f; f++) {
                ಆದರೆ (*f == '-')
                        *(t++) = '/';
                ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*f == '\\') {
                        ಪೂರ್ಣಾಂಕ a, b;

                        ಆದರೆ (f[1] != 'x')
                                ಹಿಂತಿರುಗಿ -EINVAL;

                        a = unhexchar(f[2]);
                        ಆದರೆ (a < 0)
                                ಹಿಂತಿರುಗಿ -EINVAL;

                        b = unhexchar(f[3]);
                        ಆದರೆ (b < 0)
                                ಹಿಂತಿರುಗಿ -EINVAL;

                        *(t++) = (ಅಕ್ಷರ) (((uint8_t) a << 4U) | (uint8_t) b);
                        f += 3;
                } ಇಲ್ಲದಿದ್ದರೆ
                        *(t++) = *f;
        }

        *t = 0;

        *ret = TAKE_PTR(r);

        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ unit_name_ಮಾರ್ಗ_escape(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *f, ಅಕ್ಷರ **ret) {
        _cleanup_free_ ಅಕ್ಷರ *p = ಖಾಲಿ;
        ಅಕ್ಷರ *s;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(f);
        ಪ್ರತಿಜ್ಞಾ(ret);

        r = ಮಾರ್ಗ_simplify_alloc(f, &p);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಆದರೆ (ಖಾಲಿಗಳು_or_ಬೇರು(p))
                s = strdup("-");
        ಇಲ್ಲದಿದ್ದರೆ {
                ಆದರೆ (!ಮಾರ್ಗ_is_normalized(p))
                        ಹಿಂತಿರುಗಿ -EINVAL;

                /* Truncate trailing slashes and skip leading slashes */
                delete_trailing_chars(p, "/");
                s = unit_name_escape(skip_leading_chars(p, "/"));
        }
        ಆದರೆ (!s)
                ಹಿಂತಿರುಗಿ -ENOMEM;

        *ret = s;
        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ unit_name_ಮಾರ್ಗ_unescape(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *f, ಅಕ್ಷರ **ret) {
        _cleanup_free_ ಅಕ್ಷರ *s = ಖಾಲಿ;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(f);

        ಆದರೆ (isempty(f))
                ಹಿಂತಿರುಗಿ -EINVAL;

        ಆದರೆ (streq(f, "-")) {
                s = strdup("/");
                ಆದರೆ (!s)
                        ಹಿಂತಿರುಗಿ -ENOMEM;
        } ಇಲ್ಲದಿದ್ದರೆ {
                _cleanup_free_ ಅಕ್ಷರ *w = ಖಾಲಿ;

                r = unit_name_unescape(f, &w);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;

                /* Don't accept trailing or leading slashes */
                ಆದರೆ (startswith(w, "/") || endswith(w, "/"))
                        ಹಿಂತಿರುಗಿ -EINVAL;

                /* Prefix a slash again */
                s = strjoin("/", w);
                ಆದರೆ (!s)
                        ಹಿಂತಿರುಗಿ -ENOMEM;

                ಆದರೆ (!ಮಾರ್ಗ_is_normalized(s))
                        ಹಿಂತಿರುಗಿ -EINVAL;
        }

        ಆದರೆ (ret)
                *ret = TAKE_PTR(s);

        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ unit_name_replace_instance_ತುಂಬಿದ(
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *original,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *instance,
                bool accept_glob,
                ಅಕ್ಷರ **ret) {

        _cleanup_free_ ಅಕ್ಷರ *s = ಖಾಲಿ;
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಪೂರ್ವಪ್ರತ್ಯಯ, *suffix;
        size_t pl;

        ಪ್ರತಿಜ್ಞಾ(original);
        ಪ್ರತಿಜ್ಞಾ(instance);
        ಪ್ರತಿಜ್ಞಾ(ret);

        ಆದರೆ (!unit_name_is_valid(original, UNIT_NAME_INSTANCE|UNIT_NAME_TEMPLATE))
                ಹಿಂತಿರುಗಿ -EINVAL;
        ಆದರೆ (!unit_instance_is_valid(instance) && !(accept_glob && ಒಳ_charset(instance, VALID_CHARS_GLOB)))
                ಹಿಂತಿರುಗಿ -EINVAL;

        ಪೂರ್ವಪ್ರತ್ಯಯ = ASSERT_PTR(strchr(original, '@'));
        suffix = ASSERT_PTR(strrchr(original, '.'));
        ಪ್ರತಿಜ್ಞಾ(ಪೂರ್ವಪ್ರತ್ಯಯ < suffix);

        pl = ಪೂರ್ವಪ್ರತ್ಯಯ - original + 1; /* include '@' */

        s = new(ಅಕ್ಷರ, pl + ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(instance) + ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(suffix) + 1);
        ಆದರೆ (!s)
                ಹಿಂತಿರುಗಿ -ENOMEM;

#ಆದರೆ HAS_FEATURE_ಸ್ಮರಣೆ_SANITIZER
        /* MSan doesn't like stpncpy... See also https://github.com/google/sanitizers/issues/926 */
        memzero(s, pl + ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(instance) + ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(suffix) + 1);
#ಆದರೆ_ಅಂತ್ಯ

        strcpy(stpcpy(stpncpy(s, original, pl), instance), suffix);

        /* Make sure the resulting name still is valid, i.e. didn't grow too large. Globs will be expanded
         * by clients when used, so the check is pointless. */
        ಆದರೆ (!accept_glob && !unit_name_is_valid(s, UNIT_NAME_INSTANCE))
                ಹಿಂತಿರುಗಿ -EINVAL;

        *ret = TAKE_PTR(s);
        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ unit_name_template(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *f, ಅಕ್ಷರ **ret) {
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p, *e;
        ಅಕ್ಷರ *s;
        size_t a;

        ಪ್ರತಿಜ್ಞಾ(f);
        ಪ್ರತಿಜ್ಞಾ(ret);

        ಆದರೆ (!unit_name_is_valid(f, UNIT_NAME_INSTANCE|UNIT_NAME_TEMPLATE))
                ಹಿಂತಿರುಗಿ -EINVAL;

        assert_se(p = strchr(f, '@'));
        assert_se(e = strrchr(f, '.'));

        a = p - f;

        s = new(ಅಕ್ಷರ, a + 1 + ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(e) + 1);
        ಆದರೆ (!s)
                ಹಿಂತಿರುಗಿ -ENOMEM;

        strcpy(mempcpy(s, f, a + 1), e);

        *ret = s;
        ಹಿಂತಿರುಗಿ 0;
}

bool unit_name_is_hashed(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name) {
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s;

        ಆದರೆ (!unit_name_is_valid(name, UNIT_NAME_PLAIN))
                ಹಿಂತಿರುಗಿ false;

        assert_se(s = strrchr(name, '.'));

        ಆದರೆ (s - name < UNIT_NAME_HASH_ಉದ್ದಳತೆ_CHARS + 1)
                ಹಿಂತಿರುಗಿ false;

        s -= UNIT_NAME_HASH_ಉದ್ದಳತೆ_CHARS;
        ಆದರೆ (s[-1] != '_')
                ಹಿಂತಿರುಗಿ false;

        ಗಾಗಿ (size_t i = 0; i < UNIT_NAME_HASH_ಉದ್ದಳತೆ_CHARS; i++)
                ಆದರೆ (!strchr(LOWERCASE_HEXDIGITS, s[i]))
                        ಹಿಂತಿರುಗಿ false;

        ಹಿಂತಿರುಗಿ true;
}

ಪೂರ್ಣಾಂಕ unit_name_hash_long(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name, ಅಕ್ಷರ **ret) {
        _cleanup_free_ ಅಕ್ಷರ *n = ಖಾಲಿ, *hash = ಖಾಲಿ;
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *suffix;
        le64_t h;
        size_t len;

        ಪ್ರತಿಜ್ಞಾ(ret);

        ಆದರೆ (ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(name) < UNIT_NAME_MAX)
                ಹಿಂತಿರುಗಿ -EMSGSIZE;

        suffix = strrchr(name, '.');
        ಆದರೆ (!suffix)
                ಹಿಂತಿರುಗಿ -EINVAL;

        ಆದರೆ (unit_type_from_string(suffix+1) < 0)
                ಹಿಂತಿರುಗಿ -EINVAL;

        h = htole64(siphash24_string(name, LONG_UNIT_NAME_HASH_ಕೀಲಿ.ಬೈಟುಗಳು));

        hash = hexmem(&h, ಗಾತ್ರ(h));
        ಆದರೆ (!hash)
                ಹಿಂತಿರುಗಿ -ENOMEM;

        assert_se(ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(hash) == UNIT_NAME_HASH_ಉದ್ದಳತೆ_CHARS);

        len = UNIT_NAME_MAX - 1 - ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(suffix+1) - UNIT_NAME_HASH_ಉದ್ದಳತೆ_CHARS - 2;
        ಪ್ರತಿಜ್ಞಾ(len > 0 && len < UNIT_NAME_MAX);

        n = strndup(name, len);
        ಆದರೆ (!n)
                ಹಿಂತಿರುಗಿ -ENOMEM;

        ಆದರೆ (!strextend(&n, "_", hash, suffix))
                ಹಿಂತಿರುಗಿ -ENOMEM;
        assert_se(unit_name_is_valid(n, UNIT_NAME_PLAIN));

        *ret = TAKE_PTR(n);

        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ unit_name_from_ಮಾರ್ಗ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *suffix, ಅಕ್ಷರ **ret) {
        _cleanup_free_ ಅಕ್ಷರ *p = ಖಾಲಿ, *s = ಖಾಲಿ;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(ಮಾರ್ಗ);
        ಪ್ರತಿಜ್ಞಾ(suffix);
        ಪ್ರತಿಜ್ಞಾ(ret);

        ಆದರೆ (!unit_suffix_is_valid(suffix))
                ಹಿಂತಿರುಗಿ -EINVAL;

        r = unit_name_ಮಾರ್ಗ_escape(ಮಾರ್ಗ, &p);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        s = strjoin(p, suffix);
        ಆದರೆ (!s)
                ಹಿಂತಿರುಗಿ -ENOMEM;

        ಆದರೆ (ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(s) >= UNIT_NAME_MAX) {
                _cleanup_free_ ಅಕ್ಷರ *n = ಖಾಲಿ;

                log_debug("Unit name \"%s\" too long, falling back to hashed unit name.", s);

                r = unit_name_hash_long(s, &n);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;

                free_and_replace(s, n);
        }

        /* Refuse if this for some other reason didn't result in a valid name */
        ಆದರೆ (!unit_name_is_valid(s, UNIT_NAME_PLAIN))
                ಹಿಂತಿರುಗಿ -EINVAL;

        *ret = TAKE_PTR(s);
        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ unit_name_from_ಮಾರ್ಗ_instance(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಪೂರ್ವಪ್ರತ್ಯಯ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *suffix, ಅಕ್ಷರ **ret) {
        _cleanup_free_ ಅಕ್ಷರ *p = ಖಾಲಿ, *s = ಖಾಲಿ;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(ಪೂರ್ವಪ್ರತ್ಯಯ);
        ಪ್ರತಿಜ್ಞಾ(ಮಾರ್ಗ);
        ಪ್ರತಿಜ್ಞಾ(suffix);
        ಪ್ರತಿಜ್ಞಾ(ret);

        ಆದರೆ (!unit_ಪೂರ್ವಪ್ರತ್ಯಯ_is_valid(ಪೂರ್ವಪ್ರತ್ಯಯ))
                ಹಿಂತಿರುಗಿ -EINVAL;

        ಆದರೆ (!unit_suffix_is_valid(suffix))
                ಹಿಂತಿರುಗಿ -EINVAL;

        r = unit_name_ಮಾರ್ಗ_escape(ಮಾರ್ಗ, &p);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        s = strjoin(ಪೂರ್ವಪ್ರತ್ಯಯ, "@", p, suffix);
        ಆದರೆ (!s)
                ಹಿಂತಿರುಗಿ -ENOMEM;

        ಆದರೆ (ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(s) >= UNIT_NAME_MAX) /* Return a slightly more descriptive error for this specific condition */
                ಹಿಂತಿರುಗಿ -ENAMETOOLONG;

        /* Refuse if this for some other reason didn't result in a valid name */
        ಆದರೆ (!unit_name_is_valid(s, UNIT_NAME_INSTANCE))
                ಹಿಂತಿರುಗಿ -EINVAL;

        *ret = TAKE_PTR(s);
        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ unit_name_ಗಮ್ಯ_ಮಾರ್ಗ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name, ಅಕ್ಷರ **ret) {
        _cleanup_free_ ಅಕ್ಷರ *ಪೂರ್ವಪ್ರತ್ಯಯ = ಖಾಲಿ;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(name);

        r = unit_name_ಗಮ್ಯ_ಪೂರ್ವಪ್ರತ್ಯಯ(name, &ಪೂರ್ವಪ್ರತ್ಯಯ);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಆದರೆ (unit_name_is_hashed(name))
                ಹಿಂತಿರುಗಿ -ENAMETOOLONG;

        ಹಿಂತಿರುಗಿ unit_name_ಮಾರ್ಗ_unescape(ಪೂರ್ವಪ್ರತ್ಯಯ, ret);
}

ಸ್ಥಿರ bool do_escape_mangle(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *f, bool allow_globs, ಅಕ್ಷರ *t) {
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *valid_chars;
        bool mangled = false;

        ಪ್ರತಿಜ್ಞಾ(f);
        ಪ್ರತಿಜ್ಞಾ(t);

        /* We'll only escape the obvious characters here, to play safe.
         *
         * Returns true if any characters were mangled, false otherwise.
         */

        valid_chars = allow_globs ? VALID_CHARS_GLOB : VALID_CHARS_WITH_AT;

        ಗಾಗಿ (; *f; f++)
                ಆದರೆ (*f == '/') {
                        *(t++) = '-';
                        mangled = true;
                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!strchr(valid_chars, *f)) {
                        t = do_escape_char(*f, t);
                        mangled = true;
                } ಇಲ್ಲದಿದ್ದರೆ
                        *(t++) = *f;
        *t = 0;

        ಹಿಂತಿರುಗಿ mangled;
}

/**
 *  Convert a string to a unit name. /dev/blah is converted to dev-blah.device,
 *  /blah/blah is converted to blah-blah.mount, anything else is left alone,
 *  except that @suffix is appended if a valid unit suffix is not present.
 *
 *  If @allow_globs, globs characters are preserved. Otherwise, they are escaped.
 */
ಪೂರ್ಣಾಂಕ unit_name_mangle_with_suffix(
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *operation,
                UnitNameMangle flags,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *suffix,
                ಅಕ್ಷರ **ret) {

        _cleanup_free_ ಅಕ್ಷರ *s = ಖಾಲಿ;
        bool mangled, suggest_escape = true, warn = FLAGS_ಗಣ(flags, UNIT_NAME_MANGLE_WARN);
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(name);
        ಪ್ರತಿಜ್ಞಾ(suffix);
        ಪ್ರತಿಜ್ಞಾ(ret);

        ಆದರೆ (isempty(name)) /* We cannot mangle empty unit names to become valid, sorry. */
                ಹಿಂತಿರುಗಿ -EINVAL;

        ಆದರೆ (!unit_suffix_is_valid(suffix))
                ಹಿಂತಿರುಗಿ -EINVAL;

        /* Already a fully valid unit name? If so, no mangling is necessary... */
        ಆದರೆ (unit_name_is_valid(name, UNIT_NAME_ANY)) {
                ಆದರೆ (FLAGS_ಗಣ(flags, UNIT_NAME_MANGLE_STRICT) && !endswith(name, suffix)) {
                        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *e = ASSERT_PTR(strrchr(name, '.'));

                        ಹಿಂತಿರುಗಿ log_ತುಂಬಿದ_errno(warn ? LOG_NOTICE : LOG_DEBUG,
                                              SYNTHETIC_ERRNO(EINVAL),
                                              "Unit name \"%s\" has unit type \"%s\", but \"%s\" is expected%s%s.",
                                              name, e + 1, suffix + 1,
                                              operation ? " " : "", strempty(operation));
                }

                ಹೋಗು good;
        }

        /* Already a fully valid globbing expression? If so, no mangling is necessary either... */
        ಆದರೆ (string_is_glob(name) && ಒಳ_charset(name, VALID_CHARS_GLOB)) {
                ಆದರೆ (FLAGS_ಗಣ(flags, UNIT_NAME_MANGLE_GLOB))
                        ಹೋಗು good;
                log_ತುಂಬಿದ(warn ? LOG_NOTICE : LOG_DEBUG,
                         "Glob pattern passed%s%s, but globs are not supported for this.",
                         operation ? " " : "", strempty(operation));
                suggest_escape = false;
        }

        ಆದರೆ (ಮಾರ್ಗ_is_absolute(name)) {
                _cleanup_free_ ಅಕ್ಷರ *n = ಖಾಲಿ, *u = ಖಾಲಿ;

                r = ಮಾರ್ಗ_simplify_alloc(name, &n);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;

                ಆದರೆ (is_device_ಮಾರ್ಗ(n)) {
                        r = unit_name_from_ಮಾರ್ಗ(n, ".device", &u);
                        ಆದರೆ (r >= 0) {
                                ಆದರೆ (FLAGS_ಗಣ(flags, UNIT_NAME_MANGLE_STRICT) && !streq(suffix, ".device"))
                                        ಹಿಂತಿರುಗಿ log_ತುಂಬಿದ_errno(warn ? LOG_NOTICE : LOG_DEBUG,
                                                              SYNTHETIC_ERRNO(EINVAL),
                                                              "Path \"%s\" resolves to unit type \"device\", but \"%s\" is expected%s%s.",
                                                              name, suffix + 1,
                                                              operation ? " " : "", strempty(operation));

                                *ret = TAKE_PTR(u);
                                ಹಿಂತಿರುಗಿ 1;
                        }
                        ಆದರೆ (r != -EINVAL)
                                ಹಿಂತಿರುಗಿ r;
                }

                r = unit_name_from_ಮಾರ್ಗ(n, ".mount", &u);
                ಆದರೆ (r >= 0) {
                        ಆದರೆ (FLAGS_ಗಣ(flags, UNIT_NAME_MANGLE_STRICT) && !streq(suffix, ".mount"))
                                ಹಿಂತಿರುಗಿ log_ತುಂಬಿದ_errno(warn ? LOG_NOTICE : LOG_DEBUG,
                                                      SYNTHETIC_ERRNO(EINVAL),
                                                      "Path \"%s\" resolves to unit type \"mount\", but \"%s\" is expected%s%s.",
                                                      name, suffix + 1,
                                                      operation ? " " : "", strempty(operation));

                        *ret = TAKE_PTR(u);
                        ಹಿಂತಿರುಗಿ 1;
                }
                ಆದರೆ (r != -EINVAL)
                        ಹಿಂತಿರುಗಿ r;
        }

        s = new(ಅಕ್ಷರ, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(name) * 4 + ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(suffix) + 1);
        ಆದರೆ (!s)
                ಹಿಂತಿರುಗಿ -ENOMEM;

        mangled = do_escape_mangle(name, FLAGS_ಗಣ(flags, UNIT_NAME_MANGLE_GLOB), s);
        ಆದರೆ (mangled)
                log_ತುಂಬಿದ(warn ? LOG_NOTICE : LOG_DEBUG,
                         "Invalid unit name \"%s\" escaped as \"%s\"%s.",
                         name, s,
                         suggest_escape ? " (maybe you should use systemd-escape?)" : "");

        /* Append a suffix if it doesn't have any, but only if this is not a glob, so that we can allow
         * "foo.*" as a valid glob. */
        ಆದರೆ ((!FLAGS_ಗಣ(flags, UNIT_NAME_MANGLE_GLOB) || !string_is_glob(s)) && unit_name_ಗಮ್ಯ_type(s) < 0)
                strcat(s, suffix);

        /* Make sure mangling didn't grow this too large (but don't do this check if globbing is allowed,
         * since globs generally do not qualify as valid unit names) */
        ಆದರೆ (!FLAGS_ಗಣ(flags, UNIT_NAME_MANGLE_GLOB) && !unit_name_is_valid(s, UNIT_NAME_ANY))
                ಹಿಂತಿರುಗಿ -EINVAL;

        *ret = TAKE_PTR(s);
        ಹಿಂತಿರುಗಿ 1;

good:
        ಹಿಂತಿರುಗಿ strdup_ಗಮ್ಯ(ret, name);
}

ಪೂರ್ಣಾಂಕ ತುಂಡು_build_ಪೋಷಕ_ತುಂಡು(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ತುಂಡು, ಅಕ್ಷರ **ret) {
        ಪ್ರತಿಜ್ಞಾ(ತುಂಡು);
        ಪ್ರತಿಜ್ಞಾ(ret);

        ಆದರೆ (!ತುಂಡು_name_is_valid(ತುಂಡು))
                ಹಿಂತಿರುಗಿ -EINVAL;

        ಆದರೆ (streq(ತುಂಡು, SPECIAL_ಬೇರು_ತುಂಡು)) {
                *ret = ಖಾಲಿ;
                ಹಿಂತಿರುಗಿ 0;
        }

        _cleanup_free_ ಅಕ್ಷರ *s = strdup(ತುಂಡು);
        ಆದರೆ (!s)
                ಹಿಂತಿರುಗಿ -ENOMEM;

        ಅಕ್ಷರ *dash = strrchr(s, '-');
        ಆದರೆ (!dash)
                ಹಿಂತಿರುಗಿ strdup_ಗಮ್ಯ_ತುಂಬಿದ(ret, SPECIAL_ಬೇರು_ತುಂಡು);

        /* We know that s ended with .slice before truncation, so we have enough space. */
        strcpy(dash, ".slice");

        *ret = TAKE_PTR(s);
        ಹಿಂತಿರುಗಿ 1;
}

ಪೂರ್ಣಾಂಕ ತುಂಡು_build_subslice(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ತುಂಡು, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name, ಅಕ್ಷರ **ret) {
        ಅಕ್ಷರ *subslice;

        ಪ್ರತಿಜ್ಞಾ(ತುಂಡು);
        ಪ್ರತಿಜ್ಞಾ(name);
        ಪ್ರತಿಜ್ಞಾ(ret);

        ಆದರೆ (!ತುಂಡು_name_is_valid(ತುಂಡು))
                ಹಿಂತಿರುಗಿ -EINVAL;

        ಆದರೆ (!unit_ಪೂರ್ವಪ್ರತ್ಯಯ_is_valid(name))
                ಹಿಂತಿರುಗಿ -EINVAL;

        ಆದರೆ (streq(ತುಂಡು, SPECIAL_ಬೇರು_ತುಂಡು))
                subslice = strjoin(name, ".slice");
        ಇಲ್ಲದಿದ್ದರೆ {
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *e;

                assert_se(e = endswith(ತುಂಡು, ".slice"));

                subslice = new(ಅಕ್ಷರ, (e - ತುಂಡು) + 1 + ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(name) + 6 + 1);
                ಆದರೆ (!subslice)
                        ಹಿಂತಿರುಗಿ -ENOMEM;

                stpcpy(stpcpy(stpcpy(mempcpy(subslice, ತುಂಡು, e - ತುಂಡು), "-"), name), ".slice");
        }

        *ret = subslice;
        ಹಿಂತಿರುಗಿ 0;
}

bool ತುಂಡು_name_is_valid(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name) {
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p, *e;
        bool dash = false;

        ಆದರೆ (!unit_name_is_valid(name, UNIT_NAME_PLAIN))
                ಹಿಂತಿರುಗಿ false;

        ಆದರೆ (streq(name, SPECIAL_ಬೇರು_ತುಂಡು))
                ಹಿಂತಿರುಗಿ true;

        e = endswith(name, ".slice");
        ಆದರೆ (!e)
                ಹಿಂತಿರುಗಿ false;

        ಗಾಗಿ (p = name; p < e; p++) {

                ಆದರೆ (*p == '-') {

                        /* Don't allow initial dash */
                        ಆದರೆ (p == name)
                                ಹಿಂತಿರುಗಿ false;

                        /* Don't allow multiple dashes */
                        ಆದರೆ (dash)
                                ಹಿಂತಿರುಗಿ false;

                        dash = true;
                } ಇಲ್ಲದಿದ್ದರೆ
                        dash = false;
        }

        /* Don't allow trailing hash */
        ಆದರೆ (dash)
                ಹಿಂತಿರುಗಿ false;

        ಹಿಂತಿರುಗಿ true;
}

bool unit_name_ಪೂರ್ವಪ್ರತ್ಯಯ_equal(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *a, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *b) {
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p, *q;

        ಪ್ರತಿಜ್ಞಾ(a);
        ಪ್ರತಿಜ್ಞಾ(b);

        ಆದರೆ (!unit_name_is_valid(a, UNIT_NAME_ANY) || !unit_name_is_valid(b, UNIT_NAME_ANY))
                ಹಿಂತಿರುಗಿ false;

        p = strchr(a, '@');
        ಆದರೆ (!p)
                p = strrchr(a, '.');

        q = strchr(b, '@');
        ಆದರೆ (!q)
                q = strrchr(b, '.');

        ಪ್ರತಿಜ್ಞಾ(p);
        ಪ್ರತಿಜ್ಞಾ(q);

        ಹಿಂತಿರುಗಿ memcmp_nn(a, p - a, b, q - b) == 0;
}
