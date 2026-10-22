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
#define UNIT_NAME_HASH_দৈর্ঘ্য_CHARS 16

bool unit_name_is_valid(ধ্রুবক অক্ষর *n, UnitNameFlags flags) {
        ধ্রুবক অক্ষর *e, *i, *at;

        প্রতিজ্ঞা((flags & ~(UNIT_NAME_PLAIN|UNIT_NAME_INSTANCE|UNIT_NAME_TEMPLATE)) == 0);

        যদি (_unlikely_(flags == 0))
                ফেরত false;

        যদি (isempty(n))
                ফেরত false;

        যদি (strlen(n) >= UNIT_NAME_MAX)
                ফেরত false;

        e = strrchr(n, '.');
        যদি (!e || e == n)
                ফেরত false;

        যদি (unit_type_from_string(e + 1) < 0)
                ফেরত false;

        জন্য (i = n, at = ফাঁকা; i < e; i++) {

                যদি (*i == '@' && !at)
                        at = i;

                যদি (!strchr(VALID_CHARS_WITH_AT, *i))
                        ফেরত false;
        }

        যদি (at == n)
                ফেরত false;

        যদি (flags & UNIT_NAME_PLAIN)
                যদি (!at)
                        ফেরত true;

        যদি (flags & UNIT_NAME_INSTANCE)
                যদি (at && e > at + 1)
                        ফেরত true;

        যদি (flags & UNIT_NAME_TEMPLATE)
                যদি (at && e == at + 1)
                        ফেরত true;

        ফেরত false;
}

bool unit_prefix_is_valid(ধ্রুবক অক্ষর *p) {

        /* We don't allow additional @ in the prefix string */

        যদি (isempty(p))
                ফেরত false;

        ফেরত in_charset(p, VALID_CHARS);
}

bool unit_instance_is_valid(ধ্রুবক অক্ষর *i) {

        /* The max length depends on the length of the string, so we
         * don't really check this here. */

        যদি (isempty(i))
                ফেরত false;

        /* We allow additional @ in the instance string, we do not
         * allow them in the prefix! */

        ফেরত in_charset(i, "@" VALID_CHARS);
}

bool unit_suffix_is_valid(ধ্রুবক অক্ষর *s) {
        যদি (isempty(s))
                ফেরত false;

        যদি (s[0] != '.')
                ফেরত false;

        যদি (unit_type_from_string(s + 1) < 0)
                ফেরত false;

        ফেরত true;
}

পূর্ণসংখ্যা unit_name_to_prefix(ধ্রুবক অক্ষর *n, অক্ষর **ret) {
        ধ্রুবক অক্ষর *p;
        অক্ষর *s;

        প্রতিজ্ঞা(n);
        প্রতিজ্ঞা(ret);

        যদি (!unit_name_is_valid(n, UNIT_NAME_ANY))
                ফেরত -EINVAL;

        p = strchr(n, '@');
        যদি (!p)
                p = strrchr(n, '.');

        assert_se(p);

        s = strndup(n, p - n);
        যদি (!s)
                ফেরত -ENOMEM;

        *ret = s;
        ফেরত 0;
}

UnitNameFlags unit_name_to_instance(ধ্রুবক অক্ষর *n, অক্ষর **ret) {
        ধ্রুবক অক্ষর *p, *d;

        প্রতিজ্ঞা(n);

        যদি (!unit_name_is_valid(n, UNIT_NAME_ANY))
                ফেরত -EINVAL;

        /* Everything past the first @ and before the last . is the instance */
        p = strchr(n, '@');
        যদি (!p) {
                যদি (ret)
                        *ret = ফাঁকা;
                ফেরত UNIT_NAME_PLAIN;
        }

        p++;

        d = strrchr(p, '.');
        যদি (!d)
                ফেরত -EINVAL;

        যদি (ret) {
                অক্ষর *i = strndup(p, d-p);
                যদি (!i)
                        ফেরত -ENOMEM;

                *ret = i;
        }
        ফেরত d > p ? UNIT_NAME_INSTANCE : UNIT_NAME_TEMPLATE;
}

পূর্ণসংখ্যা unit_name_to_prefix_and_instance(ধ্রুবক অক্ষর *n, অক্ষর **ret) {
        ধ্রুবক অক্ষর *d;
        অক্ষর *s;

        প্রতিজ্ঞা(n);
        প্রতিজ্ঞা(ret);

        যদি (!unit_name_is_valid(n, UNIT_NAME_ANY))
                ফেরত -EINVAL;

        d = strrchr(n, '.');
        যদি (!d)
                ফেরত -EINVAL;

        s = strndup(n, d - n);
        যদি (!s)
                ফেরত -ENOMEM;

        *ret = s;
        ফেরত 0;
}

UnitType unit_name_to_type(ধ্রুবক অক্ষর *n) {
        ধ্রুবক অক্ষর *e;

        প্রতিজ্ঞা(n);

        যদি (!unit_name_is_valid(n, UNIT_NAME_ANY))
                ফেরত _UNIT_TYPE_INVALID;

        assert_se(e = strrchr(n, '.'));

        ফেরত unit_type_from_string(e + 1);
}

পূর্ণসংখ্যা unit_name_change_suffix(ধ্রুবক অক্ষর *n, ধ্রুবক অক্ষর *suffix, অক্ষর **ret) {
        _cleanup_free_ অক্ষর *s = ফাঁকা;
        size_t a, b;
        ধ্রুবক অক্ষর *e;

        প্রতিজ্ঞা(n);
        প্রতিজ্ঞা(suffix);
        প্রতিজ্ঞা(ret);

        যদি (!unit_name_is_valid(n, UNIT_NAME_ANY))
                ফেরত -EINVAL;

        যদি (!unit_suffix_is_valid(suffix))
                ফেরত -EINVAL;

        assert_se(e = strrchr(n, '.'));

        a = e - n;
        b = strlen(suffix);

        s = new(অক্ষর, a + b + 1);
        যদি (!s)
                ফেরত -ENOMEM;

        strcpy(mempcpy(s, n, a), suffix);

        /* Make sure the name is still valid (i.e. didn't grow too large due to longer suffix) */
        যদি (!unit_name_is_valid(s, UNIT_NAME_ANY))
                ফেরত -EINVAL;

        *ret = TAKE_PTR(s);
        ফেরত 0;
}

পূর্ণসংখ্যা unit_name_build(ধ্রুবক অক্ষর *prefix, ধ্রুবক অক্ষর *instance, ধ্রুবক অক্ষর *suffix, অক্ষর **ret) {
        UnitType type;

        প্রতিজ্ঞা(prefix);
        প্রতিজ্ঞা(suffix);
        প্রতিজ্ঞা(ret);

        যদি (suffix[0] != '.')
                ফেরত -EINVAL;

        type = unit_type_from_string(suffix + 1);
        যদি (type < 0)
                ফেরত type;

        ফেরত unit_name_build_from_type(prefix, instance, type, ret);
}

পূর্ণসংখ্যা unit_name_build_from_type(ধ্রুবক অক্ষর *prefix, ধ্রুবক অক্ষর *instance, UnitType type, অক্ষর **ret) {
        _cleanup_free_ অক্ষর *s = ফাঁকা;
        ধ্রুবক অক্ষর *ut;

        প্রতিজ্ঞা(prefix);
        প্রতিজ্ঞা(type >= 0);
        প্রতিজ্ঞা(type < _UNIT_TYPE_MAX);
        প্রতিজ্ঞা(ret);

        যদি (!unit_prefix_is_valid(prefix))
                ফেরত -EINVAL;

        ut = unit_type_to_string(type);

        যদি (instance) {
                যদি (!unit_instance_is_valid(instance))
                        ফেরত -EINVAL;

                s = strjoin(prefix, "@", instance, ".", ut);
        } অন্যথা
                s = strjoin(prefix, ".", ut);
        যদি (!s)
                ফেরত -ENOMEM;

        /* Verify that this didn't grow too large (or otherwise is invalid) */
        যদি (!unit_name_is_valid(s, instance ? UNIT_NAME_INSTANCE : UNIT_NAME_PLAIN))
                ফেরত -EINVAL;

        *ret = TAKE_PTR(s);
        ফেরত 0;
}

স্থির অক্ষর *do_escape_char(অক্ষর c, অক্ষর *t) {
        প্রতিজ্ঞা(t);

        *(t++) = '\\';
        *(t++) = 'x';
        *(t++) = hexchar(c >> 4);
        *(t++) = hexchar(c);

        ফেরত t;
}

স্থির অক্ষর *do_escape(ধ্রুবক অক্ষর *f, অক্ষর *t) {
        প্রতিজ্ঞা(f);
        প্রতিজ্ঞা(t);

        /* do not create units with a leading '.', like for "/.dotdir" mount points */
        যদি (*f == '.') {
                t = do_escape_char(*f, t);
                f++;
        }

        জন্য (; *f; f++) {
                যদি (*f == '/')
                        *(t++) = '-';
                অন্যথা যদি (IN_SET(*f, '-', '\\') || !strchr(VALID_CHARS, *f))
                        t = do_escape_char(*f, t);
                অন্যথা
                        *(t++) = *f;
        }

        ফেরত t;
}

অক্ষর* unit_name_escape(ধ্রুবক অক্ষর *f) {
        অক্ষর *r, *t;

        প্রতিজ্ঞা(f);

        r = new(অক্ষর, strlen(f)*4+1);
        যদি (!r)
                ফেরত ফাঁকা;

        t = do_escape(f, r);
        *t = 0;

        ফেরত r;
}

পূর্ণসংখ্যা unit_name_unescape(ধ্রুবক অক্ষর *f, অক্ষর **ret) {
        _cleanup_free_ অক্ষর *r = ফাঁকা;
        অক্ষর *t;

        প্রতিজ্ঞা(f);
        প্রতিজ্ঞা(ret);

        r = strdup(f);
        যদি (!r)
                ফেরত -ENOMEM;

        জন্য (t = r; *f; f++) {
                যদি (*f == '-')
                        *(t++) = '/';
                অন্যথা যদি (*f == '\\') {
                        পূর্ণসংখ্যা a, b;

                        যদি (f[1] != 'x')
                                ফেরত -EINVAL;

                        a = unhexchar(f[2]);
                        যদি (a < 0)
                                ফেরত -EINVAL;

                        b = unhexchar(f[3]);
                        যদি (b < 0)
                                ফেরত -EINVAL;

                        *(t++) = (অক্ষর) (((uint8_t) a << 4U) | (uint8_t) b);
                        f += 3;
                } অন্যথা
                        *(t++) = *f;
        }

        *t = 0;

        *ret = TAKE_PTR(r);

        ফেরত 0;
}

পূর্ণসংখ্যা unit_name_path_escape(ধ্রুবক অক্ষর *f, অক্ষর **ret) {
        _cleanup_free_ অক্ষর *p = ফাঁকা;
        অক্ষর *s;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(f);
        প্রতিজ্ঞা(ret);

        r = path_simplify_alloc(f, &p);
        যদি (r < 0)
                ফেরত r;

        যদি (empty_or_মূল(p))
                s = strdup("-");
        অন্যথা {
                যদি (!path_is_normalized(p))
                        ফেরত -EINVAL;

                /* Truncate trailing slashes and skip leading slashes */
                delete_trailing_chars(p, "/");
                s = unit_name_escape(skip_leading_chars(p, "/"));
        }
        যদি (!s)
                ফেরত -ENOMEM;

        *ret = s;
        ফেরত 0;
}

পূর্ণসংখ্যা unit_name_path_unescape(ধ্রুবক অক্ষর *f, অক্ষর **ret) {
        _cleanup_free_ অক্ষর *s = ফাঁকা;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(f);

        যদি (isempty(f))
                ফেরত -EINVAL;

        যদি (streq(f, "-")) {
                s = strdup("/");
                যদি (!s)
                        ফেরত -ENOMEM;
        } অন্যথা {
                _cleanup_free_ অক্ষর *w = ফাঁকা;

                r = unit_name_unescape(f, &w);
                যদি (r < 0)
                        ফেরত r;

                /* Don't accept trailing or leading slashes */
                যদি (startswith(w, "/") || endswith(w, "/"))
                        ফেরত -EINVAL;

                /* Prefix a slash again */
                s = strjoin("/", w);
                যদি (!s)
                        ফেরত -ENOMEM;

                যদি (!path_is_normalized(s))
                        ফেরত -EINVAL;
        }

        যদি (ret)
                *ret = TAKE_PTR(s);

        ফেরত 0;
}

পূর্ণসংখ্যা unit_name_replace_instance_full(
                ধ্রুবক অক্ষর *original,
                ধ্রুবক অক্ষর *instance,
                bool accept_glob,
                অক্ষর **ret) {

        _cleanup_free_ অক্ষর *s = ফাঁকা;
        ধ্রুবক অক্ষর *prefix, *suffix;
        size_t pl;

        প্রতিজ্ঞা(original);
        প্রতিজ্ঞা(instance);
        প্রতিজ্ঞা(ret);

        যদি (!unit_name_is_valid(original, UNIT_NAME_INSTANCE|UNIT_NAME_TEMPLATE))
                ফেরত -EINVAL;
        যদি (!unit_instance_is_valid(instance) && !(accept_glob && in_charset(instance, VALID_CHARS_GLOB)))
                ফেরত -EINVAL;

        prefix = ASSERT_PTR(strchr(original, '@'));
        suffix = ASSERT_PTR(strrchr(original, '.'));
        প্রতিজ্ঞা(prefix < suffix);

        pl = prefix - original + 1; /* include '@' */

        s = new(অক্ষর, pl + strlen(instance) + strlen(suffix) + 1);
        যদি (!s)
                ফেরত -ENOMEM;

#যদি HAS_FEATURE_MEMORY_SANITIZER
        /* MSan doesn't like stpncpy... See also https://github.com/google/sanitizers/issues/926 */
        memzero(s, pl + strlen(instance) + strlen(suffix) + 1);
#endif

        strcpy(stpcpy(stpncpy(s, original, pl), instance), suffix);

        /* Make sure the resulting name still is valid, i.e. didn't grow too large. Globs will be expanded
         * by clients when used, so the check is pointless. */
        যদি (!accept_glob && !unit_name_is_valid(s, UNIT_NAME_INSTANCE))
                ফেরত -EINVAL;

        *ret = TAKE_PTR(s);
        ফেরত 0;
}

পূর্ণসংখ্যা unit_name_template(ধ্রুবক অক্ষর *f, অক্ষর **ret) {
        ধ্রুবক অক্ষর *p, *e;
        অক্ষর *s;
        size_t a;

        প্রতিজ্ঞা(f);
        প্রতিজ্ঞা(ret);

        যদি (!unit_name_is_valid(f, UNIT_NAME_INSTANCE|UNIT_NAME_TEMPLATE))
                ফেরত -EINVAL;

        assert_se(p = strchr(f, '@'));
        assert_se(e = strrchr(f, '.'));

        a = p - f;

        s = new(অক্ষর, a + 1 + strlen(e) + 1);
        যদি (!s)
                ফেরত -ENOMEM;

        strcpy(mempcpy(s, f, a + 1), e);

        *ret = s;
        ফেরত 0;
}

bool unit_name_is_hashed(ধ্রুবক অক্ষর *name) {
        ধ্রুবক অক্ষর *s;

        যদি (!unit_name_is_valid(name, UNIT_NAME_PLAIN))
                ফেরত false;

        assert_se(s = strrchr(name, '.'));

        যদি (s - name < UNIT_NAME_HASH_দৈর্ঘ্য_CHARS + 1)
                ফেরত false;

        s -= UNIT_NAME_HASH_দৈর্ঘ্য_CHARS;
        যদি (s[-1] != '_')
                ফেরত false;

        জন্য (size_t i = 0; i < UNIT_NAME_HASH_দৈর্ঘ্য_CHARS; i++)
                যদি (!strchr(LOWERCASE_HEXDIGITS, s[i]))
                        ফেরত false;

        ফেরত true;
}

পূর্ণসংখ্যা unit_name_hash_long(ধ্রুবক অক্ষর *name, অক্ষর **ret) {
        _cleanup_free_ অক্ষর *n = ফাঁকা, *hash = ফাঁকা;
        ধ্রুবক অক্ষর *suffix;
        le64_t h;
        size_t len;

        প্রতিজ্ঞা(ret);

        যদি (strlen(name) < UNIT_NAME_MAX)
                ফেরত -EMSGSIZE;

        suffix = strrchr(name, '.');
        যদি (!suffix)
                ফেরত -EINVAL;

        যদি (unit_type_from_string(suffix+1) < 0)
                ফেরত -EINVAL;

        h = htole64(siphash24_string(name, LONG_UNIT_NAME_HASH_চাবি.bytes));

        hash = hexmem(&h, আকার(h));
        যদি (!hash)
                ফেরত -ENOMEM;

        assert_se(strlen(hash) == UNIT_NAME_HASH_দৈর্ঘ্য_CHARS);

        len = UNIT_NAME_MAX - 1 - strlen(suffix+1) - UNIT_NAME_HASH_দৈর্ঘ্য_CHARS - 2;
        প্রতিজ্ঞা(len > 0 && len < UNIT_NAME_MAX);

        n = strndup(name, len);
        যদি (!n)
                ফেরত -ENOMEM;

        যদি (!strextend(&n, "_", hash, suffix))
                ফেরত -ENOMEM;
        assert_se(unit_name_is_valid(n, UNIT_NAME_PLAIN));

        *ret = TAKE_PTR(n);

        ফেরত 0;
}

পূর্ণসংখ্যা unit_name_from_path(ধ্রুবক অক্ষর *path, ধ্রুবক অক্ষর *suffix, অক্ষর **ret) {
        _cleanup_free_ অক্ষর *p = ফাঁকা, *s = ফাঁকা;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(path);
        প্রতিজ্ঞা(suffix);
        প্রতিজ্ঞা(ret);

        যদি (!unit_suffix_is_valid(suffix))
                ফেরত -EINVAL;

        r = unit_name_path_escape(path, &p);
        যদি (r < 0)
                ফেরত r;

        s = strjoin(p, suffix);
        যদি (!s)
                ফেরত -ENOMEM;

        যদি (strlen(s) >= UNIT_NAME_MAX) {
                _cleanup_free_ অক্ষর *n = ফাঁকা;

                log_debug("Unit name \"%s\" too long, falling back to hashed unit name.", s);

                r = unit_name_hash_long(s, &n);
                যদি (r < 0)
                        ফেরত r;

                free_and_replace(s, n);
        }

        /* Refuse if this for some other reason didn't result in a valid name */
        যদি (!unit_name_is_valid(s, UNIT_NAME_PLAIN))
                ফেরত -EINVAL;

        *ret = TAKE_PTR(s);
        ফেরত 0;
}

পূর্ণসংখ্যা unit_name_from_path_instance(ধ্রুবক অক্ষর *prefix, ধ্রুবক অক্ষর *path, ধ্রুবক অক্ষর *suffix, অক্ষর **ret) {
        _cleanup_free_ অক্ষর *p = ফাঁকা, *s = ফাঁকা;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(prefix);
        প্রতিজ্ঞা(path);
        প্রতিজ্ঞা(suffix);
        প্রতিজ্ঞা(ret);

        যদি (!unit_prefix_is_valid(prefix))
                ফেরত -EINVAL;

        যদি (!unit_suffix_is_valid(suffix))
                ফেরত -EINVAL;

        r = unit_name_path_escape(path, &p);
        যদি (r < 0)
                ফেরত r;

        s = strjoin(prefix, "@", p, suffix);
        যদি (!s)
                ফেরত -ENOMEM;

        যদি (strlen(s) >= UNIT_NAME_MAX) /* Return a slightly more descriptive error for this specific condition */
                ফেরত -ENAMETOOLONG;

        /* Refuse if this for some other reason didn't result in a valid name */
        যদি (!unit_name_is_valid(s, UNIT_NAME_INSTANCE))
                ফেরত -EINVAL;

        *ret = TAKE_PTR(s);
        ফেরত 0;
}

পূর্ণসংখ্যা unit_name_to_path(ধ্রুবক অক্ষর *name, অক্ষর **ret) {
        _cleanup_free_ অক্ষর *prefix = ফাঁকা;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(name);

        r = unit_name_to_prefix(name, &prefix);
        যদি (r < 0)
                ফেরত r;

        যদি (unit_name_is_hashed(name))
                ফেরত -ENAMETOOLONG;

        ফেরত unit_name_path_unescape(prefix, ret);
}

স্থির bool do_escape_mangle(ধ্রুবক অক্ষর *f, bool allow_globs, অক্ষর *t) {
        ধ্রুবক অক্ষর *valid_chars;
        bool mangled = false;

        প্রতিজ্ঞা(f);
        প্রতিজ্ঞা(t);

        /* We'll only escape the obvious characters here, to play safe.
         *
         * Returns true if any characters were mangled, false otherwise.
         */

        valid_chars = allow_globs ? VALID_CHARS_GLOB : VALID_CHARS_WITH_AT;

        জন্য (; *f; f++)
                যদি (*f == '/') {
                        *(t++) = '-';
                        mangled = true;
                } অন্যথা যদি (!strchr(valid_chars, *f)) {
                        t = do_escape_char(*f, t);
                        mangled = true;
                } অন্যথা
                        *(t++) = *f;
        *t = 0;

        ফেরত mangled;
}

/**
 *  Convert a string to a unit name. /dev/blah is converted to dev-blah.device,
 *  /blah/blah is converted to blah-blah.mount, anything else is left alone,
 *  except that @suffix is appended if a valid unit suffix is not present.
 *
 *  If @allow_globs, globs characters are preserved. Otherwise, they are escaped.
 */
পূর্ণসংখ্যা unit_name_mangle_with_suffix(
                ধ্রুবক অক্ষর *name,
                ধ্রুবক অক্ষর *operation,
                UnitNameMangle flags,
                ধ্রুবক অক্ষর *suffix,
                অক্ষর **ret) {

        _cleanup_free_ অক্ষর *s = ফাঁকা;
        bool mangled, suggest_escape = true, warn = FLAGS_SET(flags, UNIT_NAME_MANGLE_WARN);
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(name);
        প্রতিজ্ঞা(suffix);
        প্রতিজ্ঞা(ret);

        যদি (isempty(name)) /* We cannot mangle empty unit names to become valid, sorry. */
                ফেরত -EINVAL;

        যদি (!unit_suffix_is_valid(suffix))
                ফেরত -EINVAL;

        /* Already a fully valid unit name? If so, no mangling is necessary... */
        যদি (unit_name_is_valid(name, UNIT_NAME_ANY)) {
                যদি (FLAGS_SET(flags, UNIT_NAME_MANGLE_STRICT) && !endswith(name, suffix)) {
                        ধ্রুবক অক্ষর *e = ASSERT_PTR(strrchr(name, '.'));

                        ফেরত log_full_errno(warn ? LOG_NOTICE : LOG_DEBUG,
                                              SYNTHETIC_ERRNO(EINVAL),
                                              "Unit name \"%s\" has unit type \"%s\", but \"%s\" is expected%s%s.",
                                              name, e + 1, suffix + 1,
                                              operation ? " " : "", strempty(operation));
                }

                যাও good;
        }

        /* Already a fully valid globbing expression? If so, no mangling is necessary either... */
        যদি (string_is_glob(name) && in_charset(name, VALID_CHARS_GLOB)) {
                যদি (FLAGS_SET(flags, UNIT_NAME_MANGLE_GLOB))
                        যাও good;
                log_full(warn ? LOG_NOTICE : LOG_DEBUG,
                         "Glob pattern passed%s%s, but globs are not supported for this.",
                         operation ? " " : "", strempty(operation));
                suggest_escape = false;
        }

        যদি (path_is_absolute(name)) {
                _cleanup_free_ অক্ষর *n = ফাঁকা, *u = ফাঁকা;

                r = path_simplify_alloc(name, &n);
                যদি (r < 0)
                        ফেরত r;

                যদি (is_device_path(n)) {
                        r = unit_name_from_path(n, ".device", &u);
                        যদি (r >= 0) {
                                যদি (FLAGS_SET(flags, UNIT_NAME_MANGLE_STRICT) && !streq(suffix, ".device"))
                                        ফেরত log_full_errno(warn ? LOG_NOTICE : LOG_DEBUG,
                                                              SYNTHETIC_ERRNO(EINVAL),
                                                              "Path \"%s\" resolves to unit type \"device\", but \"%s\" is expected%s%s.",
                                                              name, suffix + 1,
                                                              operation ? " " : "", strempty(operation));

                                *ret = TAKE_PTR(u);
                                ফেরত 1;
                        }
                        যদি (r != -EINVAL)
                                ফেরত r;
                }

                r = unit_name_from_path(n, ".mount", &u);
                যদি (r >= 0) {
                        যদি (FLAGS_SET(flags, UNIT_NAME_MANGLE_STRICT) && !streq(suffix, ".mount"))
                                ফেরত log_full_errno(warn ? LOG_NOTICE : LOG_DEBUG,
                                                      SYNTHETIC_ERRNO(EINVAL),
                                                      "Path \"%s\" resolves to unit type \"mount\", but \"%s\" is expected%s%s.",
                                                      name, suffix + 1,
                                                      operation ? " " : "", strempty(operation));

                        *ret = TAKE_PTR(u);
                        ফেরত 1;
                }
                যদি (r != -EINVAL)
                        ফেরত r;
        }

        s = new(অক্ষর, strlen(name) * 4 + strlen(suffix) + 1);
        যদি (!s)
                ফেরত -ENOMEM;

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
                ফেরত -EINVAL;

        *ret = TAKE_PTR(s);
        ফেরত 1;

good:
        ফেরত strdup_to(ret, name);
}

পূর্ণসংখ্যা slice_build_parent_slice(ধ্রুবক অক্ষর *slice, অক্ষর **ret) {
        প্রতিজ্ঞা(slice);
        প্রতিজ্ঞা(ret);

        যদি (!slice_name_is_valid(slice))
                ফেরত -EINVAL;

        যদি (streq(slice, SPECIAL_মূল_SLICE)) {
                *ret = ফাঁকা;
                ফেরত 0;
        }

        _cleanup_free_ অক্ষর *s = strdup(slice);
        যদি (!s)
                ফেরত -ENOMEM;

        অক্ষর *dash = strrchr(s, '-');
        যদি (!dash)
                ফেরত strdup_to_full(ret, SPECIAL_মূল_SLICE);

        /* We know that s ended with .slice before truncation, so we have enough space. */
        strcpy(dash, ".slice");

        *ret = TAKE_PTR(s);
        ফেরত 1;
}

পূর্ণসংখ্যা slice_build_subslice(ধ্রুবক অক্ষর *slice, ধ্রুবক অক্ষর *name, অক্ষর **ret) {
        অক্ষর *subslice;

        প্রতিজ্ঞা(slice);
        প্রতিজ্ঞা(name);
        প্রতিজ্ঞা(ret);

        যদি (!slice_name_is_valid(slice))
                ফেরত -EINVAL;

        যদি (!unit_prefix_is_valid(name))
                ফেরত -EINVAL;

        যদি (streq(slice, SPECIAL_মূল_SLICE))
                subslice = strjoin(name, ".slice");
        অন্যথা {
                ধ্রুবক অক্ষর *e;

                assert_se(e = endswith(slice, ".slice"));

                subslice = new(অক্ষর, (e - slice) + 1 + strlen(name) + 6 + 1);
                যদি (!subslice)
                        ফেরত -ENOMEM;

                stpcpy(stpcpy(stpcpy(mempcpy(subslice, slice, e - slice), "-"), name), ".slice");
        }

        *ret = subslice;
        ফেরত 0;
}

bool slice_name_is_valid(ধ্রুবক অক্ষর *name) {
        ধ্রুবক অক্ষর *p, *e;
        bool dash = false;

        যদি (!unit_name_is_valid(name, UNIT_NAME_PLAIN))
                ফেরত false;

        যদি (streq(name, SPECIAL_মূল_SLICE))
                ফেরত true;

        e = endswith(name, ".slice");
        যদি (!e)
                ফেরত false;

        জন্য (p = name; p < e; p++) {

                যদি (*p == '-') {

                        /* Don't allow initial dash */
                        যদি (p == name)
                                ফেরত false;

                        /* Don't allow multiple dashes */
                        যদি (dash)
                                ফেরত false;

                        dash = true;
                } অন্যথা
                        dash = false;
        }

        /* Don't allow trailing hash */
        যদি (dash)
                ফেরত false;

        ফেরত true;
}

bool unit_name_prefix_equal(ধ্রুবক অক্ষর *a, ধ্রুবক অক্ষর *b) {
        ধ্রুবক অক্ষর *p, *q;

        প্রতিজ্ঞা(a);
        প্রতিজ্ঞা(b);

        যদি (!unit_name_is_valid(a, UNIT_NAME_ANY) || !unit_name_is_valid(b, UNIT_NAME_ANY))
                ফেরত false;

        p = strchr(a, '@');
        যদি (!p)
                p = strrchr(a, '.');

        q = strchr(b, '@');
        যদি (!q)
                q = strrchr(b, '.');

        প্রতিজ্ঞা(p);
        প্রতিজ্ঞা(q);

        ফেরত memcmp_nn(a, p - a, b, q - b) == 0;
}
