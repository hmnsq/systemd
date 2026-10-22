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
#define VALID_CHARS                             \
        ALPHANUMERICAL                          \
        ":-_.\\"

/* The same, but also permits the single @ character that may appear */
#define VALID_CHARS_WITH_AT                     \
        "@"                                     \
        VALID_CHARS

/* All chars valid in a unit name glob */
#define VALID_CHARS_GLOB                        \
        VALID_CHARS_WITH_AT                     \
        "[]!-*?"

#define LONG_UNIT_NAME_HASH_ಕೀಲಿ SD_ID128_MAKE(ec,f2,37,fb,58,32,4a,32,84,9f,06,9b,0d,21,eb,9a)
#define UNIT_NAME_HASH_LENGTH_CHARS 16

bool unit_name_is_valid(ಧ್ರುವ ವರ್ಣ *n, UnitNameFlags flags) {
        ಧ್ರುವ ವರ್ಣ *e, *i, *at;

        ಪ್ರತಿಜ್ಞಾ((flags & ~(UNIT_NAME_PLAIN|UNIT_NAME_INSTANCE|UNIT_NAME_TEMPLATE)) == 0);

        ಯದಿ (_unlikely_(flags == 0))
                ಪ್ರತ್ಯಾಗಮ false;

        ಯದಿ (isempty(n))
                ಪ್ರತ್ಯಾಗಮ false;

        ಯದಿ (strlen(n) >= UNIT_NAME_MAX)
                ಪ್ರತ್ಯಾಗಮ false;

        e = strrchr(n, '.');
        ಯದಿ (!e || e == n)
                ಪ್ರತ್ಯಾಗಮ false;

        ಯದಿ (unit_type_from_string(e + 1) < 0)
                ಪ್ರತ್ಯಾಗಮ false;

        ಪ್ರತಿ (i = n, at = ರಿಕ್ತ; i < e; i++) {

                ಯದಿ (*i == '@' && !at)
                        at = i;

                ಯದಿ (!strchr(VALID_CHARS_WITH_AT, *i))
                        ಪ್ರತ್ಯಾಗಮ false;
        }

        ಯದಿ (at == n)
                ಪ್ರತ್ಯಾಗಮ false;

        ಯದಿ (flags & UNIT_NAME_PLAIN)
                ಯದಿ (!at)
                        ಪ್ರತ್ಯಾಗಮ true;

        ಯದಿ (flags & UNIT_NAME_INSTANCE)
                ಯದಿ (at && e > at + 1)
                        ಪ್ರತ್ಯಾಗಮ true;

        ಯದಿ (flags & UNIT_NAME_TEMPLATE)
                ಯದಿ (at && e == at + 1)
                        ಪ್ರತ್ಯಾಗಮ true;

        ಪ್ರತ್ಯಾಗಮ false;
}

bool unit_prefix_is_valid(ಧ್ರುವ ವರ್ಣ *p) {

        /* We don't allow additional @ in the prefix string */

        ಯದಿ (isempty(p))
                ಪ್ರತ್ಯಾಗಮ false;

        ಪ್ರತ್ಯಾಗಮ in_charset(p, VALID_CHARS);
}

bool unit_instance_is_valid(ಧ್ರುವ ವರ್ಣ *i) {

        /* The max length depends on the length of the string, so we
         * don't really check this here. */

        ಯದಿ (isempty(i))
                ಪ್ರತ್ಯಾಗಮ false;

        /* We allow additional @ in the instance string, we do not
         * allow them in the prefix! */

        ಪ್ರತ್ಯಾಗಮ in_charset(i, "@" VALID_CHARS);
}

bool unit_suffix_is_valid(ಧ್ರುವ ವರ್ಣ *s) {
        ಯದಿ (isempty(s))
                ಪ್ರತ್ಯಾಗಮ false;

        ಯದಿ (s[0] != '.')
                ಪ್ರತ್ಯಾಗಮ false;

        ಯದಿ (unit_type_from_string(s + 1) < 0)
                ಪ್ರತ್ಯಾಗಮ false;

        ಪ್ರತ್ಯಾಗಮ true;
}

ಪೂರ್ಣಾಙ್ಕ unit_name_to_prefix(ಧ್ರುವ ವರ್ಣ *n, ವರ್ಣ **ret) {
        ಧ್ರುವ ವರ್ಣ *p;
        ವರ್ಣ *s;

        ಪ್ರತಿಜ್ಞಾ(n);
        ಪ್ರತಿಜ್ಞಾ(ret);

        ಯದಿ (!unit_name_is_valid(n, UNIT_NAME_ANY))
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

        p = strchr(n, '@');
        ಯದಿ (!p)
                p = strrchr(n, '.');

        assert_se(p);

        s = strndup(n, p - n);
        ಯದಿ (!s)
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        *ret = s;
        ಪ್ರತ್ಯಾಗಮ 0;
}

UnitNameFlags unit_name_to_instance(ಧ್ರುವ ವರ್ಣ *n, ವರ್ಣ **ret) {
        ಧ್ರುವ ವರ್ಣ *p, *d;

        ಪ್ರತಿಜ್ಞಾ(n);

        ಯದಿ (!unit_name_is_valid(n, UNIT_NAME_ANY))
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

        /* Everything past the first @ and before the last . is the instance */
        p = strchr(n, '@');
        ಯದಿ (!p) {
                ಯದಿ (ret)
                        *ret = ರಿಕ್ತ;
                ಪ್ರತ್ಯಾಗಮ UNIT_NAME_PLAIN;
        }

        p++;

        d = strrchr(p, '.');
        ಯದಿ (!d)
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

        ಯದಿ (ret) {
                ವರ್ಣ *i = strndup(p, d-p);
                ಯದಿ (!i)
                        ಪ್ರತ್ಯಾಗಮ -ENOMEM;

                *ret = i;
        }
        ಪ್ರತ್ಯಾಗಮ d > p ? UNIT_NAME_INSTANCE : UNIT_NAME_TEMPLATE;
}

ಪೂರ್ಣಾಙ್ಕ unit_name_to_prefix_and_instance(ಧ್ರುವ ವರ್ಣ *n, ವರ್ಣ **ret) {
        ಧ್ರುವ ವರ್ಣ *d;
        ವರ್ಣ *s;

        ಪ್ರತಿಜ್ಞಾ(n);
        ಪ್ರತಿಜ್ಞಾ(ret);

        ಯದಿ (!unit_name_is_valid(n, UNIT_NAME_ANY))
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

        d = strrchr(n, '.');
        ಯದಿ (!d)
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

        s = strndup(n, d - n);
        ಯದಿ (!s)
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        *ret = s;
        ಪ್ರತ್ಯಾಗಮ 0;
}

UnitType unit_name_to_type(ಧ್ರುವ ವರ್ಣ *n) {
        ಧ್ರುವ ವರ್ಣ *e;

        ಪ್ರತಿಜ್ಞಾ(n);

        ಯದಿ (!unit_name_is_valid(n, UNIT_NAME_ANY))
                ಪ್ರತ್ಯಾಗಮ _UNIT_TYPE_INVALID;

        assert_se(e = strrchr(n, '.'));

        ಪ್ರತ್ಯಾಗಮ unit_type_from_string(e + 1);
}

ಪೂರ್ಣಾಙ್ಕ unit_name_change_suffix(ಧ್ರುವ ವರ್ಣ *n, ಧ್ರುವ ವರ್ಣ *suffix, ವರ್ಣ **ret) {
        _cleanup_free_ ವರ್ಣ *s = ರಿಕ್ತ;
        size_t a, b;
        ಧ್ರುವ ವರ್ಣ *e;

        ಪ್ರತಿಜ್ಞಾ(n);
        ಪ್ರತಿಜ್ಞಾ(suffix);
        ಪ್ರತಿಜ್ಞಾ(ret);

        ಯದಿ (!unit_name_is_valid(n, UNIT_NAME_ANY))
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

        ಯದಿ (!unit_suffix_is_valid(suffix))
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

        assert_se(e = strrchr(n, '.'));

        a = e - n;
        b = strlen(suffix);

        s = new(ವರ್ಣ, a + b + 1);
        ಯದಿ (!s)
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        strcpy(mempcpy(s, n, a), suffix);

        /* Make sure the name is still valid (i.e. didn't grow too large due to longer suffix) */
        ಯದಿ (!unit_name_is_valid(s, UNIT_NAME_ANY))
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

        *ret = TAKE_PTR(s);
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ unit_name_build(ಧ್ರುವ ವರ್ಣ *prefix, ಧ್ರುವ ವರ್ಣ *instance, ಧ್ರುವ ವರ್ಣ *suffix, ವರ್ಣ **ret) {
        UnitType type;

        ಪ್ರತಿಜ್ಞಾ(prefix);
        ಪ್ರತಿಜ್ಞಾ(suffix);
        ಪ್ರತಿಜ್ಞಾ(ret);

        ಯದಿ (suffix[0] != '.')
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

        type = unit_type_from_string(suffix + 1);
        ಯದಿ (type < 0)
                ಪ್ರತ್ಯಾಗಮ type;

        ಪ್ರತ್ಯಾಗಮ unit_name_build_from_type(prefix, instance, type, ret);
}

ಪೂರ್ಣಾಙ್ಕ unit_name_build_from_type(ಧ್ರುವ ವರ್ಣ *prefix, ಧ್ರುವ ವರ್ಣ *instance, UnitType type, ವರ್ಣ **ret) {
        _cleanup_free_ ವರ್ಣ *s = ರಿಕ್ತ;
        ಧ್ರುವ ವರ್ಣ *ut;

        ಪ್ರತಿಜ್ಞಾ(prefix);
        ಪ್ರತಿಜ್ಞಾ(type >= 0);
        ಪ್ರತಿಜ್ಞಾ(type < _UNIT_TYPE_MAX);
        ಪ್ರತಿಜ್ಞಾ(ret);

        ಯದಿ (!unit_prefix_is_valid(prefix))
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

        ut = unit_type_to_string(type);

        ಯದಿ (instance) {
                ಯದಿ (!unit_instance_is_valid(instance))
                        ಪ್ರತ್ಯಾಗಮ -EINVAL;

                s = strjoin(prefix, "@", instance, ".", ut);
        } ಅನ್ಯಥಾ
                s = strjoin(prefix, ".", ut);
        ಯದಿ (!s)
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        /* Verify that this didn't grow too large (or otherwise is invalid) */
        ಯದಿ (!unit_name_is_valid(s, instance ? UNIT_NAME_INSTANCE : UNIT_NAME_PLAIN))
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

        *ret = TAKE_PTR(s);
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ವರ್ಣ *do_escape_char(ವರ್ಣ c, ವರ್ಣ *t) {
        ಪ್ರತಿಜ್ಞಾ(t);

        *(t++) = '\\';
        *(t++) = 'x';
        *(t++) = hexchar(c >> 4);
        *(t++) = hexchar(c);

        ಪ್ರತ್ಯಾಗಮ t;
}

ಸ್ಥಿರ ವರ್ಣ *do_escape(ಧ್ರುವ ವರ್ಣ *f, ವರ್ಣ *t) {
        ಪ್ರತಿಜ್ಞಾ(f);
        ಪ್ರತಿಜ್ಞಾ(t);

        /* do not create units with a leading '.', like for "/.dotdir" mount points */
        ಯದಿ (*f == '.') {
                t = do_escape_char(*f, t);
                f++;
        }

        ಪ್ರತಿ (; *f; f++) {
                ಯದಿ (*f == '/')
                        *(t++) = '-';
                ಅನ್ಯಥಾ ಯದಿ (IN_SET(*f, '-', '\\') || !strchr(VALID_CHARS, *f))
                        t = do_escape_char(*f, t);
                ಅನ್ಯಥಾ
                        *(t++) = *f;
        }

        ಪ್ರತ್ಯಾಗಮ t;
}

ವರ್ಣ* unit_name_escape(ಧ್ರುವ ವರ್ಣ *f) {
        ವರ್ಣ *r, *t;

        ಪ್ರತಿಜ್ಞಾ(f);

        r = new(ವರ್ಣ, strlen(f)*4+1);
        ಯದಿ (!r)
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        t = do_escape(f, r);
        *t = 0;

        ಪ್ರತ್ಯಾಗಮ r;
}

ಪೂರ್ಣಾಙ್ಕ unit_name_unescape(ಧ್ರುವ ವರ್ಣ *f, ವರ್ಣ **ret) {
        _cleanup_free_ ವರ್ಣ *r = ರಿಕ್ತ;
        ವರ್ಣ *t;

        ಪ್ರತಿಜ್ಞಾ(f);
        ಪ್ರತಿಜ್ಞಾ(ret);

        r = strdup(f);
        ಯದಿ (!r)
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        ಪ್ರತಿ (t = r; *f; f++) {
                ಯದಿ (*f == '-')
                        *(t++) = '/';
                ಅನ್ಯಥಾ ಯದಿ (*f == '\\') {
                        ಪೂರ್ಣಾಙ್ಕ a, b;

                        ಯದಿ (f[1] != 'x')
                                ಪ್ರತ್ಯಾಗಮ -EINVAL;

                        a = unhexchar(f[2]);
                        ಯದಿ (a < 0)
                                ಪ್ರತ್ಯಾಗಮ -EINVAL;

                        b = unhexchar(f[3]);
                        ಯದಿ (b < 0)
                                ಪ್ರತ್ಯಾಗಮ -EINVAL;

                        *(t++) = (ವರ್ಣ) (((uint8_t) a << 4U) | (uint8_t) b);
                        f += 3;
                } ಅನ್ಯಥಾ
                        *(t++) = *f;
        }

        *t = 0;

        *ret = TAKE_PTR(r);

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ unit_name_path_escape(ಧ್ರುವ ವರ್ಣ *f, ವರ್ಣ **ret) {
        _cleanup_free_ ವರ್ಣ *p = ರಿಕ್ತ;
        ವರ್ಣ *s;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(f);
        ಪ್ರತಿಜ್ಞಾ(ret);

        r = path_simplify_alloc(f, &p);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಯದಿ (empty_or_ಬೇರು(p))
                s = strdup("-");
        ಅನ್ಯಥಾ {
                ಯದಿ (!path_is_normalized(p))
                        ಪ್ರತ್ಯಾಗಮ -EINVAL;

                /* Truncate trailing slashes and skip leading slashes */
                delete_trailing_chars(p, "/");
                s = unit_name_escape(skip_leading_chars(p, "/"));
        }
        ಯದಿ (!s)
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        *ret = s;
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ unit_name_path_unescape(ಧ್ರುವ ವರ್ಣ *f, ವರ್ಣ **ret) {
        _cleanup_free_ ವರ್ಣ *s = ರಿಕ್ತ;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(f);

        ಯದಿ (isempty(f))
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

        ಯದಿ (streq(f, "-")) {
                s = strdup("/");
                ಯದಿ (!s)
                        ಪ್ರತ್ಯಾಗಮ -ENOMEM;
        } ಅನ್ಯಥಾ {
                _cleanup_free_ ವರ್ಣ *w = ರಿಕ್ತ;

                r = unit_name_unescape(f, &w);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;

                /* Don't accept trailing or leading slashes */
                ಯದಿ (startswith(w, "/") || endswith(w, "/"))
                        ಪ್ರತ್ಯಾಗಮ -EINVAL;

                /* Prefix a slash again */
                s = strjoin("/", w);
                ಯದಿ (!s)
                        ಪ್ರತ್ಯಾಗಮ -ENOMEM;

                ಯದಿ (!path_is_normalized(s))
                        ಪ್ರತ್ಯಾಗಮ -EINVAL;
        }

        ಯದಿ (ret)
                *ret = TAKE_PTR(s);

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ unit_name_replace_instance_full(
                ಧ್ರುವ ವರ್ಣ *original,
                ಧ್ರುವ ವರ್ಣ *instance,
                bool accept_glob,
                ವರ್ಣ **ret) {

        _cleanup_free_ ವರ್ಣ *s = ರಿಕ್ತ;
        ಧ್ರುವ ವರ್ಣ *prefix, *suffix;
        size_t pl;

        ಪ್ರತಿಜ್ಞಾ(original);
        ಪ್ರತಿಜ್ಞಾ(instance);
        ಪ್ರತಿಜ್ಞಾ(ret);

        ಯದಿ (!unit_name_is_valid(original, UNIT_NAME_INSTANCE|UNIT_NAME_TEMPLATE))
                ಪ್ರತ್ಯಾಗಮ -EINVAL;
        ಯದಿ (!unit_instance_is_valid(instance) && !(accept_glob && in_charset(instance, VALID_CHARS_GLOB)))
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

        prefix = ASSERT_PTR(strchr(original, '@'));
        suffix = ASSERT_PTR(strrchr(original, '.'));
        ಪ್ರತಿಜ್ಞಾ(prefix < suffix);

        pl = prefix - original + 1; /* include '@' */

        s = new(ವರ್ಣ, pl + strlen(instance) + strlen(suffix) + 1);
        ಯದಿ (!s)
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

#ಯದಿ HAS_FEATURE_MEMORY_SANITIZER
        /* MSan doesn't like stpncpy... See also https://github.com/google/sanitizers/issues/926 */
        memzero(s, pl + strlen(instance) + strlen(suffix) + 1);
#endif

        strcpy(stpcpy(stpncpy(s, original, pl), instance), suffix);

        /* Make sure the resulting name still is valid, i.e. didn't grow too large. Globs will be expanded
         * by clients when used, so the check is pointless. */
        ಯದಿ (!accept_glob && !unit_name_is_valid(s, UNIT_NAME_INSTANCE))
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

        *ret = TAKE_PTR(s);
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ unit_name_template(ಧ್ರುವ ವರ್ಣ *f, ವರ್ಣ **ret) {
        ಧ್ರುವ ವರ್ಣ *p, *e;
        ವರ್ಣ *s;
        size_t a;

        ಪ್ರತಿಜ್ಞಾ(f);
        ಪ್ರತಿಜ್ಞಾ(ret);

        ಯದಿ (!unit_name_is_valid(f, UNIT_NAME_INSTANCE|UNIT_NAME_TEMPLATE))
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

        assert_se(p = strchr(f, '@'));
        assert_se(e = strrchr(f, '.'));

        a = p - f;

        s = new(ವರ್ಣ, a + 1 + strlen(e) + 1);
        ಯದಿ (!s)
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        strcpy(mempcpy(s, f, a + 1), e);

        *ret = s;
        ಪ್ರತ್ಯಾಗಮ 0;
}

bool unit_name_is_hashed(ಧ್ರುವ ವರ್ಣ *name) {
        ಧ್ರುವ ವರ್ಣ *s;

        ಯದಿ (!unit_name_is_valid(name, UNIT_NAME_PLAIN))
                ಪ್ರತ್ಯಾಗಮ false;

        assert_se(s = strrchr(name, '.'));

        ಯದಿ (s - name < UNIT_NAME_HASH_LENGTH_CHARS + 1)
                ಪ್ರತ್ಯಾಗಮ false;

        s -= UNIT_NAME_HASH_LENGTH_CHARS;
        ಯದಿ (s[-1] != '_')
                ಪ್ರತ್ಯಾಗಮ false;

        ಪ್ರತಿ (size_t i = 0; i < UNIT_NAME_HASH_LENGTH_CHARS; i++)
                ಯದಿ (!strchr(LOWERCASE_HEXDIGITS, s[i]))
                        ಪ್ರತ್ಯಾಗಮ false;

        ಪ್ರತ್ಯಾಗಮ true;
}

ಪೂರ್ಣಾಙ್ಕ unit_name_hash_long(ಧ್ರುವ ವರ್ಣ *name, ವರ್ಣ **ret) {
        _cleanup_free_ ವರ್ಣ *n = ರಿಕ್ತ, *hash = ರಿಕ್ತ;
        ಧ್ರುವ ವರ್ಣ *suffix;
        le64_t h;
        size_t len;

        ಪ್ರತಿಜ್ಞಾ(ret);

        ಯದಿ (strlen(name) < UNIT_NAME_MAX)
                ಪ್ರತ್ಯಾಗಮ -EMSGSIZE;

        suffix = strrchr(name, '.');
        ಯದಿ (!suffix)
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

        ಯದಿ (unit_type_from_string(suffix+1) < 0)
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

        h = htole64(siphash24_string(name, LONG_UNIT_NAME_HASH_ಕೀಲಿ.bytes));

        hash = hexmem(&h, ಪರಿಮಾಣ(h));
        ಯದಿ (!hash)
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        assert_se(strlen(hash) == UNIT_NAME_HASH_LENGTH_CHARS);

        len = UNIT_NAME_MAX - 1 - strlen(suffix+1) - UNIT_NAME_HASH_LENGTH_CHARS - 2;
        ಪ್ರತಿಜ್ಞಾ(len > 0 && len < UNIT_NAME_MAX);

        n = strndup(name, len);
        ಯದಿ (!n)
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        ಯದಿ (!strextend(&n, "_", hash, suffix))
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;
        assert_se(unit_name_is_valid(n, UNIT_NAME_PLAIN));

        *ret = TAKE_PTR(n);

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ unit_name_from_path(ಧ್ರುವ ವರ್ಣ *path, ಧ್ರುವ ವರ್ಣ *suffix, ವರ್ಣ **ret) {
        _cleanup_free_ ವರ್ಣ *p = ರಿಕ್ತ, *s = ರಿಕ್ತ;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(path);
        ಪ್ರತಿಜ್ಞಾ(suffix);
        ಪ್ರತಿಜ್ಞಾ(ret);

        ಯದಿ (!unit_suffix_is_valid(suffix))
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

        r = unit_name_path_escape(path, &p);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        s = strjoin(p, suffix);
        ಯದಿ (!s)
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        ಯದಿ (strlen(s) >= UNIT_NAME_MAX) {
                _cleanup_free_ ವರ್ಣ *n = ರಿಕ್ತ;

                log_debug("Unit name \"%s\" too long, falling back to hashed unit name.", s);

                r = unit_name_hash_long(s, &n);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;

                free_and_replace(s, n);
        }

        /* Refuse if this for some other reason didn't result in a valid name */
        ಯದಿ (!unit_name_is_valid(s, UNIT_NAME_PLAIN))
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

        *ret = TAKE_PTR(s);
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ unit_name_from_path_instance(ಧ್ರುವ ವರ್ಣ *prefix, ಧ್ರುವ ವರ್ಣ *path, ಧ್ರುವ ವರ್ಣ *suffix, ವರ್ಣ **ret) {
        _cleanup_free_ ವರ್ಣ *p = ರಿಕ್ತ, *s = ರಿಕ್ತ;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(prefix);
        ಪ್ರತಿಜ್ಞಾ(path);
        ಪ್ರತಿಜ್ಞಾ(suffix);
        ಪ್ರತಿಜ್ಞಾ(ret);

        ಯದಿ (!unit_prefix_is_valid(prefix))
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

        ಯದಿ (!unit_suffix_is_valid(suffix))
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

        r = unit_name_path_escape(path, &p);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        s = strjoin(prefix, "@", p, suffix);
        ಯದಿ (!s)
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        ಯದಿ (strlen(s) >= UNIT_NAME_MAX) /* Return a slightly more descriptive error for this specific condition */
                ಪ್ರತ್ಯಾಗಮ -ENAMETOOLONG;

        /* Refuse if this for some other reason didn't result in a valid name */
        ಯದಿ (!unit_name_is_valid(s, UNIT_NAME_INSTANCE))
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

        *ret = TAKE_PTR(s);
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ unit_name_to_path(ಧ್ರುವ ವರ್ಣ *name, ವರ್ಣ **ret) {
        _cleanup_free_ ವರ್ಣ *prefix = ರಿಕ್ತ;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(name);

        r = unit_name_to_prefix(name, &prefix);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಯದಿ (unit_name_is_hashed(name))
                ಪ್ರತ್ಯಾಗಮ -ENAMETOOLONG;

        ಪ್ರತ್ಯಾಗಮ unit_name_path_unescape(prefix, ret);
}

ಸ್ಥಿರ bool do_escape_mangle(ಧ್ರುವ ವರ್ಣ *f, bool allow_globs, ವರ್ಣ *t) {
        ಧ್ರುವ ವರ್ಣ *valid_chars;
        bool mangled = false;

        ಪ್ರತಿಜ್ಞಾ(f);
        ಪ್ರತಿಜ್ಞಾ(t);

        /* We'll only escape the obvious characters here, to play safe.
         *
         * Returns true if any characters were mangled, false otherwise.
         */

        valid_chars = allow_globs ? VALID_CHARS_GLOB : VALID_CHARS_WITH_AT;

        ಪ್ರತಿ (; *f; f++)
                ಯದಿ (*f == '/') {
                        *(t++) = '-';
                        mangled = true;
                } ಅನ್ಯಥಾ ಯದಿ (!strchr(valid_chars, *f)) {
                        t = do_escape_char(*f, t);
                        mangled = true;
                } ಅನ್ಯಥಾ
                        *(t++) = *f;
        *t = 0;

        ಪ್ರತ್ಯಾಗಮ mangled;
}

/**
 *  Convert a string to a unit name. /dev/blah is converted to dev-blah.device,
 *  /blah/blah is converted to blah-blah.mount, anything else is left alone,
 *  except that @suffix is appended if a valid unit suffix is not present.
 *
 *  If @allow_globs, globs characters are preserved. Otherwise, they are escaped.
 */
ಪೂರ್ಣಾಙ್ಕ unit_name_mangle_with_suffix(
                ಧ್ರುವ ವರ್ಣ *name,
                ಧ್ರುವ ವರ್ಣ *operation,
                UnitNameMangle flags,
                ಧ್ರುವ ವರ್ಣ *suffix,
                ವರ್ಣ **ret) {

        _cleanup_free_ ವರ್ಣ *s = ರಿಕ್ತ;
        bool mangled, suggest_escape = true, warn = FLAGS_SET(flags, UNIT_NAME_MANGLE_WARN);
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(name);
        ಪ್ರತಿಜ್ಞಾ(suffix);
        ಪ್ರತಿಜ್ಞಾ(ret);

        ಯದಿ (isempty(name)) /* We cannot mangle empty unit names to become valid, sorry. */
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

        ಯದಿ (!unit_suffix_is_valid(suffix))
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

        /* Already a fully valid unit name? If so, no mangling is necessary... */
        ಯದಿ (unit_name_is_valid(name, UNIT_NAME_ANY)) {
                ಯದಿ (FLAGS_SET(flags, UNIT_NAME_MANGLE_STRICT) && !endswith(name, suffix)) {
                        ಧ್ರುವ ವರ್ಣ *e = ASSERT_PTR(strrchr(name, '.'));

                        ಪ್ರತ್ಯಾಗಮ log_full_errno(warn ? LOG_NOTICE : LOG_DEBUG,
                                              SYNTHETIC_ERRNO(EINVAL),
                                              "Unit name \"%s\" has unit type \"%s\", but \"%s\" is expected%s%s.",
                                              name, e + 1, suffix + 1,
                                              operation ? " " : "", strempty(operation));
                }

                ಗಚ್ಛ good;
        }

        /* Already a fully valid globbing expression? If so, no mangling is necessary either... */
        ಯದಿ (string_is_glob(name) && in_charset(name, VALID_CHARS_GLOB)) {
                ಯದಿ (FLAGS_SET(flags, UNIT_NAME_MANGLE_GLOB))
                        ಗಚ್ಛ good;
                log_full(warn ? LOG_NOTICE : LOG_DEBUG,
                         "Glob pattern passed%s%s, but globs are not supported for this.",
                         operation ? " " : "", strempty(operation));
                suggest_escape = false;
        }

        ಯದಿ (path_is_absolute(name)) {
                _cleanup_free_ ವರ್ಣ *n = ರಿಕ್ತ, *u = ರಿಕ್ತ;

                r = path_simplify_alloc(name, &n);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;

                ಯದಿ (is_device_path(n)) {
                        r = unit_name_from_path(n, ".device", &u);
                        ಯದಿ (r >= 0) {
                                ಯದಿ (FLAGS_SET(flags, UNIT_NAME_MANGLE_STRICT) && !streq(suffix, ".device"))
                                        ಪ್ರತ್ಯಾಗಮ log_full_errno(warn ? LOG_NOTICE : LOG_DEBUG,
                                                              SYNTHETIC_ERRNO(EINVAL),
                                                              "Path \"%s\" resolves to unit type \"device\", but \"%s\" is expected%s%s.",
                                                              name, suffix + 1,
                                                              operation ? " " : "", strempty(operation));

                                *ret = TAKE_PTR(u);
                                ಪ್ರತ್ಯಾಗಮ 1;
                        }
                        ಯದಿ (r != -EINVAL)
                                ಪ್ರತ್ಯಾಗಮ r;
                }

                r = unit_name_from_path(n, ".mount", &u);
                ಯದಿ (r >= 0) {
                        ಯದಿ (FLAGS_SET(flags, UNIT_NAME_MANGLE_STRICT) && !streq(suffix, ".mount"))
                                ಪ್ರತ್ಯಾಗಮ log_full_errno(warn ? LOG_NOTICE : LOG_DEBUG,
                                                      SYNTHETIC_ERRNO(EINVAL),
                                                      "Path \"%s\" resolves to unit type \"mount\", but \"%s\" is expected%s%s.",
                                                      name, suffix + 1,
                                                      operation ? " " : "", strempty(operation));

                        *ret = TAKE_PTR(u);
                        ಪ್ರತ್ಯಾಗಮ 1;
                }
                ಯದಿ (r != -EINVAL)
                        ಪ್ರತ್ಯಾಗಮ r;
        }

        s = new(ವರ್ಣ, strlen(name) * 4 + strlen(suffix) + 1);
        ಯದಿ (!s)
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        mangled = do_escape_mangle(name, FLAGS_SET(flags, UNIT_NAME_MANGLE_GLOB), s);
        ಯದಿ (mangled)
                log_full(warn ? LOG_NOTICE : LOG_DEBUG,
                         "Invalid unit name \"%s\" escaped as \"%s\"%s.",
                         name, s,
                         suggest_escape ? " (maybe you should use systemd-escape?)" : "");

        /* Append a suffix if it doesn't have any, but only if this is not a glob, so that we can allow
         * "foo.*" as a valid glob. */
        ಯದಿ ((!FLAGS_SET(flags, UNIT_NAME_MANGLE_GLOB) || !string_is_glob(s)) && unit_name_to_type(s) < 0)
                strcat(s, suffix);

        /* Make sure mangling didn't grow this too large (but don't do this check if globbing is allowed,
         * since globs generally do not qualify as valid unit names) */
        ಯದಿ (!FLAGS_SET(flags, UNIT_NAME_MANGLE_GLOB) && !unit_name_is_valid(s, UNIT_NAME_ANY))
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

        *ret = TAKE_PTR(s);
        ಪ್ರತ್ಯಾಗಮ 1;

good:
        ಪ್ರತ್ಯಾಗಮ strdup_to(ret, name);
}

ಪೂರ್ಣಾಙ್ಕ slice_build_parent_slice(ಧ್ರುವ ವರ್ಣ *slice, ವರ್ಣ **ret) {
        ಪ್ರತಿಜ್ಞಾ(slice);
        ಪ್ರತಿಜ್ಞಾ(ret);

        ಯದಿ (!slice_name_is_valid(slice))
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

        ಯದಿ (streq(slice, SPECIAL_ಬೇರು_SLICE)) {
                *ret = ರಿಕ್ತ;
                ಪ್ರತ್ಯಾಗಮ 0;
        }

        _cleanup_free_ ವರ್ಣ *s = strdup(slice);
        ಯದಿ (!s)
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        ವರ್ಣ *dash = strrchr(s, '-');
        ಯದಿ (!dash)
                ಪ್ರತ್ಯಾಗಮ strdup_to_full(ret, SPECIAL_ಬೇರು_SLICE);

        /* We know that s ended with .slice before truncation, so we have enough space. */
        strcpy(dash, ".slice");

        *ret = TAKE_PTR(s);
        ಪ್ರತ್ಯಾಗಮ 1;
}

ಪೂರ್ಣಾಙ್ಕ slice_build_subslice(ಧ್ರುವ ವರ್ಣ *slice, ಧ್ರುವ ವರ್ಣ *name, ವರ್ಣ **ret) {
        ವರ್ಣ *subslice;

        ಪ್ರತಿಜ್ಞಾ(slice);
        ಪ್ರತಿಜ್ಞಾ(name);
        ಪ್ರತಿಜ್ಞಾ(ret);

        ಯದಿ (!slice_name_is_valid(slice))
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

        ಯದಿ (!unit_prefix_is_valid(name))
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

        ಯದಿ (streq(slice, SPECIAL_ಬೇರು_SLICE))
                subslice = strjoin(name, ".slice");
        ಅನ್ಯಥಾ {
                ಧ್ರುವ ವರ್ಣ *e;

                assert_se(e = endswith(slice, ".slice"));

                subslice = new(ವರ್ಣ, (e - slice) + 1 + strlen(name) + 6 + 1);
                ಯದಿ (!subslice)
                        ಪ್ರತ್ಯಾಗಮ -ENOMEM;

                stpcpy(stpcpy(stpcpy(mempcpy(subslice, slice, e - slice), "-"), name), ".slice");
        }

        *ret = subslice;
        ಪ್ರತ್ಯಾಗಮ 0;
}

bool slice_name_is_valid(ಧ್ರುವ ವರ್ಣ *name) {
        ಧ್ರುವ ವರ್ಣ *p, *e;
        bool dash = false;

        ಯದಿ (!unit_name_is_valid(name, UNIT_NAME_PLAIN))
                ಪ್ರತ್ಯಾಗಮ false;

        ಯದಿ (streq(name, SPECIAL_ಬೇರು_SLICE))
                ಪ್ರತ್ಯಾಗಮ true;

        e = endswith(name, ".slice");
        ಯದಿ (!e)
                ಪ್ರತ್ಯಾಗಮ false;

        ಪ್ರತಿ (p = name; p < e; p++) {

                ಯದಿ (*p == '-') {

                        /* Don't allow initial dash */
                        ಯದಿ (p == name)
                                ಪ್ರತ್ಯಾಗಮ false;

                        /* Don't allow multiple dashes */
                        ಯದಿ (dash)
                                ಪ್ರತ್ಯಾಗಮ false;

                        dash = true;
                } ಅನ್ಯಥಾ
                        dash = false;
        }

        /* Don't allow trailing hash */
        ಯದಿ (dash)
                ಪ್ರತ್ಯಾಗಮ false;

        ಪ್ರತ್ಯಾಗಮ true;
}

bool unit_name_prefix_equal(ಧ್ರುವ ವರ್ಣ *a, ಧ್ರುವ ವರ್ಣ *b) {
        ಧ್ರುವ ವರ್ಣ *p, *q;

        ಪ್ರತಿಜ್ಞಾ(a);
        ಪ್ರತಿಜ್ಞಾ(b);

        ಯದಿ (!unit_name_is_valid(a, UNIT_NAME_ANY) || !unit_name_is_valid(b, UNIT_NAME_ANY))
                ಪ್ರತ್ಯಾಗಮ false;

        p = strchr(a, '@');
        ಯದಿ (!p)
                p = strrchr(a, '.');

        q = strchr(b, '@');
        ಯದಿ (!q)
                q = strrchr(b, '.');

        ಪ್ರತಿಜ್ಞಾ(p);
        ಪ್ರತಿಜ್ಞಾ(q);

        ಪ್ರತ್ಯಾಗಮ memcmp_nn(a, p - a, b, q - b) == 0;
}
