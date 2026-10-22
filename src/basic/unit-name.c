/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাব "sd-id128.h"

#অন্তর্ভাব "alloc-util.h"
#অন্তর্ভাব "glob-util.h"
#অন্তর্ভাব "hexdecoct.h"
#অন্তর্ভাব "log.h"
#অন্তর্ভাব "memory-util.h"
#অন্তর্ভাব "path-util.h"
#অন্তর্ভাব "sparse-endian.h"
#অন্তর্ভাব "special.h"
#অন্তর্ভাব "siphash24.h"
#অন্তর্ভাব "string-util.h"
#অন্তর্ভাব "unit-def.h"
#অন্তর্ভাব "unit-name.h"

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

bool unit_name_is_valid(ধ্রুব বর্ণ *n, UnitNameFlags flags) {
        ধ্রুব বর্ণ *e, *i, *at;

        প্রতিজ্ঞা((flags & ~(UNIT_NAME_PLAIN|UNIT_NAME_INSTANCE|UNIT_NAME_TEMPLATE)) == 0);

        যদি (_unlikely_(flags == 0))
                প্রত্যাগম false;

        যদি (isempty(n))
                প্রত্যাগম false;

        যদি (strlen(n) >= UNIT_NAME_MAX)
                প্রত্যাগম false;

        e = strrchr(n, '.');
        যদি (!e || e == n)
                প্রত্যাগম false;

        যদি (unit_type_from_string(e + 1) < 0)
                প্রত্যাগম false;

        প্রতি (i = n, at = রিক্ত; i < e; i++) {

                যদি (*i == '@' && !at)
                        at = i;

                যদি (!strchr(VALID_CHARS_WITH_AT, *i))
                        প্রত্যাগম false;
        }

        যদি (at == n)
                প্রত্যাগম false;

        যদি (flags & UNIT_NAME_PLAIN)
                যদি (!at)
                        প্রত্যাগম true;

        যদি (flags & UNIT_NAME_INSTANCE)
                যদি (at && e > at + 1)
                        প্রত্যাগম true;

        যদি (flags & UNIT_NAME_TEMPLATE)
                যদি (at && e == at + 1)
                        প্রত্যাগম true;

        প্রত্যাগম false;
}

bool unit_prefix_is_valid(ধ্রুব বর্ণ *p) {

        /* We don't allow additional @ in the prefix string */

        যদি (isempty(p))
                প্রত্যাগম false;

        প্রত্যাগম in_charset(p, VALID_CHARS);
}

bool unit_instance_is_valid(ধ্রুব বর্ণ *i) {

        /* The max length depends on the length of the string, so we
         * don't really check this here. */

        যদি (isempty(i))
                প্রত্যাগম false;

        /* We allow additional @ in the instance string, we do not
         * allow them in the prefix! */

        প্রত্যাগম in_charset(i, "@" VALID_CHARS);
}

bool unit_suffix_is_valid(ধ্রুব বর্ণ *s) {
        যদি (isempty(s))
                প্রত্যাগম false;

        যদি (s[0] != '.')
                প্রত্যাগম false;

        যদি (unit_type_from_string(s + 1) < 0)
                প্রত্যাগম false;

        প্রত্যাগম true;
}

পূর্ণাঙ্ক unit_name_to_prefix(ধ্রুব বর্ণ *n, বর্ণ **ret) {
        ধ্রুব বর্ণ *p;
        বর্ণ *s;

        প্রতিজ্ঞা(n);
        প্রতিজ্ঞা(ret);

        যদি (!unit_name_is_valid(n, UNIT_NAME_ANY))
                প্রত্যাগম -EINVAL;

        p = strchr(n, '@');
        যদি (!p)
                p = strrchr(n, '.');

        assert_se(p);

        s = strndup(n, p - n);
        যদি (!s)
                প্রত্যাগম -ENOMEM;

        *ret = s;
        প্রত্যাগম 0;
}

UnitNameFlags unit_name_to_instance(ধ্রুব বর্ণ *n, বর্ণ **ret) {
        ধ্রুব বর্ণ *p, *d;

        প্রতিজ্ঞা(n);

        যদি (!unit_name_is_valid(n, UNIT_NAME_ANY))
                প্রত্যাগম -EINVAL;

        /* Everything past the first @ and before the last . is the instance */
        p = strchr(n, '@');
        যদি (!p) {
                যদি (ret)
                        *ret = রিক্ত;
                প্রত্যাগম UNIT_NAME_PLAIN;
        }

        p++;

        d = strrchr(p, '.');
        যদি (!d)
                প্রত্যাগম -EINVAL;

        যদি (ret) {
                বর্ণ *i = strndup(p, d-p);
                যদি (!i)
                        প্রত্যাগম -ENOMEM;

                *ret = i;
        }
        প্রত্যাগম d > p ? UNIT_NAME_INSTANCE : UNIT_NAME_TEMPLATE;
}

পূর্ণাঙ্ক unit_name_to_prefix_and_instance(ধ্রুব বর্ণ *n, বর্ণ **ret) {
        ধ্রুব বর্ণ *d;
        বর্ণ *s;

        প্রতিজ্ঞা(n);
        প্রতিজ্ঞা(ret);

        যদি (!unit_name_is_valid(n, UNIT_NAME_ANY))
                প্রত্যাগম -EINVAL;

        d = strrchr(n, '.');
        যদি (!d)
                প্রত্যাগম -EINVAL;

        s = strndup(n, d - n);
        যদি (!s)
                প্রত্যাগম -ENOMEM;

        *ret = s;
        প্রত্যাগম 0;
}

UnitType unit_name_to_type(ধ্রুব বর্ণ *n) {
        ধ্রুব বর্ণ *e;

        প্রতিজ্ঞা(n);

        যদি (!unit_name_is_valid(n, UNIT_NAME_ANY))
                প্রত্যাগম _UNIT_TYPE_INVALID;

        assert_se(e = strrchr(n, '.'));

        প্রত্যাগম unit_type_from_string(e + 1);
}

পূর্ণাঙ্ক unit_name_change_suffix(ধ্রুব বর্ণ *n, ধ্রুব বর্ণ *suffix, বর্ণ **ret) {
        _cleanup_free_ বর্ণ *s = রিক্ত;
        size_t a, b;
        ধ্রুব বর্ণ *e;

        প্রতিজ্ঞা(n);
        প্রতিজ্ঞা(suffix);
        প্রতিজ্ঞা(ret);

        যদি (!unit_name_is_valid(n, UNIT_NAME_ANY))
                প্রত্যাগম -EINVAL;

        যদি (!unit_suffix_is_valid(suffix))
                প্রত্যাগম -EINVAL;

        assert_se(e = strrchr(n, '.'));

        a = e - n;
        b = strlen(suffix);

        s = new(বর্ণ, a + b + 1);
        যদি (!s)
                প্রত্যাগম -ENOMEM;

        strcpy(mempcpy(s, n, a), suffix);

        /* Make sure the name is still valid (i.e. didn't grow too large due to longer suffix) */
        যদি (!unit_name_is_valid(s, UNIT_NAME_ANY))
                প্রত্যাগম -EINVAL;

        *ret = TAKE_PTR(s);
        প্রত্যাগম 0;
}

পূর্ণাঙ্ক unit_name_build(ধ্রুব বর্ণ *prefix, ধ্রুব বর্ণ *instance, ধ্রুব বর্ণ *suffix, বর্ণ **ret) {
        UnitType type;

        প্রতিজ্ঞা(prefix);
        প্রতিজ্ঞা(suffix);
        প্রতিজ্ঞা(ret);

        যদি (suffix[0] != '.')
                প্রত্যাগম -EINVAL;

        type = unit_type_from_string(suffix + 1);
        যদি (type < 0)
                প্রত্যাগম type;

        প্রত্যাগম unit_name_build_from_type(prefix, instance, type, ret);
}

পূর্ণাঙ্ক unit_name_build_from_type(ধ্রুব বর্ণ *prefix, ধ্রুব বর্ণ *instance, UnitType type, বর্ণ **ret) {
        _cleanup_free_ বর্ণ *s = রিক্ত;
        ধ্রুব বর্ণ *ut;

        প্রতিজ্ঞা(prefix);
        প্রতিজ্ঞা(type >= 0);
        প্রতিজ্ঞা(type < _UNIT_TYPE_MAX);
        প্রতিজ্ঞা(ret);

        যদি (!unit_prefix_is_valid(prefix))
                প্রত্যাগম -EINVAL;

        ut = unit_type_to_string(type);

        যদি (instance) {
                যদি (!unit_instance_is_valid(instance))
                        প্রত্যাগম -EINVAL;

                s = strjoin(prefix, "@", instance, ".", ut);
        } অন্যথা
                s = strjoin(prefix, ".", ut);
        যদি (!s)
                প্রত্যাগম -ENOMEM;

        /* Verify that this didn't grow too large (or otherwise is invalid) */
        যদি (!unit_name_is_valid(s, instance ? UNIT_NAME_INSTANCE : UNIT_NAME_PLAIN))
                প্রত্যাগম -EINVAL;

        *ret = TAKE_PTR(s);
        প্রত্যাগম 0;
}

স্থির বর্ণ *do_escape_char(বর্ণ c, বর্ণ *t) {
        প্রতিজ্ঞা(t);

        *(t++) = '\\';
        *(t++) = 'x';
        *(t++) = hexchar(c >> 4);
        *(t++) = hexchar(c);

        প্রত্যাগম t;
}

স্থির বর্ণ *do_escape(ধ্রুব বর্ণ *f, বর্ণ *t) {
        প্রতিজ্ঞা(f);
        প্রতিজ্ঞা(t);

        /* do not create units with a leading '.', like for "/.dotdir" mount points */
        যদি (*f == '.') {
                t = do_escape_char(*f, t);
                f++;
        }

        প্রতি (; *f; f++) {
                যদি (*f == '/')
                        *(t++) = '-';
                অন্যথা যদি (IN_SET(*f, '-', '\\') || !strchr(VALID_CHARS, *f))
                        t = do_escape_char(*f, t);
                অন্যথা
                        *(t++) = *f;
        }

        প্রত্যাগম t;
}

বর্ণ* unit_name_escape(ধ্রুব বর্ণ *f) {
        বর্ণ *r, *t;

        প্রতিজ্ঞা(f);

        r = new(বর্ণ, strlen(f)*4+1);
        যদি (!r)
                প্রত্যাগম রিক্ত;

        t = do_escape(f, r);
        *t = 0;

        প্রত্যাগম r;
}

পূর্ণাঙ্ক unit_name_unescape(ধ্রুব বর্ণ *f, বর্ণ **ret) {
        _cleanup_free_ বর্ণ *r = রিক্ত;
        বর্ণ *t;

        প্রতিজ্ঞা(f);
        প্রতিজ্ঞা(ret);

        r = strdup(f);
        যদি (!r)
                প্রত্যাগম -ENOMEM;

        প্রতি (t = r; *f; f++) {
                যদি (*f == '-')
                        *(t++) = '/';
                অন্যথা যদি (*f == '\\') {
                        পূর্ণাঙ্ক a, b;

                        যদি (f[1] != 'x')
                                প্রত্যাগম -EINVAL;

                        a = unhexchar(f[2]);
                        যদি (a < 0)
                                প্রত্যাগম -EINVAL;

                        b = unhexchar(f[3]);
                        যদি (b < 0)
                                প্রত্যাগম -EINVAL;

                        *(t++) = (বর্ণ) (((uint8_t) a << 4U) | (uint8_t) b);
                        f += 3;
                } অন্যথা
                        *(t++) = *f;
        }

        *t = 0;

        *ret = TAKE_PTR(r);

        প্রত্যাগম 0;
}

পূর্ণাঙ্ক unit_name_path_escape(ধ্রুব বর্ণ *f, বর্ণ **ret) {
        _cleanup_free_ বর্ণ *p = রিক্ত;
        বর্ণ *s;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(f);
        প্রতিজ্ঞা(ret);

        r = path_simplify_alloc(f, &p);
        যদি (r < 0)
                প্রত্যাগম r;

        যদি (empty_or_মূল(p))
                s = strdup("-");
        অন্যথা {
                যদি (!path_is_normalized(p))
                        প্রত্যাগম -EINVAL;

                /* Truncate trailing slashes and skip leading slashes */
                delete_trailing_chars(p, "/");
                s = unit_name_escape(skip_leading_chars(p, "/"));
        }
        যদি (!s)
                প্রত্যাগম -ENOMEM;

        *ret = s;
        প্রত্যাগম 0;
}

পূর্ণাঙ্ক unit_name_path_unescape(ধ্রুব বর্ণ *f, বর্ণ **ret) {
        _cleanup_free_ বর্ণ *s = রিক্ত;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(f);

        যদি (isempty(f))
                প্রত্যাগম -EINVAL;

        যদি (streq(f, "-")) {
                s = strdup("/");
                যদি (!s)
                        প্রত্যাগম -ENOMEM;
        } অন্যথা {
                _cleanup_free_ বর্ণ *w = রিক্ত;

                r = unit_name_unescape(f, &w);
                যদি (r < 0)
                        প্রত্যাগম r;

                /* Don't accept trailing or leading slashes */
                যদি (startswith(w, "/") || endswith(w, "/"))
                        প্রত্যাগম -EINVAL;

                /* Prefix a slash again */
                s = strjoin("/", w);
                যদি (!s)
                        প্রত্যাগম -ENOMEM;

                যদি (!path_is_normalized(s))
                        প্রত্যাগম -EINVAL;
        }

        যদি (ret)
                *ret = TAKE_PTR(s);

        প্রত্যাগম 0;
}

পূর্ণাঙ্ক unit_name_replace_instance_full(
                ধ্রুব বর্ণ *original,
                ধ্রুব বর্ণ *instance,
                bool accept_glob,
                বর্ণ **ret) {

        _cleanup_free_ বর্ণ *s = রিক্ত;
        ধ্রুব বর্ণ *prefix, *suffix;
        size_t pl;

        প্রতিজ্ঞা(original);
        প্রতিজ্ঞা(instance);
        প্রতিজ্ঞা(ret);

        যদি (!unit_name_is_valid(original, UNIT_NAME_INSTANCE|UNIT_NAME_TEMPLATE))
                প্রত্যাগম -EINVAL;
        যদি (!unit_instance_is_valid(instance) && !(accept_glob && in_charset(instance, VALID_CHARS_GLOB)))
                প্রত্যাগম -EINVAL;

        prefix = ASSERT_PTR(strchr(original, '@'));
        suffix = ASSERT_PTR(strrchr(original, '.'));
        প্রতিজ্ঞা(prefix < suffix);

        pl = prefix - original + 1; /* include '@' */

        s = new(বর্ণ, pl + strlen(instance) + strlen(suffix) + 1);
        যদি (!s)
                প্রত্যাগম -ENOMEM;

#যদি HAS_FEATURE_MEMORY_SANITIZER
        /* MSan doesn't like stpncpy... See also https://github.com/google/sanitizers/issues/926 */
        memzero(s, pl + strlen(instance) + strlen(suffix) + 1);
#endif

        strcpy(stpcpy(stpncpy(s, original, pl), instance), suffix);

        /* Make sure the resulting name still is valid, i.e. didn't grow too large. Globs will be expanded
         * by clients when used, so the check is pointless. */
        যদি (!accept_glob && !unit_name_is_valid(s, UNIT_NAME_INSTANCE))
                প্রত্যাগম -EINVAL;

        *ret = TAKE_PTR(s);
        প্রত্যাগম 0;
}

পূর্ণাঙ্ক unit_name_template(ধ্রুব বর্ণ *f, বর্ণ **ret) {
        ধ্রুব বর্ণ *p, *e;
        বর্ণ *s;
        size_t a;

        প্রতিজ্ঞা(f);
        প্রতিজ্ঞা(ret);

        যদি (!unit_name_is_valid(f, UNIT_NAME_INSTANCE|UNIT_NAME_TEMPLATE))
                প্রত্যাগম -EINVAL;

        assert_se(p = strchr(f, '@'));
        assert_se(e = strrchr(f, '.'));

        a = p - f;

        s = new(বর্ণ, a + 1 + strlen(e) + 1);
        যদি (!s)
                প্রত্যাগম -ENOMEM;

        strcpy(mempcpy(s, f, a + 1), e);

        *ret = s;
        প্রত্যাগম 0;
}

bool unit_name_is_hashed(ধ্রুব বর্ণ *name) {
        ধ্রুব বর্ণ *s;

        যদি (!unit_name_is_valid(name, UNIT_NAME_PLAIN))
                প্রত্যাগম false;

        assert_se(s = strrchr(name, '.'));

        যদি (s - name < UNIT_NAME_HASH_LENGTH_CHARS + 1)
                প্রত্যাগম false;

        s -= UNIT_NAME_HASH_LENGTH_CHARS;
        যদি (s[-1] != '_')
                প্রত্যাগম false;

        প্রতি (size_t i = 0; i < UNIT_NAME_HASH_LENGTH_CHARS; i++)
                যদি (!strchr(LOWERCASE_HEXDIGITS, s[i]))
                        প্রত্যাগম false;

        প্রত্যাগম true;
}

পূর্ণাঙ্ক unit_name_hash_long(ধ্রুব বর্ণ *name, বর্ণ **ret) {
        _cleanup_free_ বর্ণ *n = রিক্ত, *hash = রিক্ত;
        ধ্রুব বর্ণ *suffix;
        le64_t h;
        size_t len;

        প্রতিজ্ঞা(ret);

        যদি (strlen(name) < UNIT_NAME_MAX)
                প্রত্যাগম -EMSGSIZE;

        suffix = strrchr(name, '.');
        যদি (!suffix)
                প্রত্যাগম -EINVAL;

        যদি (unit_type_from_string(suffix+1) < 0)
                প্রত্যাগম -EINVAL;

        h = htole64(siphash24_string(name, LONG_UNIT_NAME_HASH_চাবি.bytes));

        hash = hexmem(&h, পরিমাণ(h));
        যদি (!hash)
                প্রত্যাগম -ENOMEM;

        assert_se(strlen(hash) == UNIT_NAME_HASH_LENGTH_CHARS);

        len = UNIT_NAME_MAX - 1 - strlen(suffix+1) - UNIT_NAME_HASH_LENGTH_CHARS - 2;
        প্রতিজ্ঞা(len > 0 && len < UNIT_NAME_MAX);

        n = strndup(name, len);
        যদি (!n)
                প্রত্যাগম -ENOMEM;

        যদি (!strextend(&n, "_", hash, suffix))
                প্রত্যাগম -ENOMEM;
        assert_se(unit_name_is_valid(n, UNIT_NAME_PLAIN));

        *ret = TAKE_PTR(n);

        প্রত্যাগম 0;
}

পূর্ণাঙ্ক unit_name_from_path(ধ্রুব বর্ণ *path, ধ্রুব বর্ণ *suffix, বর্ণ **ret) {
        _cleanup_free_ বর্ণ *p = রিক্ত, *s = রিক্ত;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(path);
        প্রতিজ্ঞা(suffix);
        প্রতিজ্ঞা(ret);

        যদি (!unit_suffix_is_valid(suffix))
                প্রত্যাগম -EINVAL;

        r = unit_name_path_escape(path, &p);
        যদি (r < 0)
                প্রত্যাগম r;

        s = strjoin(p, suffix);
        যদি (!s)
                প্রত্যাগম -ENOMEM;

        যদি (strlen(s) >= UNIT_NAME_MAX) {
                _cleanup_free_ বর্ণ *n = রিক্ত;

                log_debug("Unit name \"%s\" too long, falling back to hashed unit name.", s);

                r = unit_name_hash_long(s, &n);
                যদি (r < 0)
                        প্রত্যাগম r;

                free_and_replace(s, n);
        }

        /* Refuse if this for some other reason didn't result in a valid name */
        যদি (!unit_name_is_valid(s, UNIT_NAME_PLAIN))
                প্রত্যাগম -EINVAL;

        *ret = TAKE_PTR(s);
        প্রত্যাগম 0;
}

পূর্ণাঙ্ক unit_name_from_path_instance(ধ্রুব বর্ণ *prefix, ধ্রুব বর্ণ *path, ধ্রুব বর্ণ *suffix, বর্ণ **ret) {
        _cleanup_free_ বর্ণ *p = রিক্ত, *s = রিক্ত;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(prefix);
        প্রতিজ্ঞা(path);
        প্রতিজ্ঞা(suffix);
        প্রতিজ্ঞা(ret);

        যদি (!unit_prefix_is_valid(prefix))
                প্রত্যাগম -EINVAL;

        যদি (!unit_suffix_is_valid(suffix))
                প্রত্যাগম -EINVAL;

        r = unit_name_path_escape(path, &p);
        যদি (r < 0)
                প্রত্যাগম r;

        s = strjoin(prefix, "@", p, suffix);
        যদি (!s)
                প্রত্যাগম -ENOMEM;

        যদি (strlen(s) >= UNIT_NAME_MAX) /* Return a slightly more descriptive error for this specific condition */
                প্রত্যাগম -ENAMETOOLONG;

        /* Refuse if this for some other reason didn't result in a valid name */
        যদি (!unit_name_is_valid(s, UNIT_NAME_INSTANCE))
                প্রত্যাগম -EINVAL;

        *ret = TAKE_PTR(s);
        প্রত্যাগম 0;
}

পূর্ণাঙ্ক unit_name_to_path(ধ্রুব বর্ণ *name, বর্ণ **ret) {
        _cleanup_free_ বর্ণ *prefix = রিক্ত;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(name);

        r = unit_name_to_prefix(name, &prefix);
        যদি (r < 0)
                প্রত্যাগম r;

        যদি (unit_name_is_hashed(name))
                প্রত্যাগম -ENAMETOOLONG;

        প্রত্যাগম unit_name_path_unescape(prefix, ret);
}

স্থির bool do_escape_mangle(ধ্রুব বর্ণ *f, bool allow_globs, বর্ণ *t) {
        ধ্রুব বর্ণ *valid_chars;
        bool mangled = false;

        প্রতিজ্ঞা(f);
        প্রতিজ্ঞা(t);

        /* We'll only escape the obvious characters here, to play safe.
         *
         * Returns true if any characters were mangled, false otherwise.
         */

        valid_chars = allow_globs ? VALID_CHARS_GLOB : VALID_CHARS_WITH_AT;

        প্রতি (; *f; f++)
                যদি (*f == '/') {
                        *(t++) = '-';
                        mangled = true;
                } অন্যথা যদি (!strchr(valid_chars, *f)) {
                        t = do_escape_char(*f, t);
                        mangled = true;
                } অন্যথা
                        *(t++) = *f;
        *t = 0;

        প্রত্যাগম mangled;
}

/**
 *  Convert a string to a unit name. /dev/blah is converted to dev-blah.device,
 *  /blah/blah is converted to blah-blah.mount, anything else is left alone,
 *  except that @suffix is appended if a valid unit suffix is not present.
 *
 *  If @allow_globs, globs characters are preserved. Otherwise, they are escaped.
 */
পূর্ণাঙ্ক unit_name_mangle_with_suffix(
                ধ্রুব বর্ণ *name,
                ধ্রুব বর্ণ *operation,
                UnitNameMangle flags,
                ধ্রুব বর্ণ *suffix,
                বর্ণ **ret) {

        _cleanup_free_ বর্ণ *s = রিক্ত;
        bool mangled, suggest_escape = true, warn = FLAGS_SET(flags, UNIT_NAME_MANGLE_WARN);
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(name);
        প্রতিজ্ঞা(suffix);
        প্রতিজ্ঞা(ret);

        যদি (isempty(name)) /* We cannot mangle empty unit names to become valid, sorry. */
                প্রত্যাগম -EINVAL;

        যদি (!unit_suffix_is_valid(suffix))
                প্রত্যাগম -EINVAL;

        /* Already a fully valid unit name? If so, no mangling is necessary... */
        যদি (unit_name_is_valid(name, UNIT_NAME_ANY)) {
                যদি (FLAGS_SET(flags, UNIT_NAME_MANGLE_STRICT) && !endswith(name, suffix)) {
                        ধ্রুব বর্ণ *e = ASSERT_PTR(strrchr(name, '.'));

                        প্রত্যাগম log_full_errno(warn ? LOG_NOTICE : LOG_DEBUG,
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
                _cleanup_free_ বর্ণ *n = রিক্ত, *u = রিক্ত;

                r = path_simplify_alloc(name, &n);
                যদি (r < 0)
                        প্রত্যাগম r;

                যদি (is_device_path(n)) {
                        r = unit_name_from_path(n, ".device", &u);
                        যদি (r >= 0) {
                                যদি (FLAGS_SET(flags, UNIT_NAME_MANGLE_STRICT) && !streq(suffix, ".device"))
                                        প্রত্যাগম log_full_errno(warn ? LOG_NOTICE : LOG_DEBUG,
                                                              SYNTHETIC_ERRNO(EINVAL),
                                                              "Path \"%s\" resolves to unit type \"device\", but \"%s\" is expected%s%s.",
                                                              name, suffix + 1,
                                                              operation ? " " : "", strempty(operation));

                                *ret = TAKE_PTR(u);
                                প্রত্যাগম 1;
                        }
                        যদি (r != -EINVAL)
                                প্রত্যাগম r;
                }

                r = unit_name_from_path(n, ".mount", &u);
                যদি (r >= 0) {
                        যদি (FLAGS_SET(flags, UNIT_NAME_MANGLE_STRICT) && !streq(suffix, ".mount"))
                                প্রত্যাগম log_full_errno(warn ? LOG_NOTICE : LOG_DEBUG,
                                                      SYNTHETIC_ERRNO(EINVAL),
                                                      "Path \"%s\" resolves to unit type \"mount\", but \"%s\" is expected%s%s.",
                                                      name, suffix + 1,
                                                      operation ? " " : "", strempty(operation));

                        *ret = TAKE_PTR(u);
                        প্রত্যাগম 1;
                }
                যদি (r != -EINVAL)
                        প্রত্যাগম r;
        }

        s = new(বর্ণ, strlen(name) * 4 + strlen(suffix) + 1);
        যদি (!s)
                প্রত্যাগম -ENOMEM;

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
                প্রত্যাগম -EINVAL;

        *ret = TAKE_PTR(s);
        প্রত্যাগম 1;

good:
        প্রত্যাগম strdup_to(ret, name);
}

পূর্ণাঙ্ক slice_build_parent_slice(ধ্রুব বর্ণ *slice, বর্ণ **ret) {
        প্রতিজ্ঞা(slice);
        প্রতিজ্ঞা(ret);

        যদি (!slice_name_is_valid(slice))
                প্রত্যাগম -EINVAL;

        যদি (streq(slice, SPECIAL_মূল_SLICE)) {
                *ret = রিক্ত;
                প্রত্যাগম 0;
        }

        _cleanup_free_ বর্ণ *s = strdup(slice);
        যদি (!s)
                প্রত্যাগম -ENOMEM;

        বর্ণ *dash = strrchr(s, '-');
        যদি (!dash)
                প্রত্যাগম strdup_to_full(ret, SPECIAL_মূল_SLICE);

        /* We know that s ended with .slice before truncation, so we have enough space. */
        strcpy(dash, ".slice");

        *ret = TAKE_PTR(s);
        প্রত্যাগম 1;
}

পূর্ণাঙ্ক slice_build_subslice(ধ্রুব বর্ণ *slice, ধ্রুব বর্ণ *name, বর্ণ **ret) {
        বর্ণ *subslice;

        প্রতিজ্ঞা(slice);
        প্রতিজ্ঞা(name);
        প্রতিজ্ঞা(ret);

        যদি (!slice_name_is_valid(slice))
                প্রত্যাগম -EINVAL;

        যদি (!unit_prefix_is_valid(name))
                প্রত্যাগম -EINVAL;

        যদি (streq(slice, SPECIAL_মূল_SLICE))
                subslice = strjoin(name, ".slice");
        অন্যথা {
                ধ্রুব বর্ণ *e;

                assert_se(e = endswith(slice, ".slice"));

                subslice = new(বর্ণ, (e - slice) + 1 + strlen(name) + 6 + 1);
                যদি (!subslice)
                        প্রত্যাগম -ENOMEM;

                stpcpy(stpcpy(stpcpy(mempcpy(subslice, slice, e - slice), "-"), name), ".slice");
        }

        *ret = subslice;
        প্রত্যাগম 0;
}

bool slice_name_is_valid(ধ্রুব বর্ণ *name) {
        ধ্রুব বর্ণ *p, *e;
        bool dash = false;

        যদি (!unit_name_is_valid(name, UNIT_NAME_PLAIN))
                প্রত্যাগম false;

        যদি (streq(name, SPECIAL_মূল_SLICE))
                প্রত্যাগম true;

        e = endswith(name, ".slice");
        যদি (!e)
                প্রত্যাগম false;

        প্রতি (p = name; p < e; p++) {

                যদি (*p == '-') {

                        /* Don't allow initial dash */
                        যদি (p == name)
                                প্রত্যাগম false;

                        /* Don't allow multiple dashes */
                        যদি (dash)
                                প্রত্যাগম false;

                        dash = true;
                } অন্যথা
                        dash = false;
        }

        /* Don't allow trailing hash */
        যদি (dash)
                প্রত্যাগম false;

        প্রত্যাগম true;
}

bool unit_name_prefix_equal(ধ্রুব বর্ণ *a, ধ্রুব বর্ণ *b) {
        ধ্রুব বর্ণ *p, *q;

        প্রতিজ্ঞা(a);
        প্রতিজ্ঞা(b);

        যদি (!unit_name_is_valid(a, UNIT_NAME_ANY) || !unit_name_is_valid(b, UNIT_NAME_ANY))
                প্রত্যাগম false;

        p = strchr(a, '@');
        যদি (!p)
                p = strrchr(a, '.');

        q = strchr(b, '@');
        যদি (!q)
                q = strrchr(b, '.');

        প্রতিজ্ঞা(p);
        প্রতিজ্ঞা(q);

        প্রত্যাগম memcmp_nn(a, p - a, b, q - b) == 0;
}
