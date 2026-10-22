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

        𐤀𐤌 (_unlikely_(flags == 0))
                أرجع false;

        𐤀𐤌 (isempty(n))
                أرجع false;

        𐤀𐤌 (strlen(n) >= UNIT_NAME_MAX)
                أرجع false;

        e = strrchr(n, '.');
        𐤀𐤌 (!e || e == n)
                أرجع false;

        𐤀𐤌 (unit_type_from_string(e + 1) < 0)
                أرجع false;

        لكل (i = n, at = عدم; i < e; i++) {

                𐤀𐤌 (*i == '@' && !at)
                        at = i;

                𐤀𐤌 (!strchr(VALID_CHARS_WITH_AT, *i))
                        أرجع false;
        }

        𐤀𐤌 (at == n)
                أرجع false;

        𐤀𐤌 (flags & UNIT_NAME_PLAIN)
                𐤀𐤌 (!at)
                        أرجع true;

        𐤀𐤌 (flags & UNIT_NAME_INSTANCE)
                𐤀𐤌 (at && e > at + 1)
                        أرجع true;

        𐤀𐤌 (flags & UNIT_NAME_TEMPLATE)
                𐤀𐤌 (at && e == at + 1)
                        أرجع true;

        أرجع false;
}

bool unit_prefix_is_valid(ثابت حرف *p) {

        /* We don't allow additional @ in the prefix string */

        𐤀𐤌 (isempty(p))
                أرجع false;

        أرجع in_charset(p, VALID_CHARS);
}

bool unit_instance_is_valid(ثابت حرف *i) {

        /* The max length depends on the length of the string, so we
         * don't really check this here. */

        𐤀𐤌 (isempty(i))
                أرجع false;

        /* We allow additional @ in the instance string, we do not
         * allow them in the prefix! */

        أرجع in_charset(i, "@" VALID_CHARS);
}

bool unit_suffix_is_valid(ثابت حرف *s) {
        𐤀𐤌 (isempty(s))
                أرجع false;

        𐤀𐤌 (s[0] != '.')
                أرجع false;

        𐤀𐤌 (unit_type_from_string(s + 1) < 0)
                أرجع false;

        أرجع true;
}

صحيح unit_name_to_prefix(ثابت حرف *n, حرف **ret) {
        ثابت حرف *p;
        حرف *s;

        أكد(n);
        أكد(ret);

        𐤀𐤌 (!unit_name_is_valid(n, UNIT_NAME_ANY))
                أرجع -EINVAL;

        p = strchr(n, '@');
        𐤀𐤌 (!p)
                p = strrchr(n, '.');

        assert_se(p);

        s = strndup(n, p - n);
        𐤀𐤌 (!s)
                أرجع -ENOMEM;

        *ret = s;
        أرجع 0;
}

UnitNameFlags unit_name_to_instance(ثابت حرف *n, حرف **ret) {
        ثابت حرف *p, *d;

        أكد(n);

        𐤀𐤌 (!unit_name_is_valid(n, UNIT_NAME_ANY))
                أرجع -EINVAL;

        /* Everything past the first @ and before the last . is the instance */
        p = strchr(n, '@');
        𐤀𐤌 (!p) {
                𐤀𐤌 (ret)
                        *ret = عدم;
                أرجع UNIT_NAME_PLAIN;
        }

        p++;

        d = strrchr(p, '.');
        𐤀𐤌 (!d)
                أرجع -EINVAL;

        𐤀𐤌 (ret) {
                حرف *i = strndup(p, d-p);
                𐤀𐤌 (!i)
                        أرجع -ENOMEM;

                *ret = i;
        }
        أرجع d > p ? UNIT_NAME_INSTANCE : UNIT_NAME_TEMPLATE;
}

صحيح unit_name_to_prefix_and_instance(ثابت حرف *n, حرف **ret) {
        ثابت حرف *d;
        حرف *s;

        أكد(n);
        أكد(ret);

        𐤀𐤌 (!unit_name_is_valid(n, UNIT_NAME_ANY))
                أرجع -EINVAL;

        d = strrchr(n, '.');
        𐤀𐤌 (!d)
                أرجع -EINVAL;

        s = strndup(n, d - n);
        𐤀𐤌 (!s)
                أرجع -ENOMEM;

        *ret = s;
        أرجع 0;
}

UnitType unit_name_to_type(ثابت حرف *n) {
        ثابت حرف *e;

        أكد(n);

        𐤀𐤌 (!unit_name_is_valid(n, UNIT_NAME_ANY))
                أرجع _UNIT_TYPE_INVALID;

        assert_se(e = strrchr(n, '.'));

        أرجع unit_type_from_string(e + 1);
}

صحيح unit_name_change_suffix(ثابت حرف *n, ثابت حرف *suffix, حرف **ret) {
        _cleanup_free_ حرف *s = عدم;
        size_t a, b;
        ثابت حرف *e;

        أكد(n);
        أكد(suffix);
        أكد(ret);

        𐤀𐤌 (!unit_name_is_valid(n, UNIT_NAME_ANY))
                أرجع -EINVAL;

        𐤀𐤌 (!unit_suffix_is_valid(suffix))
                أرجع -EINVAL;

        assert_se(e = strrchr(n, '.'));

        a = e - n;
        b = strlen(suffix);

        s = new(حرف, a + b + 1);
        𐤀𐤌 (!s)
                أرجع -ENOMEM;

        strcpy(mempcpy(s, n, a), suffix);

        /* Make sure the name is still valid (i.e. didn't grow too large due to longer suffix) */
        𐤀𐤌 (!unit_name_is_valid(s, UNIT_NAME_ANY))
                أرجع -EINVAL;

        *ret = TAKE_PTR(s);
        أرجع 0;
}

صحيح unit_name_build(ثابت حرف *prefix, ثابت حرف *instance, ثابت حرف *suffix, حرف **ret) {
        UnitType type;

        أكد(prefix);
        أكد(suffix);
        أكد(ret);

        𐤀𐤌 (suffix[0] != '.')
                أرجع -EINVAL;

        type = unit_type_from_string(suffix + 1);
        𐤀𐤌 (type < 0)
                أرجع type;

        أرجع unit_name_build_from_type(prefix, instance, type, ret);
}

صحيح unit_name_build_from_type(ثابت حرف *prefix, ثابت حرف *instance, UnitType type, حرف **ret) {
        _cleanup_free_ حرف *s = عدم;
        ثابت حرف *ut;

        أكد(prefix);
        أكد(type >= 0);
        أكد(type < _UNIT_TYPE_MAX);
        أكد(ret);

        𐤀𐤌 (!unit_prefix_is_valid(prefix))
                أرجع -EINVAL;

        ut = unit_type_to_string(type);

        𐤀𐤌 (instance) {
                𐤀𐤌 (!unit_instance_is_valid(instance))
                        أرجع -EINVAL;

                s = strjoin(prefix, "@", instance, ".", ut);
        } وإلا
                s = strjoin(prefix, ".", ut);
        𐤀𐤌 (!s)
                أرجع -ENOMEM;

        /* Verify that this didn't grow too large (or otherwise is invalid) */
        𐤀𐤌 (!unit_name_is_valid(s, instance ? UNIT_NAME_INSTANCE : UNIT_NAME_PLAIN))
                أرجع -EINVAL;

        *ret = TAKE_PTR(s);
        أرجع 0;
}

ساكن حرف *do_escape_char(حرف c, حرف *t) {
        أكد(t);

        *(t++) = '\\';
        *(t++) = 'x';
        *(t++) = hexchar(c >> 4);
        *(t++) = hexchar(c);

        أرجع t;
}

ساكن حرف *do_escape(ثابت حرف *f, حرف *t) {
        أكد(f);
        أكد(t);

        /* do not create units with a leading '.', like for "/.dotdir" mount points */
        𐤀𐤌 (*f == '.') {
                t = do_escape_char(*f, t);
                f++;
        }

        لكل (; *f; f++) {
                𐤀𐤌 (*f == '/')
                        *(t++) = '-';
                وإلا 𐤀𐤌 (IN_SET(*f, '-', '\\') || !strchr(VALID_CHARS, *f))
                        t = do_escape_char(*f, t);
                وإلا
                        *(t++) = *f;
        }

        أرجع t;
}

حرف* unit_name_escape(ثابت حرف *f) {
        حرف *r, *t;

        أكد(f);

        r = new(حرف, strlen(f)*4+1);
        𐤀𐤌 (!r)
                أرجع عدم;

        t = do_escape(f, r);
        *t = 0;

        أرجع r;
}

صحيح unit_name_unescape(ثابت حرف *f, حرف **ret) {
        _cleanup_free_ حرف *r = عدم;
        حرف *t;

        أكد(f);
        أكد(ret);

        r = strdup(f);
        𐤀𐤌 (!r)
                أرجع -ENOMEM;

        لكل (t = r; *f; f++) {
                𐤀𐤌 (*f == '-')
                        *(t++) = '/';
                وإلا 𐤀𐤌 (*f == '\\') {
                        صحيح a, b;

                        𐤀𐤌 (f[1] != 'x')
                                أرجع -EINVAL;

                        a = unhexchar(f[2]);
                        𐤀𐤌 (a < 0)
                                أرجع -EINVAL;

                        b = unhexchar(f[3]);
                        𐤀𐤌 (b < 0)
                                أرجع -EINVAL;

                        *(t++) = (حرف) (((uint8_t) a << 4U) | (uint8_t) b);
                        f += 3;
                } وإلا
                        *(t++) = *f;
        }

        *t = 0;

        *ret = TAKE_PTR(r);

        أرجع 0;
}

صحيح unit_name_path_escape(ثابت حرف *f, حرف **ret) {
        _cleanup_free_ حرف *p = عدم;
        حرف *s;
        صحيح r;

        أكد(f);
        أكد(ret);

        r = path_simplify_alloc(f, &p);
        𐤀𐤌 (r < 0)
                أرجع r;

        𐤀𐤌 (empty_or_root(p))
                s = strdup("-");
        وإلا {
                𐤀𐤌 (!path_is_normalized(p))
                        أرجع -EINVAL;

                /* Truncate trailing slashes and skip leading slashes */
                delete_trailing_chars(p, "/");
                s = unit_name_escape(skip_leading_chars(p, "/"));
        }
        𐤀𐤌 (!s)
                أرجع -ENOMEM;

        *ret = s;
        أرجع 0;
}

صحيح unit_name_path_unescape(ثابت حرف *f, حرف **ret) {
        _cleanup_free_ حرف *s = عدم;
        صحيح r;

        أكد(f);

        𐤀𐤌 (isempty(f))
                أرجع -EINVAL;

        𐤀𐤌 (streq(f, "-")) {
                s = strdup("/");
                𐤀𐤌 (!s)
                        أرجع -ENOMEM;
        } وإلا {
                _cleanup_free_ حرف *w = عدم;

                r = unit_name_unescape(f, &w);
                𐤀𐤌 (r < 0)
                        أرجع r;

                /* Don't accept trailing or leading slashes */
                𐤀𐤌 (startswith(w, "/") || endswith(w, "/"))
                        أرجع -EINVAL;

                /* Prefix a slash again */
                s = strjoin("/", w);
                𐤀𐤌 (!s)
                        أرجع -ENOMEM;

                𐤀𐤌 (!path_is_normalized(s))
                        أرجع -EINVAL;
        }

        𐤀𐤌 (ret)
                *ret = TAKE_PTR(s);

        أرجع 0;
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

        𐤀𐤌 (!unit_name_is_valid(original, UNIT_NAME_INSTANCE|UNIT_NAME_TEMPLATE))
                أرجع -EINVAL;
        𐤀𐤌 (!unit_instance_is_valid(instance) && !(accept_glob && in_charset(instance, VALID_CHARS_GLOB)))
                أرجع -EINVAL;

        prefix = ASSERT_PTR(strchr(original, '@'));
        suffix = ASSERT_PTR(strrchr(original, '.'));
        أكد(prefix < suffix);

        pl = prefix - original + 1; /* include '@' */

        s = new(حرف, pl + strlen(instance) + strlen(suffix) + 1);
        𐤀𐤌 (!s)
                أرجع -ENOMEM;

#𐤀𐤌 HAS_FEATURE_MEMORY_SANITIZER
        /* MSan doesn't like stpncpy... See also https://github.com/google/sanitizers/issues/926 */
        memzero(s, pl + strlen(instance) + strlen(suffix) + 1);
#endif

        strcpy(stpcpy(stpncpy(s, original, pl), instance), suffix);

        /* Make sure the resulting name still is valid, i.e. didn't grow too large. Globs will be expanded
         * by clients when used, so the check is pointless. */
        𐤀𐤌 (!accept_glob && !unit_name_is_valid(s, UNIT_NAME_INSTANCE))
                أرجع -EINVAL;

        *ret = TAKE_PTR(s);
        أرجع 0;
}

صحيح unit_name_template(ثابت حرف *f, حرف **ret) {
        ثابت حرف *p, *e;
        حرف *s;
        size_t a;

        أكد(f);
        أكد(ret);

        𐤀𐤌 (!unit_name_is_valid(f, UNIT_NAME_INSTANCE|UNIT_NAME_TEMPLATE))
                أرجع -EINVAL;

        assert_se(p = strchr(f, '@'));
        assert_se(e = strrchr(f, '.'));

        a = p - f;

        s = new(حرف, a + 1 + strlen(e) + 1);
        𐤀𐤌 (!s)
                أرجع -ENOMEM;

        strcpy(mempcpy(s, f, a + 1), e);

        *ret = s;
        أرجع 0;
}

bool unit_name_is_hashed(ثابت حرف *name) {
        ثابت حرف *s;

        𐤀𐤌 (!unit_name_is_valid(name, UNIT_NAME_PLAIN))
                أرجع false;

        assert_se(s = strrchr(name, '.'));

        𐤀𐤌 (s - name < UNIT_NAME_HASH_LENGTH_CHARS + 1)
                أرجع false;

        s -= UNIT_NAME_HASH_LENGTH_CHARS;
        𐤀𐤌 (s[-1] != '_')
                أرجع false;

        لكل (size_t i = 0; i < UNIT_NAME_HASH_LENGTH_CHARS; i++)
                𐤀𐤌 (!strchr(LOWERCASE_HEXDIGITS, s[i]))
                        أرجع false;

        أرجع true;
}

صحيح unit_name_hash_long(ثابت حرف *name, حرف **ret) {
        _cleanup_free_ حرف *n = عدم, *hash = عدم;
        ثابت حرف *suffix;
        le64_t h;
        size_t len;

        أكد(ret);

        𐤀𐤌 (strlen(name) < UNIT_NAME_MAX)
                أرجع -EMSGSIZE;

        suffix = strrchr(name, '.');
        𐤀𐤌 (!suffix)
                أرجع -EINVAL;

        𐤀𐤌 (unit_type_from_string(suffix+1) < 0)
                أرجع -EINVAL;

        h = htole64(siphash24_string(name, LONG_UNIT_NAME_HASH_KEY.bytes));

        hash = hexmem(&h, حجم(h));
        𐤀𐤌 (!hash)
                أرجع -ENOMEM;

        assert_se(strlen(hash) == UNIT_NAME_HASH_LENGTH_CHARS);

        len = UNIT_NAME_MAX - 1 - strlen(suffix+1) - UNIT_NAME_HASH_LENGTH_CHARS - 2;
        أكد(len > 0 && len < UNIT_NAME_MAX);

        n = strndup(name, len);
        𐤀𐤌 (!n)
                أرجع -ENOMEM;

        𐤀𐤌 (!strextend(&n, "_", hash, suffix))
                أرجع -ENOMEM;
        assert_se(unit_name_is_valid(n, UNIT_NAME_PLAIN));

        *ret = TAKE_PTR(n);

        أرجع 0;
}

صحيح unit_name_from_path(ثابت حرف *path, ثابت حرف *suffix, حرف **ret) {
        _cleanup_free_ حرف *p = عدم, *s = عدم;
        صحيح r;

        أكد(path);
        أكد(suffix);
        أكد(ret);

        𐤀𐤌 (!unit_suffix_is_valid(suffix))
                أرجع -EINVAL;

        r = unit_name_path_escape(path, &p);
        𐤀𐤌 (r < 0)
                أرجع r;

        s = strjoin(p, suffix);
        𐤀𐤌 (!s)
                أرجع -ENOMEM;

        𐤀𐤌 (strlen(s) >= UNIT_NAME_MAX) {
                _cleanup_free_ حرف *n = عدم;

                log_debug("Unit name \"%s\" too long, falling back to hashed unit name.", s);

                r = unit_name_hash_long(s, &n);
                𐤀𐤌 (r < 0)
                        أرجع r;

                free_and_replace(s, n);
        }

        /* Refuse if this for some other reason didn't result in a valid name */
        𐤀𐤌 (!unit_name_is_valid(s, UNIT_NAME_PLAIN))
                أرجع -EINVAL;

        *ret = TAKE_PTR(s);
        أرجع 0;
}

صحيح unit_name_from_path_instance(ثابت حرف *prefix, ثابت حرف *path, ثابت حرف *suffix, حرف **ret) {
        _cleanup_free_ حرف *p = عدم, *s = عدم;
        صحيح r;

        أكد(prefix);
        أكد(path);
        أكد(suffix);
        أكد(ret);

        𐤀𐤌 (!unit_prefix_is_valid(prefix))
                أرجع -EINVAL;

        𐤀𐤌 (!unit_suffix_is_valid(suffix))
                أرجع -EINVAL;

        r = unit_name_path_escape(path, &p);
        𐤀𐤌 (r < 0)
                أرجع r;

        s = strjoin(prefix, "@", p, suffix);
        𐤀𐤌 (!s)
                أرجع -ENOMEM;

        𐤀𐤌 (strlen(s) >= UNIT_NAME_MAX) /* Return a slightly more descriptive error for this specific condition */
                أرجع -ENAMETOOLONG;

        /* Refuse if this for some other reason didn't result in a valid name */
        𐤀𐤌 (!unit_name_is_valid(s, UNIT_NAME_INSTANCE))
                أرجع -EINVAL;

        *ret = TAKE_PTR(s);
        أرجع 0;
}

صحيح unit_name_to_path(ثابت حرف *name, حرف **ret) {
        _cleanup_free_ حرف *prefix = عدم;
        صحيح r;

        أكد(name);

        r = unit_name_to_prefix(name, &prefix);
        𐤀𐤌 (r < 0)
                أرجع r;

        𐤀𐤌 (unit_name_is_hashed(name))
                أرجع -ENAMETOOLONG;

        أرجع unit_name_path_unescape(prefix, ret);
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
                𐤀𐤌 (*f == '/') {
                        *(t++) = '-';
                        mangled = true;
                } وإلا 𐤀𐤌 (!strchr(valid_chars, *f)) {
                        t = do_escape_char(*f, t);
                        mangled = true;
                } وإلا
                        *(t++) = *f;
        *t = 0;

        أرجع mangled;
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

        𐤀𐤌 (isempty(name)) /* We cannot mangle empty unit names to become valid, sorry. */
                أرجع -EINVAL;

        𐤀𐤌 (!unit_suffix_is_valid(suffix))
                أرجع -EINVAL;

        /* Already a fully valid unit name? If so, no mangling is necessary... */
        𐤀𐤌 (unit_name_is_valid(name, UNIT_NAME_ANY)) {
                𐤀𐤌 (FLAGS_SET(flags, UNIT_NAME_MANGLE_STRICT) && !endswith(name, suffix)) {
                        ثابت حرف *e = ASSERT_PTR(strrchr(name, '.'));

                        أرجع log_full_errno(warn ? LOG_NOTICE : LOG_DEBUG,
                                              SYNTHETIC_ERRNO(EINVAL),
                                              "Unit name \"%s\" has unit type \"%s\", but \"%s\" is expected%s%s.",
                                              name, e + 1, suffix + 1,
                                              operation ? " " : "", strempty(operation));
                }

                اذهب good;
        }

        /* Already a fully valid globbing expression? If so, no mangling is necessary either... */
        𐤀𐤌 (string_is_glob(name) && in_charset(name, VALID_CHARS_GLOB)) {
                𐤀𐤌 (FLAGS_SET(flags, UNIT_NAME_MANGLE_GLOB))
                        اذهب good;
                log_full(warn ? LOG_NOTICE : LOG_DEBUG,
                         "Glob pattern passed%s%s, but globs are not supported for this.",
                         operation ? " " : "", strempty(operation));
                suggest_escape = false;
        }

        𐤀𐤌 (path_is_absolute(name)) {
                _cleanup_free_ حرف *n = عدم, *u = عدم;

                r = path_simplify_alloc(name, &n);
                𐤀𐤌 (r < 0)
                        أرجع r;

                𐤀𐤌 (is_device_path(n)) {
                        r = unit_name_from_path(n, ".device", &u);
                        𐤀𐤌 (r >= 0) {
                                𐤀𐤌 (FLAGS_SET(flags, UNIT_NAME_MANGLE_STRICT) && !streq(suffix, ".device"))
                                        أرجع log_full_errno(warn ? LOG_NOTICE : LOG_DEBUG,
                                                              SYNTHETIC_ERRNO(EINVAL),
                                                              "Path \"%s\" resolves to unit type \"device\", but \"%s\" is expected%s%s.",
                                                              name, suffix + 1,
                                                              operation ? " " : "", strempty(operation));

                                *ret = TAKE_PTR(u);
                                أرجع 1;
                        }
                        𐤀𐤌 (r != -EINVAL)
                                أرجع r;
                }

                r = unit_name_from_path(n, ".mount", &u);
                𐤀𐤌 (r >= 0) {
                        𐤀𐤌 (FLAGS_SET(flags, UNIT_NAME_MANGLE_STRICT) && !streq(suffix, ".mount"))
                                أرجع log_full_errno(warn ? LOG_NOTICE : LOG_DEBUG,
                                                      SYNTHETIC_ERRNO(EINVAL),
                                                      "Path \"%s\" resolves to unit type \"mount\", but \"%s\" is expected%s%s.",
                                                      name, suffix + 1,
                                                      operation ? " " : "", strempty(operation));

                        *ret = TAKE_PTR(u);
                        أرجع 1;
                }
                𐤀𐤌 (r != -EINVAL)
                        أرجع r;
        }

        s = new(حرف, strlen(name) * 4 + strlen(suffix) + 1);
        𐤀𐤌 (!s)
                أرجع -ENOMEM;

        mangled = do_escape_mangle(name, FLAGS_SET(flags, UNIT_NAME_MANGLE_GLOB), s);
        𐤀𐤌 (mangled)
                log_full(warn ? LOG_NOTICE : LOG_DEBUG,
                         "Invalid unit name \"%s\" escaped as \"%s\"%s.",
                         name, s,
                         suggest_escape ? " (maybe you should use systemd-escape?)" : "");

        /* Append a suffix if it doesn't have any, but only if this is not a glob, so that we can allow
         * "foo.*" as a valid glob. */
        𐤀𐤌 ((!FLAGS_SET(flags, UNIT_NAME_MANGLE_GLOB) || !string_is_glob(s)) && unit_name_to_type(s) < 0)
                strcat(s, suffix);

        /* Make sure mangling didn't grow this too large (but don't do this check if globbing is allowed,
         * since globs generally do not qualify as valid unit names) */
        𐤀𐤌 (!FLAGS_SET(flags, UNIT_NAME_MANGLE_GLOB) && !unit_name_is_valid(s, UNIT_NAME_ANY))
                أرجع -EINVAL;

        *ret = TAKE_PTR(s);
        أرجع 1;

good:
        أرجع strdup_to(ret, name);
}

صحيح slice_build_parent_slice(ثابت حرف *slice, حرف **ret) {
        أكد(slice);
        أكد(ret);

        𐤀𐤌 (!slice_name_is_valid(slice))
                أرجع -EINVAL;

        𐤀𐤌 (streq(slice, SPECIAL_ROOT_SLICE)) {
                *ret = عدم;
                أرجع 0;
        }

        _cleanup_free_ حرف *s = strdup(slice);
        𐤀𐤌 (!s)
                أرجع -ENOMEM;

        حرف *dash = strrchr(s, '-');
        𐤀𐤌 (!dash)
                أرجع strdup_to_full(ret, SPECIAL_ROOT_SLICE);

        /* We know that s ended with .slice before truncation, so we have enough space. */
        strcpy(dash, ".slice");

        *ret = TAKE_PTR(s);
        أرجع 1;
}

صحيح slice_build_subslice(ثابت حرف *slice, ثابت حرف *name, حرف **ret) {
        حرف *subslice;

        أكد(slice);
        أكد(name);
        أكد(ret);

        𐤀𐤌 (!slice_name_is_valid(slice))
                أرجع -EINVAL;

        𐤀𐤌 (!unit_prefix_is_valid(name))
                أرجع -EINVAL;

        𐤀𐤌 (streq(slice, SPECIAL_ROOT_SLICE))
                subslice = strjoin(name, ".slice");
        وإلا {
                ثابت حرف *e;

                assert_se(e = endswith(slice, ".slice"));

                subslice = new(حرف, (e - slice) + 1 + strlen(name) + 6 + 1);
                𐤀𐤌 (!subslice)
                        أرجع -ENOMEM;

                stpcpy(stpcpy(stpcpy(mempcpy(subslice, slice, e - slice), "-"), name), ".slice");
        }

        *ret = subslice;
        أرجع 0;
}

bool slice_name_is_valid(ثابت حرف *name) {
        ثابت حرف *p, *e;
        bool dash = false;

        𐤀𐤌 (!unit_name_is_valid(name, UNIT_NAME_PLAIN))
                أرجع false;

        𐤀𐤌 (streq(name, SPECIAL_ROOT_SLICE))
                أرجع true;

        e = endswith(name, ".slice");
        𐤀𐤌 (!e)
                أرجع false;

        لكل (p = name; p < e; p++) {

                𐤀𐤌 (*p == '-') {

                        /* Don't allow initial dash */
                        𐤀𐤌 (p == name)
                                أرجع false;

                        /* Don't allow multiple dashes */
                        𐤀𐤌 (dash)
                                أرجع false;

                        dash = true;
                } وإلا
                        dash = false;
        }

        /* Don't allow trailing hash */
        𐤀𐤌 (dash)
                أرجع false;

        أرجع true;
}

bool unit_name_prefix_equal(ثابت حرف *a, ثابت حرف *b) {
        ثابت حرف *p, *q;

        أكد(a);
        أكد(b);

        𐤀𐤌 (!unit_name_is_valid(a, UNIT_NAME_ANY) || !unit_name_is_valid(b, UNIT_NAME_ANY))
                أرجع false;

        p = strchr(a, '@');
        𐤀𐤌 (!p)
                p = strrchr(a, '.');

        q = strchr(b, '@');
        𐤀𐤌 (!q)
                q = strrchr(b, '.');

        أكد(p);
        أكد(q);

        أرجع memcmp_nn(a, p - a, b, q - b) == 0;
}
