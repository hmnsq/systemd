/* SPDX-License-Identifier: LGPL-2.1-or-later */

#شامل "sd-id128.h"

#شامل "alloc-util.h"
#شامل "glob-util.h"
#شامل "hexdecoct.h"
#شامل "log.h"
#شامل "memory-util.h"
#شامل "path-util.h"
#شامل "sparse-endian.h"
#شامل "special.h"
#شامل "siphash24.h"
#شامل "string-util.h"
#شامل "unit-def.h"
#شامل "unit-name.h"

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

#define LONG_UNIT_NAME_HASH_کلید SD_ID128_MAKE(ec,f2,37,fb,58,32,4a,32,84,9f,06,9b,0d,21,eb,9a)
#define UNIT_NAME_HASH_LENGTH_CHARS 16

bool unit_name_is_valid(ثابت نویسه *n, UnitNameFlags flags) {
        ثابت نویسه *e, *i, *at;

        أكد((flags & ~(UNIT_NAME_PLAIN|UNIT_NAME_INSTANCE|UNIT_NAME_TEMPLATE)) == 0);

        اگر (_unlikely_(flags == 0))
                بازگشت false;

        اگر (isempty(n))
                بازگشت false;

        اگر (strlen(n) >= UNIT_NAME_MAX)
                بازگشت false;

        e = strrchr(n, '.');
        اگر (!e || e == n)
                بازگشت false;

        اگر (unit_type_from_string(e + 1) < 0)
                بازگشت false;

        برای (i = n, at = پوچ; i < e; i++) {

                اگر (*i == '@' && !at)
                        at = i;

                اگر (!strchr(VALID_CHARS_WITH_AT, *i))
                        بازگشت false;
        }

        اگر (at == n)
                بازگشت false;

        اگر (flags & UNIT_NAME_PLAIN)
                اگر (!at)
                        بازگشت true;

        اگر (flags & UNIT_NAME_INSTANCE)
                اگر (at && e > at + 1)
                        بازگشت true;

        اگر (flags & UNIT_NAME_TEMPLATE)
                اگر (at && e == at + 1)
                        بازگشت true;

        بازگشت false;
}

bool unit_prefix_is_valid(ثابت نویسه *p) {

        /* We don't allow additional @ in the prefix string */

        اگر (isempty(p))
                بازگشت false;

        بازگشت in_charset(p, VALID_CHARS);
}

bool unit_instance_is_valid(ثابت نویسه *i) {

        /* The max length depends on the length of the string, so we
         * don't really check this here. */

        اگر (isempty(i))
                بازگشت false;

        /* We allow additional @ in the instance string, we do not
         * allow them in the prefix! */

        بازگشت in_charset(i, "@" VALID_CHARS);
}

bool unit_suffix_is_valid(ثابت نویسه *s) {
        اگر (isempty(s))
                بازگشت false;

        اگر (s[0] != '.')
                بازگشت false;

        اگر (unit_type_from_string(s + 1) < 0)
                بازگشت false;

        بازگشت true;
}

صحیح unit_name_to_prefix(ثابت نویسه *n, نویسه **ret) {
        ثابت نویسه *p;
        نویسه *s;

        أكد(n);
        أكد(ret);

        اگر (!unit_name_is_valid(n, UNIT_NAME_ANY))
                بازگشت -EINVAL;

        p = strchr(n, '@');
        اگر (!p)
                p = strrchr(n, '.');

        assert_se(p);

        s = strndup(n, p - n);
        اگر (!s)
                بازگشت -ENOMEM;

        *ret = s;
        بازگشت 0;
}

UnitNameFlags unit_name_to_instance(ثابت نویسه *n, نویسه **ret) {
        ثابت نویسه *p, *d;

        أكد(n);

        اگر (!unit_name_is_valid(n, UNIT_NAME_ANY))
                بازگشت -EINVAL;

        /* Everything past the first @ and before the last . is the instance */
        p = strchr(n, '@');
        اگر (!p) {
                اگر (ret)
                        *ret = پوچ;
                بازگشت UNIT_NAME_PLAIN;
        }

        p++;

        d = strrchr(p, '.');
        اگر (!d)
                بازگشت -EINVAL;

        اگر (ret) {
                نویسه *i = strndup(p, d-p);
                اگر (!i)
                        بازگشت -ENOMEM;

                *ret = i;
        }
        بازگشت d > p ? UNIT_NAME_INSTANCE : UNIT_NAME_TEMPLATE;
}

صحیح unit_name_to_prefix_and_instance(ثابت نویسه *n, نویسه **ret) {
        ثابت نویسه *d;
        نویسه *s;

        أكد(n);
        أكد(ret);

        اگر (!unit_name_is_valid(n, UNIT_NAME_ANY))
                بازگشت -EINVAL;

        d = strrchr(n, '.');
        اگر (!d)
                بازگشت -EINVAL;

        s = strndup(n, d - n);
        اگر (!s)
                بازگشت -ENOMEM;

        *ret = s;
        بازگشت 0;
}

UnitType unit_name_to_type(ثابت نویسه *n) {
        ثابت نویسه *e;

        أكد(n);

        اگر (!unit_name_is_valid(n, UNIT_NAME_ANY))
                بازگشت _UNIT_TYPE_INVALID;

        assert_se(e = strrchr(n, '.'));

        بازگشت unit_type_from_string(e + 1);
}

صحیح unit_name_change_suffix(ثابت نویسه *n, ثابت نویسه *suffix, نویسه **ret) {
        _cleanup_free_ نویسه *s = پوچ;
        size_t a, b;
        ثابت نویسه *e;

        أكد(n);
        أكد(suffix);
        أكد(ret);

        اگر (!unit_name_is_valid(n, UNIT_NAME_ANY))
                بازگشت -EINVAL;

        اگر (!unit_suffix_is_valid(suffix))
                بازگشت -EINVAL;

        assert_se(e = strrchr(n, '.'));

        a = e - n;
        b = strlen(suffix);

        s = new(نویسه, a + b + 1);
        اگر (!s)
                بازگشت -ENOMEM;

        strcpy(mempcpy(s, n, a), suffix);

        /* Make sure the name is still valid (i.e. didn't grow too large due to longer suffix) */
        اگر (!unit_name_is_valid(s, UNIT_NAME_ANY))
                بازگشت -EINVAL;

        *ret = TAKE_PTR(s);
        بازگشت 0;
}

صحیح unit_name_build(ثابت نویسه *prefix, ثابت نویسه *instance, ثابت نویسه *suffix, نویسه **ret) {
        UnitType type;

        أكد(prefix);
        أكد(suffix);
        أكد(ret);

        اگر (suffix[0] != '.')
                بازگشت -EINVAL;

        type = unit_type_from_string(suffix + 1);
        اگر (type < 0)
                بازگشت type;

        بازگشت unit_name_build_from_type(prefix, instance, type, ret);
}

صحیح unit_name_build_from_type(ثابت نویسه *prefix, ثابت نویسه *instance, UnitType type, نویسه **ret) {
        _cleanup_free_ نویسه *s = پوچ;
        ثابت نویسه *ut;

        أكد(prefix);
        أكد(type >= 0);
        أكد(type < _UNIT_TYPE_MAX);
        أكد(ret);

        اگر (!unit_prefix_is_valid(prefix))
                بازگشت -EINVAL;

        ut = unit_type_to_string(type);

        اگر (instance) {
                اگر (!unit_instance_is_valid(instance))
                        بازگشت -EINVAL;

                s = strjoin(prefix, "@", instance, ".", ut);
        } وگرنه
                s = strjoin(prefix, ".", ut);
        اگر (!s)
                بازگشت -ENOMEM;

        /* Verify that this didn't grow too large (or otherwise is invalid) */
        اگر (!unit_name_is_valid(s, instance ? UNIT_NAME_INSTANCE : UNIT_NAME_PLAIN))
                بازگشت -EINVAL;

        *ret = TAKE_PTR(s);
        بازگشت 0;
}

ایستا نویسه *do_escape_char(نویسه c, نویسه *t) {
        أكد(t);

        *(t++) = '\\';
        *(t++) = 'x';
        *(t++) = hexchar(c >> 4);
        *(t++) = hexchar(c);

        بازگشت t;
}

ایستا نویسه *do_escape(ثابت نویسه *f, نویسه *t) {
        أكد(f);
        أكد(t);

        /* do not create units with a leading '.', like for "/.dotdir" mount points */
        اگر (*f == '.') {
                t = do_escape_char(*f, t);
                f++;
        }

        برای (; *f; f++) {
                اگر (*f == '/')
                        *(t++) = '-';
                وگرنه اگر (IN_SET(*f, '-', '\\') || !strchr(VALID_CHARS, *f))
                        t = do_escape_char(*f, t);
                وگرنه
                        *(t++) = *f;
        }

        بازگشت t;
}

نویسه* unit_name_escape(ثابت نویسه *f) {
        نویسه *r, *t;

        أكد(f);

        r = new(نویسه, strlen(f)*4+1);
        اگر (!r)
                بازگشت پوچ;

        t = do_escape(f, r);
        *t = 0;

        بازگشت r;
}

صحیح unit_name_unescape(ثابت نویسه *f, نویسه **ret) {
        _cleanup_free_ نویسه *r = پوچ;
        نویسه *t;

        أكد(f);
        أكد(ret);

        r = strdup(f);
        اگر (!r)
                بازگشت -ENOMEM;

        برای (t = r; *f; f++) {
                اگر (*f == '-')
                        *(t++) = '/';
                وگرنه اگر (*f == '\\') {
                        صحیح a, b;

                        اگر (f[1] != 'x')
                                بازگشت -EINVAL;

                        a = unhexchar(f[2]);
                        اگر (a < 0)
                                بازگشت -EINVAL;

                        b = unhexchar(f[3]);
                        اگر (b < 0)
                                بازگشت -EINVAL;

                        *(t++) = (نویسه) (((uint8_t) a << 4U) | (uint8_t) b);
                        f += 3;
                } وگرنه
                        *(t++) = *f;
        }

        *t = 0;

        *ret = TAKE_PTR(r);

        بازگشت 0;
}

صحیح unit_name_path_escape(ثابت نویسه *f, نویسه **ret) {
        _cleanup_free_ نویسه *p = پوچ;
        نویسه *s;
        صحیح r;

        أكد(f);
        أكد(ret);

        r = path_simplify_alloc(f, &p);
        اگر (r < 0)
                بازگشت r;

        اگر (empty_or_ریشه(p))
                s = strdup("-");
        وگرنه {
                اگر (!path_is_normalized(p))
                        بازگشت -EINVAL;

                /* Truncate trailing slashes and skip leading slashes */
                delete_trailing_chars(p, "/");
                s = unit_name_escape(skip_leading_chars(p, "/"));
        }
        اگر (!s)
                بازگشت -ENOMEM;

        *ret = s;
        بازگشت 0;
}

صحیح unit_name_path_unescape(ثابت نویسه *f, نویسه **ret) {
        _cleanup_free_ نویسه *s = پوچ;
        صحیح r;

        أكد(f);

        اگر (isempty(f))
                بازگشت -EINVAL;

        اگر (streq(f, "-")) {
                s = strdup("/");
                اگر (!s)
                        بازگشت -ENOMEM;
        } وگرنه {
                _cleanup_free_ نویسه *w = پوچ;

                r = unit_name_unescape(f, &w);
                اگر (r < 0)
                        بازگشت r;

                /* Don't accept trailing or leading slashes */
                اگر (startswith(w, "/") || endswith(w, "/"))
                        بازگشت -EINVAL;

                /* Prefix a slash again */
                s = strjoin("/", w);
                اگر (!s)
                        بازگشت -ENOMEM;

                اگر (!path_is_normalized(s))
                        بازگشت -EINVAL;
        }

        اگر (ret)
                *ret = TAKE_PTR(s);

        بازگشت 0;
}

صحیح unit_name_replace_instance_full(
                ثابت نویسه *original,
                ثابت نویسه *instance,
                bool accept_glob,
                نویسه **ret) {

        _cleanup_free_ نویسه *s = پوچ;
        ثابت نویسه *prefix, *suffix;
        size_t pl;

        أكد(original);
        أكد(instance);
        أكد(ret);

        اگر (!unit_name_is_valid(original, UNIT_NAME_INSTANCE|UNIT_NAME_TEMPLATE))
                بازگشت -EINVAL;
        اگر (!unit_instance_is_valid(instance) && !(accept_glob && in_charset(instance, VALID_CHARS_GLOB)))
                بازگشت -EINVAL;

        prefix = ASSERT_PTR(strchr(original, '@'));
        suffix = ASSERT_PTR(strrchr(original, '.'));
        أكد(prefix < suffix);

        pl = prefix - original + 1; /* include '@' */

        s = new(نویسه, pl + strlen(instance) + strlen(suffix) + 1);
        اگر (!s)
                بازگشت -ENOMEM;

#اگر HAS_FEATURE_MEMORY_SANITIZER
        /* MSan doesn't like stpncpy... See also https://github.com/google/sanitizers/issues/926 */
        memzero(s, pl + strlen(instance) + strlen(suffix) + 1);
#endif

        strcpy(stpcpy(stpncpy(s, original, pl), instance), suffix);

        /* Make sure the resulting name still is valid, i.e. didn't grow too large. Globs will be expanded
         * by clients when used, so the check is pointless. */
        اگر (!accept_glob && !unit_name_is_valid(s, UNIT_NAME_INSTANCE))
                بازگشت -EINVAL;

        *ret = TAKE_PTR(s);
        بازگشت 0;
}

صحیح unit_name_template(ثابت نویسه *f, نویسه **ret) {
        ثابت نویسه *p, *e;
        نویسه *s;
        size_t a;

        أكد(f);
        أكد(ret);

        اگر (!unit_name_is_valid(f, UNIT_NAME_INSTANCE|UNIT_NAME_TEMPLATE))
                بازگشت -EINVAL;

        assert_se(p = strchr(f, '@'));
        assert_se(e = strrchr(f, '.'));

        a = p - f;

        s = new(نویسه, a + 1 + strlen(e) + 1);
        اگر (!s)
                بازگشت -ENOMEM;

        strcpy(mempcpy(s, f, a + 1), e);

        *ret = s;
        بازگشت 0;
}

bool unit_name_is_hashed(ثابت نویسه *name) {
        ثابت نویسه *s;

        اگر (!unit_name_is_valid(name, UNIT_NAME_PLAIN))
                بازگشت false;

        assert_se(s = strrchr(name, '.'));

        اگر (s - name < UNIT_NAME_HASH_LENGTH_CHARS + 1)
                بازگشت false;

        s -= UNIT_NAME_HASH_LENGTH_CHARS;
        اگر (s[-1] != '_')
                بازگشت false;

        برای (size_t i = 0; i < UNIT_NAME_HASH_LENGTH_CHARS; i++)
                اگر (!strchr(LOWERCASE_HEXDIGITS, s[i]))
                        بازگشت false;

        بازگشت true;
}

صحیح unit_name_hash_long(ثابت نویسه *name, نویسه **ret) {
        _cleanup_free_ نویسه *n = پوچ, *hash = پوچ;
        ثابت نویسه *suffix;
        le64_t h;
        size_t len;

        أكد(ret);

        اگر (strlen(name) < UNIT_NAME_MAX)
                بازگشت -EMSGSIZE;

        suffix = strrchr(name, '.');
        اگر (!suffix)
                بازگشت -EINVAL;

        اگر (unit_type_from_string(suffix+1) < 0)
                بازگشت -EINVAL;

        h = htole64(siphash24_string(name, LONG_UNIT_NAME_HASH_کلید.bytes));

        hash = hexmem(&h, اندازه(h));
        اگر (!hash)
                بازگشت -ENOMEM;

        assert_se(strlen(hash) == UNIT_NAME_HASH_LENGTH_CHARS);

        len = UNIT_NAME_MAX - 1 - strlen(suffix+1) - UNIT_NAME_HASH_LENGTH_CHARS - 2;
        أكد(len > 0 && len < UNIT_NAME_MAX);

        n = strndup(name, len);
        اگر (!n)
                بازگشت -ENOMEM;

        اگر (!strextend(&n, "_", hash, suffix))
                بازگشت -ENOMEM;
        assert_se(unit_name_is_valid(n, UNIT_NAME_PLAIN));

        *ret = TAKE_PTR(n);

        بازگشت 0;
}

صحیح unit_name_from_path(ثابت نویسه *path, ثابت نویسه *suffix, نویسه **ret) {
        _cleanup_free_ نویسه *p = پوچ, *s = پوچ;
        صحیح r;

        أكد(path);
        أكد(suffix);
        أكد(ret);

        اگر (!unit_suffix_is_valid(suffix))
                بازگشت -EINVAL;

        r = unit_name_path_escape(path, &p);
        اگر (r < 0)
                بازگشت r;

        s = strjoin(p, suffix);
        اگر (!s)
                بازگشت -ENOMEM;

        اگر (strlen(s) >= UNIT_NAME_MAX) {
                _cleanup_free_ نویسه *n = پوچ;

                log_debug("Unit name \"%s\" too long, falling back to hashed unit name.", s);

                r = unit_name_hash_long(s, &n);
                اگر (r < 0)
                        بازگشت r;

                free_and_replace(s, n);
        }

        /* Refuse if this for some other reason didn't result in a valid name */
        اگر (!unit_name_is_valid(s, UNIT_NAME_PLAIN))
                بازگشت -EINVAL;

        *ret = TAKE_PTR(s);
        بازگشت 0;
}

صحیح unit_name_from_path_instance(ثابت نویسه *prefix, ثابت نویسه *path, ثابت نویسه *suffix, نویسه **ret) {
        _cleanup_free_ نویسه *p = پوچ, *s = پوچ;
        صحیح r;

        أكد(prefix);
        أكد(path);
        أكد(suffix);
        أكد(ret);

        اگر (!unit_prefix_is_valid(prefix))
                بازگشت -EINVAL;

        اگر (!unit_suffix_is_valid(suffix))
                بازگشت -EINVAL;

        r = unit_name_path_escape(path, &p);
        اگر (r < 0)
                بازگشت r;

        s = strjoin(prefix, "@", p, suffix);
        اگر (!s)
                بازگشت -ENOMEM;

        اگر (strlen(s) >= UNIT_NAME_MAX) /* Return a slightly more descriptive error for this specific condition */
                بازگشت -ENAMETOOLONG;

        /* Refuse if this for some other reason didn't result in a valid name */
        اگر (!unit_name_is_valid(s, UNIT_NAME_INSTANCE))
                بازگشت -EINVAL;

        *ret = TAKE_PTR(s);
        بازگشت 0;
}

صحیح unit_name_to_path(ثابت نویسه *name, نویسه **ret) {
        _cleanup_free_ نویسه *prefix = پوچ;
        صحیح r;

        أكد(name);

        r = unit_name_to_prefix(name, &prefix);
        اگر (r < 0)
                بازگشت r;

        اگر (unit_name_is_hashed(name))
                بازگشت -ENAMETOOLONG;

        بازگشت unit_name_path_unescape(prefix, ret);
}

ایستا bool do_escape_mangle(ثابت نویسه *f, bool allow_globs, نویسه *t) {
        ثابت نویسه *valid_chars;
        bool mangled = false;

        أكد(f);
        أكد(t);

        /* We'll only escape the obvious characters here, to play safe.
         *
         * Returns true if any characters were mangled, false otherwise.
         */

        valid_chars = allow_globs ? VALID_CHARS_GLOB : VALID_CHARS_WITH_AT;

        برای (; *f; f++)
                اگر (*f == '/') {
                        *(t++) = '-';
                        mangled = true;
                } وگرنه اگر (!strchr(valid_chars, *f)) {
                        t = do_escape_char(*f, t);
                        mangled = true;
                } وگرنه
                        *(t++) = *f;
        *t = 0;

        بازگشت mangled;
}

/**
 *  Convert a string to a unit name. /dev/blah is converted to dev-blah.device,
 *  /blah/blah is converted to blah-blah.mount, anything else is left alone,
 *  except that @suffix is appended if a valid unit suffix is not present.
 *
 *  If @allow_globs, globs characters are preserved. Otherwise, they are escaped.
 */
صحیح unit_name_mangle_with_suffix(
                ثابت نویسه *name,
                ثابت نویسه *operation,
                UnitNameMangle flags,
                ثابت نویسه *suffix,
                نویسه **ret) {

        _cleanup_free_ نویسه *s = پوچ;
        bool mangled, suggest_escape = true, warn = FLAGS_SET(flags, UNIT_NAME_MANGLE_WARN);
        صحیح r;

        أكد(name);
        أكد(suffix);
        أكد(ret);

        اگر (isempty(name)) /* We cannot mangle empty unit names to become valid, sorry. */
                بازگشت -EINVAL;

        اگر (!unit_suffix_is_valid(suffix))
                بازگشت -EINVAL;

        /* Already a fully valid unit name? If so, no mangling is necessary... */
        اگر (unit_name_is_valid(name, UNIT_NAME_ANY)) {
                اگر (FLAGS_SET(flags, UNIT_NAME_MANGLE_STRICT) && !endswith(name, suffix)) {
                        ثابت نویسه *e = ASSERT_PTR(strrchr(name, '.'));

                        بازگشت log_full_errno(warn ? LOG_NOTICE : LOG_DEBUG,
                                              SYNTHETIC_ERRNO(EINVAL),
                                              "Unit name \"%s\" has unit type \"%s\", but \"%s\" is expected%s%s.",
                                              name, e + 1, suffix + 1,
                                              operation ? " " : "", strempty(operation));
                }

                برو good;
        }

        /* Already a fully valid globbing expression? If so, no mangling is necessary either... */
        اگر (string_is_glob(name) && in_charset(name, VALID_CHARS_GLOB)) {
                اگر (FLAGS_SET(flags, UNIT_NAME_MANGLE_GLOB))
                        برو good;
                log_full(warn ? LOG_NOTICE : LOG_DEBUG,
                         "Glob pattern passed%s%s, but globs are not supported for this.",
                         operation ? " " : "", strempty(operation));
                suggest_escape = false;
        }

        اگر (path_is_absolute(name)) {
                _cleanup_free_ نویسه *n = پوچ, *u = پوچ;

                r = path_simplify_alloc(name, &n);
                اگر (r < 0)
                        بازگشت r;

                اگر (is_device_path(n)) {
                        r = unit_name_from_path(n, ".device", &u);
                        اگر (r >= 0) {
                                اگر (FLAGS_SET(flags, UNIT_NAME_MANGLE_STRICT) && !streq(suffix, ".device"))
                                        بازگشت log_full_errno(warn ? LOG_NOTICE : LOG_DEBUG,
                                                              SYNTHETIC_ERRNO(EINVAL),
                                                              "Path \"%s\" resolves to unit type \"device\", but \"%s\" is expected%s%s.",
                                                              name, suffix + 1,
                                                              operation ? " " : "", strempty(operation));

                                *ret = TAKE_PTR(u);
                                بازگشت 1;
                        }
                        اگر (r != -EINVAL)
                                بازگشت r;
                }

                r = unit_name_from_path(n, ".mount", &u);
                اگر (r >= 0) {
                        اگر (FLAGS_SET(flags, UNIT_NAME_MANGLE_STRICT) && !streq(suffix, ".mount"))
                                بازگشت log_full_errno(warn ? LOG_NOTICE : LOG_DEBUG,
                                                      SYNTHETIC_ERRNO(EINVAL),
                                                      "Path \"%s\" resolves to unit type \"mount\", but \"%s\" is expected%s%s.",
                                                      name, suffix + 1,
                                                      operation ? " " : "", strempty(operation));

                        *ret = TAKE_PTR(u);
                        بازگشت 1;
                }
                اگر (r != -EINVAL)
                        بازگشت r;
        }

        s = new(نویسه, strlen(name) * 4 + strlen(suffix) + 1);
        اگر (!s)
                بازگشت -ENOMEM;

        mangled = do_escape_mangle(name, FLAGS_SET(flags, UNIT_NAME_MANGLE_GLOB), s);
        اگر (mangled)
                log_full(warn ? LOG_NOTICE : LOG_DEBUG,
                         "Invalid unit name \"%s\" escaped as \"%s\"%s.",
                         name, s,
                         suggest_escape ? " (maybe you should use systemd-escape?)" : "");

        /* Append a suffix if it doesn't have any, but only if this is not a glob, so that we can allow
         * "foo.*" as a valid glob. */
        اگر ((!FLAGS_SET(flags, UNIT_NAME_MANGLE_GLOB) || !string_is_glob(s)) && unit_name_to_type(s) < 0)
                strcat(s, suffix);

        /* Make sure mangling didn't grow this too large (but don't do this check if globbing is allowed,
         * since globs generally do not qualify as valid unit names) */
        اگر (!FLAGS_SET(flags, UNIT_NAME_MANGLE_GLOB) && !unit_name_is_valid(s, UNIT_NAME_ANY))
                بازگشت -EINVAL;

        *ret = TAKE_PTR(s);
        بازگشت 1;

good:
        بازگشت strdup_to(ret, name);
}

صحیح slice_build_parent_slice(ثابت نویسه *slice, نویسه **ret) {
        أكد(slice);
        أكد(ret);

        اگر (!slice_name_is_valid(slice))
                بازگشت -EINVAL;

        اگر (streq(slice, SPECIAL_ریشه_SLICE)) {
                *ret = پوچ;
                بازگشت 0;
        }

        _cleanup_free_ نویسه *s = strdup(slice);
        اگر (!s)
                بازگشت -ENOMEM;

        نویسه *dash = strrchr(s, '-');
        اگر (!dash)
                بازگشت strdup_to_full(ret, SPECIAL_ریشه_SLICE);

        /* We know that s ended with .slice before truncation, so we have enough space. */
        strcpy(dash, ".slice");

        *ret = TAKE_PTR(s);
        بازگشت 1;
}

صحیح slice_build_subslice(ثابت نویسه *slice, ثابت نویسه *name, نویسه **ret) {
        نویسه *subslice;

        أكد(slice);
        أكد(name);
        أكد(ret);

        اگر (!slice_name_is_valid(slice))
                بازگشت -EINVAL;

        اگر (!unit_prefix_is_valid(name))
                بازگشت -EINVAL;

        اگر (streq(slice, SPECIAL_ریشه_SLICE))
                subslice = strjoin(name, ".slice");
        وگرنه {
                ثابت نویسه *e;

                assert_se(e = endswith(slice, ".slice"));

                subslice = new(نویسه, (e - slice) + 1 + strlen(name) + 6 + 1);
                اگر (!subslice)
                        بازگشت -ENOMEM;

                stpcpy(stpcpy(stpcpy(mempcpy(subslice, slice, e - slice), "-"), name), ".slice");
        }

        *ret = subslice;
        بازگشت 0;
}

bool slice_name_is_valid(ثابت نویسه *name) {
        ثابت نویسه *p, *e;
        bool dash = false;

        اگر (!unit_name_is_valid(name, UNIT_NAME_PLAIN))
                بازگشت false;

        اگر (streq(name, SPECIAL_ریشه_SLICE))
                بازگشت true;

        e = endswith(name, ".slice");
        اگر (!e)
                بازگشت false;

        برای (p = name; p < e; p++) {

                اگر (*p == '-') {

                        /* Don't allow initial dash */
                        اگر (p == name)
                                بازگشت false;

                        /* Don't allow multiple dashes */
                        اگر (dash)
                                بازگشت false;

                        dash = true;
                } وگرنه
                        dash = false;
        }

        /* Don't allow trailing hash */
        اگر (dash)
                بازگشت false;

        بازگشت true;
}

bool unit_name_prefix_equal(ثابت نویسه *a, ثابت نویسه *b) {
        ثابت نویسه *p, *q;

        أكد(a);
        أكد(b);

        اگر (!unit_name_is_valid(a, UNIT_NAME_ANY) || !unit_name_is_valid(b, UNIT_NAME_ANY))
                بازگشت false;

        p = strchr(a, '@');
        اگر (!p)
                p = strrchr(a, '.');

        q = strchr(b, '@');
        اگر (!q)
                q = strrchr(b, '.');

        أكد(p);
        أكد(q);

        بازگشت memcmp_nn(a, p - a, b, q - b) == 0;
}
