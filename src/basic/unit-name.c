/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাৱ "sd-id128.h"

#অন্তর্ভাৱ "alloc-util.h"
#অন্তর্ভাৱ "glob-util.h"
#অন্তর্ভাৱ "hexdecoct.h"
#অন্তর্ভাৱ "log.h"
#অন্তর্ভাৱ "memory-util.h"
#অন্তর্ভাৱ "path-util.h"
#অন্তর্ভাৱ "sparse-endian.h"
#অন্তর্ভাৱ "special.h"
#অন্তর্ভাৱ "siphash24.h"
#অন্তর্ভাৱ "string-util.h"
#অন্তর্ভাৱ "unit-def.h"
#অন্তর্ভাৱ "unit-name.h"

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

#define LONG_UNIT_NAME_HASH_চাবি SD_ID128_MAKE(ec,f2,37,fb,58,32,4a,32,84,9f,06,9b,0d,21,eb,9a)
#define UNIT_NAME_HASH_LENGTH_CHARS 16

bool unit_name_is_valid(ধ্রুৱ ৱর্ণ *n, UnitNameFlags flags) {
        ধ্রুৱ ৱর্ণ *e, *i, *at;

        প্রতিজ্ঞা((flags & ~(UNIT_NAME_PLAIN|UNIT_NAME_INSTANCE|UNIT_NAME_TEMPLATE)) == 0);

        যদি (_unlikely_(flags == 0))
                ঘূৰাই false;

        যদি (isempty(n))
                ঘূৰাই false;

        যদি (strlen(n) >= UNIT_NAME_MAX)
                ঘূৰাই false;

        e = strrchr(n, '.');
        যদি (!e || e == n)
                ঘূৰাই false;

        যদি (unit_type_from_string(e + 1) < 0)
                ঘূৰাই false;

        বাবে (i = n, at = রিক্ত; i < e; i++) {

                যদি (*i == '@' && !at)
                        at = i;

                যদি (!strchr(VALID_CHARS_WITH_AT, *i))
                        ঘূৰাই false;
        }

        যদি (at == n)
                ঘূৰাই false;

        যদি (flags & UNIT_NAME_PLAIN)
                যদি (!at)
                        ঘূৰাই true;

        যদি (flags & UNIT_NAME_INSTANCE)
                যদি (at && e > at + 1)
                        ঘূৰাই true;

        যদি (flags & UNIT_NAME_TEMPLATE)
                যদি (at && e == at + 1)
                        ঘূৰাই true;

        ঘূৰাই false;
}

bool unit_prefix_is_valid(ধ্রুৱ ৱর্ণ *p) {

        /* We don't allow additional @ in the prefix string */

        যদি (isempty(p))
                ঘূৰাই false;

        ঘূৰাই in_charset(p, VALID_CHARS);
}

bool unit_instance_is_valid(ধ্রুৱ ৱর্ণ *i) {

        /* The max length depends on the length of the string, so we
         * don't really check this here. */

        যদি (isempty(i))
                ঘূৰাই false;

        /* We allow additional @ in the instance string, we do not
         * allow them in the prefix! */

        ঘূৰাই in_charset(i, "@" VALID_CHARS);
}

bool unit_suffix_is_valid(ধ্রুৱ ৱর্ণ *s) {
        যদি (isempty(s))
                ঘূৰাই false;

        যদি (s[0] != '.')
                ঘূৰাই false;

        যদি (unit_type_from_string(s + 1) < 0)
                ঘূৰাই false;

        ঘূৰাই true;
}

পূর্ণাঙ্ক unit_name_to_prefix(ধ্রুৱ ৱর্ণ *n, ৱর্ণ **ret) {
        ধ্রুৱ ৱর্ণ *p;
        ৱর্ণ *s;

        প্রতিজ্ঞা(n);
        প্রতিজ্ঞা(ret);

        যদি (!unit_name_is_valid(n, UNIT_NAME_ANY))
                ঘূৰাই -EINVAL;

        p = strchr(n, '@');
        যদি (!p)
                p = strrchr(n, '.');

        assert_se(p);

        s = strndup(n, p - n);
        যদি (!s)
                ঘূৰাই -ENOMEM;

        *ret = s;
        ঘূৰাই 0;
}

UnitNameFlags unit_name_to_instance(ধ্রুৱ ৱর্ণ *n, ৱর্ণ **ret) {
        ধ্রুৱ ৱর্ণ *p, *d;

        প্রতিজ্ঞা(n);

        যদি (!unit_name_is_valid(n, UNIT_NAME_ANY))
                ঘূৰাই -EINVAL;

        /* Everything past the first @ and before the last . is the instance */
        p = strchr(n, '@');
        যদি (!p) {
                যদি (ret)
                        *ret = রিক্ত;
                ঘূৰাই UNIT_NAME_PLAIN;
        }

        p++;

        d = strrchr(p, '.');
        যদি (!d)
                ঘূৰাই -EINVAL;

        যদি (ret) {
                ৱর্ণ *i = strndup(p, d-p);
                যদি (!i)
                        ঘূৰাই -ENOMEM;

                *ret = i;
        }
        ঘূৰাই d > p ? UNIT_NAME_INSTANCE : UNIT_NAME_TEMPLATE;
}

পূর্ণাঙ্ক unit_name_to_prefix_and_instance(ধ্রুৱ ৱর্ণ *n, ৱর্ণ **ret) {
        ধ্রুৱ ৱর্ণ *d;
        ৱর্ণ *s;

        প্রতিজ্ঞা(n);
        প্রতিজ্ঞা(ret);

        যদি (!unit_name_is_valid(n, UNIT_NAME_ANY))
                ঘূৰাই -EINVAL;

        d = strrchr(n, '.');
        যদি (!d)
                ঘূৰাই -EINVAL;

        s = strndup(n, d - n);
        যদি (!s)
                ঘূৰাই -ENOMEM;

        *ret = s;
        ঘূৰাই 0;
}

UnitType unit_name_to_type(ধ্রুৱ ৱর্ণ *n) {
        ধ্রুৱ ৱর্ণ *e;

        প্রতিজ্ঞা(n);

        যদি (!unit_name_is_valid(n, UNIT_NAME_ANY))
                ঘূৰাই _UNIT_TYPE_INVALID;

        assert_se(e = strrchr(n, '.'));

        ঘূৰাই unit_type_from_string(e + 1);
}

পূর্ণাঙ্ক unit_name_change_suffix(ধ্রুৱ ৱর্ণ *n, ধ্রুৱ ৱর্ণ *suffix, ৱর্ণ **ret) {
        _cleanup_free_ ৱর্ণ *s = রিক্ত;
        size_t a, b;
        ধ্রুৱ ৱর্ণ *e;

        প্রতিজ্ঞা(n);
        প্রতিজ্ঞা(suffix);
        প্রতিজ্ঞা(ret);

        যদি (!unit_name_is_valid(n, UNIT_NAME_ANY))
                ঘূৰাই -EINVAL;

        যদি (!unit_suffix_is_valid(suffix))
                ঘূৰাই -EINVAL;

        assert_se(e = strrchr(n, '.'));

        a = e - n;
        b = strlen(suffix);

        s = new(ৱর্ণ, a + b + 1);
        যদি (!s)
                ঘূৰাই -ENOMEM;

        strcpy(mempcpy(s, n, a), suffix);

        /* Make sure the name is still valid (i.e. didn't grow too large due to longer suffix) */
        যদি (!unit_name_is_valid(s, UNIT_NAME_ANY))
                ঘূৰাই -EINVAL;

        *ret = TAKE_PTR(s);
        ঘূৰাই 0;
}

পূর্ণাঙ্ক unit_name_build(ধ্রুৱ ৱর্ণ *prefix, ধ্রুৱ ৱর্ণ *instance, ধ্রুৱ ৱর্ণ *suffix, ৱর্ণ **ret) {
        UnitType type;

        প্রতিজ্ঞা(prefix);
        প্রতিজ্ঞা(suffix);
        প্রতিজ্ঞা(ret);

        যদি (suffix[0] != '.')
                ঘূৰাই -EINVAL;

        type = unit_type_from_string(suffix + 1);
        যদি (type < 0)
                ঘূৰাই type;

        ঘূৰাই unit_name_build_from_type(prefix, instance, type, ret);
}

পূর্ণাঙ্ক unit_name_build_from_type(ধ্রুৱ ৱর্ণ *prefix, ধ্রুৱ ৱর্ণ *instance, UnitType type, ৱর্ণ **ret) {
        _cleanup_free_ ৱর্ণ *s = রিক্ত;
        ধ্রুৱ ৱর্ণ *ut;

        প্রতিজ্ঞা(prefix);
        প্রতিজ্ঞা(type >= 0);
        প্রতিজ্ঞা(type < _UNIT_TYPE_MAX);
        প্রতিজ্ঞা(ret);

        যদি (!unit_prefix_is_valid(prefix))
                ঘূৰাই -EINVAL;

        ut = unit_type_to_string(type);

        যদি (instance) {
                যদি (!unit_instance_is_valid(instance))
                        ঘূৰাই -EINVAL;

                s = strjoin(prefix, "@", instance, ".", ut);
        } অন্যথা
                s = strjoin(prefix, ".", ut);
        যদি (!s)
                ঘূৰাই -ENOMEM;

        /* Verify that this didn't grow too large (or otherwise is invalid) */
        যদি (!unit_name_is_valid(s, instance ? UNIT_NAME_INSTANCE : UNIT_NAME_PLAIN))
                ঘূৰাই -EINVAL;

        *ret = TAKE_PTR(s);
        ঘূৰাই 0;
}

স্থির ৱর্ণ *do_escape_char(ৱর্ণ c, ৱর্ণ *t) {
        প্রতিজ্ঞা(t);

        *(t++) = '\\';
        *(t++) = 'x';
        *(t++) = hexchar(c >> 4);
        *(t++) = hexchar(c);

        ঘূৰাই t;
}

স্থির ৱর্ণ *do_escape(ধ্রুৱ ৱর্ণ *f, ৱর্ণ *t) {
        প্রতিজ্ঞা(f);
        প্রতিজ্ঞা(t);

        /* do not create units with a leading '.', like for "/.dotdir" mount points */
        যদি (*f == '.') {
                t = do_escape_char(*f, t);
                f++;
        }

        বাবে (; *f; f++) {
                যদি (*f == '/')
                        *(t++) = '-';
                অন্যথা যদি (IN_SET(*f, '-', '\\') || !strchr(VALID_CHARS, *f))
                        t = do_escape_char(*f, t);
                অন্যথা
                        *(t++) = *f;
        }

        ঘূৰাই t;
}

ৱর্ণ* unit_name_escape(ধ্রুৱ ৱর্ণ *f) {
        ৱর্ণ *r, *t;

        প্রতিজ্ঞা(f);

        r = new(ৱর্ণ, strlen(f)*4+1);
        যদি (!r)
                ঘূৰাই রিক্ত;

        t = do_escape(f, r);
        *t = 0;

        ঘূৰাই r;
}

পূর্ণাঙ্ক unit_name_unescape(ধ্রুৱ ৱর্ণ *f, ৱর্ণ **ret) {
        _cleanup_free_ ৱর্ণ *r = রিক্ত;
        ৱর্ণ *t;

        প্রতিজ্ঞা(f);
        প্রতিজ্ঞা(ret);

        r = strdup(f);
        যদি (!r)
                ঘূৰাই -ENOMEM;

        বাবে (t = r; *f; f++) {
                যদি (*f == '-')
                        *(t++) = '/';
                অন্যথা যদি (*f == '\\') {
                        পূর্ণাঙ্ক a, b;

                        যদি (f[1] != 'x')
                                ঘূৰাই -EINVAL;

                        a = unhexchar(f[2]);
                        যদি (a < 0)
                                ঘূৰাই -EINVAL;

                        b = unhexchar(f[3]);
                        যদি (b < 0)
                                ঘূৰাই -EINVAL;

                        *(t++) = (ৱর্ণ) (((uint8_t) a << 4U) | (uint8_t) b);
                        f += 3;
                } অন্যথা
                        *(t++) = *f;
        }

        *t = 0;

        *ret = TAKE_PTR(r);

        ঘূৰাই 0;
}

পূর্ণাঙ্ক unit_name_path_escape(ধ্রুৱ ৱর্ণ *f, ৱর্ণ **ret) {
        _cleanup_free_ ৱর্ণ *p = রিক্ত;
        ৱর্ণ *s;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(f);
        প্রতিজ্ঞা(ret);

        r = path_simplify_alloc(f, &p);
        যদি (r < 0)
                ঘূৰাই r;

        যদি (empty_or_মূল(p))
                s = strdup("-");
        অন্যথা {
                যদি (!path_is_normalized(p))
                        ঘূৰাই -EINVAL;

                /* Truncate trailing slashes and skip leading slashes */
                delete_trailing_chars(p, "/");
                s = unit_name_escape(skip_leading_chars(p, "/"));
        }
        যদি (!s)
                ঘূৰাই -ENOMEM;

        *ret = s;
        ঘূৰাই 0;
}

পূর্ণাঙ্ক unit_name_path_unescape(ধ্রুৱ ৱর্ণ *f, ৱর্ণ **ret) {
        _cleanup_free_ ৱর্ণ *s = রিক্ত;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(f);

        যদি (isempty(f))
                ঘূৰাই -EINVAL;

        যদি (streq(f, "-")) {
                s = strdup("/");
                যদি (!s)
                        ঘূৰাই -ENOMEM;
        } অন্যথা {
                _cleanup_free_ ৱর্ণ *w = রিক্ত;

                r = unit_name_unescape(f, &w);
                যদি (r < 0)
                        ঘূৰাই r;

                /* Don't accept trailing or leading slashes */
                যদি (startswith(w, "/") || endswith(w, "/"))
                        ঘূৰাই -EINVAL;

                /* Prefix a slash again */
                s = strjoin("/", w);
                যদি (!s)
                        ঘূৰাই -ENOMEM;

                যদি (!path_is_normalized(s))
                        ঘূৰাই -EINVAL;
        }

        যদি (ret)
                *ret = TAKE_PTR(s);

        ঘূৰাই 0;
}

পূর্ণাঙ্ক unit_name_replace_instance_full(
                ধ্রুৱ ৱর্ণ *original,
                ধ্রুৱ ৱর্ণ *instance,
                bool accept_glob,
                ৱর্ণ **ret) {

        _cleanup_free_ ৱর্ণ *s = রিক্ত;
        ধ্রুৱ ৱর্ণ *prefix, *suffix;
        size_t pl;

        প্রতিজ্ঞা(original);
        প্রতিজ্ঞা(instance);
        প্রতিজ্ঞা(ret);

        যদি (!unit_name_is_valid(original, UNIT_NAME_INSTANCE|UNIT_NAME_TEMPLATE))
                ঘূৰাই -EINVAL;
        যদি (!unit_instance_is_valid(instance) && !(accept_glob && in_charset(instance, VALID_CHARS_GLOB)))
                ঘূৰাই -EINVAL;

        prefix = ASSERT_PTR(strchr(original, '@'));
        suffix = ASSERT_PTR(strrchr(original, '.'));
        প্রতিজ্ঞা(prefix < suffix);

        pl = prefix - original + 1; /* include '@' */

        s = new(ৱর্ণ, pl + strlen(instance) + strlen(suffix) + 1);
        যদি (!s)
                ঘূৰাই -ENOMEM;

#যদি HAS_FEATURE_MEMORY_SANITIZER
        /* MSan doesn't like stpncpy... See also https://github.com/google/sanitizers/issues/926 */
        memzero(s, pl + strlen(instance) + strlen(suffix) + 1);
#endif

        strcpy(stpcpy(stpncpy(s, original, pl), instance), suffix);

        /* Make sure the resulting name still is valid, i.e. didn't grow too large. Globs will be expanded
         * by clients when used, so the check is pointless. */
        যদি (!accept_glob && !unit_name_is_valid(s, UNIT_NAME_INSTANCE))
                ঘূৰাই -EINVAL;

        *ret = TAKE_PTR(s);
        ঘূৰাই 0;
}

পূর্ণাঙ্ক unit_name_template(ধ্রুৱ ৱর্ণ *f, ৱর্ণ **ret) {
        ধ্রুৱ ৱর্ণ *p, *e;
        ৱর্ণ *s;
        size_t a;

        প্রতিজ্ঞা(f);
        প্রতিজ্ঞা(ret);

        যদি (!unit_name_is_valid(f, UNIT_NAME_INSTANCE|UNIT_NAME_TEMPLATE))
                ঘূৰাই -EINVAL;

        assert_se(p = strchr(f, '@'));
        assert_se(e = strrchr(f, '.'));

        a = p - f;

        s = new(ৱর্ণ, a + 1 + strlen(e) + 1);
        যদি (!s)
                ঘূৰাই -ENOMEM;

        strcpy(mempcpy(s, f, a + 1), e);

        *ret = s;
        ঘূৰাই 0;
}

bool unit_name_is_hashed(ধ্রুৱ ৱর্ণ *name) {
        ধ্রুৱ ৱর্ণ *s;

        যদি (!unit_name_is_valid(name, UNIT_NAME_PLAIN))
                ঘূৰাই false;

        assert_se(s = strrchr(name, '.'));

        যদি (s - name < UNIT_NAME_HASH_LENGTH_CHARS + 1)
                ঘূৰাই false;

        s -= UNIT_NAME_HASH_LENGTH_CHARS;
        যদি (s[-1] != '_')
                ঘূৰাই false;

        বাবে (size_t i = 0; i < UNIT_NAME_HASH_LENGTH_CHARS; i++)
                যদি (!strchr(LOWERCASE_HEXDIGITS, s[i]))
                        ঘূৰাই false;

        ঘূৰাই true;
}

পূর্ণাঙ্ক unit_name_hash_long(ধ্রুৱ ৱর্ণ *name, ৱর্ণ **ret) {
        _cleanup_free_ ৱর্ণ *n = রিক্ত, *hash = রিক্ত;
        ধ্রুৱ ৱর্ণ *suffix;
        le64_t h;
        size_t len;

        প্রতিজ্ঞা(ret);

        যদি (strlen(name) < UNIT_NAME_MAX)
                ঘূৰাই -EMSGSIZE;

        suffix = strrchr(name, '.');
        যদি (!suffix)
                ঘূৰাই -EINVAL;

        যদি (unit_type_from_string(suffix+1) < 0)
                ঘূৰাই -EINVAL;

        h = htole64(siphash24_string(name, LONG_UNIT_NAME_HASH_চাবি.bytes));

        hash = hexmem(&h, পরিমাণ(h));
        যদি (!hash)
                ঘূৰাই -ENOMEM;

        assert_se(strlen(hash) == UNIT_NAME_HASH_LENGTH_CHARS);

        len = UNIT_NAME_MAX - 1 - strlen(suffix+1) - UNIT_NAME_HASH_LENGTH_CHARS - 2;
        প্রতিজ্ঞা(len > 0 && len < UNIT_NAME_MAX);

        n = strndup(name, len);
        যদি (!n)
                ঘূৰাই -ENOMEM;

        যদি (!strextend(&n, "_", hash, suffix))
                ঘূৰাই -ENOMEM;
        assert_se(unit_name_is_valid(n, UNIT_NAME_PLAIN));

        *ret = TAKE_PTR(n);

        ঘূৰাই 0;
}

পূর্ণাঙ্ক unit_name_from_path(ধ্রুৱ ৱর্ণ *path, ধ্রুৱ ৱর্ণ *suffix, ৱর্ণ **ret) {
        _cleanup_free_ ৱর্ণ *p = রিক্ত, *s = রিক্ত;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(path);
        প্রতিজ্ঞা(suffix);
        প্রতিজ্ঞা(ret);

        যদি (!unit_suffix_is_valid(suffix))
                ঘূৰাই -EINVAL;

        r = unit_name_path_escape(path, &p);
        যদি (r < 0)
                ঘূৰাই r;

        s = strjoin(p, suffix);
        যদি (!s)
                ঘূৰাই -ENOMEM;

        যদি (strlen(s) >= UNIT_NAME_MAX) {
                _cleanup_free_ ৱর্ণ *n = রিক্ত;

                log_debug("Unit name \"%s\" too long, falling back to hashed unit name.", s);

                r = unit_name_hash_long(s, &n);
                যদি (r < 0)
                        ঘূৰাই r;

                free_and_replace(s, n);
        }

        /* Refuse if this for some other reason didn't result in a valid name */
        যদি (!unit_name_is_valid(s, UNIT_NAME_PLAIN))
                ঘূৰাই -EINVAL;

        *ret = TAKE_PTR(s);
        ঘূৰাই 0;
}

পূর্ণাঙ্ক unit_name_from_path_instance(ধ্রুৱ ৱর্ণ *prefix, ধ্রুৱ ৱর্ণ *path, ধ্রুৱ ৱর্ণ *suffix, ৱর্ণ **ret) {
        _cleanup_free_ ৱর্ণ *p = রিক্ত, *s = রিক্ত;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(prefix);
        প্রতিজ্ঞা(path);
        প্রতিজ্ঞা(suffix);
        প্রতিজ্ঞা(ret);

        যদি (!unit_prefix_is_valid(prefix))
                ঘূৰাই -EINVAL;

        যদি (!unit_suffix_is_valid(suffix))
                ঘূৰাই -EINVAL;

        r = unit_name_path_escape(path, &p);
        যদি (r < 0)
                ঘূৰাই r;

        s = strjoin(prefix, "@", p, suffix);
        যদি (!s)
                ঘূৰাই -ENOMEM;

        যদি (strlen(s) >= UNIT_NAME_MAX) /* Return a slightly more descriptive error for this specific condition */
                ঘূৰাই -ENAMETOOLONG;

        /* Refuse if this for some other reason didn't result in a valid name */
        যদি (!unit_name_is_valid(s, UNIT_NAME_INSTANCE))
                ঘূৰাই -EINVAL;

        *ret = TAKE_PTR(s);
        ঘূৰাই 0;
}

পূর্ণাঙ্ক unit_name_to_path(ধ্রুৱ ৱর্ণ *name, ৱর্ণ **ret) {
        _cleanup_free_ ৱর্ণ *prefix = রিক্ত;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(name);

        r = unit_name_to_prefix(name, &prefix);
        যদি (r < 0)
                ঘূৰাই r;

        যদি (unit_name_is_hashed(name))
                ঘূৰাই -ENAMETOOLONG;

        ঘূৰাই unit_name_path_unescape(prefix, ret);
}

স্থির bool do_escape_mangle(ধ্রুৱ ৱর্ণ *f, bool allow_globs, ৱর্ণ *t) {
        ধ্রুৱ ৱর্ণ *valid_chars;
        bool mangled = false;

        প্রতিজ্ঞা(f);
        প্রতিজ্ঞা(t);

        /* We'll only escape the obvious characters here, to play safe.
         *
         * Returns true if any characters were mangled, false otherwise.
         */

        valid_chars = allow_globs ? VALID_CHARS_GLOB : VALID_CHARS_WITH_AT;

        বাবে (; *f; f++)
                যদি (*f == '/') {
                        *(t++) = '-';
                        mangled = true;
                } অন্যথা যদি (!strchr(valid_chars, *f)) {
                        t = do_escape_char(*f, t);
                        mangled = true;
                } অন্যথা
                        *(t++) = *f;
        *t = 0;

        ঘূৰাই mangled;
}

/**
 *  Convert a string to a unit name. /dev/blah is converted to dev-blah.device,
 *  /blah/blah is converted to blah-blah.mount, anything else is left alone,
 *  except that @suffix is appended if a valid unit suffix is not present.
 *
 *  If @allow_globs, globs characters are preserved. Otherwise, they are escaped.
 */
পূর্ণাঙ্ক unit_name_mangle_with_suffix(
                ধ্রুৱ ৱর্ণ *name,
                ধ্রুৱ ৱর্ণ *operation,
                UnitNameMangle flags,
                ধ্রুৱ ৱর্ণ *suffix,
                ৱর্ণ **ret) {

        _cleanup_free_ ৱর্ণ *s = রিক্ত;
        bool mangled, suggest_escape = true, warn = FLAGS_SET(flags, UNIT_NAME_MANGLE_WARN);
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(name);
        প্রতিজ্ঞা(suffix);
        প্রতিজ্ঞা(ret);

        যদি (isempty(name)) /* We cannot mangle empty unit names to become valid, sorry. */
                ঘূৰাই -EINVAL;

        যদি (!unit_suffix_is_valid(suffix))
                ঘূৰাই -EINVAL;

        /* Already a fully valid unit name? If so, no mangling is necessary... */
        যদি (unit_name_is_valid(name, UNIT_NAME_ANY)) {
                যদি (FLAGS_SET(flags, UNIT_NAME_MANGLE_STRICT) && !endswith(name, suffix)) {
                        ধ্রুৱ ৱর্ণ *e = ASSERT_PTR(strrchr(name, '.'));

                        ঘূৰাই log_full_errno(warn ? LOG_NOTICE : LOG_DEBUG,
                                              SYNTHETIC_ERRNO(EINVAL),
                                              "Unit name \"%s\" has unit type \"%s\", but \"%s\" is expected%s%s.",
                                              name, e + 1, suffix + 1,
                                              operation ? " " : "", strempty(operation));
                }

                গচ্ছ good;
        }

        /* Already a fully valid globbing expression? If so, no mangling is necessary either... */
        যদি (string_is_glob(name) && in_charset(name, VALID_CHARS_GLOB)) {
                যদি (FLAGS_SET(flags, UNIT_NAME_MANGLE_GLOB))
                        গচ্ছ good;
                log_full(warn ? LOG_NOTICE : LOG_DEBUG,
                         "Glob pattern passed%s%s, but globs are not supported for this.",
                         operation ? " " : "", strempty(operation));
                suggest_escape = false;
        }

        যদি (path_is_absolute(name)) {
                _cleanup_free_ ৱর্ণ *n = রিক্ত, *u = রিক্ত;

                r = path_simplify_alloc(name, &n);
                যদি (r < 0)
                        ঘূৰাই r;

                যদি (is_device_path(n)) {
                        r = unit_name_from_path(n, ".device", &u);
                        যদি (r >= 0) {
                                যদি (FLAGS_SET(flags, UNIT_NAME_MANGLE_STRICT) && !streq(suffix, ".device"))
                                        ঘূৰাই log_full_errno(warn ? LOG_NOTICE : LOG_DEBUG,
                                                              SYNTHETIC_ERRNO(EINVAL),
                                                              "Path \"%s\" resolves to unit type \"device\", but \"%s\" is expected%s%s.",
                                                              name, suffix + 1,
                                                              operation ? " " : "", strempty(operation));

                                *ret = TAKE_PTR(u);
                                ঘূৰাই 1;
                        }
                        যদি (r != -EINVAL)
                                ঘূৰাই r;
                }

                r = unit_name_from_path(n, ".mount", &u);
                যদি (r >= 0) {
                        যদি (FLAGS_SET(flags, UNIT_NAME_MANGLE_STRICT) && !streq(suffix, ".mount"))
                                ঘূৰাই log_full_errno(warn ? LOG_NOTICE : LOG_DEBUG,
                                                      SYNTHETIC_ERRNO(EINVAL),
                                                      "Path \"%s\" resolves to unit type \"mount\", but \"%s\" is expected%s%s.",
                                                      name, suffix + 1,
                                                      operation ? " " : "", strempty(operation));

                        *ret = TAKE_PTR(u);
                        ঘূৰাই 1;
                }
                যদি (r != -EINVAL)
                        ঘূৰাই r;
        }

        s = new(ৱর্ণ, strlen(name) * 4 + strlen(suffix) + 1);
        যদি (!s)
                ঘূৰাই -ENOMEM;

        mangled = do_escape_mangle(name, FLAGS_SET(flags, UNIT_NAME_MANGLE_GLOB), s);
        যদি (mangled)
                log_full(warn ? LOG_NOTICE : LOG_DEBUG,
                         "Invalid unit name \"%s\" escaped as \"%s\"%s.",
                         name, s,
                         suggest_escape ? " (maybe you should use systemd-escape?)" : "");

        /* Append a suffix if it doesn't have any, but only if this is not a glob, so that we can allow
         * "foo.*" as a valid glob. */
        যদি ((!FLAGS_SET(flags, UNIT_NAME_MANGLE_GLOB) || !string_is_glob(s)) && unit_name_to_type(s) < 0)
                strcat(s, suffix);

        /* Make sure mangling didn't grow this too large (but don't do this check if globbing is allowed,
         * since globs generally do not qualify as valid unit names) */
        যদি (!FLAGS_SET(flags, UNIT_NAME_MANGLE_GLOB) && !unit_name_is_valid(s, UNIT_NAME_ANY))
                ঘূৰাই -EINVAL;

        *ret = TAKE_PTR(s);
        ঘূৰাই 1;

good:
        ঘূৰাই strdup_to(ret, name);
}

পূর্ণাঙ্ক slice_build_parent_slice(ধ্রুৱ ৱর্ণ *slice, ৱর্ণ **ret) {
        প্রতিজ্ঞা(slice);
        প্রতিজ্ঞা(ret);

        যদি (!slice_name_is_valid(slice))
                ঘূৰাই -EINVAL;

        যদি (streq(slice, SPECIAL_মূল_SLICE)) {
                *ret = রিক্ত;
                ঘূৰাই 0;
        }

        _cleanup_free_ ৱর্ণ *s = strdup(slice);
        যদি (!s)
                ঘূৰাই -ENOMEM;

        ৱর্ণ *dash = strrchr(s, '-');
        যদি (!dash)
                ঘূৰাই strdup_to_full(ret, SPECIAL_মূল_SLICE);

        /* We know that s ended with .slice before truncation, so we have enough space. */
        strcpy(dash, ".slice");

        *ret = TAKE_PTR(s);
        ঘূৰাই 1;
}

পূর্ণাঙ্ক slice_build_subslice(ধ্রুৱ ৱর্ণ *slice, ধ্রুৱ ৱর্ণ *name, ৱর্ণ **ret) {
        ৱর্ণ *subslice;

        প্রতিজ্ঞা(slice);
        প্রতিজ্ঞা(name);
        প্রতিজ্ঞা(ret);

        যদি (!slice_name_is_valid(slice))
                ঘূৰাই -EINVAL;

        যদি (!unit_prefix_is_valid(name))
                ঘূৰাই -EINVAL;

        যদি (streq(slice, SPECIAL_মূল_SLICE))
                subslice = strjoin(name, ".slice");
        অন্যথা {
                ধ্রুৱ ৱর্ণ *e;

                assert_se(e = endswith(slice, ".slice"));

                subslice = new(ৱর্ণ, (e - slice) + 1 + strlen(name) + 6 + 1);
                যদি (!subslice)
                        ঘূৰাই -ENOMEM;

                stpcpy(stpcpy(stpcpy(mempcpy(subslice, slice, e - slice), "-"), name), ".slice");
        }

        *ret = subslice;
        ঘূৰাই 0;
}

bool slice_name_is_valid(ধ্রুৱ ৱর্ণ *name) {
        ধ্রুৱ ৱর্ণ *p, *e;
        bool dash = false;

        যদি (!unit_name_is_valid(name, UNIT_NAME_PLAIN))
                ঘূৰাই false;

        যদি (streq(name, SPECIAL_মূল_SLICE))
                ঘূৰাই true;

        e = endswith(name, ".slice");
        যদি (!e)
                ঘূৰাই false;

        বাবে (p = name; p < e; p++) {

                যদি (*p == '-') {

                        /* Don't allow initial dash */
                        যদি (p == name)
                                ঘূৰাই false;

                        /* Don't allow multiple dashes */
                        যদি (dash)
                                ঘূৰাই false;

                        dash = true;
                } অন্যথা
                        dash = false;
        }

        /* Don't allow trailing hash */
        যদি (dash)
                ঘূৰাই false;

        ঘূৰাই true;
}

bool unit_name_prefix_equal(ধ্রুৱ ৱর্ণ *a, ধ্রুৱ ৱর্ণ *b) {
        ধ্রুৱ ৱর্ণ *p, *q;

        প্রতিজ্ঞা(a);
        প্রতিজ্ঞা(b);

        যদি (!unit_name_is_valid(a, UNIT_NAME_ANY) || !unit_name_is_valid(b, UNIT_NAME_ANY))
                ঘূৰাই false;

        p = strchr(a, '@');
        যদি (!p)
                p = strrchr(a, '.');

        q = strchr(b, '@');
        যদি (!q)
                q = strrchr(b, '.');

        প্রতিজ্ঞা(p);
        প্রতিজ্ঞা(q);

        ঘূৰাই memcmp_nn(a, p - a, b, q - b) == 0;
}
