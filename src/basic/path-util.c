/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <fnmatch.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "alloc-util.h"
#include "chase.h"
#include "errno-util.h"
#include "extract-word.h"
#include "fd-util.h"
#include "fs-util.h"
#include "glob-util.h"
#include "log.h"
#include "path-util.h"
#include "stat-util.h"
#include "string-util.h"
#include "strv.h"

bool is_path(const char *p) {
        якщо (!p) /* A NULL pointer is definitely not a path */
                повернути false;

        повернути strchr(p, '/');
}

ціле path_split_and_make_absolute(const char *p, char ***ret) {
        _cleanup_strv_free_ char **l = NULL;
        ціле r;

        assert(p);
        assert(ret);

        l = strv_split(p, ":");
        якщо (!l)
                повернути -ENOMEM;

        r = path_strv_make_absolute_cwd(l);
        якщо (r < 0)
                повернути r;

        *ret = TAKE_PTR(l);
        повернути r;
}

char* path_make_absolute(const char *p, const char *prefix) {
        assert(p);

        /* Makes every item in the list an absolute path by prepending
         * the prefix, if specified and necessary */

        якщо (path_is_absolute(p) || isempty(prefix))
                повернути strdup(p);

        повернути path_join(prefix, p);
}

ціле safe_getcwd(char **ret) {
        _cleanup_free_ char *cwd = NULL;

        cwd = get_current_dir_name();
        якщо (!cwd)
                повернути negative_errno();

        /* Let's make sure the directory is really absolute, to protect us from the logic behind
         * CVE-2018-1000001 */
        якщо (cwd[0] != '/')
                повернути -ENOMEDIUM;

        якщо (ret)
                *ret = TAKE_PTR(cwd);

        повернути 0;
}

ціле path_make_absolute_cwd(const char *p, char **ret) {
        char *c;
        ціле r;

        assert(p);
        assert(ret);

        /* Similar to path_make_absolute(), but prefixes with the
         * current working directory. */

        якщо (path_is_absolute(p))
                c = strdup(p);
        інакше {
                _cleanup_free_ char *cwd = NULL;

                r = safe_getcwd(&cwd);
                якщо (r < 0)
                        повернути r;

                c = path_join(cwd, p);
        }
        якщо (!c)
                повернути -ENOMEM;

        *ret = c;
        повернути 0;
}

ціле path_make_relative(const char *from, const char *to, char **ret) {
        _cleanup_free_ char *result = NULL;
        unsigned n_parents;
        const char *f, *t;
        ціле r, k;
        char *p;

        assert(from);
        assert(to);
        assert(ret);

        /* Strips the common part, and adds ".." elements as necessary. */

        якщо (!path_is_absolute(from) || !path_is_absolute(to))
                повернути -EINVAL;

        для (;;) {
                r = path_find_first_component(&from, true, &f);
                якщо (r < 0)
                        повернути r;

                k = path_find_first_component(&to, true, &t);
                якщо (k < 0)
                        повернути k;

                якщо (r == 0) {
                        /* end of 'from' */
                        якщо (k == 0) {
                                /* from and to are equivalent. */
                                result = strdup(".");
                                якщо (!result)
                                        повернути -ENOMEM;
                        } інакше {
                                /* 'to' is inside of 'from'. */
                                r = path_simplify_alloc(t, &result);
                                якщо (r < 0)
                                        повернути r;

                                якщо (!path_is_valid(result))
                                        повернути -EINVAL;
                        }

                        *ret = TAKE_PTR(result);
                        повернути 0;
                }

                якщо (r != k || !strneq(f, t, r))
                        break;
        }

        /* If we're here, then "from_dir" has one or more elements that need to
         * be replaced with "..". */

        для (n_parents = 1;; n_parents++) {
                /* If this includes ".." we can't do a simple series of "..". */
                r = path_find_first_component(&from, false, &f);
                якщо (r < 0)
                        повернути r;
                якщо (r == 0)
                        break;
        }

        якщо (isempty(t) && n_parents * 3 > PATH_MAX)
                /* PATH_MAX is counted *with* the trailing NUL byte */
                повернути -EINVAL;

        result = new(char, n_parents * 3 + !isempty(t) + strlen_ptr(t));
        якщо (!result)
                повернути -ENOMEM;

        для (p = result; n_parents > 0; n_parents--)
                p = mempcpy(p, "../", 3);

        якщо (isempty(t)) {
                /* Remove trailing slash and terminate string. */
                *(--p) = '\0';
                *ret = TAKE_PTR(result);
                повернути 0;
        }

        strcpy(p, t);

        path_simplify(result);

        якщо (!path_is_valid(result))
                повернути -EINVAL;

        *ret = TAKE_PTR(result);
        повернути 0;
}

ціле path_make_relative_parent(const char *from_child, const char *to, char **ret) {
        _cleanup_free_ char *from = NULL;
        ціле r;

        assert(from_child);
        assert(to);
        assert(ret);

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
        якщо (r < 0)
                повернути r;

        повернути path_make_relative(from, to, ret);
}

char* path_startswith_strv(const char *p, char * const *strv) {
        assert(p);

        STRV_FOREACH(s, strv) {
                char *t;

                t = path_startswith(p, *s);
                якщо (t)
                        повернути t;
        }

        повернути NULL;
}

ціле path_strv_make_absolute_cwd(char **l) {
        ціле r;

        /* Goes through every item in the string list and makes it
         * absolute. This works in place and won't rollback any
         * changes on failure. */

        STRV_FOREACH(s, l) {
                char *t;

                r = path_make_absolute_cwd(*s, &t);
                якщо (r < 0)
                        повернути r;

                path_simplify(t);
                free_and_replace(*s, t);
        }

        повернути 0;
}

char** path_strv_resolve(char **l, const char *root) {
        unsigned k = 0;
        bool enomem = false;
        ціле r;

        якщо (strv_isempty(l))
                повернути l;

        /* Go through every item in the string list and canonicalize the path.
         * This works in place and doesn't roll back any changes on failure. */

        STRV_FOREACH(s, l) {
                _cleanup_free_ char *orig = NULL;
                char *t, *u;

                якщо (root) {
                        якщо (!path_is_absolute(*s)) {
                                log_debug("Ignoring non-absolute search path '%s'.", *s);
                                free(*s);
                                continue;
                        }

                        orig = *s;
                        t = path_join(root, orig);
                        якщо (!t) {
                                enomem = true;
                                continue;
                        }
                } інакше
                        t = *s;

                r = chase(t, root, 0, &u, NULL);
                якщо (r == -ENOENT) {
                        якщо (root) {
                                u = TAKE_PTR(orig);
                                free(t);
                        } інакше
                                u = t;
                } інакше якщо (r < 0) {
                        free(t);

                        якщо (r == -ENOMEM)
                                enomem = true;

                        continue;
                } інакше якщо (root) {
                        char *x;

                        free(t);
                        x = path_startswith(u, root);
                        якщо (x) {
                                /* restore the slash if it was lost */
                                якщо (!startswith(x, "/"))
                                        *(--x) = '/';

                                t = strdup(x);
                                free(u);
                                якщо (!t) {
                                        enomem = true;
                                        continue;
                                }
                                u = t;
                        } інакше {
                                /* canonicalized path goes outside of
                                 * prefix, keep the original path instead */
                                free_and_replace(u, orig);
                        }
                } інакше
                        free(t);

                l[k++] = u;
        }

        l[k] = NULL;

        якщо (enomem)
                повернути NULL;

        повернути l;
}

char** path_strv_resolve_uniq(char **l, const char *root) {

        якщо (strv_isempty(l))
                повернути l;

        якщо (!path_strv_resolve(l, root))
                повернути NULL;

        повернути strv_uniq(l);
}

char* skip_leading_slash(const char *p) {
        повернути skip_leading_chars(p, "/");
}

char* path_simplify_full(char *path, PathSimplifyFlags flags) {
        bool add_slash = false, keep_trailing_slash, absolute, beginning = true;
        char *f = path;
        ціле r;

        /* Removes redundant inner and trailing slashes. Also removes unnecessary dots.
         * Modifies the passed string in-place.
         *
         * ///foo//./bar/.   becomes /foo/bar
         * .//./foo//./bar/. becomes foo/bar
         * /../foo/bar       becomes /foo/bar
         * /../foo/bar/..    becomes /foo/bar/..
         */

        якщо (isempty(path))
                повернути path;

        keep_trailing_slash = FLAGS_SET(flags, PATH_SIMPLIFY_KEEP_TRAILING_SLASH) && endswith(path, "/");

        absolute = path_is_absolute(path);
        f += absolute;  /* Keep leading /, if present. */

        для (const char *p = f;;) {
                const char *e;

                r = path_find_first_component(&p, true, &e);
                якщо (r == 0)
                        break;

                якщо (r > 0 && absolute && beginning && path_startswith(e, ".."))
                        /* If we're at the beginning of an absolute path, we can safely skip ".." */
                        continue;

                beginning = false;

                якщо (add_slash)
                        *f++ = '/';

                якщо (r < 0) {
                        /* if path is invalid, then refuse to simplify the remaining part. */
                        memmove(f, p, strlen(p) + 1);
                        повернути path;
                }

                memmove(f, e, r);
                f += r;

                add_slash = true;
        }

        /* Special rule, if we stripped everything, we need a "." for the current directory. */
        якщо (f == path)
                *f++ = '.';

        якщо (*(f-1) != '/' && keep_trailing_slash)
                *f++ = '/';

        *f = '\0';
        повернути path;
}

ціле path_simplify_alloc(const char *path, char **ret) {
        assert(ret);

        якщо (!path) {
                *ret = NULL;
                повернути 0;
        }

        char *t = strdup(path);
        якщо (!t)
                повернути -ENOMEM;

        *ret = path_simplify(t);
        повернути 0;
}

char* path_startswith_full(const char *original_path, const char *prefix, PathStartWithFlags flags) {
        assert(original_path);
        assert(prefix);

        /* Returns a pointer to the start of the first component after the parts matched by
         * the prefix, iff
         * - both paths are absolute or both paths are relative,
         * and
         * - each component in prefix in turn matches a component in path at the same position.
         * An empty string will be returned when the prefix and path are equivalent.
         *
         * Returns NULL otherwise.
         */

        const char *path = original_path;

        якщо ((path[0] == '/') != (prefix[0] == '/'))
                повернути NULL;

        для (;;) {
                const char *p, *q;
                ціле m, n;

                m = path_find_first_component(&path, !FLAGS_SET(flags, PATH_STARTSWITH_REFUSE_DOT_DOT), &p);
                якщо (m < 0)
                        повернути NULL;

                n = path_find_first_component(&prefix, !FLAGS_SET(flags, PATH_STARTSWITH_REFUSE_DOT_DOT), &q);
                якщо (n < 0)
                        повернути NULL;

                якщо (n == 0) {
                        якщо (!p)
                                p = path;

                        якщо (FLAGS_SET(flags, PATH_STARTSWITH_RETURN_LEADING_SLASH)) {

                                якщо (p <= original_path)
                                        повернути NULL;

                                p--;

                                якщо (*p != '/')
                                        повернути NULL;
                        }

                        повернути (char*) p;
                }

                якщо (m != n)
                        повернути NULL;

                якщо (!strneq(p, q, m))
                        повернути NULL;
        }
}

ціле path_compare(const char *a, const char *b) {
        ціле r;

        /* Order NULL before non-NULL */
        r = CMP(!!a, !!b);
        якщо (r != 0)
                повернути r;

        /* A relative path and an absolute path must not compare as equal.
         * Which one is sorted before the other does not really matter.
         * Here a relative path is ordered before an absolute path. */
        r = CMP(path_is_absolute(a), path_is_absolute(b));
        якщо (r != 0)
                повернути r;

        для (;;) {
                const char *aa, *bb;
                ціле j, k;

                j = path_find_first_component(&a, true, &aa);
                k = path_find_first_component(&b, true, &bb);

                якщо (j < 0 || k < 0) {
                        /* When one of paths is invalid, order invalid path after valid one. */
                        r = CMP(j < 0, k < 0);
                        якщо (r != 0)
                                повернути r;

                        /* fallback to use strcmp() if both paths are invalid. */
                        повернути strcmp(a, b);
                }

                /* Order prefixes first: "/foo" before "/foo/bar" */
                якщо (j == 0) {
                        якщо (k == 0)
                                повернути 0;
                        повернути -1;
                }
                якщо (k == 0)
                        повернути 1;

                /* Alphabetical sort: "/foo/aaa" before "/foo/b" */
                r = memcmp(aa, bb, MIN(j, k));
                якщо (r != 0)
                        повернути r;

                /* Sort "/foo/a" before "/foo/aaa" */
                r = CMP(j, k);
                якщо (r != 0)
                        повернути r;
        }
}

ціле path_compare_filename(const char *a, const char *b) {
        _cleanup_free_ char *fa = NULL, *fb = NULL;
        ціле r, j, k;

        /* Order NULL before non-NULL */
        r = CMP(!!a, !!b);
        якщо (r != 0)
                повернути r;

        j = path_extract_filename(a, &fa);
        k = path_extract_filename(b, &fb);

        /* When one of paths is "." or root, then order it earlier. */
        r = CMP(j != -EADDRNOTAVAIL, k != -EADDRNOTAVAIL);
        якщо (r != 0)
                повернути r;

        /* When one of paths is invalid (or we get OOM), order invalid path after valid one. */
        r = CMP(j < 0, k < 0);
        якщо (r != 0)
                повернути r;

        /* fallback to use strcmp() if both paths are invalid. */
        якщо (j < 0)
                повернути strcmp(a, b);

        повернути strcmp(fa, fb);
}

ціле path_equal_or_inode_same_full(const char *a, const char *b, ціле flags) {
        /* Returns true if paths are of the same entry, false if not, <0 on error. */

        якщо (path_equal(a, b))
                повернути 1;

        якщо (!a || !b)
                повернути 0;

        повернути inode_same(a, b, flags);
}

char* path_extend_internal(char **x, ...) {
        size_t sz, old_sz;
        char *q, *nx;
        const char *p;
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
        поки ((p = va_arg(ap, char*)) != POINTER_MAX) {
                size_t add;

                якщо (isempty(p))
                        continue;

                add = 1 + strlen(p);
                якщо (sz > SIZE_MAX - add) { /* overflow check */
                        va_end(ap);
                        повернути NULL;
                }

                sz += add;
        }
        va_end(ap);

        nx = realloc(x ? *x : NULL, GREEDY_ALLOC_ROUND_UP(sz+1));
        якщо (!nx)
                повернути NULL;
        якщо (x)
                *x = nx;

        якщо (old_sz > 0)
                slash = nx[old_sz-1] == '/';
        інакше {
                nx[old_sz] = 0;
                slash = true; /* no need to generate a slash anymore */
        }

        q = nx + old_sz;

        va_start(ap, x);
        поки ((p = va_arg(ap, char*)) != POINTER_MAX) {
                якщо (isempty(p))
                        continue;

                якщо (!slash && p[0] != '/')
                        *(q++) = '/';

                q = stpcpy(q, p);
                slash = endswith(p, "/");
        }
        va_end(ap);

        повернути nx;
}

ціле open_and_check_executable(const char *name, const char *root, char **ret_path, ціле *ret_fd) {
        _cleanup_close_ ціле fd = -EBADF;
        _cleanup_free_ char *resolved = NULL;
        ціле r;

        assert(name);

        /* Function chase() is invoked only when root is not NULL, as using it regardless of
         * root value would alter the behavior of existing callers for example: /bin/sleep would become
         * /usr/bin/sleep when find_executables is called. Hence, this function should be invoked when
         * needed to avoid unforeseen regression or other complicated changes. */
        якщо (root) {
                /* prefix root to name in case full paths are not specified */
                r = chase(name, root, CHASE_PREFIX_ROOT, &resolved, &fd);
                якщо (r < 0)
                        повернути r;

                name = resolved;
        } інакше {
                /* We need to use O_PATH because there may be executables for which we have only exec permissions,
                 * but not read (usually suid executables). */
                fd = open(name, O_PATH|O_CLOEXEC);
                якщо (fd < 0)
                        повернути -errno;
        }

        r = fd_verify_regular(fd);
        якщо (r < 0)
                повернути r;

        r = access_fd(fd, X_OK);
        якщо (r == -ENOSYS)
                /* /proc/ is not mounted. Fall back to access(). */
                r = RET_NERRNO(access(name, X_OK));
        якщо (r < 0)
                повернути r;

        якщо (ret_path) {
                якщо (resolved)
                        *ret_path = TAKE_PTR(resolved);
                інакше {
                        r = path_make_absolute_cwd(name, ret_path);
                        якщо (r < 0)
                                повернути r;

                        path_simplify(*ret_path);
                }
        }

        якщо (ret_fd)
                *ret_fd = TAKE_FD(fd);

        повернути 0;
}

ціле find_executable_full(
                const char *name,
                const char *root,
                char * const *exec_search_path,
                bool use_path_envvar,
                char **ret_filename,
                ціле *ret_fd) {

        ціле last_error = -ENOENT, r = 0;

        assert(name);

        якщо (is_path(name))
                повернути open_and_check_executable(name, root, ret_filename, ret_fd);

        якщо (exec_search_path) {
                STRV_FOREACH(element, exec_search_path) {
                        _cleanup_free_ char *full_path = NULL;

                        якщо (!path_is_absolute(*element)) {
                                log_debug("Exec search path '%s' isn't absolute, ignoring.", *element);
                                continue;
                        }

                        full_path = path_join(*element, name);
                        якщо (!full_path)
                                повернути -ENOMEM;

                        r = open_and_check_executable(full_path, root, ret_filename, ret_fd);
                        якщо (r >= 0)
                                повернути 0;
                        якщо (r != -EACCES)
                                last_error = r;
                }
                повернути last_error;
        }

        const char *p = NULL;

        якщо (use_path_envvar)
                /* Plain getenv, not secure_getenv, because we want to actually allow the user to pick the
                 * binary. */
                p = getenv("PATH");
        якщо (!p)
                p = default_PATH();

        /* Resolve a single-component name to a full path */
        для (;;) {
                _cleanup_free_ char *element = NULL;

                r = extract_first_word(&p, &element, ":", EXTRACT_RELAX|EXTRACT_DONT_COALESCE_SEPARATORS);
                якщо (r < 0)
                        повернути r;
                якщо (r == 0)
                        break;

                якщо (!path_is_absolute(element)) {
                        log_debug("Exec search path '%s' isn't absolute, ignoring.", element);
                        continue;
                }

                якщо (!path_extend(&element, name))
                        повернути -ENOMEM;

                r = open_and_check_executable(element, root, ret_filename, ret_fd);
                якщо (r >= 0) /* Found it! */
                        повернути 0;
                /* PATH entries which we don't have access to are ignored, as per tradition. */
                якщо (r != -EACCES)
                        last_error = r;
        }

        повернути last_error;
}

static ціле executable_is_good(const char *executable) {
        _cleanup_free_ char *p = NULL, *d = NULL;
        ціле r;

        r = find_executable(executable, &p);
        якщо (r == -ENOENT)
                повернути 0;
        якщо (r < 0)
                повернути r;

        /* An fsck that is linked to /bin/true is a non-existent fsck */

        r = readlink_malloc(p, &d);
        якщо (r == -EINVAL) /* not a symlink */
                повернути 1;
        якщо (r < 0)
                повернути r;

        повернути !PATH_IN_SET(d, "true",
                               "/bin/true",
                               "/usr/bin/true",
                               "/dev/null");
}

ціле fsck_exists(void) {
        повернути executable_is_good("fsck");
}

ціле fsck_exists_for_fstype(const char *fstype) {
        const char *checker;
        ціле r;

        assert(fstype);

        якщо (streq(fstype, "auto"))
                повернути -EINVAL;

        r = fsck_exists();
        якщо (r <= 0)
                повернути r;

        checker = strjoina("fsck.", fstype);
        повернути executable_is_good(checker);
}

static const char* skip_slash_or_dot(const char *p) {
        POINTER_MAY_BE_NULL(p);

        для (; !isempty(p); p++) {
                якщо (*p == '/')
                        continue;
                якщо (startswith(p, "./")) {
                        p++;
                        continue;
                }
                break;
        }
        повернути p;
}

ціле path_find_first_component(const char **p, bool accept_dot_dot, const char **ret) {
        const char *q, *first, *end_first, *next;
        size_t len;

        assert(p);

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
        якщо (isempty(first)) {
                *p = first;
                якщо (ret)
                        *ret = NULL;
                повернути 0;
        }
        якщо (streq(first, ".")) {
                *p = first + 1;
                якщо (ret)
                        *ret = NULL;
                повернути 0;
        }

        end_first = strchrnul(first, '/');
        len = end_first - first;

        якщо (len > NAME_MAX)
                повернути -EINVAL;
        якщо (!accept_dot_dot && len == 2 && first[0] == '.' && first[1] == '.')
                повернути -EINVAL;

        next = skip_slash_or_dot(end_first);

        *p = next + streq(next, ".");
        якщо (ret)
                *ret = first;
        повернути len;
}

static const char* skip_slash_or_dot_backward(const char *path, const char *q) {
        assert(path);
        assert(!q || q >= path);

        для (; q; q = PTR_SUB1(q, path)) {
                якщо (*q == '/')
                        continue;
                якщо (q > path && strneq(q - 1, "/.", 2))
                        continue;
                якщо (q == path && *q == '.')
                        continue;
                break;
        }
        повернути q;
}

ціле path_find_last_component(const char *path, bool accept_dot_dot, const char **next, const char **ret) {
        const char *q, *last_end, *last_begin;
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

        якщо (isempty(path)) {
                якщо (next)
                        *next = path;
                якщо (ret)
                        *ret = NULL;
                повернути 0;
        }

        якщо (next && *next) {
                якщо (*next < path || *next > path + strlen(path))
                        повернути -EINVAL;
                якщо (*next == path) {
                        якщо (ret)
                                *ret = NULL;
                        повернути 0;
                }
                якщо (!IN_SET(**next, '\0', '/'))
                        повернути -EINVAL;
                q = *next - 1;
        } інакше
                q = path + strlen(path) - 1;

        q = skip_slash_or_dot_backward(path, q);
        якщо (!q) { /* the root directory, "." or "./" */
                якщо (next)
                        *next = path;
                якщо (ret)
                        *ret = NULL;
                повернути 0;
        }

        last_end = q + 1;

        поки (q && *q != '/')
                q = PTR_SUB1(q, path);

        last_begin = q ? q + 1 : path;
        len = last_end - last_begin;

        якщо (len > NAME_MAX)
                повернути -EINVAL;
        якщо (!accept_dot_dot && len == 2 && strneq(last_begin, "..", 2))
                повернути -EINVAL;

        якщо (next) {
                q = skip_slash_or_dot_backward(path, q);
                *next = q ? q + 1 : path;
        }

        якщо (ret)
                *ret = last_begin;
        повернути len;
}

const char* last_path_component(const char *path) {

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

        якщо (!path)
                повернути NULL;

        l = k = strlen(path);
        якщо (l == 0) /* special case — an empty string */
                повернути path;

        поки (k > 0 && path[k-1] == '/')
                k--;

        якщо (k == 0) /* the root directory */
                повернути path + l - 1;

        поки (k > 0 && path[k-1] != '/')
                k--;

        повернути path + k;
}

ціле path_split_prefix_filename(const char *path, char **ret_dir, char **ret_filename) {
        _cleanup_free_ char *d = NULL;
        const char *c, *next = NULL;
        ціле r;

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

        якщо (isempty(path))
                повернути -EINVAL;

        r = path_find_last_component(path, /* accept_dot_dot = */ false, &next, &c);
        якщо (r < 0)
                повернути r;
        якщо (r == 0) /* root directory or "." */
                повернути -EADDRNOTAVAIL;

        якщо (ret_dir) {
                якщо (next == path) {
                        якщо (*path != '/') { /* filename only */
                                якщо (!ret_filename)
                                        повернути -EDESTADDRREQ;
                        } інакше {
                                d = strdup("/");
                                якщо (!d)
                                        повернути -ENOMEM;
                        }
                } інакше {
                        d = strndup(path, next - path);
                        якщо (!d)
                                повернути -ENOMEM;

                        path_simplify(d);

                        якщо (!path_is_valid(d))
                                повернути -EINVAL;
                }

        } інакше якщо (!path_is_valid(path))
                /* We didn't validate the dir prefix, hence check if the whole path is valid now */
                повернути -EINVAL;

        якщо (ret_filename) {
                char *fn = strndup(c, r);
                якщо (!fn)
                        повернути -ENOMEM;

                *ret_filename = fn;
        }

        якщо (ret_dir)
                *ret_dir = TAKE_PTR(d);

        повернути strlen(c) > (size_t) r ? O_DIRECTORY : 0;
}

bool filename_part_is_valid(const char *p) {
        const char *e;

        /* Checks f the specified string is OK to be *part* of a filename. This is different from
         * filename_is_valid() as "." and ".." and "" are OK by this call, but not by filename_is_valid(). */

        якщо (!p)
                повернути false;

        e = strchrnul(p, '/');
        якщо (*e != 0)
                повернути false;

        якщо (e - p > NAME_MAX) /* NAME_MAX is counted *without* the trailing NUL byte */
                повернути false;

        повернути true;
}

bool filename_is_valid(const char *p) {

        якщо (isempty(p))
                повернути false;

        якщо (dot_or_dot_dot(p)) /* Yes, in this context we consider "." and ".." invalid */
                повернути false;

        повернути filename_part_is_valid(p);
}

bool path_is_valid_full(const char *p, bool accept_dot_dot) {
        якщо (isempty(p))
                повернути false;

        для (const char *e = p;;) {
                ціле r;

                r = path_find_first_component(&e, accept_dot_dot, NULL);
                якщо (r < 0)
                        повернути false;

                якщо (e - p >= PATH_MAX) /* Already reached the maximum length for a path? (PATH_MAX is counted
                                        * *with* the trailing NUL byte) */
                        повернути false;
                якщо (*e == 0)           /* End of string? Yay! */
                        повернути true;
        }
}

bool path_is_normalized(const char *p) {
        якщо (!path_is_safe(p))
                повернути false;

        якщо (streq(p, ".") || startswith(p, "./") || endswith(p, "/.") || strstr(p, "/./"))
                повернути false;

        якщо (strstr(p, "//"))
                повернути false;

        повернути true;
}

ціле file_in_same_dir(const char *path, const char *filename, char **ret) {
        _cleanup_free_ char *b = NULL;
        ціле r;

        assert(path);
        assert(filename);
        assert(ret);

        /* This removes the last component of path and appends filename, unless the latter is absolute anyway
         * or the former isn't */

        якщо (path_is_absolute(filename))
                b = strdup(filename);
        інакше {
                _cleanup_free_ char *dn = NULL;

                r = path_extract_directory(path, &dn);
                якщо (r == -EDESTADDRREQ) /* no path prefix */
                        b = strdup(filename);
                інакше якщо (r < 0)
                        повернути r;
                інакше
                        b = path_join(dn, filename);
        }
        якщо (!b)
                повернути -ENOMEM;

        *ret = TAKE_PTR(b);
        повернути 0;
}

bool hidden_or_backup_file(const char *filename) {
        assert(filename);

        якщо (filename[0] == '.' ||
            STR_IN_SET(filename,
                       "lost+found",
                       "aquota.user",
                       "aquota.group") ||
            endswith(filename, "~"))
                повернути true;

        const char *dot = strrchr(filename, '.');
        якщо (!dot)
                повернути false;

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

        повернути STR_IN_SET(dot + 1,
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

bool is_device_path(const char *path) {

        /* Returns true for paths that likely refer to a device, either by path in sysfs or to something in
         * /dev. This accepts any path that starts with /dev/ or /sys/ and has something after that prefix.
         * It does not actually resolve the path.
         *
         * Examples:
         * /dev/sda, /dev/sda/foo, /sys/class, /dev/.., /sys/.., /./dev/foo → yes.
         * /../dev/sda, /dev, /sys, /usr/path, /usr/../dev/sda → no.
         */

        const char *p = PATH_STARTSWITH_SET(ASSERT_PTR(path), "/dev/", "/sys/");
        повернути !isempty(p);
}

bool valid_device_node_path(const char *path) {

        /* Some superficial checks whether the specified path is a valid device node path, all without
         * looking at the actual device node. */

        якщо (!PATH_STARTSWITH_SET(path, "/dev/", "/run/systemd/inaccessible/"))
                повернути false;

        якщо (endswith(path, "/")) /* can't be a device node if it ends in a slash */
                повернути false;

        повернути path_is_normalized(path);
}

bool valid_device_allow_pattern(const char *path) {
        assert(path);

        /* Like valid_device_node_path(), but also allows full-subsystem expressions like those accepted by
         * DeviceAllow= and DeviceDeny=. */

        якщо (STARTSWITH_SET(path, "block-", "char-"))
                повернути true;

        повернути valid_device_node_path(path);
}

bool dot_or_dot_dot(const char *path) {
        якщо (!path)
                повернути false;
        якщо (path[0] != '.')
                повернути false;
        якщо (path[1] == 0)
                повернути true;
        якщо (path[1] != '.')
                повернути false;

        повернути path[2] == 0;
}

bool path_implies_directory(const char *path) {

        /* Sometimes, if we look at a path we already know it must refer to a directory, because it is
         * suffixed with a slash, or its last component is "." or ".." */

        якщо (!path)
                повернути false;

        якщо (dot_or_dot_dot(path))
                повернути true;

        повернути ENDSWITH_SET(path, "/", "/.", "/..");
}

bool empty_or_root(const char *path) {

        /* For operations relative to some root directory, returns true if the specified root directory is
         * redundant, i.e. either / or NULL or the empty string or any equivalent. */

        якщо (isempty(path))
                повернути true;

        повернути path_equal(path, "/");
}

const char* empty_to_root(const char *path) {
        повернути isempty(path) ? "/" : path;
}

ціле empty_or_root_harder_to_null(const char **path) {
        ціле r;

        assert(path);

        /* This nullifies the input path when the path is empty or points to "/". */

        якщо (empty_or_root(*path)) {
                *path = NULL;
                повернути 0;
        }

        r = path_is_root(*path);
        якщо (r < 0)
                повернути r;
        якщо (r > 0)
                *path = NULL;

        повернути 0;
}

bool path_strv_contains(char * const *l, const char *path) {
        assert(path);

        STRV_FOREACH(i, l)
                якщо (path_equal(*i, path))
                        повернути true;

        повернути false;
}

bool prefixed_path_strv_contains(char * const *l, const char *path) {
        assert(path);

        STRV_FOREACH(i, l) {
                const char *j = *i;

                якщо (*j == '-')
                        j++;
                якщо (*j == '+')
                        j++;

                якщо (path_equal(j, path))
                        повернути true;
        }

        повернути false;
}

ціле path_glob_can_match(const char *pattern, const char *prefix, char **ret) {
        assert(pattern);
        assert(prefix);

        для (const char *a = pattern, *b = prefix;;) {
                _cleanup_free_ char *g = NULL, *h = NULL;
                const char *p, *q;
                ціле r, s;

                r = path_find_first_component(&a, /* accept_dot_dot= */ false, &p);
                якщо (r < 0)
                        повернути r;

                s = path_find_first_component(&b, /* accept_dot_dot= */ false, &q);
                якщо (s < 0)
                        повернути s;

                якщо (s == 0) {
                        /* The pattern matches the prefix. */
                        якщо (ret) {
                                char *t;

                                t = path_join(prefix, p);
                                якщо (!t)
                                        повернути -ENOMEM;

                                *ret = t;
                        }
                        повернути true;
                }

                якщо (r == 0)
                        break;

                якщо (r == s && strneq(p, q, r))
                        continue; /* common component. Check next. */

                g = strndup(p, r);
                якщо (!g)
                        повернути -ENOMEM;

                якщо (!string_is_glob(g))
                        break;

                /* We found a glob component. Check if the glob pattern matches the prefix component. */

                h = strndup(q, s);
                якщо (!h)
                        повернути -ENOMEM;

                r = fnmatch(g, h, 0);
                якщо (r == FNM_NOMATCH)
                        break;
                якщо (r != 0) /* Failure to process pattern? */
                        повернути -EINVAL;
        }

        /* The pattern does not match the prefix. */
        якщо (ret)
                *ret = NULL;
        повернути false;
}

#якщо HAVE_SPLIT_BIN
static bool dir_is_split(const char *a, const char *b) {
        ціле r;

        r = inode_same(a, b, AT_NO_AUTOMOUNT);
        якщо (r < 0 && r != -ENOENT) {
                log_debug_errno(r, "Failed to compare \"%s\" and \"%s\", assuming split directories: %m", a, b);
                повернути true;
        }
        повернути r == 0;
}
#endif

const char* default_PATH(void) {
#якщо HAVE_SPLIT_BIN
        static const char *default_path = NULL;

        /* Return one of the three sets of paths:
         * a) split /usr/s?bin, /usr/local/sbin doesn't matter.
         * b) merged /usr/s?bin, /usr/sbin is a symlink, but /usr/local/sbin is not,
         * c) fully merged, neither /usr/sbin nor /usr/local/sbin are symlinks,
         *
         * On error the fallback to the safe value with both directories as configured is returned.
         */

        якщо (default_path)
                повернути default_path;

        якщо (dir_is_split("/usr/sbin", "/usr/bin"))
                повернути (default_path = DEFAULT_PATH_WITH_FULL_SBIN);  /* a */
        якщо (dir_is_split("/usr/local/sbin", "/usr/local/bin"))
                повернути (default_path = DEFAULT_PATH_WITH_LOCAL_SBIN); /* b */
        повернути (default_path = DEFAULT_PATH_WITHOUT_SBIN);            /* c */
#інакше
        повернути DEFAULT_PATH_WITHOUT_SBIN;
#endif
}
