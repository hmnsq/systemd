/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ଅନ୍ତର୍ଭାଵ <dirent.h>
#ଅନ୍ତର୍ଭାଵ <sys/stat.h>

#ଅନ୍ତର୍ଭାଵ "dirent-util.h"
#ଅନ୍ତର୍ଭାଵ "errno-util.h"
#ଅନ୍ତର୍ଭାଵ "glob-util.h"
#ଅନ୍ତର୍ଭାଵ "string-util.h"
#ଅନ୍ତର୍ଭାଵ "strv.h"

#ifndef __GLIBC__
ସ୍ଥିର bool safe_glob_verify(ଅଚଳ ଅକ୍ଷର *p, ଅଚଳ ଅକ୍ଷର *prefix) {
        POINTER_MAY_BE_NULL(p);
        POINTER_MAY_BE_NULL(prefix);

        ଯଦି (isempty(p))
                ଫେରନ୍ତୁ false; /* should not happen, but for safey. */

        ଯଦି (prefix) {
                /* Skip the prefix, as we allow dots in prefix.
                 * Note, glob() does not normalize paths, hence do not use path_startswith(). */
                p = startswith(p, prefix);
                ଯଦି (!p)
                        ଫେରନ୍ତୁ false; /* should not happen, but for safety. */
        }

        ପାଇଁ (;;) {
                p += strspn(p, "/");
                ଯଦି (*p == '\0')
                        ଫେରନ୍ତୁ true;
                ଯଦି (*p == '.') {
                        p++;
                        ଯଦି (IN_SET(*p, '/', '\0'))
                                ଫେରନ୍ତୁ false; /* refuse dot */
                        ଯଦି (*p == '.') {
                                p++;
                                ଯଦି (IN_SET(*p, '/', '\0'))
                                        ଫେରନ୍ତୁ false; /* refuse dot dot */
                        }
                }

                p += strcspn(p, "/");
                ଯଦି (*p == '\0')
                        ଫେରନ୍ତୁ true;
        }
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା filter_glob_result(ଅକ୍ଷର * ଅଚଳ *result, ଅଚଳ ଅକ୍ଷର *path, ଅକ୍ଷର ***ret) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(path);

        ଯଦି (strv_isempty(result))
                ଫେରନ୍ତୁ -ENOENT;

        _cleanup_free_ ଅକ୍ଷର *prefix = ଖାଲି;
        r = glob_non_glob_prefix(path, &prefix);
        ଯଦି (r < 0 && r != -ENOENT)
                ଫେରନ୍ତୁ r;

        _cleanup_strv_free_ ଅକ୍ଷର **filtered = ଖାଲି;
        size_t n_filtered = 0;
        STRV_FOREACH(p, result) {
                ଯଦି (!safe_glob_verify(*p, prefix))
                        ଜାରି;

                ଯଦି (!ret)
                        ଫେରନ୍ତୁ 0; /* Found at least one entry, let's return earlier. */

                /* When musl is used, each entry is not a head of allocated memory. Hence, it is
                 * necessary to copy the string. */
                r = strv_extend_with_size(&filtered, &n_filtered, *p);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;
        }

        ଯଦି (n_filtered == 0)
                ଫେରନ୍ତୁ -ENOENT;

        ପ୍ରତିଜ୍ଞା(ret);
        *ret = TAKE_PTR(filtered);
        ଫେରନ୍ତୁ 0;
}
#endif

DEFINE_TRIVIAL_DESTRUCTOR(closedir_wrapper, ଶୂନ୍ୟ, closedir);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା safe_glob_full(ଅଚଳ ଅକ୍ଷର *path, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags, opendir_t opendir_func, ଅକ୍ଷର ***ret) {
        _cleanup_(globfree) glob_t g = {
                .gl_closedir = closedir_wrapper,
                .gl_readdir = (ସଂରଚନା dirent* (*)(ଶୂନ୍ୟ *)) readdir_no_dot,
                .gl_opendir = (ଶୂନ୍ୟ* (*)(ଅଚଳ ଅକ୍ଷର *)) (opendir_func ?: opendir),
                .gl_lstat = lstat,
                .gl_stat = stat,
        };
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(path);

        errno = 0;
        r = glob(path, flags | GLOB_ALTDIRFUNC, ଖାଲି, &g);
        ଯଦି (r == GLOB_NOMATCH)
                ଫେରନ୍ତୁ -ENOENT;
        ଯଦି (r == GLOB_NOSPACE)
                ଫେରନ୍ତୁ -ENOMEM;
        ଯଦି (r != 0)
                ଫେରନ୍ତୁ errno_or_else(EIO);

#ifndef __GLIBC__
        ଫେରନ୍ତୁ filter_glob_result(g.gl_pathv, path, ret);
#endif

        ଯଦି (strv_isempty(g.gl_pathv))
                ଫେରନ୍ତୁ -ENOENT;

        ଯଦି (ret) {
                *ret = g.gl_pathv;
                TAKE_STRUCT(g); /* To avoid the result being freed. */
        }

        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା glob_first(ଅଚଳ ଅକ୍ଷର *path, ଅକ୍ଷର **ret) {
        _cleanup_strv_free_ ଅକ୍ଷର **v = ଖାଲି;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(path);

        r = safe_glob(path, GLOB_NOSORT|GLOB_BRACE, &v);
        ଯଦି (r == -ENOENT) {
                ଯଦି (ret)
                        *ret = ଖାଲି;
                ଫେରନ୍ତୁ false;
        }
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ପ୍ରତିଜ୍ଞା(!strv_isempty(v));

        ଯଦି (ret) {
                /* Free all results except for the first one. */
                STRV_FOREACH(p, strv_skip(v, 1))
                        *p = mfree(*p);

                /* Then, take the first result. */
                *ret = TAKE_PTR(*v);
        }

        ଫେରନ୍ତୁ true;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା glob_extend(ଅକ୍ଷର ***strv, ଅଚଳ ଅକ୍ଷର *path, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags) {
        ଅକ୍ଷର **v;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(path);

        r = safe_glob(path, GLOB_NOSORT|GLOB_BRACE|flags, &v);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଫେରନ୍ତୁ strv_extend_strv_consume(strv, v, /* filter_duplicates= */ false);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା glob_non_glob_prefix(ଅଚଳ ଅକ୍ଷର *path, ଅକ୍ଷର **ret) {
        ପ୍ରତିଜ୍ଞା(path);
        ପ୍ରତିଜ୍ଞା(ret);

        /* Return the path of the path that has no glob characters. */

        size_t n = strcspn(path, GLOB_CHARS);

        ଯଦି (path[n] != '\0')
                ଯେତେବେଳେ (n > 0 && path[n-1] != '/')
                        n--;

        ଯଦି (n == 0)
                ଫେରନ୍ତୁ -ENOENT;

        ଅକ୍ଷର *ans = strndup(path, n);
        ଯଦି (!ans)
                ଫେରନ୍ତୁ -ENOMEM;
        *ret = ans;
        ଫେରନ୍ତୁ 0;
}

bool string_is_glob(ଅଚଳ ଅକ୍ଷର *p) {
        /* Check if a string contains any glob patterns. */
        ଫେରନ୍ତୁ !!strpbrk(p, GLOB_CHARS);
}
