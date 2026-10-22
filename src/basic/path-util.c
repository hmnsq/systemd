/* SPDX-License-Identifier: LGPL-2.1-or-later */

#समावेश <fnmatch.h>
#समावेश <stdio.h>
#समावेश <stdlib.h>
#समावेश <unistd.h>

#समावेश "alloc-util.h"
#समावेश "chase.h"
#समावेश "errno-util.h"
#समावेश "extract-word.h"
#समावेश "fd-util.h"
#समावेश "fs-util.h"
#समावेश "glob-util.h"
#समावेश "log.h"
#समावेश "path-util.h"
#समावेश "stat-util.h"
#समावेश "string-util.h"
#समावेश "strv.h"

bool is_path(स्थिर अक्षर *p) {
        अगर (!p) /* A NULL pointer is definitely not a path */
                वापस false;

        वापस म_अक्षर(p, '/');
}

पूर्णांक path_split_and_make_absolute(स्थिर अक्षर *p, अक्षर ***ret) {
        _cleanup_strv_free_ अक्षर **l = शून्य;
        पूर्णांक r;

        निश्चित(p);
        निश्चित(ret);

        l = strv_split(p, ":");
        अगर (!l)
                वापस -ENOMEM;

        r = path_strv_make_absolute_cwd(l);
        अगर (r < 0)
                वापस r;

        *ret = TAKE_PTR(l);
        वापस r;
}

अक्षर* path_make_absolute(स्थिर अक्षर *p, स्थिर अक्षर *prefix) {
        निश्चित(p);

        /* Makes every item in the list an absolute path by prepending
         * the prefix, if specified and necessary */

        अगर (path_is_absolute(p) || isempty(prefix))
                वापस strdup(p);

        वापस path_join(prefix, p);
}

पूर्णांक safe_getcwd(अक्षर **ret) {
        _cleanup_free_ अक्षर *cwd = शून्य;

        cwd = get_current_dir_name();
        अगर (!cwd)
                वापस negative_errno();

        /* Let's make sure the directory is really absolute, to protect us from the logic behind
         * CVE-2018-1000001 */
        अगर (cwd[0] != '/')
                वापस -ENOMEDIUM;

        अगर (ret)
                *ret = TAKE_PTR(cwd);

        वापस 0;
}

पूर्णांक path_make_absolute_cwd(स्थिर अक्षर *p, अक्षर **ret) {
        अक्षर *c;
        पूर्णांक r;

        निश्चित(p);
        निश्चित(ret);

        /* Similar to path_make_absolute(), but prefixes with the
         * current working directory. */

        अगर (path_is_absolute(p))
                c = strdup(p);
        अन्यथा {
                _cleanup_free_ अक्षर *cwd = शून्य;

                r = safe_getcwd(&cwd);
                अगर (r < 0)
                        वापस r;

                c = path_join(cwd, p);
        }
        अगर (!c)
                वापस -ENOMEM;

        *ret = c;
        वापस 0;
}

पूर्णांक path_make_relative(स्थिर अक्षर *from, स्थिर अक्षर *to, अक्षर **ret) {
        _cleanup_free_ अक्षर *परिणाम = शून्य;
        अचिन्हित n_parents;
        स्थिर अक्षर *f, *t;
        पूर्णांक r, k;
        अक्षर *p;

        निश्चित(from);
        निश्चित(to);
        निश्चित(ret);

        /* Strips the common part, and adds ".." elements as necessary. */

        अगर (!path_is_absolute(from) || !path_is_absolute(to))
                वापस -EINVAL;

        क्रम (;;) {
                r = path_find_first_component(&from, true, &f);
                अगर (r < 0)
                        वापस r;

                k = path_find_first_component(&to, true, &t);
                अगर (k < 0)
                        वापस k;

                अगर (r == 0) {
                        /* end of 'from' */
                        अगर (k == 0) {
                                /* from and to are equivalent. */
                                परिणाम = strdup(".");
                                अगर (!परिणाम)
                                        वापस -ENOMEM;
                        } अन्यथा {
                                /* 'to' is inside of 'from'. */
                                r = path_simplify_alloc(t, &परिणाम);
                                अगर (r < 0)
                                        वापस r;

                                अगर (!path_is_valid(परिणाम))
                                        वापस -EINVAL;
                        }

                        *ret = TAKE_PTR(परिणाम);
                        वापस 0;
                }

                अगर (r != k || !strneq(f, t, r))
                        अवरोध;
        }

        /* If we're here, then "from_dir" has one or more elements that need to
         * be replaced with "..". */

        क्रम (n_parents = 1;; n_parents++) {
                /* If this includes ".." we can't do a simple series of "..". */
                r = path_find_first_component(&from, false, &f);
                अगर (r < 0)
                        वापस r;
                अगर (r == 0)
                        अवरोध;
        }

        अगर (isempty(t) && n_parents * 3 > PATH_MAX)
                /* PATH_MAX is counted *with* the trailing NUL byte */
                वापस -EINVAL;

        परिणाम = new(अक्षर, n_parents * 3 + !isempty(t) + strlen_ptr(t));
        अगर (!परिणाम)
                वापस -ENOMEM;

        क्रम (p = परिणाम; n_parents > 0; n_parents--)
                p = mempcpy(p, "../", 3);

        अगर (isempty(t)) {
                /* Remove trailing slash and terminate string. */
                *(--p) = '\0';
                *ret = TAKE_PTR(परिणाम);
                वापस 0;
        }

        म_नकल(p, t);

        path_simplify(परिणाम);

        अगर (!path_is_valid(परिणाम))
                वापस -EINVAL;

        *ret = TAKE_PTR(परिणाम);
        वापस 0;
}

पूर्णांक path_make_relative_parent(स्थिर अक्षर *from_child, स्थिर अक्षर *to, अक्षर **ret) {
        _cleanup_free_ अक्षर *from = शून्य;
        पूर्णांक r;

        निश्चित(from_child);
        निश्चित(to);
        निश्चित(ret);

        /* Similar to path_make_relative(), but provides the relative path from the parent directory of
         * 'from_child'. This may be useful when creating relative symlink.
         *
         * E.g.
         * - from = "/path/to/aaa", to = "/path/to/bbb"
         *      path_make_relative(from, to) = "../bbb"
         *      path_make_relative_parent(from, to) = "bbb"
         *
         * - from = "/path/to/aaa/bbb", to = "/path/to/ccc/ddd"
         *      path_make_relative(from, to) = "../../ccc/ddd"
         *      path_make_relative_parent(from, to) = "../ccc/ddd"
         */

        r = path_extract_directory(from_child, &from);
        अगर (r < 0)
                वापस r;

        वापस path_make_relative(from, to, ret);
}

अक्षर* path_startswith_strv(स्थिर अक्षर *p, अक्षर * स्थिर *strv) {
        निश्चित(p);

        STRV_FOREACH(s, strv) {
                अक्षर *t;

                t = path_startswith(p, *s);
                अगर (t)
                        वापस t;
        }

        वापस शून्य;
}

पूर्णांक path_strv_make_absolute_cwd(अक्षर **l) {
        पूर्णांक r;

        /* Goes through every item in the string list and makes it
         * absolute. This works in place and won't rollback any
         * changes on failure. */

        STRV_FOREACH(s, l) {
                अक्षर *t;

                r = path_make_absolute_cwd(*s, &t);
                अगर (r < 0)
                        वापस r;

                path_simplify(t);
                free_and_replace(*s, t);
        }

        वापस 0;
}

अक्षर** path_strv_resolve(अक्षर **l, स्थिर अक्षर *मूल) {
        अचिन्हित k = 0;
        bool enomem = false;
        पूर्णांक r;

        अगर (strv_isempty(l))
                वापस l;

        /* Go through every item in the string list and canonicalize the path.
         * This works in place and doesn't roll back any changes on failure. */

        STRV_FOREACH(s, l) {
                _cleanup_free_ अक्षर *orig = शून्य;
                अक्षर *t, *u;

                अगर (मूल) {
                        अगर (!path_is_absolute(*s)) {
                                log_debug("Ignoring non-absolute search path '%s'.", *s);
                                मुक्त(*s);
                                जारी;
                        }

                        orig = *s;
                        t = path_join(मूल, orig);
                        अगर (!t) {
                                enomem = true;
                                जारी;
                        }
                } अन्यथा
                        t = *s;

                r = chase(t, मूल, 0, &u, शून्य);
                अगर (r == -ENOENT) {
                        अगर (मूल) {
                                u = TAKE_PTR(orig);
                                मुक्त(t);
                        } अन्यथा
                                u = t;
                } अन्यथा अगर (r < 0) {
                        मुक्त(t);

                        अगर (r == -ENOMEM)
                                enomem = true;

                        जारी;
                } अन्यथा अगर (मूल) {
                        अक्षर *x;

                        मुक्त(t);
                        x = path_startswith(u, मूल);
                        अगर (x) {
                                /* restore the slash if it was lost */
                                अगर (!startswith(x, "/"))
                                        *(--x) = '/';

                                t = strdup(x);
                                मुक्त(u);
                                अगर (!t) {
                                        enomem = true;
                                        जारी;
                                }
                                u = t;
                        } अन्यथा {
                                /* canonicalized path goes outside of
                                 * prefix, keep the original path instead */
                                free_and_replace(u, orig);
                        }
                } अन्यथा
                        मुक्त(t);

                l[k++] = u;
        }

        l[k] = शून्य;

        अगर (enomem)
                वापस शून्य;

        वापस l;
}

अक्षर** path_strv_resolve_uniq(अक्षर **l, स्थिर अक्षर *मूल) {

        अगर (strv_isempty(l))
                वापस l;

        अगर (!path_strv_resolve(l, मूल))
                वापस शून्य;

        वापस strv_uniq(l);
}

अक्षर* skip_leading_slash(स्थिर अक्षर *p) {
        वापस skip_leading_chars(p, "/");
}

अक्षर* path_simplify_full(अक्षर *path, PathSimplifyFlags flags) {
        bool add_slash = false, keep_trailing_slash, absolute, beginning = true;
        अक्षर *f = path;
        पूर्णांक r;

        /* Removes redundant inner and trailing slashes. Also removes unnecessary dots.
         * Modifies the passed string in-place.
         *
         * ///foo//./bar/.   becomes /foo/bar
         * .//./foo//./bar/. becomes foo/bar
         * /../foo/bar       becomes /foo/bar
         * /../foo/bar/..    becomes /foo/bar/..
         */

        अगर (isempty(path))
                वापस path;

        keep_trailing_slash = FLAGS_SET(flags, PATH_SIMPLIFY_KEEP_TRAILING_SLASH) && endswith(path, "/");

        absolute = path_is_absolute(path);
        f += absolute;  /* Keep leading /, if present. */

        क्रम (स्थिर अक्षर *p = f;;) {
                स्थिर अक्षर *e;

                r = path_find_first_component(&p, true, &e);
                अगर (r == 0)
                        अवरोध;

                अगर (r > 0 && absolute && beginning && path_startswith(e, ".."))
                        /* If we're at the beginning of an absolute path, we can safely skip ".." */
                        जारी;

                beginning = false;

                अगर (add_slash)
                        *f++ = '/';

                अगर (r < 0) {
                        /* if path is invalid, then refuse to simplify the remaining part. */
                        स_हटाओ(f, p, म_माप(p) + 1);
                        वापस path;
                }

                स_हटाओ(f, e, r);
                f += r;

                add_slash = true;
        }

        /* Special rule, if we stripped everything, we need a "." for the current directory. */
        अगर (f == path)
                *f++ = '.';

        अगर (*(f-1) != '/' && keep_trailing_slash)
                *f++ = '/';

        *f = '\0';
        वापस path;
}

पूर्णांक path_simplify_alloc(स्थिर अक्षर *path, अक्षर **ret) {
        निश्चित(ret);

        अगर (!path) {
                *ret = शून्य;
                वापस 0;
        }

        अक्षर *t = strdup(path);
        अगर (!t)
                वापस -ENOMEM;

        *ret = path_simplify(t);
        वापस 0;
}

अक्षर* path_startswith_full(स्थिर अक्षर *original_path, स्थिर अक्षर *prefix, PathStartWithFlags flags) {
        निश्चित(original_path);
        निश्चित(prefix);

        /* Returns a pointer to the start of the first component after the parts matched by
         * the prefix, iff
         * - both paths are absolute or both paths are relative,
         * and
         * - each component in prefix in turn matches a component in path at the same position.
         * An empty string will be returned when the prefix and path are equivalent.
         *
         * Returns NULL otherwise.
         */

        स्थिर अक्षर *path = original_path;

        अगर ((path[0] == '/') != (prefix[0] == '/'))
                वापस शून्य;

        क्रम (;;) {
                स्थिर अक्षर *p, *q;
                पूर्णांक m, n;

                m = path_find_first_component(&path, !FLAGS_SET(flags, PATH_STARTSWITH_REFUSE_DOT_DOT), &p);
                अगर (m < 0)
                        वापस शून्य;

                n = path_find_first_component(&prefix, !FLAGS_SET(flags, PATH_STARTSWITH_REFUSE_DOT_DOT), &q);
                अगर (n < 0)
                        वापस शून्य;

                अगर (n == 0) {
                        अगर (!p)
                                p = path;

                        अगर (FLAGS_SET(flags, PATH_STARTSWITH_RETURN_LEADING_SLASH)) {

                                अगर (p <= original_path)
                                        वापस शून्य;

                                p--;

                                अगर (*p != '/')
                                        वापस शून्य;
                        }

                        वापस (अक्षर*) p;
                }

                अगर (m != n)
                        वापस शून्य;

                अगर (!strneq(p, q, m))
                        वापस शून्य;
        }
}

पूर्णांक path_compare(स्थिर अक्षर *a, स्थिर अक्षर *b) {
        पूर्णांक r;

        /* Order NULL before non-NULL */
        r = CMP(!!a, !!b);
        अगर (r != 0)
                वापस r;

        /* A relative path and an absolute path must not compare as equal.
         * Which one is sorted before the other does not really matter.
         * Here a relative path is ordered before an absolute path. */
        r = CMP(path_is_absolute(a), path_is_absolute(b));
        अगर (r != 0)
                वापस r;

        क्रम (;;) {
                स्थिर अक्षर *aa, *bb;
                पूर्णांक j, k;

                j = path_find_first_component(&a, true, &aa);
                k = path_find_first_component(&b, true, &bb);

                अगर (j < 0 || k < 0) {
                        /* When one of paths is invalid, order invalid path after valid one. */
                        r = CMP(j < 0, k < 0);
                        अगर (r != 0)
                                वापस r;

                        /* fallback to use strcmp() if both paths are invalid. */
                        वापस म_भेद(a, b);
                }

                /* Order prefixes first: "/foo" before "/foo/bar" */
                अगर (j == 0) {
                        अगर (k == 0)
                                वापस 0;
                        वापस -1;
                }
                अगर (k == 0)
                        वापस 1;

                /* Alphabetical sort: "/foo/aaa" before "/foo/b" */
                r = स_भेद(aa, bb, MIN(j, k));
                अगर (r != 0)
                        वापस r;

                /* Sort "/foo/a" before "/foo/aaa" */
                r = CMP(j, k);
                अगर (r != 0)
                        वापस r;
        }
}

पूर्णांक path_compare_filename(स्थिर अक्षर *a, स्थिर अक्षर *b) {
        _cleanup_free_ अक्षर *fa = शून्य, *fb = शून्य;
        पूर्णांक r, j, k;

        /* Order NULL before non-NULL */
        r = CMP(!!a, !!b);
        अगर (r != 0)
                वापस r;

        j = path_extract_filename(a, &fa);
        k = path_extract_filename(b, &fb);

        /* When one of paths is "." or root, then order it earlier. */
        r = CMP(j != -EADDRNOTAVAIL, k != -EADDRNOTAVAIL);
        अगर (r != 0)
                वापस r;

        /* When one of paths is invalid (or we get OOM), order invalid path after valid one. */
        r = CMP(j < 0, k < 0);
        अगर (r != 0)
                वापस r;

        /* fallback to use strcmp() if both paths are invalid. */
        अगर (j < 0)
                वापस म_भेद(a, b);

        वापस म_भेद(fa, fb);
}

पूर्णांक path_equal_or_inode_same_full(स्थिर अक्षर *a, स्थिर अक्षर *b, पूर्णांक flags) {
        /* Returns true if paths are of the same entry, false if not, <0 on error. */

        अगर (path_equal(a, b))
                वापस 1;

        अगर (!a || !b)
                वापस 0;

        वापस inode_same(a, b, flags);
}

अक्षर* path_extend_internal(अक्षर **x, ...) {
        माप_प्रकार sz, old_sz;
        अक्षर *q, *nx;
        स्थिर अक्षर *p;
        बहु_सूची ap;
        bool slash;

        POINTER_MAY_BE_NULL(x);

        /* Joins all listed strings until the sentinel and places a "/" between them unless the strings
         * end/begin already with one so that it is unnecessary. Note that slashes which are already
         * duplicate won't be removed. The string returned is hence always equal to or longer than the sum of
         * the lengths of the individual strings.
         *
         * The first argument may be an already allocated string that is extended via realloc() if
         * non-NULL. path_extend() and path_join() are macro wrappers around this function, making use of the
         * first parameter to distinguish the two operations.
         *
         * Note: any listed empty string is simply skipped. This can be useful for concatenating strings of
         * which some are optional.
         *
         * Examples:
         *
         * path_join("foo", "bar") → "foo/bar"
         * path_join("foo/", "bar") → "foo/bar"
         * path_join("", "foo", "", "bar", "") → "foo/bar" */

        sz = old_sz = x ? strlen_ptr(*x) : 0;
        बहु_शुरू(ap, x);
        जबतक ((p = बहु_तर्क(ap, अक्षर*)) != POINTER_MAX) {
                माप_प्रकार add;

                अगर (isempty(p))
                        जारी;

                add = 1 + म_माप(p);
                अगर (sz > SIZE_MAX - add) { /* overflow check */
                        बहु_पूर्ण(ap);
                        वापस शून्य;
                }

                sz += add;
        }
        बहु_पूर्ण(ap);

        nx = पुनः_स्मृति(x ? *x : शून्य, GREEDY_ALLOC_ROUND_UP(sz+1));
        अगर (!nx)
                वापस शून्य;
        अगर (x)
                *x = nx;

        अगर (old_sz > 0)
                slash = nx[old_sz-1] == '/';
        अन्यथा {
                nx[old_sz] = 0;
                slash = true; /* no need to generate a slash anymore */
        }

        q = nx + old_sz;

        बहु_शुरू(ap, x);
        जबतक ((p = बहु_तर्क(ap, अक्षर*)) != POINTER_MAX) {
                अगर (isempty(p))
                        जारी;

                अगर (!slash && p[0] != '/')
                        *(q++) = '/';

                q = stpcpy(q, p);
                slash = endswith(p, "/");
        }
        बहु_पूर्ण(ap);

        वापस nx;
}

पूर्णांक open_and_check_executable(स्थिर अक्षर *name, स्थिर अक्षर *मूल, अक्षर **ret_path, पूर्णांक *ret_fd) {
        _cleanup_close_ पूर्णांक fd = -EBADF;
        _cleanup_free_ अक्षर *resolved = शून्य;
        पूर्णांक r;

        निश्चित(name);

        /* Function chase() is invoked only when root is not NULL, as using it regardless of
         * root value would alter the behavior of existing callers for example: /bin/sleep would become
         * /usr/bin/sleep when find_executables is called. Hence, this function should be invoked when
         * needed to avoid unforeseen regression or other complicated changes. */
        अगर (मूल) {
                /* prefix root to name in case full paths are not specified */
                r = chase(name, मूल, CHASE_PREFIX_मूल, &resolved, &fd);
                अगर (r < 0)
                        वापस r;

                name = resolved;
        } अन्यथा {
                /* We need to use O_PATH because there may be executables for which we have only exec permissions,
                 * but not read (usually suid executables). */
                fd = खोलो(name, O_PATH|O_CLOEXEC);
                अगर (fd < 0)
                        वापस -त्रुटि_सं;
        }

        r = fd_verify_regular(fd);
        अगर (r < 0)
                वापस r;

        r = access_fd(fd, X_OK);
        अगर (r == -ENOSYS)
                /* /proc/ is not mounted. Fall back to access(). */
                r = RET_NERRNO(access(name, X_OK));
        अगर (r < 0)
                वापस r;

        अगर (ret_path) {
                अगर (resolved)
                        *ret_path = TAKE_PTR(resolved);
                अन्यथा {
                        r = path_make_absolute_cwd(name, ret_path);
                        अगर (r < 0)
                                वापस r;

                        path_simplify(*ret_path);
                }
        }

        अगर (ret_fd)
                *ret_fd = TAKE_FD(fd);

        वापस 0;
}

पूर्णांक find_executable_full(
                स्थिर अक्षर *name,
                स्थिर अक्षर *मूल,
                अक्षर * स्थिर *exec_खोज_path,
                bool use_path_envvar,
                अक्षर **ret_filename,
                पूर्णांक *ret_fd) {

        पूर्णांक last_error = -ENOENT, r = 0;

        निश्चित(name);

        अगर (is_path(name))
                वापस open_and_check_executable(name, मूल, ret_filename, ret_fd);

        अगर (exec_खोज_path) {
                STRV_FOREACH(element, exec_खोज_path) {
                        _cleanup_free_ अक्षर *full_path = शून्य;

                        अगर (!path_is_absolute(*element)) {
                                log_debug("Exec search path '%s' isn't absolute, ignoring.", *element);
                                जारी;
                        }

                        full_path = path_join(*element, name);
                        अगर (!full_path)
                                वापस -ENOMEM;

                        r = open_and_check_executable(full_path, मूल, ret_filename, ret_fd);
                        अगर (r >= 0)
                                वापस 0;
                        अगर (r != -EACCES)
                                last_error = r;
                }
                वापस last_error;
        }

        स्थिर अक्षर *p = शून्य;

        अगर (use_path_envvar)
                /* Plain getenv, not secure_getenv, because we want to actually allow the user to pick the
                 * binary. */
                p = दो_पर्या("PATH");
        अगर (!p)
                p = default_PATH();

        /* Resolve a single-component name to a full path */
        क्रम (;;) {
                _cleanup_free_ अक्षर *element = शून्य;

                r = extract_first_word(&p, &element, ":", EXTRACT_RELAX|EXTRACT_DONT_COALESCE_SEPARATORS);
                अगर (r < 0)
                        वापस r;
                अगर (r == 0)
                        अवरोध;

                अगर (!path_is_absolute(element)) {
                        log_debug("Exec search path '%s' isn't absolute, ignoring.", element);
                        जारी;
                }

                अगर (!path_extend(&element, name))
                        वापस -ENOMEM;

                r = open_and_check_executable(element, मूल, ret_filename, ret_fd);
                अगर (r >= 0) /* Found it! */
                        वापस 0;
                /* PATH entries which we don't have access to are ignored, as per tradition. */
                अगर (r != -EACCES)
                        last_error = r;
        }

        वापस last_error;
}

अटल पूर्णांक executable_is_good(स्थिर अक्षर *executable) {
        _cleanup_free_ अक्षर *p = शून्य, *d = शून्य;
        पूर्णांक r;

        r = find_executable(executable, &p);
        अगर (r == -ENOENT)
                वापस 0;
        अगर (r < 0)
                वापस r;

        /* An fsck that is linked to /bin/true is a non-existent fsck */

        r = readlink_malloc(p, &d);
        अगर (r == -EINVAL) /* not a symlink */
                वापस 1;
        अगर (r < 0)
                वापस r;

        वापस !PATH_IN_SET(d, "true",
                               "/bin/true",
                               "/usr/bin/true",
                               "/dev/null");
}

पूर्णांक fsck_exists(व्योम) {
        वापस executable_is_good("fsck");
}

पूर्णांक fsck_exists_for_fstype(स्थिर अक्षर *fstype) {
        स्थिर अक्षर *checker;
        पूर्णांक r;

        निश्चित(fstype);

        अगर (streq(fstype, "auto"))
                वापस -EINVAL;

        r = fsck_exists();
        अगर (r <= 0)
                वापस r;

        checker = strjoina("fsck.", fstype);
        वापस executable_is_good(checker);
}

अटल स्थिर अक्षर* skip_slash_or_dot(स्थिर अक्षर *p) {
        POINTER_MAY_BE_NULL(p);

        क्रम (; !isempty(p); p++) {
                अगर (*p == '/')
                        जारी;
                अगर (startswith(p, "./")) {
                        p++;
                        जारी;
                }
                अवरोध;
        }
        वापस p;
}

पूर्णांक path_find_first_component(स्थिर अक्षर **p, bool accept_dot_dot, स्थिर अक्षर **ret) {
        स्थिर अक्षर *q, *first, *end_first, *अग्र_2;
        माप_प्रकार len;

        निश्चित(p);

        /* When a path is input, then returns the pointer to the first component and its length, and
         * move the input pointer to the next component or nul. This skips both over any '/'
         * immediately *before* and *after* the first component before returning.
         *
         * Examples
         *   Input:  p: "//.//aaa///bbbbb/cc"
         *   Output: p: "bbbbb///cc"
         *           ret: "aaa///bbbbb/cc"
         *           return value: 3 (== strlen("aaa"))
         *
         *   Input:  p: "aaa//"
         *   Output: p: (pointer to NUL)
         *           ret: "aaa//"
         *           return value: 3 (== strlen("aaa"))
         *
         *   Input:  p: "/", ".", ""
         *   Output: p: (pointer to NUL)
         *           ret: NULL
         *           return value: 0
         *
         *   Input:  p: NULL
         *   Output: p: NULL
         *           ret: NULL
         *           return value: 0
         *
         *   Input:  p: "(too long component)"
         *   Output: return value: -EINVAL
         *
         *   (when accept_dot_dot is false)
         *   Input:  p: "//..//aaa///bbbbb/cc"
         *   Output: return value: -EINVAL
         */

        q = *p;

        first = skip_slash_or_dot(q);
        अगर (isempty(first)) {
                *p = first;
                अगर (ret)
                        *ret = शून्य;
                वापस 0;
        }
        अगर (streq(first, ".")) {
                *p = first + 1;
                अगर (ret)
                        *ret = शून्य;
                वापस 0;
        }

        end_first = strchrnul(first, '/');
        len = end_first - first;

        अगर (len > NAME_MAX)
                वापस -EINVAL;
        अगर (!accept_dot_dot && len == 2 && first[0] == '.' && first[1] == '.')
                वापस -EINVAL;

        अग्र_2 = skip_slash_or_dot(end_first);

        *p = अग्र_2 + streq(अग्र_2, ".");
        अगर (ret)
                *ret = first;
        वापस len;
}

अटल स्थिर अक्षर* skip_slash_or_dot_backward(स्थिर अक्षर *path, स्थिर अक्षर *q) {
        निश्चित(path);
        निश्चित(!q || q >= path);

        क्रम (; q; q = PTR_SUB1(q, path)) {
                अगर (*q == '/')
                        जारी;
                अगर (q > path && strneq(q - 1, "/.", 2))
                        जारी;
                अगर (q == path && *q == '.')
                        जारी;
                अवरोध;
        }
        वापस q;
}

पूर्णांक path_find_last_component(स्थिर अक्षर *path, bool accept_dot_dot, स्थिर अक्षर **अग्र_2, स्थिर अक्षर **ret) {
        स्थिर अक्षर *q, *last_end, *last_begin;
        माप_प्रकार len;

        POINTER_MAY_BE_NULL(अग्र_2);
        POINTER_MAY_BE_NULL(ret);

        /* Similar to path_find_first_component(), but search components from the end.
        *
        * Examples
        *   Input:  path: "//.//aaa///bbbbb/cc//././"
        *           next: NULL
        *   Output: next: "/cc//././"
        *           ret: "cc//././"
        *           return value: 2 (== strlen("cc"))
        *
        *   Input:  path: "//.//aaa///bbbbb/cc//././"
        *           next: "/cc//././"
        *   Output: next: "///bbbbb/cc//././"
        *           ret: "bbbbb/cc//././"
        *           return value: 5 (== strlen("bbbbb"))
        *
        *   Input:  path: "//.//aaa///bbbbb/cc//././"
        *           next: "///bbbbb/cc//././"
        *   Output: next: "//.//aaa///bbbbb/cc//././" (next == path)
        *           ret: "aaa///bbbbb/cc//././"
        *           return value: 3 (== strlen("aaa"))
        *
        *   Input:  path: "/", ".", "", or NULL
        *   Output: next: equivalent to path
        *           ret: NULL
        *           return value: 0
        *
        *   Input:  path: "(too long component)"
        *   Output: return value: -EINVAL
        *
        *   (when accept_dot_dot is false)
        *   Input:  path: "//..//aaa///bbbbb/cc/..//"
        *   Output: return value: -EINVAL
        */

        अगर (isempty(path)) {
                अगर (अग्र_2)
                        *अग्र_2 = path;
                अगर (ret)
                        *ret = शून्य;
                वापस 0;
        }

        अगर (अग्र_2 && *अग्र_2) {
                अगर (*अग्र_2 < path || *अग्र_2 > path + म_माप(path))
                        वापस -EINVAL;
                अगर (*अग्र_2 == path) {
                        अगर (ret)
                                *ret = शून्य;
                        वापस 0;
                }
                अगर (!IN_SET(**अग्र_2, '\0', '/'))
                        वापस -EINVAL;
                q = *अग्र_2 - 1;
        } अन्यथा
                q = path + म_माप(path) - 1;

        q = skip_slash_or_dot_backward(path, q);
        अगर (!q) { /* the root directory, "." or "./" */
                अगर (अग्र_2)
                        *अग्र_2 = path;
                अगर (ret)
                        *ret = शून्य;
                वापस 0;
        }

        last_end = q + 1;

        जबतक (q && *q != '/')
                q = PTR_SUB1(q, path);

        last_begin = q ? q + 1 : path;
        len = last_end - last_begin;

        अगर (len > NAME_MAX)
                वापस -EINVAL;
        अगर (!accept_dot_dot && len == 2 && strneq(last_begin, "..", 2))
                वापस -EINVAL;

        अगर (अग्र_2) {
                q = skip_slash_or_dot_backward(path, q);
                *अग्र_2 = q ? q + 1 : path;
        }

        अगर (ret)
                *ret = last_begin;
        वापस len;
}

स्थिर अक्षर* last_path_component(स्थिर अक्षर *path) {

        /* Finds the last component of the path, preserving the optional trailing slash that signifies a directory.
         *
         *    a/b/c → c
         *    a/b/c/ → c/
         *    x → x
         *    x/ → x/
         *    /y → y
         *    /y/ → y/
         *    / → /
         *    // → /
         *    /foo/a → a
         *    /foo/a/ → a/
         *
         *    Also, the empty string is mapped to itself.
         *
         * This is different than basename(), which returns "" when a trailing slash is present.
         *
         * This always succeeds (except if you pass NULL in which case it returns NULL, too).
         */

        अचिन्हित l, k;

        अगर (!path)
                वापस शून्य;

        l = k = म_माप(path);
        अगर (l == 0) /* special case — an empty string */
                वापस path;

        जबतक (k > 0 && path[k-1] == '/')
                k--;

        अगर (k == 0) /* the root directory */
                वापस path + l - 1;

        जबतक (k > 0 && path[k-1] != '/')
                k--;

        वापस path + k;
}

पूर्णांक path_split_prefix_filename(स्थिर अक्षर *path, अक्षर **ret_dir, अक्षर **ret_filename) {
        _cleanup_free_ अक्षर *d = शून्य;
        स्थिर अक्षर *c, *अग्र_2 = शून्य;
        पूर्णांक r;

        POINTER_MAY_BE_NULL(path);

        /* Split the path into dir prefix/filename pair. Returns:
         *
         * -EINVAL        → if the path is not valid
         * -EADDRNOTAVAIL → if the path refers to the uppermost directory in hierarchy (i.e. has neither
         *                  dir prefix nor filename - the root dir itself or ".")
         * -EDESTADDRREQ  → if only a filename was passed, and caller only specifies ret_dir
         * -ENOMEM        → no memory
         *
         * Returns >= 0 on success. If the input path has a trailing slash, returns O_DIRECTORY, to
         * indicate the referenced file must be a directory.
         *
         * This function guarantees to return a fully valid filename, i.e. one that passes
         * filename_is_valid() – this means "." and ".." are not accepted. */

        अगर (isempty(path))
                वापस -EINVAL;

        r = path_find_last_component(path, /* accept_dot_dot = */ false, &अग्र_2, &c);
        अगर (r < 0)
                वापस r;
        अगर (r == 0) /* root directory or "." */
                वापस -EADDRNOTAVAIL;

        अगर (ret_dir) {
                अगर (अग्र_2 == path) {
                        अगर (*path != '/') { /* filename only */
                                अगर (!ret_filename)
                                        वापस -EDESTADDRREQ;
                        } अन्यथा {
                                d = strdup("/");
                                अगर (!d)
                                        वापस -ENOMEM;
                        }
                } अन्यथा {
                        d = strndup(path, अग्र_2 - path);
                        अगर (!d)
                                वापस -ENOMEM;

                        path_simplify(d);

                        अगर (!path_is_valid(d))
                                वापस -EINVAL;
                }

        } अन्यथा अगर (!path_is_valid(path))
                /* We didn't validate the dir prefix, hence check if the whole path is valid now */
                वापस -EINVAL;

        अगर (ret_filename) {
                अक्षर *fn = strndup(c, r);
                अगर (!fn)
                        वापस -ENOMEM;

                *ret_filename = fn;
        }

        अगर (ret_dir)
                *ret_dir = TAKE_PTR(d);

        वापस म_माप(c) > (माप_प्रकार) r ? O_DIRECTORY : 0;
}

bool filename_part_is_valid(स्थिर अक्षर *p) {
        स्थिर अक्षर *e;

        /* Checks f the specified string is OK to be *part* of a filename. This is different from
         * filename_is_valid() as "." and ".." and "" are OK by this call, but not by filename_is_valid(). */

        अगर (!p)
                वापस false;

        e = strchrnul(p, '/');
        अगर (*e != 0)
                वापस false;

        अगर (e - p > NAME_MAX) /* NAME_MAX is counted *without* the trailing NUL byte */
                वापस false;

        वापस true;
}

bool filename_is_valid(स्थिर अक्षर *p) {

        अगर (isempty(p))
                वापस false;

        अगर (dot_or_dot_dot(p)) /* Yes, in this context we consider "." and ".." invalid */
                वापस false;

        वापस filename_part_is_valid(p);
}

bool path_is_valid_full(स्थिर अक्षर *p, bool accept_dot_dot) {
        अगर (isempty(p))
                वापस false;

        क्रम (स्थिर अक्षर *e = p;;) {
                पूर्णांक r;

                r = path_find_first_component(&e, accept_dot_dot, शून्य);
                अगर (r < 0)
                        वापस false;

                अगर (e - p >= PATH_MAX) /* Already reached the maximum length for a path? (PATH_MAX is counted
                                        * *with* the trailing NUL byte) */
                        वापस false;
                अगर (*e == 0)           /* End of string? Yay! */
                        वापस true;
        }
}

bool path_is_normalized(स्थिर अक्षर *p) {
        अगर (!path_is_safe(p))
                वापस false;

        अगर (streq(p, ".") || startswith(p, "./") || endswith(p, "/.") || म_माला(p, "/./"))
                वापस false;

        अगर (म_माला(p, "//"))
                वापस false;

        वापस true;
}

पूर्णांक file_in_same_dir(स्थिर अक्षर *path, स्थिर अक्षर *filename, अक्षर **ret) {
        _cleanup_free_ अक्षर *b = शून्य;
        पूर्णांक r;

        निश्चित(path);
        निश्चित(filename);
        निश्चित(ret);

        /* This removes the last component of path and appends filename, unless the latter is absolute anyway
         * or the former isn't */

        अगर (path_is_absolute(filename))
                b = strdup(filename);
        अन्यथा {
                _cleanup_free_ अक्षर *dn = शून्य;

                r = path_extract_directory(path, &dn);
                अगर (r == -EDESTADDRREQ) /* no path prefix */
                        b = strdup(filename);
                अन्यथा अगर (r < 0)
                        वापस r;
                अन्यथा
                        b = path_join(dn, filename);
        }
        अगर (!b)
                वापस -ENOMEM;

        *ret = TAKE_PTR(b);
        वापस 0;
}

bool hidden_or_backup_file(स्थिर अक्षर *filename) {
        निश्चित(filename);

        अगर (filename[0] == '.' ||
            STR_IN_SET(filename,
                       "lost+found",
                       "aquota.user",
                       "aquota.group") ||
            endswith(filename, "~"))
                वापस true;

        स्थिर अक्षर *dot = म_खोजप(filename, '.');
        अगर (!dot)
                वापस false;

        /* Please, let's not add more entries to the list below. If external projects think it's a good idea
         * to come up with always new suffixes and that everybody else should just adjust to that, then it
         * really should be on them. Hence, in future, let's not add any more entries. Instead, let's ask
         * those packages to instead adopt one of the generic suffixes/prefixes for hidden files or backups,
         * possibly augmented with an additional string. Specifically: there's now:
         *
         *    The generic suffixes "~" and ".bak" for backup files
         *    The generic prefix "." for hidden files
         *
         * Thus, if a new package manager "foopkg" wants its own set of ".foopkg-new", ".foopkg-old",
         * ".foopkg-dist" or so registered, let's refuse that and ask them to use ".foopkg.new",
         * ".foopkg.old" or ".foopkg~" instead.
         */

        वापस STR_IN_SET(dot + 1,
                          "ignore",
                          "rpmnew",
                          "rpmsave",
                          "rpmorig",
                          "dpkg-old",
                          "dpkg-new",
                          "dpkg-tmp",
                          "dpkg-dist",
                          "dpkg-bak",
                          "dpkg-backup",
                          "dpkg-remove",
                          "ucf-new",
                          "ucf-old",
                          "ucf-dist",
                          "swp",
                          "bak",
                          "old",
                          "new");
}

bool is_device_path(स्थिर अक्षर *path) {

        /* Returns true for paths that likely refer to a device, either by path in sysfs or to something in
         * /dev. This accepts any path that starts with /dev/ or /sys/ and has something after that prefix.
         * It does not actually resolve the path.
         *
         * Examples:
         * /dev/sda, /dev/sda/foo, /sys/class, /dev/.., /sys/.., /./dev/foo → yes.
         * /../dev/sda, /dev, /sys, /usr/path, /usr/../dev/sda → no.
         */

        स्थिर अक्षर *p = PATH_STARTSWITH_SET(ASSERT_PTR(path), "/dev/", "/sys/");
        वापस !isempty(p);
}

bool valid_device_गाँठ_path(स्थिर अक्षर *path) {

        /* Some superficial checks whether the specified path is a valid device node path, all without
         * looking at the actual device node. */

        अगर (!PATH_STARTSWITH_SET(path, "/dev/", "/run/systemd/inaccessible/"))
                वापस false;

        अगर (endswith(path, "/")) /* can't be a device node if it ends in a slash */
                वापस false;

        वापस path_is_normalized(path);
}

bool valid_device_allow_pattern(स्थिर अक्षर *path) {
        निश्चित(path);

        /* Like valid_device_node_path(), but also allows full-subsystem expressions like those accepted by
         * DeviceAllow= and DeviceDeny=. */

        अगर (STARTSWITH_SET(path, "block-", "char-"))
                वापस true;

        वापस valid_device_गाँठ_path(path);
}

bool dot_or_dot_dot(स्थिर अक्षर *path) {
        अगर (!path)
                वापस false;
        अगर (path[0] != '.')
                वापस false;
        अगर (path[1] == 0)
                वापस true;
        अगर (path[1] != '.')
                वापस false;

        वापस path[2] == 0;
}

bool path_implies_directory(स्थिर अक्षर *path) {

        /* Sometimes, if we look at a path we already know it must refer to a directory, because it is
         * suffixed with a slash, or its last component is "." or ".." */

        अगर (!path)
                वापस false;

        अगर (dot_or_dot_dot(path))
                वापस true;

        वापस ENDSWITH_SET(path, "/", "/.", "/..");
}

bool empty_or_मूल(स्थिर अक्षर *path) {

        /* For operations relative to some root directory, returns true if the specified root directory is
         * redundant, i.e. either / or NULL or the empty string or any equivalent. */

        अगर (isempty(path))
                वापस true;

        वापस path_equal(path, "/");
}

स्थिर अक्षर* empty_to_मूल(स्थिर अक्षर *path) {
        वापस isempty(path) ? "/" : path;
}

पूर्णांक empty_or_मूल_harder_to_null(स्थिर अक्षर **path) {
        पूर्णांक r;

        निश्चित(path);

        /* This nullifies the input path when the path is empty or points to "/". */

        अगर (empty_or_मूल(*path)) {
                *path = शून्य;
                वापस 0;
        }

        r = path_is_मूल(*path);
        अगर (r < 0)
                वापस r;
        अगर (r > 0)
                *path = शून्य;

        वापस 0;
}

bool path_strv_contains(अक्षर * स्थिर *l, स्थिर अक्षर *path) {
        निश्चित(path);

        STRV_FOREACH(i, l)
                अगर (path_equal(*i, path))
                        वापस true;

        वापस false;
}

bool prefixed_path_strv_contains(अक्षर * स्थिर *l, स्थिर अक्षर *path) {
        निश्चित(path);

        STRV_FOREACH(i, l) {
                स्थिर अक्षर *j = *i;

                अगर (*j == '-')
                        j++;
                अगर (*j == '+')
                        j++;

                अगर (path_equal(j, path))
                        वापस true;
        }

        वापस false;
}

पूर्णांक path_glob_can_match(स्थिर अक्षर *pattern, स्थिर अक्षर *prefix, अक्षर **ret) {
        निश्चित(pattern);
        निश्चित(prefix);

        क्रम (स्थिर अक्षर *a = pattern, *b = prefix;;) {
                _cleanup_free_ अक्षर *g = शून्य, *h = शून्य;
                स्थिर अक्षर *p, *q;
                पूर्णांक r, s;

                r = path_find_first_component(&a, /* accept_dot_dot= */ false, &p);
                अगर (r < 0)
                        वापस r;

                s = path_find_first_component(&b, /* accept_dot_dot= */ false, &q);
                अगर (s < 0)
                        वापस s;

                अगर (s == 0) {
                        /* The pattern matches the prefix. */
                        अगर (ret) {
                                अक्षर *t;

                                t = path_join(prefix, p);
                                अगर (!t)
                                        वापस -ENOMEM;

                                *ret = t;
                        }
                        वापस true;
                }

                अगर (r == 0)
                        अवरोध;

                अगर (r == s && strneq(p, q, r))
                        जारी; /* common component. Check next. */

                g = strndup(p, r);
                अगर (!g)
                        वापस -ENOMEM;

                अगर (!string_is_glob(g))
                        अवरोध;

                /* We found a glob component. Check if the glob pattern matches the prefix component. */

                h = strndup(q, s);
                अगर (!h)
                        वापस -ENOMEM;

                r = fnmatch(g, h, 0);
                अगर (r == FNM_NOMATCH)
                        अवरोध;
                अगर (r != 0) /* Failure to process pattern? */
                        वापस -EINVAL;
        }

        /* The pattern does not match the prefix. */
        अगर (ret)
                *ret = शून्य;
        वापस false;
}

#अगर HAVE_SPLIT_BIN
अटल bool dir_is_split(स्थिर अक्षर *a, स्थिर अक्षर *b) {
        पूर्णांक r;

        r = inode_same(a, b, AT_NO_AUTOMOUNT);
        अगर (r < 0 && r != -ENOENT) {
                log_debug_errno(r, "Failed to compare \"%s\" and \"%s\", assuming split directories: %m", a, b);
                वापस true;
        }
        वापस r == 0;
}
#पूर्ण_अगर

स्थिर अक्षर* default_PATH(व्योम) {
#अगर HAVE_SPLIT_BIN
        अटल स्थिर अक्षर *default_path = शून्य;

        /* Return one of the three sets of paths:
         * a) split /usr/s?bin, /usr/local/sbin doesn't matter.
         * b) merged /usr/s?bin, /usr/sbin is a symlink, but /usr/local/sbin is not,
         * c) fully merged, neither /usr/sbin nor /usr/local/sbin are symlinks,
         *
         * On error the fallback to the safe value with both directories as configured is returned.
         */

        अगर (default_path)
                वापस default_path;

        अगर (dir_is_split("/usr/sbin", "/usr/bin"))
                वापस (default_path = DEFAULT_PATH_WITH_FULL_SBIN);  /* a */
        अगर (dir_is_split("/usr/local/sbin", "/usr/local/bin"))
                वापस (default_path = DEFAULT_PATH_WITH_LOCAL_SBIN); /* b */
        वापस (default_path = DEFAULT_PATH_WITHOUT_SBIN);            /* c */
#अन्यथा
        वापस DEFAULT_PATH_WITHOUT_SBIN;
#पूर्ण_अगर
}
