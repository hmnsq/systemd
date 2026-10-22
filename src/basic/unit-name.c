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

        אם (_unlikely_(flags == 0))
                החזר false;

        אם (isempty(n))
                החזר false;

        אם (strlen(n) >= UNIT_NAME_MAX)
                החזר false;

        e = strrchr(n, '.');
        אם (!e || e == n)
                החזר false;

        אם (unit_type_from_string(e + 1) < 0)
                החזר false;

        עבור (i = n, at = عدم; i < e; i++) {

                אם (*i == '@' && !at)
                        at = i;

                אם (!strchr(VALID_CHARS_WITH_AT, *i))
                        החזר false;
        }

        אם (at == n)
                החזר false;

        אם (flags & UNIT_NAME_PLAIN)
                אם (!at)
                        החזר true;

        אם (flags & UNIT_NAME_INSTANCE)
                אם (at && e > at + 1)
                        החזר true;

        אם (flags & UNIT_NAME_TEMPLATE)
                אם (at && e == at + 1)
                        החזר true;

        החזר false;
}

bool unit_prefix_is_valid(ثابت حرف *p) {

        /* We don't allow additional @ in the prefix string */

        אם (isempty(p))
                החזר false;

        החזר in_charset(p, VALID_CHARS);
}

bool unit_instance_is_valid(ثابت حرف *i) {

        /* The max length depends on the length of the string, so we
         * don't really check this here. */

        אם (isempty(i))
                החזר false;

        /* We allow additional @ in the instance string, we do not
         * allow them in the prefix! */

        החזר in_charset(i, "@" VALID_CHARS);
}

bool unit_suffix_is_valid(ثابت حرف *s) {
        אם (isempty(s))
                החזר false;

        אם (s[0] != '.')
                החזר false;

        אם (unit_type_from_string(s + 1) < 0)
                החזר false;

        החזר true;
}

صحيح unit_name_to_prefix(ثابت حرف *n, حرف **ret) {
        ثابت حرف *p;
        حرف *s;

        أكد(n);
        أكد(ret);

        אם (!unit_name_is_valid(n, UNIT_NAME_ANY))
                החזר -EINVAL;

        p = strchr(n, '@');
        אם (!p)
                p = strrchr(n, '.');

        assert_se(p);

        s = strndup(n, p - n);
        אם (!s)
                החזר -ENOMEM;

        *ret = s;
        החזר 0;
}

UnitNameFlags unit_name_to_instance(ثابت حرف *n, حرف **ret) {
        ثابت حرف *p, *d;

        أكد(n);

        אם (!unit_name_is_valid(n, UNIT_NAME_ANY))
                החזר -EINVAL;

        /* Everything past the first @ and before the last . is the instance */
        p = strchr(n, '@');
        אם (!p) {
                אם (ret)
                        *ret = عدم;
                החזר UNIT_NAME_PLAIN;
        }

        p++;

        d = strrchr(p, '.');
        אם (!d)
                החזר -EINVAL;

        אם (ret) {
                حرف *i = strndup(p, d-p);
                אם (!i)
                        החזר -ENOMEM;

                *ret = i;
        }
        החזר d > p ? UNIT_NAME_INSTANCE : UNIT_NAME_TEMPLATE;
}

صحيح unit_name_to_prefix_and_instance(ثابت حرف *n, حرف **ret) {
        ثابت حرف *d;
        حرف *s;

        أكد(n);
        أكد(ret);

        אם (!unit_name_is_valid(n, UNIT_NAME_ANY))
                החזר -EINVAL;

        d = strrchr(n, '.');
        אם (!d)
                החזר -EINVAL;

        s = strndup(n, d - n);
        אם (!s)
                החזר -ENOMEM;

        *ret = s;
        החזר 0;
}

UnitType unit_name_to_type(ثابت حرف *n) {
        ثابت حرف *e;

        أكد(n);

        אם (!unit_name_is_valid(n, UNIT_NAME_ANY))
                החזר _UNIT_TYPE_INVALID;

        assert_se(e = strrchr(n, '.'));

        החזר unit_type_from_string(e + 1);
}

صحيح unit_name_change_suffix(ثابت حرف *n, ثابت حرف *suffix, حرف **ret) {
        _cleanup_free_ حرف *s = عدم;
        size_t a, b;
        ثابت حرف *e;

        أكد(n);
        أكد(suffix);
        أكد(ret);

        אם (!unit_name_is_valid(n, UNIT_NAME_ANY))
                החזר -EINVAL;

        אם (!unit_suffix_is_valid(suffix))
                החזר -EINVAL;

        assert_se(e = strrchr(n, '.'));

        a = e - n;
        b = strlen(suffix);

        s = new(حرف, a + b + 1);
        אם (!s)
                החזר -ENOMEM;

        strcpy(mempcpy(s, n, a), suffix);

        /* Make sure the name is still valid (i.e. didn't grow too large due to longer suffix) */
        אם (!unit_name_is_valid(s, UNIT_NAME_ANY))
                החזר -EINVAL;

        *ret = TAKE_PTR(s);
        החזר 0;
}

صحيح unit_name_build(ثابت حرف *prefix, ثابت حرف *instance, ثابت حرف *suffix, حرف **ret) {
        UnitType type;

        أكد(prefix);
        أكد(suffix);
        أكد(ret);

        אם (suffix[0] != '.')
                החזר -EINVAL;

        type = unit_type_from_string(suffix + 1);
        אם (type < 0)
                החזר type;

        החזר unit_name_build_from_type(prefix, instance, type, ret);
}

صحيح unit_name_build_from_type(ثابت حرف *prefix, ثابت حرف *instance, UnitType type, حرف **ret) {
        _cleanup_free_ حرف *s = عدم;
        ثابت حرف *ut;

        أكد(prefix);
        أكد(type >= 0);
        أكد(type < _UNIT_TYPE_MAX);
        أكد(ret);

        אם (!unit_prefix_is_valid(prefix))
                החזר -EINVAL;

        ut = unit_type_to_string(type);

        אם (instance) {
                אם (!unit_instance_is_valid(instance))
                        החזר -EINVAL;

                s = strjoin(prefix, "@", instance, ".", ut);
        } אחרת
                s = strjoin(prefix, ".", ut);
        אם (!s)
                החזר -ENOMEM;

        /* Verify that this didn't grow too large (or otherwise is invalid) */
        אם (!unit_name_is_valid(s, instance ? UNIT_NAME_INSTANCE : UNIT_NAME_PLAIN))
                החזר -EINVAL;

        *ret = TAKE_PTR(s);
        החזר 0;
}

ساكن حرف *do_escape_char(حرف c, حرف *t) {
        أكد(t);

        *(t++) = '\\';
        *(t++) = 'x';
        *(t++) = hexchar(c >> 4);
        *(t++) = hexchar(c);

        החזר t;
}

ساكن حرف *do_escape(ثابت حرف *f, حرف *t) {
        أكد(f);
        أكد(t);

        /* do not create units with a leading '.', like for "/.dotdir" mount points */
        אם (*f == '.') {
                t = do_escape_char(*f, t);
                f++;
        }

        עבור (; *f; f++) {
                אם (*f == '/')
                        *(t++) = '-';
                אחרת אם (IN_SET(*f, '-', '\\') || !strchr(VALID_CHARS, *f))
                        t = do_escape_char(*f, t);
                אחרת
                        *(t++) = *f;
        }

        החזר t;
}

حرف* unit_name_escape(ثابت حرف *f) {
        حرف *r, *t;

        أكد(f);

        r = new(حرف, strlen(f)*4+1);
        אם (!r)
                החזר عدم;

        t = do_escape(f, r);
        *t = 0;

        החזר r;
}

صحيح unit_name_unescape(ثابت حرف *f, حرف **ret) {
        _cleanup_free_ حرف *r = عدم;
        حرف *t;

        أكد(f);
        أكد(ret);

        r = strdup(f);
        אם (!r)
                החזר -ENOMEM;

        עבור (t = r; *f; f++) {
                אם (*f == '-')
                        *(t++) = '/';
                אחרת אם (*f == '\\') {
                        صحيح a, b;

                        אם (f[1] != 'x')
                                החזר -EINVAL;

                        a = unhexchar(f[2]);
                        אם (a < 0)
                                החזר -EINVAL;

                        b = unhexchar(f[3]);
                        אם (b < 0)
                                החזר -EINVAL;

                        *(t++) = (حرف) (((uint8_t) a << 4U) | (uint8_t) b);
                        f += 3;
                } אחרת
                        *(t++) = *f;
        }

        *t = 0;

        *ret = TAKE_PTR(r);

        החזר 0;
}

صحيح unit_name_path_escape(ثابت حرف *f, حرف **ret) {
        _cleanup_free_ حرف *p = عدم;
        حرف *s;
        صحيح r;

        أكد(f);
        أكد(ret);

        r = path_simplify_alloc(f, &p);
        אם (r < 0)
                החזר r;

        אם (empty_or_root(p))
                s = strdup("-");
        אחרת {
                אם (!path_is_normalized(p))
                        החזר -EINVAL;

                /* Truncate trailing slashes and skip leading slashes */
                delete_trailing_chars(p, "/");
                s = unit_name_escape(skip_leading_chars(p, "/"));
        }
        אם (!s)
                החזר -ENOMEM;

        *ret = s;
        החזר 0;
}

صحيح unit_name_path_unescape(ثابت حرف *f, حرف **ret) {
        _cleanup_free_ حرف *s = عدم;
        صحيح r;

        أكد(f);

        אם (isempty(f))
                החזר -EINVAL;

        אם (streq(f, "-")) {
                s = strdup("/");
                אם (!s)
                        החזר -ENOMEM;
        } אחרת {
                _cleanup_free_ حرف *w = عدم;

                r = unit_name_unescape(f, &w);
                אם (r < 0)
                        החזר r;

                /* Don't accept trailing or leading slashes */
                אם (startswith(w, "/") || endswith(w, "/"))
                        החזר -EINVAL;

                /* Prefix a slash again */
                s = strjoin("/", w);
                אם (!s)
                        החזר -ENOMEM;

                אם (!path_is_normalized(s))
                        החזר -EINVAL;
        }

        אם (ret)
                *ret = TAKE_PTR(s);

        החזר 0;
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

        אם (!unit_name_is_valid(original, UNIT_NAME_INSTANCE|UNIT_NAME_TEMPLATE))
                החזר -EINVAL;
        אם (!unit_instance_is_valid(instance) && !(accept_glob && in_charset(instance, VALID_CHARS_GLOB)))
                החזר -EINVAL;

        prefix = ASSERT_PTR(strchr(original, '@'));
        suffix = ASSERT_PTR(strrchr(original, '.'));
        أكد(prefix < suffix);

        pl = prefix - original + 1; /* include '@' */

        s = new(حرف, pl + strlen(instance) + strlen(suffix) + 1);
        אם (!s)
                החזר -ENOMEM;

#אם HAS_FEATURE_MEMORY_SANITIZER
        /* MSan doesn't like stpncpy... See also https://github.com/google/sanitizers/issues/926 */
        memzero(s, pl + strlen(instance) + strlen(suffix) + 1);
#endif

        strcpy(stpcpy(stpncpy(s, original, pl), instance), suffix);

        /* Make sure the resulting name still is valid, i.e. didn't grow too large. Globs will be expanded
         * by clients when used, so the check is pointless. */
        אם (!accept_glob && !unit_name_is_valid(s, UNIT_NAME_INSTANCE))
                החזר -EINVAL;

        *ret = TAKE_PTR(s);
        החזר 0;
}

صحيح unit_name_template(ثابت حرف *f, حرف **ret) {
        ثابت حرف *p, *e;
        حرف *s;
        size_t a;

        أكد(f);
        أكد(ret);

        אם (!unit_name_is_valid(f, UNIT_NAME_INSTANCE|UNIT_NAME_TEMPLATE))
                החזר -EINVAL;

        assert_se(p = strchr(f, '@'));
        assert_se(e = strrchr(f, '.'));

        a = p - f;

        s = new(حرف, a + 1 + strlen(e) + 1);
        אם (!s)
                החזר -ENOMEM;

        strcpy(mempcpy(s, f, a + 1), e);

        *ret = s;
        החזר 0;
}

bool unit_name_is_hashed(ثابت حرف *name) {
        ثابت حرف *s;

        אם (!unit_name_is_valid(name, UNIT_NAME_PLAIN))
                החזר false;

        assert_se(s = strrchr(name, '.'));

        אם (s - name < UNIT_NAME_HASH_LENGTH_CHARS + 1)
                החזר false;

        s -= UNIT_NAME_HASH_LENGTH_CHARS;
        אם (s[-1] != '_')
                החזר false;

        עבור (size_t i = 0; i < UNIT_NAME_HASH_LENGTH_CHARS; i++)
                אם (!strchr(LOWERCASE_HEXDIGITS, s[i]))
                        החזר false;

        החזר true;
}

صحيح unit_name_hash_long(ثابت حرف *name, حرف **ret) {
        _cleanup_free_ حرف *n = عدم, *hash = عدم;
        ثابت حرف *suffix;
        le64_t h;
        size_t len;

        أكد(ret);

        אם (strlen(name) < UNIT_NAME_MAX)
                החזר -EMSGSIZE;

        suffix = strrchr(name, '.');
        אם (!suffix)
                החזר -EINVAL;

        אם (unit_type_from_string(suffix+1) < 0)
                החזר -EINVAL;

        h = htole64(siphash24_string(name, LONG_UNIT_NAME_HASH_KEY.bytes));

        hash = hexmem(&h, حجم(h));
        אם (!hash)
                החזר -ENOMEM;

        assert_se(strlen(hash) == UNIT_NAME_HASH_LENGTH_CHARS);

        len = UNIT_NAME_MAX - 1 - strlen(suffix+1) - UNIT_NAME_HASH_LENGTH_CHARS - 2;
        أكد(len > 0 && len < UNIT_NAME_MAX);

        n = strndup(name, len);
        אם (!n)
                החזר -ENOMEM;

        אם (!strextend(&n, "_", hash, suffix))
                החזר -ENOMEM;
        assert_se(unit_name_is_valid(n, UNIT_NAME_PLAIN));

        *ret = TAKE_PTR(n);

        החזר 0;
}

صحيح unit_name_from_path(ثابت حرف *path, ثابت حرف *suffix, حرف **ret) {
        _cleanup_free_ حرف *p = عدم, *s = عدم;
        صحيح r;

        أكد(path);
        أكد(suffix);
        أكد(ret);

        אם (!unit_suffix_is_valid(suffix))
                החזר -EINVAL;

        r = unit_name_path_escape(path, &p);
        אם (r < 0)
                החזר r;

        s = strjoin(p, suffix);
        אם (!s)
                החזר -ENOMEM;

        אם (strlen(s) >= UNIT_NAME_MAX) {
                _cleanup_free_ حرف *n = عدم;

                log_debug("Unit name \"%s\" too long, falling back to hashed unit name.", s);

                r = unit_name_hash_long(s, &n);
                אם (r < 0)
                        החזר r;

                free_and_replace(s, n);
        }

        /* Refuse if this for some other reason didn't result in a valid name */
        אם (!unit_name_is_valid(s, UNIT_NAME_PLAIN))
                החזר -EINVAL;

        *ret = TAKE_PTR(s);
        החזר 0;
}

صحيح unit_name_from_path_instance(ثابت حرف *prefix, ثابت حرف *path, ثابت حرف *suffix, حرف **ret) {
        _cleanup_free_ حرف *p = عدم, *s = عدم;
        صحيح r;

        أكد(prefix);
        أكد(path);
        أكد(suffix);
        أكد(ret);

        אם (!unit_prefix_is_valid(prefix))
                החזר -EINVAL;

        אם (!unit_suffix_is_valid(suffix))
                החזר -EINVAL;

        r = unit_name_path_escape(path, &p);
        אם (r < 0)
                החזר r;

        s = strjoin(prefix, "@", p, suffix);
        אם (!s)
                החזר -ENOMEM;

        אם (strlen(s) >= UNIT_NAME_MAX) /* Return a slightly more descriptive error for this specific condition */
                החזר -ENAMETOOLONG;

        /* Refuse if this for some other reason didn't result in a valid name */
        אם (!unit_name_is_valid(s, UNIT_NAME_INSTANCE))
                החזר -EINVAL;

        *ret = TAKE_PTR(s);
        החזר 0;
}

صحيح unit_name_to_path(ثابت حرف *name, حرف **ret) {
        _cleanup_free_ حرف *prefix = عدم;
        صحيح r;

        أكد(name);

        r = unit_name_to_prefix(name, &prefix);
        אם (r < 0)
                החזר r;

        אם (unit_name_is_hashed(name))
                החזר -ENAMETOOLONG;

        החזר unit_name_path_unescape(prefix, ret);
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

        עבור (; *f; f++)
                אם (*f == '/') {
                        *(t++) = '-';
                        mangled = true;
                } אחרת אם (!strchr(valid_chars, *f)) {
                        t = do_escape_char(*f, t);
                        mangled = true;
                } אחרת
                        *(t++) = *f;
        *t = 0;

        החזר mangled;
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

        אם (isempty(name)) /* We cannot mangle empty unit names to become valid, sorry. */
                החזר -EINVAL;

        אם (!unit_suffix_is_valid(suffix))
                החזר -EINVAL;

        /* Already a fully valid unit name? If so, no mangling is necessary... */
        אם (unit_name_is_valid(name, UNIT_NAME_ANY)) {
                אם (FLAGS_SET(flags, UNIT_NAME_MANGLE_STRICT) && !endswith(name, suffix)) {
                        ثابت حرف *e = ASSERT_PTR(strrchr(name, '.'));

                        החזר log_full_errno(warn ? LOG_NOTICE : LOG_DEBUG,
                                              SYNTHETIC_ERRNO(EINVAL),
                                              "Unit name \"%s\" has unit type \"%s\", but \"%s\" is expected%s%s.",
                                              name, e + 1, suffix + 1,
                                              operation ? " " : "", strempty(operation));
                }

                اذهب good;
        }

        /* Already a fully valid globbing expression? If so, no mangling is necessary either... */
        אם (string_is_glob(name) && in_charset(name, VALID_CHARS_GLOB)) {
                אם (FLAGS_SET(flags, UNIT_NAME_MANGLE_GLOB))
                        اذهب good;
                log_full(warn ? LOG_NOTICE : LOG_DEBUG,
                         "Glob pattern passed%s%s, but globs are not supported for this.",
                         operation ? " " : "", strempty(operation));
                suggest_escape = false;
        }

        אם (path_is_absolute(name)) {
                _cleanup_free_ حرف *n = عدم, *u = عدم;

                r = path_simplify_alloc(name, &n);
                אם (r < 0)
                        החזר r;

                אם (is_device_path(n)) {
                        r = unit_name_from_path(n, ".device", &u);
                        אם (r >= 0) {
                                אם (FLAGS_SET(flags, UNIT_NAME_MANGLE_STRICT) && !streq(suffix, ".device"))
                                        החזר log_full_errno(warn ? LOG_NOTICE : LOG_DEBUG,
                                                              SYNTHETIC_ERRNO(EINVAL),
                                                              "Path \"%s\" resolves to unit type \"device\", but \"%s\" is expected%s%s.",
                                                              name, suffix + 1,
                                                              operation ? " " : "", strempty(operation));

                                *ret = TAKE_PTR(u);
                                החזר 1;
                        }
                        אם (r != -EINVAL)
                                החזר r;
                }

                r = unit_name_from_path(n, ".mount", &u);
                אם (r >= 0) {
                        אם (FLAGS_SET(flags, UNIT_NAME_MANGLE_STRICT) && !streq(suffix, ".mount"))
                                החזר log_full_errno(warn ? LOG_NOTICE : LOG_DEBUG,
                                                      SYNTHETIC_ERRNO(EINVAL),
                                                      "Path \"%s\" resolves to unit type \"mount\", but \"%s\" is expected%s%s.",
                                                      name, suffix + 1,
                                                      operation ? " " : "", strempty(operation));

                        *ret = TAKE_PTR(u);
                        החזר 1;
                }
                אם (r != -EINVAL)
                        החזר r;
        }

        s = new(حرف, strlen(name) * 4 + strlen(suffix) + 1);
        אם (!s)
                החזר -ENOMEM;

        mangled = do_escape_mangle(name, FLAGS_SET(flags, UNIT_NAME_MANGLE_GLOB), s);
        אם (mangled)
                log_full(warn ? LOG_NOTICE : LOG_DEBUG,
                         "Invalid unit name \"%s\" escaped as \"%s\"%s.",
                         name, s,
                         suggest_escape ? " (maybe you should use systemd-escape?)" : "");

        /* Append a suffix if it doesn't have any, but only if this is not a glob, so that we can allow
         * "foo.*" as a valid glob. */
        אם ((!FLAGS_SET(flags, UNIT_NAME_MANGLE_GLOB) || !string_is_glob(s)) && unit_name_to_type(s) < 0)
                strcat(s, suffix);

        /* Make sure mangling didn't grow this too large (but don't do this check if globbing is allowed,
         * since globs generally do not qualify as valid unit names) */
        אם (!FLAGS_SET(flags, UNIT_NAME_MANGLE_GLOB) && !unit_name_is_valid(s, UNIT_NAME_ANY))
                החזר -EINVAL;

        *ret = TAKE_PTR(s);
        החזר 1;

good:
        החזר strdup_to(ret, name);
}

صحيح slice_build_parent_slice(ثابت حرف *slice, حرف **ret) {
        أكد(slice);
        أكد(ret);

        אם (!slice_name_is_valid(slice))
                החזר -EINVAL;

        אם (streq(slice, SPECIAL_ROOT_SLICE)) {
                *ret = عدم;
                החזר 0;
        }

        _cleanup_free_ حرف *s = strdup(slice);
        אם (!s)
                החזר -ENOMEM;

        حرف *dash = strrchr(s, '-');
        אם (!dash)
                החזר strdup_to_full(ret, SPECIAL_ROOT_SLICE);

        /* We know that s ended with .slice before truncation, so we have enough space. */
        strcpy(dash, ".slice");

        *ret = TAKE_PTR(s);
        החזר 1;
}

صحيح slice_build_subslice(ثابت حرف *slice, ثابت حرف *name, حرف **ret) {
        حرف *subslice;

        أكد(slice);
        أكد(name);
        أكد(ret);

        אם (!slice_name_is_valid(slice))
                החזר -EINVAL;

        אם (!unit_prefix_is_valid(name))
                החזר -EINVAL;

        אם (streq(slice, SPECIAL_ROOT_SLICE))
                subslice = strjoin(name, ".slice");
        אחרת {
                ثابت حرف *e;

                assert_se(e = endswith(slice, ".slice"));

                subslice = new(حرف, (e - slice) + 1 + strlen(name) + 6 + 1);
                אם (!subslice)
                        החזר -ENOMEM;

                stpcpy(stpcpy(stpcpy(mempcpy(subslice, slice, e - slice), "-"), name), ".slice");
        }

        *ret = subslice;
        החזר 0;
}

bool slice_name_is_valid(ثابت حرف *name) {
        ثابت حرف *p, *e;
        bool dash = false;

        אם (!unit_name_is_valid(name, UNIT_NAME_PLAIN))
                החזר false;

        אם (streq(name, SPECIAL_ROOT_SLICE))
                החזר true;

        e = endswith(name, ".slice");
        אם (!e)
                החזר false;

        עבור (p = name; p < e; p++) {

                אם (*p == '-') {

                        /* Don't allow initial dash */
                        אם (p == name)
                                החזר false;

                        /* Don't allow multiple dashes */
                        אם (dash)
                                החזר false;

                        dash = true;
                } אחרת
                        dash = false;
        }

        /* Don't allow trailing hash */
        אם (dash)
                החזר false;

        החזר true;
}

bool unit_name_prefix_equal(ثابت حرف *a, ثابت حرف *b) {
        ثابت حرف *p, *q;

        أكد(a);
        أكد(b);

        אם (!unit_name_is_valid(a, UNIT_NAME_ANY) || !unit_name_is_valid(b, UNIT_NAME_ANY))
                החזר false;

        p = strchr(a, '@');
        אם (!p)
                p = strrchr(a, '.');

        q = strchr(b, '@');
        אם (!q)
                q = strrchr(b, '.');

        أكد(p);
        أكد(q);

        החזר memcmp_nn(a, p - a, b, q - b) == 0;
}
