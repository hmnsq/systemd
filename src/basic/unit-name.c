/* SPDX-License-Identifier: LGPL-2.1-or-later */

#અન્તર્ભાવ "sd-id128.h"

#અન્તર્ભાવ "alloc-util.h"
#અન્તર્ભાવ "glob-util.h"
#અન્તર્ભાવ "hexdecoct.h"
#અન્તર્ભાવ "log.h"
#અન્તર્ભાવ "memory-util.h"
#અન્તર્ભાવ "path-util.h"
#અન્તર્ભાવ "sparse-endian.h"
#અન્તર્ભાવ "special.h"
#અન્તર્ભાવ "siphash24.h"
#અન્તર્ભાવ "string-util.h"
#અન્તર્ભાવ "unit-def.h"
#અન્તર્ભાવ "unit-name.h"

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

#define LONG_UNIT_NAME_HASH_ચાવી SD_ID128_MAKE(ec,f2,37,fb,58,32,4a,32,84,9f,06,9b,0d,21,eb,9a)
#define UNIT_NAME_HASH_LENGTH_CHARS 16

bool unit_name_is_valid(અચલ અક્ષર *n, UnitNameFlags flags) {
        અચલ અક્ષર *e, *i, *at;

        પ્રતિજ્ઞા((flags & ~(UNIT_NAME_PLAIN|UNIT_NAME_INSTANCE|UNIT_NAME_TEMPLATE)) == 0);

        જો (_unlikely_(flags == 0))
                પાછા false;

        જો (isempty(n))
                પાછા false;

        જો (strlen(n) >= UNIT_NAME_MAX)
                પાછા false;

        e = strrchr(n, '.');
        જો (!e || e == n)
                પાછા false;

        જો (unit_type_from_string(e + 1) < 0)
                પાછા false;

        માટે (i = n, at = ખાલી; i < e; i++) {

                જો (*i == '@' && !at)
                        at = i;

                જો (!strchr(VALID_CHARS_WITH_AT, *i))
                        પાછા false;
        }

        જો (at == n)
                પાછા false;

        જો (flags & UNIT_NAME_PLAIN)
                જો (!at)
                        પાછા true;

        જો (flags & UNIT_NAME_INSTANCE)
                જો (at && e > at + 1)
                        પાછા true;

        જો (flags & UNIT_NAME_TEMPLATE)
                જો (at && e == at + 1)
                        પાછા true;

        પાછા false;
}

bool unit_prefix_is_valid(અચલ અક્ષર *p) {

        /* We don't allow additional @ in the prefix string */

        જો (isempty(p))
                પાછા false;

        પાછા in_charset(p, VALID_CHARS);
}

bool unit_instance_is_valid(અચલ અક્ષર *i) {

        /* The max length depends on the length of the string, so we
         * don't really check this here. */

        જો (isempty(i))
                પાછા false;

        /* We allow additional @ in the instance string, we do not
         * allow them in the prefix! */

        પાછા in_charset(i, "@" VALID_CHARS);
}

bool unit_suffix_is_valid(અચલ અક્ષર *s) {
        જો (isempty(s))
                પાછા false;

        જો (s[0] != '.')
                પાછા false;

        જો (unit_type_from_string(s + 1) < 0)
                પાછા false;

        પાછા true;
}

પૂર્ણાંક unit_name_to_prefix(અચલ અક્ષર *n, અક્ષર **ret) {
        અચલ અક્ષર *p;
        અક્ષર *s;

        પ્રતિજ્ઞા(n);
        પ્રતિજ્ઞા(ret);

        જો (!unit_name_is_valid(n, UNIT_NAME_ANY))
                પાછા -EINVAL;

        p = strchr(n, '@');
        જો (!p)
                p = strrchr(n, '.');

        assert_se(p);

        s = strndup(n, p - n);
        જો (!s)
                પાછા -ENOMEM;

        *ret = s;
        પાછા 0;
}

UnitNameFlags unit_name_to_instance(અચલ અક્ષર *n, અક્ષર **ret) {
        અચલ અક્ષર *p, *d;

        પ્રતિજ્ઞા(n);

        જો (!unit_name_is_valid(n, UNIT_NAME_ANY))
                પાછા -EINVAL;

        /* Everything past the first @ and before the last . is the instance */
        p = strchr(n, '@');
        જો (!p) {
                જો (ret)
                        *ret = ખાલી;
                પાછા UNIT_NAME_PLAIN;
        }

        p++;

        d = strrchr(p, '.');
        જો (!d)
                પાછા -EINVAL;

        જો (ret) {
                અક્ષર *i = strndup(p, d-p);
                જો (!i)
                        પાછા -ENOMEM;

                *ret = i;
        }
        પાછા d > p ? UNIT_NAME_INSTANCE : UNIT_NAME_TEMPLATE;
}

પૂર્ણાંક unit_name_to_prefix_and_instance(અચલ અક્ષર *n, અક્ષર **ret) {
        અચલ અક્ષર *d;
        અક્ષર *s;

        પ્રતિજ્ઞા(n);
        પ્રતિજ્ઞા(ret);

        જો (!unit_name_is_valid(n, UNIT_NAME_ANY))
                પાછા -EINVAL;

        d = strrchr(n, '.');
        જો (!d)
                પાછા -EINVAL;

        s = strndup(n, d - n);
        જો (!s)
                પાછા -ENOMEM;

        *ret = s;
        પાછા 0;
}

UnitType unit_name_to_type(અચલ અક્ષર *n) {
        અચલ અક્ષર *e;

        પ્રતિજ્ઞા(n);

        જો (!unit_name_is_valid(n, UNIT_NAME_ANY))
                પાછા _UNIT_TYPE_INVALID;

        assert_se(e = strrchr(n, '.'));

        પાછા unit_type_from_string(e + 1);
}

પૂર્ણાંક unit_name_change_suffix(અચલ અક્ષર *n, અચલ અક્ષર *suffix, અક્ષર **ret) {
        _cleanup_free_ અક્ષર *s = ખાલી;
        size_t a, b;
        અચલ અક્ષર *e;

        પ્રતિજ્ઞા(n);
        પ્રતિજ્ઞા(suffix);
        પ્રતિજ્ઞા(ret);

        જો (!unit_name_is_valid(n, UNIT_NAME_ANY))
                પાછા -EINVAL;

        જો (!unit_suffix_is_valid(suffix))
                પાછા -EINVAL;

        assert_se(e = strrchr(n, '.'));

        a = e - n;
        b = strlen(suffix);

        s = new(અક્ષર, a + b + 1);
        જો (!s)
                પાછા -ENOMEM;

        strcpy(mempcpy(s, n, a), suffix);

        /* Make sure the name is still valid (i.e. didn't grow too large due to longer suffix) */
        જો (!unit_name_is_valid(s, UNIT_NAME_ANY))
                પાછા -EINVAL;

        *ret = TAKE_PTR(s);
        પાછા 0;
}

પૂર્ણાંક unit_name_build(અચલ અક્ષર *prefix, અચલ અક્ષર *instance, અચલ અક્ષર *suffix, અક્ષર **ret) {
        UnitType type;

        પ્રતિજ્ઞા(prefix);
        પ્રતિજ્ઞા(suffix);
        પ્રતિજ્ઞા(ret);

        જો (suffix[0] != '.')
                પાછા -EINVAL;

        type = unit_type_from_string(suffix + 1);
        જો (type < 0)
                પાછા type;

        પાછા unit_name_build_from_type(prefix, instance, type, ret);
}

પૂર્ણાંક unit_name_build_from_type(અચલ અક્ષર *prefix, અચલ અક્ષર *instance, UnitType type, અક્ષર **ret) {
        _cleanup_free_ અક્ષર *s = ખાલી;
        અચલ અક્ષર *ut;

        પ્રતિજ્ઞા(prefix);
        પ્રતિજ્ઞા(type >= 0);
        પ્રતિજ્ઞા(type < _UNIT_TYPE_MAX);
        પ્રતિજ્ઞા(ret);

        જો (!unit_prefix_is_valid(prefix))
                પાછા -EINVAL;

        ut = unit_type_to_string(type);

        જો (instance) {
                જો (!unit_instance_is_valid(instance))
                        પાછા -EINVAL;

                s = strjoin(prefix, "@", instance, ".", ut);
        } નહીંતર
                s = strjoin(prefix, ".", ut);
        જો (!s)
                પાછા -ENOMEM;

        /* Verify that this didn't grow too large (or otherwise is invalid) */
        જો (!unit_name_is_valid(s, instance ? UNIT_NAME_INSTANCE : UNIT_NAME_PLAIN))
                પાછા -EINVAL;

        *ret = TAKE_PTR(s);
        પાછા 0;
}

સ્થિર અક્ષર *do_escape_char(અક્ષર c, અક્ષર *t) {
        પ્રતિજ્ઞા(t);

        *(t++) = '\\';
        *(t++) = 'x';
        *(t++) = hexchar(c >> 4);
        *(t++) = hexchar(c);

        પાછા t;
}

સ્થિર અક્ષર *do_escape(અચલ અક્ષર *f, અક્ષર *t) {
        પ્રતિજ્ઞા(f);
        પ્રતિજ્ઞા(t);

        /* do not create units with a leading '.', like for "/.dotdir" mount points */
        જો (*f == '.') {
                t = do_escape_char(*f, t);
                f++;
        }

        માટે (; *f; f++) {
                જો (*f == '/')
                        *(t++) = '-';
                નહીંતર જો (IN_SET(*f, '-', '\\') || !strchr(VALID_CHARS, *f))
                        t = do_escape_char(*f, t);
                નહીંતર
                        *(t++) = *f;
        }

        પાછા t;
}

અક્ષર* unit_name_escape(અચલ અક્ષર *f) {
        અક્ષર *r, *t;

        પ્રતિજ્ઞા(f);

        r = new(અક્ષર, strlen(f)*4+1);
        જો (!r)
                પાછા ખાલી;

        t = do_escape(f, r);
        *t = 0;

        પાછા r;
}

પૂર્ણાંક unit_name_unescape(અચલ અક્ષર *f, અક્ષર **ret) {
        _cleanup_free_ અક્ષર *r = ખાલી;
        અક્ષર *t;

        પ્રતિજ્ઞા(f);
        પ્રતિજ્ઞા(ret);

        r = strdup(f);
        જો (!r)
                પાછા -ENOMEM;

        માટે (t = r; *f; f++) {
                જો (*f == '-')
                        *(t++) = '/';
                નહીંતર જો (*f == '\\') {
                        પૂર્ણાંક a, b;

                        જો (f[1] != 'x')
                                પાછા -EINVAL;

                        a = unhexchar(f[2]);
                        જો (a < 0)
                                પાછા -EINVAL;

                        b = unhexchar(f[3]);
                        જો (b < 0)
                                પાછા -EINVAL;

                        *(t++) = (અક્ષર) (((uint8_t) a << 4U) | (uint8_t) b);
                        f += 3;
                } નહીંતર
                        *(t++) = *f;
        }

        *t = 0;

        *ret = TAKE_PTR(r);

        પાછા 0;
}

પૂર્ણાંક unit_name_path_escape(અચલ અક્ષર *f, અક્ષર **ret) {
        _cleanup_free_ અક્ષર *p = ખાલી;
        અક્ષર *s;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(f);
        પ્રતિજ્ઞા(ret);

        r = path_simplify_alloc(f, &p);
        જો (r < 0)
                પાછા r;

        જો (empty_or_મૂળ(p))
                s = strdup("-");
        નહીંતર {
                જો (!path_is_normalized(p))
                        પાછા -EINVAL;

                /* Truncate trailing slashes and skip leading slashes */
                delete_trailing_chars(p, "/");
                s = unit_name_escape(skip_leading_chars(p, "/"));
        }
        જો (!s)
                પાછા -ENOMEM;

        *ret = s;
        પાછા 0;
}

પૂર્ણાંક unit_name_path_unescape(અચલ અક્ષર *f, અક્ષર **ret) {
        _cleanup_free_ અક્ષર *s = ખાલી;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(f);

        જો (isempty(f))
                પાછા -EINVAL;

        જો (streq(f, "-")) {
                s = strdup("/");
                જો (!s)
                        પાછા -ENOMEM;
        } નહીંતર {
                _cleanup_free_ અક્ષર *w = ખાલી;

                r = unit_name_unescape(f, &w);
                જો (r < 0)
                        પાછા r;

                /* Don't accept trailing or leading slashes */
                જો (startswith(w, "/") || endswith(w, "/"))
                        પાછા -EINVAL;

                /* Prefix a slash again */
                s = strjoin("/", w);
                જો (!s)
                        પાછા -ENOMEM;

                જો (!path_is_normalized(s))
                        પાછા -EINVAL;
        }

        જો (ret)
                *ret = TAKE_PTR(s);

        પાછા 0;
}

પૂર્ણાંક unit_name_replace_instance_full(
                અચલ અક્ષર *original,
                અચલ અક્ષર *instance,
                bool accept_glob,
                અક્ષર **ret) {

        _cleanup_free_ અક્ષર *s = ખાલી;
        અચલ અક્ષર *prefix, *suffix;
        size_t pl;

        પ્રતિજ્ઞા(original);
        પ્રતિજ્ઞા(instance);
        પ્રતિજ્ઞા(ret);

        જો (!unit_name_is_valid(original, UNIT_NAME_INSTANCE|UNIT_NAME_TEMPLATE))
                પાછા -EINVAL;
        જો (!unit_instance_is_valid(instance) && !(accept_glob && in_charset(instance, VALID_CHARS_GLOB)))
                પાછા -EINVAL;

        prefix = ASSERT_PTR(strchr(original, '@'));
        suffix = ASSERT_PTR(strrchr(original, '.'));
        પ્રતિજ્ઞા(prefix < suffix);

        pl = prefix - original + 1; /* include '@' */

        s = new(અક્ષર, pl + strlen(instance) + strlen(suffix) + 1);
        જો (!s)
                પાછા -ENOMEM;

#જો HAS_FEATURE_MEMORY_SANITIZER
        /* MSan doesn't like stpncpy... See also https://github.com/google/sanitizers/issues/926 */
        memzero(s, pl + strlen(instance) + strlen(suffix) + 1);
#endif

        strcpy(stpcpy(stpncpy(s, original, pl), instance), suffix);

        /* Make sure the resulting name still is valid, i.e. didn't grow too large. Globs will be expanded
         * by clients when used, so the check is pointless. */
        જો (!accept_glob && !unit_name_is_valid(s, UNIT_NAME_INSTANCE))
                પાછા -EINVAL;

        *ret = TAKE_PTR(s);
        પાછા 0;
}

પૂર્ણાંક unit_name_template(અચલ અક્ષર *f, અક્ષર **ret) {
        અચલ અક્ષર *p, *e;
        અક્ષર *s;
        size_t a;

        પ્રતિજ્ઞા(f);
        પ્રતિજ્ઞા(ret);

        જો (!unit_name_is_valid(f, UNIT_NAME_INSTANCE|UNIT_NAME_TEMPLATE))
                પાછા -EINVAL;

        assert_se(p = strchr(f, '@'));
        assert_se(e = strrchr(f, '.'));

        a = p - f;

        s = new(અક્ષર, a + 1 + strlen(e) + 1);
        જો (!s)
                પાછા -ENOMEM;

        strcpy(mempcpy(s, f, a + 1), e);

        *ret = s;
        પાછા 0;
}

bool unit_name_is_hashed(અચલ અક્ષર *name) {
        અચલ અક્ષર *s;

        જો (!unit_name_is_valid(name, UNIT_NAME_PLAIN))
                પાછા false;

        assert_se(s = strrchr(name, '.'));

        જો (s - name < UNIT_NAME_HASH_LENGTH_CHARS + 1)
                પાછા false;

        s -= UNIT_NAME_HASH_LENGTH_CHARS;
        જો (s[-1] != '_')
                પાછા false;

        માટે (size_t i = 0; i < UNIT_NAME_HASH_LENGTH_CHARS; i++)
                જો (!strchr(LOWERCASE_HEXDIGITS, s[i]))
                        પાછા false;

        પાછા true;
}

પૂર્ણાંક unit_name_hash_long(અચલ અક્ષર *name, અક્ષર **ret) {
        _cleanup_free_ અક્ષર *n = ખાલી, *hash = ખાલી;
        અચલ અક્ષર *suffix;
        le64_t h;
        size_t len;

        પ્રતિજ્ઞા(ret);

        જો (strlen(name) < UNIT_NAME_MAX)
                પાછા -EMSGSIZE;

        suffix = strrchr(name, '.');
        જો (!suffix)
                પાછા -EINVAL;

        જો (unit_type_from_string(suffix+1) < 0)
                પાછા -EINVAL;

        h = htole64(siphash24_string(name, LONG_UNIT_NAME_HASH_ચાવી.bytes));

        hash = hexmem(&h, કદ(h));
        જો (!hash)
                પાછા -ENOMEM;

        assert_se(strlen(hash) == UNIT_NAME_HASH_LENGTH_CHARS);

        len = UNIT_NAME_MAX - 1 - strlen(suffix+1) - UNIT_NAME_HASH_LENGTH_CHARS - 2;
        પ્રતિજ્ઞા(len > 0 && len < UNIT_NAME_MAX);

        n = strndup(name, len);
        જો (!n)
                પાછા -ENOMEM;

        જો (!strextend(&n, "_", hash, suffix))
                પાછા -ENOMEM;
        assert_se(unit_name_is_valid(n, UNIT_NAME_PLAIN));

        *ret = TAKE_PTR(n);

        પાછા 0;
}

પૂર્ણાંક unit_name_from_path(અચલ અક્ષર *path, અચલ અક્ષર *suffix, અક્ષર **ret) {
        _cleanup_free_ અક્ષર *p = ખાલી, *s = ખાલી;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(path);
        પ્રતિજ્ઞા(suffix);
        પ્રતિજ્ઞા(ret);

        જો (!unit_suffix_is_valid(suffix))
                પાછા -EINVAL;

        r = unit_name_path_escape(path, &p);
        જો (r < 0)
                પાછા r;

        s = strjoin(p, suffix);
        જો (!s)
                પાછા -ENOMEM;

        જો (strlen(s) >= UNIT_NAME_MAX) {
                _cleanup_free_ અક્ષર *n = ખાલી;

                log_debug("Unit name \"%s\" too long, falling back to hashed unit name.", s);

                r = unit_name_hash_long(s, &n);
                જો (r < 0)
                        પાછા r;

                free_and_replace(s, n);
        }

        /* Refuse if this for some other reason didn't result in a valid name */
        જો (!unit_name_is_valid(s, UNIT_NAME_PLAIN))
                પાછા -EINVAL;

        *ret = TAKE_PTR(s);
        પાછા 0;
}

પૂર્ણાંક unit_name_from_path_instance(અચલ અક્ષર *prefix, અચલ અક્ષર *path, અચલ અક્ષર *suffix, અક્ષર **ret) {
        _cleanup_free_ અક્ષર *p = ખાલી, *s = ખાલી;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(prefix);
        પ્રતિજ્ઞા(path);
        પ્રતિજ્ઞા(suffix);
        પ્રતિજ્ઞા(ret);

        જો (!unit_prefix_is_valid(prefix))
                પાછા -EINVAL;

        જો (!unit_suffix_is_valid(suffix))
                પાછા -EINVAL;

        r = unit_name_path_escape(path, &p);
        જો (r < 0)
                પાછા r;

        s = strjoin(prefix, "@", p, suffix);
        જો (!s)
                પાછા -ENOMEM;

        જો (strlen(s) >= UNIT_NAME_MAX) /* Return a slightly more descriptive error for this specific condition */
                પાછા -ENAMETOOLONG;

        /* Refuse if this for some other reason didn't result in a valid name */
        જો (!unit_name_is_valid(s, UNIT_NAME_INSTANCE))
                પાછા -EINVAL;

        *ret = TAKE_PTR(s);
        પાછા 0;
}

પૂર્ણાંક unit_name_to_path(અચલ અક્ષર *name, અક્ષર **ret) {
        _cleanup_free_ અક્ષર *prefix = ખાલી;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(name);

        r = unit_name_to_prefix(name, &prefix);
        જો (r < 0)
                પાછા r;

        જો (unit_name_is_hashed(name))
                પાછા -ENAMETOOLONG;

        પાછા unit_name_path_unescape(prefix, ret);
}

સ્થિર bool do_escape_mangle(અચલ અક્ષર *f, bool allow_globs, અક્ષર *t) {
        અચલ અક્ષર *valid_chars;
        bool mangled = false;

        પ્રતિજ્ઞા(f);
        પ્રતિજ્ઞા(t);

        /* We'll only escape the obvious characters here, to play safe.
         *
         * Returns true if any characters were mangled, false otherwise.
         */

        valid_chars = allow_globs ? VALID_CHARS_GLOB : VALID_CHARS_WITH_AT;

        માટે (; *f; f++)
                જો (*f == '/') {
                        *(t++) = '-';
                        mangled = true;
                } નહીંતર જો (!strchr(valid_chars, *f)) {
                        t = do_escape_char(*f, t);
                        mangled = true;
                } નહીંતર
                        *(t++) = *f;
        *t = 0;

        પાછા mangled;
}

/**
 *  Convert a string to a unit name. /dev/blah is converted to dev-blah.device,
 *  /blah/blah is converted to blah-blah.mount, anything else is left alone,
 *  except that @suffix is appended if a valid unit suffix is not present.
 *
 *  If @allow_globs, globs characters are preserved. Otherwise, they are escaped.
 */
પૂર્ણાંક unit_name_mangle_with_suffix(
                અચલ અક્ષર *name,
                અચલ અક્ષર *operation,
                UnitNameMangle flags,
                અચલ અક્ષર *suffix,
                અક્ષર **ret) {

        _cleanup_free_ અક્ષર *s = ખાલી;
        bool mangled, suggest_escape = true, warn = FLAGS_SET(flags, UNIT_NAME_MANGLE_WARN);
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(name);
        પ્રતિજ્ઞા(suffix);
        પ્રતિજ્ઞા(ret);

        જો (isempty(name)) /* We cannot mangle empty unit names to become valid, sorry. */
                પાછા -EINVAL;

        જો (!unit_suffix_is_valid(suffix))
                પાછા -EINVAL;

        /* Already a fully valid unit name? If so, no mangling is necessary... */
        જો (unit_name_is_valid(name, UNIT_NAME_ANY)) {
                જો (FLAGS_SET(flags, UNIT_NAME_MANGLE_STRICT) && !endswith(name, suffix)) {
                        અચલ અક્ષર *e = ASSERT_PTR(strrchr(name, '.'));

                        પાછા log_full_errno(warn ? LOG_NOTICE : LOG_DEBUG,
                                              SYNTHETIC_ERRNO(EINVAL),
                                              "Unit name \"%s\" has unit type \"%s\", but \"%s\" is expected%s%s.",
                                              name, e + 1, suffix + 1,
                                              operation ? " " : "", strempty(operation));
                }

                જાઓ good;
        }

        /* Already a fully valid globbing expression? If so, no mangling is necessary either... */
        જો (string_is_glob(name) && in_charset(name, VALID_CHARS_GLOB)) {
                જો (FLAGS_SET(flags, UNIT_NAME_MANGLE_GLOB))
                        જાઓ good;
                log_full(warn ? LOG_NOTICE : LOG_DEBUG,
                         "Glob pattern passed%s%s, but globs are not supported for this.",
                         operation ? " " : "", strempty(operation));
                suggest_escape = false;
        }

        જો (path_is_absolute(name)) {
                _cleanup_free_ અક્ષર *n = ખાલી, *u = ખાલી;

                r = path_simplify_alloc(name, &n);
                જો (r < 0)
                        પાછા r;

                જો (is_device_path(n)) {
                        r = unit_name_from_path(n, ".device", &u);
                        જો (r >= 0) {
                                જો (FLAGS_SET(flags, UNIT_NAME_MANGLE_STRICT) && !streq(suffix, ".device"))
                                        પાછા log_full_errno(warn ? LOG_NOTICE : LOG_DEBUG,
                                                              SYNTHETIC_ERRNO(EINVAL),
                                                              "Path \"%s\" resolves to unit type \"device\", but \"%s\" is expected%s%s.",
                                                              name, suffix + 1,
                                                              operation ? " " : "", strempty(operation));

                                *ret = TAKE_PTR(u);
                                પાછા 1;
                        }
                        જો (r != -EINVAL)
                                પાછા r;
                }

                r = unit_name_from_path(n, ".mount", &u);
                જો (r >= 0) {
                        જો (FLAGS_SET(flags, UNIT_NAME_MANGLE_STRICT) && !streq(suffix, ".mount"))
                                પાછા log_full_errno(warn ? LOG_NOTICE : LOG_DEBUG,
                                                      SYNTHETIC_ERRNO(EINVAL),
                                                      "Path \"%s\" resolves to unit type \"mount\", but \"%s\" is expected%s%s.",
                                                      name, suffix + 1,
                                                      operation ? " " : "", strempty(operation));

                        *ret = TAKE_PTR(u);
                        પાછા 1;
                }
                જો (r != -EINVAL)
                        પાછા r;
        }

        s = new(અક્ષર, strlen(name) * 4 + strlen(suffix) + 1);
        જો (!s)
                પાછા -ENOMEM;

        mangled = do_escape_mangle(name, FLAGS_SET(flags, UNIT_NAME_MANGLE_GLOB), s);
        જો (mangled)
                log_full(warn ? LOG_NOTICE : LOG_DEBUG,
                         "Invalid unit name \"%s\" escaped as \"%s\"%s.",
                         name, s,
                         suggest_escape ? " (maybe you should use systemd-escape?)" : "");

        /* Append a suffix if it doesn't have any, but only if this is not a glob, so that we can allow
         * "foo.*" as a valid glob. */
        જો ((!FLAGS_SET(flags, UNIT_NAME_MANGLE_GLOB) || !string_is_glob(s)) && unit_name_to_type(s) < 0)
                strcat(s, suffix);

        /* Make sure mangling didn't grow this too large (but don't do this check if globbing is allowed,
         * since globs generally do not qualify as valid unit names) */
        જો (!FLAGS_SET(flags, UNIT_NAME_MANGLE_GLOB) && !unit_name_is_valid(s, UNIT_NAME_ANY))
                પાછા -EINVAL;

        *ret = TAKE_PTR(s);
        પાછા 1;

good:
        પાછા strdup_to(ret, name);
}

પૂર્ણાંક slice_build_parent_slice(અચલ અક્ષર *slice, અક્ષર **ret) {
        પ્રતિજ્ઞા(slice);
        પ્રતિજ્ઞા(ret);

        જો (!slice_name_is_valid(slice))
                પાછા -EINVAL;

        જો (streq(slice, SPECIAL_મૂળ_SLICE)) {
                *ret = ખાલી;
                પાછા 0;
        }

        _cleanup_free_ અક્ષર *s = strdup(slice);
        જો (!s)
                પાછા -ENOMEM;

        અક્ષર *dash = strrchr(s, '-');
        જો (!dash)
                પાછા strdup_to_full(ret, SPECIAL_મૂળ_SLICE);

        /* We know that s ended with .slice before truncation, so we have enough space. */
        strcpy(dash, ".slice");

        *ret = TAKE_PTR(s);
        પાછા 1;
}

પૂર્ણાંક slice_build_subslice(અચલ અક્ષર *slice, અચલ અક્ષર *name, અક્ષર **ret) {
        અક્ષર *subslice;

        પ્રતિજ્ઞા(slice);
        પ્રતિજ્ઞા(name);
        પ્રતિજ્ઞા(ret);

        જો (!slice_name_is_valid(slice))
                પાછા -EINVAL;

        જો (!unit_prefix_is_valid(name))
                પાછા -EINVAL;

        જો (streq(slice, SPECIAL_મૂળ_SLICE))
                subslice = strjoin(name, ".slice");
        નહીંતર {
                અચલ અક્ષર *e;

                assert_se(e = endswith(slice, ".slice"));

                subslice = new(અક્ષર, (e - slice) + 1 + strlen(name) + 6 + 1);
                જો (!subslice)
                        પાછા -ENOMEM;

                stpcpy(stpcpy(stpcpy(mempcpy(subslice, slice, e - slice), "-"), name), ".slice");
        }

        *ret = subslice;
        પાછા 0;
}

bool slice_name_is_valid(અચલ અક્ષર *name) {
        અચલ અક્ષર *p, *e;
        bool dash = false;

        જો (!unit_name_is_valid(name, UNIT_NAME_PLAIN))
                પાછા false;

        જો (streq(name, SPECIAL_મૂળ_SLICE))
                પાછા true;

        e = endswith(name, ".slice");
        જો (!e)
                પાછા false;

        માટે (p = name; p < e; p++) {

                જો (*p == '-') {

                        /* Don't allow initial dash */
                        જો (p == name)
                                પાછા false;

                        /* Don't allow multiple dashes */
                        જો (dash)
                                પાછા false;

                        dash = true;
                } નહીંતર
                        dash = false;
        }

        /* Don't allow trailing hash */
        જો (dash)
                પાછા false;

        પાછા true;
}

bool unit_name_prefix_equal(અચલ અક્ષર *a, અચલ અક્ષર *b) {
        અચલ અક્ષર *p, *q;

        પ્રતિજ્ઞા(a);
        પ્રતિજ્ઞા(b);

        જો (!unit_name_is_valid(a, UNIT_NAME_ANY) || !unit_name_is_valid(b, UNIT_NAME_ANY))
                પાછા false;

        p = strchr(a, '@');
        જો (!p)
                p = strrchr(a, '.');

        q = strchr(b, '@');
        જો (!q)
                q = strrchr(b, '.');

        પ્રતિજ્ઞા(p);
        પ્રતિજ્ઞા(q);

        પાછા memcmp_nn(a, p - a, b, q - b) == 0;
}
