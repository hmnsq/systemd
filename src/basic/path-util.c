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
        如果 (!p) /* A NULL pointer is definitely not a path */
                返回 false;

        返回 strchr(p, '/');
}

整数 path_split_and_make_absolute(const char *p, char ***ret) {
        _cleanup_strv_free_ char **l = NULL;
        整数 r;

        assert(p);
        assert(ret);

        l = strv_split(p, ":");
        如果 (!l)
                返回 -ENOMEM;

        r = path_strv_make_absolute_cwd(l);
        如果 (r < 0)
                返回 r;

        *ret = TAKE_PTR(l);
        返回 r;
}

char* path_make_absolute(const char *p, const char *prefix) {
        assert(p);

        /* Makes every item in the list an absolute path by prepending
         * the prefix, if specified and necessary */

        如果 (path_is_absolute(p) || isempty(prefix))
                返回 strdup(p);

        返回 path_join(prefix, p);
}

整数 safe_getcwd(char **ret) {
        _cleanup_free_ char *cwd = NULL;

        cwd = get_current_dir_name();
        如果 (!cwd)
                返回 negative_errno();

        /* Let's make sure the directory is really absolute, to protect us from the logic behind
         * CVE-2018-1000001 */
        如果 (cwd[0] != '/')
                返回 -ENOMEDIUM;

        如果 (ret)
                *ret = TAKE_PTR(cwd);

        返回 0;
}

整数 path_make_absolute_cwd(const char *p, char **ret) {
        char *c;
        整数 r;

        assert(p);
        assert(ret);

        /* Similar to path_make_absolute(), but prefixes with the
         * current working directory. */

        如果 (path_is_absolute(p))
                c = strdup(p);
        否则 {
                _cleanup_free_ char *cwd = NULL;

                r = safe_getcwd(&cwd);
                如果 (r < 0)
                        返回 r;

                c = path_join(cwd, p);
        }
        如果 (!c)
                返回 -ENOMEM;

        *ret = c;
        返回 0;
}

整数 path_make_relative(const char *from, const char *to, char **ret) {
        _cleanup_free_ char *result = NULL;
        unsigned n_parents;
        const char *f, *t;
        整数 r, k;
        char *p;

        assert(from);
        assert(to);
        assert(ret);

        /* Strips the common part, and adds ".." elements as necessary. */

        如果 (!path_is_absolute(from) || !path_is_absolute(to))
                返回 -EINVAL;

        为 (;;) {
                r = path_find_first_component(&from, true, &f);
                如果 (r < 0)
                        返回 r;

                k = path_find_first_component(&to, true, &t);
                如果 (k < 0)
                        返回 k;

                如果 (r == 0) {
                        /* end of 'from' */
                        如果 (k == 0) {
                                /* from and to are equivalent. */
                                result = strdup(".");
                                如果 (!result)
                                        返回 -ENOMEM;
                        } 否则 {
                                /* 'to' is inside of 'from'. */
                                r = path_simplify_alloc(t, &result);
                                如果 (r < 0)
                                        返回 r;

                                如果 (!path_is_valid(result))
                                        返回 -EINVAL;
                        }

                        *ret = TAKE_PTR(result);
                        返回 0;
                }

                如果 (r != k || !strneq(f, t, r))
                        break;
        }

        /* If we're here, then "from_dir" has one or more elements that need to
         * be replaced with "..". */

        为 (n_parents = 1;; n_parents++) {
                /* If this includes ".." we can't do a simple series of "..". */
                r = path_find_first_component(&from, false, &f);
                如果 (r < 0)
                        返回 r;
                如果 (r == 0)
                        break;
        }

        如果 (isempty(t) && n_parents * 3 > PATH_MAX)
                /* PATH_MAX is counted *with* the trailing NUL byte */
                返回 -EINVAL;

        result = new(char, n_parents * 3 + !isempty(t) + strlen_ptr(t));
        如果 (!result)
                返回 -ENOMEM;

        为 (p = result; n_parents > 0; n_parents--)
                p = mempcpy(p, "../", 3);

        如果 (isempty(t)) {
                /* Remove trailing slash and terminate string. */
                *(--p) = '\0';
                *ret = TAKE_PTR(result);
                返回 0;
        }

        strcpy(p, t);

        path_simplify(result);

        如果 (!path_is_valid(result))
                返回 -EINVAL;

        *ret = TAKE_PTR(result);
        返回 0;
}

整数 path_make_relative_parent(const char *from_child, const char *to, char **ret) {
        _cleanup_free_ char *from = NULL;
        整数 r;

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
        如果 (r < 0)
                返回 r;

        返回 path_make_relative(from, to, ret);
}

char* path_startswith_strv(const char *p, char * const *strv) {
        assert(p);

        STRV_FOREACH(s, strv) {
                char *t;

                t = path_startswith(p, *s);
                如果 (t)
                        返回 t;
        }

        返回 NULL;
}

整数 path_strv_make_absolute_cwd(char **l) {
        整数 r;

        /* Goes through every item in the string list and makes it
         * absolute. This works in place and won't rollback any
         * changes on failure. */

        STRV_FOREACH(s, l) {
                char *t;

                r = path_make_absolute_cwd(*s, &t);
                如果 (r < 0)
                        返回 r;

                path_simplify(t);
                free_and_replace(*s, t);
        }

        返回 0;
}

char** path_strv_resolve(char **l, const char *root) {
        unsigned k = 0;
        bool enomem = false;
        整数 r;

        如果 (strv_isempty(l))
                返回 l;

        /* Go through every item in the string list and canonicalize the path.
         * This works in place and doesn't roll back any changes on failure. */

        STRV_FOREACH(s, l) {
                _cleanup_free_ char *orig = NULL;
                char *t, *u;

                如果 (root) {
                        如果 (!path_is_absolute(*s)) {
                                log_debug("Ignoring non-absolute search path '%s'.", *s);
                                free(*s);
                                continue;
                        }

                        orig = *s;
                        t = path_join(root, orig);
                        如果 (!t) {
                                enomem = true;
                                continue;
                        }
                } 否则
                        t = *s;

                r = chase(t, root, 0, &u, NULL);
                如果 (r == -ENOENT) {
                        如果 (root) {
                                u = TAKE_PTR(orig);
                                free(t);
                        } 否则
                                u = t;
                } 否则 如果 (r < 0) {
                        free(t);

                        如果 (r == -ENOMEM)
                                enomem = true;

                        continue;
                } 否则 如果 (root) {
                        char *x;

                        free(t);
                        x = path_startswith(u, root);
                        如果 (x) {
                                /* restore the slash if it was lost */
                                如果 (!startswith(x, "/"))
                                        *(--x) = '/';

                                t = strdup(x);
                                free(u);
                                如果 (!t) {
                                        enomem = true;
                                        continue;
                                }
                                u = t;
                        } 否则 {
                                /* canonicalized path goes outside of
                                 * prefix, keep the original path instead */
                                free_and_replace(u, orig);
                        }
                } 否则
                        free(t);

                l[k++] = u;
        }

        l[k] = NULL;

        如果 (enomem)
                返回 NULL;

        返回 l;
}

char** path_strv_resolve_uniq(char **l, const char *root) {

        如果 (strv_isempty(l))
                返回 l;

        如果 (!path_strv_resolve(l, root))
                返回 NULL;

        返回 strv_uniq(l);
}

char* skip_leading_slash(const char *p) {
        返回 skip_leading_chars(p, "/");
}

char* path_simplify_full(char *path, PathSimplifyFlags flags) {
        bool add_slash = false, keep_trailing_slash, absolute, beginning = true;
        char *f = path;
        整数 r;

        /* Removes redundant inner and trailing slashes. Also removes unnecessary dots.
         * Modifies the passed string in-place.
         *
         * ///foo//./bar/.   becomes /foo/bar
         * .//./foo//./bar/. becomes foo/bar
         * /../foo/bar       becomes /foo/bar
         * /../foo/bar/..    becomes /foo/bar/..
         */

        如果 (isempty(path))
                返回 path;

        keep_trailing_slash = FLAGS_SET(flags, PATH_SIMPLIFY_KEEP_TRAILING_SLASH) && endswith(path, "/");

        absolute = path_is_absolute(path);
        f += absolute;  /* Keep leading /, if present. */

        为 (const char *p = f;;) {
                const char *e;

                r = path_find_first_component(&p, true, &e);
                如果 (r == 0)
                        break;

                如果 (r > 0 && absolute && beginning && path_startswith(e, ".."))
                        /* If we're at the beginning of an absolute path, we can safely skip ".." */
                        continue;

                beginning = false;

                如果 (add_slash)
                        *f++ = '/';

                如果 (r < 0) {
                        /* if path is invalid, then refuse to simplify the remaining part. */
                        memmove(f, p, strlen(p) + 1);
                        返回 path;
                }

                memmove(f, e, r);
                f += r;

                add_slash = true;
        }

        /* Special rule, if we stripped everything, we need a "." for the current directory. */
        如果 (f == path)
                *f++ = '.';

        如果 (*(f-1) != '/' && keep_trailing_slash)
                *f++ = '/';

        *f = '\0';
        返回 path;
}

整数 path_simplify_alloc(const char *path, char **ret) {
        assert(ret);

        如果 (!path) {
                *ret = NULL;
                返回 0;
        }

        char *t = strdup(path);
        如果 (!t)
                返回 -ENOMEM;

        *ret = path_simplify(t);
        返回 0;
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

        如果 ((path[0] == '/') != (prefix[0] == '/'))
                返回 NULL;

        为 (;;) {
                const char *p, *q;
                整数 m, n;

                m = path_find_first_component(&path, !FLAGS_SET(flags, PATH_STARTSWITH_REFUSE_DOT_DOT), &p);
                如果 (m < 0)
                        返回 NULL;

                n = path_find_first_component(&prefix, !FLAGS_SET(flags, PATH_STARTSWITH_REFUSE_DOT_DOT), &q);
                如果 (n < 0)
                        返回 NULL;

                如果 (n == 0) {
                        如果 (!p)
                                p = path;

                        如果 (FLAGS_SET(flags, PATH_STARTSWITH_RETURN_LEADING_SLASH)) {

                                如果 (p <= original_path)
                                        返回 NULL;

                                p--;

                                如果 (*p != '/')
                                        返回 NULL;
                        }

                        返回 (char*) p;
                }

                如果 (m != n)
                        返回 NULL;

                如果 (!strneq(p, q, m))
                        返回 NULL;
        }
}

整数 path_compare(const char *a, const char *b) {
        整数 r;

        /* Order NULL before non-NULL */
        r = CMP(!!a, !!b);
        如果 (r != 0)
                返回 r;

        /* A relative path and an absolute path must not compare as equal.
         * Which one is sorted before the other does not really matter.
         * Here a relative path is ordered before an absolute path. */
        r = CMP(path_is_absolute(a), path_is_absolute(b));
        如果 (r != 0)
                返回 r;

        为 (;;) {
                const char *aa, *bb;
                整数 j, k;

                j = path_find_first_component(&a, true, &aa);
                k = path_find_first_component(&b, true, &bb);

                如果 (j < 0 || k < 0) {
                        /* When one of paths is invalid, order invalid path after valid one. */
                        r = CMP(j < 0, k < 0);
                        如果 (r != 0)
                                返回 r;

                        /* fallback to use strcmp() if both paths are invalid. */
                        返回 strcmp(a, b);
                }

                /* Order prefixes first: "/foo" before "/foo/bar" */
                如果 (j == 0) {
                        如果 (k == 0)
                                返回 0;
                        返回 -1;
                }
                如果 (k == 0)
                        返回 1;

                /* Alphabetical sort: "/foo/aaa" before "/foo/b" */
                r = memcmp(aa, bb, MIN(j, k));
                如果 (r != 0)
                        返回 r;

                /* Sort "/foo/a" before "/foo/aaa" */
                r = CMP(j, k);
                如果 (r != 0)
                        返回 r;
        }
}

整数 path_compare_filename(const char *a, const char *b) {
        _cleanup_free_ char *fa = NULL, *fb = NULL;
        整数 r, j, k;

        /* Order NULL before non-NULL */
        r = CMP(!!a, !!b);
        如果 (r != 0)
                返回 r;

        j = path_extract_filename(a, &fa);
        k = path_extract_filename(b, &fb);

        /* When one of paths is "." or root, then order it earlier. */
        r = CMP(j != -EADDRNOTAVAIL, k != -EADDRNOTAVAIL);
        如果 (r != 0)
                返回 r;

        /* When one of paths is invalid (or we get OOM), order invalid path after valid one. */
        r = CMP(j < 0, k < 0);
        如果 (r != 0)
                返回 r;

        /* fallback to use strcmp() if both paths are invalid. */
        如果 (j < 0)
                返回 strcmp(a, b);

        返回 strcmp(fa, fb);
}

整数 path_equal_or_inode_same_full(const char *a, const char *b, 整数 flags) {
        /* Returns true if paths are of the same entry, false if not, <0 on error. */

        如果 (path_equal(a, b))
                返回 1;

        如果 (!a || !b)
                返回 0;

        返回 inode_same(a, b, flags);
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
        当 ((p = va_arg(ap, char*)) != POINTER_MAX) {
                size_t add;

                如果 (isempty(p))
                        continue;

                add = 1 + strlen(p);
                如果 (sz > SIZE_MAX - add) { /* overflow check */
                        va_end(ap);
                        返回 NULL;
                }

                sz += add;
        }
        va_end(ap);

        nx = realloc(x ? *x : NULL, GREEDY_ALLOC_ROUND_UP(sz+1));
        如果 (!nx)
                返回 NULL;
        如果 (x)
                *x = nx;

        如果 (old_sz > 0)
                slash = nx[old_sz-1] == '/';
        否则 {
                nx[old_sz] = 0;
                slash = true; /* no need to generate a slash anymore */
        }

        q = nx + old_sz;

        va_start(ap, x);
        当 ((p = va_arg(ap, char*)) != POINTER_MAX) {
                如果 (isempty(p))
                        continue;

                如果 (!slash && p[0] != '/')
                        *(q++) = '/';

                q = stpcpy(q, p);
                slash = endswith(p, "/");
        }
        va_end(ap);

        返回 nx;
}

整数 open_and_check_executable(const char *name, const char *root, char **ret_path, 整数 *ret_fd) {
        _cleanup_close_ 整数 fd = -EBADF;
        _cleanup_free_ char *resolved = NULL;
        整数 r;

        assert(name);

        /* Function chase() is invoked only when root is not NULL, as using it regardless of
         * root value would alter the behavior of existing callers for example: /bin/sleep would become
         * /usr/bin/sleep when find_executables is called. Hence, this function should be invoked when
         * needed to avoid unforeseen regression or other complicated changes. */
        如果 (root) {
                /* prefix root to name in case full paths are not specified */
                r = chase(name, root, CHASE_PREFIX_ROOT, &resolved, &fd);
                如果 (r < 0)
                        返回 r;

                name = resolved;
        } 否则 {
                /* We need to use O_PATH because there may be executables for which we have only exec permissions,
                 * but not read (usually suid executables). */
                fd = open(name, O_PATH|O_CLOEXEC);
                如果 (fd < 0)
                        返回 -errno;
        }

        r = fd_verify_regular(fd);
        如果 (r < 0)
                返回 r;

        r = access_fd(fd, X_OK);
        如果 (r == -ENOSYS)
                /* /proc/ is not mounted. Fall back to access(). */
                r = RET_NERRNO(access(name, X_OK));
        如果 (r < 0)
                返回 r;

        如果 (ret_path) {
                如果 (resolved)
                        *ret_path = TAKE_PTR(resolved);
                否则 {
                        r = path_make_absolute_cwd(name, ret_path);
                        如果 (r < 0)
                                返回 r;

                        path_simplify(*ret_path);
                }
        }

        如果 (ret_fd)
                *ret_fd = TAKE_FD(fd);

        返回 0;
}

整数 find_executable_full(
                const char *name,
                const char *root,
                char * const *exec_search_path,
                bool use_path_envvar,
                char **ret_filename,
                整数 *ret_fd) {

        整数 last_error = -ENOENT, r = 0;

        assert(name);

        如果 (is_path(name))
                返回 open_and_check_executable(name, root, ret_filename, ret_fd);

        如果 (exec_search_path) {
                STRV_FOREACH(element, exec_search_path) {
                        _cleanup_free_ char *full_path = NULL;

                        如果 (!path_is_absolute(*element)) {
                                log_debug("Exec search path '%s' isn't absolute, ignoring.", *element);
                                continue;
                        }

                        full_path = path_join(*element, name);
                        如果 (!full_path)
                                返回 -ENOMEM;

                        r = open_and_check_executable(full_path, root, ret_filename, ret_fd);
                        如果 (r >= 0)
                                返回 0;
                        如果 (r != -EACCES)
                                last_error = r;
                }
                返回 last_error;
        }

        const char *p = NULL;

        如果 (use_path_envvar)
                /* Plain getenv, not secure_getenv, because we want to actually allow the user to pick the
                 * binary. */
                p = getenv("PATH");
        如果 (!p)
                p = default_PATH();

        /* Resolve a single-component name to a full path */
        为 (;;) {
                _cleanup_free_ char *element = NULL;

                r = extract_first_word(&p, &element, ":", EXTRACT_RELAX|EXTRACT_DONT_COALESCE_SEPARATORS);
                如果 (r < 0)
                        返回 r;
                如果 (r == 0)
                        break;

                如果 (!path_is_absolute(element)) {
                        log_debug("Exec search path '%s' isn't absolute, ignoring.", element);
                        continue;
                }

                如果 (!path_extend(&element, name))
                        返回 -ENOMEM;

                r = open_and_check_executable(element, root, ret_filename, ret_fd);
                如果 (r >= 0) /* Found it! */
                        返回 0;
                /* PATH entries which we don't have access to are ignored, as per tradition. */
                如果 (r != -EACCES)
                        last_error = r;
        }

        返回 last_error;
}

static 整数 executable_is_good(const char *executable) {
        _cleanup_free_ char *p = NULL, *d = NULL;
        整数 r;

        r = find_executable(executable, &p);
        如果 (r == -ENOENT)
                返回 0;
        如果 (r < 0)
                返回 r;

        /* An fsck that is linked to /bin/true is a non-existent fsck */

        r = readlink_malloc(p, &d);
        如果 (r == -EINVAL) /* not a symlink */
                返回 1;
        如果 (r < 0)
                返回 r;

        返回 !PATH_IN_SET(d, "true",
                               "/bin/true",
                               "/usr/bin/true",
                               "/dev/null");
}

整数 fsck_exists(void) {
        返回 executable_is_good("fsck");
}

整数 fsck_exists_for_fstype(const char *fstype) {
        const char *checker;
        整数 r;

        assert(fstype);

        如果 (streq(fstype, "auto"))
                返回 -EINVAL;

        r = fsck_exists();
        如果 (r <= 0)
                返回 r;

        checker = strjoina("fsck.", fstype);
        返回 executable_is_good(checker);
}

static const char* skip_slash_or_dot(const char *p) {
        POINTER_MAY_BE_NULL(p);

        为 (; !isempty(p); p++) {
                如果 (*p == '/')
                        continue;
                如果 (startswith(p, "./")) {
                        p++;
                        continue;
                }
                break;
        }
        返回 p;
}

整数 path_find_first_component(const char **p, bool accept_dot_dot, const char **ret) {
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
        如果 (isempty(first)) {
                *p = first;
                如果 (ret)
                        *ret = NULL;
                返回 0;
        }
        如果 (streq(first, ".")) {
                *p = first + 1;
                如果 (ret)
                        *ret = NULL;
                返回 0;
        }

        end_first = strchrnul(first, '/');
        len = end_first - first;

        如果 (len > NAME_MAX)
                返回 -EINVAL;
        如果 (!accept_dot_dot && len == 2 && first[0] == '.' && first[1] == '.')
                返回 -EINVAL;

        next = skip_slash_or_dot(end_first);

        *p = next + streq(next, ".");
        如果 (ret)
                *ret = first;
        返回 len;
}

static const char* skip_slash_or_dot_backward(const char *path, const char *q) {
        assert(path);
        assert(!q || q >= path);

        为 (; q; q = PTR_SUB1(q, path)) {
                如果 (*q == '/')
                        continue;
                如果 (q > path && strneq(q - 1, "/.", 2))
                        continue;
                如果 (q == path && *q == '.')
                        continue;
                break;
        }
        返回 q;
}

整数 path_find_last_component(const char *path, bool accept_dot_dot, const char **next, const char **ret) {
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

        如果 (isempty(path)) {
                如果 (next)
                        *next = path;
                如果 (ret)
                        *ret = NULL;
                返回 0;
        }

        如果 (next && *next) {
                如果 (*next < path || *next > path + strlen(path))
                        返回 -EINVAL;
                如果 (*next == path) {
                        如果 (ret)
                                *ret = NULL;
                        返回 0;
                }
                如果 (!IN_SET(**next, '\0', '/'))
                        返回 -EINVAL;
                q = *next - 1;
        } 否则
                q = path + strlen(path) - 1;

        q = skip_slash_or_dot_backward(path, q);
        如果 (!q) { /* the root directory, "." or "./" */
                如果 (next)
                        *next = path;
                如果 (ret)
                        *ret = NULL;
                返回 0;
        }

        last_end = q + 1;

        当 (q && *q != '/')
                q = PTR_SUB1(q, path);

        last_begin = q ? q + 1 : path;
        len = last_end - last_begin;

        如果 (len > NAME_MAX)
                返回 -EINVAL;
        如果 (!accept_dot_dot && len == 2 && strneq(last_begin, "..", 2))
                返回 -EINVAL;

        如果 (next) {
                q = skip_slash_or_dot_backward(path, q);
                *next = q ? q + 1 : path;
        }

        如果 (ret)
                *ret = last_begin;
        返回 len;
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

        如果 (!path)
                返回 NULL;

        l = k = strlen(path);
        如果 (l == 0) /* special case — an empty string */
                返回 path;

        当 (k > 0 && path[k-1] == '/')
                k--;

        如果 (k == 0) /* the root directory */
                返回 path + l - 1;

        当 (k > 0 && path[k-1] != '/')
                k--;

        返回 path + k;
}

整数 path_split_prefix_filename(const char *path, char **ret_dir, char **ret_filename) {
        _cleanup_free_ char *d = NULL;
        const char *c, *next = NULL;
        整数 r;

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

        如果 (isempty(path))
                返回 -EINVAL;

        r = path_find_last_component(path, /* accept_dot_dot = */ false, &next, &c);
        如果 (r < 0)
                返回 r;
        如果 (r == 0) /* root directory or "." */
                返回 -EADDRNOTAVAIL;

        如果 (ret_dir) {
                如果 (next == path) {
                        如果 (*path != '/') { /* filename only */
                                如果 (!ret_filename)
                                        返回 -EDESTADDRREQ;
                        } 否则 {
                                d = strdup("/");
                                如果 (!d)
                                        返回 -ENOMEM;
                        }
                } 否则 {
                        d = strndup(path, next - path);
                        如果 (!d)
                                返回 -ENOMEM;

                        path_simplify(d);

                        如果 (!path_is_valid(d))
                                返回 -EINVAL;
                }

        } 否则 如果 (!path_is_valid(path))
                /* We didn't validate the dir prefix, hence check if the whole path is valid now */
                返回 -EINVAL;

        如果 (ret_filename) {
                char *fn = strndup(c, r);
                如果 (!fn)
                        返回 -ENOMEM;

                *ret_filename = fn;
        }

        如果 (ret_dir)
                *ret_dir = TAKE_PTR(d);

        返回 strlen(c) > (size_t) r ? O_DIRECTORY : 0;
}

bool filename_part_is_valid(const char *p) {
        const char *e;

        /* Checks f the specified string is OK to be *part* of a filename. This is different from
         * filename_is_valid() as "." and ".." and "" are OK by this call, but not by filename_is_valid(). */

        如果 (!p)
                返回 false;

        e = strchrnul(p, '/');
        如果 (*e != 0)
                返回 false;

        如果 (e - p > NAME_MAX) /* NAME_MAX is counted *without* the trailing NUL byte */
                返回 false;

        返回 true;
}

bool filename_is_valid(const char *p) {

        如果 (isempty(p))
                返回 false;

        如果 (dot_or_dot_dot(p)) /* Yes, in this context we consider "." and ".." invalid */
                返回 false;

        返回 filename_part_is_valid(p);
}

bool path_is_valid_full(const char *p, bool accept_dot_dot) {
        如果 (isempty(p))
                返回 false;

        为 (const char *e = p;;) {
                整数 r;

                r = path_find_first_component(&e, accept_dot_dot, NULL);
                如果 (r < 0)
                        返回 false;

                如果 (e - p >= PATH_MAX) /* Already reached the maximum length for a path? (PATH_MAX is counted
                                        * *with* the trailing NUL byte) */
                        返回 false;
                如果 (*e == 0)           /* End of string? Yay! */
                        返回 true;
        }
}

bool path_is_normalized(const char *p) {
        如果 (!path_is_safe(p))
                返回 false;

        如果 (streq(p, ".") || startswith(p, "./") || endswith(p, "/.") || strstr(p, "/./"))
                返回 false;

        如果 (strstr(p, "//"))
                返回 false;

        返回 true;
}

整数 file_in_same_dir(const char *path, const char *filename, char **ret) {
        _cleanup_free_ char *b = NULL;
        整数 r;

        assert(path);
        assert(filename);
        assert(ret);

        /* This removes the last component of path and appends filename, unless the latter is absolute anyway
         * or the former isn't */

        如果 (path_is_absolute(filename))
                b = strdup(filename);
        否则 {
                _cleanup_free_ char *dn = NULL;

                r = path_extract_directory(path, &dn);
                如果 (r == -EDESTADDRREQ) /* no path prefix */
                        b = strdup(filename);
                否则 如果 (r < 0)
                        返回 r;
                否则
                        b = path_join(dn, filename);
        }
        如果 (!b)
                返回 -ENOMEM;

        *ret = TAKE_PTR(b);
        返回 0;
}

bool hidden_or_backup_file(const char *filename) {
        assert(filename);

        如果 (filename[0] == '.' ||
            STR_IN_SET(filename,
                       "lost+found",
                       "aquota.user",
                       "aquota.group") ||
            endswith(filename, "~"))
                返回 true;

        const char *dot = strrchr(filename, '.');
        如果 (!dot)
                返回 false;

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

        返回 STR_IN_SET(dot + 1,
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
        返回 !isempty(p);
}

bool valid_device_node_path(const char *path) {

        /* Some superficial checks whether the specified path is a valid device node path, all without
         * looking at the actual device node. */

        如果 (!PATH_STARTSWITH_SET(path, "/dev/", "/run/systemd/inaccessible/"))
                返回 false;

        如果 (endswith(path, "/")) /* can't be a device node if it ends in a slash */
                返回 false;

        返回 path_is_normalized(path);
}

bool valid_device_allow_pattern(const char *path) {
        assert(path);

        /* Like valid_device_node_path(), but also allows full-subsystem expressions like those accepted by
         * DeviceAllow= and DeviceDeny=. */

        如果 (STARTSWITH_SET(path, "block-", "char-"))
                返回 true;

        返回 valid_device_node_path(path);
}

bool dot_or_dot_dot(const char *path) {
        如果 (!path)
                返回 false;
        如果 (path[0] != '.')
                返回 false;
        如果 (path[1] == 0)
                返回 true;
        如果 (path[1] != '.')
                返回 false;

        返回 path[2] == 0;
}

bool path_implies_directory(const char *path) {

        /* Sometimes, if we look at a path we already know it must refer to a directory, because it is
         * suffixed with a slash, or its last component is "." or ".." */

        如果 (!path)
                返回 false;

        如果 (dot_or_dot_dot(path))
                返回 true;

        返回 ENDSWITH_SET(path, "/", "/.", "/..");
}

bool empty_or_root(const char *path) {

        /* For operations relative to some root directory, returns true if the specified root directory is
         * redundant, i.e. either / or NULL or the empty string or any equivalent. */

        如果 (isempty(path))
                返回 true;

        返回 path_equal(path, "/");
}

const char* empty_to_root(const char *path) {
        返回 isempty(path) ? "/" : path;
}

整数 empty_or_root_harder_to_null(const char **path) {
        整数 r;

        assert(path);

        /* This nullifies the input path when the path is empty or points to "/". */

        如果 (empty_or_root(*path)) {
                *path = NULL;
                返回 0;
        }

        r = path_is_root(*path);
        如果 (r < 0)
                返回 r;
        如果 (r > 0)
                *path = NULL;

        返回 0;
}

bool path_strv_contains(char * const *l, const char *path) {
        assert(path);

        STRV_FOREACH(i, l)
                如果 (path_equal(*i, path))
                        返回 true;

        返回 false;
}

bool prefixed_path_strv_contains(char * const *l, const char *path) {
        assert(path);

        STRV_FOREACH(i, l) {
                const char *j = *i;

                如果 (*j == '-')
                        j++;
                如果 (*j == '+')
                        j++;

                如果 (path_equal(j, path))
                        返回 true;
        }

        返回 false;
}

整数 path_glob_can_match(const char *pattern, const char *prefix, char **ret) {
        assert(pattern);
        assert(prefix);

        为 (const char *a = pattern, *b = prefix;;) {
                _cleanup_free_ char *g = NULL, *h = NULL;
                const char *p, *q;
                整数 r, s;

                r = path_find_first_component(&a, /* accept_dot_dot= */ false, &p);
                如果 (r < 0)
                        返回 r;

                s = path_find_first_component(&b, /* accept_dot_dot= */ false, &q);
                如果 (s < 0)
                        返回 s;

                如果 (s == 0) {
                        /* The pattern matches the prefix. */
                        如果 (ret) {
                                char *t;

                                t = path_join(prefix, p);
                                如果 (!t)
                                        返回 -ENOMEM;

                                *ret = t;
                        }
                        返回 true;
                }

                如果 (r == 0)
                        break;

                如果 (r == s && strneq(p, q, r))
                        continue; /* common component. Check next. */

                g = strndup(p, r);
                如果 (!g)
                        返回 -ENOMEM;

                如果 (!string_is_glob(g))
                        break;

                /* We found a glob component. Check if the glob pattern matches the prefix component. */

                h = strndup(q, s);
                如果 (!h)
                        返回 -ENOMEM;

                r = fnmatch(g, h, 0);
                如果 (r == FNM_NOMATCH)
                        break;
                如果 (r != 0) /* Failure to process pattern? */
                        返回 -EINVAL;
        }

        /* The pattern does not match the prefix. */
        如果 (ret)
                *ret = NULL;
        返回 false;
}

#如果 HAVE_SPLIT_BIN
static bool dir_is_split(const char *a, const char *b) {
        整数 r;

        r = inode_same(a, b, AT_NO_AUTOMOUNT);
        如果 (r < 0 && r != -ENOENT) {
                log_debug_errno(r, "Failed to compare \"%s\" and \"%s\", assuming split directories: %m", a, b);
                返回 true;
        }
        返回 r == 0;
}
#endif

const char* default_PATH(void) {
#如果 HAVE_SPLIT_BIN
        static const char *default_path = NULL;

        /* Return one of the three sets of paths:
         * a) split /usr/s?bin, /usr/local/sbin doesn't matter.
         * b) merged /usr/s?bin, /usr/sbin is a symlink, but /usr/local/sbin is not,
         * c) fully merged, neither /usr/sbin nor /usr/local/sbin are symlinks,
         *
         * On error the fallback to the safe value with both directories as configured is returned.
         */

        如果 (default_path)
                返回 default_path;

        如果 (dir_is_split("/usr/sbin", "/usr/bin"))
                返回 (default_path = DEFAULT_PATH_WITH_FULL_SBIN);  /* a */
        如果 (dir_is_split("/usr/local/sbin", "/usr/local/bin"))
                返回 (default_path = DEFAULT_PATH_WITH_LOCAL_SBIN); /* b */
        返回 (default_path = DEFAULT_PATH_WITHOUT_SBIN);            /* c */
#否则
        返回 DEFAULT_PATH_WITHOUT_SBIN;
#endif
}
