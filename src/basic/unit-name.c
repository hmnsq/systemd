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

#define LONG_UNIT_NAME_HASH_مفتاح SD_ID128_MAKE(ec,f2,37,fb,58,32,4a,32,84,9f,06,9b,0d,21,eb,9a)
#define UNIT_NAME_HASH_LENGTH_CHARS 16

bool unit_name_is_valid(ثابت حرف *n, UnitNameFlags flags) {
        ثابت حرف *e, *i, *at;

        أكد((flags & ~(UNIT_NAME_PLAIN|UNIT_NAME_INSTANCE|UNIT_NAME_TEMPLATE)) == 0);

        إذا (_unlikely_(flags == 0))
                إرجاع false;

        إذا (isempty(n))
                إرجاع false;

        إذا (strlen(n) >= UNIT_NAME_MAX)
                إرجاع false;

        e = strrchr(n, '.');
        إذا (!e || e == n)
                إرجاع false;

        إذا (unit_type_from_string(e + 1) < 0)
                إرجاع false;

        لكل (i = n, at = عدم; i < e; i++) {

                إذا (*i == '@' && !at)
                        at = i;

                إذا (!strchr(VALID_CHARS_WITH_AT, *i))
                        إرجاع false;
        }

        إذا (at == n)
                إرجاع false;

        إذا (flags & UNIT_NAME_PLAIN)
                إذا (!at)
                        إرجاع true;

        إذا (flags & UNIT_NAME_INSTANCE)
                إذا (at && e > at + 1)
                        إرجاع true;

        إذا (flags & UNIT_NAME_TEMPLATE)
                إذا (at && e == at + 1)
                        إرجاع true;

        إرجاع false;
}

bool unit_prefix_is_valid(ثابت حرف *p) {

        /* We don't allow additional @ in the prefix string */

        إذا (isempty(p))
                إرجاع false;

        إرجاع in_charset(p, VALID_CHARS);
}

bool unit_instance_is_valid(ثابت حرف *i) {

        /* The max length depends on the length of the string, so we
         * don't really check this here. */

        إذا (isempty(i))
                إرجاع false;

        /* We allow additional @ in the instance string, we do not
         * allow them in the prefix! */

        إرجاع in_charset(i, "@" VALID_CHARS);
}

bool unit_suffix_is_valid(ثابت حرف *s) {
        إذا (isempty(s))
                إرجاع false;

        إذا (s[0] != '.')
                إرجاع false;

        إذا (unit_type_from_string(s + 1) < 0)
                إرجاع false;

        إرجاع true;
}

صحيح unit_name_to_prefix(ثابت حرف *n, حرف **ret) {
        ثابت حرف *p;
        حرف *s;

        أكد(n);
        أكد(ret);

        إذا (!unit_name_is_valid(n, UNIT_NAME_ANY))
                إرجاع -EINVAL;

        p = strchr(n, '@');
        إذا (!p)
                p = strrchr(n, '.');

        assert_se(p);

        s = strndup(n, p - n);
        إذا (!s)
                إرجاع -ENOMEM;

        *ret = s;
        إرجاع 0;
}

UnitNameFlags unit_name_to_instance(ثابت حرف *n, حرف **ret) {
        ثابت حرف *p, *d;

        أكد(n);

        إذا (!unit_name_is_valid(n, UNIT_NAME_ANY))
                إرجاع -EINVAL;

        /* Everything past the first @ and before the last . is the instance */
        p = strchr(n, '@');
        إذا (!p) {
                إذا (ret)
                        *ret = عدم;
                إرجاع UNIT_NAME_PLAIN;
        }

        p++;

        d = strrchr(p, '.');
        إذا (!d)
                إرجاع -EINVAL;

        إذا (ret) {
                حرف *i = strndup(p, d-p);
                إذا (!i)
                        إرجاع -ENOMEM;

                *ret = i;
        }
        إرجاع d > p ? UNIT_NAME_INSTANCE : UNIT_NAME_TEMPLATE;
}

صحيح unit_name_to_prefix_and_instance(ثابت حرف *n, حرف **ret) {
        ثابت حرف *d;
        حرف *s;

        أكد(n);
        أكد(ret);

        إذا (!unit_name_is_valid(n, UNIT_NAME_ANY))
                إرجاع -EINVAL;

        d = strrchr(n, '.');
        إذا (!d)
                إرجاع -EINVAL;

        s = strndup(n, d - n);
        إذا (!s)
                إرجاع -ENOMEM;

        *ret = s;
        إرجاع 0;
}

UnitType unit_name_to_type(ثابت حرف *n) {
        ثابت حرف *e;

        أكد(n);

        إذا (!unit_name_is_valid(n, UNIT_NAME_ANY))
                إرجاع _UNIT_TYPE_INVALID;

        assert_se(e = strrchr(n, '.'));

        إرجاع unit_type_from_string(e + 1);
}

صحيح unit_name_change_suffix(ثابت حرف *n, ثابت حرف *suffix, حرف **ret) {
        _cleanup_free_ حرف *s = عدم;
        size_t a, b;
        ثابت حرف *e;

        أكد(n);
        أكد(suffix);
        أكد(ret);

        إذا (!unit_name_is_valid(n, UNIT_NAME_ANY))
                إرجاع -EINVAL;

        إذا (!unit_suffix_is_valid(suffix))
                إرجاع -EINVAL;

        assert_se(e = strrchr(n, '.'));

        a = e - n;
        b = strlen(suffix);

        s = new(حرف, a + b + 1);
        إذا (!s)
                إرجاع -ENOMEM;

        strcpy(mempcpy(s, n, a), suffix);

        /* Make sure the name is still valid (i.e. didn't grow too large due to longer suffix) */
        إذا (!unit_name_is_valid(s, UNIT_NAME_ANY))
                إرجاع -EINVAL;

        *ret = TAKE_PTR(s);
        إرجاع 0;
}

صحيح unit_name_build(ثابت حرف *prefix, ثابت حرف *instance, ثابت حرف *suffix, حرف **ret) {
        UnitType type;

        أكد(prefix);
        أكد(suffix);
        أكد(ret);

        إذا (suffix[0] != '.')
                إرجاع -EINVAL;

        type = unit_type_from_string(suffix + 1);
        إذا (type < 0)
                إرجاع type;

        إرجاع unit_name_build_from_type(prefix, instance, type, ret);
}

صحيح unit_name_build_from_type(ثابت حرف *prefix, ثابت حرف *instance, UnitType type, حرف **ret) {
        _cleanup_free_ حرف *s = عدم;
        ثابت حرف *ut;

        أكد(prefix);
        أكد(type >= 0);
        أكد(type < _UNIT_TYPE_MAX);
        أكد(ret);

        إذا (!unit_prefix_is_valid(prefix))
                إرجاع -EINVAL;

        ut = unit_type_to_string(type);

        إذا (instance) {
                إذا (!unit_instance_is_valid(instance))
                        إرجاع -EINVAL;

                s = strjoin(prefix, "@", instance, ".", ut);
        } وإلا
                s = strjoin(prefix, ".", ut);
        إذا (!s)
                إرجاع -ENOMEM;

        /* Verify that this didn't grow too large (or otherwise is invalid) */
        إذا (!unit_name_is_valid(s, instance ? UNIT_NAME_INSTANCE : UNIT_NAME_PLAIN))
                إرجاع -EINVAL;

        *ret = TAKE_PTR(s);
        إرجاع 0;
}

ساكن حرف *do_escape_char(حرف c, حرف *t) {
        أكد(t);

        *(t++) = '\\';
        *(t++) = 'x';
        *(t++) = hexchar(c >> 4);
        *(t++) = hexchar(c);

        إرجاع t;
}

ساكن حرف *do_escape(ثابت حرف *f, حرف *t) {
        أكد(f);
        أكد(t);

        /* do not create units with a leading '.', like for "/.dotdir" mount points */
        إذا (*f == '.') {
                t = do_escape_char(*f, t);
                f++;
        }

        لكل (; *f; f++) {
                إذا (*f == '/')
                        *(t++) = '-';
                وإلا إذا (IN_SET(*f, '-', '\\') || !strchr(VALID_CHARS, *f))
                        t = do_escape_char(*f, t);
                وإلا
                        *(t++) = *f;
        }

        إرجاع t;
}

حرف* unit_name_escape(ثابت حرف *f) {
        حرف *r, *t;

        أكد(f);

        r = new(حرف, strlen(f)*4+1);
        إذا (!r)
                إرجاع عدم;

        t = do_escape(f, r);
        *t = 0;

        إرجاع r;
}

صحيح unit_name_unescape(ثابت حرف *f, حرف **ret) {
        _cleanup_free_ حرف *r = عدم;
        حرف *t;

        أكد(f);
        أكد(ret);

        r = strdup(f);
        إذا (!r)
                إرجاع -ENOMEM;

        لكل (t = r; *f; f++) {
                إذا (*f == '-')
                        *(t++) = '/';
                وإلا إذا (*f == '\\') {
                        صحيح a, b;

                        إذا (f[1] != 'x')
                                إرجاع -EINVAL;

                        a = unhexchar(f[2]);
                        إذا (a < 0)
                                إرجاع -EINVAL;

                        b = unhexchar(f[3]);
                        إذا (b < 0)
                                إرجاع -EINVAL;

                        *(t++) = (حرف) (((uint8_t) a << 4U) | (uint8_t) b);
                        f += 3;
                } وإلا
                        *(t++) = *f;
        }

        *t = 0;

        *ret = TAKE_PTR(r);

        إرجاع 0;
}

صحيح unit_name_path_escape(ثابت حرف *f, حرف **ret) {
        _cleanup_free_ حرف *p = عدم;
        حرف *s;
        صحيح r;

        أكد(f);
        أكد(ret);

        r = path_simplify_alloc(f, &p);
        إذا (r < 0)
                إرجاع r;

        إذا (empty_or_جذر(p))
                s = strdup("-");
        وإلا {
                إذا (!path_is_normalized(p))
                        إرجاع -EINVAL;

                /* Truncate trailing slashes and skip leading slashes */
                delete_trailing_chars(p, "/");
                s = unit_name_escape(skip_leading_chars(p, "/"));
        }
        إذا (!s)
                إرجاع -ENOMEM;

        *ret = s;
        إرجاع 0;
}

صحيح unit_name_path_unescape(ثابت حرف *f, حرف **ret) {
        _cleanup_free_ حرف *s = عدم;
        صحيح r;

        أكد(f);

        إذا (isempty(f))
                إرجاع -EINVAL;

        إذا (streq(f, "-")) {
                s = strdup("/");
                إذا (!s)
                        إرجاع -ENOMEM;
        } وإلا {
                _cleanup_free_ حرف *w = عدم;

                r = unit_name_unescape(f, &w);
                إذا (r < 0)
                        إرجاع r;

                /* Don't accept trailing or leading slashes */
                إذا (startswith(w, "/") || endswith(w, "/"))
                        إرجاع -EINVAL;

                /* Prefix a slash again */
                s = strjoin("/", w);
                إذا (!s)
                        إرجاع -ENOMEM;

                إذا (!path_is_normalized(s))
                        إرجاع -EINVAL;
        }

        إذا (ret)
                *ret = TAKE_PTR(s);

        إرجاع 0;
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

        إذا (!unit_name_is_valid(original, UNIT_NAME_INSTANCE|UNIT_NAME_TEMPLATE))
                إرجاع -EINVAL;
        إذا (!unit_instance_is_valid(instance) && !(accept_glob && in_charset(instance, VALID_CHARS_GLOB)))
                إرجاع -EINVAL;

        prefix = ASSERT_PTR(strchr(original, '@'));
        suffix = ASSERT_PTR(strrchr(original, '.'));
        أكد(prefix < suffix);

        pl = prefix - original + 1; /* include '@' */

        s = new(حرف, pl + strlen(instance) + strlen(suffix) + 1);
        إذا (!s)
                إرجاع -ENOMEM;

#إذا HAS_FEATURE_MEMORY_SANITIZER
        /* MSan doesn't like stpncpy... See also https://github.com/google/sanitizers/issues/926 */
        memzero(s, pl + strlen(instance) + strlen(suffix) + 1);
#endif

        strcpy(stpcpy(stpncpy(s, original, pl), instance), suffix);

        /* Make sure the resulting name still is valid, i.e. didn't grow too large. Globs will be expanded
         * by clients when used, so the check is pointless. */
        إذا (!accept_glob && !unit_name_is_valid(s, UNIT_NAME_INSTANCE))
                إرجاع -EINVAL;

        *ret = TAKE_PTR(s);
        إرجاع 0;
}

صحيح unit_name_template(ثابت حرف *f, حرف **ret) {
        ثابت حرف *p, *e;
        حرف *s;
        size_t a;

        أكد(f);
        أكد(ret);

        إذا (!unit_name_is_valid(f, UNIT_NAME_INSTANCE|UNIT_NAME_TEMPLATE))
                إرجاع -EINVAL;

        assert_se(p = strchr(f, '@'));
        assert_se(e = strrchr(f, '.'));

        a = p - f;

        s = new(حرف, a + 1 + strlen(e) + 1);
        إذا (!s)
                إرجاع -ENOMEM;

        strcpy(mempcpy(s, f, a + 1), e);

        *ret = s;
        إرجاع 0;
}

bool unit_name_is_hashed(ثابت حرف *name) {
        ثابت حرف *s;

        إذا (!unit_name_is_valid(name, UNIT_NAME_PLAIN))
                إرجاع false;

        assert_se(s = strrchr(name, '.'));

        إذا (s - name < UNIT_NAME_HASH_LENGTH_CHARS + 1)
                إرجاع false;

        s -= UNIT_NAME_HASH_LENGTH_CHARS;
        إذا (s[-1] != '_')
                إرجاع false;

        لكل (size_t i = 0; i < UNIT_NAME_HASH_LENGTH_CHARS; i++)
                إذا (!strchr(LOWERCASE_HEXDIGITS, s[i]))
                        إرجاع false;

        إرجاع true;
}

صحيح unit_name_hash_long(ثابت حرف *name, حرف **ret) {
        _cleanup_free_ حرف *n = عدم, *hash = عدم;
        ثابت حرف *suffix;
        le64_t h;
        size_t len;

        أكد(ret);

        إذا (strlen(name) < UNIT_NAME_MAX)
                إرجاع -EMSGSIZE;

        suffix = strrchr(name, '.');
        إذا (!suffix)
                إرجاع -EINVAL;

        إذا (unit_type_from_string(suffix+1) < 0)
                إرجاع -EINVAL;

        h = htole64(siphash24_string(name, LONG_UNIT_NAME_HASH_مفتاح.bytes));

        hash = hexmem(&h, حجم(h));
        إذا (!hash)
                إرجاع -ENOMEM;

        assert_se(strlen(hash) == UNIT_NAME_HASH_LENGTH_CHARS);

        len = UNIT_NAME_MAX - 1 - strlen(suffix+1) - UNIT_NAME_HASH_LENGTH_CHARS - 2;
        أكد(len > 0 && len < UNIT_NAME_MAX);

        n = strndup(name, len);
        إذا (!n)
                إرجاع -ENOMEM;

        إذا (!strextend(&n, "_", hash, suffix))
                إرجاع -ENOMEM;
        assert_se(unit_name_is_valid(n, UNIT_NAME_PLAIN));

        *ret = TAKE_PTR(n);

        إرجاع 0;
}

صحيح unit_name_from_path(ثابت حرف *path, ثابت حرف *suffix, حرف **ret) {
        _cleanup_free_ حرف *p = عدم, *s = عدم;
        صحيح r;

        أكد(path);
        أكد(suffix);
        أكد(ret);

        إذا (!unit_suffix_is_valid(suffix))
                إرجاع -EINVAL;

        r = unit_name_path_escape(path, &p);
        إذا (r < 0)
                إرجاع r;

        s = strjoin(p, suffix);
        إذا (!s)
                إرجاع -ENOMEM;

        إذا (strlen(s) >= UNIT_NAME_MAX) {
                _cleanup_free_ حرف *n = عدم;

                log_debug("Unit name \"%s\" too long, falling back to hashed unit name.", s);

                r = unit_name_hash_long(s, &n);
                إذا (r < 0)
                        إرجاع r;

                free_and_replace(s, n);
        }

        /* Refuse if this for some other reason didn't result in a valid name */
        إذا (!unit_name_is_valid(s, UNIT_NAME_PLAIN))
                إرجاع -EINVAL;

        *ret = TAKE_PTR(s);
        إرجاع 0;
}

صحيح unit_name_from_path_instance(ثابت حرف *prefix, ثابت حرف *path, ثابت حرف *suffix, حرف **ret) {
        _cleanup_free_ حرف *p = عدم, *s = عدم;
        صحيح r;

        أكد(prefix);
        أكد(path);
        أكد(suffix);
        أكد(ret);

        إذا (!unit_prefix_is_valid(prefix))
                إرجاع -EINVAL;

        إذا (!unit_suffix_is_valid(suffix))
                إرجاع -EINVAL;

        r = unit_name_path_escape(path, &p);
        إذا (r < 0)
                إرجاع r;

        s = strjoin(prefix, "@", p, suffix);
        إذا (!s)
                إرجاع -ENOMEM;

        إذا (strlen(s) >= UNIT_NAME_MAX) /* Return a slightly more descriptive error for this specific condition */
                إرجاع -ENAMETOOLONG;

        /* Refuse if this for some other reason didn't result in a valid name */
        إذا (!unit_name_is_valid(s, UNIT_NAME_INSTANCE))
                إرجاع -EINVAL;

        *ret = TAKE_PTR(s);
        إرجاع 0;
}

صحيح unit_name_to_path(ثابت حرف *name, حرف **ret) {
        _cleanup_free_ حرف *prefix = عدم;
        صحيح r;

        أكد(name);

        r = unit_name_to_prefix(name, &prefix);
        إذا (r < 0)
                إرجاع r;

        إذا (unit_name_is_hashed(name))
                إرجاع -ENAMETOOLONG;

        إرجاع unit_name_path_unescape(prefix, ret);
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
                إذا (*f == '/') {
                        *(t++) = '-';
                        mangled = true;
                } وإلا إذا (!strchr(valid_chars, *f)) {
                        t = do_escape_char(*f, t);
                        mangled = true;
                } وإلا
                        *(t++) = *f;
        *t = 0;

        إرجاع mangled;
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

        إذا (isempty(name)) /* We cannot mangle empty unit names to become valid, sorry. */
                إرجاع -EINVAL;

        إذا (!unit_suffix_is_valid(suffix))
                إرجاع -EINVAL;

        /* Already a fully valid unit name? If so, no mangling is necessary... */
        إذا (unit_name_is_valid(name, UNIT_NAME_ANY)) {
                إذا (FLAGS_SET(flags, UNIT_NAME_MANGLE_STRICT) && !endswith(name, suffix)) {
                        ثابت حرف *e = ASSERT_PTR(strrchr(name, '.'));

                        إرجاع log_full_errno(warn ? LOG_NOTICE : LOG_DEBUG,
                                              SYNTHETIC_ERRNO(EINVAL),
                                              "Unit name \"%s\" has unit type \"%s\", but \"%s\" is expected%s%s.",
                                              name, e + 1, suffix + 1,
                                              operation ? " " : "", strempty(operation));
                }

                اذهب good;
        }

        /* Already a fully valid globbing expression? If so, no mangling is necessary either... */
        إذا (string_is_glob(name) && in_charset(name, VALID_CHARS_GLOB)) {
                إذا (FLAGS_SET(flags, UNIT_NAME_MANGLE_GLOB))
                        اذهب good;
                log_full(warn ? LOG_NOTICE : LOG_DEBUG,
                         "Glob pattern passed%s%s, but globs are not supported for this.",
                         operation ? " " : "", strempty(operation));
                suggest_escape = false;
        }

        إذا (path_is_absolute(name)) {
                _cleanup_free_ حرف *n = عدم, *u = عدم;

                r = path_simplify_alloc(name, &n);
                إذا (r < 0)
                        إرجاع r;

                إذا (is_device_path(n)) {
                        r = unit_name_from_path(n, ".device", &u);
                        إذا (r >= 0) {
                                إذا (FLAGS_SET(flags, UNIT_NAME_MANGLE_STRICT) && !streq(suffix, ".device"))
                                        إرجاع log_full_errno(warn ? LOG_NOTICE : LOG_DEBUG,
                                                              SYNTHETIC_ERRNO(EINVAL),
                                                              "Path \"%s\" resolves to unit type \"device\", but \"%s\" is expected%s%s.",
                                                              name, suffix + 1,
                                                              operation ? " " : "", strempty(operation));

                                *ret = TAKE_PTR(u);
                                إرجاع 1;
                        }
                        إذا (r != -EINVAL)
                                إرجاع r;
                }

                r = unit_name_from_path(n, ".mount", &u);
                إذا (r >= 0) {
                        إذا (FLAGS_SET(flags, UNIT_NAME_MANGLE_STRICT) && !streq(suffix, ".mount"))
                                إرجاع log_full_errno(warn ? LOG_NOTICE : LOG_DEBUG,
                                                      SYNTHETIC_ERRNO(EINVAL),
                                                      "Path \"%s\" resolves to unit type \"mount\", but \"%s\" is expected%s%s.",
                                                      name, suffix + 1,
                                                      operation ? " " : "", strempty(operation));

                        *ret = TAKE_PTR(u);
                        إرجاع 1;
                }
                إذا (r != -EINVAL)
                        إرجاع r;
        }

        s = new(حرف, strlen(name) * 4 + strlen(suffix) + 1);
        إذا (!s)
                إرجاع -ENOMEM;

        mangled = do_escape_mangle(name, FLAGS_SET(flags, UNIT_NAME_MANGLE_GLOB), s);
        إذا (mangled)
                log_full(warn ? LOG_NOTICE : LOG_DEBUG,
                         "Invalid unit name \"%s\" escaped as \"%s\"%s.",
                         name, s,
                         suggest_escape ? " (maybe you should use systemd-escape?)" : "");

        /* Append a suffix if it doesn't have any, but only if this is not a glob, so that we can allow
         * "foo.*" as a valid glob. */
        إذا ((!FLAGS_SET(flags, UNIT_NAME_MANGLE_GLOB) || !string_is_glob(s)) && unit_name_to_type(s) < 0)
                strcat(s, suffix);

        /* Make sure mangling didn't grow this too large (but don't do this check if globbing is allowed,
         * since globs generally do not qualify as valid unit names) */
        إذا (!FLAGS_SET(flags, UNIT_NAME_MANGLE_GLOB) && !unit_name_is_valid(s, UNIT_NAME_ANY))
                إرجاع -EINVAL;

        *ret = TAKE_PTR(s);
        إرجاع 1;

good:
        إرجاع strdup_to(ret, name);
}

صحيح slice_build_parent_slice(ثابت حرف *slice, حرف **ret) {
        أكد(slice);
        أكد(ret);

        إذا (!slice_name_is_valid(slice))
                إرجاع -EINVAL;

        إذا (streq(slice, SPECIAL_جذر_SLICE)) {
                *ret = عدم;
                إرجاع 0;
        }

        _cleanup_free_ حرف *s = strdup(slice);
        إذا (!s)
                إرجاع -ENOMEM;

        حرف *dash = strrchr(s, '-');
        إذا (!dash)
                إرجاع strdup_to_full(ret, SPECIAL_جذر_SLICE);

        /* We know that s ended with .slice before truncation, so we have enough space. */
        strcpy(dash, ".slice");

        *ret = TAKE_PTR(s);
        إرجاع 1;
}

صحيح slice_build_subslice(ثابت حرف *slice, ثابت حرف *name, حرف **ret) {
        حرف *subslice;

        أكد(slice);
        أكد(name);
        أكد(ret);

        إذا (!slice_name_is_valid(slice))
                إرجاع -EINVAL;

        إذا (!unit_prefix_is_valid(name))
                إرجاع -EINVAL;

        إذا (streq(slice, SPECIAL_جذر_SLICE))
                subslice = strjoin(name, ".slice");
        وإلا {
                ثابت حرف *e;

                assert_se(e = endswith(slice, ".slice"));

                subslice = new(حرف, (e - slice) + 1 + strlen(name) + 6 + 1);
                إذا (!subslice)
                        إرجاع -ENOMEM;

                stpcpy(stpcpy(stpcpy(mempcpy(subslice, slice, e - slice), "-"), name), ".slice");
        }

        *ret = subslice;
        إرجاع 0;
}

bool slice_name_is_valid(ثابت حرف *name) {
        ثابت حرف *p, *e;
        bool dash = false;

        إذا (!unit_name_is_valid(name, UNIT_NAME_PLAIN))
                إرجاع false;

        إذا (streq(name, SPECIAL_جذر_SLICE))
                إرجاع true;

        e = endswith(name, ".slice");
        إذا (!e)
                إرجاع false;

        لكل (p = name; p < e; p++) {

                إذا (*p == '-') {

                        /* Don't allow initial dash */
                        إذا (p == name)
                                إرجاع false;

                        /* Don't allow multiple dashes */
                        إذا (dash)
                                إرجاع false;

                        dash = true;
                } وإلا
                        dash = false;
        }

        /* Don't allow trailing hash */
        إذا (dash)
                إرجاع false;

        إرجاع true;
}

bool unit_name_prefix_equal(ثابت حرف *a, ثابت حرف *b) {
        ثابت حرف *p, *q;

        أكد(a);
        أكد(b);

        إذا (!unit_name_is_valid(a, UNIT_NAME_ANY) || !unit_name_is_valid(b, UNIT_NAME_ANY))
                إرجاع false;

        p = strchr(a, '@');
        إذا (!p)
                p = strrchr(a, '.');

        q = strchr(b, '@');
        إذا (!q)
                q = strrchr(b, '.');

        أكد(p);
        أكد(q);

        إرجاع memcmp_nn(a, p - a, b, q - b) == 0;
}
