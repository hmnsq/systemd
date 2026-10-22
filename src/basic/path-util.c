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

bool is_path(ध्रुव वर्ण *p) {
        जइ (!p) /* A NULL pointer is definitely not a path */
                प्रत्यागम false;

        प्रत्यागम strchr(p, '/');
}

पूर्णाङ्क path_split_and_make_absolute(ध्रुव वर्ण *p, वर्ण ***ret) {
        _cleanup_strv_free_ वर्ण **l = रिक्त;
        पूर्णाङ्क r;

        प्रतिज्ञा(p);
        प्रतिज्ञा(ret);

        l = strv_split(p, ":");
        जइ (!l)
                प्रत्यागम -ENOMEM;

        r = path_strv_make_absolute_cwd(l);
        जइ (r < 0)
                प्रत्यागम r;

        *ret = TAKE_PTR(l);
        प्रत्यागम r;
}

वर्ण* path_make_absolute(ध्रुव वर्ण *p, ध्रुव वर्ण *prefix) {
        प्रतिज्ञा(p);

        /* Makes every item in the list an absolute path by prepending
         * the prefix, if specified and necessary */

        जइ (path_is_absolute(p) || isempty(prefix))
                प्रत्यागम strdup(p);

        प्रत्यागम path_join(prefix, p);
}

पूर्णाङ्क safe_getcwd(वर्ण **ret) {
        _cleanup_free_ वर्ण *cwd = रिक्त;

        cwd = get_current_dir_name();
        जइ (!cwd)
                प्रत्यागम negative_errno();

        /* Let's make sure the directory is really absolute, to protect us from the logic behind
         * CVE-2018-1000001 */
        जइ (cwd[0] != '/')
                प्रत्यागम -ENOMEDIUM;

        जइ (ret)
                *ret = TAKE_PTR(cwd);

        प्रत्यागम 0;
}

पूर्णाङ्क path_make_absolute_cwd(ध्रुव वर्ण *p, वर्ण **ret) {
        वर्ण *c;
        पूर्णाङ्क r;

        प्रतिज्ञा(p);
        प्रतिज्ञा(ret);

        /* Similar to path_make_absolute(), but prefixes with the
         * current working directory. */

        जइ (path_is_absolute(p))
                c = strdup(p);
        अण्णहा {
                _cleanup_free_ वर्ण *cwd = रिक्त;

                r = safe_getcwd(&cwd);
                जइ (r < 0)
                        प्रत्यागम r;

                c = path_join(cwd, p);
        }
        जइ (!c)
                प्रत्यागम -ENOMEM;

        *ret = c;
        प्रत्यागम 0;
}

पूर्णाङ्क path_make_relative(ध्रुव वर्ण *from, ध्रुव वर्ण *to, वर्ण **ret) {
        _cleanup_free_ वर्ण *result = रिक्त;
        unsigned n_parents;
        ध्रुव वर्ण *f, *t;
        पूर्णाङ्क r, k;
        वर्ण *p;

        प्रतिज्ञा(from);
        प्रतिज्ञा(to);
        प्रतिज्ञा(ret);

        /* Strips the common part, and adds ".." elements as necessary. */

        जइ (!path_is_absolute(from) || !path_is_absolute(to))
                प्रत्यागम -EINVAL;

        प्रति (;;) {
                r = path_find_first_component(&from, true, &f);
                जइ (r < 0)
                        प्रत्यागम r;

                k = path_find_first_component(&to, true, &t);
                जइ (k < 0)
                        प्रत्यागम k;

                जइ (r == 0) {
                        /* end of 'from' */
                        जइ (k == 0) {
                                /* from and to are equivalent. */
                                result = strdup(".");
                                जइ (!result)
                                        प्रत्यागम -ENOMEM;
                        } अण्णहा {
                                /* 'to' is inside of 'from'. */
                                r = path_simplify_alloc(t, &result);
                                जइ (r < 0)
                                        प्रत्यागम r;

                                जइ (!path_is_valid(result))
                                        प्रत्यागम -EINVAL;
                        }

                        *ret = TAKE_PTR(result);
                        प्रत्यागम 0;
                }

                जइ (r != k || !strneq(f, t, r))
                        भङ्ग;
        }

        /* If we're here, then "from_dir" has one or more elements that need to
         * be replaced with "..". */

        प्रति (n_parents = 1;; n_parents++) {
                /* If this includes ".." we can't do a simple series of "..". */
                r = path_find_first_component(&from, false, &f);
                जइ (r < 0)
                        प्रत्यागम r;
                जइ (r == 0)
                        भङ्ग;
        }

        जइ (isempty(t) && n_parents * 3 > PATH_MAX)
                /* PATH_MAX is counted *with* the trailing NUL byte */
                प्रत्यागम -EINVAL;

        result = new(वर्ण, n_parents * 3 + !isempty(t) + strlen_ptr(t));
        जइ (!result)
                प्रत्यागम -ENOMEM;

        प्रति (p = result; n_parents > 0; n_parents--)
                p = mempcpy(p, "../", 3);

        जइ (isempty(t)) {
                /* Remove trailing slash and terminate string. */
                *(--p) = '\0';
                *ret = TAKE_PTR(result);
                प्रत्यागम 0;
        }

        strcpy(p, t);

        path_simplify(result);

        जइ (!path_is_valid(result))
                प्रत्यागम -EINVAL;

        *ret = TAKE_PTR(result);
        प्रत्यागम 0;
}

पूर्णाङ्क path_make_relative_parent(ध्रुव वर्ण *from_child, ध्रुव वर्ण *to, वर्ण **ret) {
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
        जइ (r < 0)
                प्रत्यागम r;

        प्रत्यागम path_make_relative(from, to, ret);
}

वर्ण* path_startswith_strv(ध्रुव वर्ण *p, वर्ण * ध्रुव *strv) {
        प्रतिज्ञा(p);

        STRV_FOREACH(s, strv) {
                वर्ण *t;

                t = path_startswith(p, *s);
                जइ (t)
                        प्रत्यागम t;
        }

        प्रत्यागम रिक्त;
}

पूर्णाङ्क path_strv_make_absolute_cwd(वर्ण **l) {
        पूर्णाङ्क r;

        /* Goes through every item in the string list and makes it
         * absolute. This works in place and won't rollback any
         * changes on failure. */

        STRV_FOREACH(s, l) {
                वर्ण *t;

                r = path_make_absolute_cwd(*s, &t);
                जइ (r < 0)
                        प्रत्यागम r;

                path_simplify(t);
                free_and_replace(*s, t);
        }

        प्रत्यागम 0;
}

वर्ण** path_strv_resolve(वर्ण **l, ध्रुव वर्ण *मूलम्) {
        unsigned k = 0;
        bool enomem = false;
        पूर्णाङ्क r;

        जइ (strv_isempty(l))
                प्रत्यागम l;

        /* Go through every item in the string list and canonicalize the path.
         * This works in place and doesn't roll back any changes on failure. */

        STRV_FOREACH(s, l) {
                _cleanup_free_ वर्ण *orig = रिक्त;
                वर्ण *t, *u;

                जइ (मूलम्) {
                        जइ (!path_is_absolute(*s)) {
                                log_debug("Ignoring non-absolute search path '%s'.", *s);
                                मुक्त(*s);
                                अनुवर्त;
                        }

                        orig = *s;
                        t = path_join(मूलम्, orig);
                        जइ (!t) {
                                enomem = true;
                                अनुवर्त;
                        }
                } अण्णहा
                        t = *s;

                r = chase(t, मूलम्, 0, &u, रिक्त);
                जइ (r == -ENOENT) {
                        जइ (मूलम्) {
                                u = TAKE_PTR(orig);
                                मुक्त(t);
                        } अण्णहा
                                u = t;
                } अण्णहा जइ (r < 0) {
                        मुक्त(t);

                        जइ (r == -ENOMEM)
                                enomem = true;

                        अनुवर्त;
                } अण्णहा जइ (मूलम्) {
                        वर्ण *x;

                        मुक्त(t);
                        x = path_startswith(u, मूलम्);
                        जइ (x) {
                                /* restore the slash if it was lost */
                                जइ (!startswith(x, "/"))
                                        *(--x) = '/';

                                t = strdup(x);
                                मुक्त(u);
                                जइ (!t) {
                                        enomem = true;
                                        अनुवर्त;
                                }
                                u = t;
                        } अण्णहा {
                                /* canonicalized path goes outside of
                                 * prefix, keep the original path instead */
                                free_and_replace(u, orig);
                        }
                } अण्णहा
                        मुक्त(t);

                l[k++] = u;
        }

        l[k] = रिक्त;

        जइ (enomem)
                प्रत्यागम रिक्त;

        प्रत्यागम l;
}

वर्ण** path_strv_resolve_uniq(वर्ण **l, ध्रुव वर्ण *मूलम्) {

        जइ (strv_isempty(l))
                प्रत्यागम l;

        जइ (!path_strv_resolve(l, मूलम्))
                प्रत्यागम रिक्त;

        प्रत्यागम strv_uniq(l);
}

वर्ण* skip_leading_slash(ध्रुव वर्ण *p) {
        प्रत्यागम skip_leading_chars(p, "/");
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

        जइ (isempty(path))
                प्रत्यागम path;

        keep_trailing_slash = FLAGS_SET(flags, PATH_SIMPLIFY_KEEP_TRAILING_SLASH) && endswith(path, "/");

        absolute = path_is_absolute(path);
        f += absolute;  /* Keep leading /, if present. */

        प्रति (ध्रुव वर्ण *p = f;;) {
                ध्रुव वर्ण *e;

                r = path_find_first_component(&p, true, &e);
                जइ (r == 0)
                        भङ्ग;

                जइ (r > 0 && absolute && beginning && path_startswith(e, ".."))
                        /* If we're at the beginning of an absolute path, we can safely skip ".." */
                        अनुवर्त;

                beginning = false;

                जइ (add_slash)
                        *f++ = '/';

                जइ (r < 0) {
                        /* if path is invalid, then refuse to simplify the remaining part. */
                        memmove(f, p, strlen(p) + 1);
                        प्रत्यागम path;
                }

                memmove(f, e, r);
                f += r;

                add_slash = true;
        }

        /* Special rule, if we stripped everything, we need a "." for the current directory. */
        जइ (f == path)
                *f++ = '.';

        जइ (*(f-1) != '/' && keep_trailing_slash)
                *f++ = '/';

        *f = '\0';
        प्रत्यागम path;
}

पूर्णाङ्क path_simplify_alloc(ध्रुव वर्ण *path, वर्ण **ret) {
        प्रतिज्ञा(ret);

        जइ (!path) {
                *ret = रिक्त;
                प्रत्यागम 0;
        }

        वर्ण *t = strdup(path);
        जइ (!t)
                प्रत्यागम -ENOMEM;

        *ret = path_simplify(t);
        प्रत्यागम 0;
}

वर्ण* path_startswith_full(ध्रुव वर्ण *original_path, ध्रुव वर्ण *prefix, PathStartWithFlags flags) {
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

        ध्रुव वर्ण *path = original_path;

        जइ ((path[0] == '/') != (prefix[0] == '/'))
                प्रत्यागम रिक्त;

        प्रति (;;) {
                ध्रुव वर्ण *p, *q;
                पूर्णाङ्क m, n;

                m = path_find_first_component(&path, !FLAGS_SET(flags, PATH_STARTSWITH_REFUSE_DOT_DOT), &p);
                जइ (m < 0)
                        प्रत्यागम रिक्त;

                n = path_find_first_component(&prefix, !FLAGS_SET(flags, PATH_STARTSWITH_REFUSE_DOT_DOT), &q);
                जइ (n < 0)
                        प्रत्यागम रिक्त;

                जइ (n == 0) {
                        जइ (!p)
                                p = path;

                        जइ (FLAGS_SET(flags, PATH_STARTSWITH_RETURN_LEADING_SLASH)) {

                                जइ (p <= original_path)
                                        प्रत्यागम रिक्त;

                                p--;

                                जइ (*p != '/')
                                        प्रत्यागम रिक्त;
                        }

                        प्रत्यागम (वर्ण*) p;
                }

                जइ (m != n)
                        प्रत्यागम रिक्त;

                जइ (!strneq(p, q, m))
                        प्रत्यागम रिक्त;
        }
}

पूर्णाङ्क path_compare(ध्रुव वर्ण *a, ध्रुव वर्ण *b) {
        पूर्णाङ्क r;

        /* Order NULL before non-NULL */
        r = CMP(!!a, !!b);
        जइ (r != 0)
                प्रत्यागम r;

        /* A relative path and an absolute path must not compare as equal.
         * Which one is sorted before the other does not really matter.
         * Here a relative path is ordered before an absolute path. */
        r = CMP(path_is_absolute(a), path_is_absolute(b));
        जइ (r != 0)
                प्रत्यागम r;

        प्रति (;;) {
                ध्रुव वर्ण *aa, *bb;
                पूर्णाङ्क j, k;

                j = path_find_first_component(&a, true, &aa);
                k = path_find_first_component(&b, true, &bb);

                जइ (j < 0 || k < 0) {
                        /* When one of paths is invalid, order invalid path after valid one. */
                        r = CMP(j < 0, k < 0);
                        जइ (r != 0)
                                प्रत्यागम r;

                        /* fallback to use strcmp() if both paths are invalid. */
                        प्रत्यागम strcmp(a, b);
                }

                /* Order prefixes first: "/foo" before "/foo/bar" */
                जइ (j == 0) {
                        जइ (k == 0)
                                प्रत्यागम 0;
                        प्रत्यागम -1;
                }
                जइ (k == 0)
                        प्रत्यागम 1;

                /* Alphabetical sort: "/foo/aaa" before "/foo/b" */
                r = memcmp(aa, bb, MIN(j, k));
                जइ (r != 0)
                        प्रत्यागम r;

                /* Sort "/foo/a" before "/foo/aaa" */
                r = CMP(j, k);
                जइ (r != 0)
                        प्रत्यागम r;
        }
}

पूर्णाङ्क path_compare_filename(ध्रुव वर्ण *a, ध्रुव वर्ण *b) {
        _cleanup_free_ वर्ण *fa = रिक्त, *fb = रिक्त;
        पूर्णाङ्क r, j, k;

        /* Order NULL before non-NULL */
        r = CMP(!!a, !!b);
        जइ (r != 0)
                प्रत्यागम r;

        j = path_extract_filename(a, &fa);
        k = path_extract_filename(b, &fb);

        /* When one of paths is "." or root, then order it earlier. */
        r = CMP(j != -EADDRNOTAVAIL, k != -EADDRNOTAVAIL);
        जइ (r != 0)
                प्रत्यागम r;

        /* When one of paths is invalid (or we get OOM), order invalid path after valid one. */
        r = CMP(j < 0, k < 0);
        जइ (r != 0)
                प्रत्यागम r;

        /* fallback to use strcmp() if both paths are invalid. */
        जइ (j < 0)
                प्रत्यागम strcmp(a, b);

        प्रत्यागम strcmp(fa, fb);
}

पूर्णाङ्क path_equal_or_inode_same_full(ध्रुव वर्ण *a, ध्रुव वर्ण *b, पूर्णाङ्क flags) {
        /* Returns true if paths are of the same entry, false if not, <0 on error. */

        जइ (path_equal(a, b))
                प्रत्यागम 1;

        जइ (!a || !b)
                प्रत्यागम 0;

        प्रत्यागम inode_same(a, b, flags);
}

वर्ण* path_extend_internal(वर्ण **x, ...) {
        size_t sz, old_sz;
        वर्ण *q, *nx;
        ध्रुव वर्ण *p;
        va_list ap;
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
        va_start(ap, x);
        जाव ((p = va_arg(ap, वर्ण*)) != POINTER_MAX) {
                size_t add;

                जइ (isempty(p))
                        अनुवर्त;

                add = 1 + strlen(p);
                जइ (sz > SIZE_MAX - add) { /* overflow check */
                        va_end(ap);
                        प्रत्यागम रिक्त;
                }

                sz += add;
        }
        va_end(ap);

        nx = realloc(x ? *x : रिक्त, GREEDY_ALLOC_ROUND_UP(sz+1));
        जइ (!nx)
                प्रत्यागम रिक्त;
        जइ (x)
                *x = nx;

        जइ (old_sz > 0)
                slash = nx[old_sz-1] == '/';
        अण्णहा {
                nx[old_sz] = 0;
                slash = true; /* no need to generate a slash anymore */
        }

        q = nx + old_sz;

        va_start(ap, x);
        जाव ((p = va_arg(ap, वर्ण*)) != POINTER_MAX) {
                जइ (isempty(p))
                        अनुवर्त;

                जइ (!slash && p[0] != '/')
                        *(q++) = '/';

                q = stpcpy(q, p);
                slash = endswith(p, "/");
        }
        va_end(ap);

        प्रत्यागम nx;
}

पूर्णाङ्क open_and_check_executable(ध्रुव वर्ण *name, ध्रुव वर्ण *मूलम्, वर्ण **ret_path, पूर्णाङ्क *ret_fd) {
        _cleanup_close_ पूर्णाङ्क fd = -EBADF;
        _cleanup_free_ वर्ण *resolved = रिक्त;
        पूर्णाङ्क r;

        प्रतिज्ञा(name);

        /* Function chase() is invoked only when root is not NULL, as using it regardless of
         * root value would alter the behavior of existing callers for example: /bin/sleep would become
         * /usr/bin/sleep when find_executables is called. Hence, this function should be invoked when
         * needed to avoid unforeseen regression or other complicated changes. */
        जइ (मूलम्) {
                /* prefix root to name in case full paths are not specified */
                r = chase(name, मूलम्, CHASE_PREFIX_मूलम्, &resolved, &fd);
                जइ (r < 0)
                        प्रत्यागम r;

                name = resolved;
        } अण्णहा {
                /* We need to use O_PATH because there may be executables for which we have only exec permissions,
                 * but not read (usually suid executables). */
                fd = open(name, O_PATH|O_CLOEXEC);
                जइ (fd < 0)
                        प्रत्यागम -errno;
        }

        r = fd_verify_regular(fd);
        जइ (r < 0)
                प्रत्यागम r;

        r = access_fd(fd, X_OK);
        जइ (r == -ENOSYS)
                /* /proc/ is not mounted. Fall back to access(). */
                r = RET_NERRNO(access(name, X_OK));
        जइ (r < 0)
                प्रत्यागम r;

        जइ (ret_path) {
                जइ (resolved)
                        *ret_path = TAKE_PTR(resolved);
                अण्णहा {
                        r = path_make_absolute_cwd(name, ret_path);
                        जइ (r < 0)
                                प्रत्यागम r;

                        path_simplify(*ret_path);
                }
        }

        जइ (ret_fd)
                *ret_fd = TAKE_FD(fd);

        प्रत्यागम 0;
}

पूर्णाङ्क find_executable_full(
                ध्रुव वर्ण *name,
                ध्रुव वर्ण *मूलम्,
                वर्ण * ध्रुव *exec_अन्वेषणम्_path,
                bool use_path_envvar,
                वर्ण **ret_filename,
                पूर्णाङ्क *ret_fd) {

        पूर्णाङ्क last_error = -ENOENT, r = 0;

        प्रतिज्ञा(name);

        जइ (is_path(name))
                प्रत्यागम open_and_check_executable(name, मूलम्, ret_filename, ret_fd);

        जइ (exec_अन्वेषणम्_path) {
                STRV_FOREACH(element, exec_अन्वेषणम्_path) {
                        _cleanup_free_ वर्ण *full_path = रिक्त;

                        जइ (!path_is_absolute(*element)) {
                                log_debug("Exec search path '%s' isn't absolute, ignoring.", *element);
                                अनुवर्त;
                        }

                        full_path = path_join(*element, name);
                        जइ (!full_path)
                                प्रत्यागम -ENOMEM;

                        r = open_and_check_executable(full_path, मूलम्, ret_filename, ret_fd);
                        जइ (r >= 0)
                                प्रत्यागम 0;
                        जइ (r != -EACCES)
                                last_error = r;
                }
                प्रत्यागम last_error;
        }

        ध्रुव वर्ण *p = रिक्त;

        जइ (use_path_envvar)
                /* Plain getenv, not secure_getenv, because we want to actually allow the user to pick the
                 * binary. */
                p = getenv("PATH");
        जइ (!p)
                p = default_PATH();

        /* Resolve a single-component name to a full path */
        प्रति (;;) {
                _cleanup_free_ वर्ण *element = रिक्त;

                r = extract_first_word(&p, &element, ":", EXTRACT_RELAX|EXTRACT_DONT_COALESCE_SEPARATORS);
                जइ (r < 0)
                        प्रत्यागम r;
                जइ (r == 0)
                        भङ्ग;

                जइ (!path_is_absolute(element)) {
                        log_debug("Exec search path '%s' isn't absolute, ignoring.", element);
                        अनुवर्त;
                }

                जइ (!path_extend(&element, name))
                        प्रत्यागम -ENOMEM;

                r = open_and_check_executable(element, मूलम्, ret_filename, ret_fd);
                जइ (r >= 0) /* Found it! */
                        प्रत्यागम 0;
                /* PATH entries which we don't have access to are ignored, as per tradition. */
                जइ (r != -EACCES)
                        last_error = r;
        }

        प्रत्यागम last_error;
}

स्थिर पूर्णाङ्क executable_is_good(ध्रुव वर्ण *executable) {
        _cleanup_free_ वर्ण *p = रिक्त, *d = रिक्त;
        पूर्णाङ्क r;

        r = find_executable(executable, &p);
        जइ (r == -ENOENT)
                प्रत्यागम 0;
        जइ (r < 0)
                प्रत्यागम r;

        /* An fsck that is linked to /bin/true is a non-existent fsck */

        r = readlink_malloc(p, &d);
        जइ (r == -EINVAL) /* not a symlink */
                प्रत्यागम 1;
        जइ (r < 0)
                प्रत्यागम r;

        प्रत्यागम !PATH_IN_SET(d, "true",
                               "/bin/true",
                               "/usr/bin/true",
                               "/dev/null");
}

पूर्णाङ्क fsck_exists(शून्य) {
        प्रत्यागम executable_is_good("fsck");
}

पूर्णाङ्क fsck_exists_for_fstype(ध्रुव वर्ण *fstype) {
        ध्रुव वर्ण *checker;
        पूर्णाङ्क r;

        प्रतिज्ञा(fstype);

        जइ (streq(fstype, "auto"))
                प्रत्यागम -EINVAL;

        r = fsck_exists();
        जइ (r <= 0)
                प्रत्यागम r;

        checker = strjoina("fsck.", fstype);
        प्रत्यागम executable_is_good(checker);
}

स्थिर ध्रुव वर्ण* skip_slash_or_dot(ध्रुव वर्ण *p) {
        POINTER_MAY_BE_NULL(p);

        प्रति (; !isempty(p); p++) {
                जइ (*p == '/')
                        अनुवर्त;
                जइ (startswith(p, "./")) {
                        p++;
                        अनुवर्त;
                }
                भङ्ग;
        }
        प्रत्यागम p;
}

पूर्णाङ्क path_find_first_component(ध्रुव वर्ण **p, bool accept_dot_dot, ध्रुव वर्ण **ret) {
        ध्रुव वर्ण *q, *first, *end_first, *next;
        size_t len;

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
        जइ (isempty(first)) {
                *p = first;
                जइ (ret)
                        *ret = रिक्त;
                प्रत्यागम 0;
        }
        जइ (streq(first, ".")) {
                *p = first + 1;
                जइ (ret)
                        *ret = रिक्त;
                प्रत्यागम 0;
        }

        end_first = strchrnul(first, '/');
        len = end_first - first;

        जइ (len > NAME_MAX)
                प्रत्यागम -EINVAL;
        जइ (!accept_dot_dot && len == 2 && first[0] == '.' && first[1] == '.')
                प्रत्यागम -EINVAL;

        next = skip_slash_or_dot(end_first);

        *p = next + streq(next, ".");
        जइ (ret)
                *ret = first;
        प्रत्यागम len;
}

स्थिर ध्रुव वर्ण* skip_slash_or_dot_backward(ध्रुव वर्ण *path, ध्रुव वर्ण *q) {
        प्रतिज्ञा(path);
        प्रतिज्ञा(!q || q >= path);

        प्रति (; q; q = PTR_SUB1(q, path)) {
                जइ (*q == '/')
                        अनुवर्त;
                जइ (q > path && strneq(q - 1, "/.", 2))
                        अनुवर्त;
                जइ (q == path && *q == '.')
                        अनुवर्त;
                भङ्ग;
        }
        प्रत्यागम q;
}

पूर्णाङ्क path_find_last_component(ध्रुव वर्ण *path, bool accept_dot_dot, ध्रुव वर्ण **next, ध्रुव वर्ण **ret) {
        ध्रुव वर्ण *q, *last_end, *last_begin;
        size_t len;

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

        जइ (isempty(path)) {
                जइ (next)
                        *next = path;
                जइ (ret)
                        *ret = रिक्त;
                प्रत्यागम 0;
        }

        जइ (next && *next) {
                जइ (*next < path || *next > path + strlen(path))
                        प्रत्यागम -EINVAL;
                जइ (*next == path) {
                        जइ (ret)
                                *ret = रिक्त;
                        प्रत्यागम 0;
                }
                जइ (!IN_SET(**next, '\0', '/'))
                        प्रत्यागम -EINVAL;
                q = *next - 1;
        } अण्णहा
                q = path + strlen(path) - 1;

        q = skip_slash_or_dot_backward(path, q);
        जइ (!q) { /* the root directory, "." or "./" */
                जइ (next)
                        *next = path;
                जइ (ret)
                        *ret = रिक्त;
                प्रत्यागम 0;
        }

        last_end = q + 1;

        जाव (q && *q != '/')
                q = PTR_SUB1(q, path);

        last_begin = q ? q + 1 : path;
        len = last_end - last_begin;

        जइ (len > NAME_MAX)
                प्रत्यागम -EINVAL;
        जइ (!accept_dot_dot && len == 2 && strneq(last_begin, "..", 2))
                प्रत्यागम -EINVAL;

        जइ (next) {
                q = skip_slash_or_dot_backward(path, q);
                *next = q ? q + 1 : path;
        }

        जइ (ret)
                *ret = last_begin;
        प्रत्यागम len;
}

ध्रुव वर्ण* last_path_component(ध्रुव वर्ण *path) {

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

        unsigned l, k;

        जइ (!path)
                प्रत्यागम रिक्त;

        l = k = strlen(path);
        जइ (l == 0) /* special case — an empty string */
                प्रत्यागम path;

        जाव (k > 0 && path[k-1] == '/')
                k--;

        जइ (k == 0) /* the root directory */
                प्रत्यागम path + l - 1;

        जाव (k > 0 && path[k-1] != '/')
                k--;

        प्रत्यागम path + k;
}

पूर्णाङ्क path_split_prefix_filename(ध्रुव वर्ण *path, वर्ण **ret_dir, वर्ण **ret_filename) {
        _cleanup_free_ वर्ण *d = रिक्त;
        ध्रुव वर्ण *c, *next = रिक्त;
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

        जइ (isempty(path))
                प्रत्यागम -EINVAL;

        r = path_find_last_component(path, /* accept_dot_dot = */ false, &next, &c);
        जइ (r < 0)
                प्रत्यागम r;
        जइ (r == 0) /* root directory or "." */
                प्रत्यागम -EADDRNOTAVAIL;

        जइ (ret_dir) {
                जइ (next == path) {
                        जइ (*path != '/') { /* filename only */
                                जइ (!ret_filename)
                                        प्रत्यागम -EDESTADDRREQ;
                        } अण्णहा {
                                d = strdup("/");
                                जइ (!d)
                                        प्रत्यागम -ENOMEM;
                        }
                } अण्णहा {
                        d = strndup(path, next - path);
                        जइ (!d)
                                प्रत्यागम -ENOMEM;

                        path_simplify(d);

                        जइ (!path_is_valid(d))
                                प्रत्यागम -EINVAL;
                }

        } अण्णहा जइ (!path_is_valid(path))
                /* We didn't validate the dir prefix, hence check if the whole path is valid now */
                प्रत्यागम -EINVAL;

        जइ (ret_filename) {
                वर्ण *fn = strndup(c, r);
                जइ (!fn)
                        प्रत्यागम -ENOMEM;

                *ret_filename = fn;
        }

        जइ (ret_dir)
                *ret_dir = TAKE_PTR(d);

        प्रत्यागम strlen(c) > (size_t) r ? O_DIRECTORY : 0;
}

bool filename_part_is_valid(ध्रुव वर्ण *p) {
        ध्रुव वर्ण *e;

        /* Checks f the specified string is OK to be *part* of a filename. This is different from
         * filename_is_valid() as "." and ".." and "" are OK by this call, but not by filename_is_valid(). */

        जइ (!p)
                प्रत्यागम false;

        e = strchrnul(p, '/');
        जइ (*e != 0)
                प्रत्यागम false;

        जइ (e - p > NAME_MAX) /* NAME_MAX is counted *without* the trailing NUL byte */
                प्रत्यागम false;

        प्रत्यागम true;
}

bool filename_is_valid(ध्रुव वर्ण *p) {

        जइ (isempty(p))
                प्रत्यागम false;

        जइ (dot_or_dot_dot(p)) /* Yes, in this context we consider "." and ".." invalid */
                प्रत्यागम false;

        प्रत्यागम filename_part_is_valid(p);
}

bool path_is_valid_full(ध्रुव वर्ण *p, bool accept_dot_dot) {
        जइ (isempty(p))
                प्रत्यागम false;

        प्रति (ध्रुव वर्ण *e = p;;) {
                पूर्णाङ्क r;

                r = path_find_first_component(&e, accept_dot_dot, रिक्त);
                जइ (r < 0)
                        प्रत्यागम false;

                जइ (e - p >= PATH_MAX) /* Already reached the maximum length for a path? (PATH_MAX is counted
                                        * *with* the trailing NUL byte) */
                        प्रत्यागम false;
                जइ (*e == 0)           /* End of string? Yay! */
                        प्रत्यागम true;
        }
}

bool path_is_normalized(ध्रुव वर्ण *p) {
        जइ (!path_is_safe(p))
                प्रत्यागम false;

        जइ (streq(p, ".") || startswith(p, "./") || endswith(p, "/.") || strstr(p, "/./"))
                प्रत्यागम false;

        जइ (strstr(p, "//"))
                प्रत्यागम false;

        प्रत्यागम true;
}

पूर्णाङ्क file_in_same_dir(ध्रुव वर्ण *path, ध्रुव वर्ण *filename, वर्ण **ret) {
        _cleanup_free_ वर्ण *b = रिक्त;
        पूर्णाङ्क r;

        प्रतिज्ञा(path);
        प्रतिज्ञा(filename);
        प्रतिज्ञा(ret);

        /* This removes the last component of path and appends filename, unless the latter is absolute anyway
         * or the former isn't */

        जइ (path_is_absolute(filename))
                b = strdup(filename);
        अण्णहा {
                _cleanup_free_ वर्ण *dn = रिक्त;

                r = path_extract_directory(path, &dn);
                जइ (r == -EDESTADDRREQ) /* no path prefix */
                        b = strdup(filename);
                अण्णहा जइ (r < 0)
                        प्रत्यागम r;
                अण्णहा
                        b = path_join(dn, filename);
        }
        जइ (!b)
                प्रत्यागम -ENOMEM;

        *ret = TAKE_PTR(b);
        प्रत्यागम 0;
}

bool hidden_or_backup_file(ध्रुव वर्ण *filename) {
        प्रतिज्ञा(filename);

        जइ (filename[0] == '.' ||
            STR_IN_SET(filename,
                       "lost+found",
                       "aquota.user",
                       "aquota.group") ||
            endswith(filename, "~"))
                प्रत्यागम true;

        ध्रुव वर्ण *dot = strrchr(filename, '.');
        जइ (!dot)
                प्रत्यागम false;

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

        प्रत्यागम STR_IN_SET(dot + 1,
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

bool is_device_path(ध्रुव वर्ण *path) {

        /* Returns true for paths that likely refer to a device, either by path in sysfs or to something in
         * /dev. This accepts any path that starts with /dev/ or /sys/ and has something after that prefix.
         * It does not actually resolve the path.
         *
         * Examples:
         * /dev/sda, /dev/sda/foo, /sys/class, /dev/.., /sys/.., /./dev/foo → yes.
         * /../dev/sda, /dev, /sys, /usr/path, /usr/../dev/sda → no.
         */

        ध्रुव वर्ण *p = PATH_STARTSWITH_SET(ASSERT_PTR(path), "/dev/", "/sys/");
        प्रत्यागम !isempty(p);
}

bool valid_device_ग्रन्थिः_path(ध्रुव वर्ण *path) {

        /* Some superficial checks whether the specified path is a valid device node path, all without
         * looking at the actual device node. */

        जइ (!PATH_STARTSWITH_SET(path, "/dev/", "/run/systemd/inaccessible/"))
                प्रत्यागम false;

        जइ (endswith(path, "/")) /* can't be a device node if it ends in a slash */
                प्रत्यागम false;

        प्रत्यागम path_is_normalized(path);
}

bool valid_device_allow_pattern(ध्रुव वर्ण *path) {
        प्रतिज्ञा(path);

        /* Like valid_device_node_path(), but also allows full-subsystem expressions like those accepted by
         * DeviceAllow= and DeviceDeny=. */

        जइ (STARTSWITH_SET(path, "block-", "char-"))
                प्रत्यागम true;

        प्रत्यागम valid_device_ग्रन्थिः_path(path);
}

bool dot_or_dot_dot(ध्रुव वर्ण *path) {
        जइ (!path)
                प्रत्यागम false;
        जइ (path[0] != '.')
                प्रत्यागम false;
        जइ (path[1] == 0)
                प्रत्यागम true;
        जइ (path[1] != '.')
                प्रत्यागम false;

        प्रत्यागम path[2] == 0;
}

bool path_implies_directory(ध्रुव वर्ण *path) {

        /* Sometimes, if we look at a path we already know it must refer to a directory, because it is
         * suffixed with a slash, or its last component is "." or ".." */

        जइ (!path)
                प्रत्यागम false;

        जइ (dot_or_dot_dot(path))
                प्रत्यागम true;

        प्रत्यागम ENDSWITH_SET(path, "/", "/.", "/..");
}

bool empty_or_मूलम्(ध्रुव वर्ण *path) {

        /* For operations relative to some root directory, returns true if the specified root directory is
         * redundant, i.e. either / or NULL or the empty string or any equivalent. */

        जइ (isempty(path))
                प्रत्यागम true;

        प्रत्यागम path_equal(path, "/");
}

ध्रुव वर्ण* empty_to_मूलम्(ध्रुव वर्ण *path) {
        प्रत्यागम isempty(path) ? "/" : path;
}

पूर्णाङ्क empty_or_मूलम्_harder_to_null(ध्रुव वर्ण **path) {
        पूर्णाङ्क r;

        प्रतिज्ञा(path);

        /* This nullifies the input path when the path is empty or points to "/". */

        जइ (empty_or_मूलम्(*path)) {
                *path = रिक्त;
                प्रत्यागम 0;
        }

        r = path_is_मूलम्(*path);
        जइ (r < 0)
                प्रत्यागम r;
        जइ (r > 0)
                *path = रिक्त;

        प्रत्यागम 0;
}

bool path_strv_contains(वर्ण * ध्रुव *l, ध्रुव वर्ण *path) {
        प्रतिज्ञा(path);

        STRV_FOREACH(i, l)
                जइ (path_equal(*i, path))
                        प्रत्यागम true;

        प्रत्यागम false;
}

bool prefixed_path_strv_contains(वर्ण * ध्रुव *l, ध्रुव वर्ण *path) {
        प्रतिज्ञा(path);

        STRV_FOREACH(i, l) {
                ध्रुव वर्ण *j = *i;

                जइ (*j == '-')
                        j++;
                जइ (*j == '+')
                        j++;

                जइ (path_equal(j, path))
                        प्रत्यागम true;
        }

        प्रत्यागम false;
}

पूर्णाङ्क path_glob_can_match(ध्रुव वर्ण *pattern, ध्रुव वर्ण *prefix, वर्ण **ret) {
        प्रतिज्ञा(pattern);
        प्रतिज्ञा(prefix);

        प्रति (ध्रुव वर्ण *a = pattern, *b = prefix;;) {
                _cleanup_free_ वर्ण *g = रिक्त, *h = रिक्त;
                ध्रुव वर्ण *p, *q;
                पूर्णाङ्क r, s;

                r = path_find_first_component(&a, /* accept_dot_dot= */ false, &p);
                जइ (r < 0)
                        प्रत्यागम r;

                s = path_find_first_component(&b, /* accept_dot_dot= */ false, &q);
                जइ (s < 0)
                        प्रत्यागम s;

                जइ (s == 0) {
                        /* The pattern matches the prefix. */
                        जइ (ret) {
                                वर्ण *t;

                                t = path_join(prefix, p);
                                जइ (!t)
                                        प्रत्यागम -ENOMEM;

                                *ret = t;
                        }
                        प्रत्यागम true;
                }

                जइ (r == 0)
                        भङ्ग;

                जइ (r == s && strneq(p, q, r))
                        अनुवर्त; /* common component. Check next. */

                g = strndup(p, r);
                जइ (!g)
                        प्रत्यागम -ENOMEM;

                जइ (!string_is_glob(g))
                        भङ्ग;

                /* We found a glob component. Check if the glob pattern matches the prefix component. */

                h = strndup(q, s);
                जइ (!h)
                        प्रत्यागम -ENOMEM;

                r = fnmatch(g, h, 0);
                जइ (r == FNM_NOMATCH)
                        भङ्ग;
                जइ (r != 0) /* Failure to process pattern? */
                        प्रत्यागम -EINVAL;
        }

        /* The pattern does not match the prefix. */
        जइ (ret)
                *ret = रिक्त;
        प्रत्यागम false;
}

#जइ HAVE_SPLIT_BIN
स्थिर bool dir_is_split(ध्रुव वर्ण *a, ध्रुव वर्ण *b) {
        पूर्णाङ्क r;

        r = inode_same(a, b, AT_NO_AUTOMOUNT);
        जइ (r < 0 && r != -ENOENT) {
                log_debug_errno(r, "Failed to compare \"%s\" and \"%s\", assuming split directories: %m", a, b);
                प्रत्यागम true;
        }
        प्रत्यागम r == 0;
}
#endif

ध्रुव वर्ण* default_PATH(शून्य) {
#जइ HAVE_SPLIT_BIN
        स्थिर ध्रुव वर्ण *default_path = रिक्त;

        /* Return one of the three sets of paths:
         * a) split /usr/s?bin, /usr/local/sbin doesn't matter.
         * b) merged /usr/s?bin, /usr/sbin is a symlink, but /usr/local/sbin is not,
         * c) fully merged, neither /usr/sbin nor /usr/local/sbin are symlinks,
         *
         * On error the fallback to the safe value with both directories as configured is returned.
         */

        जइ (default_path)
                प्रत्यागम default_path;

        जइ (dir_is_split("/usr/sbin", "/usr/bin"))
                प्रत्यागम (default_path = DEFAULT_PATH_WITH_FULL_SBIN);  /* a */
        जइ (dir_is_split("/usr/local/sbin", "/usr/local/bin"))
                प्रत्यागम (default_path = DEFAULT_PATH_WITH_LOCAL_SBIN); /* b */
        प्रत्यागम (default_path = DEFAULT_PATH_WITHOUT_SBIN);            /* c */
#अण्णहा
        प्रत्यागम DEFAULT_PATH_WITHOUT_SBIN;
#endif
}
