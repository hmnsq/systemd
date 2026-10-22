/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن <fnmatch.h>
#ضمن <stdio.h>
#ضمن <stdlib.h>
#ضمن <unistd.h>

#ضمن "alloc-util.h"
#ضمن "chase.h"
#ضمن "errno-util.h"
#ضمن "extract-word.h"
#ضمن "fd-util.h"
#ضمن "fs-util.h"
#ضمن "glob-util.h"
#ضمن "log.h"
#ضمن "path-util.h"
#ضمن "stat-util.h"
#ضمن "string-util.h"
#ضمن "strv.h"

bool is_path(ثابت حرف *p) {
        אם (!p) /* A NULL pointer is definitely not a path */
                החזר false;

        החזר strchr(p, '/');
}

صحيح path_split_and_make_absolute(ثابت حرف *p, حرف ***ret) {
        _cleanup_strv_free_ حرف **l = عدم;
        صحيح r;

        أكد(p);
        أكد(ret);

        l = strv_split(p, ":");
        אם (!l)
                החזר -ENOMEM;

        r = path_strv_make_absolute_cwd(l);
        אם (r < 0)
                החזר r;

        *ret = TAKE_PTR(l);
        החזר r;
}

حرف* path_make_absolute(ثابت حرف *p, ثابت حرف *prefix) {
        أكد(p);

        /* Makes every item in the list an absolute path by prepending
         * the prefix, if specified and necessary */

        אם (path_is_absolute(p) || isempty(prefix))
                החזר strdup(p);

        החזר path_join(prefix, p);
}

صحيح safe_getcwd(حرف **ret) {
        _cleanup_free_ حرف *cwd = عدم;

        cwd = get_current_dir_name();
        אם (!cwd)
                החזר negative_errno();

        /* Let's make sure the directory is really absolute, to protect us from the logic behind
         * CVE-2018-1000001 */
        אם (cwd[0] != '/')
                החזר -ENOMEDIUM;

        אם (ret)
                *ret = TAKE_PTR(cwd);

        החזר 0;
}

صحيح path_make_absolute_cwd(ثابت حرف *p, حرف **ret) {
        حرف *c;
        صحيح r;

        أكد(p);
        أكد(ret);

        /* Similar to path_make_absolute(), but prefixes with the
         * current working directory. */

        אם (path_is_absolute(p))
                c = strdup(p);
        אחרת {
                _cleanup_free_ حرف *cwd = عدم;

                r = safe_getcwd(&cwd);
                אם (r < 0)
                        החזר r;

                c = path_join(cwd, p);
        }
        אם (!c)
                החזר -ENOMEM;

        *ret = c;
        החזר 0;
}

صحيح path_make_relative(ثابت حرف *from, ثابت حرف *to, حرف **ret) {
        _cleanup_free_ حرف *result = عدم;
        unsigned n_parents;
        ثابت حرف *f, *t;
        صحيح r, k;
        حرف *p;

        أكد(from);
        أكد(to);
        أكد(ret);

        /* Strips the common part, and adds ".." elements as necessary. */

        אם (!path_is_absolute(from) || !path_is_absolute(to))
                החזר -EINVAL;

        עבור (;;) {
                r = path_find_first_component(&from, true, &f);
                אם (r < 0)
                        החזר r;

                k = path_find_first_component(&to, true, &t);
                אם (k < 0)
                        החזר k;

                אם (r == 0) {
                        /* end of 'from' */
                        אם (k == 0) {
                                /* from and to are equivalent. */
                                result = strdup(".");
                                אם (!result)
                                        החזר -ENOMEM;
                        } אחרת {
                                /* 'to' is inside of 'from'. */
                                r = path_simplify_alloc(t, &result);
                                אם (r < 0)
                                        החזר r;

                                אם (!path_is_valid(result))
                                        החזר -EINVAL;
                        }

                        *ret = TAKE_PTR(result);
                        החזר 0;
                }

                אם (r != k || !strneq(f, t, r))
                        اكسر;
        }

        /* If we're here, then "from_dir" has one or more elements that need to
         * be replaced with "..". */

        עבור (n_parents = 1;; n_parents++) {
                /* If this includes ".." we can't do a simple series of "..". */
                r = path_find_first_component(&from, false, &f);
                אם (r < 0)
                        החזר r;
                אם (r == 0)
                        اكسر;
        }

        אם (isempty(t) && n_parents * 3 > PATH_MAX)
                /* PATH_MAX is counted *with* the trailing NUL byte */
                החזר -EINVAL;

        result = new(حرف, n_parents * 3 + !isempty(t) + strlen_ptr(t));
        אם (!result)
                החזר -ENOMEM;

        עבור (p = result; n_parents > 0; n_parents--)
                p = mempcpy(p, "../", 3);

        אם (isempty(t)) {
                /* Remove trailing slash and terminate string. */
                *(--p) = '\0';
                *ret = TAKE_PTR(result);
                החזר 0;
        }

        strcpy(p, t);

        path_simplify(result);

        אם (!path_is_valid(result))
                החזר -EINVAL;

        *ret = TAKE_PTR(result);
        החזר 0;
}

صحيح path_make_relative_parent(ثابت حرف *from_child, ثابت حرف *to, حرف **ret) {
        _cleanup_free_ حرف *from = عدم;
        صحيح r;

        أكد(from_child);
        أكد(to);
        أكد(ret);

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
        אם (r < 0)
                החזר r;

        החזר path_make_relative(from, to, ret);
}

حرف* path_startswith_strv(ثابت حرف *p, حرف * ثابت *strv) {
        أكد(p);

        STRV_FOREACH(s, strv) {
                حرف *t;

                t = path_startswith(p, *s);
                אם (t)
                        החזר t;
        }

        החזר عدم;
}

صحيح path_strv_make_absolute_cwd(حرف **l) {
        صحيح r;

        /* Goes through every item in the string list and makes it
         * absolute. This works in place and won't rollback any
         * changes on failure. */

        STRV_FOREACH(s, l) {
                حرف *t;

                r = path_make_absolute_cwd(*s, &t);
                אם (r < 0)
                        החזר r;

                path_simplify(t);
                free_and_replace(*s, t);
        }

        החזר 0;
}

حرف** path_strv_resolve(حرف **l, ثابت حرف *root) {
        unsigned k = 0;
        bool enomem = false;
        صحيح r;

        אם (strv_isempty(l))
                החזר l;

        /* Go through every item in the string list and canonicalize the path.
         * This works in place and doesn't roll back any changes on failure. */

        STRV_FOREACH(s, l) {
                _cleanup_free_ حرف *orig = عدم;
                حرف *t, *u;

                אם (root) {
                        אם (!path_is_absolute(*s)) {
                                log_debug("Ignoring non-absolute search path '%s'.", *s);
                                حرر(*s);
                                استمر;
                        }

                        orig = *s;
                        t = path_join(root, orig);
                        אם (!t) {
                                enomem = true;
                                استمر;
                        }
                } אחרת
                        t = *s;

                r = chase(t, root, 0, &u, عدم);
                אם (r == -ENOENT) {
                        אם (root) {
                                u = TAKE_PTR(orig);
                                حرر(t);
                        } אחרת
                                u = t;
                } אחרת אם (r < 0) {
                        حرر(t);

                        אם (r == -ENOMEM)
                                enomem = true;

                        استمر;
                } אחרת אם (root) {
                        حرف *x;

                        حرر(t);
                        x = path_startswith(u, root);
                        אם (x) {
                                /* restore the slash if it was lost */
                                אם (!startswith(x, "/"))
                                        *(--x) = '/';

                                t = strdup(x);
                                حرر(u);
                                אם (!t) {
                                        enomem = true;
                                        استمر;
                                }
                                u = t;
                        } אחרת {
                                /* canonicalized path goes outside of
                                 * prefix, keep the original path instead */
                                free_and_replace(u, orig);
                        }
                } אחרת
                        حرر(t);

                l[k++] = u;
        }

        l[k] = عدم;

        אם (enomem)
                החזר عدم;

        החזר l;
}

حرف** path_strv_resolve_uniq(حرف **l, ثابت حرف *root) {

        אם (strv_isempty(l))
                החזר l;

        אם (!path_strv_resolve(l, root))
                החזר عدم;

        החזר strv_uniq(l);
}

حرف* skip_leading_slash(ثابت حرف *p) {
        החזר skip_leading_chars(p, "/");
}

حرف* path_simplify_full(حرف *path, PathSimplifyFlags flags) {
        bool add_slash = false, keep_trailing_slash, absolute, beginning = true;
        حرف *f = path;
        صحيح r;

        /* Removes redundant inner and trailing slashes. Also removes unnecessary dots.
         * Modifies the passed string in-place.
         *
         * ///foo//./bar/.   becomes /foo/bar
         * .//./foo//./bar/. becomes foo/bar
         * /../foo/bar       becomes /foo/bar
         * /../foo/bar/..    becomes /foo/bar/..
         */

        אם (isempty(path))
                החזר path;

        keep_trailing_slash = FLAGS_SET(flags, PATH_SIMPLIFY_KEEP_TRAILING_SLASH) && endswith(path, "/");

        absolute = path_is_absolute(path);
        f += absolute;  /* Keep leading /, if present. */

        עבור (ثابت حرف *p = f;;) {
                ثابت حرف *e;

                r = path_find_first_component(&p, true, &e);
                אם (r == 0)
                        اكسر;

                אם (r > 0 && absolute && beginning && path_startswith(e, ".."))
                        /* If we're at the beginning of an absolute path, we can safely skip ".." */
                        استمر;

                beginning = false;

                אם (add_slash)
                        *f++ = '/';

                אם (r < 0) {
                        /* if path is invalid, then refuse to simplify the remaining part. */
                        memmove(f, p, strlen(p) + 1);
                        החזר path;
                }

                memmove(f, e, r);
                f += r;

                add_slash = true;
        }

        /* Special rule, if we stripped everything, we need a "." for the current directory. */
        אם (f == path)
                *f++ = '.';

        אם (*(f-1) != '/' && keep_trailing_slash)
                *f++ = '/';

        *f = '\0';
        החזר path;
}

صحيح path_simplify_alloc(ثابت حرف *path, حرف **ret) {
        أكد(ret);

        אם (!path) {
                *ret = عدم;
                החזר 0;
        }

        حرف *t = strdup(path);
        אם (!t)
                החזר -ENOMEM;

        *ret = path_simplify(t);
        החזר 0;
}

حرف* path_startswith_full(ثابت حرف *original_path, ثابت حرف *prefix, PathStartWithFlags flags) {
        أكد(original_path);
        أكد(prefix);

        /* Returns a pointer to the start of the first component after the parts matched by
         * the prefix, iff
         * - both paths are absolute or both paths are relative,
         * and
         * - each component in prefix in turn matches a component in path at the same position.
         * An empty string will be returned when the prefix and path are equivalent.
         *
         * Returns NULL otherwise.
         */

        ثابت حرف *path = original_path;

        אם ((path[0] == '/') != (prefix[0] == '/'))
                החזר عدم;

        עבור (;;) {
                ثابت حرف *p, *q;
                صحيح m, n;

                m = path_find_first_component(&path, !FLAGS_SET(flags, PATH_STARTSWITH_REFUSE_DOT_DOT), &p);
                אם (m < 0)
                        החזר عدم;

                n = path_find_first_component(&prefix, !FLAGS_SET(flags, PATH_STARTSWITH_REFUSE_DOT_DOT), &q);
                אם (n < 0)
                        החזר عدم;

                אם (n == 0) {
                        אם (!p)
                                p = path;

                        אם (FLAGS_SET(flags, PATH_STARTSWITH_RETURN_LEADING_SLASH)) {

                                אם (p <= original_path)
                                        החזר عدم;

                                p--;

                                אם (*p != '/')
                                        החזר عدم;
                        }

                        החזר (حرف*) p;
                }

                אם (m != n)
                        החזר عدم;

                אם (!strneq(p, q, m))
                        החזר عدم;
        }
}

صحيح path_compare(ثابت حرف *a, ثابت حرف *b) {
        صحيح r;

        /* Order NULL before non-NULL */
        r = CMP(!!a, !!b);
        אם (r != 0)
                החזר r;

        /* A relative path and an absolute path must not compare as equal.
         * Which one is sorted before the other does not really matter.
         * Here a relative path is ordered before an absolute path. */
        r = CMP(path_is_absolute(a), path_is_absolute(b));
        אם (r != 0)
                החזר r;

        עבור (;;) {
                ثابت حرف *aa, *bb;
                صحيح j, k;

                j = path_find_first_component(&a, true, &aa);
                k = path_find_first_component(&b, true, &bb);

                אם (j < 0 || k < 0) {
                        /* When one of paths is invalid, order invalid path after valid one. */
                        r = CMP(j < 0, k < 0);
                        אם (r != 0)
                                החזר r;

                        /* fallback to use strcmp() if both paths are invalid. */
                        החזר strcmp(a, b);
                }

                /* Order prefixes first: "/foo" before "/foo/bar" */
                אם (j == 0) {
                        אם (k == 0)
                                החזר 0;
                        החזר -1;
                }
                אם (k == 0)
                        החזר 1;

                /* Alphabetical sort: "/foo/aaa" before "/foo/b" */
                r = memcmp(aa, bb, MIN(j, k));
                אם (r != 0)
                        החזר r;

                /* Sort "/foo/a" before "/foo/aaa" */
                r = CMP(j, k);
                אם (r != 0)
                        החזר r;
        }
}

صحيح path_compare_filename(ثابت حرف *a, ثابت حرف *b) {
        _cleanup_free_ حرف *fa = عدم, *fb = عدم;
        صحيح r, j, k;

        /* Order NULL before non-NULL */
        r = CMP(!!a, !!b);
        אם (r != 0)
                החזר r;

        j = path_extract_filename(a, &fa);
        k = path_extract_filename(b, &fb);

        /* When one of paths is "." or root, then order it earlier. */
        r = CMP(j != -EADDRNOTAVAIL, k != -EADDRNOTAVAIL);
        אם (r != 0)
                החזר r;

        /* When one of paths is invalid (or we get OOM), order invalid path after valid one. */
        r = CMP(j < 0, k < 0);
        אם (r != 0)
                החזר r;

        /* fallback to use strcmp() if both paths are invalid. */
        אם (j < 0)
                החזר strcmp(a, b);

        החזר strcmp(fa, fb);
}

صحيح path_equal_or_inode_same_full(ثابت حرف *a, ثابت حرف *b, صحيح flags) {
        /* Returns true if paths are of the same entry, false if not, <0 on error. */

        אם (path_equal(a, b))
                החזר 1;

        אם (!a || !b)
                החזר 0;

        החזר inode_same(a, b, flags);
}

حرف* path_extend_internal(حرف **x, ...) {
        size_t sz, old_sz;
        حرف *q, *nx;
        ثابت حرف *p;
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
        כלעוד ((p = va_arg(ap, حرف*)) != POINTER_MAX) {
                size_t add;

                אם (isempty(p))
                        استمر;

                add = 1 + strlen(p);
                אם (sz > SIZE_MAX - add) { /* overflow check */
                        va_end(ap);
                        החזר عدم;
                }

                sz += add;
        }
        va_end(ap);

        nx = realloc(x ? *x : عدم, GREEDY_ALLOC_ROUND_UP(sz+1));
        אם (!nx)
                החזר عدم;
        אם (x)
                *x = nx;

        אם (old_sz > 0)
                slash = nx[old_sz-1] == '/';
        אחרת {
                nx[old_sz] = 0;
                slash = true; /* no need to generate a slash anymore */
        }

        q = nx + old_sz;

        va_start(ap, x);
        כלעוד ((p = va_arg(ap, حرف*)) != POINTER_MAX) {
                אם (isempty(p))
                        استمر;

                אם (!slash && p[0] != '/')
                        *(q++) = '/';

                q = stpcpy(q, p);
                slash = endswith(p, "/");
        }
        va_end(ap);

        החזר nx;
}

صحيح open_and_check_executable(ثابت حرف *name, ثابت حرف *root, حرف **ret_path, صحيح *ret_fd) {
        _cleanup_close_ صحيح fd = -EBADF;
        _cleanup_free_ حرف *resolved = عدم;
        صحيح r;

        أكد(name);

        /* Function chase() is invoked only when root is not NULL, as using it regardless of
         * root value would alter the behavior of existing callers for example: /bin/sleep would become
         * /usr/bin/sleep when find_executables is called. Hence, this function should be invoked when
         * needed to avoid unforeseen regression or other complicated changes. */
        אם (root) {
                /* prefix root to name in case full paths are not specified */
                r = chase(name, root, CHASE_PREFIX_ROOT, &resolved, &fd);
                אם (r < 0)
                        החזר r;

                name = resolved;
        } אחרת {
                /* We need to use O_PATH because there may be executables for which we have only exec permissions,
                 * but not read (usually suid executables). */
                fd = open(name, O_PATH|O_CLOEXEC);
                אם (fd < 0)
                        החזר -errno;
        }

        r = fd_verify_regular(fd);
        אם (r < 0)
                החזר r;

        r = access_fd(fd, X_OK);
        אם (r == -ENOSYS)
                /* /proc/ is not mounted. Fall back to access(). */
                r = RET_NERRNO(access(name, X_OK));
        אם (r < 0)
                החזר r;

        אם (ret_path) {
                אם (resolved)
                        *ret_path = TAKE_PTR(resolved);
                אחרת {
                        r = path_make_absolute_cwd(name, ret_path);
                        אם (r < 0)
                                החזר r;

                        path_simplify(*ret_path);
                }
        }

        אם (ret_fd)
                *ret_fd = TAKE_FD(fd);

        החזר 0;
}

صحيح find_executable_full(
                ثابت حرف *name,
                ثابت حرف *root,
                حرف * ثابت *exec_search_path,
                bool use_path_envvar,
                حرف **ret_filename,
                صحيح *ret_fd) {

        صحيح last_error = -ENOENT, r = 0;

        أكد(name);

        אם (is_path(name))
                החזר open_and_check_executable(name, root, ret_filename, ret_fd);

        אם (exec_search_path) {
                STRV_FOREACH(element, exec_search_path) {
                        _cleanup_free_ حرف *full_path = عدم;

                        אם (!path_is_absolute(*element)) {
                                log_debug("Exec search path '%s' isn't absolute, ignoring.", *element);
                                استمر;
                        }

                        full_path = path_join(*element, name);
                        אם (!full_path)
                                החזר -ENOMEM;

                        r = open_and_check_executable(full_path, root, ret_filename, ret_fd);
                        אם (r >= 0)
                                החזר 0;
                        אם (r != -EACCES)
                                last_error = r;
                }
                החזר last_error;
        }

        ثابت حرف *p = عدم;

        אם (use_path_envvar)
                /* Plain getenv, not secure_getenv, because we want to actually allow the user to pick the
                 * binary. */
                p = getenv("PATH");
        אם (!p)
                p = default_PATH();

        /* Resolve a single-component name to a full path */
        עבור (;;) {
                _cleanup_free_ حرف *element = عدم;

                r = extract_first_word(&p, &element, ":", EXTRACT_RELAX|EXTRACT_DONT_COALESCE_SEPARATORS);
                אם (r < 0)
                        החזר r;
                אם (r == 0)
                        اكسر;

                אם (!path_is_absolute(element)) {
                        log_debug("Exec search path '%s' isn't absolute, ignoring.", element);
                        استمر;
                }

                אם (!path_extend(&element, name))
                        החזר -ENOMEM;

                r = open_and_check_executable(element, root, ret_filename, ret_fd);
                אם (r >= 0) /* Found it! */
                        החזר 0;
                /* PATH entries which we don't have access to are ignored, as per tradition. */
                אם (r != -EACCES)
                        last_error = r;
        }

        החזר last_error;
}

ساكن صحيح executable_is_good(ثابت حرف *executable) {
        _cleanup_free_ حرف *p = عدم, *d = عدم;
        صحيح r;

        r = find_executable(executable, &p);
        אם (r == -ENOENT)
                החזר 0;
        אם (r < 0)
                החזר r;

        /* An fsck that is linked to /bin/true is a non-existent fsck */

        r = readlink_malloc(p, &d);
        אם (r == -EINVAL) /* not a symlink */
                החזר 1;
        אם (r < 0)
                החזר r;

        החזר !PATH_IN_SET(d, "true",
                               "/bin/true",
                               "/usr/bin/true",
                               "/dev/null");
}

صحيح fsck_exists(فراغ) {
        החזר executable_is_good("fsck");
}

صحيح fsck_exists_for_fstype(ثابت حرف *fstype) {
        ثابت حرف *checker;
        صحيح r;

        أكد(fstype);

        אם (streq(fstype, "auto"))
                החזר -EINVAL;

        r = fsck_exists();
        אם (r <= 0)
                החזר r;

        checker = strjoina("fsck.", fstype);
        החזר executable_is_good(checker);
}

ساكن ثابت حرف* skip_slash_or_dot(ثابت حرف *p) {
        POINTER_MAY_BE_NULL(p);

        עבור (; !isempty(p); p++) {
                אם (*p == '/')
                        استمر;
                אם (startswith(p, "./")) {
                        p++;
                        استمر;
                }
                اكسر;
        }
        החזר p;
}

صحيح path_find_first_component(ثابت حرف **p, bool accept_dot_dot, ثابت حرف **ret) {
        ثابت حرف *q, *first, *end_first, *next;
        size_t len;

        أكد(p);

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
        אם (isempty(first)) {
                *p = first;
                אם (ret)
                        *ret = عدم;
                החזר 0;
        }
        אם (streq(first, ".")) {
                *p = first + 1;
                אם (ret)
                        *ret = عدم;
                החזר 0;
        }

        end_first = strchrnul(first, '/');
        len = end_first - first;

        אם (len > NAME_MAX)
                החזר -EINVAL;
        אם (!accept_dot_dot && len == 2 && first[0] == '.' && first[1] == '.')
                החזר -EINVAL;

        next = skip_slash_or_dot(end_first);

        *p = next + streq(next, ".");
        אם (ret)
                *ret = first;
        החזר len;
}

ساكن ثابت حرف* skip_slash_or_dot_backward(ثابت حرف *path, ثابت حرف *q) {
        أكد(path);
        أكد(!q || q >= path);

        עבור (; q; q = PTR_SUB1(q, path)) {
                אם (*q == '/')
                        استمر;
                אם (q > path && strneq(q - 1, "/.", 2))
                        استمر;
                אם (q == path && *q == '.')
                        استمر;
                اكسر;
        }
        החזר q;
}

صحيح path_find_last_component(ثابت حرف *path, bool accept_dot_dot, ثابت حرف **next, ثابت حرف **ret) {
        ثابت حرف *q, *last_end, *last_begin;
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

        אם (isempty(path)) {
                אם (next)
                        *next = path;
                אם (ret)
                        *ret = عدم;
                החזר 0;
        }

        אם (next && *next) {
                אם (*next < path || *next > path + strlen(path))
                        החזר -EINVAL;
                אם (*next == path) {
                        אם (ret)
                                *ret = عدم;
                        החזר 0;
                }
                אם (!IN_SET(**next, '\0', '/'))
                        החזר -EINVAL;
                q = *next - 1;
        } אחרת
                q = path + strlen(path) - 1;

        q = skip_slash_or_dot_backward(path, q);
        אם (!q) { /* the root directory, "." or "./" */
                אם (next)
                        *next = path;
                אם (ret)
                        *ret = عدم;
                החזר 0;
        }

        last_end = q + 1;

        כלעוד (q && *q != '/')
                q = PTR_SUB1(q, path);

        last_begin = q ? q + 1 : path;
        len = last_end - last_begin;

        אם (len > NAME_MAX)
                החזר -EINVAL;
        אם (!accept_dot_dot && len == 2 && strneq(last_begin, "..", 2))
                החזר -EINVAL;

        אם (next) {
                q = skip_slash_or_dot_backward(path, q);
                *next = q ? q + 1 : path;
        }

        אם (ret)
                *ret = last_begin;
        החזר len;
}

ثابت حرف* last_path_component(ثابت حرف *path) {

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

        אם (!path)
                החזר عدم;

        l = k = strlen(path);
        אם (l == 0) /* special case — an empty string */
                החזר path;

        כלעוד (k > 0 && path[k-1] == '/')
                k--;

        אם (k == 0) /* the root directory */
                החזר path + l - 1;

        כלעוד (k > 0 && path[k-1] != '/')
                k--;

        החזר path + k;
}

صحيح path_split_prefix_filename(ثابت حرف *path, حرف **ret_dir, حرف **ret_filename) {
        _cleanup_free_ حرف *d = عدم;
        ثابت حرف *c, *next = عدم;
        صحيح r;

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

        אם (isempty(path))
                החזר -EINVAL;

        r = path_find_last_component(path, /* accept_dot_dot = */ false, &next, &c);
        אם (r < 0)
                החזר r;
        אם (r == 0) /* root directory or "." */
                החזר -EADDRNOTAVAIL;

        אם (ret_dir) {
                אם (next == path) {
                        אם (*path != '/') { /* filename only */
                                אם (!ret_filename)
                                        החזר -EDESTADDRREQ;
                        } אחרת {
                                d = strdup("/");
                                אם (!d)
                                        החזר -ENOMEM;
                        }
                } אחרת {
                        d = strndup(path, next - path);
                        אם (!d)
                                החזר -ENOMEM;

                        path_simplify(d);

                        אם (!path_is_valid(d))
                                החזר -EINVAL;
                }

        } אחרת אם (!path_is_valid(path))
                /* We didn't validate the dir prefix, hence check if the whole path is valid now */
                החזר -EINVAL;

        אם (ret_filename) {
                حرف *fn = strndup(c, r);
                אם (!fn)
                        החזר -ENOMEM;

                *ret_filename = fn;
        }

        אם (ret_dir)
                *ret_dir = TAKE_PTR(d);

        החזר strlen(c) > (size_t) r ? O_DIRECTORY : 0;
}

bool filename_part_is_valid(ثابت حرف *p) {
        ثابت حرف *e;

        /* Checks f the specified string is OK to be *part* of a filename. This is different from
         * filename_is_valid() as "." and ".." and "" are OK by this call, but not by filename_is_valid(). */

        אם (!p)
                החזר false;

        e = strchrnul(p, '/');
        אם (*e != 0)
                החזר false;

        אם (e - p > NAME_MAX) /* NAME_MAX is counted *without* the trailing NUL byte */
                החזר false;

        החזר true;
}

bool filename_is_valid(ثابت حرف *p) {

        אם (isempty(p))
                החזר false;

        אם (dot_or_dot_dot(p)) /* Yes, in this context we consider "." and ".." invalid */
                החזר false;

        החזר filename_part_is_valid(p);
}

bool path_is_valid_full(ثابت حرف *p, bool accept_dot_dot) {
        אם (isempty(p))
                החזר false;

        עבור (ثابت حرف *e = p;;) {
                صحيح r;

                r = path_find_first_component(&e, accept_dot_dot, عدم);
                אם (r < 0)
                        החזר false;

                אם (e - p >= PATH_MAX) /* Already reached the maximum length for a path? (PATH_MAX is counted
                                        * *with* the trailing NUL byte) */
                        החזר false;
                אם (*e == 0)           /* End of string? Yay! */
                        החזר true;
        }
}

bool path_is_normalized(ثابت حرف *p) {
        אם (!path_is_safe(p))
                החזר false;

        אם (streq(p, ".") || startswith(p, "./") || endswith(p, "/.") || strstr(p, "/./"))
                החזר false;

        אם (strstr(p, "//"))
                החזר false;

        החזר true;
}

صحيح file_in_same_dir(ثابت حرف *path, ثابت حرف *filename, حرف **ret) {
        _cleanup_free_ حرف *b = عدم;
        صحيح r;

        أكد(path);
        أكد(filename);
        أكد(ret);

        /* This removes the last component of path and appends filename, unless the latter is absolute anyway
         * or the former isn't */

        אם (path_is_absolute(filename))
                b = strdup(filename);
        אחרת {
                _cleanup_free_ حرف *dn = عدم;

                r = path_extract_directory(path, &dn);
                אם (r == -EDESTADDRREQ) /* no path prefix */
                        b = strdup(filename);
                אחרת אם (r < 0)
                        החזר r;
                אחרת
                        b = path_join(dn, filename);
        }
        אם (!b)
                החזר -ENOMEM;

        *ret = TAKE_PTR(b);
        החזר 0;
}

bool hidden_or_backup_file(ثابت حرف *filename) {
        أكد(filename);

        אם (filename[0] == '.' ||
            STR_IN_SET(filename,
                       "lost+found",
                       "aquota.user",
                       "aquota.group") ||
            endswith(filename, "~"))
                החזר true;

        ثابت حرف *dot = strrchr(filename, '.');
        אם (!dot)
                החזר false;

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

        החזר STR_IN_SET(dot + 1,
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

bool is_device_path(ثابت حرف *path) {

        /* Returns true for paths that likely refer to a device, either by path in sysfs or to something in
         * /dev. This accepts any path that starts with /dev/ or /sys/ and has something after that prefix.
         * It does not actually resolve the path.
         *
         * Examples:
         * /dev/sda, /dev/sda/foo, /sys/class, /dev/.., /sys/.., /./dev/foo → yes.
         * /../dev/sda, /dev, /sys, /usr/path, /usr/../dev/sda → no.
         */

        ثابت حرف *p = PATH_STARTSWITH_SET(ASSERT_PTR(path), "/dev/", "/sys/");
        החזר !isempty(p);
}

bool valid_device_node_path(ثابت حرف *path) {

        /* Some superficial checks whether the specified path is a valid device node path, all without
         * looking at the actual device node. */

        אם (!PATH_STARTSWITH_SET(path, "/dev/", "/run/systemd/inaccessible/"))
                החזר false;

        אם (endswith(path, "/")) /* can't be a device node if it ends in a slash */
                החזר false;

        החזר path_is_normalized(path);
}

bool valid_device_allow_pattern(ثابت حرف *path) {
        أكد(path);

        /* Like valid_device_node_path(), but also allows full-subsystem expressions like those accepted by
         * DeviceAllow= and DeviceDeny=. */

        אם (STARTSWITH_SET(path, "block-", "char-"))
                החזר true;

        החזר valid_device_node_path(path);
}

bool dot_or_dot_dot(ثابت حرف *path) {
        אם (!path)
                החזר false;
        אם (path[0] != '.')
                החזר false;
        אם (path[1] == 0)
                החזר true;
        אם (path[1] != '.')
                החזר false;

        החזר path[2] == 0;
}

bool path_implies_directory(ثابت حرف *path) {

        /* Sometimes, if we look at a path we already know it must refer to a directory, because it is
         * suffixed with a slash, or its last component is "." or ".." */

        אם (!path)
                החזר false;

        אם (dot_or_dot_dot(path))
                החזר true;

        החזר ENDSWITH_SET(path, "/", "/.", "/..");
}

bool empty_or_root(ثابت حرف *path) {

        /* For operations relative to some root directory, returns true if the specified root directory is
         * redundant, i.e. either / or NULL or the empty string or any equivalent. */

        אם (isempty(path))
                החזר true;

        החזר path_equal(path, "/");
}

ثابت حرف* empty_to_root(ثابت حرف *path) {
        החזר isempty(path) ? "/" : path;
}

صحيح empty_or_root_harder_to_null(ثابت حرف **path) {
        صحيح r;

        أكد(path);

        /* This nullifies the input path when the path is empty or points to "/". */

        אם (empty_or_root(*path)) {
                *path = عدم;
                החזר 0;
        }

        r = path_is_root(*path);
        אם (r < 0)
                החזר r;
        אם (r > 0)
                *path = عدم;

        החזר 0;
}

bool path_strv_contains(حرف * ثابت *l, ثابت حرف *path) {
        أكد(path);

        STRV_FOREACH(i, l)
                אם (path_equal(*i, path))
                        החזר true;

        החזר false;
}

bool prefixed_path_strv_contains(حرف * ثابت *l, ثابت حرف *path) {
        أكد(path);

        STRV_FOREACH(i, l) {
                ثابت حرف *j = *i;

                אם (*j == '-')
                        j++;
                אם (*j == '+')
                        j++;

                אם (path_equal(j, path))
                        החזר true;
        }

        החזר false;
}

صحيح path_glob_can_match(ثابت حرف *pattern, ثابت حرف *prefix, حرف **ret) {
        أكد(pattern);
        أكد(prefix);

        עבור (ثابت حرف *a = pattern, *b = prefix;;) {
                _cleanup_free_ حرف *g = عدم, *h = عدم;
                ثابت حرف *p, *q;
                صحيح r, s;

                r = path_find_first_component(&a, /* accept_dot_dot= */ false, &p);
                אם (r < 0)
                        החזר r;

                s = path_find_first_component(&b, /* accept_dot_dot= */ false, &q);
                אם (s < 0)
                        החזר s;

                אם (s == 0) {
                        /* The pattern matches the prefix. */
                        אם (ret) {
                                حرف *t;

                                t = path_join(prefix, p);
                                אם (!t)
                                        החזר -ENOMEM;

                                *ret = t;
                        }
                        החזר true;
                }

                אם (r == 0)
                        اكسر;

                אם (r == s && strneq(p, q, r))
                        استمر; /* common component. Check next. */

                g = strndup(p, r);
                אם (!g)
                        החזר -ENOMEM;

                אם (!string_is_glob(g))
                        اكسر;

                /* We found a glob component. Check if the glob pattern matches the prefix component. */

                h = strndup(q, s);
                אם (!h)
                        החזר -ENOMEM;

                r = fnmatch(g, h, 0);
                אם (r == FNM_NOMATCH)
                        اكسر;
                אם (r != 0) /* Failure to process pattern? */
                        החזר -EINVAL;
        }

        /* The pattern does not match the prefix. */
        אם (ret)
                *ret = عدم;
        החזר false;
}

#אם HAVE_SPLIT_BIN
ساكن bool dir_is_split(ثابت حرف *a, ثابت حرف *b) {
        صحيح r;

        r = inode_same(a, b, AT_NO_AUTOMOUNT);
        אם (r < 0 && r != -ENOENT) {
                log_debug_errno(r, "Failed to compare \"%s\" and \"%s\", assuming split directories: %m", a, b);
                החזר true;
        }
        החזר r == 0;
}
#endif

ثابت حرف* default_PATH(فراغ) {
#אם HAVE_SPLIT_BIN
        ساكن ثابت حرف *default_path = عدم;

        /* Return one of the three sets of paths:
         * a) split /usr/s?bin, /usr/local/sbin doesn't matter.
         * b) merged /usr/s?bin, /usr/sbin is a symlink, but /usr/local/sbin is not,
         * c) fully merged, neither /usr/sbin nor /usr/local/sbin are symlinks,
         *
         * On error the fallback to the safe value with both directories as configured is returned.
         */

        אם (default_path)
                החזר default_path;

        אם (dir_is_split("/usr/sbin", "/usr/bin"))
                החזר (default_path = DEFAULT_PATH_WITH_FULL_SBIN);  /* a */
        אם (dir_is_split("/usr/local/sbin", "/usr/local/bin"))
                החזר (default_path = DEFAULT_PATH_WITH_LOCAL_SBIN); /* b */
        החזר (default_path = DEFAULT_PATH_WITHOUT_SBIN);            /* c */
#אחרת
        החזר DEFAULT_PATH_WITHOUT_SBIN;
#endif
}
