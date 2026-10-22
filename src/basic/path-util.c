/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव <fnmatch.h>
#अन्तर्भाव <stdio.h>
#अन्तर्भाव <stdlib.h>
#अन्तर्भाव <unistd.h>

#अन्तर्भाव "alloc-util.h"
#अन्तर्भाव "chase.h"
#अन्तर्भाव "errno-util.h"
#अन्तर्भाव "extract-word.h"
#अन्तर्भाव "fd-util.h"
#अन्तर्भाव "fs-util.h"
#अन्तर्भाव "glob-util.h"
#अन्तर्भाव "log.h"
#अन्तर्भाव "path-util.h"
#अन्तर्भाव "stat-util.h"
#अन्तर्भाव "string-util.h"
#अन्तर्भाव "strv.h"

bool is_path(स्थिरांक अक्षर *p) {
        जर (!p) /* A NULL pointer is definitely not a path */
                परत false;

        परत म_अक्षर(p, '/');
}

पूर्णांक path_split_and_make_absolute(स्थिरांक अक्षर *p, अक्षर ***ret) {
        _cleanup_strv_free_ अक्षर **l = रिक्त;
        पूर्णांक r;

        प्रतिज्ञा(p);
        प्रतिज्ञा(ret);

        l = strv_split(p, ":");
        जर (!l)
                परत -ENOMEM;

        r = path_strv_make_absolute_cwd(l);
        जर (r < 0)
                परत r;

        *ret = TAKE_PTR(l);
        परत r;
}

अक्षर* path_make_absolute(स्थिरांक अक्षर *p, स्थिरांक अक्षर *prefix) {
        प्रतिज्ञा(p);

        /* Makes every item in the list an absolute path by prepending
         * the prefix, if specified and necessary */

        जर (path_is_absolute(p) || isempty(prefix))
                परत strdup(p);

        परत path_join(prefix, p);
}

पूर्णांक safe_getcwd(अक्षर **ret) {
        _cleanup_free_ अक्षर *cwd = रिक्त;

        cwd = get_current_dir_name();
        जर (!cwd)
                परत negative_errno();

        /* Let's make sure the directory is really absolute, to protect us from the logic behind
         * CVE-2018-1000001 */
        जर (cwd[0] != '/')
                परत -ENOMEDIUM;

        जर (ret)
                *ret = TAKE_PTR(cwd);

        परत 0;
}

पूर्णांक path_make_absolute_cwd(स्थिरांक अक्षर *p, अक्षर **ret) {
        अक्षर *c;
        पूर्णांक r;

        प्रतिज्ञा(p);
        प्रतिज्ञा(ret);

        /* Similar to path_make_absolute(), but prefixes with the
         * current working directory. */

        जर (path_is_absolute(p))
                c = strdup(p);
        अन्यथा {
                _cleanup_free_ अक्षर *cwd = रिक्त;

                r = safe_getcwd(&cwd);
                जर (r < 0)
                        परत r;

                c = path_join(cwd, p);
        }
        जर (!c)
                परत -ENOMEM;

        *ret = c;
        परत 0;
}

पूर्णांक path_make_relative(स्थिरांक अक्षर *from, स्थिरांक अक्षर *to, अक्षर **ret) {
        _cleanup_free_ अक्षर *result = रिक्त;
        अचिन्हित n_parents;
        स्थिरांक अक्षर *f, *t;
        पूर्णांक r, k;
        अक्षर *p;

        प्रतिज्ञा(from);
        प्रतिज्ञा(to);
        प्रतिज्ञा(ret);

        /* Strips the common part, and adds ".." elements as necessary. */

        जर (!path_is_absolute(from) || !path_is_absolute(to))
                परत -EINVAL;

        साठी (;;) {
                r = path_find_first_component(&from, true, &f);
                जर (r < 0)
                        परत r;

                k = path_find_first_component(&to, true, &t);
                जर (k < 0)
                        परत k;

                जर (r == 0) {
                        /* end of 'from' */
                        जर (k == 0) {
                                /* from and to are equivalent. */
                                result = strdup(".");
                                जर (!result)
                                        परत -ENOMEM;
                        } अन्यथा {
                                /* 'to' is inside of 'from'. */
                                r = path_simplify_alloc(t, &result);
                                जर (r < 0)
                                        परत r;

                                जर (!path_is_valid(result))
                                        परत -EINVAL;
                        }

                        *ret = TAKE_PTR(result);
                        परत 0;
                }

                जर (r != k || !strneq(f, t, r))
                        थांबा;
        }

        /* If we're here, then "from_dir" has one or more elements that need to
         * be replaced with "..". */

        साठी (n_parents = 1;; n_parents++) {
                /* If this includes ".." we can't do a simple series of "..". */
                r = path_find_first_component(&from, false, &f);
                जर (r < 0)
                        परत r;
                जर (r == 0)
                        थांबा;
        }

        जर (isempty(t) && n_parents * 3 > PATH_MAX)
                /* PATH_MAX is counted *with* the trailing NUL byte */
                परत -EINVAL;

        result = new(अक्षर, n_parents * 3 + !isempty(t) + strlen_ptr(t));
        जर (!result)
                परत -ENOMEM;

        साठी (p = result; n_parents > 0; n_parents--)
                p = mempcpy(p, "../", 3);

        जर (isempty(t)) {
                /* Remove trailing slash and terminate string. */
                *(--p) = '\0';
                *ret = TAKE_PTR(result);
                परत 0;
        }

        म_नकल(p, t);

        path_simplify(result);

        जर (!path_is_valid(result))
                परत -EINVAL;

        *ret = TAKE_PTR(result);
        परत 0;
}

पूर्णांक path_make_relative_parent(स्थिरांक अक्षर *from_child, स्थिरांक अक्षर *to, अक्षर **ret) {
        _cleanup_free_ अक्षर *from = रिक्त;
        पूर्णांक r;

        प्रतिज्ञा(from_child);
        प्रतिज्ञा(to);
        प्रतिज्ञा(ret);

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
        जर (r < 0)
                परत r;

        परत path_make_relative(from, to, ret);
}

अक्षर* path_startswith_strv(स्थिरांक अक्षर *p, अक्षर * स्थिरांक *strv) {
        प्रतिज्ञा(p);

        STRV_FOREACH(s, strv) {
                अक्षर *t;

                t = path_startswith(p, *s);
                जर (t)
                        परत t;
        }

        परत रिक्त;
}

पूर्णांक path_strv_make_absolute_cwd(अक्षर **l) {
        पूर्णांक r;

        /* Goes through every item in the string list and makes it
         * absolute. This works in place and won't rollback any
         * changes on failure. */

        STRV_FOREACH(s, l) {
                अक्षर *t;

                r = path_make_absolute_cwd(*s, &t);
                जर (r < 0)
                        परत r;

                path_simplify(t);
                free_and_replace(*s, t);
        }

        परत 0;
}

अक्षर** path_strv_resolve(अक्षर **l, स्थिरांक अक्षर *मूळ) {
        अचिन्हित k = 0;
        bool enomem = false;
        पूर्णांक r;

        जर (strv_isempty(l))
                परत l;

        /* Go through every item in the string list and canonicalize the path.
         * This works in place and doesn't roll back any changes on failure. */

        STRV_FOREACH(s, l) {
                _cleanup_free_ अक्षर *orig = रिक्त;
                अक्षर *t, *u;

                जर (मूळ) {
                        जर (!path_is_absolute(*s)) {
                                log_debug("Ignoring non-absolute search path '%s'.", *s);
                                मुक्त(*s);
                                सुरूठेवा;
                        }

                        orig = *s;
                        t = path_join(मूळ, orig);
                        जर (!t) {
                                enomem = true;
                                सुरूठेवा;
                        }
                } अन्यथा
                        t = *s;

                r = chase(t, मूळ, 0, &u, रिक्त);
                जर (r == -ENOENT) {
                        जर (मूळ) {
                                u = TAKE_PTR(orig);
                                मुक्त(t);
                        } अन्यथा
                                u = t;
                } अन्यथा जर (r < 0) {
                        मुक्त(t);

                        जर (r == -ENOMEM)
                                enomem = true;

                        सुरूठेवा;
                } अन्यथा जर (मूळ) {
                        अक्षर *x;

                        मुक्त(t);
                        x = path_startswith(u, मूळ);
                        जर (x) {
                                /* restore the slash if it was lost */
                                जर (!startswith(x, "/"))
                                        *(--x) = '/';

                                t = strdup(x);
                                मुक्त(u);
                                जर (!t) {
                                        enomem = true;
                                        सुरूठेवा;
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

        l[k] = रिक्त;

        जर (enomem)
                परत रिक्त;

        परत l;
}

अक्षर** path_strv_resolve_uniq(अक्षर **l, स्थिरांक अक्षर *मूळ) {

        जर (strv_isempty(l))
                परत l;

        जर (!path_strv_resolve(l, मूळ))
                परत रिक्त;

        परत strv_uniq(l);
}

अक्षर* skip_leading_slash(स्थिरांक अक्षर *p) {
        परत skip_leading_chars(p, "/");
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

        जर (isempty(path))
                परत path;

        keep_trailing_slash = FLAGS_SET(flags, PATH_SIMPLIFY_KEEP_TRAILING_SLASH) && endswith(path, "/");

        absolute = path_is_absolute(path);
        f += absolute;  /* Keep leading /, if present. */

        साठी (स्थिरांक अक्षर *p = f;;) {
                स्थिरांक अक्षर *e;

                r = path_find_first_component(&p, true, &e);
                जर (r == 0)
                        थांबा;

                जर (r > 0 && absolute && beginning && path_startswith(e, ".."))
                        /* If we're at the beginning of an absolute path, we can safely skip ".." */
                        सुरूठेवा;

                beginning = false;

                जर (add_slash)
                        *f++ = '/';

                जर (r < 0) {
                        /* if path is invalid, then refuse to simplify the remaining part. */
                        स_हटाओ(f, p, म_माप(p) + 1);
                        परत path;
                }

                स_हटाओ(f, e, r);
                f += r;

                add_slash = true;
        }

        /* Special rule, if we stripped everything, we need a "." for the current directory. */
        जर (f == path)
                *f++ = '.';

        जर (*(f-1) != '/' && keep_trailing_slash)
                *f++ = '/';

        *f = '\0';
        परत path;
}

पूर्णांक path_simplify_alloc(स्थिरांक अक्षर *path, अक्षर **ret) {
        प्रतिज्ञा(ret);

        जर (!path) {
                *ret = रिक्त;
                परत 0;
        }

        अक्षर *t = strdup(path);
        जर (!t)
                परत -ENOMEM;

        *ret = path_simplify(t);
        परत 0;
}

अक्षर* path_startswith_full(स्थिरांक अक्षर *original_path, स्थिरांक अक्षर *prefix, PathStartWithFlags flags) {
        प्रतिज्ञा(original_path);
        प्रतिज्ञा(prefix);

        /* Returns a pointer to the start of the first component after the parts matched by
         * the prefix, iff
         * - both paths are absolute or both paths are relative,
         * and
         * - each component in prefix in turn matches a component in path at the same position.
         * An empty string will be returned when the prefix and path are equivalent.
         *
         * Returns NULL otherwise.
         */

        स्थिरांक अक्षर *path = original_path;

        जर ((path[0] == '/') != (prefix[0] == '/'))
                परत रिक्त;

        साठी (;;) {
                स्थिरांक अक्षर *p, *q;
                पूर्णांक m, n;

                m = path_find_first_component(&path, !FLAGS_SET(flags, PATH_STARTSWITH_REFUSE_DOT_DOT), &p);
                जर (m < 0)
                        परत रिक्त;

                n = path_find_first_component(&prefix, !FLAGS_SET(flags, PATH_STARTSWITH_REFUSE_DOT_DOT), &q);
                जर (n < 0)
                        परत रिक्त;

                जर (n == 0) {
                        जर (!p)
                                p = path;

                        जर (FLAGS_SET(flags, PATH_STARTSWITH_RETURN_LEADING_SLASH)) {

                                जर (p <= original_path)
                                        परत रिक्त;

                                p--;

                                जर (*p != '/')
                                        परत रिक्त;
                        }

                        परत (अक्षर*) p;
                }

                जर (m != n)
                        परत रिक्त;

                जर (!strneq(p, q, m))
                        परत रिक्त;
        }
}

पूर्णांक path_compare(स्थिरांक अक्षर *a, स्थिरांक अक्षर *b) {
        पूर्णांक r;

        /* Order NULL before non-NULL */
        r = CMP(!!a, !!b);
        जर (r != 0)
                परत r;

        /* A relative path and an absolute path must not compare as equal.
         * Which one is sorted before the other does not really matter.
         * Here a relative path is ordered before an absolute path. */
        r = CMP(path_is_absolute(a), path_is_absolute(b));
        जर (r != 0)
                परत r;

        साठी (;;) {
                स्थिरांक अक्षर *aa, *bb;
                पूर्णांक j, k;

                j = path_find_first_component(&a, true, &aa);
                k = path_find_first_component(&b, true, &bb);

                जर (j < 0 || k < 0) {
                        /* When one of paths is invalid, order invalid path after valid one. */
                        r = CMP(j < 0, k < 0);
                        जर (r != 0)
                                परत r;

                        /* fallback to use strcmp() if both paths are invalid. */
                        परत म_भेद(a, b);
                }

                /* Order prefixes first: "/foo" before "/foo/bar" */
                जर (j == 0) {
                        जर (k == 0)
                                परत 0;
                        परत -1;
                }
                जर (k == 0)
                        परत 1;

                /* Alphabetical sort: "/foo/aaa" before "/foo/b" */
                r = स_भेद(aa, bb, MIN(j, k));
                जर (r != 0)
                        परत r;

                /* Sort "/foo/a" before "/foo/aaa" */
                r = CMP(j, k);
                जर (r != 0)
                        परत r;
        }
}

पूर्णांक path_compare_filename(स्थिरांक अक्षर *a, स्थिरांक अक्षर *b) {
        _cleanup_free_ अक्षर *fa = रिक्त, *fb = रिक्त;
        पूर्णांक r, j, k;

        /* Order NULL before non-NULL */
        r = CMP(!!a, !!b);
        जर (r != 0)
                परत r;

        j = path_extract_filename(a, &fa);
        k = path_extract_filename(b, &fb);

        /* When one of paths is "." or root, then order it earlier. */
        r = CMP(j != -EADDRNOTAVAIL, k != -EADDRNOTAVAIL);
        जर (r != 0)
                परत r;

        /* When one of paths is invalid (or we get OOM), order invalid path after valid one. */
        r = CMP(j < 0, k < 0);
        जर (r != 0)
                परत r;

        /* fallback to use strcmp() if both paths are invalid. */
        जर (j < 0)
                परत म_भेद(a, b);

        परत म_भेद(fa, fb);
}

पूर्णांक path_equal_or_inode_same_full(स्थिरांक अक्षर *a, स्थिरांक अक्षर *b, पूर्णांक flags) {
        /* Returns true if paths are of the same entry, false if not, <0 on error. */

        जर (path_equal(a, b))
                परत 1;

        जर (!a || !b)
                परत 0;

        परत inode_same(a, b, flags);
}

अक्षर* path_extend_internal(अक्षर **x, ...) {
        माप_प्रकार sz, old_sz;
        अक्षर *q, *nx;
        स्थिरांक अक्षर *p;
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
        पर्यंत ((p = बहु_तर्क(ap, अक्षर*)) != POINTER_MAX) {
                माप_प्रकार add;

                जर (isempty(p))
                        सुरूठेवा;

                add = 1 + म_माप(p);
                जर (sz > SIZE_MAX - add) { /* overflow check */
                        बहु_पूर्ण(ap);
                        परत रिक्त;
                }

                sz += add;
        }
        बहु_पूर्ण(ap);

        nx = पुनः_स्मृति(x ? *x : रिक्त, GREEDY_ALLOC_ROUND_UP(sz+1));
        जर (!nx)
                परत रिक्त;
        जर (x)
                *x = nx;

        जर (old_sz > 0)
                slash = nx[old_sz-1] == '/';
        अन्यथा {
                nx[old_sz] = 0;
                slash = true; /* no need to generate a slash anymore */
        }

        q = nx + old_sz;

        बहु_शुरू(ap, x);
        पर्यंत ((p = बहु_तर्क(ap, अक्षर*)) != POINTER_MAX) {
                जर (isempty(p))
                        सुरूठेवा;

                जर (!slash && p[0] != '/')
                        *(q++) = '/';

                q = stpcpy(q, p);
                slash = endswith(p, "/");
        }
        बहु_पूर्ण(ap);

        परत nx;
}

पूर्णांक open_and_check_executable(स्थिरांक अक्षर *name, स्थिरांक अक्षर *मूळ, अक्षर **ret_path, पूर्णांक *ret_fd) {
        _cleanup_close_ पूर्णांक fd = -EBADF;
        _cleanup_free_ अक्षर *resolved = रिक्त;
        पूर्णांक r;

        प्रतिज्ञा(name);

        /* Function chase() is invoked only when root is not NULL, as using it regardless of
         * root value would alter the behavior of existing callers for example: /bin/sleep would become
         * /usr/bin/sleep when find_executables is called. Hence, this function should be invoked when
         * needed to avoid unforeseen regression or other complicated changes. */
        जर (मूळ) {
                /* prefix root to name in case full paths are not specified */
                r = chase(name, मूळ, CHASE_PREFIX_मूळ, &resolved, &fd);
                जर (r < 0)
                        परत r;

                name = resolved;
        } अन्यथा {
                /* We need to use O_PATH because there may be executables for which we have only exec permissions,
                 * but not read (usually suid executables). */
                fd = खोलो(name, O_PATH|O_CLOEXEC);
                जर (fd < 0)
                        परत -त्रुटि_सं;
        }

        r = fd_verify_regular(fd);
        जर (r < 0)
                परत r;

        r = access_fd(fd, X_OK);
        जर (r == -ENOSYS)
                /* /proc/ is not mounted. Fall back to access(). */
                r = RET_NERRNO(access(name, X_OK));
        जर (r < 0)
                परत r;

        जर (ret_path) {
                जर (resolved)
                        *ret_path = TAKE_PTR(resolved);
                अन्यथा {
                        r = path_make_absolute_cwd(name, ret_path);
                        जर (r < 0)
                                परत r;

                        path_simplify(*ret_path);
                }
        }

        जर (ret_fd)
                *ret_fd = TAKE_FD(fd);

        परत 0;
}

पूर्णांक find_executable_full(
                स्थिरांक अक्षर *name,
                स्थिरांक अक्षर *मूळ,
                अक्षर * स्थिरांक *exec_शोध_path,
                bool use_path_envvar,
                अक्षर **ret_filename,
                पूर्णांक *ret_fd) {

        पूर्णांक last_error = -ENOENT, r = 0;

        प्रतिज्ञा(name);

        जर (is_path(name))
                परत open_and_check_executable(name, मूळ, ret_filename, ret_fd);

        जर (exec_शोध_path) {
                STRV_FOREACH(element, exec_शोध_path) {
                        _cleanup_free_ अक्षर *full_path = रिक्त;

                        जर (!path_is_absolute(*element)) {
                                log_debug("Exec search path '%s' isn't absolute, ignoring.", *element);
                                सुरूठेवा;
                        }

                        full_path = path_join(*element, name);
                        जर (!full_path)
                                परत -ENOMEM;

                        r = open_and_check_executable(full_path, मूळ, ret_filename, ret_fd);
                        जर (r >= 0)
                                परत 0;
                        जर (r != -EACCES)
                                last_error = r;
                }
                परत last_error;
        }

        स्थिरांक अक्षर *p = रिक्त;

        जर (use_path_envvar)
                /* Plain getenv, not secure_getenv, because we want to actually allow the user to pick the
                 * binary. */
                p = दो_पर्या("PATH");
        जर (!p)
                p = default_PATH();

        /* Resolve a single-component name to a full path */
        साठी (;;) {
                _cleanup_free_ अक्षर *element = रिक्त;

                r = extract_first_word(&p, &element, ":", EXTRACT_RELAX|EXTRACT_DONT_COALESCE_SEPARATORS);
                जर (r < 0)
                        परत r;
                जर (r == 0)
                        थांबा;

                जर (!path_is_absolute(element)) {
                        log_debug("Exec search path '%s' isn't absolute, ignoring.", element);
                        सुरूठेवा;
                }

                जर (!path_extend(&element, name))
                        परत -ENOMEM;

                r = open_and_check_executable(element, मूळ, ret_filename, ret_fd);
                जर (r >= 0) /* Found it! */
                        परत 0;
                /* PATH entries which we don't have access to are ignored, as per tradition. */
                जर (r != -EACCES)
                        last_error = r;
        }

        परत last_error;
}

स्थिर पूर्णांक executable_is_good(स्थिरांक अक्षर *executable) {
        _cleanup_free_ अक्षर *p = रिक्त, *d = रिक्त;
        पूर्णांक r;

        r = find_executable(executable, &p);
        जर (r == -ENOENT)
                परत 0;
        जर (r < 0)
                परत r;

        /* An fsck that is linked to /bin/true is a non-existent fsck */

        r = readlink_malloc(p, &d);
        जर (r == -EINVAL) /* not a symlink */
                परत 1;
        जर (r < 0)
                परत r;

        परत !PATH_IN_SET(d, "true",
                               "/bin/true",
                               "/usr/bin/true",
                               "/dev/null");
}

पूर्णांक fsck_exists(शून्य) {
        परत executable_is_good("fsck");
}

पूर्णांक fsck_exists_for_fstype(स्थिरांक अक्षर *fstype) {
        स्थिरांक अक्षर *checker;
        पूर्णांक r;

        प्रतिज्ञा(fstype);

        जर (streq(fstype, "auto"))
                परत -EINVAL;

        r = fsck_exists();
        जर (r <= 0)
                परत r;

        checker = strjoina("fsck.", fstype);
        परत executable_is_good(checker);
}

स्थिर स्थिरांक अक्षर* skip_slash_or_dot(स्थिरांक अक्षर *p) {
        POINTER_MAY_BE_NULL(p);

        साठी (; !isempty(p); p++) {
                जर (*p == '/')
                        सुरूठेवा;
                जर (startswith(p, "./")) {
                        p++;
                        सुरूठेवा;
                }
                थांबा;
        }
        परत p;
}

पूर्णांक path_find_first_component(स्थिरांक अक्षर **p, bool accept_dot_dot, स्थिरांक अक्षर **ret) {
        स्थिरांक अक्षर *q, *first, *end_first, *next;
        माप_प्रकार len;

        प्रतिज्ञा(p);

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
        जर (isempty(first)) {
                *p = first;
                जर (ret)
                        *ret = रिक्त;
                परत 0;
        }
        जर (streq(first, ".")) {
                *p = first + 1;
                जर (ret)
                        *ret = रिक्त;
                परत 0;
        }

        end_first = strchrnul(first, '/');
        len = end_first - first;

        जर (len > NAME_MAX)
                परत -EINVAL;
        जर (!accept_dot_dot && len == 2 && first[0] == '.' && first[1] == '.')
                परत -EINVAL;

        next = skip_slash_or_dot(end_first);

        *p = next + streq(next, ".");
        जर (ret)
                *ret = first;
        परत len;
}

स्थिर स्थिरांक अक्षर* skip_slash_or_dot_backward(स्थिरांक अक्षर *path, स्थिरांक अक्षर *q) {
        प्रतिज्ञा(path);
        प्रतिज्ञा(!q || q >= path);

        साठी (; q; q = PTR_SUB1(q, path)) {
                जर (*q == '/')
                        सुरूठेवा;
                जर (q > path && strneq(q - 1, "/.", 2))
                        सुरूठेवा;
                जर (q == path && *q == '.')
                        सुरूठेवा;
                थांबा;
        }
        परत q;
}

पूर्णांक path_find_last_component(स्थिरांक अक्षर *path, bool accept_dot_dot, स्थिरांक अक्षर **next, स्थिरांक अक्षर **ret) {
        स्थिरांक अक्षर *q, *last_end, *last_begin;
        माप_प्रकार len;

        POINTER_MAY_BE_NULL(next);
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

        जर (isempty(path)) {
                जर (next)
                        *next = path;
                जर (ret)
                        *ret = रिक्त;
                परत 0;
        }

        जर (next && *next) {
                जर (*next < path || *next > path + म_माप(path))
                        परत -EINVAL;
                जर (*next == path) {
                        जर (ret)
                                *ret = रिक्त;
                        परत 0;
                }
                जर (!IN_SET(**next, '\0', '/'))
                        परत -EINVAL;
                q = *next - 1;
        } अन्यथा
                q = path + म_माप(path) - 1;

        q = skip_slash_or_dot_backward(path, q);
        जर (!q) { /* the root directory, "." or "./" */
                जर (next)
                        *next = path;
                जर (ret)
                        *ret = रिक्त;
                परत 0;
        }

        last_end = q + 1;

        पर्यंत (q && *q != '/')
                q = PTR_SUB1(q, path);

        last_begin = q ? q + 1 : path;
        len = last_end - last_begin;

        जर (len > NAME_MAX)
                परत -EINVAL;
        जर (!accept_dot_dot && len == 2 && strneq(last_begin, "..", 2))
                परत -EINVAL;

        जर (next) {
                q = skip_slash_or_dot_backward(path, q);
                *next = q ? q + 1 : path;
        }

        जर (ret)
                *ret = last_begin;
        परत len;
}

स्थिरांक अक्षर* last_path_component(स्थिरांक अक्षर *path) {

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

        जर (!path)
                परत रिक्त;

        l = k = म_माप(path);
        जर (l == 0) /* special case — an empty string */
                परत path;

        पर्यंत (k > 0 && path[k-1] == '/')
                k--;

        जर (k == 0) /* the root directory */
                परत path + l - 1;

        पर्यंत (k > 0 && path[k-1] != '/')
                k--;

        परत path + k;
}

पूर्णांक path_split_prefix_filename(स्थिरांक अक्षर *path, अक्षर **ret_dir, अक्षर **ret_filename) {
        _cleanup_free_ अक्षर *d = रिक्त;
        स्थिरांक अक्षर *c, *next = रिक्त;
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

        जर (isempty(path))
                परत -EINVAL;

        r = path_find_last_component(path, /* accept_dot_dot = */ false, &next, &c);
        जर (r < 0)
                परत r;
        जर (r == 0) /* root directory or "." */
                परत -EADDRNOTAVAIL;

        जर (ret_dir) {
                जर (next == path) {
                        जर (*path != '/') { /* filename only */
                                जर (!ret_filename)
                                        परत -EDESTADDRREQ;
                        } अन्यथा {
                                d = strdup("/");
                                जर (!d)
                                        परत -ENOMEM;
                        }
                } अन्यथा {
                        d = strndup(path, next - path);
                        जर (!d)
                                परत -ENOMEM;

                        path_simplify(d);

                        जर (!path_is_valid(d))
                                परत -EINVAL;
                }

        } अन्यथा जर (!path_is_valid(path))
                /* We didn't validate the dir prefix, hence check if the whole path is valid now */
                परत -EINVAL;

        जर (ret_filename) {
                अक्षर *fn = strndup(c, r);
                जर (!fn)
                        परत -ENOMEM;

                *ret_filename = fn;
        }

        जर (ret_dir)
                *ret_dir = TAKE_PTR(d);

        परत म_माप(c) > (माप_प्रकार) r ? O_DIRECTORY : 0;
}

bool filename_part_is_valid(स्थिरांक अक्षर *p) {
        स्थिरांक अक्षर *e;

        /* Checks f the specified string is OK to be *part* of a filename. This is different from
         * filename_is_valid() as "." and ".." and "" are OK by this call, but not by filename_is_valid(). */

        जर (!p)
                परत false;

        e = strchrnul(p, '/');
        जर (*e != 0)
                परत false;

        जर (e - p > NAME_MAX) /* NAME_MAX is counted *without* the trailing NUL byte */
                परत false;

        परत true;
}

bool filename_is_valid(स्थिरांक अक्षर *p) {

        जर (isempty(p))
                परत false;

        जर (dot_or_dot_dot(p)) /* Yes, in this context we consider "." and ".." invalid */
                परत false;

        परत filename_part_is_valid(p);
}

bool path_is_valid_full(स्थिरांक अक्षर *p, bool accept_dot_dot) {
        जर (isempty(p))
                परत false;

        साठी (स्थिरांक अक्षर *e = p;;) {
                पूर्णांक r;

                r = path_find_first_component(&e, accept_dot_dot, रिक्त);
                जर (r < 0)
                        परत false;

                जर (e - p >= PATH_MAX) /* Already reached the maximum length for a path? (PATH_MAX is counted
                                        * *with* the trailing NUL byte) */
                        परत false;
                जर (*e == 0)           /* End of string? Yay! */
                        परत true;
        }
}

bool path_is_normalized(स्थिरांक अक्षर *p) {
        जर (!path_is_safe(p))
                परत false;

        जर (streq(p, ".") || startswith(p, "./") || endswith(p, "/.") || म_माला(p, "/./"))
                परत false;

        जर (म_माला(p, "//"))
                परत false;

        परत true;
}

पूर्णांक file_in_same_dir(स्थिरांक अक्षर *path, स्थिरांक अक्षर *filename, अक्षर **ret) {
        _cleanup_free_ अक्षर *b = रिक्त;
        पूर्णांक r;

        प्रतिज्ञा(path);
        प्रतिज्ञा(filename);
        प्रतिज्ञा(ret);

        /* This removes the last component of path and appends filename, unless the latter is absolute anyway
         * or the former isn't */

        जर (path_is_absolute(filename))
                b = strdup(filename);
        अन्यथा {
                _cleanup_free_ अक्षर *dn = रिक्त;

                r = path_extract_directory(path, &dn);
                जर (r == -EDESTADDRREQ) /* no path prefix */
                        b = strdup(filename);
                अन्यथा जर (r < 0)
                        परत r;
                अन्यथा
                        b = path_join(dn, filename);
        }
        जर (!b)
                परत -ENOMEM;

        *ret = TAKE_PTR(b);
        परत 0;
}

bool hidden_or_backup_file(स्थिरांक अक्षर *filename) {
        प्रतिज्ञा(filename);

        जर (filename[0] == '.' ||
            STR_IN_SET(filename,
                       "lost+found",
                       "aquota.user",
                       "aquota.group") ||
            endswith(filename, "~"))
                परत true;

        स्थिरांक अक्षर *dot = म_खोजप(filename, '.');
        जर (!dot)
                परत false;

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

        परत STR_IN_SET(dot + 1,
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

bool is_device_path(स्थिरांक अक्षर *path) {

        /* Returns true for paths that likely refer to a device, either by path in sysfs or to something in
         * /dev. This accepts any path that starts with /dev/ or /sys/ and has something after that prefix.
         * It does not actually resolve the path.
         *
         * Examples:
         * /dev/sda, /dev/sda/foo, /sys/class, /dev/.., /sys/.., /./dev/foo → yes.
         * /../dev/sda, /dev, /sys, /usr/path, /usr/../dev/sda → no.
         */

        स्थिरांक अक्षर *p = PATH_STARTSWITH_SET(ASSERT_PTR(path), "/dev/", "/sys/");
        परत !isempty(p);
}

bool valid_device_गाठ_path(स्थिरांक अक्षर *path) {

        /* Some superficial checks whether the specified path is a valid device node path, all without
         * looking at the actual device node. */

        जर (!PATH_STARTSWITH_SET(path, "/dev/", "/run/systemd/inaccessible/"))
                परत false;

        जर (endswith(path, "/")) /* can't be a device node if it ends in a slash */
                परत false;

        परत path_is_normalized(path);
}

bool valid_device_allow_pattern(स्थिरांक अक्षर *path) {
        प्रतिज्ञा(path);

        /* Like valid_device_node_path(), but also allows full-subsystem expressions like those accepted by
         * DeviceAllow= and DeviceDeny=. */

        जर (STARTSWITH_SET(path, "block-", "char-"))
                परत true;

        परत valid_device_गाठ_path(path);
}

bool dot_or_dot_dot(स्थिरांक अक्षर *path) {
        जर (!path)
                परत false;
        जर (path[0] != '.')
                परत false;
        जर (path[1] == 0)
                परत true;
        जर (path[1] != '.')
                परत false;

        परत path[2] == 0;
}

bool path_implies_directory(स्थिरांक अक्षर *path) {

        /* Sometimes, if we look at a path we already know it must refer to a directory, because it is
         * suffixed with a slash, or its last component is "." or ".." */

        जर (!path)
                परत false;

        जर (dot_or_dot_dot(path))
                परत true;

        परत ENDSWITH_SET(path, "/", "/.", "/..");
}

bool empty_or_मूळ(स्थिरांक अक्षर *path) {

        /* For operations relative to some root directory, returns true if the specified root directory is
         * redundant, i.e. either / or NULL or the empty string or any equivalent. */

        जर (isempty(path))
                परत true;

        परत path_equal(path, "/");
}

स्थिरांक अक्षर* empty_to_मूळ(स्थिरांक अक्षर *path) {
        परत isempty(path) ? "/" : path;
}

पूर्णांक empty_or_मूळ_harder_to_null(स्थिरांक अक्षर **path) {
        पूर्णांक r;

        प्रतिज्ञा(path);

        /* This nullifies the input path when the path is empty or points to "/". */

        जर (empty_or_मूळ(*path)) {
                *path = रिक्त;
                परत 0;
        }

        r = path_is_मूळ(*path);
        जर (r < 0)
                परत r;
        जर (r > 0)
                *path = रिक्त;

        परत 0;
}

bool path_strv_contains(अक्षर * स्थिरांक *l, स्थिरांक अक्षर *path) {
        प्रतिज्ञा(path);

        STRV_FOREACH(i, l)
                जर (path_equal(*i, path))
                        परत true;

        परत false;
}

bool prefixed_path_strv_contains(अक्षर * स्थिरांक *l, स्थिरांक अक्षर *path) {
        प्रतिज्ञा(path);

        STRV_FOREACH(i, l) {
                स्थिरांक अक्षर *j = *i;

                जर (*j == '-')
                        j++;
                जर (*j == '+')
                        j++;

                जर (path_equal(j, path))
                        परत true;
        }

        परत false;
}

पूर्णांक path_glob_can_match(स्थिरांक अक्षर *pattern, स्थिरांक अक्षर *prefix, अक्षर **ret) {
        प्रतिज्ञा(pattern);
        प्रतिज्ञा(prefix);

        साठी (स्थिरांक अक्षर *a = pattern, *b = prefix;;) {
                _cleanup_free_ अक्षर *g = रिक्त, *h = रिक्त;
                स्थिरांक अक्षर *p, *q;
                पूर्णांक r, s;

                r = path_find_first_component(&a, /* accept_dot_dot= */ false, &p);
                जर (r < 0)
                        परत r;

                s = path_find_first_component(&b, /* accept_dot_dot= */ false, &q);
                जर (s < 0)
                        परत s;

                जर (s == 0) {
                        /* The pattern matches the prefix. */
                        जर (ret) {
                                अक्षर *t;

                                t = path_join(prefix, p);
                                जर (!t)
                                        परत -ENOMEM;

                                *ret = t;
                        }
                        परत true;
                }

                जर (r == 0)
                        थांबा;

                जर (r == s && strneq(p, q, r))
                        सुरूठेवा; /* common component. Check next. */

                g = strndup(p, r);
                जर (!g)
                        परत -ENOMEM;

                जर (!string_is_glob(g))
                        थांबा;

                /* We found a glob component. Check if the glob pattern matches the prefix component. */

                h = strndup(q, s);
                जर (!h)
                        परत -ENOMEM;

                r = fnmatch(g, h, 0);
                जर (r == FNM_NOMATCH)
                        थांबा;
                जर (r != 0) /* Failure to process pattern? */
                        परत -EINVAL;
        }

        /* The pattern does not match the prefix. */
        जर (ret)
                *ret = रिक्त;
        परत false;
}

#जर HAVE_SPLIT_BIN
स्थिर bool dir_is_split(स्थिरांक अक्षर *a, स्थिरांक अक्षर *b) {
        पूर्णांक r;

        r = inode_same(a, b, AT_NO_AUTOMOUNT);
        जर (r < 0 && r != -ENOENT) {
                log_debug_errno(r, "Failed to compare \"%s\" and \"%s\", assuming split directories: %m", a, b);
                परत true;
        }
        परत r == 0;
}
#endif

स्थिरांक अक्षर* default_PATH(शून्य) {
#जर HAVE_SPLIT_BIN
        स्थिर स्थिरांक अक्षर *default_path = रिक्त;

        /* Return one of the three sets of paths:
         * a) split /usr/s?bin, /usr/local/sbin doesn't matter.
         * b) merged /usr/s?bin, /usr/sbin is a symlink, but /usr/local/sbin is not,
         * c) fully merged, neither /usr/sbin nor /usr/local/sbin are symlinks,
         *
         * On error the fallback to the safe value with both directories as configured is returned.
         */

        जर (default_path)
                परत default_path;

        जर (dir_is_split("/usr/sbin", "/usr/bin"))
                परत (default_path = DEFAULT_PATH_WITH_FULL_SBIN);  /* a */
        जर (dir_is_split("/usr/local/sbin", "/usr/local/bin"))
                परत (default_path = DEFAULT_PATH_WITH_LOCAL_SBIN); /* b */
        परत (default_path = DEFAULT_PATH_WITHOUT_SBIN);            /* c */
#अन्यथा
        परत DEFAULT_PATH_WITHOUT_SBIN;
#endif
}
