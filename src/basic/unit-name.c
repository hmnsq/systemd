/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव "sd-id128.h"

#अन्तर्भाव "alloc-util.h"
#अन्तर्भाव "glob-util.h"
#अन्तर्भाव "hexdecoct.h"
#अन्तर्भाव "log.h"
#अन्तर्भाव "memory-util.h"
#अन्तर्भाव "path-util.h"
#अन्तर्भाव "sparse-endian.h"
#अन्तर्भाव "special.h"
#अन्तर्भाव "siphash24.h"
#अन्तर्भाव "string-util.h"
#अन्तर्भाव "unit-def.h"
#अन्तर्भाव "unit-name.h"

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

#define LONG_UNIT_NAME_HASH_किल्ली SD_ID128_MAKE(ec,f2,37,fb,58,32,4a,32,84,9f,06,9b,0d,21,eb,9a)
#define UNIT_NAME_HASH_LENGTH_CHARS 16

bool unit_name_is_valid(स्थिरांक अक्षर *n, UnitNameFlags flags) {
        स्थिरांक अक्षर *e, *i, *at;

        प्रतिज्ञा((flags & ~(UNIT_NAME_PLAIN|UNIT_NAME_INSTANCE|UNIT_NAME_TEMPLATE)) == 0);

        जर (_unlikely_(flags == 0))
                परत false;

        जर (isempty(n))
                परत false;

        जर (म_माप(n) >= UNIT_NAME_MAX)
                परत false;

        e = म_खोजप(n, '.');
        जर (!e || e == n)
                परत false;

        जर (unit_type_from_string(e + 1) < 0)
                परत false;

        साठी (i = n, at = रिक्त; i < e; i++) {

                जर (*i == '@' && !at)
                        at = i;

                जर (!म_अक्षर(VALID_CHARS_WITH_AT, *i))
                        परत false;
        }

        जर (at == n)
                परत false;

        जर (flags & UNIT_NAME_PLAIN)
                जर (!at)
                        परत true;

        जर (flags & UNIT_NAME_INSTANCE)
                जर (at && e > at + 1)
                        परत true;

        जर (flags & UNIT_NAME_TEMPLATE)
                जर (at && e == at + 1)
                        परत true;

        परत false;
}

bool unit_prefix_is_valid(स्थिरांक अक्षर *p) {

        /* We don't allow additional @ in the prefix string */

        जर (isempty(p))
                परत false;

        परत in_charset(p, VALID_CHARS);
}

bool unit_instance_is_valid(स्थिरांक अक्षर *i) {

        /* The max length depends on the length of the string, so we
         * don't really check this here. */

        जर (isempty(i))
                परत false;

        /* We allow additional @ in the instance string, we do not
         * allow them in the prefix! */

        परत in_charset(i, "@" VALID_CHARS);
}

bool unit_suffix_is_valid(स्थिरांक अक्षर *s) {
        जर (isempty(s))
                परत false;

        जर (s[0] != '.')
                परत false;

        जर (unit_type_from_string(s + 1) < 0)
                परत false;

        परत true;
}

पूर्णांक unit_name_to_prefix(स्थिरांक अक्षर *n, अक्षर **ret) {
        स्थिरांक अक्षर *p;
        अक्षर *s;

        प्रतिज्ञा(n);
        प्रतिज्ञा(ret);

        जर (!unit_name_is_valid(n, UNIT_NAME_ANY))
                परत -EINVAL;

        p = म_अक्षर(n, '@');
        जर (!p)
                p = म_खोजप(n, '.');

        assert_se(p);

        s = strndup(n, p - n);
        जर (!s)
                परत -ENOMEM;

        *ret = s;
        परत 0;
}

UnitNameFlags unit_name_to_instance(स्थिरांक अक्षर *n, अक्षर **ret) {
        स्थिरांक अक्षर *p, *d;

        प्रतिज्ञा(n);

        जर (!unit_name_is_valid(n, UNIT_NAME_ANY))
                परत -EINVAL;

        /* Everything past the first @ and before the last . is the instance */
        p = म_अक्षर(n, '@');
        जर (!p) {
                जर (ret)
                        *ret = रिक्त;
                परत UNIT_NAME_PLAIN;
        }

        p++;

        d = म_खोजप(p, '.');
        जर (!d)
                परत -EINVAL;

        जर (ret) {
                अक्षर *i = strndup(p, d-p);
                जर (!i)
                        परत -ENOMEM;

                *ret = i;
        }
        परत d > p ? UNIT_NAME_INSTANCE : UNIT_NAME_TEMPLATE;
}

पूर्णांक unit_name_to_prefix_and_instance(स्थिरांक अक्षर *n, अक्षर **ret) {
        स्थिरांक अक्षर *d;
        अक्षर *s;

        प्रतिज्ञा(n);
        प्रतिज्ञा(ret);

        जर (!unit_name_is_valid(n, UNIT_NAME_ANY))
                परत -EINVAL;

        d = म_खोजप(n, '.');
        जर (!d)
                परत -EINVAL;

        s = strndup(n, d - n);
        जर (!s)
                परत -ENOMEM;

        *ret = s;
        परत 0;
}

UnitType unit_name_to_type(स्थिरांक अक्षर *n) {
        स्थिरांक अक्षर *e;

        प्रतिज्ञा(n);

        जर (!unit_name_is_valid(n, UNIT_NAME_ANY))
                परत _UNIT_TYPE_INVALID;

        assert_se(e = म_खोजप(n, '.'));

        परत unit_type_from_string(e + 1);
}

पूर्णांक unit_name_change_suffix(स्थिरांक अक्षर *n, स्थिरांक अक्षर *suffix, अक्षर **ret) {
        _cleanup_free_ अक्षर *s = रिक्त;
        माप_प्रकार a, b;
        स्थिरांक अक्षर *e;

        प्रतिज्ञा(n);
        प्रतिज्ञा(suffix);
        प्रतिज्ञा(ret);

        जर (!unit_name_is_valid(n, UNIT_NAME_ANY))
                परत -EINVAL;

        जर (!unit_suffix_is_valid(suffix))
                परत -EINVAL;

        assert_se(e = म_खोजप(n, '.'));

        a = e - n;
        b = म_माप(suffix);

        s = new(अक्षर, a + b + 1);
        जर (!s)
                परत -ENOMEM;

        म_नकल(mempcpy(s, n, a), suffix);

        /* Make sure the name is still valid (i.e. didn't grow too large due to longer suffix) */
        जर (!unit_name_is_valid(s, UNIT_NAME_ANY))
                परत -EINVAL;

        *ret = TAKE_PTR(s);
        परत 0;
}

पूर्णांक unit_name_build(स्थिरांक अक्षर *prefix, स्थिरांक अक्षर *instance, स्थिरांक अक्षर *suffix, अक्षर **ret) {
        UnitType type;

        प्रतिज्ञा(prefix);
        प्रतिज्ञा(suffix);
        प्रतिज्ञा(ret);

        जर (suffix[0] != '.')
                परत -EINVAL;

        type = unit_type_from_string(suffix + 1);
        जर (type < 0)
                परत type;

        परत unit_name_build_from_type(prefix, instance, type, ret);
}

पूर्णांक unit_name_build_from_type(स्थिरांक अक्षर *prefix, स्थिरांक अक्षर *instance, UnitType type, अक्षर **ret) {
        _cleanup_free_ अक्षर *s = रिक्त;
        स्थिरांक अक्षर *ut;

        प्रतिज्ञा(prefix);
        प्रतिज्ञा(type >= 0);
        प्रतिज्ञा(type < _UNIT_TYPE_MAX);
        प्रतिज्ञा(ret);

        जर (!unit_prefix_is_valid(prefix))
                परत -EINVAL;

        ut = unit_type_to_string(type);

        जर (instance) {
                जर (!unit_instance_is_valid(instance))
                        परत -EINVAL;

                s = strjoin(prefix, "@", instance, ".", ut);
        } अन्यथा
                s = strjoin(prefix, ".", ut);
        जर (!s)
                परत -ENOMEM;

        /* Verify that this didn't grow too large (or otherwise is invalid) */
        जर (!unit_name_is_valid(s, instance ? UNIT_NAME_INSTANCE : UNIT_NAME_PLAIN))
                परत -EINVAL;

        *ret = TAKE_PTR(s);
        परत 0;
}

स्थिर अक्षर *do_escape_char(अक्षर c, अक्षर *t) {
        प्रतिज्ञा(t);

        *(t++) = '\\';
        *(t++) = 'x';
        *(t++) = hexchar(c >> 4);
        *(t++) = hexchar(c);

        परत t;
}

स्थिर अक्षर *do_escape(स्थिरांक अक्षर *f, अक्षर *t) {
        प्रतिज्ञा(f);
        प्रतिज्ञा(t);

        /* do not create units with a leading '.', like for "/.dotdir" mount points */
        जर (*f == '.') {
                t = do_escape_char(*f, t);
                f++;
        }

        साठी (; *f; f++) {
                जर (*f == '/')
                        *(t++) = '-';
                अन्यथा जर (IN_SET(*f, '-', '\\') || !म_अक्षर(VALID_CHARS, *f))
                        t = do_escape_char(*f, t);
                अन्यथा
                        *(t++) = *f;
        }

        परत t;
}

अक्षर* unit_name_escape(स्थिरांक अक्षर *f) {
        अक्षर *r, *t;

        प्रतिज्ञा(f);

        r = new(अक्षर, म_माप(f)*4+1);
        जर (!r)
                परत रिक्त;

        t = do_escape(f, r);
        *t = 0;

        परत r;
}

पूर्णांक unit_name_unescape(स्थिरांक अक्षर *f, अक्षर **ret) {
        _cleanup_free_ अक्षर *r = रिक्त;
        अक्षर *t;

        प्रतिज्ञा(f);
        प्रतिज्ञा(ret);

        r = strdup(f);
        जर (!r)
                परत -ENOMEM;

        साठी (t = r; *f; f++) {
                जर (*f == '-')
                        *(t++) = '/';
                अन्यथा जर (*f == '\\') {
                        पूर्णांक a, b;

                        जर (f[1] != 'x')
                                परत -EINVAL;

                        a = unhexchar(f[2]);
                        जर (a < 0)
                                परत -EINVAL;

                        b = unhexchar(f[3]);
                        जर (b < 0)
                                परत -EINVAL;

                        *(t++) = (अक्षर) (((uint8_t) a << 4U) | (uint8_t) b);
                        f += 3;
                } अन्यथा
                        *(t++) = *f;
        }

        *t = 0;

        *ret = TAKE_PTR(r);

        परत 0;
}

पूर्णांक unit_name_path_escape(स्थिरांक अक्षर *f, अक्षर **ret) {
        _cleanup_free_ अक्षर *p = रिक्त;
        अक्षर *s;
        पूर्णांक r;

        प्रतिज्ञा(f);
        प्रतिज्ञा(ret);

        r = path_simplify_alloc(f, &p);
        जर (r < 0)
                परत r;

        जर (empty_or_मूळ(p))
                s = strdup("-");
        अन्यथा {
                जर (!path_is_normalized(p))
                        परत -EINVAL;

                /* Truncate trailing slashes and skip leading slashes */
                delete_trailing_chars(p, "/");
                s = unit_name_escape(skip_leading_chars(p, "/"));
        }
        जर (!s)
                परत -ENOMEM;

        *ret = s;
        परत 0;
}

पूर्णांक unit_name_path_unescape(स्थिरांक अक्षर *f, अक्षर **ret) {
        _cleanup_free_ अक्षर *s = रिक्त;
        पूर्णांक r;

        प्रतिज्ञा(f);

        जर (isempty(f))
                परत -EINVAL;

        जर (streq(f, "-")) {
                s = strdup("/");
                जर (!s)
                        परत -ENOMEM;
        } अन्यथा {
                _cleanup_free_ अक्षर *w = रिक्त;

                r = unit_name_unescape(f, &w);
                जर (r < 0)
                        परत r;

                /* Don't accept trailing or leading slashes */
                जर (startswith(w, "/") || endswith(w, "/"))
                        परत -EINVAL;

                /* Prefix a slash again */
                s = strjoin("/", w);
                जर (!s)
                        परत -ENOMEM;

                जर (!path_is_normalized(s))
                        परत -EINVAL;
        }

        जर (ret)
                *ret = TAKE_PTR(s);

        परत 0;
}

पूर्णांक unit_name_replace_instance_full(
                स्थिरांक अक्षर *original,
                स्थिरांक अक्षर *instance,
                bool accept_glob,
                अक्षर **ret) {

        _cleanup_free_ अक्षर *s = रिक्त;
        स्थिरांक अक्षर *prefix, *suffix;
        माप_प्रकार pl;

        प्रतिज्ञा(original);
        प्रतिज्ञा(instance);
        प्रतिज्ञा(ret);

        जर (!unit_name_is_valid(original, UNIT_NAME_INSTANCE|UNIT_NAME_TEMPLATE))
                परत -EINVAL;
        जर (!unit_instance_is_valid(instance) && !(accept_glob && in_charset(instance, VALID_CHARS_GLOB)))
                परत -EINVAL;

        prefix = ASSERT_PTR(म_अक्षर(original, '@'));
        suffix = ASSERT_PTR(म_खोजप(original, '.'));
        प्रतिज्ञा(prefix < suffix);

        pl = prefix - original + 1; /* include '@' */

        s = new(अक्षर, pl + म_माप(instance) + म_माप(suffix) + 1);
        जर (!s)
                परत -ENOMEM;

#जर HAS_FEATURE_MEMORY_SANITIZER
        /* MSan doesn't like stpncpy... See also https://github.com/google/sanitizers/issues/926 */
        memzero(s, pl + म_माप(instance) + म_माप(suffix) + 1);
#endif

        म_नकल(stpcpy(stpncpy(s, original, pl), instance), suffix);

        /* Make sure the resulting name still is valid, i.e. didn't grow too large. Globs will be expanded
         * by clients when used, so the check is pointless. */
        जर (!accept_glob && !unit_name_is_valid(s, UNIT_NAME_INSTANCE))
                परत -EINVAL;

        *ret = TAKE_PTR(s);
        परत 0;
}

पूर्णांक unit_name_template(स्थिरांक अक्षर *f, अक्षर **ret) {
        स्थिरांक अक्षर *p, *e;
        अक्षर *s;
        माप_प्रकार a;

        प्रतिज्ञा(f);
        प्रतिज्ञा(ret);

        जर (!unit_name_is_valid(f, UNIT_NAME_INSTANCE|UNIT_NAME_TEMPLATE))
                परत -EINVAL;

        assert_se(p = म_अक्षर(f, '@'));
        assert_se(e = म_खोजप(f, '.'));

        a = p - f;

        s = new(अक्षर, a + 1 + म_माप(e) + 1);
        जर (!s)
                परत -ENOMEM;

        म_नकल(mempcpy(s, f, a + 1), e);

        *ret = s;
        परत 0;
}

bool unit_name_is_hashed(स्थिरांक अक्षर *name) {
        स्थिरांक अक्षर *s;

        जर (!unit_name_is_valid(name, UNIT_NAME_PLAIN))
                परत false;

        assert_se(s = म_खोजप(name, '.'));

        जर (s - name < UNIT_NAME_HASH_LENGTH_CHARS + 1)
                परत false;

        s -= UNIT_NAME_HASH_LENGTH_CHARS;
        जर (s[-1] != '_')
                परत false;

        साठी (माप_प्रकार i = 0; i < UNIT_NAME_HASH_LENGTH_CHARS; i++)
                जर (!म_अक्षर(LOWERCASE_HEXDIGITS, s[i]))
                        परत false;

        परत true;
}

पूर्णांक unit_name_hash_long(स्थिरांक अक्षर *name, अक्षर **ret) {
        _cleanup_free_ अक्षर *n = रिक्त, *hash = रिक्त;
        स्थिरांक अक्षर *suffix;
        le64_t h;
        माप_प्रकार len;

        प्रतिज्ञा(ret);

        जर (म_माप(name) < UNIT_NAME_MAX)
                परत -EMSGSIZE;

        suffix = म_खोजप(name, '.');
        जर (!suffix)
                परत -EINVAL;

        जर (unit_type_from_string(suffix+1) < 0)
                परत -EINVAL;

        h = htole64(siphash24_string(name, LONG_UNIT_NAME_HASH_किल्ली.bytes));

        hash = hexmem(&h, आकार(h));
        जर (!hash)
                परत -ENOMEM;

        assert_se(म_माप(hash) == UNIT_NAME_HASH_LENGTH_CHARS);

        len = UNIT_NAME_MAX - 1 - म_माप(suffix+1) - UNIT_NAME_HASH_LENGTH_CHARS - 2;
        प्रतिज्ञा(len > 0 && len < UNIT_NAME_MAX);

        n = strndup(name, len);
        जर (!n)
                परत -ENOMEM;

        जर (!strextend(&n, "_", hash, suffix))
                परत -ENOMEM;
        assert_se(unit_name_is_valid(n, UNIT_NAME_PLAIN));

        *ret = TAKE_PTR(n);

        परत 0;
}

पूर्णांक unit_name_from_path(स्थिरांक अक्षर *path, स्थिरांक अक्षर *suffix, अक्षर **ret) {
        _cleanup_free_ अक्षर *p = रिक्त, *s = रिक्त;
        पूर्णांक r;

        प्रतिज्ञा(path);
        प्रतिज्ञा(suffix);
        प्रतिज्ञा(ret);

        जर (!unit_suffix_is_valid(suffix))
                परत -EINVAL;

        r = unit_name_path_escape(path, &p);
        जर (r < 0)
                परत r;

        s = strjoin(p, suffix);
        जर (!s)
                परत -ENOMEM;

        जर (म_माप(s) >= UNIT_NAME_MAX) {
                _cleanup_free_ अक्षर *n = रिक्त;

                log_debug("Unit name \"%s\" too long, falling back to hashed unit name.", s);

                r = unit_name_hash_long(s, &n);
                जर (r < 0)
                        परत r;

                free_and_replace(s, n);
        }

        /* Refuse if this for some other reason didn't result in a valid name */
        जर (!unit_name_is_valid(s, UNIT_NAME_PLAIN))
                परत -EINVAL;

        *ret = TAKE_PTR(s);
        परत 0;
}

पूर्णांक unit_name_from_path_instance(स्थिरांक अक्षर *prefix, स्थिरांक अक्षर *path, स्थिरांक अक्षर *suffix, अक्षर **ret) {
        _cleanup_free_ अक्षर *p = रिक्त, *s = रिक्त;
        पूर्णांक r;

        प्रतिज्ञा(prefix);
        प्रतिज्ञा(path);
        प्रतिज्ञा(suffix);
        प्रतिज्ञा(ret);

        जर (!unit_prefix_is_valid(prefix))
                परत -EINVAL;

        जर (!unit_suffix_is_valid(suffix))
                परत -EINVAL;

        r = unit_name_path_escape(path, &p);
        जर (r < 0)
                परत r;

        s = strjoin(prefix, "@", p, suffix);
        जर (!s)
                परत -ENOMEM;

        जर (म_माप(s) >= UNIT_NAME_MAX) /* Return a slightly more descriptive error for this specific condition */
                परत -ENAMETOOLONG;

        /* Refuse if this for some other reason didn't result in a valid name */
        जर (!unit_name_is_valid(s, UNIT_NAME_INSTANCE))
                परत -EINVAL;

        *ret = TAKE_PTR(s);
        परत 0;
}

पूर्णांक unit_name_to_path(स्थिरांक अक्षर *name, अक्षर **ret) {
        _cleanup_free_ अक्षर *prefix = रिक्त;
        पूर्णांक r;

        प्रतिज्ञा(name);

        r = unit_name_to_prefix(name, &prefix);
        जर (r < 0)
                परत r;

        जर (unit_name_is_hashed(name))
                परत -ENAMETOOLONG;

        परत unit_name_path_unescape(prefix, ret);
}

स्थिर bool do_escape_mangle(स्थिरांक अक्षर *f, bool allow_globs, अक्षर *t) {
        स्थिरांक अक्षर *valid_chars;
        bool mangled = false;

        प्रतिज्ञा(f);
        प्रतिज्ञा(t);

        /* We'll only escape the obvious characters here, to play safe.
         *
         * Returns true if any characters were mangled, false otherwise.
         */

        valid_chars = allow_globs ? VALID_CHARS_GLOB : VALID_CHARS_WITH_AT;

        साठी (; *f; f++)
                जर (*f == '/') {
                        *(t++) = '-';
                        mangled = true;
                } अन्यथा जर (!म_अक्षर(valid_chars, *f)) {
                        t = do_escape_char(*f, t);
                        mangled = true;
                } अन्यथा
                        *(t++) = *f;
        *t = 0;

        परत mangled;
}

/**
 *  Convert a string to a unit name. /dev/blah is converted to dev-blah.device,
 *  /blah/blah is converted to blah-blah.mount, anything else is left alone,
 *  except that @suffix is appended if a valid unit suffix is not present.
 *
 *  If @allow_globs, globs characters are preserved. Otherwise, they are escaped.
 */
पूर्णांक unit_name_mangle_with_suffix(
                स्थिरांक अक्षर *name,
                स्थिरांक अक्षर *operation,
                UnitNameMangle flags,
                स्थिरांक अक्षर *suffix,
                अक्षर **ret) {

        _cleanup_free_ अक्षर *s = रिक्त;
        bool mangled, suggest_escape = true, warn = FLAGS_SET(flags, UNIT_NAME_MANGLE_WARN);
        पूर्णांक r;

        प्रतिज्ञा(name);
        प्रतिज्ञा(suffix);
        प्रतिज्ञा(ret);

        जर (isempty(name)) /* We cannot mangle empty unit names to become valid, sorry. */
                परत -EINVAL;

        जर (!unit_suffix_is_valid(suffix))
                परत -EINVAL;

        /* Already a fully valid unit name? If so, no mangling is necessary... */
        जर (unit_name_is_valid(name, UNIT_NAME_ANY)) {
                जर (FLAGS_SET(flags, UNIT_NAME_MANGLE_STRICT) && !endswith(name, suffix)) {
                        स्थिरांक अक्षर *e = ASSERT_PTR(म_खोजप(name, '.'));

                        परत log_full_errno(warn ? LOG_NOTICE : LOG_DEBUG,
                                              SYNTHETIC_ERRNO(EINVAL),
                                              "Unit name \"%s\" has unit type \"%s\", but \"%s\" is expected%s%s.",
                                              name, e + 1, suffix + 1,
                                              operation ? " " : "", strempty(operation));
                }

                जा good;
        }

        /* Already a fully valid globbing expression? If so, no mangling is necessary either... */
        जर (string_is_glob(name) && in_charset(name, VALID_CHARS_GLOB)) {
                जर (FLAGS_SET(flags, UNIT_NAME_MANGLE_GLOB))
                        जा good;
                log_full(warn ? LOG_NOTICE : LOG_DEBUG,
                         "Glob pattern passed%s%s, but globs are not supported for this.",
                         operation ? " " : "", strempty(operation));
                suggest_escape = false;
        }

        जर (path_is_absolute(name)) {
                _cleanup_free_ अक्षर *n = रिक्त, *u = रिक्त;

                r = path_simplify_alloc(name, &n);
                जर (r < 0)
                        परत r;

                जर (is_device_path(n)) {
                        r = unit_name_from_path(n, ".device", &u);
                        जर (r >= 0) {
                                जर (FLAGS_SET(flags, UNIT_NAME_MANGLE_STRICT) && !streq(suffix, ".device"))
                                        परत log_full_errno(warn ? LOG_NOTICE : LOG_DEBUG,
                                                              SYNTHETIC_ERRNO(EINVAL),
                                                              "Path \"%s\" resolves to unit type \"device\", but \"%s\" is expected%s%s.",
                                                              name, suffix + 1,
                                                              operation ? " " : "", strempty(operation));

                                *ret = TAKE_PTR(u);
                                परत 1;
                        }
                        जर (r != -EINVAL)
                                परत r;
                }

                r = unit_name_from_path(n, ".mount", &u);
                जर (r >= 0) {
                        जर (FLAGS_SET(flags, UNIT_NAME_MANGLE_STRICT) && !streq(suffix, ".mount"))
                                परत log_full_errno(warn ? LOG_NOTICE : LOG_DEBUG,
                                                      SYNTHETIC_ERRNO(EINVAL),
                                                      "Path \"%s\" resolves to unit type \"mount\", but \"%s\" is expected%s%s.",
                                                      name, suffix + 1,
                                                      operation ? " " : "", strempty(operation));

                        *ret = TAKE_PTR(u);
                        परत 1;
                }
                जर (r != -EINVAL)
                        परत r;
        }

        s = new(अक्षर, म_माप(name) * 4 + म_माप(suffix) + 1);
        जर (!s)
                परत -ENOMEM;

        mangled = do_escape_mangle(name, FLAGS_SET(flags, UNIT_NAME_MANGLE_GLOB), s);
        जर (mangled)
                log_full(warn ? LOG_NOTICE : LOG_DEBUG,
                         "Invalid unit name \"%s\" escaped as \"%s\"%s.",
                         name, s,
                         suggest_escape ? " (maybe you should use systemd-escape?)" : "");

        /* Append a suffix if it doesn't have any, but only if this is not a glob, so that we can allow
         * "foo.*" as a valid glob. */
        जर ((!FLAGS_SET(flags, UNIT_NAME_MANGLE_GLOB) || !string_is_glob(s)) && unit_name_to_type(s) < 0)
                म_जोड़ो(s, suffix);

        /* Make sure mangling didn't grow this too large (but don't do this check if globbing is allowed,
         * since globs generally do not qualify as valid unit names) */
        जर (!FLAGS_SET(flags, UNIT_NAME_MANGLE_GLOB) && !unit_name_is_valid(s, UNIT_NAME_ANY))
                परत -EINVAL;

        *ret = TAKE_PTR(s);
        परत 1;

good:
        परत strdup_to(ret, name);
}

पूर्णांक slice_build_parent_slice(स्थिरांक अक्षर *slice, अक्षर **ret) {
        प्रतिज्ञा(slice);
        प्रतिज्ञा(ret);

        जर (!slice_name_is_valid(slice))
                परत -EINVAL;

        जर (streq(slice, SPECIAL_मूळ_SLICE)) {
                *ret = रिक्त;
                परत 0;
        }

        _cleanup_free_ अक्षर *s = strdup(slice);
        जर (!s)
                परत -ENOMEM;

        अक्षर *dash = म_खोजप(s, '-');
        जर (!dash)
                परत strdup_to_full(ret, SPECIAL_मूळ_SLICE);

        /* We know that s ended with .slice before truncation, so we have enough space. */
        म_नकल(dash, ".slice");

        *ret = TAKE_PTR(s);
        परत 1;
}

पूर्णांक slice_build_subslice(स्थिरांक अक्षर *slice, स्थिरांक अक्षर *name, अक्षर **ret) {
        अक्षर *subslice;

        प्रतिज्ञा(slice);
        प्रतिज्ञा(name);
        प्रतिज्ञा(ret);

        जर (!slice_name_is_valid(slice))
                परत -EINVAL;

        जर (!unit_prefix_is_valid(name))
                परत -EINVAL;

        जर (streq(slice, SPECIAL_मूळ_SLICE))
                subslice = strjoin(name, ".slice");
        अन्यथा {
                स्थिरांक अक्षर *e;

                assert_se(e = endswith(slice, ".slice"));

                subslice = new(अक्षर, (e - slice) + 1 + म_माप(name) + 6 + 1);
                जर (!subslice)
                        परत -ENOMEM;

                stpcpy(stpcpy(stpcpy(mempcpy(subslice, slice, e - slice), "-"), name), ".slice");
        }

        *ret = subslice;
        परत 0;
}

bool slice_name_is_valid(स्थिरांक अक्षर *name) {
        स्थिरांक अक्षर *p, *e;
        bool dash = false;

        जर (!unit_name_is_valid(name, UNIT_NAME_PLAIN))
                परत false;

        जर (streq(name, SPECIAL_मूळ_SLICE))
                परत true;

        e = endswith(name, ".slice");
        जर (!e)
                परत false;

        साठी (p = name; p < e; p++) {

                जर (*p == '-') {

                        /* Don't allow initial dash */
                        जर (p == name)
                                परत false;

                        /* Don't allow multiple dashes */
                        जर (dash)
                                परत false;

                        dash = true;
                } अन्यथा
                        dash = false;
        }

        /* Don't allow trailing hash */
        जर (dash)
                परत false;

        परत true;
}

bool unit_name_prefix_equal(स्थिरांक अक्षर *a, स्थिरांक अक्षर *b) {
        स्थिरांक अक्षर *p, *q;

        प्रतिज्ञा(a);
        प्रतिज्ञा(b);

        जर (!unit_name_is_valid(a, UNIT_NAME_ANY) || !unit_name_is_valid(b, UNIT_NAME_ANY))
                परत false;

        p = म_अक्षर(a, '@');
        जर (!p)
                p = म_खोजप(a, '.');

        q = म_अक्षर(b, '@');
        जर (!q)
                q = म_खोजप(b, '.');

        प्रतिज्ञा(p);
        प्रतिज्ञा(q);

        परत memcmp_nn(a, p - a, b, q - b) == 0;
}
