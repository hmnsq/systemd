/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن "sd-id128.h"

#ضمن "alloc-util.h"
#ضمن "glob-util.h"
#ضمن "hexdecoct.h"
#ضمن "log.h"
#ضمن "memory-util.h"
#ضمن "path-util.h"
#ضمن "sparse-endian.h"
#ضمن "special.h"
#ضمن "siphash24.h"
#ضمن "string-util.h"
#ضمن "unit-def.h"
#ضمن "unit-name.h"

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

#define LONG_UNIT_NAME_HASH_KEY SD_ID128_MAKE(ec,f2,37,fb,58,32,4a,32,84,9f,06,9b,0d,21,eb,9a)
#define UNIT_NAME_HASH_LENGTH_CHARS 16

bool unit_name_is_valid(ثابت حرف *n, UnitNameFlags flags) {
        ثابت حرف *e, *i, *at;

        أكد((flags & ~(UNIT_NAME_PLAIN|UNIT_NAME_INSTANCE|UNIT_NAME_TEMPLATE)) == 0);

        ܐܢ (_unlikely_(flags == 0))
                ܦܢܐ false;

        ܐܢ (isempty(n))
                ܦܢܐ false;

        ܐܢ (strlen(n) >= UNIT_NAME_MAX)
                ܦܢܐ false;

        e = strrchr(n, '.');
        ܐܢ (!e || e == n)
                ܦܢܐ false;

        ܐܢ (unit_type_from_string(e + 1) < 0)
                ܦܢܐ false;

        لكل (i = n, at = عدم; i < e; i++) {

                ܐܢ (*i == '@' && !at)
                        at = i;

                ܐܢ (!strchr(VALID_CHARS_WITH_AT, *i))
                        ܦܢܐ false;
        }

        ܐܢ (at == n)
                ܦܢܐ false;

        ܐܢ (flags & UNIT_NAME_PLAIN)
                ܐܢ (!at)
                        ܦܢܐ true;

        ܐܢ (flags & UNIT_NAME_INSTANCE)
                ܐܢ (at && e > at + 1)
                        ܦܢܐ true;

        ܐܢ (flags & UNIT_NAME_TEMPLATE)
                ܐܢ (at && e == at + 1)
                        ܦܢܐ true;

        ܦܢܐ false;
}

bool unit_prefix_is_valid(ثابت حرف *p) {

        /* We don't allow additional @ in the prefix string */

        ܐܢ (isempty(p))
                ܦܢܐ false;

        ܦܢܐ in_charset(p, VALID_CHARS);
}

bool unit_instance_is_valid(ثابت حرف *i) {

        /* The max length depends on the length of the string, so we
         * don't really check this here. */

        ܐܢ (isempty(i))
                ܦܢܐ false;

        /* We allow additional @ in the instance string, we do not
         * allow them in the prefix! */

        ܦܢܐ in_charset(i, "@" VALID_CHARS);
}

bool unit_suffix_is_valid(ثابت حرف *s) {
        ܐܢ (isempty(s))
                ܦܢܐ false;

        ܐܢ (s[0] != '.')
                ܦܢܐ false;

        ܐܢ (unit_type_from_string(s + 1) < 0)
                ܦܢܐ false;

        ܦܢܐ true;
}

صحيح unit_name_to_prefix(ثابت حرف *n, حرف **ret) {
        ثابت حرف *p;
        حرف *s;

        أكد(n);
        أكد(ret);

        ܐܢ (!unit_name_is_valid(n, UNIT_NAME_ANY))
                ܦܢܐ -EINVAL;

        p = strchr(n, '@');
        ܐܢ (!p)
                p = strrchr(n, '.');

        assert_se(p);

        s = strndup(n, p - n);
        ܐܢ (!s)
                ܦܢܐ -ENOMEM;

        *ret = s;
        ܦܢܐ 0;
}

UnitNameFlags unit_name_to_instance(ثابت حرف *n, حرف **ret) {
        ثابت حرف *p, *d;

        أكد(n);

        ܐܢ (!unit_name_is_valid(n, UNIT_NAME_ANY))
                ܦܢܐ -EINVAL;

        /* Everything past the first @ and before the last . is the instance */
        p = strchr(n, '@');
        ܐܢ (!p) {
                ܐܢ (ret)
                        *ret = عدم;
                ܦܢܐ UNIT_NAME_PLAIN;
        }

        p++;

        d = strrchr(p, '.');
        ܐܢ (!d)
                ܦܢܐ -EINVAL;

        ܐܢ (ret) {
                حرف *i = strndup(p, d-p);
                ܐܢ (!i)
                        ܦܢܐ -ENOMEM;

                *ret = i;
        }
        ܦܢܐ d > p ? UNIT_NAME_INSTANCE : UNIT_NAME_TEMPLATE;
}

صحيح unit_name_to_prefix_and_instance(ثابت حرف *n, حرف **ret) {
        ثابت حرف *d;
        حرف *s;

        أكد(n);
        أكد(ret);

        ܐܢ (!unit_name_is_valid(n, UNIT_NAME_ANY))
                ܦܢܐ -EINVAL;

        d = strrchr(n, '.');
        ܐܢ (!d)
                ܦܢܐ -EINVAL;

        s = strndup(n, d - n);
        ܐܢ (!s)
                ܦܢܐ -ENOMEM;

        *ret = s;
        ܦܢܐ 0;
}

UnitType unit_name_to_type(ثابت حرف *n) {
        ثابت حرف *e;

        أكد(n);

        ܐܢ (!unit_name_is_valid(n, UNIT_NAME_ANY))
                ܦܢܐ _UNIT_TYPE_INVALID;

        assert_se(e = strrchr(n, '.'));

        ܦܢܐ unit_type_from_string(e + 1);
}

صحيح unit_name_change_suffix(ثابت حرف *n, ثابت حرف *suffix, حرف **ret) {
        _cleanup_free_ حرف *s = عدم;
        size_t a, b;
        ثابت حرف *e;

        أكد(n);
        أكد(suffix);
        أكد(ret);

        ܐܢ (!unit_name_is_valid(n, UNIT_NAME_ANY))
                ܦܢܐ -EINVAL;

        ܐܢ (!unit_suffix_is_valid(suffix))
                ܦܢܐ -EINVAL;

        assert_se(e = strrchr(n, '.'));

        a = e - n;
        b = strlen(suffix);

        s = new(حرف, a + b + 1);
        ܐܢ (!s)
                ܦܢܐ -ENOMEM;

        strcpy(mempcpy(s, n, a), suffix);

        /* Make sure the name is still valid (i.e. didn't grow too large due to longer suffix) */
        ܐܢ (!unit_name_is_valid(s, UNIT_NAME_ANY))
                ܦܢܐ -EINVAL;

        *ret = TAKE_PTR(s);
        ܦܢܐ 0;
}

صحيح unit_name_build(ثابت حرف *prefix, ثابت حرف *instance, ثابت حرف *suffix, حرف **ret) {
        UnitType type;

        أكد(prefix);
        أكد(suffix);
        أكد(ret);

        ܐܢ (suffix[0] != '.')
                ܦܢܐ -EINVAL;

        type = unit_type_from_string(suffix + 1);
        ܐܢ (type < 0)
                ܦܢܐ type;

        ܦܢܐ unit_name_build_from_type(prefix, instance, type, ret);
}

صحيح unit_name_build_from_type(ثابت حرف *prefix, ثابت حرف *instance, UnitType type, حرف **ret) {
        _cleanup_free_ حرف *s = عدم;
        ثابت حرف *ut;

        أكد(prefix);
        أكد(type >= 0);
        أكد(type < _UNIT_TYPE_MAX);
        أكد(ret);

        ܐܢ (!unit_prefix_is_valid(prefix))
                ܦܢܐ -EINVAL;

        ut = unit_type_to_string(type);

        ܐܢ (instance) {
                ܐܢ (!unit_instance_is_valid(instance))
                        ܦܢܐ -EINVAL;

                s = strjoin(prefix, "@", instance, ".", ut);
        } ܐܚܪܢܐ
                s = strjoin(prefix, ".", ut);
        ܐܢ (!s)
                ܦܢܐ -ENOMEM;

        /* Verify that this didn't grow too large (or otherwise is invalid) */
        ܐܢ (!unit_name_is_valid(s, instance ? UNIT_NAME_INSTANCE : UNIT_NAME_PLAIN))
                ܦܢܐ -EINVAL;

        *ret = TAKE_PTR(s);
        ܦܢܐ 0;
}

ساكن حرف *do_escape_char(حرف c, حرف *t) {
        أكد(t);

        *(t++) = '\\';
        *(t++) = 'x';
        *(t++) = hexchar(c >> 4);
        *(t++) = hexchar(c);

        ܦܢܐ t;
}

ساكن حرف *do_escape(ثابت حرف *f, حرف *t) {
        أكد(f);
        أكد(t);

        /* do not create units with a leading '.', like for "/.dotdir" mount points */
        ܐܢ (*f == '.') {
                t = do_escape_char(*f, t);
                f++;
        }

        لكل (; *f; f++) {
                ܐܢ (*f == '/')
                        *(t++) = '-';
                ܐܚܪܢܐ ܐܢ (IN_SET(*f, '-', '\\') || !strchr(VALID_CHARS, *f))
                        t = do_escape_char(*f, t);
                ܐܚܪܢܐ
                        *(t++) = *f;
        }

        ܦܢܐ t;
}

حرف* unit_name_escape(ثابت حرف *f) {
        حرف *r, *t;

        أكد(f);

        r = new(حرف, strlen(f)*4+1);
        ܐܢ (!r)
                ܦܢܐ عدم;

        t = do_escape(f, r);
        *t = 0;

        ܦܢܐ r;
}

صحيح unit_name_unescape(ثابت حرف *f, حرف **ret) {
        _cleanup_free_ حرف *r = عدم;
        حرف *t;

        أكد(f);
        أكد(ret);

        r = strdup(f);
        ܐܢ (!r)
                ܦܢܐ -ENOMEM;

        لكل (t = r; *f; f++) {
                ܐܢ (*f == '-')
                        *(t++) = '/';
                ܐܚܪܢܐ ܐܢ (*f == '\\') {
                        صحيح a, b;

                        ܐܢ (f[1] != 'x')
                                ܦܢܐ -EINVAL;

                        a = unhexchar(f[2]);
                        ܐܢ (a < 0)
                                ܦܢܐ -EINVAL;

                        b = unhexchar(f[3]);
                        ܐܢ (b < 0)
                                ܦܢܐ -EINVAL;

                        *(t++) = (حرف) (((uint8_t) a << 4U) | (uint8_t) b);
                        f += 3;
                } ܐܚܪܢܐ
                        *(t++) = *f;
        }

        *t = 0;

        *ret = TAKE_PTR(r);

        ܦܢܐ 0;
}

صحيح unit_name_path_escape(ثابت حرف *f, حرف **ret) {
        _cleanup_free_ حرف *p = عدم;
        حرف *s;
        صحيح r;

        أكد(f);
        أكد(ret);

        r = path_simplify_alloc(f, &p);
        ܐܢ (r < 0)
                ܦܢܐ r;

        ܐܢ (empty_or_root(p))
                s = strdup("-");
        ܐܚܪܢܐ {
                ܐܢ (!path_is_normalized(p))
                        ܦܢܐ -EINVAL;

                /* Truncate trailing slashes and skip leading slashes */
                delete_trailing_chars(p, "/");
                s = unit_name_escape(skip_leading_chars(p, "/"));
        }
        ܐܢ (!s)
                ܦܢܐ -ENOMEM;

        *ret = s;
        ܦܢܐ 0;
}

صحيح unit_name_path_unescape(ثابت حرف *f, حرف **ret) {
        _cleanup_free_ حرف *s = عدم;
        صحيح r;

        أكد(f);

        ܐܢ (isempty(f))
                ܦܢܐ -EINVAL;

        ܐܢ (streq(f, "-")) {
                s = strdup("/");
                ܐܢ (!s)
                        ܦܢܐ -ENOMEM;
        } ܐܚܪܢܐ {
                _cleanup_free_ حرف *w = عدم;

                r = unit_name_unescape(f, &w);
                ܐܢ (r < 0)
                        ܦܢܐ r;

                /* Don't accept trailing or leading slashes */
                ܐܢ (startswith(w, "/") || endswith(w, "/"))
                        ܦܢܐ -EINVAL;

                /* Prefix a slash again */
                s = strjoin("/", w);
                ܐܢ (!s)
                        ܦܢܐ -ENOMEM;

                ܐܢ (!path_is_normalized(s))
                        ܦܢܐ -EINVAL;
        }

        ܐܢ (ret)
                *ret = TAKE_PTR(s);

        ܦܢܐ 0;
}

صحيح unit_name_replace_instance_full(
                ثابت حرف *original,
                ثابت حرف *instance,
                bool accept_glob,
                حرف **ret) {

        _cleanup_free_ حرف *s = عدم;
        ثابت حرف *prefix, *suffix;
        size_t pl;

        أكد(original);
        أكد(instance);
        أكد(ret);

        ܐܢ (!unit_name_is_valid(original, UNIT_NAME_INSTANCE|UNIT_NAME_TEMPLATE))
                ܦܢܐ -EINVAL;
        ܐܢ (!unit_instance_is_valid(instance) && !(accept_glob && in_charset(instance, VALID_CHARS_GLOB)))
                ܦܢܐ -EINVAL;

        prefix = ASSERT_PTR(strchr(original, '@'));
        suffix = ASSERT_PTR(strrchr(original, '.'));
        أكد(prefix < suffix);

        pl = prefix - original + 1; /* include '@' */

        s = new(حرف, pl + strlen(instance) + strlen(suffix) + 1);
        ܐܢ (!s)
                ܦܢܐ -ENOMEM;

#ܐܢ HAS_FEATURE_MEMORY_SANITIZER
        /* MSan doesn't like stpncpy... See also https://github.com/google/sanitizers/issues/926 */
        memzero(s, pl + strlen(instance) + strlen(suffix) + 1);
#endif

        strcpy(stpcpy(stpncpy(s, original, pl), instance), suffix);

        /* Make sure the resulting name still is valid, i.e. didn't grow too large. Globs will be expanded
         * by clients when used, so the check is pointless. */
        ܐܢ (!accept_glob && !unit_name_is_valid(s, UNIT_NAME_INSTANCE))
                ܦܢܐ -EINVAL;

        *ret = TAKE_PTR(s);
        ܦܢܐ 0;
}

صحيح unit_name_template(ثابت حرف *f, حرف **ret) {
        ثابت حرف *p, *e;
        حرف *s;
        size_t a;

        أكد(f);
        أكد(ret);

        ܐܢ (!unit_name_is_valid(f, UNIT_NAME_INSTANCE|UNIT_NAME_TEMPLATE))
                ܦܢܐ -EINVAL;

        assert_se(p = strchr(f, '@'));
        assert_se(e = strrchr(f, '.'));

        a = p - f;

        s = new(حرف, a + 1 + strlen(e) + 1);
        ܐܢ (!s)
                ܦܢܐ -ENOMEM;

        strcpy(mempcpy(s, f, a + 1), e);

        *ret = s;
        ܦܢܐ 0;
}

bool unit_name_is_hashed(ثابت حرف *name) {
        ثابت حرف *s;

        ܐܢ (!unit_name_is_valid(name, UNIT_NAME_PLAIN))
                ܦܢܐ false;

        assert_se(s = strrchr(name, '.'));

        ܐܢ (s - name < UNIT_NAME_HASH_LENGTH_CHARS + 1)
                ܦܢܐ false;

        s -= UNIT_NAME_HASH_LENGTH_CHARS;
        ܐܢ (s[-1] != '_')
                ܦܢܐ false;

        لكل (size_t i = 0; i < UNIT_NAME_HASH_LENGTH_CHARS; i++)
                ܐܢ (!strchr(LOWERCASE_HEXDIGITS, s[i]))
                        ܦܢܐ false;

        ܦܢܐ true;
}

صحيح unit_name_hash_long(ثابت حرف *name, حرف **ret) {
        _cleanup_free_ حرف *n = عدم, *hash = عدم;
        ثابت حرف *suffix;
        le64_t h;
        size_t len;

        أكد(ret);

        ܐܢ (strlen(name) < UNIT_NAME_MAX)
                ܦܢܐ -EMSGSIZE;

        suffix = strrchr(name, '.');
        ܐܢ (!suffix)
                ܦܢܐ -EINVAL;

        ܐܢ (unit_type_from_string(suffix+1) < 0)
                ܦܢܐ -EINVAL;

        h = htole64(siphash24_string(name, LONG_UNIT_NAME_HASH_KEY.bytes));

        hash = hexmem(&h, حجم(h));
        ܐܢ (!hash)
                ܦܢܐ -ENOMEM;

        assert_se(strlen(hash) == UNIT_NAME_HASH_LENGTH_CHARS);

        len = UNIT_NAME_MAX - 1 - strlen(suffix+1) - UNIT_NAME_HASH_LENGTH_CHARS - 2;
        أكد(len > 0 && len < UNIT_NAME_MAX);

        n = strndup(name, len);
        ܐܢ (!n)
                ܦܢܐ -ENOMEM;

        ܐܢ (!strextend(&n, "_", hash, suffix))
                ܦܢܐ -ENOMEM;
        assert_se(unit_name_is_valid(n, UNIT_NAME_PLAIN));

        *ret = TAKE_PTR(n);

        ܦܢܐ 0;
}

صحيح unit_name_from_path(ثابت حرف *path, ثابت حرف *suffix, حرف **ret) {
        _cleanup_free_ حرف *p = عدم, *s = عدم;
        صحيح r;

        أكد(path);
        أكد(suffix);
        أكد(ret);

        ܐܢ (!unit_suffix_is_valid(suffix))
                ܦܢܐ -EINVAL;

        r = unit_name_path_escape(path, &p);
        ܐܢ (r < 0)
                ܦܢܐ r;

        s = strjoin(p, suffix);
        ܐܢ (!s)
                ܦܢܐ -ENOMEM;

        ܐܢ (strlen(s) >= UNIT_NAME_MAX) {
                _cleanup_free_ حرف *n = عدم;

                log_debug("Unit name \"%s\" too long, falling back to hashed unit name.", s);

                r = unit_name_hash_long(s, &n);
                ܐܢ (r < 0)
                        ܦܢܐ r;

                free_and_replace(s, n);
        }

        /* Refuse if this for some other reason didn't result in a valid name */
        ܐܢ (!unit_name_is_valid(s, UNIT_NAME_PLAIN))
                ܦܢܐ -EINVAL;

        *ret = TAKE_PTR(s);
        ܦܢܐ 0;
}

صحيح unit_name_from_path_instance(ثابت حرف *prefix, ثابت حرف *path, ثابت حرف *suffix, حرف **ret) {
        _cleanup_free_ حرف *p = عدم, *s = عدم;
        صحيح r;

        أكد(prefix);
        أكد(path);
        أكد(suffix);
        أكد(ret);

        ܐܢ (!unit_prefix_is_valid(prefix))
                ܦܢܐ -EINVAL;

        ܐܢ (!unit_suffix_is_valid(suffix))
                ܦܢܐ -EINVAL;

        r = unit_name_path_escape(path, &p);
        ܐܢ (r < 0)
                ܦܢܐ r;

        s = strjoin(prefix, "@", p, suffix);
        ܐܢ (!s)
                ܦܢܐ -ENOMEM;

        ܐܢ (strlen(s) >= UNIT_NAME_MAX) /* Return a slightly more descriptive error for this specific condition */
                ܦܢܐ -ENAMETOOLONG;

        /* Refuse if this for some other reason didn't result in a valid name */
        ܐܢ (!unit_name_is_valid(s, UNIT_NAME_INSTANCE))
                ܦܢܐ -EINVAL;

        *ret = TAKE_PTR(s);
        ܦܢܐ 0;
}

صحيح unit_name_to_path(ثابت حرف *name, حرف **ret) {
        _cleanup_free_ حرف *prefix = عدم;
        صحيح r;

        أكد(name);

        r = unit_name_to_prefix(name, &prefix);
        ܐܢ (r < 0)
                ܦܢܐ r;

        ܐܢ (unit_name_is_hashed(name))
                ܦܢܐ -ENAMETOOLONG;

        ܦܢܐ unit_name_path_unescape(prefix, ret);
}

ساكن bool do_escape_mangle(ثابت حرف *f, bool allow_globs, حرف *t) {
        ثابت حرف *valid_chars;
        bool mangled = false;

        أكد(f);
        أكد(t);

        /* We'll only escape the obvious characters here, to play safe.
         *
         * Returns true if any characters were mangled, false otherwise.
         */

        valid_chars = allow_globs ? VALID_CHARS_GLOB : VALID_CHARS_WITH_AT;

        لكل (; *f; f++)
                ܐܢ (*f == '/') {
                        *(t++) = '-';
                        mangled = true;
                } ܐܚܪܢܐ ܐܢ (!strchr(valid_chars, *f)) {
                        t = do_escape_char(*f, t);
                        mangled = true;
                } ܐܚܪܢܐ
                        *(t++) = *f;
        *t = 0;

        ܦܢܐ mangled;
}

/**
 *  Convert a string to a unit name. /dev/blah is converted to dev-blah.device,
 *  /blah/blah is converted to blah-blah.mount, anything else is left alone,
 *  except that @suffix is appended if a valid unit suffix is not present.
 *
 *  If @allow_globs, globs characters are preserved. Otherwise, they are escaped.
 */
صحيح unit_name_mangle_with_suffix(
                ثابت حرف *name,
                ثابت حرف *operation,
                UnitNameMangle flags,
                ثابت حرف *suffix,
                حرف **ret) {

        _cleanup_free_ حرف *s = عدم;
        bool mangled, suggest_escape = true, warn = FLAGS_SET(flags, UNIT_NAME_MANGLE_WARN);
        صحيح r;

        أكد(name);
        أكد(suffix);
        أكد(ret);

        ܐܢ (isempty(name)) /* We cannot mangle empty unit names to become valid, sorry. */
                ܦܢܐ -EINVAL;

        ܐܢ (!unit_suffix_is_valid(suffix))
                ܦܢܐ -EINVAL;

        /* Already a fully valid unit name? If so, no mangling is necessary... */
        ܐܢ (unit_name_is_valid(name, UNIT_NAME_ANY)) {
                ܐܢ (FLAGS_SET(flags, UNIT_NAME_MANGLE_STRICT) && !endswith(name, suffix)) {
                        ثابت حرف *e = ASSERT_PTR(strrchr(name, '.'));

                        ܦܢܐ log_full_errno(warn ? LOG_NOTICE : LOG_DEBUG,
                                              SYNTHETIC_ERRNO(EINVAL),
                                              "Unit name \"%s\" has unit type \"%s\", but \"%s\" is expected%s%s.",
                                              name, e + 1, suffix + 1,
                                              operation ? " " : "", strempty(operation));
                }

                اذهب good;
        }

        /* Already a fully valid globbing expression? If so, no mangling is necessary either... */
        ܐܢ (string_is_glob(name) && in_charset(name, VALID_CHARS_GLOB)) {
                ܐܢ (FLAGS_SET(flags, UNIT_NAME_MANGLE_GLOB))
                        اذهب good;
                log_full(warn ? LOG_NOTICE : LOG_DEBUG,
                         "Glob pattern passed%s%s, but globs are not supported for this.",
                         operation ? " " : "", strempty(operation));
                suggest_escape = false;
        }

        ܐܢ (path_is_absolute(name)) {
                _cleanup_free_ حرف *n = عدم, *u = عدم;

                r = path_simplify_alloc(name, &n);
                ܐܢ (r < 0)
                        ܦܢܐ r;

                ܐܢ (is_device_path(n)) {
                        r = unit_name_from_path(n, ".device", &u);
                        ܐܢ (r >= 0) {
                                ܐܢ (FLAGS_SET(flags, UNIT_NAME_MANGLE_STRICT) && !streq(suffix, ".device"))
                                        ܦܢܐ log_full_errno(warn ? LOG_NOTICE : LOG_DEBUG,
                                                              SYNTHETIC_ERRNO(EINVAL),
                                                              "Path \"%s\" resolves to unit type \"device\", but \"%s\" is expected%s%s.",
                                                              name, suffix + 1,
                                                              operation ? " " : "", strempty(operation));

                                *ret = TAKE_PTR(u);
                                ܦܢܐ 1;
                        }
                        ܐܢ (r != -EINVAL)
                                ܦܢܐ r;
                }

                r = unit_name_from_path(n, ".mount", &u);
                ܐܢ (r >= 0) {
                        ܐܢ (FLAGS_SET(flags, UNIT_NAME_MANGLE_STRICT) && !streq(suffix, ".mount"))
                                ܦܢܐ log_full_errno(warn ? LOG_NOTICE : LOG_DEBUG,
                                                      SYNTHETIC_ERRNO(EINVAL),
                                                      "Path \"%s\" resolves to unit type \"mount\", but \"%s\" is expected%s%s.",
                                                      name, suffix + 1,
                                                      operation ? " " : "", strempty(operation));

                        *ret = TAKE_PTR(u);
                        ܦܢܐ 1;
                }
                ܐܢ (r != -EINVAL)
                        ܦܢܐ r;
        }

        s = new(حرف, strlen(name) * 4 + strlen(suffix) + 1);
        ܐܢ (!s)
                ܦܢܐ -ENOMEM;

        mangled = do_escape_mangle(name, FLAGS_SET(flags, UNIT_NAME_MANGLE_GLOB), s);
        ܐܢ (mangled)
                log_full(warn ? LOG_NOTICE : LOG_DEBUG,
                         "Invalid unit name \"%s\" escaped as \"%s\"%s.",
                         name, s,
                         suggest_escape ? " (maybe you should use systemd-escape?)" : "");

        /* Append a suffix if it doesn't have any, but only if this is not a glob, so that we can allow
         * "foo.*" as a valid glob. */
        ܐܢ ((!FLAGS_SET(flags, UNIT_NAME_MANGLE_GLOB) || !string_is_glob(s)) && unit_name_to_type(s) < 0)
                strcat(s, suffix);

        /* Make sure mangling didn't grow this too large (but don't do this check if globbing is allowed,
         * since globs generally do not qualify as valid unit names) */
        ܐܢ (!FLAGS_SET(flags, UNIT_NAME_MANGLE_GLOB) && !unit_name_is_valid(s, UNIT_NAME_ANY))
                ܦܢܐ -EINVAL;

        *ret = TAKE_PTR(s);
        ܦܢܐ 1;

good:
        ܦܢܐ strdup_to(ret, name);
}

صحيح slice_build_parent_slice(ثابت حرف *slice, حرف **ret) {
        أكد(slice);
        أكد(ret);

        ܐܢ (!slice_name_is_valid(slice))
                ܦܢܐ -EINVAL;

        ܐܢ (streq(slice, SPECIAL_ROOT_SLICE)) {
                *ret = عدم;
                ܦܢܐ 0;
        }

        _cleanup_free_ حرف *s = strdup(slice);
        ܐܢ (!s)
                ܦܢܐ -ENOMEM;

        حرف *dash = strrchr(s, '-');
        ܐܢ (!dash)
                ܦܢܐ strdup_to_full(ret, SPECIAL_ROOT_SLICE);

        /* We know that s ended with .slice before truncation, so we have enough space. */
        strcpy(dash, ".slice");

        *ret = TAKE_PTR(s);
        ܦܢܐ 1;
}

صحيح slice_build_subslice(ثابت حرف *slice, ثابت حرف *name, حرف **ret) {
        حرف *subslice;

        أكد(slice);
        أكد(name);
        أكد(ret);

        ܐܢ (!slice_name_is_valid(slice))
                ܦܢܐ -EINVAL;

        ܐܢ (!unit_prefix_is_valid(name))
                ܦܢܐ -EINVAL;

        ܐܢ (streq(slice, SPECIAL_ROOT_SLICE))
                subslice = strjoin(name, ".slice");
        ܐܚܪܢܐ {
                ثابت حرف *e;

                assert_se(e = endswith(slice, ".slice"));

                subslice = new(حرف, (e - slice) + 1 + strlen(name) + 6 + 1);
                ܐܢ (!subslice)
                        ܦܢܐ -ENOMEM;

                stpcpy(stpcpy(stpcpy(mempcpy(subslice, slice, e - slice), "-"), name), ".slice");
        }

        *ret = subslice;
        ܦܢܐ 0;
}

bool slice_name_is_valid(ثابت حرف *name) {
        ثابت حرف *p, *e;
        bool dash = false;

        ܐܢ (!unit_name_is_valid(name, UNIT_NAME_PLAIN))
                ܦܢܐ false;

        ܐܢ (streq(name, SPECIAL_ROOT_SLICE))
                ܦܢܐ true;

        e = endswith(name, ".slice");
        ܐܢ (!e)
                ܦܢܐ false;

        لكل (p = name; p < e; p++) {

                ܐܢ (*p == '-') {

                        /* Don't allow initial dash */
                        ܐܢ (p == name)
                                ܦܢܐ false;

                        /* Don't allow multiple dashes */
                        ܐܢ (dash)
                                ܦܢܐ false;

                        dash = true;
                } ܐܚܪܢܐ
                        dash = false;
        }

        /* Don't allow trailing hash */
        ܐܢ (dash)
                ܦܢܐ false;

        ܦܢܐ true;
}

bool unit_name_prefix_equal(ثابت حرف *a, ثابت حرف *b) {
        ثابت حرف *p, *q;

        أكد(a);
        أكد(b);

        ܐܢ (!unit_name_is_valid(a, UNIT_NAME_ANY) || !unit_name_is_valid(b, UNIT_NAME_ANY))
                ܦܢܐ false;

        p = strchr(a, '@');
        ܐܢ (!p)
                p = strrchr(a, '.');

        q = strchr(b, '@');
        ܐܢ (!q)
                q = strrchr(b, '.');

        أكد(p);
        أكد(q);

        ܦܢܐ memcmp_nn(a, p - a, b, q - b) == 0;
}
