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

bool is_path(अचर वर्ण *p) {
        यदि (!p) /* A NULL pointer is definitely not a path */
                फर्काऊ false;

        फर्काऊ म_अक्षर(p, '/');
}

पूर्णाङ्क path_split_and_make_absolute(अचर वर्ण *p, वर्ण ***ret) {
        _cleanup_strv_free_ वर्ण **l = रिक्त;
        पूर्णाङ्क r;

        प्रतिज्ञा(p);
        प्रतिज्ञा(ret);

        l = strv_split(p, ":");
        यदि (!l)
                फर्काऊ -ENOMEM;

        r = path_strv_make_absolute_cwd(l);
        यदि (r < 0)
                फर्काऊ r;

        *ret = TAKE_PTR(l);
        फर्काऊ r;
}

वर्ण* path_make_absolute(अचर वर्ण *p, अचर वर्ण *prefix) {
        प्रतिज्ञा(p);

        /* Makes every item in the list an absolute path by prepending
         * the prefix, if specified and necessary */

        यदि (path_is_absolute(p) || isempty(prefix))
                फर्काऊ strdup(p);

        फर्काऊ path_join(prefix, p);
}

पूर्णाङ्क safe_getcwd(वर्ण **ret) {
        _cleanup_free_ वर्ण *cwd = रिक्त;

        cwd = get_current_dir_name();
        यदि (!cwd)
                फर्काऊ negative_errno();

        /* Let's make sure the directory is really absolute, to protect us from the logic behind
         * CVE-2018-1000001 */
        यदि (cwd[0] != '/')
                फर्काऊ -ENOMEDIUM;

        यदि (ret)
                *ret = TAKE_PTR(cwd);

        फर्काऊ 0;
}

पूर्णाङ्क path_make_absolute_cwd(अचर वर्ण *p, वर्ण **ret) {
        वर्ण *c;
        पूर्णाङ्क r;

        प्रतिज्ञा(p);
        प्रतिज्ञा(ret);

        /* Similar to path_make_absolute(), but prefixes with the
         * current working directory. */

        यदि (path_is_absolute(p))
                c = strdup(p);
        अन्यथा {
                _cleanup_free_ वर्ण *cwd = रिक्त;

                r = safe_getcwd(&cwd);
                यदि (r < 0)
                        फर्काऊ r;

                c = path_join(cwd, p);
        }
        यदि (!c)
                फर्काऊ -ENOMEM;

        *ret = c;
        फर्काऊ 0;
}

पूर्णाङ्क path_make_relative(अचर वर्ण *from, अचर वर्ण *to, वर्ण **ret) {
        _cleanup_free_ वर्ण *result = रिक्त;
        अचिन्हित n_parents;
        अचर वर्ण *f, *t;
        पूर्णाङ्क r, k;
        वर्ण *p;

        प्रतिज्ञा(from);
        प्रतिज्ञा(to);
        प्रतिज्ञा(ret);

        /* Strips the common part, and adds ".." elements as necessary. */

        यदि (!path_is_absolute(from) || !path_is_absolute(to))
                फर्काऊ -EINVAL;

        लागि (;;) {
                r = path_find_first_component(&from, true, &f);
                यदि (r < 0)
                        फर्काऊ r;

                k = path_find_first_component(&to, true, &t);
                यदि (k < 0)
                        फर्काऊ k;

                यदि (r == 0) {
                        /* end of 'from' */
                        यदि (k == 0) {
                                /* from and to are equivalent. */
                                result = strdup(".");
                                यदि (!result)
                                        फर्काऊ -ENOMEM;
                        } अन्यथा {
                                /* 'to' is inside of 'from'. */
                                r = path_simplify_alloc(t, &result);
                                यदि (r < 0)
                                        फर्काऊ r;

                                यदि (!path_is_valid(result))
                                        फर्काऊ -EINVAL;
                        }

                        *ret = TAKE_PTR(result);
                        फर्काऊ 0;
                }

                यदि (r != k || !strneq(f, t, r))
                        रोक;
        }

        /* If we're here, then "from_dir" has one or more elements that need to
         * be replaced with "..". */

        लागि (n_parents = 1;; n_parents++) {
                /* If this includes ".." we can't do a simple series of "..". */
                r = path_find_first_component(&from, false, &f);
                यदि (r < 0)
                        फर्काऊ r;
                यदि (r == 0)
                        रोक;
        }

        यदि (isempty(t) && n_parents * 3 > PATH_MAX)
                /* PATH_MAX is counted *with* the trailing NUL byte */
                फर्काऊ -EINVAL;

        result = new(वर्ण, n_parents * 3 + !isempty(t) + strlen_ptr(t));
        यदि (!result)
                फर्काऊ -ENOMEM;

        लागि (p = result; n_parents > 0; n_parents--)
                p = mempcpy(p, "../", 3);

        यदि (isempty(t)) {
                /* Remove trailing slash and terminate string. */
                *(--p) = '\0';
                *ret = TAKE_PTR(result);
                फर्काऊ 0;
        }

        म_नकल(p, t);

        path_simplify(result);

        यदि (!path_is_valid(result))
                फर्काऊ -EINVAL;

        *ret = TAKE_PTR(result);
        फर्काऊ 0;
}

पूर्णाङ्क path_make_relative_parent(अचर वर्ण *from_child, अचर वर्ण *to, वर्ण **ret) {
        _cleanup_free_ वर्ण *from = रिक्त;
        पूर्णाङ्क r;

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
        यदि (r < 0)
                फर्काऊ r;

        फर्काऊ path_make_relative(from, to, ret);
}

वर्ण* path_startswith_strv(अचर वर्ण *p, वर्ण * अचर *strv) {
        प्रतिज्ञा(p);

        STRV_FOREACH(s, strv) {
                वर्ण *t;

                t = path_startswith(p, *s);
                यदि (t)
                        फर्काऊ t;
        }

        फर्काऊ रिक्त;
}

पूर्णाङ्क path_strv_make_absolute_cwd(वर्ण **l) {
        पूर्णाङ्क r;

        /* Goes through every item in the string list and makes it
         * absolute. This works in place and won't rollback any
         * changes on failure. */

        STRV_FOREACH(s, l) {
                वर्ण *t;

                r = path_make_absolute_cwd(*s, &t);
                यदि (r < 0)
                        फर्काऊ r;

                path_simplify(t);
                free_and_replace(*s, t);
        }

        फर्काऊ 0;
}

वर्ण** path_strv_resolve(वर्ण **l, अचर वर्ण *जरा) {
        अचिन्हित k = 0;
        bool enomem = false;
        पूर्णाङ्क r;

        यदि (strv_isempty(l))
                फर्काऊ l;

        /* Go through every item in the string list and canonicalize the path.
         * This works in place and doesn't roll back any changes on failure. */

        STRV_FOREACH(s, l) {
                _cleanup_free_ वर्ण *orig = रिक्त;
                वर्ण *t, *u;

                यदि (जरा) {
                        यदि (!path_is_absolute(*s)) {
                                log_debug("Ignoring non-absolute search path '%s'.", *s);
                                मुक्त(*s);
                                जारी;
                        }

                        orig = *s;
                        t = path_join(जरा, orig);
                        यदि (!t) {
                                enomem = true;
                                जारी;
                        }
                } अन्यथा
                        t = *s;

                r = chase(t, जरा, 0, &u, रिक्त);
                यदि (r == -ENOENT) {
                        यदि (जरा) {
                                u = TAKE_PTR(orig);
                                मुक्त(t);
                        } अन्यथा
                                u = t;
                } अन्यथा यदि (r < 0) {
                        मुक्त(t);

                        यदि (r == -ENOMEM)
                                enomem = true;

                        जारी;
                } अन्यथा यदि (जरा) {
                        वर्ण *x;

                        मुक्त(t);
                        x = path_startswith(u, जरा);
                        यदि (x) {
                                /* restore the slash if it was lost */
                                यदि (!startswith(x, "/"))
                                        *(--x) = '/';

                                t = strdup(x);
                                मुक्त(u);
                                यदि (!t) {
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

        l[k] = रिक्त;

        यदि (enomem)
                फर्काऊ रिक्त;

        फर्काऊ l;
}

वर्ण** path_strv_resolve_uniq(वर्ण **l, अचर वर्ण *जरा) {

        यदि (strv_isempty(l))
                फर्काऊ l;

        यदि (!path_strv_resolve(l, जरा))
                फर्काऊ रिक्त;

        फर्काऊ strv_uniq(l);
}

वर्ण* skip_leading_slash(अचर वर्ण *p) {
        फर्काऊ skip_leading_chars(p, "/");
}

वर्ण* path_simplify_full(वर्ण *path, PathSimplifyFlags flags) {
        bool add_slash = false, keep_trailing_slash, absolute, beginning = true;
        वर्ण *f = path;
        पूर्णाङ्क r;

        /* Removes redundant inner and trailing slashes. Also removes unnecessary dots.
         * Modifies the passed string in-place.
         *
         * ///foo//./bar/.   becomes /foo/bar
         * .//./foo//./bar/. becomes foo/bar
         * /../foo/bar       becomes /foo/bar
         * /../foo/bar/..    becomes /foo/bar/..
         */

        यदि (isempty(path))
                फर्काऊ path;

        keep_trailing_slash = FLAGS_SET(flags, PATH_SIMPLIFY_KEEP_TRAILING_SLASH) && endswith(path, "/");

        absolute = path_is_absolute(path);
        f += absolute;  /* Keep leading /, if present. */

        लागि (अचर वर्ण *p = f;;) {
                अचर वर्ण *e;

                r = path_find_first_component(&p, true, &e);
                यदि (r == 0)
                        रोक;

                यदि (r > 0 && absolute && beginning && path_startswith(e, ".."))
                        /* If we're at the beginning of an absolute path, we can safely skip ".." */
                        जारी;

                beginning = false;

                यदि (add_slash)
                        *f++ = '/';

                यदि (r < 0) {
                        /* if path is invalid, then refuse to simplify the remaining part. */
                        स_हटाओ(f, p, म_माप(p) + 1);
                        फर्काऊ path;
                }

                स_हटाओ(f, e, r);
                f += r;

                add_slash = true;
        }

        /* Special rule, if we stripped everything, we need a "." for the current directory. */
        यदि (f == path)
                *f++ = '.';

        यदि (*(f-1) != '/' && keep_trailing_slash)
                *f++ = '/';

        *f = '\0';
        फर्काऊ path;
}

पूर्णाङ्क path_simplify_alloc(अचर वर्ण *path, वर्ण **ret) {
        प्रतिज्ञा(ret);

        यदि (!path) {
                *ret = रिक्त;
                फर्काऊ 0;
        }

        वर्ण *t = strdup(path);
        यदि (!t)
                फर्काऊ -ENOMEM;

        *ret = path_simplify(t);
        फर्काऊ 0;
}

वर्ण* path_startswith_full(अचर वर्ण *original_path, अचर वर्ण *prefix, PathStartWithFlags flags) {
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

        अचर वर्ण *path = original_path;

        यदि ((path[0] == '/') != (prefix[0] == '/'))
                फर्काऊ रिक्त;

        लागि (;;) {
                अचर वर्ण *p, *q;
                पूर्णाङ्क m, n;

                m = path_find_first_component(&path, !FLAGS_SET(flags, PATH_STARTSWITH_REFUSE_DOT_DOT), &p);
                यदि (m < 0)
                        फर्काऊ रिक्त;

                n = path_find_first_component(&prefix, !FLAGS_SET(flags, PATH_STARTSWITH_REFUSE_DOT_DOT), &q);
                यदि (n < 0)
                        फर्काऊ रिक्त;

                यदि (n == 0) {
                        यदि (!p)
                                p = path;

                        यदि (FLAGS_SET(flags, PATH_STARTSWITH_RETURN_LEADING_SLASH)) {

                                यदि (p <= original_path)
                                        फर्काऊ रिक्त;

                                p--;

                                यदि (*p != '/')
                                        फर्काऊ रिक्त;
                        }

                        फर्काऊ (वर्ण*) p;
                }

                यदि (m != n)
                        फर्काऊ रिक्त;

                यदि (!strneq(p, q, m))
                        फर्काऊ रिक्त;
        }
}

पूर्णाङ्क path_compare(अचर वर्ण *a, अचर वर्ण *b) {
        पूर्णाङ्क r;

        /* Order NULL before non-NULL */
        r = CMP(!!a, !!b);
        यदि (r != 0)
                फर्काऊ r;

        /* A relative path and an absolute path must not compare as equal.
         * Which one is sorted before the other does not really matter.
         * Here a relative path is ordered before an absolute path. */
        r = CMP(path_is_absolute(a), path_is_absolute(b));
        यदि (r != 0)
                फर्काऊ r;

        लागि (;;) {
                अचर वर्ण *aa, *bb;
                पूर्णाङ्क j, k;

                j = path_find_first_component(&a, true, &aa);
                k = path_find_first_component(&b, true, &bb);

                यदि (j < 0 || k < 0) {
                        /* When one of paths is invalid, order invalid path after valid one. */
                        r = CMP(j < 0, k < 0);
                        यदि (r != 0)
                                फर्काऊ r;

                        /* fallback to use strcmp() if both paths are invalid. */
                        फर्काऊ म_भेद(a, b);
                }

                /* Order prefixes first: "/foo" before "/foo/bar" */
                यदि (j == 0) {
                        यदि (k == 0)
                                फर्काऊ 0;
                        फर्काऊ -1;
                }
                यदि (k == 0)
                        फर्काऊ 1;

                /* Alphabetical sort: "/foo/aaa" before "/foo/b" */
                r = स_भेद(aa, bb, MIN(j, k));
                यदि (r != 0)
                        फर्काऊ r;

                /* Sort "/foo/a" before "/foo/aaa" */
                r = CMP(j, k);
                यदि (r != 0)
                        फर्काऊ r;
        }
}

पूर्णाङ्क path_compare_filename(अचर वर्ण *a, अचर वर्ण *b) {
        _cleanup_free_ वर्ण *fa = रिक्त, *fb = रिक्त;
        पूर्णाङ्क r, j, k;

        /* Order NULL before non-NULL */
        r = CMP(!!a, !!b);
        यदि (r != 0)
                फर्काऊ r;

        j = path_extract_filename(a, &fa);
        k = path_extract_filename(b, &fb);

        /* When one of paths is "." or root, then order it earlier. */
        r = CMP(j != -EADDRNOTAVAIL, k != -EADDRNOTAVAIL);
        यदि (r != 0)
                फर्काऊ r;

        /* When one of paths is invalid (or we get OOM), order invalid path after valid one. */
        r = CMP(j < 0, k < 0);
        यदि (r != 0)
                फर्काऊ r;

        /* fallback to use strcmp() if both paths are invalid. */
        यदि (j < 0)
                फर्काऊ म_भेद(a, b);

        फर्काऊ म_भेद(fa, fb);
}

पूर्णाङ्क path_equal_or_inode_same_full(अचर वर्ण *a, अचर वर्ण *b, पूर्णाङ्क flags) {
        /* Returns true if paths are of the same entry, false if not, <0 on error. */

        यदि (path_equal(a, b))
                फर्काऊ 1;

        यदि (!a || !b)
                फर्काऊ 0;

        फर्काऊ inode_same(a, b, flags);
}

वर्ण* path_extend_internal(वर्ण **x, ...) {
        माप_प्रकार sz, old_sz;
        वर्ण *q, *nx;
        अचर वर्ण *p;
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
        जबसम्म ((p = बहु_तर्क(ap, वर्ण*)) != POINTER_MAX) {
                माप_प्रकार add;

                यदि (isempty(p))
                        जारी;

                add = 1 + म_माप(p);
                यदि (sz > SIZE_MAX - add) { /* overflow check */
                        बहु_पूर्ण(ap);
                        फर्काऊ रिक्त;
                }

                sz += add;
        }
        बहु_पूर्ण(ap);

        nx = पुनः_स्मृति(x ? *x : रिक्त, GREEDY_ALLOC_ROUND_UP(sz+1));
        यदि (!nx)
                फर्काऊ रिक्त;
        यदि (x)
                *x = nx;

        यदि (old_sz > 0)
                slash = nx[old_sz-1] == '/';
        अन्यथा {
                nx[old_sz] = 0;
                slash = true; /* no need to generate a slash anymore */
        }

        q = nx + old_sz;

        बहु_शुरू(ap, x);
        जबसम्म ((p = बहु_तर्क(ap, वर्ण*)) != POINTER_MAX) {
                यदि (isempty(p))
                        जारी;

                यदि (!slash && p[0] != '/')
                        *(q++) = '/';

                q = stpcpy(q, p);
                slash = endswith(p, "/");
        }
        बहु_पूर्ण(ap);

        फर्काऊ nx;
}

पूर्णाङ्क open_and_check_executable(अचर वर्ण *name, अचर वर्ण *जरा, वर्ण **ret_path, पूर्णाङ्क *ret_fd) {
        _cleanup_close_ पूर्णाङ्क fd = -EBADF;
        _cleanup_free_ वर्ण *resolved = रिक्त;
        पूर्णाङ्क r;

        प्रतिज्ञा(name);

        /* Function chase() is invoked only when root is not NULL, as using it regardless of
         * root value would alter the behavior of existing callers for example: /bin/sleep would become
         * /usr/bin/sleep when find_executables is called. Hence, this function should be invoked when
         * needed to avoid unforeseen regression or other complicated changes. */
        यदि (जरा) {
                /* prefix root to name in case full paths are not specified */
                r = chase(name, जरा, CHASE_PREFIX_जरा, &resolved, &fd);
                यदि (r < 0)
                        फर्काऊ r;

                name = resolved;
        } अन्यथा {
                /* We need to use O_PATH because there may be executables for which we have only exec permissions,
                 * but not read (usually suid executables). */
                fd = खोलो(name, O_PATH|O_CLOEXEC);
                यदि (fd < 0)
                        फर्काऊ -त्रुटि_सं;
        }

        r = fd_verify_regular(fd);
        यदि (r < 0)
                फर्काऊ r;

        r = access_fd(fd, X_OK);
        यदि (r == -ENOSYS)
                /* /proc/ is not mounted. Fall back to access(). */
                r = RET_NERRNO(access(name, X_OK));
        यदि (r < 0)
                फर्काऊ r;

        यदि (ret_path) {
                यदि (resolved)
                        *ret_path = TAKE_PTR(resolved);
                अन्यथा {
                        r = path_make_absolute_cwd(name, ret_path);
                        यदि (r < 0)
                                फर्काऊ r;

                        path_simplify(*ret_path);
                }
        }

        यदि (ret_fd)
                *ret_fd = TAKE_FD(fd);

        फर्काऊ 0;
}

पूर्णाङ्क find_executable_full(
                अचर वर्ण *name,
                अचर वर्ण *जरा,
                वर्ण * अचर *exec_खोज_path,
                bool use_path_envvar,
                वर्ण **ret_filename,
                पूर्णाङ्क *ret_fd) {

        पूर्णाङ्क last_error = -ENOENT, r = 0;

        प्रतिज्ञा(name);

        यदि (is_path(name))
                फर्काऊ open_and_check_executable(name, जरा, ret_filename, ret_fd);

        यदि (exec_खोज_path) {
                STRV_FOREACH(element, exec_खोज_path) {
                        _cleanup_free_ वर्ण *full_path = रिक्त;

                        यदि (!path_is_absolute(*element)) {
                                log_debug("Exec search path '%s' isn't absolute, ignoring.", *element);
                                जारी;
                        }

                        full_path = path_join(*element, name);
                        यदि (!full_path)
                                फर्काऊ -ENOMEM;

                        r = open_and_check_executable(full_path, जरा, ret_filename, ret_fd);
                        यदि (r >= 0)
                                फर्काऊ 0;
                        यदि (r != -EACCES)
                                last_error = r;
                }
                फर्काऊ last_error;
        }

        अचर वर्ण *p = रिक्त;

        यदि (use_path_envvar)
                /* Plain getenv, not secure_getenv, because we want to actually allow the user to pick the
                 * binary. */
                p = दो_पर्या("PATH");
        यदि (!p)
                p = default_PATH();

        /* Resolve a single-component name to a full path */
        लागि (;;) {
                _cleanup_free_ वर्ण *element = रिक्त;

                r = extract_first_word(&p, &element, ":", EXTRACT_RELAX|EXTRACT_DONT_COALESCE_SEPARATORS);
                यदि (r < 0)
                        फर्काऊ r;
                यदि (r == 0)
                        रोक;

                यदि (!path_is_absolute(element)) {
                        log_debug("Exec search path '%s' isn't absolute, ignoring.", element);
                        जारी;
                }

                यदि (!path_extend(&element, name))
                        फर्काऊ -ENOMEM;

                r = open_and_check_executable(element, जरा, ret_filename, ret_fd);
                यदि (r >= 0) /* Found it! */
                        फर्काऊ 0;
                /* PATH entries which we don't have access to are ignored, as per tradition. */
                यदि (r != -EACCES)
                        last_error = r;
        }

        फर्काऊ last_error;
}

स्थिर पूर्णाङ्क executable_is_good(अचर वर्ण *executable) {
        _cleanup_free_ वर्ण *p = रिक्त, *d = रिक्त;
        पूर्णाङ्क r;

        r = find_executable(executable, &p);
        यदि (r == -ENOENT)
                फर्काऊ 0;
        यदि (r < 0)
                फर्काऊ r;

        /* An fsck that is linked to /bin/true is a non-existent fsck */

        r = readlink_malloc(p, &d);
        यदि (r == -EINVAL) /* not a symlink */
                फर्काऊ 1;
        यदि (r < 0)
                फर्काऊ r;

        फर्काऊ !PATH_IN_SET(d, "true",
                               "/bin/true",
                               "/usr/bin/true",
                               "/dev/null");
}

पूर्णाङ्क fsck_exists(शून्य) {
        फर्काऊ executable_is_good("fsck");
}

पूर्णाङ्क fsck_exists_for_fstype(अचर वर्ण *fstype) {
        अचर वर्ण *checker;
        पूर्णाङ्क r;

        प्रतिज्ञा(fstype);

        यदि (streq(fstype, "auto"))
                फर्काऊ -EINVAL;

        r = fsck_exists();
        यदि (r <= 0)
                फर्काऊ r;

        checker = strjoina("fsck.", fstype);
        फर्काऊ executable_is_good(checker);
}

स्थिर अचर वर्ण* skip_slash_or_dot(अचर वर्ण *p) {
        POINTER_MAY_BE_NULL(p);

        लागि (; !isempty(p); p++) {
                यदि (*p == '/')
                        जारी;
                यदि (startswith(p, "./")) {
                        p++;
                        जारी;
                }
                रोक;
        }
        फर्काऊ p;
}

पूर्णाङ्क path_find_first_component(अचर वर्ण **p, bool accept_dot_dot, अचर वर्ण **ret) {
        अचर वर्ण *q, *first, *end_first, *next;
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
        यदि (isempty(first)) {
                *p = first;
                यदि (ret)
                        *ret = रिक्त;
                फर्काऊ 0;
        }
        यदि (streq(first, ".")) {
                *p = first + 1;
                यदि (ret)
                        *ret = रिक्त;
                फर्काऊ 0;
        }

        end_first = strchrnul(first, '/');
        len = end_first - first;

        यदि (len > NAME_MAX)
                फर्काऊ -EINVAL;
        यदि (!accept_dot_dot && len == 2 && first[0] == '.' && first[1] == '.')
                फर्काऊ -EINVAL;

        next = skip_slash_or_dot(end_first);

        *p = next + streq(next, ".");
        यदि (ret)
                *ret = first;
        फर्काऊ len;
}

स्थिर अचर वर्ण* skip_slash_or_dot_backward(अचर वर्ण *path, अचर वर्ण *q) {
        प्रतिज्ञा(path);
        प्रतिज्ञा(!q || q >= path);

        लागि (; q; q = PTR_SUB1(q, path)) {
                यदि (*q == '/')
                        जारी;
                यदि (q > path && strneq(q - 1, "/.", 2))
                        जारी;
                यदि (q == path && *q == '.')
                        जारी;
                रोक;
        }
        फर्काऊ q;
}

पूर्णाङ्क path_find_last_component(अचर वर्ण *path, bool accept_dot_dot, अचर वर्ण **next, अचर वर्ण **ret) {
        अचर वर्ण *q, *last_end, *last_begin;
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

        यदि (isempty(path)) {
                यदि (next)
                        *next = path;
                यदि (ret)
                        *ret = रिक्त;
                फर्काऊ 0;
        }

        यदि (next && *next) {
                यदि (*next < path || *next > path + म_माप(path))
                        फर्काऊ -EINVAL;
                यदि (*next == path) {
                        यदि (ret)
                                *ret = रिक्त;
                        फर्काऊ 0;
                }
                यदि (!IN_SET(**next, '\0', '/'))
                        फर्काऊ -EINVAL;
                q = *next - 1;
        } अन्यथा
                q = path + म_माप(path) - 1;

        q = skip_slash_or_dot_backward(path, q);
        यदि (!q) { /* the root directory, "." or "./" */
                यदि (next)
                        *next = path;
                यदि (ret)
                        *ret = रिक्त;
                फर्काऊ 0;
        }

        last_end = q + 1;

        जबसम्म (q && *q != '/')
                q = PTR_SUB1(q, path);

        last_begin = q ? q + 1 : path;
        len = last_end - last_begin;

        यदि (len > NAME_MAX)
                फर्काऊ -EINVAL;
        यदि (!accept_dot_dot && len == 2 && strneq(last_begin, "..", 2))
                फर्काऊ -EINVAL;

        यदि (next) {
                q = skip_slash_or_dot_backward(path, q);
                *next = q ? q + 1 : path;
        }

        यदि (ret)
                *ret = last_begin;
        फर्काऊ len;
}

अचर वर्ण* last_path_component(अचर वर्ण *path) {

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

        यदि (!path)
                फर्काऊ रिक्त;

        l = k = म_माप(path);
        यदि (l == 0) /* special case — an empty string */
                फर्काऊ path;

        जबसम्म (k > 0 && path[k-1] == '/')
                k--;

        यदि (k == 0) /* the root directory */
                फर्काऊ path + l - 1;

        जबसम्म (k > 0 && path[k-1] != '/')
                k--;

        फर्काऊ path + k;
}

पूर्णाङ्क path_split_prefix_filename(अचर वर्ण *path, वर्ण **ret_dir, वर्ण **ret_filename) {
        _cleanup_free_ वर्ण *d = रिक्त;
        अचर वर्ण *c, *next = रिक्त;
        पूर्णाङ्क r;

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

        यदि (isempty(path))
                फर्काऊ -EINVAL;

        r = path_find_last_component(path, /* accept_dot_dot = */ false, &next, &c);
        यदि (r < 0)
                फर्काऊ r;
        यदि (r == 0) /* root directory or "." */
                फर्काऊ -EADDRNOTAVAIL;

        यदि (ret_dir) {
                यदि (next == path) {
                        यदि (*path != '/') { /* filename only */
                                यदि (!ret_filename)
                                        फर्काऊ -EDESTADDRREQ;
                        } अन्यथा {
                                d = strdup("/");
                                यदि (!d)
                                        फर्काऊ -ENOMEM;
                        }
                } अन्यथा {
                        d = strndup(path, next - path);
                        यदि (!d)
                                फर्काऊ -ENOMEM;

                        path_simplify(d);

                        यदि (!path_is_valid(d))
                                फर्काऊ -EINVAL;
                }

        } अन्यथा यदि (!path_is_valid(path))
                /* We didn't validate the dir prefix, hence check if the whole path is valid now */
                फर्काऊ -EINVAL;

        यदि (ret_filename) {
                वर्ण *fn = strndup(c, r);
                यदि (!fn)
                        फर्काऊ -ENOMEM;

                *ret_filename = fn;
        }

        यदि (ret_dir)
                *ret_dir = TAKE_PTR(d);

        फर्काऊ म_माप(c) > (माप_प्रकार) r ? O_DIRECTORY : 0;
}

bool filename_part_is_valid(अचर वर्ण *p) {
        अचर वर्ण *e;

        /* Checks f the specified string is OK to be *part* of a filename. This is different from
         * filename_is_valid() as "." and ".." and "" are OK by this call, but not by filename_is_valid(). */

        यदि (!p)
                फर्काऊ false;

        e = strchrnul(p, '/');
        यदि (*e != 0)
                फर्काऊ false;

        यदि (e - p > NAME_MAX) /* NAME_MAX is counted *without* the trailing NUL byte */
                फर्काऊ false;

        फर्काऊ true;
}

bool filename_is_valid(अचर वर्ण *p) {

        यदि (isempty(p))
                फर्काऊ false;

        यदि (dot_or_dot_dot(p)) /* Yes, in this context we consider "." and ".." invalid */
                फर्काऊ false;

        फर्काऊ filename_part_is_valid(p);
}

bool path_is_valid_full(अचर वर्ण *p, bool accept_dot_dot) {
        यदि (isempty(p))
                फर्काऊ false;

        लागि (अचर वर्ण *e = p;;) {
                पूर्णाङ्क r;

                r = path_find_first_component(&e, accept_dot_dot, रिक्त);
                यदि (r < 0)
                        फर्काऊ false;

                यदि (e - p >= PATH_MAX) /* Already reached the maximum length for a path? (PATH_MAX is counted
                                        * *with* the trailing NUL byte) */
                        फर्काऊ false;
                यदि (*e == 0)           /* End of string? Yay! */
                        फर्काऊ true;
        }
}

bool path_is_normalized(अचर वर्ण *p) {
        यदि (!path_is_safe(p))
                फर्काऊ false;

        यदि (streq(p, ".") || startswith(p, "./") || endswith(p, "/.") || म_माला(p, "/./"))
                फर्काऊ false;

        यदि (म_माला(p, "//"))
                फर्काऊ false;

        फर्काऊ true;
}

पूर्णाङ्क file_in_same_dir(अचर वर्ण *path, अचर वर्ण *filename, वर्ण **ret) {
        _cleanup_free_ वर्ण *b = रिक्त;
        पूर्णाङ्क r;

        प्रतिज्ञा(path);
        प्रतिज्ञा(filename);
        प्रतिज्ञा(ret);

        /* This removes the last component of path and appends filename, unless the latter is absolute anyway
         * or the former isn't */

        यदि (path_is_absolute(filename))
                b = strdup(filename);
        अन्यथा {
                _cleanup_free_ वर्ण *dn = रिक्त;

                r = path_extract_directory(path, &dn);
                यदि (r == -EDESTADDRREQ) /* no path prefix */
                        b = strdup(filename);
                अन्यथा यदि (r < 0)
                        फर्काऊ r;
                अन्यथा
                        b = path_join(dn, filename);
        }
        यदि (!b)
                फर्काऊ -ENOMEM;

        *ret = TAKE_PTR(b);
        फर्काऊ 0;
}

bool hidden_or_backup_file(अचर वर्ण *filename) {
        प्रतिज्ञा(filename);

        यदि (filename[0] == '.' ||
            STR_IN_SET(filename,
                       "lost+found",
                       "aquota.user",
                       "aquota.group") ||
            endswith(filename, "~"))
                फर्काऊ true;

        अचर वर्ण *dot = म_खोजप(filename, '.');
        यदि (!dot)
                फर्काऊ false;

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

        फर्काऊ STR_IN_SET(dot + 1,
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

bool is_device_path(अचर वर्ण *path) {

        /* Returns true for paths that likely refer to a device, either by path in sysfs or to something in
         * /dev. This accepts any path that starts with /dev/ or /sys/ and has something after that prefix.
         * It does not actually resolve the path.
         *
         * Examples:
         * /dev/sda, /dev/sda/foo, /sys/class, /dev/.., /sys/.., /./dev/foo → yes.
         * /../dev/sda, /dev, /sys, /usr/path, /usr/../dev/sda → no.
         */

        अचर वर्ण *p = PATH_STARTSWITH_SET(ASSERT_PTR(path), "/dev/", "/sys/");
        फर्काऊ !isempty(p);
}

bool valid_device_गाँठो_path(अचर वर्ण *path) {

        /* Some superficial checks whether the specified path is a valid device node path, all without
         * looking at the actual device node. */

        यदि (!PATH_STARTSWITH_SET(path, "/dev/", "/run/systemd/inaccessible/"))
                फर्काऊ false;

        यदि (endswith(path, "/")) /* can't be a device node if it ends in a slash */
                फर्काऊ false;

        फर्काऊ path_is_normalized(path);
}

bool valid_device_allow_pattern(अचर वर्ण *path) {
        प्रतिज्ञा(path);

        /* Like valid_device_node_path(), but also allows full-subsystem expressions like those accepted by
         * DeviceAllow= and DeviceDeny=. */

        यदि (STARTSWITH_SET(path, "block-", "char-"))
                फर्काऊ true;

        फर्काऊ valid_device_गाँठो_path(path);
}

bool dot_or_dot_dot(अचर वर्ण *path) {
        यदि (!path)
                फर्काऊ false;
        यदि (path[0] != '.')
                फर्काऊ false;
        यदि (path[1] == 0)
                फर्काऊ true;
        यदि (path[1] != '.')
                फर्काऊ false;

        फर्काऊ path[2] == 0;
}

bool path_implies_directory(अचर वर्ण *path) {

        /* Sometimes, if we look at a path we already know it must refer to a directory, because it is
         * suffixed with a slash, or its last component is "." or ".." */

        यदि (!path)
                फर्काऊ false;

        यदि (dot_or_dot_dot(path))
                फर्काऊ true;

        फर्काऊ ENDSWITH_SET(path, "/", "/.", "/..");
}

bool empty_or_जरा(अचर वर्ण *path) {

        /* For operations relative to some root directory, returns true if the specified root directory is
         * redundant, i.e. either / or NULL or the empty string or any equivalent. */

        यदि (isempty(path))
                फर्काऊ true;

        फर्काऊ path_equal(path, "/");
}

अचर वर्ण* empty_to_जरा(अचर वर्ण *path) {
        फर्काऊ isempty(path) ? "/" : path;
}

पूर्णाङ्क empty_or_जरा_harder_to_null(अचर वर्ण **path) {
        पूर्णाङ्क r;

        प्रतिज्ञा(path);

        /* This nullifies the input path when the path is empty or points to "/". */

        यदि (empty_or_जरा(*path)) {
                *path = रिक्त;
                फर्काऊ 0;
        }

        r = path_is_जरा(*path);
        यदि (r < 0)
                फर्काऊ r;
        यदि (r > 0)
                *path = रिक्त;

        फर्काऊ 0;
}

bool path_strv_contains(वर्ण * अचर *l, अचर वर्ण *path) {
        प्रतिज्ञा(path);

        STRV_FOREACH(i, l)
                यदि (path_equal(*i, path))
                        फर्काऊ true;

        फर्काऊ false;
}

bool prefixed_path_strv_contains(वर्ण * अचर *l, अचर वर्ण *path) {
        प्रतिज्ञा(path);

        STRV_FOREACH(i, l) {
                अचर वर्ण *j = *i;

                यदि (*j == '-')
                        j++;
                यदि (*j == '+')
                        j++;

                यदि (path_equal(j, path))
                        फर्काऊ true;
        }

        फर्काऊ false;
}

पूर्णाङ्क path_glob_can_match(अचर वर्ण *pattern, अचर वर्ण *prefix, वर्ण **ret) {
        प्रतिज्ञा(pattern);
        प्रतिज्ञा(prefix);

        लागि (अचर वर्ण *a = pattern, *b = prefix;;) {
                _cleanup_free_ वर्ण *g = रिक्त, *h = रिक्त;
                अचर वर्ण *p, *q;
                पूर्णाङ्क r, s;

                r = path_find_first_component(&a, /* accept_dot_dot= */ false, &p);
                यदि (r < 0)
                        फर्काऊ r;

                s = path_find_first_component(&b, /* accept_dot_dot= */ false, &q);
                यदि (s < 0)
                        फर्काऊ s;

                यदि (s == 0) {
                        /* The pattern matches the prefix. */
                        यदि (ret) {
                                वर्ण *t;

                                t = path_join(prefix, p);
                                यदि (!t)
                                        फर्काऊ -ENOMEM;

                                *ret = t;
                        }
                        फर्काऊ true;
                }

                यदि (r == 0)
                        रोक;

                यदि (r == s && strneq(p, q, r))
                        जारी; /* common component. Check next. */

                g = strndup(p, r);
                यदि (!g)
                        फर्काऊ -ENOMEM;

                यदि (!string_is_glob(g))
                        रोक;

                /* We found a glob component. Check if the glob pattern matches the prefix component. */

                h = strndup(q, s);
                यदि (!h)
                        फर्काऊ -ENOMEM;

                r = fnmatch(g, h, 0);
                यदि (r == FNM_NOMATCH)
                        रोक;
                यदि (r != 0) /* Failure to process pattern? */
                        फर्काऊ -EINVAL;
        }

        /* The pattern does not match the prefix. */
        यदि (ret)
                *ret = रिक्त;
        फर्काऊ false;
}

#यदि HAVE_SPLIT_BIN
स्थिर bool dir_is_split(अचर वर्ण *a, अचर वर्ण *b) {
        पूर्णाङ्क r;

        r = inode_same(a, b, AT_NO_AUTOMOUNT);
        यदि (r < 0 && r != -ENOENT) {
                log_debug_errno(r, "Failed to compare \"%s\" and \"%s\", assuming split directories: %m", a, b);
                फर्काऊ true;
        }
        फर्काऊ r == 0;
}
#endif

अचर वर्ण* default_PATH(शून्य) {
#यदि HAVE_SPLIT_BIN
        स्थिर अचर वर्ण *default_path = रिक्त;

        /* Return one of the three sets of paths:
         * a) split /usr/s?bin, /usr/local/sbin doesn't matter.
         * b) merged /usr/s?bin, /usr/sbin is a symlink, but /usr/local/sbin is not,
         * c) fully merged, neither /usr/sbin nor /usr/local/sbin are symlinks,
         *
         * On error the fallback to the safe value with both directories as configured is returned.
         */

        यदि (default_path)
                फर्काऊ default_path;

        यदि (dir_is_split("/usr/sbin", "/usr/bin"))
                फर्काऊ (default_path = DEFAULT_PATH_WITH_FULL_SBIN);  /* a */
        यदि (dir_is_split("/usr/local/sbin", "/usr/local/bin"))
                फर्काऊ (default_path = DEFAULT_PATH_WITH_LOCAL_SBIN); /* b */
        फर्काऊ (default_path = DEFAULT_PATH_WITHOUT_SBIN);            /* c */
#अन्यथा
        फर्काऊ DEFAULT_PATH_WITHOUT_SBIN;
#endif
}
