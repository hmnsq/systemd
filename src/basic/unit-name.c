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

#define LONG_UNIT_NAME_HASH_साँचो SD_ID128_MAKE(ec,f2,37,fb,58,32,4a,32,84,9f,06,9b,0d,21,eb,9a)
#define UNIT_NAME_HASH_LENGTH_CHARS 16

bool unit_name_is_valid(अचर वर्ण *n, UnitNameFlags flags) {
        अचर वर्ण *e, *i, *at;

        प्रतिज्ञा((flags & ~(UNIT_NAME_PLAIN|UNIT_NAME_INSTANCE|UNIT_NAME_TEMPLATE)) == 0);

        यदि (_unlikely_(flags == 0))
                फर्काऊ false;

        यदि (isempty(n))
                फर्काऊ false;

        यदि (म_माप(n) >= UNIT_NAME_MAX)
                फर्काऊ false;

        e = म_खोजप(n, '.');
        यदि (!e || e == n)
                फर्काऊ false;

        यदि (unit_type_from_string(e + 1) < 0)
                फर्काऊ false;

        लागि (i = n, at = रिक्त; i < e; i++) {

                यदि (*i == '@' && !at)
                        at = i;

                यदि (!म_अक्षर(VALID_CHARS_WITH_AT, *i))
                        फर्काऊ false;
        }

        यदि (at == n)
                फर्काऊ false;

        यदि (flags & UNIT_NAME_PLAIN)
                यदि (!at)
                        फर्काऊ true;

        यदि (flags & UNIT_NAME_INSTANCE)
                यदि (at && e > at + 1)
                        फर्काऊ true;

        यदि (flags & UNIT_NAME_TEMPLATE)
                यदि (at && e == at + 1)
                        फर्काऊ true;

        फर्काऊ false;
}

bool unit_prefix_is_valid(अचर वर्ण *p) {

        /* We don't allow additional @ in the prefix string */

        यदि (isempty(p))
                फर्काऊ false;

        फर्काऊ in_charset(p, VALID_CHARS);
}

bool unit_instance_is_valid(अचर वर्ण *i) {

        /* The max length depends on the length of the string, so we
         * don't really check this here. */

        यदि (isempty(i))
                फर्काऊ false;

        /* We allow additional @ in the instance string, we do not
         * allow them in the prefix! */

        फर्काऊ in_charset(i, "@" VALID_CHARS);
}

bool unit_suffix_is_valid(अचर वर्ण *s) {
        यदि (isempty(s))
                फर्काऊ false;

        यदि (s[0] != '.')
                फर्काऊ false;

        यदि (unit_type_from_string(s + 1) < 0)
                फर्काऊ false;

        फर्काऊ true;
}

पूर्णाङ्क unit_name_to_prefix(अचर वर्ण *n, वर्ण **ret) {
        अचर वर्ण *p;
        वर्ण *s;

        प्रतिज्ञा(n);
        प्रतिज्ञा(ret);

        यदि (!unit_name_is_valid(n, UNIT_NAME_ANY))
                फर्काऊ -EINVAL;

        p = म_अक्षर(n, '@');
        यदि (!p)
                p = म_खोजप(n, '.');

        assert_se(p);

        s = strndup(n, p - n);
        यदि (!s)
                फर्काऊ -ENOMEM;

        *ret = s;
        फर्काऊ 0;
}

UnitNameFlags unit_name_to_instance(अचर वर्ण *n, वर्ण **ret) {
        अचर वर्ण *p, *d;

        प्रतिज्ञा(n);

        यदि (!unit_name_is_valid(n, UNIT_NAME_ANY))
                फर्काऊ -EINVAL;

        /* Everything past the first @ and before the last . is the instance */
        p = म_अक्षर(n, '@');
        यदि (!p) {
                यदि (ret)
                        *ret = रिक्त;
                फर्काऊ UNIT_NAME_PLAIN;
        }

        p++;

        d = म_खोजप(p, '.');
        यदि (!d)
                फर्काऊ -EINVAL;

        यदि (ret) {
                वर्ण *i = strndup(p, d-p);
                यदि (!i)
                        फर्काऊ -ENOMEM;

                *ret = i;
        }
        फर्काऊ d > p ? UNIT_NAME_INSTANCE : UNIT_NAME_TEMPLATE;
}

पूर्णाङ्क unit_name_to_prefix_and_instance(अचर वर्ण *n, वर्ण **ret) {
        अचर वर्ण *d;
        वर्ण *s;

        प्रतिज्ञा(n);
        प्रतिज्ञा(ret);

        यदि (!unit_name_is_valid(n, UNIT_NAME_ANY))
                फर्काऊ -EINVAL;

        d = म_खोजप(n, '.');
        यदि (!d)
                फर्काऊ -EINVAL;

        s = strndup(n, d - n);
        यदि (!s)
                फर्काऊ -ENOMEM;

        *ret = s;
        फर्काऊ 0;
}

UnitType unit_name_to_type(अचर वर्ण *n) {
        अचर वर्ण *e;

        प्रतिज्ञा(n);

        यदि (!unit_name_is_valid(n, UNIT_NAME_ANY))
                फर्काऊ _UNIT_TYPE_INVALID;

        assert_se(e = म_खोजप(n, '.'));

        फर्काऊ unit_type_from_string(e + 1);
}

पूर्णाङ्क unit_name_change_suffix(अचर वर्ण *n, अचर वर्ण *suffix, वर्ण **ret) {
        _cleanup_free_ वर्ण *s = रिक्त;
        माप_प्रकार a, b;
        अचर वर्ण *e;

        प्रतिज्ञा(n);
        प्रतिज्ञा(suffix);
        प्रतिज्ञा(ret);

        यदि (!unit_name_is_valid(n, UNIT_NAME_ANY))
                फर्काऊ -EINVAL;

        यदि (!unit_suffix_is_valid(suffix))
                फर्काऊ -EINVAL;

        assert_se(e = म_खोजप(n, '.'));

        a = e - n;
        b = म_माप(suffix);

        s = new(वर्ण, a + b + 1);
        यदि (!s)
                फर्काऊ -ENOMEM;

        म_नकल(mempcpy(s, n, a), suffix);

        /* Make sure the name is still valid (i.e. didn't grow too large due to longer suffix) */
        यदि (!unit_name_is_valid(s, UNIT_NAME_ANY))
                फर्काऊ -EINVAL;

        *ret = TAKE_PTR(s);
        फर्काऊ 0;
}

पूर्णाङ्क unit_name_build(अचर वर्ण *prefix, अचर वर्ण *instance, अचर वर्ण *suffix, वर्ण **ret) {
        UnitType type;

        प्रतिज्ञा(prefix);
        प्रतिज्ञा(suffix);
        प्रतिज्ञा(ret);

        यदि (suffix[0] != '.')
                फर्काऊ -EINVAL;

        type = unit_type_from_string(suffix + 1);
        यदि (type < 0)
                फर्काऊ type;

        फर्काऊ unit_name_build_from_type(prefix, instance, type, ret);
}

पूर्णाङ्क unit_name_build_from_type(अचर वर्ण *prefix, अचर वर्ण *instance, UnitType type, वर्ण **ret) {
        _cleanup_free_ वर्ण *s = रिक्त;
        अचर वर्ण *ut;

        प्रतिज्ञा(prefix);
        प्रतिज्ञा(type >= 0);
        प्रतिज्ञा(type < _UNIT_TYPE_MAX);
        प्रतिज्ञा(ret);

        यदि (!unit_prefix_is_valid(prefix))
                फर्काऊ -EINVAL;

        ut = unit_type_to_string(type);

        यदि (instance) {
                यदि (!unit_instance_is_valid(instance))
                        फर्काऊ -EINVAL;

                s = strjoin(prefix, "@", instance, ".", ut);
        } अन्यथा
                s = strjoin(prefix, ".", ut);
        यदि (!s)
                फर्काऊ -ENOMEM;

        /* Verify that this didn't grow too large (or otherwise is invalid) */
        यदि (!unit_name_is_valid(s, instance ? UNIT_NAME_INSTANCE : UNIT_NAME_PLAIN))
                फर्काऊ -EINVAL;

        *ret = TAKE_PTR(s);
        फर्काऊ 0;
}

स्थिर वर्ण *do_escape_char(वर्ण c, वर्ण *t) {
        प्रतिज्ञा(t);

        *(t++) = '\\';
        *(t++) = 'x';
        *(t++) = hexchar(c >> 4);
        *(t++) = hexchar(c);

        फर्काऊ t;
}

स्थिर वर्ण *do_escape(अचर वर्ण *f, वर्ण *t) {
        प्रतिज्ञा(f);
        प्रतिज्ञा(t);

        /* do not create units with a leading '.', like for "/.dotdir" mount points */
        यदि (*f == '.') {
                t = do_escape_char(*f, t);
                f++;
        }

        लागि (; *f; f++) {
                यदि (*f == '/')
                        *(t++) = '-';
                अन्यथा यदि (IN_SET(*f, '-', '\\') || !म_अक्षर(VALID_CHARS, *f))
                        t = do_escape_char(*f, t);
                अन्यथा
                        *(t++) = *f;
        }

        फर्काऊ t;
}

वर्ण* unit_name_escape(अचर वर्ण *f) {
        वर्ण *r, *t;

        प्रतिज्ञा(f);

        r = new(वर्ण, म_माप(f)*4+1);
        यदि (!r)
                फर्काऊ रिक्त;

        t = do_escape(f, r);
        *t = 0;

        फर्काऊ r;
}

पूर्णाङ्क unit_name_unescape(अचर वर्ण *f, वर्ण **ret) {
        _cleanup_free_ वर्ण *r = रिक्त;
        वर्ण *t;

        प्रतिज्ञा(f);
        प्रतिज्ञा(ret);

        r = strdup(f);
        यदि (!r)
                फर्काऊ -ENOMEM;

        लागि (t = r; *f; f++) {
                यदि (*f == '-')
                        *(t++) = '/';
                अन्यथा यदि (*f == '\\') {
                        पूर्णाङ्क a, b;

                        यदि (f[1] != 'x')
                                फर्काऊ -EINVAL;

                        a = unhexchar(f[2]);
                        यदि (a < 0)
                                फर्काऊ -EINVAL;

                        b = unhexchar(f[3]);
                        यदि (b < 0)
                                फर्काऊ -EINVAL;

                        *(t++) = (वर्ण) (((uint8_t) a << 4U) | (uint8_t) b);
                        f += 3;
                } अन्यथा
                        *(t++) = *f;
        }

        *t = 0;

        *ret = TAKE_PTR(r);

        फर्काऊ 0;
}

पूर्णाङ्क unit_name_path_escape(अचर वर्ण *f, वर्ण **ret) {
        _cleanup_free_ वर्ण *p = रिक्त;
        वर्ण *s;
        पूर्णाङ्क r;

        प्रतिज्ञा(f);
        प्रतिज्ञा(ret);

        r = path_simplify_alloc(f, &p);
        यदि (r < 0)
                फर्काऊ r;

        यदि (empty_or_जरा(p))
                s = strdup("-");
        अन्यथा {
                यदि (!path_is_normalized(p))
                        फर्काऊ -EINVAL;

                /* Truncate trailing slashes and skip leading slashes */
                delete_trailing_chars(p, "/");
                s = unit_name_escape(skip_leading_chars(p, "/"));
        }
        यदि (!s)
                फर्काऊ -ENOMEM;

        *ret = s;
        फर्काऊ 0;
}

पूर्णाङ्क unit_name_path_unescape(अचर वर्ण *f, वर्ण **ret) {
        _cleanup_free_ वर्ण *s = रिक्त;
        पूर्णाङ्क r;

        प्रतिज्ञा(f);

        यदि (isempty(f))
                फर्काऊ -EINVAL;

        यदि (streq(f, "-")) {
                s = strdup("/");
                यदि (!s)
                        फर्काऊ -ENOMEM;
        } अन्यथा {
                _cleanup_free_ वर्ण *w = रिक्त;

                r = unit_name_unescape(f, &w);
                यदि (r < 0)
                        फर्काऊ r;

                /* Don't accept trailing or leading slashes */
                यदि (startswith(w, "/") || endswith(w, "/"))
                        फर्काऊ -EINVAL;

                /* Prefix a slash again */
                s = strjoin("/", w);
                यदि (!s)
                        फर्काऊ -ENOMEM;

                यदि (!path_is_normalized(s))
                        फर्काऊ -EINVAL;
        }

        यदि (ret)
                *ret = TAKE_PTR(s);

        फर्काऊ 0;
}

पूर्णाङ्क unit_name_replace_instance_full(
                अचर वर्ण *original,
                अचर वर्ण *instance,
                bool accept_glob,
                वर्ण **ret) {

        _cleanup_free_ वर्ण *s = रिक्त;
        अचर वर्ण *prefix, *suffix;
        माप_प्रकार pl;

        प्रतिज्ञा(original);
        प्रतिज्ञा(instance);
        प्रतिज्ञा(ret);

        यदि (!unit_name_is_valid(original, UNIT_NAME_INSTANCE|UNIT_NAME_TEMPLATE))
                फर्काऊ -EINVAL;
        यदि (!unit_instance_is_valid(instance) && !(accept_glob && in_charset(instance, VALID_CHARS_GLOB)))
                फर्काऊ -EINVAL;

        prefix = ASSERT_PTR(म_अक्षर(original, '@'));
        suffix = ASSERT_PTR(म_खोजप(original, '.'));
        प्रतिज्ञा(prefix < suffix);

        pl = prefix - original + 1; /* include '@' */

        s = new(वर्ण, pl + म_माप(instance) + म_माप(suffix) + 1);
        यदि (!s)
                फर्काऊ -ENOMEM;

#यदि HAS_FEATURE_MEMORY_SANITIZER
        /* MSan doesn't like stpncpy... See also https://github.com/google/sanitizers/issues/926 */
        memzero(s, pl + म_माप(instance) + म_माप(suffix) + 1);
#endif

        म_नकल(stpcpy(stpncpy(s, original, pl), instance), suffix);

        /* Make sure the resulting name still is valid, i.e. didn't grow too large. Globs will be expanded
         * by clients when used, so the check is pointless. */
        यदि (!accept_glob && !unit_name_is_valid(s, UNIT_NAME_INSTANCE))
                फर्काऊ -EINVAL;

        *ret = TAKE_PTR(s);
        फर्काऊ 0;
}

पूर्णाङ्क unit_name_template(अचर वर्ण *f, वर्ण **ret) {
        अचर वर्ण *p, *e;
        वर्ण *s;
        माप_प्रकार a;

        प्रतिज्ञा(f);
        प्रतिज्ञा(ret);

        यदि (!unit_name_is_valid(f, UNIT_NAME_INSTANCE|UNIT_NAME_TEMPLATE))
                फर्काऊ -EINVAL;

        assert_se(p = म_अक्षर(f, '@'));
        assert_se(e = म_खोजप(f, '.'));

        a = p - f;

        s = new(वर्ण, a + 1 + म_माप(e) + 1);
        यदि (!s)
                फर्काऊ -ENOMEM;

        म_नकल(mempcpy(s, f, a + 1), e);

        *ret = s;
        फर्काऊ 0;
}

bool unit_name_is_hashed(अचर वर्ण *name) {
        अचर वर्ण *s;

        यदि (!unit_name_is_valid(name, UNIT_NAME_PLAIN))
                फर्काऊ false;

        assert_se(s = म_खोजप(name, '.'));

        यदि (s - name < UNIT_NAME_HASH_LENGTH_CHARS + 1)
                फर्काऊ false;

        s -= UNIT_NAME_HASH_LENGTH_CHARS;
        यदि (s[-1] != '_')
                फर्काऊ false;

        लागि (माप_प्रकार i = 0; i < UNIT_NAME_HASH_LENGTH_CHARS; i++)
                यदि (!म_अक्षर(LOWERCASE_HEXDIGITS, s[i]))
                        फर्काऊ false;

        फर्काऊ true;
}

पूर्णाङ्क unit_name_hash_long(अचर वर्ण *name, वर्ण **ret) {
        _cleanup_free_ वर्ण *n = रिक्त, *hash = रिक्त;
        अचर वर्ण *suffix;
        le64_t h;
        माप_प्रकार len;

        प्रतिज्ञा(ret);

        यदि (म_माप(name) < UNIT_NAME_MAX)
                फर्काऊ -EMSGSIZE;

        suffix = म_खोजप(name, '.');
        यदि (!suffix)
                फर्काऊ -EINVAL;

        यदि (unit_type_from_string(suffix+1) < 0)
                फर्काऊ -EINVAL;

        h = htole64(siphash24_string(name, LONG_UNIT_NAME_HASH_साँचो.bytes));

        hash = hexmem(&h, परिमाण(h));
        यदि (!hash)
                फर्काऊ -ENOMEM;

        assert_se(म_माप(hash) == UNIT_NAME_HASH_LENGTH_CHARS);

        len = UNIT_NAME_MAX - 1 - म_माप(suffix+1) - UNIT_NAME_HASH_LENGTH_CHARS - 2;
        प्रतिज्ञा(len > 0 && len < UNIT_NAME_MAX);

        n = strndup(name, len);
        यदि (!n)
                फर्काऊ -ENOMEM;

        यदि (!strextend(&n, "_", hash, suffix))
                फर्काऊ -ENOMEM;
        assert_se(unit_name_is_valid(n, UNIT_NAME_PLAIN));

        *ret = TAKE_PTR(n);

        फर्काऊ 0;
}

पूर्णाङ्क unit_name_from_path(अचर वर्ण *path, अचर वर्ण *suffix, वर्ण **ret) {
        _cleanup_free_ वर्ण *p = रिक्त, *s = रिक्त;
        पूर्णाङ्क r;

        प्रतिज्ञा(path);
        प्रतिज्ञा(suffix);
        प्रतिज्ञा(ret);

        यदि (!unit_suffix_is_valid(suffix))
                फर्काऊ -EINVAL;

        r = unit_name_path_escape(path, &p);
        यदि (r < 0)
                फर्काऊ r;

        s = strjoin(p, suffix);
        यदि (!s)
                फर्काऊ -ENOMEM;

        यदि (म_माप(s) >= UNIT_NAME_MAX) {
                _cleanup_free_ वर्ण *n = रिक्त;

                log_debug("Unit name \"%s\" too long, falling back to hashed unit name.", s);

                r = unit_name_hash_long(s, &n);
                यदि (r < 0)
                        फर्काऊ r;

                free_and_replace(s, n);
        }

        /* Refuse if this for some other reason didn't result in a valid name */
        यदि (!unit_name_is_valid(s, UNIT_NAME_PLAIN))
                फर्काऊ -EINVAL;

        *ret = TAKE_PTR(s);
        फर्काऊ 0;
}

पूर्णाङ्क unit_name_from_path_instance(अचर वर्ण *prefix, अचर वर्ण *path, अचर वर्ण *suffix, वर्ण **ret) {
        _cleanup_free_ वर्ण *p = रिक्त, *s = रिक्त;
        पूर्णाङ्क r;

        प्रतिज्ञा(prefix);
        प्रतिज्ञा(path);
        प्रतिज्ञा(suffix);
        प्रतिज्ञा(ret);

        यदि (!unit_prefix_is_valid(prefix))
                फर्काऊ -EINVAL;

        यदि (!unit_suffix_is_valid(suffix))
                फर्काऊ -EINVAL;

        r = unit_name_path_escape(path, &p);
        यदि (r < 0)
                फर्काऊ r;

        s = strjoin(prefix, "@", p, suffix);
        यदि (!s)
                फर्काऊ -ENOMEM;

        यदि (म_माप(s) >= UNIT_NAME_MAX) /* Return a slightly more descriptive error for this specific condition */
                फर्काऊ -ENAMETOOLONG;

        /* Refuse if this for some other reason didn't result in a valid name */
        यदि (!unit_name_is_valid(s, UNIT_NAME_INSTANCE))
                फर्काऊ -EINVAL;

        *ret = TAKE_PTR(s);
        फर्काऊ 0;
}

पूर्णाङ्क unit_name_to_path(अचर वर्ण *name, वर्ण **ret) {
        _cleanup_free_ वर्ण *prefix = रिक्त;
        पूर्णाङ्क r;

        प्रतिज्ञा(name);

        r = unit_name_to_prefix(name, &prefix);
        यदि (r < 0)
                फर्काऊ r;

        यदि (unit_name_is_hashed(name))
                फर्काऊ -ENAMETOOLONG;

        फर्काऊ unit_name_path_unescape(prefix, ret);
}

स्थिर bool do_escape_mangle(अचर वर्ण *f, bool allow_globs, वर्ण *t) {
        अचर वर्ण *valid_chars;
        bool mangled = false;

        प्रतिज्ञा(f);
        प्रतिज्ञा(t);

        /* We'll only escape the obvious characters here, to play safe.
         *
         * Returns true if any characters were mangled, false otherwise.
         */

        valid_chars = allow_globs ? VALID_CHARS_GLOB : VALID_CHARS_WITH_AT;

        लागि (; *f; f++)
                यदि (*f == '/') {
                        *(t++) = '-';
                        mangled = true;
                } अन्यथा यदि (!म_अक्षर(valid_chars, *f)) {
                        t = do_escape_char(*f, t);
                        mangled = true;
                } अन्यथा
                        *(t++) = *f;
        *t = 0;

        फर्काऊ mangled;
}

/**
 *  Convert a string to a unit name. /dev/blah is converted to dev-blah.device,
 *  /blah/blah is converted to blah-blah.mount, anything else is left alone,
 *  except that @suffix is appended if a valid unit suffix is not present.
 *
 *  If @allow_globs, globs characters are preserved. Otherwise, they are escaped.
 */
पूर्णाङ्क unit_name_mangle_with_suffix(
                अचर वर्ण *name,
                अचर वर्ण *operation,
                UnitNameMangle flags,
                अचर वर्ण *suffix,
                वर्ण **ret) {

        _cleanup_free_ वर्ण *s = रिक्त;
        bool mangled, suggest_escape = true, warn = FLAGS_SET(flags, UNIT_NAME_MANGLE_WARN);
        पूर्णाङ्क r;

        प्रतिज्ञा(name);
        प्रतिज्ञा(suffix);
        प्रतिज्ञा(ret);

        यदि (isempty(name)) /* We cannot mangle empty unit names to become valid, sorry. */
                फर्काऊ -EINVAL;

        यदि (!unit_suffix_is_valid(suffix))
                फर्काऊ -EINVAL;

        /* Already a fully valid unit name? If so, no mangling is necessary... */
        यदि (unit_name_is_valid(name, UNIT_NAME_ANY)) {
                यदि (FLAGS_SET(flags, UNIT_NAME_MANGLE_STRICT) && !endswith(name, suffix)) {
                        अचर वर्ण *e = ASSERT_PTR(म_खोजप(name, '.'));

                        फर्काऊ log_full_errno(warn ? LOG_NOTICE : LOG_DEBUG,
                                              SYNTHETIC_ERRNO(EINVAL),
                                              "Unit name \"%s\" has unit type \"%s\", but \"%s\" is expected%s%s.",
                                              name, e + 1, suffix + 1,
                                              operation ? " " : "", strempty(operation));
                }

                गच्छ good;
        }

        /* Already a fully valid globbing expression? If so, no mangling is necessary either... */
        यदि (string_is_glob(name) && in_charset(name, VALID_CHARS_GLOB)) {
                यदि (FLAGS_SET(flags, UNIT_NAME_MANGLE_GLOB))
                        गच्छ good;
                log_full(warn ? LOG_NOTICE : LOG_DEBUG,
                         "Glob pattern passed%s%s, but globs are not supported for this.",
                         operation ? " " : "", strempty(operation));
                suggest_escape = false;
        }

        यदि (path_is_absolute(name)) {
                _cleanup_free_ वर्ण *n = रिक्त, *u = रिक्त;

                r = path_simplify_alloc(name, &n);
                यदि (r < 0)
                        फर्काऊ r;

                यदि (is_device_path(n)) {
                        r = unit_name_from_path(n, ".device", &u);
                        यदि (r >= 0) {
                                यदि (FLAGS_SET(flags, UNIT_NAME_MANGLE_STRICT) && !streq(suffix, ".device"))
                                        फर्काऊ log_full_errno(warn ? LOG_NOTICE : LOG_DEBUG,
                                                              SYNTHETIC_ERRNO(EINVAL),
                                                              "Path \"%s\" resolves to unit type \"device\", but \"%s\" is expected%s%s.",
                                                              name, suffix + 1,
                                                              operation ? " " : "", strempty(operation));

                                *ret = TAKE_PTR(u);
                                फर्काऊ 1;
                        }
                        यदि (r != -EINVAL)
                                फर्काऊ r;
                }

                r = unit_name_from_path(n, ".mount", &u);
                यदि (r >= 0) {
                        यदि (FLAGS_SET(flags, UNIT_NAME_MANGLE_STRICT) && !streq(suffix, ".mount"))
                                फर्काऊ log_full_errno(warn ? LOG_NOTICE : LOG_DEBUG,
                                                      SYNTHETIC_ERRNO(EINVAL),
                                                      "Path \"%s\" resolves to unit type \"mount\", but \"%s\" is expected%s%s.",
                                                      name, suffix + 1,
                                                      operation ? " " : "", strempty(operation));

                        *ret = TAKE_PTR(u);
                        फर्काऊ 1;
                }
                यदि (r != -EINVAL)
                        फर्काऊ r;
        }

        s = new(वर्ण, म_माप(name) * 4 + म_माप(suffix) + 1);
        यदि (!s)
                फर्काऊ -ENOMEM;

        mangled = do_escape_mangle(name, FLAGS_SET(flags, UNIT_NAME_MANGLE_GLOB), s);
        यदि (mangled)
                log_full(warn ? LOG_NOTICE : LOG_DEBUG,
                         "Invalid unit name \"%s\" escaped as \"%s\"%s.",
                         name, s,
                         suggest_escape ? " (maybe you should use systemd-escape?)" : "");

        /* Append a suffix if it doesn't have any, but only if this is not a glob, so that we can allow
         * "foo.*" as a valid glob. */
        यदि ((!FLAGS_SET(flags, UNIT_NAME_MANGLE_GLOB) || !string_is_glob(s)) && unit_name_to_type(s) < 0)
                म_जोड़ो(s, suffix);

        /* Make sure mangling didn't grow this too large (but don't do this check if globbing is allowed,
         * since globs generally do not qualify as valid unit names) */
        यदि (!FLAGS_SET(flags, UNIT_NAME_MANGLE_GLOB) && !unit_name_is_valid(s, UNIT_NAME_ANY))
                फर्काऊ -EINVAL;

        *ret = TAKE_PTR(s);
        फर्काऊ 1;

good:
        फर्काऊ strdup_to(ret, name);
}

पूर्णाङ्क slice_build_parent_slice(अचर वर्ण *slice, वर्ण **ret) {
        प्रतिज्ञा(slice);
        प्रतिज्ञा(ret);

        यदि (!slice_name_is_valid(slice))
                फर्काऊ -EINVAL;

        यदि (streq(slice, SPECIAL_जरा_SLICE)) {
                *ret = रिक्त;
                फर्काऊ 0;
        }

        _cleanup_free_ वर्ण *s = strdup(slice);
        यदि (!s)
                फर्काऊ -ENOMEM;

        वर्ण *dash = म_खोजप(s, '-');
        यदि (!dash)
                फर्काऊ strdup_to_full(ret, SPECIAL_जरा_SLICE);

        /* We know that s ended with .slice before truncation, so we have enough space. */
        म_नकल(dash, ".slice");

        *ret = TAKE_PTR(s);
        फर्काऊ 1;
}

पूर्णाङ्क slice_build_subslice(अचर वर्ण *slice, अचर वर्ण *name, वर्ण **ret) {
        वर्ण *subslice;

        प्रतिज्ञा(slice);
        प्रतिज्ञा(name);
        प्रतिज्ञा(ret);

        यदि (!slice_name_is_valid(slice))
                फर्काऊ -EINVAL;

        यदि (!unit_prefix_is_valid(name))
                फर्काऊ -EINVAL;

        यदि (streq(slice, SPECIAL_जरा_SLICE))
                subslice = strjoin(name, ".slice");
        अन्यथा {
                अचर वर्ण *e;

                assert_se(e = endswith(slice, ".slice"));

                subslice = new(वर्ण, (e - slice) + 1 + म_माप(name) + 6 + 1);
                यदि (!subslice)
                        फर्काऊ -ENOMEM;

                stpcpy(stpcpy(stpcpy(mempcpy(subslice, slice, e - slice), "-"), name), ".slice");
        }

        *ret = subslice;
        फर्काऊ 0;
}

bool slice_name_is_valid(अचर वर्ण *name) {
        अचर वर्ण *p, *e;
        bool dash = false;

        यदि (!unit_name_is_valid(name, UNIT_NAME_PLAIN))
                फर्काऊ false;

        यदि (streq(name, SPECIAL_जरा_SLICE))
                फर्काऊ true;

        e = endswith(name, ".slice");
        यदि (!e)
                फर्काऊ false;

        लागि (p = name; p < e; p++) {

                यदि (*p == '-') {

                        /* Don't allow initial dash */
                        यदि (p == name)
                                फर्काऊ false;

                        /* Don't allow multiple dashes */
                        यदि (dash)
                                फर्काऊ false;

                        dash = true;
                } अन्यथा
                        dash = false;
        }

        /* Don't allow trailing hash */
        यदि (dash)
                फर्काऊ false;

        फर्काऊ true;
}

bool unit_name_prefix_equal(अचर वर्ण *a, अचर वर्ण *b) {
        अचर वर्ण *p, *q;

        प्रतिज्ञा(a);
        प्रतिज्ञा(b);

        यदि (!unit_name_is_valid(a, UNIT_NAME_ANY) || !unit_name_is_valid(b, UNIT_NAME_ANY))
                फर्काऊ false;

        p = म_अक्षर(a, '@');
        यदि (!p)
                p = म_खोजप(a, '.');

        q = म_अक्षर(b, '@');
        यदि (!q)
                q = म_खोजप(b, '.');

        प्रतिज्ञा(p);
        प्रतिज्ञा(q);

        फर्काऊ memcmp_nn(a, p - a, b, q - b) == 0;
}
