/* SPDX-License-Identifier: LGPL-2.1-or-later */

#અન્તર્ભાવ <fnmatch.h>
#અન્તર્ભાવ <stdio.h>
#અન્તર્ભાવ <stdlib.h>
#અન્તર્ભાવ <unistd.h>

#અન્તર્ભાવ "alloc-util.h"
#અન્તર્ભાવ "chase.h"
#અન્તર્ભાવ "errno-util.h"
#અન્તર્ભાવ "extract-word.h"
#અન્તર્ભાવ "fd-util.h"
#અન્તર્ભાવ "fs-util.h"
#અન્તર્ભાવ "glob-util.h"
#અન્તર્ભાવ "log.h"
#અન્તર્ભાવ "path-util.h"
#અન્તર્ભાવ "stat-util.h"
#અન્તર્ભાવ "string-util.h"
#અન્તર્ભાવ "strv.h"

bool is_path(અચલ અક્ષર *p) {
        જો (!p) /* A NULL pointer is definitely not a path */
                પાછા false;

        પાછા strchr(p, '/');
}

પૂર્ણાંક path_split_and_make_absolute(અચલ અક્ષર *p, અક્ષર ***ret) {
        _cleanup_strv_free_ અક્ષર **l = ખાલી;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(p);
        પ્રતિજ્ઞા(ret);

        l = strv_split(p, ":");
        જો (!l)
                પાછા -ENOMEM;

        r = path_strv_make_absolute_cwd(l);
        જો (r < 0)
                પાછા r;

        *ret = TAKE_PTR(l);
        પાછા r;
}

અક્ષર* path_make_absolute(અચલ અક્ષર *p, અચલ અક્ષર *prefix) {
        પ્રતિજ્ઞા(p);

        /* Makes every item in the list an absolute path by prepending
         * the prefix, if specified and necessary */

        જો (path_is_absolute(p) || isempty(prefix))
                પાછા strdup(p);

        પાછા path_join(prefix, p);
}

પૂર્ણાંક safe_getcwd(અક્ષર **ret) {
        _cleanup_free_ અક્ષર *cwd = ખાલી;

        cwd = get_current_dir_name();
        જો (!cwd)
                પાછા negative_errno();

        /* Let's make sure the directory is really absolute, to protect us from the logic behind
         * CVE-2018-1000001 */
        જો (cwd[0] != '/')
                પાછા -ENOMEDIUM;

        જો (ret)
                *ret = TAKE_PTR(cwd);

        પાછા 0;
}

પૂર્ણાંક path_make_absolute_cwd(અચલ અક્ષર *p, અક્ષર **ret) {
        અક્ષર *c;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(p);
        પ્રતિજ્ઞા(ret);

        /* Similar to path_make_absolute(), but prefixes with the
         * current working directory. */

        જો (path_is_absolute(p))
                c = strdup(p);
        નહીંતર {
                _cleanup_free_ અક્ષર *cwd = ખાલી;

                r = safe_getcwd(&cwd);
                જો (r < 0)
                        પાછા r;

                c = path_join(cwd, p);
        }
        જો (!c)
                પાછા -ENOMEM;

        *ret = c;
        પાછા 0;
}

પૂર્ણાંક path_make_relative(અચલ અક્ષર *from, અચલ અક્ષર *to, અક્ષર **ret) {
        _cleanup_free_ અક્ષર *result = ખાલી;
        unsigned n_parents;
        અચલ અક્ષર *f, *t;
        પૂર્ણાંક r, k;
        અક્ષર *p;

        પ્રતિજ્ઞા(from);
        પ્રતિજ્ઞા(to);
        પ્રતિજ્ઞા(ret);

        /* Strips the common part, and adds ".." elements as necessary. */

        જો (!path_is_absolute(from) || !path_is_absolute(to))
                પાછા -EINVAL;

        માટે (;;) {
                r = path_find_first_component(&from, true, &f);
                જો (r < 0)
                        પાછા r;

                k = path_find_first_component(&to, true, &t);
                જો (k < 0)
                        પાછા k;

                જો (r == 0) {
                        /* end of 'from' */
                        જો (k == 0) {
                                /* from and to are equivalent. */
                                result = strdup(".");
                                જો (!result)
                                        પાછા -ENOMEM;
                        } નહીંતર {
                                /* 'to' is inside of 'from'. */
                                r = path_simplify_alloc(t, &result);
                                જો (r < 0)
                                        પાછા r;

                                જો (!path_is_valid(result))
                                        પાછા -EINVAL;
                        }

                        *ret = TAKE_PTR(result);
                        પાછા 0;
                }

                જો (r != k || !strneq(f, t, r))
                        તોડો;
        }

        /* If we're here, then "from_dir" has one or more elements that need to
         * be replaced with "..". */

        માટે (n_parents = 1;; n_parents++) {
                /* If this includes ".." we can't do a simple series of "..". */
                r = path_find_first_component(&from, false, &f);
                જો (r < 0)
                        પાછા r;
                જો (r == 0)
                        તોડો;
        }

        જો (isempty(t) && n_parents * 3 > PATH_MAX)
                /* PATH_MAX is counted *with* the trailing NUL byte */
                પાછા -EINVAL;

        result = new(અક્ષર, n_parents * 3 + !isempty(t) + strlen_ptr(t));
        જો (!result)
                પાછા -ENOMEM;

        માટે (p = result; n_parents > 0; n_parents--)
                p = mempcpy(p, "../", 3);

        જો (isempty(t)) {
                /* Remove trailing slash and terminate string. */
                *(--p) = '\0';
                *ret = TAKE_PTR(result);
                પાછા 0;
        }

        strcpy(p, t);

        path_simplify(result);

        જો (!path_is_valid(result))
                પાછા -EINVAL;

        *ret = TAKE_PTR(result);
        પાછા 0;
}

પૂર્ણાંક path_make_relative_parent(અચલ અક્ષર *from_child, અચલ અક્ષર *to, અક્ષર **ret) {
        _cleanup_free_ અક્ષર *from = ખાલી;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(from_child);
        પ્રતિજ્ઞા(to);
        પ્રતિજ્ઞા(ret);

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
        જો (r < 0)
                પાછા r;

        પાછા path_make_relative(from, to, ret);
}

અક્ષર* path_startswith_strv(અચલ અક્ષર *p, અક્ષર * અચલ *strv) {
        પ્રતિજ્ઞા(p);

        STRV_FOREACH(s, strv) {
                અક્ષર *t;

                t = path_startswith(p, *s);
                જો (t)
                        પાછા t;
        }

        પાછા ખાલી;
}

પૂર્ણાંક path_strv_make_absolute_cwd(અક્ષર **l) {
        પૂર્ણાંક r;

        /* Goes through every item in the string list and makes it
         * absolute. This works in place and won't rollback any
         * changes on failure. */

        STRV_FOREACH(s, l) {
                અક્ષર *t;

                r = path_make_absolute_cwd(*s, &t);
                જો (r < 0)
                        પાછા r;

                path_simplify(t);
                free_and_replace(*s, t);
        }

        પાછા 0;
}

અક્ષર** path_strv_resolve(અક્ષર **l, અચલ અક્ષર *મૂળ) {
        unsigned k = 0;
        bool enomem = false;
        પૂર્ણાંક r;

        જો (strv_isempty(l))
                પાછા l;

        /* Go through every item in the string list and canonicalize the path.
         * This works in place and doesn't roll back any changes on failure. */

        STRV_FOREACH(s, l) {
                _cleanup_free_ અક્ષર *orig = ખાલી;
                અક્ષર *t, *u;

                જો (મૂળ) {
                        જો (!path_is_absolute(*s)) {
                                log_debug("Ignoring non-absolute search path '%s'.", *s);
                                મુક્ત(*s);
                                ચાલુ;
                        }

                        orig = *s;
                        t = path_join(મૂળ, orig);
                        જો (!t) {
                                enomem = true;
                                ચાલુ;
                        }
                } નહીંતર
                        t = *s;

                r = chase(t, મૂળ, 0, &u, ખાલી);
                જો (r == -ENOENT) {
                        જો (મૂળ) {
                                u = TAKE_PTR(orig);
                                મુક્ત(t);
                        } નહીંતર
                                u = t;
                } નહીંતર જો (r < 0) {
                        મુક્ત(t);

                        જો (r == -ENOMEM)
                                enomem = true;

                        ચાલુ;
                } નહીંતર જો (મૂળ) {
                        અક્ષર *x;

                        મુક્ત(t);
                        x = path_startswith(u, મૂળ);
                        જો (x) {
                                /* restore the slash if it was lost */
                                જો (!startswith(x, "/"))
                                        *(--x) = '/';

                                t = strdup(x);
                                મુક્ત(u);
                                જો (!t) {
                                        enomem = true;
                                        ચાલુ;
                                }
                                u = t;
                        } નહીંતર {
                                /* canonicalized path goes outside of
                                 * prefix, keep the original path instead */
                                free_and_replace(u, orig);
                        }
                } નહીંતર
                        મુક્ત(t);

                l[k++] = u;
        }

        l[k] = ખાલી;

        જો (enomem)
                પાછા ખાલી;

        પાછા l;
}

અક્ષર** path_strv_resolve_uniq(અક્ષર **l, અચલ અક્ષર *મૂળ) {

        જો (strv_isempty(l))
                પાછા l;

        જો (!path_strv_resolve(l, મૂળ))
                પાછા ખાલી;

        પાછા strv_uniq(l);
}

અક્ષર* skip_leading_slash(અચલ અક્ષર *p) {
        પાછા skip_leading_chars(p, "/");
}

અક્ષર* path_simplify_full(અક્ષર *path, PathSimplifyFlags flags) {
        bool add_slash = false, keep_trailing_slash, absolute, beginning = true;
        અક્ષર *f = path;
        પૂર્ણાંક r;

        /* Removes redundant inner and trailing slashes. Also removes unnecessary dots.
         * Modifies the passed string in-place.
         *
         * ///foo//./bar/.   becomes /foo/bar
         * .//./foo//./bar/. becomes foo/bar
         * /../foo/bar       becomes /foo/bar
         * /../foo/bar/..    becomes /foo/bar/..
         */

        જો (isempty(path))
                પાછા path;

        keep_trailing_slash = FLAGS_SET(flags, PATH_SIMPLIFY_KEEP_TRAILING_SLASH) && endswith(path, "/");

        absolute = path_is_absolute(path);
        f += absolute;  /* Keep leading /, if present. */

        માટે (અચલ અક્ષર *p = f;;) {
                અચલ અક્ષર *e;

                r = path_find_first_component(&p, true, &e);
                જો (r == 0)
                        તોડો;

                જો (r > 0 && absolute && beginning && path_startswith(e, ".."))
                        /* If we're at the beginning of an absolute path, we can safely skip ".." */
                        ચાલુ;

                beginning = false;

                જો (add_slash)
                        *f++ = '/';

                જો (r < 0) {
                        /* if path is invalid, then refuse to simplify the remaining part. */
                        memmove(f, p, strlen(p) + 1);
                        પાછા path;
                }

                memmove(f, e, r);
                f += r;

                add_slash = true;
        }

        /* Special rule, if we stripped everything, we need a "." for the current directory. */
        જો (f == path)
                *f++ = '.';

        જો (*(f-1) != '/' && keep_trailing_slash)
                *f++ = '/';

        *f = '\0';
        પાછા path;
}

પૂર્ણાંક path_simplify_alloc(અચલ અક્ષર *path, અક્ષર **ret) {
        પ્રતિજ્ઞા(ret);

        જો (!path) {
                *ret = ખાલી;
                પાછા 0;
        }

        અક્ષર *t = strdup(path);
        જો (!t)
                પાછા -ENOMEM;

        *ret = path_simplify(t);
        પાછા 0;
}

અક્ષર* path_startswith_full(અચલ અક્ષર *original_path, અચલ અક્ષર *prefix, PathStartWithFlags flags) {
        પ્રતિજ્ઞા(original_path);
        પ્રતિજ્ઞા(prefix);

        /* Returns a pointer to the start of the first component after the parts matched by
         * the prefix, iff
         * - both paths are absolute or both paths are relative,
         * and
         * - each component in prefix in turn matches a component in path at the same position.
         * An empty string will be returned when the prefix and path are equivalent.
         *
         * Returns NULL otherwise.
         */

        અચલ અક્ષર *path = original_path;

        જો ((path[0] == '/') != (prefix[0] == '/'))
                પાછા ખાલી;

        માટે (;;) {
                અચલ અક્ષર *p, *q;
                પૂર્ણાંક m, n;

                m = path_find_first_component(&path, !FLAGS_SET(flags, PATH_STARTSWITH_REFUSE_DOT_DOT), &p);
                જો (m < 0)
                        પાછા ખાલી;

                n = path_find_first_component(&prefix, !FLAGS_SET(flags, PATH_STARTSWITH_REFUSE_DOT_DOT), &q);
                જો (n < 0)
                        પાછા ખાલી;

                જો (n == 0) {
                        જો (!p)
                                p = path;

                        જો (FLAGS_SET(flags, PATH_STARTSWITH_RETURN_LEADING_SLASH)) {

                                જો (p <= original_path)
                                        પાછા ખાલી;

                                p--;

                                જો (*p != '/')
                                        પાછા ખાલી;
                        }

                        પાછા (અક્ષર*) p;
                }

                જો (m != n)
                        પાછા ખાલી;

                જો (!strneq(p, q, m))
                        પાછા ખાલી;
        }
}

પૂર્ણાંક path_compare(અચલ અક્ષર *a, અચલ અક્ષર *b) {
        પૂર્ણાંક r;

        /* Order NULL before non-NULL */
        r = CMP(!!a, !!b);
        જો (r != 0)
                પાછા r;

        /* A relative path and an absolute path must not compare as equal.
         * Which one is sorted before the other does not really matter.
         * Here a relative path is ordered before an absolute path. */
        r = CMP(path_is_absolute(a), path_is_absolute(b));
        જો (r != 0)
                પાછા r;

        માટે (;;) {
                અચલ અક્ષર *aa, *bb;
                પૂર્ણાંક j, k;

                j = path_find_first_component(&a, true, &aa);
                k = path_find_first_component(&b, true, &bb);

                જો (j < 0 || k < 0) {
                        /* When one of paths is invalid, order invalid path after valid one. */
                        r = CMP(j < 0, k < 0);
                        જો (r != 0)
                                પાછા r;

                        /* fallback to use strcmp() if both paths are invalid. */
                        પાછા strcmp(a, b);
                }

                /* Order prefixes first: "/foo" before "/foo/bar" */
                જો (j == 0) {
                        જો (k == 0)
                                પાછા 0;
                        પાછા -1;
                }
                જો (k == 0)
                        પાછા 1;

                /* Alphabetical sort: "/foo/aaa" before "/foo/b" */
                r = memcmp(aa, bb, MIN(j, k));
                જો (r != 0)
                        પાછા r;

                /* Sort "/foo/a" before "/foo/aaa" */
                r = CMP(j, k);
                જો (r != 0)
                        પાછા r;
        }
}

પૂર્ણાંક path_compare_filename(અચલ અક્ષર *a, અચલ અક્ષર *b) {
        _cleanup_free_ અક્ષર *fa = ખાલી, *fb = ખાલી;
        પૂર્ણાંક r, j, k;

        /* Order NULL before non-NULL */
        r = CMP(!!a, !!b);
        જો (r != 0)
                પાછા r;

        j = path_extract_filename(a, &fa);
        k = path_extract_filename(b, &fb);

        /* When one of paths is "." or root, then order it earlier. */
        r = CMP(j != -EADDRNOTAVAIL, k != -EADDRNOTAVAIL);
        જો (r != 0)
                પાછા r;

        /* When one of paths is invalid (or we get OOM), order invalid path after valid one. */
        r = CMP(j < 0, k < 0);
        જો (r != 0)
                પાછા r;

        /* fallback to use strcmp() if both paths are invalid. */
        જો (j < 0)
                પાછા strcmp(a, b);

        પાછા strcmp(fa, fb);
}

પૂર્ણાંક path_equal_or_inode_same_full(અચલ અક્ષર *a, અચલ અક્ષર *b, પૂર્ણાંક flags) {
        /* Returns true if paths are of the same entry, false if not, <0 on error. */

        જો (path_equal(a, b))
                પાછા 1;

        જો (!a || !b)
                પાછા 0;

        પાછા inode_same(a, b, flags);
}

અક્ષર* path_extend_internal(અક્ષર **x, ...) {
        size_t sz, old_sz;
        અક્ષર *q, *nx;
        અચલ અક્ષર *p;
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
        જ્યારે ((p = va_arg(ap, અક્ષર*)) != POINTER_MAX) {
                size_t add;

                જો (isempty(p))
                        ચાલુ;

                add = 1 + strlen(p);
                જો (sz > SIZE_MAX - add) { /* overflow check */
                        va_end(ap);
                        પાછા ખાલી;
                }

                sz += add;
        }
        va_end(ap);

        nx = realloc(x ? *x : ખાલી, GREEDY_ALLOC_ROUND_UP(sz+1));
        જો (!nx)
                પાછા ખાલી;
        જો (x)
                *x = nx;

        જો (old_sz > 0)
                slash = nx[old_sz-1] == '/';
        નહીંતર {
                nx[old_sz] = 0;
                slash = true; /* no need to generate a slash anymore */
        }

        q = nx + old_sz;

        va_start(ap, x);
        જ્યારે ((p = va_arg(ap, અક્ષર*)) != POINTER_MAX) {
                જો (isempty(p))
                        ચાલુ;

                જો (!slash && p[0] != '/')
                        *(q++) = '/';

                q = stpcpy(q, p);
                slash = endswith(p, "/");
        }
        va_end(ap);

        પાછા nx;
}

પૂર્ણાંક open_and_check_executable(અચલ અક્ષર *name, અચલ અક્ષર *મૂળ, અક્ષર **ret_path, પૂર્ણાંક *ret_fd) {
        _cleanup_close_ પૂર્ણાંક fd = -EBADF;
        _cleanup_free_ અક્ષર *resolved = ખાલી;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(name);

        /* Function chase() is invoked only when root is not NULL, as using it regardless of
         * root value would alter the behavior of existing callers for example: /bin/sleep would become
         * /usr/bin/sleep when find_executables is called. Hence, this function should be invoked when
         * needed to avoid unforeseen regression or other complicated changes. */
        જો (મૂળ) {
                /* prefix root to name in case full paths are not specified */
                r = chase(name, મૂળ, CHASE_PREFIX_મૂળ, &resolved, &fd);
                જો (r < 0)
                        પાછા r;

                name = resolved;
        } નહીંતર {
                /* We need to use O_PATH because there may be executables for which we have only exec permissions,
                 * but not read (usually suid executables). */
                fd = open(name, O_PATH|O_CLOEXEC);
                જો (fd < 0)
                        પાછા -errno;
        }

        r = fd_verify_regular(fd);
        જો (r < 0)
                પાછા r;

        r = access_fd(fd, X_OK);
        જો (r == -ENOSYS)
                /* /proc/ is not mounted. Fall back to access(). */
                r = RET_NERRNO(access(name, X_OK));
        જો (r < 0)
                પાછા r;

        જો (ret_path) {
                જો (resolved)
                        *ret_path = TAKE_PTR(resolved);
                નહીંતર {
                        r = path_make_absolute_cwd(name, ret_path);
                        જો (r < 0)
                                પાછા r;

                        path_simplify(*ret_path);
                }
        }

        જો (ret_fd)
                *ret_fd = TAKE_FD(fd);

        પાછા 0;
}

પૂર્ણાંક find_executable_full(
                અચલ અક્ષર *name,
                અચલ અક્ષર *મૂળ,
                અક્ષર * અચલ *exec_શોધ_path,
                bool use_path_envvar,
                અક્ષર **ret_filename,
                પૂર્ણાંક *ret_fd) {

        પૂર્ણાંક last_error = -ENOENT, r = 0;

        પ્રતિજ્ઞા(name);

        જો (is_path(name))
                પાછા open_and_check_executable(name, મૂળ, ret_filename, ret_fd);

        જો (exec_શોધ_path) {
                STRV_FOREACH(element, exec_શોધ_path) {
                        _cleanup_free_ અક્ષર *full_path = ખાલી;

                        જો (!path_is_absolute(*element)) {
                                log_debug("Exec search path '%s' isn't absolute, ignoring.", *element);
                                ચાલુ;
                        }

                        full_path = path_join(*element, name);
                        જો (!full_path)
                                પાછા -ENOMEM;

                        r = open_and_check_executable(full_path, મૂળ, ret_filename, ret_fd);
                        જો (r >= 0)
                                પાછા 0;
                        જો (r != -EACCES)
                                last_error = r;
                }
                પાછા last_error;
        }

        અચલ અક્ષર *p = ખાલી;

        જો (use_path_envvar)
                /* Plain getenv, not secure_getenv, because we want to actually allow the user to pick the
                 * binary. */
                p = getenv("PATH");
        જો (!p)
                p = default_PATH();

        /* Resolve a single-component name to a full path */
        માટે (;;) {
                _cleanup_free_ અક્ષર *element = ખાલી;

                r = extract_first_word(&p, &element, ":", EXTRACT_RELAX|EXTRACT_DONT_COALESCE_SEPARATORS);
                જો (r < 0)
                        પાછા r;
                જો (r == 0)
                        તોડો;

                જો (!path_is_absolute(element)) {
                        log_debug("Exec search path '%s' isn't absolute, ignoring.", element);
                        ચાલુ;
                }

                જો (!path_extend(&element, name))
                        પાછા -ENOMEM;

                r = open_and_check_executable(element, મૂળ, ret_filename, ret_fd);
                જો (r >= 0) /* Found it! */
                        પાછા 0;
                /* PATH entries which we don't have access to are ignored, as per tradition. */
                જો (r != -EACCES)
                        last_error = r;
        }

        પાછા last_error;
}

સ્થિર પૂર્ણાંક executable_is_good(અચલ અક્ષર *executable) {
        _cleanup_free_ અક્ષર *p = ખાલી, *d = ખાલી;
        પૂર્ણાંક r;

        r = find_executable(executable, &p);
        જો (r == -ENOENT)
                પાછા 0;
        જો (r < 0)
                પાછા r;

        /* An fsck that is linked to /bin/true is a non-existent fsck */

        r = readlink_malloc(p, &d);
        જો (r == -EINVAL) /* not a symlink */
                પાછા 1;
        જો (r < 0)
                પાછા r;

        પાછા !PATH_IN_SET(d, "true",
                               "/bin/true",
                               "/usr/bin/true",
                               "/dev/null");
}

પૂર્ણાંક fsck_exists(શૂન્ય) {
        પાછા executable_is_good("fsck");
}

પૂર્ણાંક fsck_exists_for_fstype(અચલ અક્ષર *fstype) {
        અચલ અક્ષર *checker;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(fstype);

        જો (streq(fstype, "auto"))
                પાછા -EINVAL;

        r = fsck_exists();
        જો (r <= 0)
                પાછા r;

        checker = strjoina("fsck.", fstype);
        પાછા executable_is_good(checker);
}

સ્થિર અચલ અક્ષર* skip_slash_or_dot(અચલ અક્ષર *p) {
        POINTER_MAY_BE_NULL(p);

        માટે (; !isempty(p); p++) {
                જો (*p == '/')
                        ચાલુ;
                જો (startswith(p, "./")) {
                        p++;
                        ચાલુ;
                }
                તોડો;
        }
        પાછા p;
}

પૂર્ણાંક path_find_first_component(અચલ અક્ષર **p, bool accept_dot_dot, અચલ અક્ષર **ret) {
        અચલ અક્ષર *q, *first, *end_first, *next;
        size_t len;

        પ્રતિજ્ઞા(p);

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
        જો (isempty(first)) {
                *p = first;
                જો (ret)
                        *ret = ખાલી;
                પાછા 0;
        }
        જો (streq(first, ".")) {
                *p = first + 1;
                જો (ret)
                        *ret = ખાલી;
                પાછા 0;
        }

        end_first = strchrnul(first, '/');
        len = end_first - first;

        જો (len > NAME_MAX)
                પાછા -EINVAL;
        જો (!accept_dot_dot && len == 2 && first[0] == '.' && first[1] == '.')
                પાછા -EINVAL;

        next = skip_slash_or_dot(end_first);

        *p = next + streq(next, ".");
        જો (ret)
                *ret = first;
        પાછા len;
}

સ્થિર અચલ અક્ષર* skip_slash_or_dot_backward(અચલ અક્ષર *path, અચલ અક્ષર *q) {
        પ્રતિજ્ઞા(path);
        પ્રતિજ્ઞા(!q || q >= path);

        માટે (; q; q = PTR_SUB1(q, path)) {
                જો (*q == '/')
                        ચાલુ;
                જો (q > path && strneq(q - 1, "/.", 2))
                        ચાલુ;
                જો (q == path && *q == '.')
                        ચાલુ;
                તોડો;
        }
        પાછા q;
}

પૂર્ણાંક path_find_last_component(અચલ અક્ષર *path, bool accept_dot_dot, અચલ અક્ષર **next, અચલ અક્ષર **ret) {
        અચલ અક્ષર *q, *last_end, *last_begin;
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

        જો (isempty(path)) {
                જો (next)
                        *next = path;
                જો (ret)
                        *ret = ખાલી;
                પાછા 0;
        }

        જો (next && *next) {
                જો (*next < path || *next > path + strlen(path))
                        પાછા -EINVAL;
                જો (*next == path) {
                        જો (ret)
                                *ret = ખાલી;
                        પાછા 0;
                }
                જો (!IN_SET(**next, '\0', '/'))
                        પાછા -EINVAL;
                q = *next - 1;
        } નહીંતર
                q = path + strlen(path) - 1;

        q = skip_slash_or_dot_backward(path, q);
        જો (!q) { /* the root directory, "." or "./" */
                જો (next)
                        *next = path;
                જો (ret)
                        *ret = ખાલી;
                પાછા 0;
        }

        last_end = q + 1;

        જ્યારે (q && *q != '/')
                q = PTR_SUB1(q, path);

        last_begin = q ? q + 1 : path;
        len = last_end - last_begin;

        જો (len > NAME_MAX)
                પાછા -EINVAL;
        જો (!accept_dot_dot && len == 2 && strneq(last_begin, "..", 2))
                પાછા -EINVAL;

        જો (next) {
                q = skip_slash_or_dot_backward(path, q);
                *next = q ? q + 1 : path;
        }

        જો (ret)
                *ret = last_begin;
        પાછા len;
}

અચલ અક્ષર* last_path_component(અચલ અક્ષર *path) {

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

        જો (!path)
                પાછા ખાલી;

        l = k = strlen(path);
        જો (l == 0) /* special case — an empty string */
                પાછા path;

        જ્યારે (k > 0 && path[k-1] == '/')
                k--;

        જો (k == 0) /* the root directory */
                પાછા path + l - 1;

        જ્યારે (k > 0 && path[k-1] != '/')
                k--;

        પાછા path + k;
}

પૂર્ણાંક path_split_prefix_filename(અચલ અક્ષર *path, અક્ષર **ret_dir, અક્ષર **ret_filename) {
        _cleanup_free_ અક્ષર *d = ખાલી;
        અચલ અક્ષર *c, *next = ખાલી;
        પૂર્ણાંક r;

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

        જો (isempty(path))
                પાછા -EINVAL;

        r = path_find_last_component(path, /* accept_dot_dot = */ false, &next, &c);
        જો (r < 0)
                પાછા r;
        જો (r == 0) /* root directory or "." */
                પાછા -EADDRNOTAVAIL;

        જો (ret_dir) {
                જો (next == path) {
                        જો (*path != '/') { /* filename only */
                                જો (!ret_filename)
                                        પાછા -EDESTADDRREQ;
                        } નહીંતર {
                                d = strdup("/");
                                જો (!d)
                                        પાછા -ENOMEM;
                        }
                } નહીંતર {
                        d = strndup(path, next - path);
                        જો (!d)
                                પાછા -ENOMEM;

                        path_simplify(d);

                        જો (!path_is_valid(d))
                                પાછા -EINVAL;
                }

        } નહીંતર જો (!path_is_valid(path))
                /* We didn't validate the dir prefix, hence check if the whole path is valid now */
                પાછા -EINVAL;

        જો (ret_filename) {
                અક્ષર *fn = strndup(c, r);
                જો (!fn)
                        પાછા -ENOMEM;

                *ret_filename = fn;
        }

        જો (ret_dir)
                *ret_dir = TAKE_PTR(d);

        પાછા strlen(c) > (size_t) r ? O_DIRECTORY : 0;
}

bool filename_part_is_valid(અચલ અક્ષર *p) {
        અચલ અક્ષર *e;

        /* Checks f the specified string is OK to be *part* of a filename. This is different from
         * filename_is_valid() as "." and ".." and "" are OK by this call, but not by filename_is_valid(). */

        જો (!p)
                પાછા false;

        e = strchrnul(p, '/');
        જો (*e != 0)
                પાછા false;

        જો (e - p > NAME_MAX) /* NAME_MAX is counted *without* the trailing NUL byte */
                પાછા false;

        પાછા true;
}

bool filename_is_valid(અચલ અક્ષર *p) {

        જો (isempty(p))
                પાછા false;

        જો (dot_or_dot_dot(p)) /* Yes, in this context we consider "." and ".." invalid */
                પાછા false;

        પાછા filename_part_is_valid(p);
}

bool path_is_valid_full(અચલ અક્ષર *p, bool accept_dot_dot) {
        જો (isempty(p))
                પાછા false;

        માટે (અચલ અક્ષર *e = p;;) {
                પૂર્ણાંક r;

                r = path_find_first_component(&e, accept_dot_dot, ખાલી);
                જો (r < 0)
                        પાછા false;

                જો (e - p >= PATH_MAX) /* Already reached the maximum length for a path? (PATH_MAX is counted
                                        * *with* the trailing NUL byte) */
                        પાછા false;
                જો (*e == 0)           /* End of string? Yay! */
                        પાછા true;
        }
}

bool path_is_normalized(અચલ અક્ષર *p) {
        જો (!path_is_safe(p))
                પાછા false;

        જો (streq(p, ".") || startswith(p, "./") || endswith(p, "/.") || strstr(p, "/./"))
                પાછા false;

        જો (strstr(p, "//"))
                પાછા false;

        પાછા true;
}

પૂર્ણાંક file_in_same_dir(અચલ અક્ષર *path, અચલ અક્ષર *filename, અક્ષર **ret) {
        _cleanup_free_ અક્ષર *b = ખાલી;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(path);
        પ્રતિજ્ઞા(filename);
        પ્રતિજ્ઞા(ret);

        /* This removes the last component of path and appends filename, unless the latter is absolute anyway
         * or the former isn't */

        જો (path_is_absolute(filename))
                b = strdup(filename);
        નહીંતર {
                _cleanup_free_ અક્ષર *dn = ખાલી;

                r = path_extract_directory(path, &dn);
                જો (r == -EDESTADDRREQ) /* no path prefix */
                        b = strdup(filename);
                નહીંતર જો (r < 0)
                        પાછા r;
                નહીંતર
                        b = path_join(dn, filename);
        }
        જો (!b)
                પાછા -ENOMEM;

        *ret = TAKE_PTR(b);
        પાછા 0;
}

bool hidden_or_backup_file(અચલ અક્ષર *filename) {
        પ્રતિજ્ઞા(filename);

        જો (filename[0] == '.' ||
            STR_IN_SET(filename,
                       "lost+found",
                       "aquota.user",
                       "aquota.group") ||
            endswith(filename, "~"))
                પાછા true;

        અચલ અક્ષર *dot = strrchr(filename, '.');
        જો (!dot)
                પાછા false;

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

        પાછા STR_IN_SET(dot + 1,
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

bool is_device_path(અચલ અક્ષર *path) {

        /* Returns true for paths that likely refer to a device, either by path in sysfs or to something in
         * /dev. This accepts any path that starts with /dev/ or /sys/ and has something after that prefix.
         * It does not actually resolve the path.
         *
         * Examples:
         * /dev/sda, /dev/sda/foo, /sys/class, /dev/.., /sys/.., /./dev/foo → yes.
         * /../dev/sda, /dev, /sys, /usr/path, /usr/../dev/sda → no.
         */

        અચલ અક્ષર *p = PATH_STARTSWITH_SET(ASSERT_PTR(path), "/dev/", "/sys/");
        પાછા !isempty(p);
}

bool valid_device_ગાંઠ_path(અચલ અક્ષર *path) {

        /* Some superficial checks whether the specified path is a valid device node path, all without
         * looking at the actual device node. */

        જો (!PATH_STARTSWITH_SET(path, "/dev/", "/run/systemd/inaccessible/"))
                પાછા false;

        જો (endswith(path, "/")) /* can't be a device node if it ends in a slash */
                પાછા false;

        પાછા path_is_normalized(path);
}

bool valid_device_allow_pattern(અચલ અક્ષર *path) {
        પ્રતિજ્ઞા(path);

        /* Like valid_device_node_path(), but also allows full-subsystem expressions like those accepted by
         * DeviceAllow= and DeviceDeny=. */

        જો (STARTSWITH_SET(path, "block-", "char-"))
                પાછા true;

        પાછા valid_device_ગાંઠ_path(path);
}

bool dot_or_dot_dot(અચલ અક્ષર *path) {
        જો (!path)
                પાછા false;
        જો (path[0] != '.')
                પાછા false;
        જો (path[1] == 0)
                પાછા true;
        જો (path[1] != '.')
                પાછા false;

        પાછા path[2] == 0;
}

bool path_implies_directory(અચલ અક્ષર *path) {

        /* Sometimes, if we look at a path we already know it must refer to a directory, because it is
         * suffixed with a slash, or its last component is "." or ".." */

        જો (!path)
                પાછા false;

        જો (dot_or_dot_dot(path))
                પાછા true;

        પાછા ENDSWITH_SET(path, "/", "/.", "/..");
}

bool empty_or_મૂળ(અચલ અક્ષર *path) {

        /* For operations relative to some root directory, returns true if the specified root directory is
         * redundant, i.e. either / or NULL or the empty string or any equivalent. */

        જો (isempty(path))
                પાછા true;

        પાછા path_equal(path, "/");
}

અચલ અક્ષર* empty_to_મૂળ(અચલ અક્ષર *path) {
        પાછા isempty(path) ? "/" : path;
}

પૂર્ણાંક empty_or_મૂળ_harder_to_null(અચલ અક્ષર **path) {
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(path);

        /* This nullifies the input path when the path is empty or points to "/". */

        જો (empty_or_મૂળ(*path)) {
                *path = ખાલી;
                પાછા 0;
        }

        r = path_is_મૂળ(*path);
        જો (r < 0)
                પાછા r;
        જો (r > 0)
                *path = ખાલી;

        પાછા 0;
}

bool path_strv_contains(અક્ષર * અચલ *l, અચલ અક્ષર *path) {
        પ્રતિજ્ઞા(path);

        STRV_FOREACH(i, l)
                જો (path_equal(*i, path))
                        પાછા true;

        પાછા false;
}

bool prefixed_path_strv_contains(અક્ષર * અચલ *l, અચલ અક્ષર *path) {
        પ્રતિજ્ઞા(path);

        STRV_FOREACH(i, l) {
                અચલ અક્ષર *j = *i;

                જો (*j == '-')
                        j++;
                જો (*j == '+')
                        j++;

                જો (path_equal(j, path))
                        પાછા true;
        }

        પાછા false;
}

પૂર્ણાંક path_glob_can_match(અચલ અક્ષર *pattern, અચલ અક્ષર *prefix, અક્ષર **ret) {
        પ્રતિજ્ઞા(pattern);
        પ્રતિજ્ઞા(prefix);

        માટે (અચલ અક્ષર *a = pattern, *b = prefix;;) {
                _cleanup_free_ અક્ષર *g = ખાલી, *h = ખાલી;
                અચલ અક્ષર *p, *q;
                પૂર્ણાંક r, s;

                r = path_find_first_component(&a, /* accept_dot_dot= */ false, &p);
                જો (r < 0)
                        પાછા r;

                s = path_find_first_component(&b, /* accept_dot_dot= */ false, &q);
                જો (s < 0)
                        પાછા s;

                જો (s == 0) {
                        /* The pattern matches the prefix. */
                        જો (ret) {
                                અક્ષર *t;

                                t = path_join(prefix, p);
                                જો (!t)
                                        પાછા -ENOMEM;

                                *ret = t;
                        }
                        પાછા true;
                }

                જો (r == 0)
                        તોડો;

                જો (r == s && strneq(p, q, r))
                        ચાલુ; /* common component. Check next. */

                g = strndup(p, r);
                જો (!g)
                        પાછા -ENOMEM;

                જો (!string_is_glob(g))
                        તોડો;

                /* We found a glob component. Check if the glob pattern matches the prefix component. */

                h = strndup(q, s);
                જો (!h)
                        પાછા -ENOMEM;

                r = fnmatch(g, h, 0);
                જો (r == FNM_NOMATCH)
                        તોડો;
                જો (r != 0) /* Failure to process pattern? */
                        પાછા -EINVAL;
        }

        /* The pattern does not match the prefix. */
        જો (ret)
                *ret = ખાલી;
        પાછા false;
}

#જો HAVE_SPLIT_BIN
સ્થિર bool dir_is_split(અચલ અક્ષર *a, અચલ અક્ષર *b) {
        પૂર્ણાંક r;

        r = inode_same(a, b, AT_NO_AUTOMOUNT);
        જો (r < 0 && r != -ENOENT) {
                log_debug_errno(r, "Failed to compare \"%s\" and \"%s\", assuming split directories: %m", a, b);
                પાછા true;
        }
        પાછા r == 0;
}
#endif

અચલ અક્ષર* default_PATH(શૂન્ય) {
#જો HAVE_SPLIT_BIN
        સ્થિર અચલ અક્ષર *default_path = ખાલી;

        /* Return one of the three sets of paths:
         * a) split /usr/s?bin, /usr/local/sbin doesn't matter.
         * b) merged /usr/s?bin, /usr/sbin is a symlink, but /usr/local/sbin is not,
         * c) fully merged, neither /usr/sbin nor /usr/local/sbin are symlinks,
         *
         * On error the fallback to the safe value with both directories as configured is returned.
         */

        જો (default_path)
                પાછા default_path;

        જો (dir_is_split("/usr/sbin", "/usr/bin"))
                પાછા (default_path = DEFAULT_PATH_WITH_FULL_SBIN);  /* a */
        જો (dir_is_split("/usr/local/sbin", "/usr/local/bin"))
                પાછા (default_path = DEFAULT_PATH_WITH_LOCAL_SBIN); /* b */
        પાછા (default_path = DEFAULT_PATH_WITHOUT_SBIN);            /* c */
#નહીંતર
        પાછા DEFAULT_PATH_WITHOUT_SBIN;
#endif
}
