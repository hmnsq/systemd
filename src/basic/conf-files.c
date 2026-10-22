/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن <stdio.h>
#ضمن <stdlib.h>

#ضمن "alloc-util.h"
#ضمن "chase.h"
#ضمن "conf-files.h"
#ضمن "dirent-util.h"
#ضمن "errno-util.h"
#ضمن "fd-util.h"
#ضمن "fileio.h"
#ضمن "glyph-util.h"
#ضمن "hashmap.h"
#ضمن "log.h"
#ضمن "nulstr-util.h"
#ضمن "path-util.h"
#ضمن "set.h"
#ضمن "stat-util.h"
#ضمن "string-util.h"
#ضمن "strv.h"

ConfFile* conf_file_free(ConfFile *c) {
        إذا (!c)
                أرجع عدم;

        حرر(c->filename);
        حرر(c->پایله);
        حرر(c->original_لار);
        حرر(c->resolved_لار);
        خوندي_close(c->fd);

        أرجع mfree(c);
}

DEFINE_اشاره_ګر_ARRAY_FREE_FUNC(ConfFile*, conf_file_free);

ساكن صحيح conf_files_log_level(ConfFilesFlags flags) {
        أرجع FLAGS_سېټ(flags, CONF_FILES_WARN) ? LOG_WARNING : LOG_DEBUG;
}

ساكن صحيح prepare_dirs(
                ثابت حرف *ریښه,
                ConfFilesFlags flags,
                حرف * ثابت *dirs,
                حرف **ret_ریښه,
                صحيح *ret_rfd,
                حرف ***ret_dirs) {

        _cleanup_free_ حرف *ریښه_abs = عدم;
        _cleanup_strv_free_ حرف **dirs_abs = عدم;
        صحيح r;

        أكد(ret_ریښه);
        أكد(ret_rfd);
        أكد(ret_dirs || strv_isempty(dirs));

        صحيح log_level = conf_files_log_level(flags);

        r = تشه_or_ریښه_harder_منزل_null(&ریښه);
        إذا (r < 0)
                أرجع log_ډک_errno(log_level, r, "Failed to determine if '%s' points to the root directory: %m", strempty(ریښه));

        إذا (ret_dirs) {
                dirs_abs = strv_copy(dirs);
                إذا (!dirs_abs)
                        أرجع log_oom_ډک(log_level);
        }

        _cleanup_close_ صحيح rfd = XAT_FDROOT;
        إذا (ریښه) {
                /* When a non-trivial root is specified, we will prefix the result later. Hence, it is not
                 * necessary to modify each config directories here. but needs to normalize the root directory. */
                r = لار_make_absolute_cwd(ریښه, &ریښه_abs);
                إذا (r < 0)
                        أرجع log_ډک_errno(log_level, r, "Failed to make '%s' absolute: %m", ریښه);

                لار_simplify(ریښه_abs);

                rfd = open(ریښه, O_CLOEXEC|O_DIRECTORY|O_لار);
                إذا (rfd < 0)
                        أرجع log_ډک_errno(log_level, errno, "Failed to open '%s': %m", ریښه_abs);

        } وإلا إذا (ret_dirs) {
                /* When an empty root or "/" is specified, we will open "/" below, hence we need to make
                 * each config directory absolute if relative. */
                r = لار_strv_make_absolute_cwd(dirs_abs);
                إذا (r < 0)
                        أرجع log_ډک_errno(log_level, r, "Failed to make directories absolute: %m");
        }

        *ret_ریښه = TAKE_PTR(ریښه_abs);
        *ret_rfd = TAKE_FD(rfd);
        إذا (ret_dirs)
                *ret_dirs = TAKE_PTR(dirs_abs);
        أرجع 0;
}

ساكن صحيح conf_file_مختاړی_ریښه(ConfFile *c, ثابت حرف *ریښه, ConfFilesFlags flags) {
        حرف *p;
        صحيح r;

        أكد(c);

        إذا (FLAGS_سېټ(flags, CONF_FILES_DONT_مختاړی_ریښه))
                أرجع 0;

        صحيح log_level = conf_files_log_level(flags);

        r = chaseat_مختاړی_ریښه(c->پایله, ریښه, &p);
        إذا (r < 0)
                أرجع log_ډک_errno(log_level, r, "Failed to prefix '%s' with root '%s': %m", c->پایله, ریښه);
        free_and_replace(c->پایله, p);

        r = chaseat_مختاړی_ریښه(c->resolved_لار, ریښه, &p);
        إذا (r < 0)
                أرجع log_ډک_errno(log_level, r, "Failed to prefix '%s' with root '%s': %m", c->resolved_لار, ریښه);
        free_and_replace(c->resolved_لار, p);

        /* Do not use chaseat_prefix_root(), as it is for the result of chaseat(), but the path is not chased. */
        p = لار_join(تشه_منزل_ریښه(ریښه), skip_leading_slash(c->original_لار));
        إذا (!p)
                أرجع log_oom_ډک(log_level);

        لار_simplify(p);
        أرجع free_and_replace(c->original_لار, p);
}

ساكن bool conf_files_need_stat(ConfFilesFlags flags) {
        أرجع (flags & (CONF_FILES_FILTER_MASKED | CONF_FILES_REGULAR | CONF_FILES_DIRECTORY | CONF_FILES_EXECUTABLE)) != 0;
}

ساكن ChaseFlags conf_files_chase_flags(ConfFilesFlags flags) {
        ChaseFlags chase_flags = 0;

        إذا (!conf_files_need_stat(flags) || FLAGS_سېټ(flags, CONF_FILES_FILTER_MASKED_BY_SYMLINK))
                /* Even if no verification is requested, let's unconditionally call chaseat(),
                 * to drop unsafe symlinks. */
                chase_flags |= CHASE_NONEXISTENT;

        أرجع chase_flags;
}

ساكن صحيح conf_file_chase_and_verify(
                ثابت حرف *ریښه,          /* for logging, can be NULL */
                صحيح rfd,
                ثابت حرف *original_لار, /* for logging */
                ثابت حرف *لار,
                ثابت حرف *name,
                سېټ_2 **masked,              /* optional */
                ConfFilesFlags flags,
                حرف **ret_لار,
                صحيح *ret_fd,
                بنية stat *ret_stat) {

        _cleanup_free_ حرف *resolved_لار = عدم;
        _cleanup_close_ صحيح fd = -EBADF;
        بنية stat st = {};
        صحيح r;

        أكد(wildcard_fd_is_valid(rfd));
        أكد(original_لار);
        أكد(لار);
        أكد(name);

        صحيح log_level = conf_files_log_level(flags);

        ریښه = تشه_منزل_ریښه(ریښه);

        r = chaseat(rfd, rfd, لار, conf_files_chase_flags(flags), &resolved_لار, &fd);
        إذا (r < 0)
                أرجع log_ډک_errno(log_level, r, "Failed to chase '%s%s': %m",
                                      ریښه, skip_leading_slash(original_لار));
        إذا (r == 0) {
                إذا (FLAGS_سېټ(flags, CONF_FILES_FILTER_MASKED_BY_SYMLINK)) {
                        /* If the path points to /dev/null in a image or so, then the device node may not exist. */
                        إذا (لار_equal(skip_leading_slash(resolved_لار), "dev/null")) {
                                إذا (masked) {
                                        /* Mark this one as masked */
                                        r = سېټ_put_strdup(masked, name);
                                        إذا (r < 0)
                                                أرجع log_oom_ډک(log_level);
                                }

                                أرجع log_debug_errno(SYNTHETIC_ERRNO(ERFKILL),
                                                       "File '%s%s' is a mask (symlink to /dev/null).",
                                                       ریښه, skip_leading_slash(original_لار));
                        }
                }

                إذا (conf_files_need_stat(flags))
                        /* If we need to have stat, skip the entry. */
                        أرجع log_ډک_errno(log_level, SYNTHETIC_ERRNO(ENOENT), "Failed to chase '%s%s': %m",
                                              ریښه, skip_leading_slash(original_لار));
        }

        /* Even if we do not need stat, let's take stat now. The caller may use the info later. */
        إذا (fd >= 0 && fstat(fd, &st) < 0)
                أرجع log_ډک_errno(log_level, errno, "Failed to stat '%s%s': %m",
                                      ریښه, skip_leading_slash(original_لار));

        /* Is this a masking entry? */
        إذا (FLAGS_سېټ(flags, CONF_FILES_FILTER_MASKED_BY_SYMLINK) && stat_may_be_dev_null(&st)) {
                إذا (masked) {
                        /* Mark this one as masked */
                        r = سېټ_put_strdup(masked, name);
                        إذا (r < 0)
                                أرجع log_oom_ډک(log_level);
                }

                أرجع log_debug_errno(SYNTHETIC_ERRNO(ERFKILL),
                                       "File '%s%s' is a mask (symlink to /dev/null).",
                                       ریښه, skip_leading_slash(original_لار));
        }

        إذا (FLAGS_سېټ(flags, CONF_FILES_FILTER_MASKED_BY_تشه) && stat_is_تشه(&st)) {
                إذا (masked) {
                        /* Mark this one as masked */
                        r = سېټ_put_strdup(masked, name);
                        إذا (r < 0)
                                أرجع log_oom_ډک(log_level);
                }

                أرجع log_debug_errno(SYNTHETIC_ERRNO(ERFKILL),
                                       "File '%s%s' is a mask (an empty file).",
                                       ریښه, skip_leading_slash(original_لار));
        }

        إذا (FLAGS_سېټ(flags, CONF_FILES_REGULAR|CONF_FILES_DIRECTORY)) {
                إذا (!S_ISREG(st.st_mode) && !S_ISDIR(st.st_mode))
                        أرجع log_debug_errno(SYNTHETIC_ERRNO(EBADFD),
                                               "File '%s%s' is neither a regular file or directory.",
                                               ریښه, skip_leading_slash(original_لار));
        } وإلا {
                /* Is this node a regular file? */
                إذا (FLAGS_سېټ(flags, CONF_FILES_REGULAR)) {
                        r = stat_verify_regular(&st);
                        إذا (r < 0)
                                أرجع log_debug_errno(r, "File '%s%s' is not a regular file: %m",
                                                       ریښه, skip_leading_slash(original_لار));
                }

                /* Is this node a directory? */
                إذا (FLAGS_سېټ(flags, CONF_FILES_DIRECTORY)) {
                        r = stat_verify_directory(&st);
                        إذا (r < 0)
                                أرجع log_debug_errno(r, "File '%s%s' is not a directory: %m",
                                                       ریښه, skip_leading_slash(original_لار));
                }
        }

        /* Does this node have the executable bit set?
         * As requested: check if the file is marked executable. Note that we don't check access(X_OK) here,
         * as we care about whether the file is marked executable at all, and not whether it is executable
         * for us, because if so, such errors are stuff we should log about. */
        إذا (FLAGS_سېټ(flags, CONF_FILES_EXECUTABLE) && (st.st_mode & 0111) == 0)
                أرجع log_debug_errno(SYNTHETIC_ERRNO(ENOEXEC),
                                       "File '%s%s' is not marked executable.",
                                       ریښه, skip_leading_slash(original_لار));

        إذا (ret_لار)
                *ret_لار = TAKE_PTR(resolved_لار);
        إذا (ret_fd)
                *ret_fd = TAKE_FD(fd);
        إذا (ret_stat)
                *ret_stat = st;

        أرجع 0;
}

صحيح conf_file_new_at(
                ثابت حرف *لار,
                ثابت حرف *ریښه,
                صحيح rfd,
                ConfFilesFlags flags,
                ConfFile **ret) {
        صحيح r;

        أكد(لار);
        أكد(wildcard_fd_is_valid(rfd));
        أكد(ret);

        صحيح log_level = conf_files_log_level(flags);

        _cleanup_free_ حرف *ریښه_2 = عدم;
        إذا (DEBUG_LOGGING && !ریښه) {
                (فراغ) fd_get_لار(rfd, &ریښه_2);
                ریښه = ریښه_2;
        }

        _cleanup_(conf_file_freep) ConfFile *c = new(ConfFile, 1);
        إذا (!c)
                أرجع log_oom_ډک(log_level);

        *c = (ConfFile) {
                .original_لار = strdup(لار),
                .fd = -EBADF,
        };

        إذا (!c->original_لار)
                أرجع log_oom_ډک(log_level);

        r = لار_ایستل_filename(لار, &c->filename);
        إذا (r < 0)
                أرجع log_ډک_errno(log_level, r, "Failed to extract filename from '%s': %m", لار);

        _cleanup_free_ حرف *dirpath = عدم, *resolved_dirpath = عدم;
        r = لار_ایستل_directory(لار, &dirpath);
        إذا (r < 0 && r != -EDESTADDRREQ)
                أرجع log_ډک_errno(log_level, r, "Failed to extract directory from '%s': %m", لار);
        إذا (r >= 0) {
                r = chaseat(rfd, rfd, dirpath,
                            CHASE_MUST_BE_DIRECTORY | conf_files_chase_flags(flags),
                            &resolved_dirpath, /* ret_fd= */ عدم);
                إذا (r < 0)
                        أرجع log_ډک_errno(log_level, r, "Failed to chase '%s%s': %m", تشه_منزل_ریښه(ریښه), skip_leading_slash(dirpath));
        }

        c->پایله = لار_join(resolved_dirpath, c->filename);
        إذا (!c->پایله)
                أرجع log_oom_ډک(log_level);

        r = conf_file_chase_and_verify(
                        ریښه,
                        rfd,
                        c->original_لار,
                        c->پایله,
                        c->filename,
                        /* masked= */ عدم,
                        flags,
                        &c->resolved_لار,
                        &c->fd,
                        &c->st);
        إذا (r < 0)
                أرجع r;

        *ret = TAKE_PTR(c);
        أرجع 0;
}

صحيح conf_file_new(ثابت حرف *لار, ثابت حرف *ریښه, ConfFilesFlags flags, ConfFile **ret) {
        صحيح r;

        أكد(لار);
        أكد(ret);

        _cleanup_free_ حرف *ریښه_abs = عدم;
        _cleanup_close_ صحيح rfd = -EBADF;
        r = prepare_dirs(ریښه, flags, /* dirs= */ عدم, &ریښه_abs, &rfd, /* ret_dirs= */ عدم);
        إذا (r < 0)
                أرجع r;

        _cleanup_free_ حرف *لار_abs = عدم;
        إذا (!ریښه_abs) {
                r = لار_make_absolute_cwd(لار, &لار_abs);
                إذا (r < 0)
                        أرجع log_ډک_errno(conf_files_log_level(flags), r, "Failed to make '%s' absolute: %m", لار);

                لار = لار_abs;
        }

        _cleanup_(conf_file_freep) ConfFile *c = عدم;
        r = conf_file_new_at(لار, ریښه_abs, rfd, flags, &c);
        إذا (r < 0)
                أرجع r;

        r = conf_file_مختاړی_ریښه(c, ریښه_abs, flags);
        إذا (r < 0)
                أرجع r;

        *ret = TAKE_PTR(c);
        أرجع 0;
}

DEFINE_شخصي_HASH_OPS_WITH_ارزښت_DESTRUCTOR(
                conf_file_hash_ops,
                حرف, string_hash_func, string_compare_func,
                ConfFile, conf_file_free);

ساكن صحيح files_جمع_کړه(
                DIR *dir,
                ثابت حرف *original_dirpath,
                ثابت حرف *resolved_dirpath,
                ثابت حرف *ریښه, /* for logging, can be NULL */
                صحيح rfd,
                Hashmap **files,
                سېټ_2 **masked,
                ثابت حرف *suffix,
                ConfFilesFlags flags) {

        صحيح r;

        أكد(dir);
        أكد(original_dirpath);
        أكد(resolved_dirpath);
        أكد(wildcard_fd_is_valid(rfd));
        أكد(files);
        أكد(masked);

        صحيح log_level = conf_files_log_level(flags);

        ریښه = تشه_منزل_ریښه(ریښه);

        FOREACH_DIRENT(de, dir, أرجع log_ډک_errno(log_level, errno, "Failed to read directory '%s%s': %m",
                                                      ریښه, skip_leading_slash(original_dirpath))) {

                _cleanup_free_ حرف *original_لار = لار_join(original_dirpath, de->d_name);
                إذا (!original_لار)
                        أرجع log_oom_ډک(log_level);

                /* Does this match the suffix? */
                إذا (suffix && !endswith(de->d_name, suffix)) {
                        log_debug("Skipping file '%s%s', suffix is not '%s'.", ریښه, skip_leading_slash(original_لار), suffix);
                        استمر;
                }

                /* Has this file already been found in an earlier directory? */
                إذا (hashmap_لري(*files, de->d_name)) {
                        log_debug("Skipping overridden file '%s%s'.", ریښه, skip_leading_slash(original_لار));
                        استمر;
                }

                /* Has this been masked in an earlier directory? */
                إذا ((flags & CONF_FILES_FILTER_MASKED) != 0 && سېټ_لري(*masked, de->d_name)) {
                        log_debug("File '%s%s' is masked by previous entry.", ریښه, skip_leading_slash(original_لار));
                        استمر;
                }

                _cleanup_free_ حرف *p = لار_join(resolved_dirpath, de->d_name);
                إذا (!p)
                        أرجع log_oom_ډک(log_level);

                _cleanup_free_ حرف *resolved_لار = عدم;
                _cleanup_close_ صحيح fd = -EBADF;
                بنية stat st;
                r = conf_file_chase_and_verify(
                                ریښه,
                                rfd,
                                original_لار,
                                p,
                                de->d_name,
                                masked,
                                flags,
                                &resolved_لار,
                                &fd,
                                &st);
                إذا (r == -ENOMEM)
                        أرجع r;
                إذا (r < 0)
                        استمر;

                _cleanup_(conf_file_freep) ConfFile *c = new(ConfFile, 1);
                إذا (!c)
                        أرجع log_oom_ډک(log_level);

                *c = (ConfFile) {
                        .filename = strdup(de->d_name),
                        .پایله = TAKE_PTR(p),
                        .original_لار = TAKE_PTR(original_لار),
                        .resolved_لار = TAKE_PTR(resolved_لار),
                        .fd = TAKE_FD(fd),
                        .st = st,
                };

                إذا (!c->filename)
                        أرجع log_oom_ډک(log_level);

                r = hashmap_ensure_put(files, &conf_file_hash_ops, c->filename, c);
                إذا (r < 0) {
                        أكد(r == -ENOMEM);
                        أرجع log_oom_ډک(log_level);
                }
                أكد(r > 0);

                TAKE_PTR(c);
        }

        أرجع 0;
}

ساكن صحيح dump_files(Hashmap *fh, ثابت حرف *ریښه, ConfFilesFlags flags, ConfFile ***ret_files, size_t *ret_n_files) {
        ConfFile **files = عدم;
        size_t n_files = 0;
        صحيح r;

        CLEANUP_ARRAY(files, n_files, conf_file_free_array);

        أكد(ret_files);
        أكد(ret_n_files);

        /* The entries in the array given by hashmap_dump_sorted() are still owned by the hashmap. */
        r = hashmap_dump_sorted(fh, (فراغ***) &files, &n_files);
        إذا (r < 0)
                أرجع log_oom_ډک(conf_files_log_level(flags));

        /* Hence, we need to remove them from the hashmap. */
        FOREACH_ARRAY(i, files, n_files)
                assert_se(hashmap_remove(fh, (*i)->filename) == *i);

        إذا (ریښه)
                FOREACH_ARRAY(i, files, n_files) {
                        r = conf_file_مختاړی_ریښه(*i, ریښه, flags);
                        إذا (r < 0)
                                أرجع r;
                }

        *ret_files = TAKE_PTR(files);
        *ret_n_files = n_files;
        أرجع 0;
}

ساكن صحيح copy_and_ترتیبول_files_from_hashmap(
                Hashmap *fh,
                ثابت حرف *suffix,
                ثابت حرف *ریښه,
                ConfFilesFlags flags,
                حرف ***ret) {

        _cleanup_strv_free_ حرف **results = عدم;
        _cleanup_free_ ConfFile **files = عدم;
        size_t n_files = 0, n_results = 0;
        صحيح r;

        أكد(ret);

        صحيح log_level = conf_files_log_level(flags);

        /* The entries in the array given by hashmap_dump_sorted() are still owned by the hashmap.
         * Hence, do not use conf_file_free_array() for 'entries' */
        r = hashmap_dump_sorted(fh, (فراغ***) &files, &n_files);
        إذا (r < 0)
                أرجع log_oom_ډک(log_level);

        FOREACH_ARRAY(i, files, n_files) {
                ConfFile *c = *i;
                ثابت حرف *جمع_کړه = عدم;

                إذا (FLAGS_سېټ(flags, CONF_FILES_BASENAME))
                        جمع_کړه = c->filename;
                وإلا إذا (ریښه && !FLAGS_سېټ(flags, CONF_FILES_DONT_مختاړی_ریښه)) {
                        _cleanup_free_ حرف *p = عدم;

                        r = chaseat_مختاړی_ریښه(c->پایله, ریښه, &p);
                        إذا (r < 0)
                                أرجع log_ډک_errno(log_level, r, "Failed to prefix '%s' with root '%s': %m", c->پایله, ریښه);

                        إذا (FLAGS_سېټ(flags, CONF_FILES_TRUNCATE_SUFFIX) && suffix) {
                                حرف *e = endswith(p, suffix);
                                إذا (!e)
                                        استمر;

                                *e = 0;
                        }

                        إذا (strv_consume_with_اندازه(&results, &n_results, TAKE_PTR(p)) < 0)
                                أرجع log_oom_ډک(log_level);

                        استمر;
                } وإلا
                        جمع_کړه = c->پایله;

                إذا (FLAGS_سېټ(flags, CONF_FILES_TRUNCATE_SUFFIX)) {
                        ثابت حرف *e = endswith(جمع_کړه, suffix);
                        إذا (!e)
                                استمر;

                        _cleanup_free_ حرف *n = strndup(جمع_کړه, e - جمع_کړه);
                        إذا (!n)
                                أرجع log_oom_ډک(log_level);

                        r = strv_consume_with_اندازه(&results, &n_results, TAKE_PTR(n));
                } وإلا
                        r = strv_extend_with_اندازه(&results, &n_results, جمع_کړه);
                إذا (r < 0)
                        أرجع log_oom_ډک(log_level);
        }

        *ret = TAKE_PTR(results);
        أرجع 0;
}

ساكن صحيح ورننباسه_replacement(Hashmap **fh, ConfFile *replacement, ConfFilesFlags flags, ثابت ConfFile **ret) {
        _cleanup_(conf_file_freep) ConfFile *c = ASSERT_PTR(replacement);
        صحيح r;

        أكد(fh);
        أكد(ret);

        /* This consumes the input ConfFile. */

        ConfFile *existing = hashmap_get(*fh, c->filename);
        إذا (existing) {
                log_debug("An entry with higher priority '%s' -> '%s' already exists, ignoring the replacement: %s",
                          existing->filename, existing->پایله, c->original_لار);
                *ret = عدم;
                أرجع 0;
        }

        r = hashmap_ensure_put(fh, &conf_file_hash_ops, c->filename, c);
        إذا (r < 0) {
                أكد(r == -ENOMEM);
                أرجع log_oom_ډک(conf_files_log_level(flags));
        }
        أكد(r > 0);

        log_debug("Inserted replacement: '%s' -> '%s'", c->filename, c->پایله);

        *ret = TAKE_PTR(c);
        أرجع 0;
}

ساكن صحيح conf_files_list_impl(
                ثابت حرف *suffix,
                ثابت حرف *ریښه, /* for logging, can be NULL */
                صحيح rfd,
                ConfFilesFlags flags,
                ثابت حرف * ثابت *dirs,
                ثابت حرف *replacement,
                Hashmap **ret,
                ثابت ConfFile **ret_inserted) {

        _cleanup_hashmap_free_ Hashmap *fh = عدم;
        cleanup_سېټ_free سېټ_2 *masked = عدم;
        _cleanup_(conf_file_freep) ConfFile *c = عدم;
        ثابت ConfFile *inserted = عدم;
        صحيح r;

        أكد(wildcard_fd_is_valid(rfd));
        أكد(ret);

        ریښه = تشه_منزل_ریښه(ریښه);

        إذا (replacement) {
                r = conf_file_new_at(replacement, ریښه, rfd, flags & CONF_FILES_WARN, &c);
                إذا (r < 0)
                        أرجع r;
        }

        STRV_FOREACH(p, dirs) {
                _cleanup_closedir_ DIR *dir = عدم;
                _cleanup_free_ حرف *لار = عدم;

                r = chase_and_opendirat(rfd, rfd, *p, 0, &لار, &dir);
                إذا (r < 0) {
                        إذا (r != -ENOENT)
                                log_ډک_errno(conf_files_log_level(flags), r,
                                               "Failed to chase and open directory '%s%s', ignoring: %m",
                                               ریښه, skip_leading_slash(*p));
                        استمر;
                }

                إذا (c && streq_ptr(لار_startswith(c->پایله, لار), c->filename)) {
                        r = ورننباسه_replacement(&fh, TAKE_PTR(c), flags, &inserted);
                        إذا (r < 0)
                                أرجع r;
                }

                r = files_جمع_کړه(dir, *p, لار, ریښه, rfd, &fh, &masked, suffix, flags);
                إذا (r == -ENOMEM)
                        أرجع r;
        }

        إذا (c) {
                r = ورننباسه_replacement(&fh, TAKE_PTR(c), flags, &inserted);
                إذا (r < 0)
                        أرجع r;
        }

        *ret = TAKE_PTR(fh);
        إذا (ret_inserted)
                *ret_inserted = inserted;
        أرجع 0;
}

صحيح conf_files_list_strv(
                حرف ***ret,
                ثابت حرف *suffix,
                ثابت حرف *ریښه,
                ConfFilesFlags flags,
                ثابت حرف * ثابت *dirs) {

        _cleanup_hashmap_free_ Hashmap *fh = عدم;
        _cleanup_close_ صحيح rfd = -EBADF;
        _cleanup_free_ حرف *ریښه_abs = عدم;
        _cleanup_strv_free_ حرف **dirs_abs = عدم;
        صحيح r;

        أكد(ret);

        r = prepare_dirs(ریښه, flags, (حرف**) dirs, &ریښه_abs, &rfd, &dirs_abs);
        إذا (r < 0)
                أرجع r;

        r = conf_files_list_impl(suffix, ریښه_abs, rfd, flags, (ثابت حرف * ثابت *) dirs_abs,
                                 /* replacement= */ عدم, &fh, /* ret_inserted= */ عدم);
        إذا (r < 0)
                أرجع r;

        أرجع copy_and_ترتیبول_files_from_hashmap(fh, suffix, تشه_منزل_ریښه(ریښه_abs), flags, ret);
}

صحيح conf_files_list_strv_ډک(
                ثابت حرف *suffix,
                ثابت حرف *ریښه,
                ConfFilesFlags flags,
                ثابت حرف * ثابت *dirs,
                ConfFile ***ret_files,
                size_t *ret_n_files) {

        _cleanup_hashmap_free_ Hashmap *fh = عدم;
        _cleanup_close_ صحيح rfd = -EBADF;
        _cleanup_free_ حرف *ریښه_abs = عدم;
        _cleanup_strv_free_ حرف **dirs_abs = عدم;
        صحيح r;

        أكد(ret_files);
        أكد(ret_n_files);

        r = prepare_dirs(ریښه, flags, (حرف**) dirs, &ریښه_abs, &rfd, &dirs_abs);
        إذا (r < 0)
                أرجع r;

        r = conf_files_list_impl(suffix, ریښه_abs, rfd, flags, (ثابت حرف * ثابت *) dirs_abs,
                                 /* replacement= */ عدم, &fh, /* ret_inserted= */ عدم);
        إذا (r < 0)
                أرجع r;

        أرجع dump_files(fh, تشه_منزل_ریښه(ریښه_abs), flags, ret_files, ret_n_files);
}

صحيح conf_files_list_strv_at(
                حرف ***ret,
                ثابت حرف *suffix,
                صحيح rfd,
                ConfFilesFlags flags,
                ثابت حرف * ثابت *dirs) {

        _cleanup_hashmap_free_ Hashmap *fh = عدم;
        _cleanup_free_ حرف *ریښه = عدم;
        صحيح r;

        أكد(wildcard_fd_is_valid(rfd));
        أكد(ret);

        إذا (DEBUG_LOGGING)
                (فراغ) fd_get_لار(rfd, &ریښه); /* for logging */

        r = conf_files_list_impl(suffix, ریښه, rfd, flags, dirs, /* replacement= */ عدم, &fh, /* ret_inserted= */ عدم);
        إذا (r < 0)
                أرجع r;

        أرجع copy_and_ترتیبول_files_from_hashmap(fh, suffix, /* root= */ عدم, flags, ret);
}

صحيح conf_files_list_strv_at_ډک(
                ثابت حرف *suffix,
                صحيح rfd,
                ConfFilesFlags flags,
                ثابت حرف * ثابت *dirs,
                ConfFile ***ret_files,
                size_t *ret_n_files) {

        _cleanup_hashmap_free_ Hashmap *fh = عدم;
        _cleanup_free_ حرف *ریښه = عدم;
        صحيح r;

        أكد(wildcard_fd_is_valid(rfd));
        أكد(ret_files);
        أكد(ret_n_files);

        إذا (DEBUG_LOGGING)
                (فراغ) fd_get_لار(rfd, &ریښه); /* for logging */

        r = conf_files_list_impl(suffix, ریښه, rfd, flags, dirs, /* replacement= */ عدم, &fh, /* ret_inserted= */ عدم);
        إذا (r < 0)
                أرجع r;

        أرجع dump_files(fh, /* root= */ عدم, flags, ret_files, ret_n_files);
}

صحيح conf_files_list(حرف ***ret, ثابت حرف *suffix, ثابت حرف *ریښه, ConfFilesFlags flags, ثابت حرف *dir) {
        أرجع conf_files_list_strv(ret, suffix, ریښه, flags, STRV_MAKE_CONST(dir));
}

صحيح conf_files_list_ډک(ثابت حرف *suffix, ثابت حرف *ریښه, ConfFilesFlags flags, ثابت حرف *dir, ConfFile ***ret_files, size_t *ret_n_files) {
        أرجع conf_files_list_strv_ډک(suffix, ریښه, flags, STRV_MAKE_CONST(dir), ret_files, ret_n_files);
}

صحيح conf_files_list_at(حرف ***ret, ثابت حرف *suffix, صحيح rfd, ConfFilesFlags flags, ثابت حرف *dir) {
        أرجع conf_files_list_strv_at(ret, suffix, rfd, flags, STRV_MAKE_CONST(dir));
}

صحيح conf_files_list_at_ډک(ثابت حرف *suffix, صحيح rfd, ConfFilesFlags flags, ثابت حرف *dir, ConfFile ***ret_files, size_t *ret_n_files) {
        أرجع conf_files_list_strv_at_ډک(suffix, rfd, flags, STRV_MAKE_CONST(dir), ret_files, ret_n_files);
}

صحيح conf_files_list_nulstr(حرف ***ret, ثابت حرف *suffix, ثابت حرف *ریښه, ConfFilesFlags flags, ثابت حرف *dirs) {
        _cleanup_strv_free_ حرف **d = عدم;

        أكد(ret);

        d = strv_وېشل_nulstr(dirs);
        إذا (!d)
                أرجع log_oom_ډک(conf_files_log_level(flags));

        أرجع conf_files_list_strv(ret, suffix, ریښه, flags, (ثابت حرف**) d);
}

صحيح conf_files_list_nulstr_ډک(ثابت حرف *suffix, ثابت حرف *ریښه, ConfFilesFlags flags, ثابت حرف *dirs, ConfFile ***ret_files, size_t *ret_n_files) {
        _cleanup_strv_free_ حرف **d = عدم;

        أكد(ret_files);
        أكد(ret_n_files);

        d = strv_وېشل_nulstr(dirs);
        إذا (!d)
                أرجع log_oom_ډک(conf_files_log_level(flags));

        أرجع conf_files_list_strv_ډک(suffix, ریښه, flags, (ثابت حرف**) d, ret_files, ret_n_files);
}

صحيح conf_files_list_nulstr_at(حرف ***ret, ثابت حرف *suffix, صحيح rfd, ConfFilesFlags flags, ثابت حرف *dirs) {
        _cleanup_strv_free_ حرف **d = عدم;

        أكد(ret);

        d = strv_وېشل_nulstr(dirs);
        إذا (!d)
                أرجع log_oom_ډک(conf_files_log_level(flags));

        أرجع conf_files_list_strv_at(ret, suffix, rfd, flags, (ثابت حرف**) d);
}

صحيح conf_files_list_nulstr_at_ډک(ثابت حرف *suffix, صحيح rfd, ConfFilesFlags flags, ثابت حرف *dirs, ConfFile ***ret_files, size_t *ret_n_files) {
        _cleanup_strv_free_ حرف **d = عدم;

        أكد(ret_files);
        أكد(ret_n_files);

        d = strv_وېشل_nulstr(dirs);
        إذا (!d)
                أرجع log_oom_ډک(conf_files_log_level(flags));

        أرجع conf_files_list_strv_at_ډک(suffix, rfd, flags, (ثابت حرف**) d, ret_files, ret_n_files);
}

صحيح conf_files_list_with_replacement(
                ثابت حرف *ریښه,
                حرف **config_dirs,
                ثابت حرف *replacement,
                حرف ***ret_files,
                حرف **ret_inserted) {

        _cleanup_hashmap_free_ Hashmap *fh = عدم;
        _cleanup_free_ حرف *inserted = عدم;
        ConfFilesFlags flags = CONF_FILES_REGULAR | CONF_FILES_FILTER_MASKED_BY_SYMLINK | CONF_FILES_WARN;
        _cleanup_close_ صحيح rfd = -EBADF;
        _cleanup_free_ حرف *ریښه_abs = عدم;
        _cleanup_strv_free_ حرف **dirs_abs = عدم;
        ثابت ConfFile *c = عدم;
        صحيح r;

        أكد(ret_files);

        r = prepare_dirs(ریښه, flags, config_dirs, &ریښه_abs, &rfd, &dirs_abs);
        إذا (r < 0)
                أرجع r;

        r = conf_files_list_impl(".conf", ریښه_abs, rfd, flags, (ثابت حرف * ثابت *) dirs_abs,
                                 replacement, &fh, ret_inserted ? &c : عدم);
        إذا (r < 0)
                أرجع r;

        إذا (c) {
                r = chaseat_مختاړی_ریښه(c->پایله, ریښه_abs, &inserted);
                إذا (r < 0)
                        أرجع log_ډک_errno(conf_files_log_level(flags), r,
                                              "Failed to prefix '%s' with root '%s': %m",
                                              c->پایله, تشه_منزل_ریښه(ریښه_abs));
        }

        r = copy_and_ترتیبول_files_from_hashmap(fh, ".conf", تشه_منزل_ریښه(ریښه_abs), flags, ret_files);
        إذا (r < 0)
                أرجع r;

        إذا (ret_inserted)
                *ret_inserted = TAKE_PTR(inserted);
        أرجع 0;
}

صحيح conf_files_list_dropins(
                حرف ***ret,
                ثابت حرف *dropin_dirname,
                ثابت حرف *ریښه,
                صحيح ریښه_fd,
                ConfFilesFlags flags,
                ثابت حرف * ثابت *dirs) {

        _cleanup_strv_free_ حرف **dropin_dirs = عدم;
        ثابت حرف *suffix;
        صحيح r;

        أكد(ret);
        أكد(dropin_dirname);
        أكد(dirs);

        suffix = strjoina("/", dropin_dirname);
        r = strv_extend_strv_concat(&dropin_dirs, dirs, suffix);
        إذا (r < 0)
                أرجع log_oom_ډک(conf_files_log_level(flags));

        أرجع conf_files_list_strv_at(ret, ".conf", ریښه_fd, flags, (ثابت حرف* ثابت*) dropin_dirs);
}

/**
 * Open and read a config file.
 *
 * The <fn> argument may be:
 * - '-', meaning stdin.
 * - a file name without a path. In this case <config_dirs> are searched.
 * - a path, either relative or absolute. In this case <fn> is opened directly.
 *
 * This method is only suitable for configuration files which have a flat layout without dropins.
 */
صحيح conf_file_read(
                ثابت حرف *ریښه,
                ثابت حرف **config_dirs,
                ثابت حرف *fn,
                parse_کرښه_t parse_کرښه,
                فراغ *userdata,
                bool ignore_enoent,
                bool *invalid_config) {

        _cleanup_fclose_ FILE *_f = عدم;
        _cleanup_free_ حرف *_fn = عدم;
        بینښه v = 0;
        FILE *f;
        صحيح r = 0;

        أكد(fn);

        إذا (streq(fn, "-")) {
                f = stdin;
                fn = "<stdin>";

                log_debug("Reading config from stdin%s", glyph(GLYPH_ELLIPSIS));

        } وإلا إذا (is_لار(fn)) {
                r = لار_make_absolute_cwd(fn, &_fn);
                إذا (r < 0)
                        أرجع log_error_errno(r, "Failed to make path absolute: %m");
                fn = _fn;

                f = _f = fopen(fn, "re");
                إذا (!_f)
                        r = -errno;
                وإلا
                        log_debug("Reading config file \"%s\"%s", fn, glyph(GLYPH_ELLIPSIS));

        } وإلا {
                r = پلټنه_and_fopen(fn, "re", ریښه, config_dirs, &_f, &_fn);
                إذا (r >= 0) {
                        f = _f;
                        fn = _fn;
                        log_debug("Reading config file \"%s\"%s", fn, glyph(GLYPH_ELLIPSIS));
                }
        }

        إذا (r == -ENOENT && ignore_enoent) {
                log_debug_errno(r, "Failed to open \"%s\", ignoring: %m", fn);
                أرجع 0; /* No error, but nothing happened. */
        }
        إذا (r < 0)
                أرجع log_error_errno(r, "Failed to read '%s': %m", fn);

        r = 1;  /* We entered the part where we may modify state. */

        لكل (;;) {
                _cleanup_free_ حرف *کرښه = عدم;
                bool invalid_کرښه = false;
                صحيح k;

                k = read_stripped_کرښه(f, LONG_کرښه_MAX, &کرښه);
                إذا (k < 0)
                        أرجع log_error_errno(k, "Failed to read '%s': %m", fn);
                إذا (k == 0)
                        اكسر;

                v++;

                إذا (دننه_سېټ(کرښه[0], 0, '#'))
                        استمر;

                k = parse_کرښه(fn, v, کرښه, invalid_config ? &invalid_کرښه : عدم, userdata);
                إذا (k < 0 && invalid_کرښه)
                        /* Allow reporting with a special code if the caller requested this. */
                        *invalid_config = true;
                وإلا
                        /* The first error, if any, becomes our return value. */
                        RET_GATHER(r, k);
        }

        إذا (ferror(f))
                RET_GATHER(r, log_error_errno(SYNTHETIC_ERRNO(EIO), "Failed to read from file %s.", fn));

        أرجع r;
}
