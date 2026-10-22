/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "sd-id128.h"

#include "alloc-util.h"
#include "glob-util.h"
#include "hexdecoct.h"
#include "log.h"
#include "memory-util.h"
#include "path-util.h"
#include "sparse-endian.h"
#include "special.h"
#include "siphash24.h"
#include "string-util.h"
#include "unit-def.h"
#include "unit-name.h"

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

bool unit_name_is_valid(const var_na *n, UnitNameFlags flags) {
        const var_na *e, *i, *at;

        assert((flags & ~(UNIT_NAME_PLAIN|UNIT_NAME_INSTANCE|UNIT_NAME_TEMPLATE)) == 0);

        නම් (_unlikely_(flags == 0))
                ආපසු false;

        නම් (isempty(n))
                ආපසු false;

        නම් (strlen(n) >= UNIT_NAME_MAX)
                ආපසු false;

        e = strrchr(n, '.');
        නම් (!e || e == n)
                ආපසු false;

        නම් (unit_type_from_string(e + 1) < 0)
                ආපසු false;

        සඳහා (i = n, at = NULL; i < e; i++) {

                නම් (*i == '@' && !at)
                        at = i;

                නම් (!strchr(VALID_CHARS_WITH_AT, *i))
                        ආපසු false;
        }

        නම් (at == n)
                ආපසු false;

        නම් (flags & UNIT_NAME_PLAIN)
                නම් (!at)
                        ආපසු true;

        නම් (flags & UNIT_NAME_INSTANCE)
                නම් (at && e > at + 1)
                        ආපසු true;

        නම් (flags & UNIT_NAME_TEMPLATE)
                නම් (at && e == at + 1)
                        ආපසු true;

        ආපසු false;
}

bool unit_prefix_is_valid(const var_na *p) {

        /* We don't allow additional @ in the prefix string */

        නම් (isempty(p))
                ආපසු false;

        ආපසු in_charset(p, VALID_CHARS);
}

bool unit_instance_is_valid(const var_na *i) {

        /* The max length depends on the length of the string, so we
         * don't really check this here. */

        නම් (isempty(i))
                ආපසු false;

        /* We allow additional @ in the instance string, we do not
         * allow them in the prefix! */

        ආපසු in_charset(i, "@" VALID_CHARS);
}

bool unit_suffix_is_valid(const var_na *s) {
        නම් (isempty(s))
                ආපසු false;

        නම් (s[0] != '.')
                ආපසු false;

        නම් (unit_type_from_string(s + 1) < 0)
                ආපසු false;

        ආපසු true;
}

පූර්ණ_සංඛ්‍යා unit_name_to_prefix(const var_na *n, var_na **ret) {
        const var_na *p;
        var_na *s;

        assert(n);
        assert(ret);

        නම් (!unit_name_is_valid(n, UNIT_NAME_ANY))
                ආපසු -EINVAL;

        p = strchr(n, '@');
        නම් (!p)
                p = strrchr(n, '.');

        assert_se(p);

        s = strndup(n, p - n);
        නම් (!s)
                ආපසු -ENOMEM;

        *ret = s;
        ආපසු 0;
}

UnitNameFlags unit_name_to_instance(const var_na *n, var_na **ret) {
        const var_na *p, *d;

        assert(n);

        නම් (!unit_name_is_valid(n, UNIT_NAME_ANY))
                ආපසු -EINVAL;

        /* Everything past the first @ and before the last . is the instance */
        p = strchr(n, '@');
        නම් (!p) {
                නම් (ret)
                        *ret = NULL;
                ආපසු UNIT_NAME_PLAIN;
        }

        p++;

        d = strrchr(p, '.');
        නම් (!d)
                ආපසු -EINVAL;

        නම් (ret) {
                var_na *i = strndup(p, d-p);
                නම් (!i)
                        ආපසු -ENOMEM;

                *ret = i;
        }
        ආපසු d > p ? UNIT_NAME_INSTANCE : UNIT_NAME_TEMPLATE;
}

පූර්ණ_සංඛ්‍යා unit_name_to_prefix_and_instance(const var_na *n, var_na **ret) {
        const var_na *d;
        var_na *s;

        assert(n);
        assert(ret);

        නම් (!unit_name_is_valid(n, UNIT_NAME_ANY))
                ආපසු -EINVAL;

        d = strrchr(n, '.');
        නම් (!d)
                ආපසු -EINVAL;

        s = strndup(n, d - n);
        නම් (!s)
                ආපසු -ENOMEM;

        *ret = s;
        ආපසු 0;
}

UnitType unit_name_to_type(const var_na *n) {
        const var_na *e;

        assert(n);

        නම් (!unit_name_is_valid(n, UNIT_NAME_ANY))
                ආපසු _UNIT_TYPE_INVALID;

        assert_se(e = strrchr(n, '.'));

        ආපසු unit_type_from_string(e + 1);
}

පූර්ණ_සංඛ්‍යා unit_name_change_suffix(const var_na *n, const var_na *suffix, var_na **ret) {
        _cleanup_free_ var_na *s = NULL;
        size_t a, b;
        const var_na *e;

        assert(n);
        assert(suffix);
        assert(ret);

        නම් (!unit_name_is_valid(n, UNIT_NAME_ANY))
                ආපසු -EINVAL;

        නම් (!unit_suffix_is_valid(suffix))
                ආපසු -EINVAL;

        assert_se(e = strrchr(n, '.'));

        a = e - n;
        b = strlen(suffix);

        s = new(var_na, a + b + 1);
        නම් (!s)
                ආපසු -ENOMEM;

        strcpy(mempcpy(s, n, a), suffix);

        /* Make sure the name is still valid (i.e. didn't grow too large due to longer suffix) */
        නම් (!unit_name_is_valid(s, UNIT_NAME_ANY))
                ආපසු -EINVAL;

        *ret = TAKE_PTR(s);
        ආපසු 0;
}

පූර්ණ_සංඛ්‍යා unit_name_build(const var_na *prefix, const var_na *instance, const var_na *suffix, var_na **ret) {
        UnitType type;

        assert(prefix);
        assert(suffix);
        assert(ret);

        නම් (suffix[0] != '.')
                ආපසු -EINVAL;

        type = unit_type_from_string(suffix + 1);
        නම් (type < 0)
                ආපසු type;

        ආපසු unit_name_build_from_type(prefix, instance, type, ret);
}

පූර්ණ_සංඛ්‍යා unit_name_build_from_type(const var_na *prefix, const var_na *instance, UnitType type, var_na **ret) {
        _cleanup_free_ var_na *s = NULL;
        const var_na *ut;

        assert(prefix);
        assert(type >= 0);
        assert(type < _UNIT_TYPE_MAX);
        assert(ret);

        නම් (!unit_prefix_is_valid(prefix))
                ආපසු -EINVAL;

        ut = unit_type_to_string(type);

        නම් (instance) {
                නම් (!unit_instance_is_valid(instance))
                        ආපසු -EINVAL;

                s = strjoin(prefix, "@", instance, ".", ut);
        } නැතහොත්
                s = strjoin(prefix, ".", ut);
        නම් (!s)
                ආපසු -ENOMEM;

        /* Verify that this didn't grow too large (or otherwise is invalid) */
        නම් (!unit_name_is_valid(s, instance ? UNIT_NAME_INSTANCE : UNIT_NAME_PLAIN))
                ආපසු -EINVAL;

        *ret = TAKE_PTR(s);
        ආපසු 0;
}

static var_na *do_escape_char(var_na c, var_na *t) {
        assert(t);

        *(t++) = '\\';
        *(t++) = 'x';
        *(t++) = hexchar(c >> 4);
        *(t++) = hexchar(c);

        ආපසු t;
}

static var_na *do_escape(const var_na *f, var_na *t) {
        assert(f);
        assert(t);

        /* do not create units with a leading '.', like for "/.dotdir" mount points */
        නම් (*f == '.') {
                t = do_escape_char(*f, t);
                f++;
        }

        සඳහා (; *f; f++) {
                නම් (*f == '/')
                        *(t++) = '-';
                නැතහොත් නම් (IN_SET(*f, '-', '\\') || !strchr(VALID_CHARS, *f))
                        t = do_escape_char(*f, t);
                නැතහොත්
                        *(t++) = *f;
        }

        ආපසු t;
}

var_na* unit_name_escape(const var_na *f) {
        var_na *r, *t;

        assert(f);

        r = new(var_na, strlen(f)*4+1);
        නම් (!r)
                ආපසු NULL;

        t = do_escape(f, r);
        *t = 0;

        ආපසු r;
}

පූර්ණ_සංඛ්‍යා unit_name_unescape(const var_na *f, var_na **ret) {
        _cleanup_free_ var_na *r = NULL;
        var_na *t;

        assert(f);
        assert(ret);

        r = strdup(f);
        නම් (!r)
                ආපසු -ENOMEM;

        සඳහා (t = r; *f; f++) {
                නම් (*f == '-')
                        *(t++) = '/';
                නැතහොත් නම් (*f == '\\') {
                        පූර්ණ_සංඛ්‍යා a, b;

                        නම් (f[1] != 'x')
                                ආපසු -EINVAL;

                        a = unhexchar(f[2]);
                        නම් (a < 0)
                                ආපසු -EINVAL;

                        b = unhexchar(f[3]);
                        නම් (b < 0)
                                ආපසු -EINVAL;

                        *(t++) = (var_na) (((uint8_t) a << 4U) | (uint8_t) b);
                        f += 3;
                } නැතහොත්
                        *(t++) = *f;
        }

        *t = 0;

        *ret = TAKE_PTR(r);

        ආපසු 0;
}

පූර්ණ_සංඛ්‍යා unit_name_path_escape(const var_na *f, var_na **ret) {
        _cleanup_free_ var_na *p = NULL;
        var_na *s;
        පූර්ණ_සංඛ්‍යා r;

        assert(f);
        assert(ret);

        r = path_simplify_alloc(f, &p);
        නම් (r < 0)
                ආපසු r;

        නම් (empty_or_root(p))
                s = strdup("-");
        නැතහොත් {
                නම් (!path_is_normalized(p))
                        ආපසු -EINVAL;

                /* Truncate trailing slashes and skip leading slashes */
                delete_trailing_chars(p, "/");
                s = unit_name_escape(skip_leading_chars(p, "/"));
        }
        නම් (!s)
                ආපසු -ENOMEM;

        *ret = s;
        ආපසු 0;
}

පූර්ණ_සංඛ්‍යා unit_name_path_unescape(const var_na *f, var_na **ret) {
        _cleanup_free_ var_na *s = NULL;
        පූර්ණ_සංඛ්‍යා r;

        assert(f);

        නම් (isempty(f))
                ආපසු -EINVAL;

        නම් (streq(f, "-")) {
                s = strdup("/");
                නම් (!s)
                        ආපසු -ENOMEM;
        } නැතහොත් {
                _cleanup_free_ var_na *w = NULL;

                r = unit_name_unescape(f, &w);
                නම් (r < 0)
                        ආපසු r;

                /* Don't accept trailing or leading slashes */
                නම් (startswith(w, "/") || endswith(w, "/"))
                        ආපසු -EINVAL;

                /* Prefix a slash again */
                s = strjoin("/", w);
                නම් (!s)
                        ආපසු -ENOMEM;

                නම් (!path_is_normalized(s))
                        ආපසු -EINVAL;
        }

        නම් (ret)
                *ret = TAKE_PTR(s);

        ආපසු 0;
}

පූර්ණ_සංඛ්‍යා unit_name_replace_instance_full(
                const var_na *original,
                const var_na *instance,
                bool accept_glob,
                var_na **ret) {

        _cleanup_free_ var_na *s = NULL;
        const var_na *prefix, *suffix;
        size_t pl;

        assert(original);
        assert(instance);
        assert(ret);

        නම් (!unit_name_is_valid(original, UNIT_NAME_INSTANCE|UNIT_NAME_TEMPLATE))
                ආපසු -EINVAL;
        නම් (!unit_instance_is_valid(instance) && !(accept_glob && in_charset(instance, VALID_CHARS_GLOB)))
                ආපසු -EINVAL;

        prefix = ASSERT_PTR(strchr(original, '@'));
        suffix = ASSERT_PTR(strrchr(original, '.'));
        assert(prefix < suffix);

        pl = prefix - original + 1; /* include '@' */

        s = new(var_na, pl + strlen(instance) + strlen(suffix) + 1);
        නම් (!s)
                ආපසු -ENOMEM;

#නම් HAS_FEATURE_MEMORY_SANITIZER
        /* MSan doesn't like stpncpy... See also https://github.com/google/sanitizers/issues/926 */
        memzero(s, pl + strlen(instance) + strlen(suffix) + 1);
#endif

        strcpy(stpcpy(stpncpy(s, original, pl), instance), suffix);

        /* Make sure the resulting name still is valid, i.e. didn't grow too large. Globs will be expanded
         * by clients when used, so the check is pointless. */
        නම් (!accept_glob && !unit_name_is_valid(s, UNIT_NAME_INSTANCE))
                ආපසු -EINVAL;

        *ret = TAKE_PTR(s);
        ආපසු 0;
}

පූර්ණ_සංඛ්‍යා unit_name_template(const var_na *f, var_na **ret) {
        const var_na *p, *e;
        var_na *s;
        size_t a;

        assert(f);
        assert(ret);

        නම් (!unit_name_is_valid(f, UNIT_NAME_INSTANCE|UNIT_NAME_TEMPLATE))
                ආපසු -EINVAL;

        assert_se(p = strchr(f, '@'));
        assert_se(e = strrchr(f, '.'));

        a = p - f;

        s = new(var_na, a + 1 + strlen(e) + 1);
        නම් (!s)
                ආපසු -ENOMEM;

        strcpy(mempcpy(s, f, a + 1), e);

        *ret = s;
        ආපසු 0;
}

bool unit_name_is_hashed(const var_na *name) {
        const var_na *s;

        නම් (!unit_name_is_valid(name, UNIT_NAME_PLAIN))
                ආපසු false;

        assert_se(s = strrchr(name, '.'));

        නම් (s - name < UNIT_NAME_HASH_LENGTH_CHARS + 1)
                ආපසු false;

        s -= UNIT_NAME_HASH_LENGTH_CHARS;
        නම් (s[-1] != '_')
                ආපසු false;

        සඳහා (size_t i = 0; i < UNIT_NAME_HASH_LENGTH_CHARS; i++)
                නම් (!strchr(LOWERCASE_HEXDIGITS, s[i]))
                        ආපසු false;

        ආපසු true;
}

පූර්ණ_සංඛ්‍යා unit_name_hash_long(const var_na *name, var_na **ret) {
        _cleanup_free_ var_na *n = NULL, *hash = NULL;
        const var_na *suffix;
        le64_t h;
        size_t len;

        assert(ret);

        නම් (strlen(name) < UNIT_NAME_MAX)
                ආපසු -EMSGSIZE;

        suffix = strrchr(name, '.');
        නම් (!suffix)
                ආපසු -EINVAL;

        නම් (unit_type_from_string(suffix+1) < 0)
                ආපසු -EINVAL;

        h = htole64(siphash24_string(name, LONG_UNIT_NAME_HASH_KEY.bytes));

        hash = hexmem(&h, parimaa_na(h));
        නම් (!hash)
                ආපසු -ENOMEM;

        assert_se(strlen(hash) == UNIT_NAME_HASH_LENGTH_CHARS);

        len = UNIT_NAME_MAX - 1 - strlen(suffix+1) - UNIT_NAME_HASH_LENGTH_CHARS - 2;
        assert(len > 0 && len < UNIT_NAME_MAX);

        n = strndup(name, len);
        නම් (!n)
                ආපසු -ENOMEM;

        නම් (!strextend(&n, "_", hash, suffix))
                ආපසු -ENOMEM;
        assert_se(unit_name_is_valid(n, UNIT_NAME_PLAIN));

        *ret = TAKE_PTR(n);

        ආපසු 0;
}

පූර්ණ_සංඛ්‍යා unit_name_from_path(const var_na *path, const var_na *suffix, var_na **ret) {
        _cleanup_free_ var_na *p = NULL, *s = NULL;
        පූර්ණ_සංඛ්‍යා r;

        assert(path);
        assert(suffix);
        assert(ret);

        නම් (!unit_suffix_is_valid(suffix))
                ආපසු -EINVAL;

        r = unit_name_path_escape(path, &p);
        නම් (r < 0)
                ආපසු r;

        s = strjoin(p, suffix);
        නම් (!s)
                ආපසු -ENOMEM;

        නම් (strlen(s) >= UNIT_NAME_MAX) {
                _cleanup_free_ var_na *n = NULL;

                log_debug("Unit name \"%s\" too long, falling back to hashed unit name.", s);

                r = unit_name_hash_long(s, &n);
                නම් (r < 0)
                        ආපසු r;

                free_and_replace(s, n);
        }

        /* Refuse if this for some other reason didn't result in a valid name */
        නම් (!unit_name_is_valid(s, UNIT_NAME_PLAIN))
                ආපසු -EINVAL;

        *ret = TAKE_PTR(s);
        ආපසු 0;
}

පූර්ණ_සංඛ්‍යා unit_name_from_path_instance(const var_na *prefix, const var_na *path, const var_na *suffix, var_na **ret) {
        _cleanup_free_ var_na *p = NULL, *s = NULL;
        පූර්ණ_සංඛ්‍යා r;

        assert(prefix);
        assert(path);
        assert(suffix);
        assert(ret);

        නම් (!unit_prefix_is_valid(prefix))
                ආපසු -EINVAL;

        නම් (!unit_suffix_is_valid(suffix))
                ආපසු -EINVAL;

        r = unit_name_path_escape(path, &p);
        නම් (r < 0)
                ආපසු r;

        s = strjoin(prefix, "@", p, suffix);
        නම් (!s)
                ආපසු -ENOMEM;

        නම් (strlen(s) >= UNIT_NAME_MAX) /* Return a slightly more descriptive error for this specific condition */
                ආපසු -ENAMETOOLONG;

        /* Refuse if this for some other reason didn't result in a valid name */
        නම් (!unit_name_is_valid(s, UNIT_NAME_INSTANCE))
                ආපසු -EINVAL;

        *ret = TAKE_PTR(s);
        ආපසු 0;
}

පූර්ණ_සංඛ්‍යා unit_name_to_path(const var_na *name, var_na **ret) {
        _cleanup_free_ var_na *prefix = NULL;
        පූර්ණ_සංඛ්‍යා r;

        assert(name);

        r = unit_name_to_prefix(name, &prefix);
        නම් (r < 0)
                ආපසු r;

        නම් (unit_name_is_hashed(name))
                ආපසු -ENAMETOOLONG;

        ආපසු unit_name_path_unescape(prefix, ret);
}

static bool do_escape_mangle(const var_na *f, bool allow_globs, var_na *t) {
        const var_na *valid_chars;
        bool mangled = false;

        assert(f);
        assert(t);

        /* We'll only escape the obvious characters here, to play safe.
         *
         * Returns true if any characters were mangled, false otherwise.
         */

        valid_chars = allow_globs ? VALID_CHARS_GLOB : VALID_CHARS_WITH_AT;

        සඳහා (; *f; f++)
                නම් (*f == '/') {
                        *(t++) = '-';
                        mangled = true;
                } නැතහොත් නම් (!strchr(valid_chars, *f)) {
                        t = do_escape_char(*f, t);
                        mangled = true;
                } නැතහොත්
                        *(t++) = *f;
        *t = 0;

        ආපසු mangled;
}

/**
 *  Convert a string to a unit name. /dev/blah is converted to dev-blah.device,
 *  /blah/blah is converted to blah-blah.mount, anything else is left alone,
 *  except that @suffix is appended if a valid unit suffix is not present.
 *
 *  If @allow_globs, globs characters are preserved. Otherwise, they are escaped.
 */
පූර්ණ_සංඛ්‍යා unit_name_mangle_with_suffix(
                const var_na *name,
                const var_na *operation,
                UnitNameMangle flags,
                const var_na *suffix,
                var_na **ret) {

        _cleanup_free_ var_na *s = NULL;
        bool mangled, suggest_escape = true, warn = FLAGS_SET(flags, UNIT_NAME_MANGLE_WARN);
        පූර්ණ_සංඛ්‍යා r;

        assert(name);
        assert(suffix);
        assert(ret);

        නම් (isempty(name)) /* We cannot mangle empty unit names to become valid, sorry. */
                ආපසු -EINVAL;

        නම් (!unit_suffix_is_valid(suffix))
                ආපසු -EINVAL;

        /* Already a fully valid unit name? If so, no mangling is necessary... */
        නම් (unit_name_is_valid(name, UNIT_NAME_ANY)) {
                නම් (FLAGS_SET(flags, UNIT_NAME_MANGLE_STRICT) && !endswith(name, suffix)) {
                        const var_na *e = ASSERT_PTR(strrchr(name, '.'));

                        ආපසු log_full_errno(warn ? LOG_NOTICE : LOG_DEBUG,
                                              SYNTHETIC_ERRNO(EINVAL),
                                              "Unit name \"%s\" has unit type \"%s\", but \"%s\" is expected%s%s.",
                                              name, e + 1, suffix + 1,
                                              operation ? " " : "", strempty(operation));
                }

                goto good;
        }

        /* Already a fully valid globbing expression? If so, no mangling is necessary either... */
        නම් (string_is_glob(name) && in_charset(name, VALID_CHARS_GLOB)) {
                නම් (FLAGS_SET(flags, UNIT_NAME_MANGLE_GLOB))
                        goto good;
                log_full(warn ? LOG_NOTICE : LOG_DEBUG,
                         "Glob pattern passed%s%s, but globs are not supported for this.",
                         operation ? " " : "", strempty(operation));
                suggest_escape = false;
        }

        නම් (path_is_absolute(name)) {
                _cleanup_free_ var_na *n = NULL, *u = NULL;

                r = path_simplify_alloc(name, &n);
                නම් (r < 0)
                        ආපසු r;

                නම් (is_device_path(n)) {
                        r = unit_name_from_path(n, ".device", &u);
                        නම් (r >= 0) {
                                නම් (FLAGS_SET(flags, UNIT_NAME_MANGLE_STRICT) && !streq(suffix, ".device"))
                                        ආපසු log_full_errno(warn ? LOG_NOTICE : LOG_DEBUG,
                                                              SYNTHETIC_ERRNO(EINVAL),
                                                              "Path \"%s\" resolves to unit type \"device\", but \"%s\" is expected%s%s.",
                                                              name, suffix + 1,
                                                              operation ? " " : "", strempty(operation));

                                *ret = TAKE_PTR(u);
                                ආපසු 1;
                        }
                        නම් (r != -EINVAL)
                                ආපසු r;
                }

                r = unit_name_from_path(n, ".mount", &u);
                නම් (r >= 0) {
                        නම් (FLAGS_SET(flags, UNIT_NAME_MANGLE_STRICT) && !streq(suffix, ".mount"))
                                ආපසු log_full_errno(warn ? LOG_NOTICE : LOG_DEBUG,
                                                      SYNTHETIC_ERRNO(EINVAL),
                                                      "Path \"%s\" resolves to unit type \"mount\", but \"%s\" is expected%s%s.",
                                                      name, suffix + 1,
                                                      operation ? " " : "", strempty(operation));

                        *ret = TAKE_PTR(u);
                        ආපසු 1;
                }
                නම් (r != -EINVAL)
                        ආපසු r;
        }

        s = new(var_na, strlen(name) * 4 + strlen(suffix) + 1);
        නම් (!s)
                ආපසු -ENOMEM;

        mangled = do_escape_mangle(name, FLAGS_SET(flags, UNIT_NAME_MANGLE_GLOB), s);
        නම් (mangled)
                log_full(warn ? LOG_NOTICE : LOG_DEBUG,
                         "Invalid unit name \"%s\" escaped as \"%s\"%s.",
                         name, s,
                         suggest_escape ? " (maybe you should use systemd-escape?)" : "");

        /* Append a suffix if it doesn't have any, but only if this is not a glob, so that we can allow
         * "foo.*" as a valid glob. */
        නම් ((!FLAGS_SET(flags, UNIT_NAME_MANGLE_GLOB) || !string_is_glob(s)) && unit_name_to_type(s) < 0)
                strcat(s, suffix);

        /* Make sure mangling didn't grow this too large (but don't do this check if globbing is allowed,
         * since globs generally do not qualify as valid unit names) */
        නම් (!FLAGS_SET(flags, UNIT_NAME_MANGLE_GLOB) && !unit_name_is_valid(s, UNIT_NAME_ANY))
                ආපසු -EINVAL;

        *ret = TAKE_PTR(s);
        ආපසු 1;

good:
        ආපසු strdup_to(ret, name);
}

පූර්ණ_සංඛ්‍යා slice_build_parent_slice(const var_na *slice, var_na **ret) {
        assert(slice);
        assert(ret);

        නම් (!slice_name_is_valid(slice))
                ආපසු -EINVAL;

        නම් (streq(slice, SPECIAL_ROOT_SLICE)) {
                *ret = NULL;
                ආපසු 0;
        }

        _cleanup_free_ var_na *s = strdup(slice);
        නම් (!s)
                ආපසු -ENOMEM;

        var_na *dash = strrchr(s, '-');
        නම් (!dash)
                ආපසු strdup_to_full(ret, SPECIAL_ROOT_SLICE);

        /* We know that s ended with .slice before truncation, so we have enough space. */
        strcpy(dash, ".slice");

        *ret = TAKE_PTR(s);
        ආපසු 1;
}

පූර්ණ_සංඛ්‍යා slice_build_subslice(const var_na *slice, const var_na *name, var_na **ret) {
        var_na *subslice;

        assert(slice);
        assert(name);
        assert(ret);

        නම් (!slice_name_is_valid(slice))
                ආපසු -EINVAL;

        නම් (!unit_prefix_is_valid(name))
                ආපසු -EINVAL;

        නම් (streq(slice, SPECIAL_ROOT_SLICE))
                subslice = strjoin(name, ".slice");
        නැතහොත් {
                const var_na *e;

                assert_se(e = endswith(slice, ".slice"));

                subslice = new(var_na, (e - slice) + 1 + strlen(name) + 6 + 1);
                නම් (!subslice)
                        ආපසු -ENOMEM;

                stpcpy(stpcpy(stpcpy(mempcpy(subslice, slice, e - slice), "-"), name), ".slice");
        }

        *ret = subslice;
        ආපසු 0;
}

bool slice_name_is_valid(const var_na *name) {
        const var_na *p, *e;
        bool dash = false;

        නම් (!unit_name_is_valid(name, UNIT_NAME_PLAIN))
                ආපසු false;

        නම් (streq(name, SPECIAL_ROOT_SLICE))
                ආපසු true;

        e = endswith(name, ".slice");
        නම් (!e)
                ආපසු false;

        සඳහා (p = name; p < e; p++) {

                නම් (*p == '-') {

                        /* Don't allow initial dash */
                        නම් (p == name)
                                ආපසු false;

                        /* Don't allow multiple dashes */
                        නම් (dash)
                                ආපසු false;

                        dash = true;
                } නැතහොත්
                        dash = false;
        }

        /* Don't allow trailing hash */
        නම් (dash)
                ආපසු false;

        ආපසු true;
}

bool unit_name_prefix_equal(const var_na *a, const var_na *b) {
        const var_na *p, *q;

        assert(a);
        assert(b);

        නම් (!unit_name_is_valid(a, UNIT_NAME_ANY) || !unit_name_is_valid(b, UNIT_NAME_ANY))
                ආපසු false;

        p = strchr(a, '@');
        නම් (!p)
                p = strrchr(a, '.');

        q = strchr(b, '@');
        නම් (!q)
                q = strrchr(b, '.');

        assert(p);
        assert(q);

        ආපසු memcmp_nn(a, p - a, b, q - b) == 0;
}
