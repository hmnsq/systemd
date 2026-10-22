/* SPDX-License-Identifier: LGPL-2.1-or-later */

#समावेश "sd-id128.h"

#समावेश "alloc-util.h"
#समावेश "glob-util.h"
#समावेश "hexdecoct.h"
#समावेश "log.h"
#समावेश "memory-util.h"
#समावेश "path-util.h"
#समावेश "sparse-endian.h"
#समावेश "special.h"
#समावेश "siphash24.h"
#समावेश "string-util.h"
#समावेश "unit-def.h"
#समावेश "unit-name.h"

/* Characters valid in a unit name. */
#घोषणा VALID_CHARS                             \
        ALPHANUMERICAL                          \
        ":-_.\\"

/* The same, but also permits the single @ character that may appear */
#घोषणा VALID_CHARS_WITH_AT                     \
        "@"                                     \
        VALID_CHARS

/* All chars valid in a unit name glob */
#घोषणा VALID_CHARS_GLOB                        \
        VALID_CHARS_WITH_AT                     \
        "[]!-*?"

#घोषणा LONG_UNIT_NAME_HASH_कुंजी SD_ID128_MAKE(ec,f2,37,fb,58,32,4a,32,84,9f,06,9b,0d,21,eb,9a)
#घोषणा UNIT_NAME_HASH_लंबाई_CHARS 16

bool unit_name_is_valid(स्थिर अक्षर *n, UnitNameFlags flags) {
        स्थिर अक्षर *e, *i, *at;

        निश्चित((flags & ~(UNIT_NAME_PLAIN|UNIT_NAME_INSTANCE|UNIT_NAME_TEMPLATE)) == 0);

        अगर (_unlikely_(flags == 0))
                वापस false;

        अगर (isempty(n))
                वापस false;

        अगर (म_माप(n) >= UNIT_NAME_MAX)
                वापस false;

        e = म_खोजप(n, '.');
        अगर (!e || e == n)
                वापस false;

        अगर (unit_type_from_string(e + 1) < 0)
                वापस false;

        क्रम (i = n, at = शून्य; i < e; i++) {

                अगर (*i == '@' && !at)
                        at = i;

                अगर (!म_अक्षर(VALID_CHARS_WITH_AT, *i))
                        वापस false;
        }

        अगर (at == n)
                वापस false;

        अगर (flags & UNIT_NAME_PLAIN)
                अगर (!at)
                        वापस true;

        अगर (flags & UNIT_NAME_INSTANCE)
                अगर (at && e > at + 1)
                        वापस true;

        अगर (flags & UNIT_NAME_TEMPLATE)
                अगर (at && e == at + 1)
                        वापस true;

        वापस false;
}

bool unit_prefix_is_valid(स्थिर अक्षर *p) {

        /* We don't allow additional @ in the prefix string */

        अगर (isempty(p))
                वापस false;

        वापस in_charset(p, VALID_CHARS);
}

bool unit_instance_is_valid(स्थिर अक्षर *i) {

        /* The max length depends on the length of the string, so we
         * don't really check this here. */

        अगर (isempty(i))
                वापस false;

        /* We allow additional @ in the instance string, we do not
         * allow them in the prefix! */

        वापस in_charset(i, "@" VALID_CHARS);
}

bool unit_suffix_is_valid(स्थिर अक्षर *s) {
        अगर (isempty(s))
                वापस false;

        अगर (s[0] != '.')
                वापस false;

        अगर (unit_type_from_string(s + 1) < 0)
                वापस false;

        वापस true;
}

पूर्णांक unit_name_to_prefix(स्थिर अक्षर *n, अक्षर **ret) {
        स्थिर अक्षर *p;
        अक्षर *s;

        निश्चित(n);
        निश्चित(ret);

        अगर (!unit_name_is_valid(n, UNIT_NAME_ANY))
                वापस -EINVAL;

        p = म_अक्षर(n, '@');
        अगर (!p)
                p = म_खोजप(n, '.');

        assert_se(p);

        s = strndup(n, p - n);
        अगर (!s)
                वापस -ENOMEM;

        *ret = s;
        वापस 0;
}

UnitNameFlags unit_name_to_instance(स्थिर अक्षर *n, अक्षर **ret) {
        स्थिर अक्षर *p, *d;

        निश्चित(n);

        अगर (!unit_name_is_valid(n, UNIT_NAME_ANY))
                वापस -EINVAL;

        /* Everything past the first @ and before the last . is the instance */
        p = म_अक्षर(n, '@');
        अगर (!p) {
                अगर (ret)
                        *ret = शून्य;
                वापस UNIT_NAME_PLAIN;
        }

        p++;

        d = म_खोजप(p, '.');
        अगर (!d)
                वापस -EINVAL;

        अगर (ret) {
                अक्षर *i = strndup(p, d-p);
                अगर (!i)
                        वापस -ENOMEM;

                *ret = i;
        }
        वापस d > p ? UNIT_NAME_INSTANCE : UNIT_NAME_TEMPLATE;
}

पूर्णांक unit_name_to_prefix_and_instance(स्थिर अक्षर *n, अक्षर **ret) {
        स्थिर अक्षर *d;
        अक्षर *s;

        निश्चित(n);
        निश्चित(ret);

        अगर (!unit_name_is_valid(n, UNIT_NAME_ANY))
                वापस -EINVAL;

        d = म_खोजप(n, '.');
        अगर (!d)
                वापस -EINVAL;

        s = strndup(n, d - n);
        अगर (!s)
                वापस -ENOMEM;

        *ret = s;
        वापस 0;
}

UnitType unit_name_to_type(स्थिर अक्षर *n) {
        स्थिर अक्षर *e;

        निश्चित(n);

        अगर (!unit_name_is_valid(n, UNIT_NAME_ANY))
                वापस _UNIT_TYPE_INVALID;

        assert_se(e = म_खोजप(n, '.'));

        वापस unit_type_from_string(e + 1);
}

पूर्णांक unit_name_change_suffix(स्थिर अक्षर *n, स्थिर अक्षर *suffix, अक्षर **ret) {
        _cleanup_free_ अक्षर *s = शून्य;
        माप_प्रकार a, b;
        स्थिर अक्षर *e;

        निश्चित(n);
        निश्चित(suffix);
        निश्चित(ret);

        अगर (!unit_name_is_valid(n, UNIT_NAME_ANY))
                वापस -EINVAL;

        अगर (!unit_suffix_is_valid(suffix))
                वापस -EINVAL;

        assert_se(e = म_खोजप(n, '.'));

        a = e - n;
        b = म_माप(suffix);

        s = new(अक्षर, a + b + 1);
        अगर (!s)
                वापस -ENOMEM;

        म_नकल(mempcpy(s, n, a), suffix);

        /* Make sure the name is still valid (i.e. didn't grow too large due to longer suffix) */
        अगर (!unit_name_is_valid(s, UNIT_NAME_ANY))
                वापस -EINVAL;

        *ret = TAKE_PTR(s);
        वापस 0;
}

पूर्णांक unit_name_build(स्थिर अक्षर *prefix, स्थिर अक्षर *instance, स्थिर अक्षर *suffix, अक्षर **ret) {
        UnitType type;

        निश्चित(prefix);
        निश्चित(suffix);
        निश्चित(ret);

        अगर (suffix[0] != '.')
                वापस -EINVAL;

        type = unit_type_from_string(suffix + 1);
        अगर (type < 0)
                वापस type;

        वापस unit_name_build_from_type(prefix, instance, type, ret);
}

पूर्णांक unit_name_build_from_type(स्थिर अक्षर *prefix, स्थिर अक्षर *instance, UnitType type, अक्षर **ret) {
        _cleanup_free_ अक्षर *s = शून्य;
        स्थिर अक्षर *ut;

        निश्चित(prefix);
        निश्चित(type >= 0);
        निश्चित(type < _UNIT_TYPE_MAX);
        निश्चित(ret);

        अगर (!unit_prefix_is_valid(prefix))
                वापस -EINVAL;

        ut = unit_type_to_string(type);

        अगर (instance) {
                अगर (!unit_instance_is_valid(instance))
                        वापस -EINVAL;

                s = strjoin(prefix, "@", instance, ".", ut);
        } अन्यथा
                s = strjoin(prefix, ".", ut);
        अगर (!s)
                वापस -ENOMEM;

        /* Verify that this didn't grow too large (or otherwise is invalid) */
        अगर (!unit_name_is_valid(s, instance ? UNIT_NAME_INSTANCE : UNIT_NAME_PLAIN))
                वापस -EINVAL;

        *ret = TAKE_PTR(s);
        वापस 0;
}

अटल अक्षर *do_escape_char(अक्षर c, अक्षर *t) {
        निश्चित(t);

        *(t++) = '\\';
        *(t++) = 'x';
        *(t++) = hexchar(c >> 4);
        *(t++) = hexchar(c);

        वापस t;
}

अटल अक्षर *do_escape(स्थिर अक्षर *f, अक्षर *t) {
        निश्चित(f);
        निश्चित(t);

        /* do not create units with a leading '.', like for "/.dotdir" mount points */
        अगर (*f == '.') {
                t = do_escape_char(*f, t);
                f++;
        }

        क्रम (; *f; f++) {
                अगर (*f == '/')
                        *(t++) = '-';
                अन्यथा अगर (IN_SET(*f, '-', '\\') || !म_अक्षर(VALID_CHARS, *f))
                        t = do_escape_char(*f, t);
                अन्यथा
                        *(t++) = *f;
        }

        वापस t;
}

अक्षर* unit_name_escape(स्थिर अक्षर *f) {
        अक्षर *r, *t;

        निश्चित(f);

        r = new(अक्षर, म_माप(f)*4+1);
        अगर (!r)
                वापस शून्य;

        t = do_escape(f, r);
        *t = 0;

        वापस r;
}

पूर्णांक unit_name_unescape(स्थिर अक्षर *f, अक्षर **ret) {
        _cleanup_free_ अक्षर *r = शून्य;
        अक्षर *t;

        निश्चित(f);
        निश्चित(ret);

        r = strdup(f);
        अगर (!r)
                वापस -ENOMEM;

        क्रम (t = r; *f; f++) {
                अगर (*f == '-')
                        *(t++) = '/';
                अन्यथा अगर (*f == '\\') {
                        पूर्णांक a, b;

                        अगर (f[1] != 'x')
                                वापस -EINVAL;

                        a = unhexchar(f[2]);
                        अगर (a < 0)
                                वापस -EINVAL;

                        b = unhexchar(f[3]);
                        अगर (b < 0)
                                वापस -EINVAL;

                        *(t++) = (अक्षर) (((uint8_t) a << 4U) | (uint8_t) b);
                        f += 3;
                } अन्यथा
                        *(t++) = *f;
        }

        *t = 0;

        *ret = TAKE_PTR(r);

        वापस 0;
}

पूर्णांक unit_name_path_escape(स्थिर अक्षर *f, अक्षर **ret) {
        _cleanup_free_ अक्षर *p = शून्य;
        अक्षर *s;
        पूर्णांक r;

        निश्चित(f);
        निश्चित(ret);

        r = path_simplify_alloc(f, &p);
        अगर (r < 0)
                वापस r;

        अगर (empty_or_मूल(p))
                s = strdup("-");
        अन्यथा {
                अगर (!path_is_normalized(p))
                        वापस -EINVAL;

                /* Truncate trailing slashes and skip leading slashes */
                delete_trailing_chars(p, "/");
                s = unit_name_escape(skip_leading_chars(p, "/"));
        }
        अगर (!s)
                वापस -ENOMEM;

        *ret = s;
        वापस 0;
}

पूर्णांक unit_name_path_unescape(स्थिर अक्षर *f, अक्षर **ret) {
        _cleanup_free_ अक्षर *s = शून्य;
        पूर्णांक r;

        निश्चित(f);

        अगर (isempty(f))
                वापस -EINVAL;

        अगर (streq(f, "-")) {
                s = strdup("/");
                अगर (!s)
                        वापस -ENOMEM;
        } अन्यथा {
                _cleanup_free_ अक्षर *w = शून्य;

                r = unit_name_unescape(f, &w);
                अगर (r < 0)
                        वापस r;

                /* Don't accept trailing or leading slashes */
                अगर (startswith(w, "/") || endswith(w, "/"))
                        वापस -EINVAL;

                /* Prefix a slash again */
                s = strjoin("/", w);
                अगर (!s)
                        वापस -ENOMEM;

                अगर (!path_is_normalized(s))
                        वापस -EINVAL;
        }

        अगर (ret)
                *ret = TAKE_PTR(s);

        वापस 0;
}

पूर्णांक unit_name_replace_instance_full(
                स्थिर अक्षर *original,
                स्थिर अक्षर *instance,
                bool accept_glob,
                अक्षर **ret) {

        _cleanup_free_ अक्षर *s = शून्य;
        स्थिर अक्षर *prefix, *suffix;
        माप_प्रकार pl;

        निश्चित(original);
        निश्चित(instance);
        निश्चित(ret);

        अगर (!unit_name_is_valid(original, UNIT_NAME_INSTANCE|UNIT_NAME_TEMPLATE))
                वापस -EINVAL;
        अगर (!unit_instance_is_valid(instance) && !(accept_glob && in_charset(instance, VALID_CHARS_GLOB)))
                वापस -EINVAL;

        prefix = ASSERT_PTR(म_अक्षर(original, '@'));
        suffix = ASSERT_PTR(म_खोजप(original, '.'));
        निश्चित(prefix < suffix);

        pl = prefix - original + 1; /* include '@' */

        s = new(अक्षर, pl + म_माप(instance) + म_माप(suffix) + 1);
        अगर (!s)
                वापस -ENOMEM;

#अगर HAS_FEATURE_MEMORY_SANITIZER
        /* MSan doesn't like stpncpy... See also https://github.com/google/sanitizers/issues/926 */
        memzero(s, pl + म_माप(instance) + म_माप(suffix) + 1);
#पूर्ण_अगर

        म_नकल(stpcpy(stpncpy(s, original, pl), instance), suffix);

        /* Make sure the resulting name still is valid, i.e. didn't grow too large. Globs will be expanded
         * by clients when used, so the check is pointless. */
        अगर (!accept_glob && !unit_name_is_valid(s, UNIT_NAME_INSTANCE))
                वापस -EINVAL;

        *ret = TAKE_PTR(s);
        वापस 0;
}

पूर्णांक unit_name_template(स्थिर अक्षर *f, अक्षर **ret) {
        स्थिर अक्षर *p, *e;
        अक्षर *s;
        माप_प्रकार a;

        निश्चित(f);
        निश्चित(ret);

        अगर (!unit_name_is_valid(f, UNIT_NAME_INSTANCE|UNIT_NAME_TEMPLATE))
                वापस -EINVAL;

        assert_se(p = म_अक्षर(f, '@'));
        assert_se(e = म_खोजप(f, '.'));

        a = p - f;

        s = new(अक्षर, a + 1 + म_माप(e) + 1);
        अगर (!s)
                वापस -ENOMEM;

        म_नकल(mempcpy(s, f, a + 1), e);

        *ret = s;
        वापस 0;
}

bool unit_name_is_hashed(स्थिर अक्षर *name) {
        स्थिर अक्षर *s;

        अगर (!unit_name_is_valid(name, UNIT_NAME_PLAIN))
                वापस false;

        assert_se(s = म_खोजप(name, '.'));

        अगर (s - name < UNIT_NAME_HASH_लंबाई_CHARS + 1)
                वापस false;

        s -= UNIT_NAME_HASH_लंबाई_CHARS;
        अगर (s[-1] != '_')
                वापस false;

        क्रम (माप_प्रकार i = 0; i < UNIT_NAME_HASH_लंबाई_CHARS; i++)
                अगर (!म_अक्षर(LOWERCASE_HEXDIGITS, s[i]))
                        वापस false;

        वापस true;
}

पूर्णांक unit_name_hash_long(स्थिर अक्षर *name, अक्षर **ret) {
        _cleanup_free_ अक्षर *n = शून्य, *hash = शून्य;
        स्थिर अक्षर *suffix;
        le64_t h;
        माप_प्रकार len;

        निश्चित(ret);

        अगर (म_माप(name) < UNIT_NAME_MAX)
                वापस -EMSGSIZE;

        suffix = म_खोजप(name, '.');
        अगर (!suffix)
                वापस -EINVAL;

        अगर (unit_type_from_string(suffix+1) < 0)
                वापस -EINVAL;

        h = htole64(siphash24_string(name, LONG_UNIT_NAME_HASH_कुंजी.bytes));

        hash = hexmem(&h, माप(h));
        अगर (!hash)
                वापस -ENOMEM;

        assert_se(म_माप(hash) == UNIT_NAME_HASH_लंबाई_CHARS);

        len = UNIT_NAME_MAX - 1 - म_माप(suffix+1) - UNIT_NAME_HASH_लंबाई_CHARS - 2;
        निश्चित(len > 0 && len < UNIT_NAME_MAX);

        n = strndup(name, len);
        अगर (!n)
                वापस -ENOMEM;

        अगर (!strextend(&n, "_", hash, suffix))
                वापस -ENOMEM;
        assert_se(unit_name_is_valid(n, UNIT_NAME_PLAIN));

        *ret = TAKE_PTR(n);

        वापस 0;
}

पूर्णांक unit_name_from_path(स्थिर अक्षर *path, स्थिर अक्षर *suffix, अक्षर **ret) {
        _cleanup_free_ अक्षर *p = शून्य, *s = शून्य;
        पूर्णांक r;

        निश्चित(path);
        निश्चित(suffix);
        निश्चित(ret);

        अगर (!unit_suffix_is_valid(suffix))
                वापस -EINVAL;

        r = unit_name_path_escape(path, &p);
        अगर (r < 0)
                वापस r;

        s = strjoin(p, suffix);
        अगर (!s)
                वापस -ENOMEM;

        अगर (म_माप(s) >= UNIT_NAME_MAX) {
                _cleanup_free_ अक्षर *n = शून्य;

                log_debug("Unit name \"%s\" too long, falling back to hashed unit name.", s);

                r = unit_name_hash_long(s, &n);
                अगर (r < 0)
                        वापस r;

                free_and_replace(s, n);
        }

        /* Refuse if this for some other reason didn't result in a valid name */
        अगर (!unit_name_is_valid(s, UNIT_NAME_PLAIN))
                वापस -EINVAL;

        *ret = TAKE_PTR(s);
        वापस 0;
}

पूर्णांक unit_name_from_path_instance(स्थिर अक्षर *prefix, स्थिर अक्षर *path, स्थिर अक्षर *suffix, अक्षर **ret) {
        _cleanup_free_ अक्षर *p = शून्य, *s = शून्य;
        पूर्णांक r;

        निश्चित(prefix);
        निश्चित(path);
        निश्चित(suffix);
        निश्चित(ret);

        अगर (!unit_prefix_is_valid(prefix))
                वापस -EINVAL;

        अगर (!unit_suffix_is_valid(suffix))
                वापस -EINVAL;

        r = unit_name_path_escape(path, &p);
        अगर (r < 0)
                वापस r;

        s = strjoin(prefix, "@", p, suffix);
        अगर (!s)
                वापस -ENOMEM;

        अगर (म_माप(s) >= UNIT_NAME_MAX) /* Return a slightly more descriptive error for this specific condition */
                वापस -ENAMETOOLONG;

        /* Refuse if this for some other reason didn't result in a valid name */
        अगर (!unit_name_is_valid(s, UNIT_NAME_INSTANCE))
                वापस -EINVAL;

        *ret = TAKE_PTR(s);
        वापस 0;
}

पूर्णांक unit_name_to_path(स्थिर अक्षर *name, अक्षर **ret) {
        _cleanup_free_ अक्षर *prefix = शून्य;
        पूर्णांक r;

        निश्चित(name);

        r = unit_name_to_prefix(name, &prefix);
        अगर (r < 0)
                वापस r;

        अगर (unit_name_is_hashed(name))
                वापस -ENAMETOOLONG;

        वापस unit_name_path_unescape(prefix, ret);
}

अटल bool do_escape_mangle(स्थिर अक्षर *f, bool allow_globs, अक्षर *t) {
        स्थिर अक्षर *valid_chars;
        bool mangled = false;

        निश्चित(f);
        निश्चित(t);

        /* We'll only escape the obvious characters here, to play safe.
         *
         * Returns true if any characters were mangled, false otherwise.
         */

        valid_chars = allow_globs ? VALID_CHARS_GLOB : VALID_CHARS_WITH_AT;

        क्रम (; *f; f++)
                अगर (*f == '/') {
                        *(t++) = '-';
                        mangled = true;
                } अन्यथा अगर (!म_अक्षर(valid_chars, *f)) {
                        t = do_escape_char(*f, t);
                        mangled = true;
                } अन्यथा
                        *(t++) = *f;
        *t = 0;

        वापस mangled;
}

/**
 *  Convert a string to a unit name. /dev/blah is converted to dev-blah.device,
 *  /blah/blah is converted to blah-blah.mount, anything else is left alone,
 *  except that @suffix is appended if a valid unit suffix is not present.
 *
 *  If @allow_globs, globs characters are preserved. Otherwise, they are escaped.
 */
पूर्णांक unit_name_mangle_with_suffix(
                स्थिर अक्षर *name,
                स्थिर अक्षर *operation,
                UnitNameMangle flags,
                स्थिर अक्षर *suffix,
                अक्षर **ret) {

        _cleanup_free_ अक्षर *s = शून्य;
        bool mangled, suggest_escape = true, warn = FLAGS_SET(flags, UNIT_NAME_MANGLE_WARN);
        पूर्णांक r;

        निश्चित(name);
        निश्चित(suffix);
        निश्चित(ret);

        अगर (isempty(name)) /* We cannot mangle empty unit names to become valid, sorry. */
                वापस -EINVAL;

        अगर (!unit_suffix_is_valid(suffix))
                वापस -EINVAL;

        /* Already a fully valid unit name? If so, no mangling is necessary... */
        अगर (unit_name_is_valid(name, UNIT_NAME_ANY)) {
                अगर (FLAGS_SET(flags, UNIT_NAME_MANGLE_STRICT) && !endswith(name, suffix)) {
                        स्थिर अक्षर *e = ASSERT_PTR(म_खोजप(name, '.'));

                        वापस log_full_errno(warn ? LOG_NOTICE : LOG_DEBUG,
                                              SYNTHETIC_ERRNO(EINVAL),
                                              "Unit name \"%s\" has unit type \"%s\", but \"%s\" is expected%s%s.",
                                              name, e + 1, suffix + 1,
                                              operation ? " " : "", strempty(operation));
                }

                जाओ good;
        }

        /* Already a fully valid globbing expression? If so, no mangling is necessary either... */
        अगर (string_is_glob(name) && in_charset(name, VALID_CHARS_GLOB)) {
                अगर (FLAGS_SET(flags, UNIT_NAME_MANGLE_GLOB))
                        जाओ good;
                log_full(warn ? LOG_NOTICE : LOG_DEBUG,
                         "Glob pattern passed%s%s, but globs are not supported for this.",
                         operation ? " " : "", strempty(operation));
                suggest_escape = false;
        }

        अगर (path_is_absolute(name)) {
                _cleanup_free_ अक्षर *n = शून्य, *u = शून्य;

                r = path_simplify_alloc(name, &n);
                अगर (r < 0)
                        वापस r;

                अगर (is_device_path(n)) {
                        r = unit_name_from_path(n, ".device", &u);
                        अगर (r >= 0) {
                                अगर (FLAGS_SET(flags, UNIT_NAME_MANGLE_STRICT) && !streq(suffix, ".device"))
                                        वापस log_full_errno(warn ? LOG_NOTICE : LOG_DEBUG,
                                                              SYNTHETIC_ERRNO(EINVAL),
                                                              "Path \"%s\" resolves to unit type \"device\", but \"%s\" is expected%s%s.",
                                                              name, suffix + 1,
                                                              operation ? " " : "", strempty(operation));

                                *ret = TAKE_PTR(u);
                                वापस 1;
                        }
                        अगर (r != -EINVAL)
                                वापस r;
                }

                r = unit_name_from_path(n, ".mount", &u);
                अगर (r >= 0) {
                        अगर (FLAGS_SET(flags, UNIT_NAME_MANGLE_STRICT) && !streq(suffix, ".mount"))
                                वापस log_full_errno(warn ? LOG_NOTICE : LOG_DEBUG,
                                                      SYNTHETIC_ERRNO(EINVAL),
                                                      "Path \"%s\" resolves to unit type \"mount\", but \"%s\" is expected%s%s.",
                                                      name, suffix + 1,
                                                      operation ? " " : "", strempty(operation));

                        *ret = TAKE_PTR(u);
                        वापस 1;
                }
                अगर (r != -EINVAL)
                        वापस r;
        }

        s = new(अक्षर, म_माप(name) * 4 + म_माप(suffix) + 1);
        अगर (!s)
                वापस -ENOMEM;

        mangled = do_escape_mangle(name, FLAGS_SET(flags, UNIT_NAME_MANGLE_GLOB), s);
        अगर (mangled)
                log_full(warn ? LOG_NOTICE : LOG_DEBUG,
                         "Invalid unit name \"%s\" escaped as \"%s\"%s.",
                         name, s,
                         suggest_escape ? " (maybe you should use systemd-escape?)" : "");

        /* Append a suffix if it doesn't have any, but only if this is not a glob, so that we can allow
         * "foo.*" as a valid glob. */
        अगर ((!FLAGS_SET(flags, UNIT_NAME_MANGLE_GLOB) || !string_is_glob(s)) && unit_name_to_type(s) < 0)
                म_जोड़ो(s, suffix);

        /* Make sure mangling didn't grow this too large (but don't do this check if globbing is allowed,
         * since globs generally do not qualify as valid unit names) */
        अगर (!FLAGS_SET(flags, UNIT_NAME_MANGLE_GLOB) && !unit_name_is_valid(s, UNIT_NAME_ANY))
                वापस -EINVAL;

        *ret = TAKE_PTR(s);
        वापस 1;

good:
        वापस strdup_to(ret, name);
}

पूर्णांक slice_build_parent_slice(स्थिर अक्षर *slice, अक्षर **ret) {
        निश्चित(slice);
        निश्चित(ret);

        अगर (!slice_name_is_valid(slice))
                वापस -EINVAL;

        अगर (streq(slice, SPECIAL_मूल_SLICE)) {
                *ret = शून्य;
                वापस 0;
        }

        _cleanup_free_ अक्षर *s = strdup(slice);
        अगर (!s)
                वापस -ENOMEM;

        अक्षर *dash = म_खोजप(s, '-');
        अगर (!dash)
                वापस strdup_to_full(ret, SPECIAL_मूल_SLICE);

        /* We know that s ended with .slice before truncation, so we have enough space. */
        म_नकल(dash, ".slice");

        *ret = TAKE_PTR(s);
        वापस 1;
}

पूर्णांक slice_build_subslice(स्थिर अक्षर *slice, स्थिर अक्षर *name, अक्षर **ret) {
        अक्षर *subslice;

        निश्चित(slice);
        निश्चित(name);
        निश्चित(ret);

        अगर (!slice_name_is_valid(slice))
                वापस -EINVAL;

        अगर (!unit_prefix_is_valid(name))
                वापस -EINVAL;

        अगर (streq(slice, SPECIAL_मूल_SLICE))
                subslice = strjoin(name, ".slice");
        अन्यथा {
                स्थिर अक्षर *e;

                assert_se(e = endswith(slice, ".slice"));

                subslice = new(अक्षर, (e - slice) + 1 + म_माप(name) + 6 + 1);
                अगर (!subslice)
                        वापस -ENOMEM;

                stpcpy(stpcpy(stpcpy(mempcpy(subslice, slice, e - slice), "-"), name), ".slice");
        }

        *ret = subslice;
        वापस 0;
}

bool slice_name_is_valid(स्थिर अक्षर *name) {
        स्थिर अक्षर *p, *e;
        bool dash = false;

        अगर (!unit_name_is_valid(name, UNIT_NAME_PLAIN))
                वापस false;

        अगर (streq(name, SPECIAL_मूल_SLICE))
                वापस true;

        e = endswith(name, ".slice");
        अगर (!e)
                वापस false;

        क्रम (p = name; p < e; p++) {

                अगर (*p == '-') {

                        /* Don't allow initial dash */
                        अगर (p == name)
                                वापस false;

                        /* Don't allow multiple dashes */
                        अगर (dash)
                                वापस false;

                        dash = true;
                } अन्यथा
                        dash = false;
        }

        /* Don't allow trailing hash */
        अगर (dash)
                वापस false;

        वापस true;
}

bool unit_name_prefix_equal(स्थिर अक्षर *a, स्थिर अक्षर *b) {
        स्थिर अक्षर *p, *q;

        निश्चित(a);
        निश्चित(b);

        अगर (!unit_name_is_valid(a, UNIT_NAME_ANY) || !unit_name_is_valid(b, UNIT_NAME_ANY))
                वापस false;

        p = म_अक्षर(a, '@');
        अगर (!p)
                p = म_खोजप(a, '.');

        q = म_अक्षर(b, '@');
        अगर (!q)
                q = म_खोजप(b, '.');

        निश्चित(p);
        निश्चित(q);

        वापस memcmp_nn(a, p - a, b, q - b) == 0;
}
