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
        حرر(c->result);
        حرر(c->original_path);
        حرر(c->resolved_path);
        safe_close(c->fd);

        أرجع mfree(c);
}

DEFINE_POINTER_ARRAY_FREE_FUNC(ConfFile*, conf_file_free);

ساكن صحيح conf_files_log_level(ConfFilesFlags flags) {
        أرجع FLAGS_SET(flags, CONF_FILES_WARN) ? LOG_WARNING : LOG_DEBUG;
}

ساكن صحيح prepare_dirs(
                ثابت حرف *جذر,
                ConfFilesFlags flags,
                حرف * ثابت *dirs,
                حرف **ret_جذر,
                صحيح *ret_rfd,
                حرف ***ret_dirs) {

        _cleanup_free_ حرف *جذر_abs = عدم;
        _cleanup_strv_free_ حرف **dirs_abs = عدم;
        صحيح r;

        أكد(ret_جذر);
        أكد(ret_rfd);
        أكد(ret_dirs || strv_isempty(dirs));

        صحيح log_level = conf_files_log_level(flags);

        r = empty_or_جذر_harder_to_null(&جذر);
        إذا (r < 0)
                أرجع log_full_errno(log_level, r, "Failed to determine if '%s' points to the root directory: %m", strempty(جذر));

        إذا (ret_dirs) {
                dirs_abs = strv_copy(dirs);
                إذا (!dirs_abs)
                        أرجع log_oom_full(log_level);
        }

        _cleanup_close_ صحيح rfd = XAT_FDROOT;
        إذا (جذر) {
                /* When a non-trivial root is specified, we will prefix the result later. Hence, it is not
                 * necessary to modify each config directories here. but needs to normalize the root directory. */
                r = path_make_absolute_cwd(جذر, &جذر_abs);
                إذا (r < 0)
                        أرجع log_full_errno(log_level, r, "Failed to make '%s' absolute: %m", جذر);

                path_simplify(جذر_abs);

                rfd = open(جذر, O_CLOEXEC|O_DIRECTORY|O_PATH);
                إذا (rfd < 0)
                        أرجع log_full_errno(log_level, errno, "Failed to open '%s': %m", جذر_abs);

        } وإلا إذا (ret_dirs) {
                /* When an empty root or "/" is specified, we will open "/" below, hence we need to make
                 * each config directory absolute if relative. */
                r = path_strv_make_absolute_cwd(dirs_abs);
                إذا (r < 0)
                        أرجع log_full_errno(log_level, r, "Failed to make directories absolute: %m");
        }

        *ret_جذر = TAKE_PTR(جذر_abs);
        *ret_rfd = TAKE_FD(rfd);
        إذا (ret_dirs)
                *ret_dirs = TAKE_PTR(dirs_abs);
        أرجع 0;
}

ساكن صحيح conf_file_prefix_جذر(ConfFile *c, ثابت حرف *جذر, ConfFilesFlags flags) {
        حرف *p;
        صحيح r;

        أكد(c);

        إذا (FLAGS_SET(flags, CONF_FILES_DONT_PREFIX_جذر))
                أرجع 0;

        صحيح log_level = conf_files_log_level(flags);

        r = chaseat_prefix_جذر(c->result, جذر, &p);
        إذا (r < 0)
                أرجع log_full_errno(log_level, r, "Failed to prefix '%s' with root '%s': %m", c->result, جذر);
        free_and_replace(c->result, p);

        r = chaseat_prefix_جذر(c->resolved_path, جذر, &p);
        إذا (r < 0)
                أرجع log_full_errno(log_level, r, "Failed to prefix '%s' with root '%s': %m", c->resolved_path, جذر);
        free_and_replace(c->resolved_path, p);

        /* Do not use chaseat_prefix_root(), as it is for the result of chaseat(), but the path is not chased. */
        p = path_join(empty_to_جذر(جذر), skip_leading_slash(c->original_path));
        إذا (!p)
                أرجع log_oom_full(log_level);

        path_simplify(p);
        أرجع free_and_replace(c->original_path, p);
}

ساكن bool conf_files_need_stat(ConfFilesFlags flags) {
        أرجع (flags & (CONF_FILES_FILTER_MASKED | CONF_FILES_REGULAR | CONF_FILES_DIRECTORY | CONF_FILES_EXECUTABLE)) != 0;
}

ساكن ChaseFlags conf_files_chase_flags(ConfFilesFlags flags) {
        ChaseFlags chase_flags = 0;

        إذا (!conf_files_need_stat(flags) || FLAGS_SET(flags, CONF_FILES_FILTER_MASKED_BY_SYMLINK))
                /* Even if no verification is requested, let's unconditionally call chaseat(),
                 * to drop unsafe symlinks. */
                chase_flags |= CHASE_NONEXISTENT;

        أرجع chase_flags;
}

ساكن صحيح conf_file_chase_and_verify(
                ثابت حرف *جذر,          /* for logging, can be NULL */
                صحيح rfd,
                ثابت حرف *original_path, /* for logging */
                ثابت حرف *path,
                ثابت حرف *name,
                Set **masked,              /* optional */
                ConfFilesFlags flags,
                حرف **ret_path,
                صحيح *ret_fd,
                بنية stat *ret_stat) {

        _cleanup_free_ حرف *resolved_path = عدم;
        _cleanup_close_ صحيح fd = -EBADF;
        بنية stat st = {};
        صحيح r;

        أكد(wildcard_fd_is_valid(rfd));
        أكد(original_path);
        أكد(path);
        أكد(name);

        صحيح log_level = conf_files_log_level(flags);

        جذر = empty_to_جذر(جذر);

        r = chaseat(rfd, rfd, path, conf_files_chase_flags(flags), &resolved_path, &fd);
        إذا (r < 0)
                أرجع log_full_errno(log_level, r, "Failed to chase '%s%s': %m",
                                      جذر, skip_leading_slash(original_path));
        إذا (r == 0) {
                إذا (FLAGS_SET(flags, CONF_FILES_FILTER_MASKED_BY_SYMLINK)) {
                        /* If the path points to /dev/null in a image or so, then the device node may not exist. */
                        إذا (path_equal(skip_leading_slash(resolved_path), "dev/null")) {
                                إذا (masked) {
                                        /* Mark this one as masked */
                                        r = set_put_strdup(masked, name);
                                        إذا (r < 0)
                                                أرجع log_oom_full(log_level);
                                }

                                أرجع log_debug_errno(SYNTHETIC_ERRNO(ERFKILL),
                                                       "File '%s%s' is a mask (symlink to /dev/null).",
                                                       جذر, skip_leading_slash(original_path));
                        }
                }

                إذا (conf_files_need_stat(flags))
                        /* If we need to have stat, skip the entry. */
                        أرجع log_full_errno(log_level, SYNTHETIC_ERRNO(ENOENT), "Failed to chase '%s%s': %m",
                                              جذر, skip_leading_slash(original_path));
        }

        /* Even if we do not need stat, let's take stat now. The caller may use the info later. */
        إذا (fd >= 0 && fstat(fd, &st) < 0)
                أرجع log_full_errno(log_level, errno, "Failed to stat '%s%s': %m",
                                      جذر, skip_leading_slash(original_path));

        /* Is this a masking entry? */
        إذا (FLAGS_SET(flags, CONF_FILES_FILTER_MASKED_BY_SYMLINK) && stat_may_be_dev_null(&st)) {
                إذا (masked) {
                        /* Mark this one as masked */
                        r = set_put_strdup(masked, name);
                        إذا (r < 0)
                                أرجع log_oom_full(log_level);
                }

                أرجع log_debug_errno(SYNTHETIC_ERRNO(ERFKILL),
                                       "File '%s%s' is a mask (symlink to /dev/null).",
                                       جذر, skip_leading_slash(original_path));
        }

        إذا (FLAGS_SET(flags, CONF_FILES_FILTER_MASKED_BY_EMPTY) && stat_is_empty(&st)) {
                إذا (masked) {
                        /* Mark this one as masked */
                        r = set_put_strdup(masked, name);
                        إذا (r < 0)
                                أرجع log_oom_full(log_level);
                }

                أرجع log_debug_errno(SYNTHETIC_ERRNO(ERFKILL),
                                       "File '%s%s' is a mask (an empty file).",
                                       جذر, skip_leading_slash(original_path));
        }

        إذا (FLAGS_SET(flags, CONF_FILES_REGULAR|CONF_FILES_DIRECTORY)) {
                إذا (!S_ISREG(st.st_mode) && !S_ISDIR(st.st_mode))
                        أرجع log_debug_errno(SYNTHETIC_ERRNO(EBADFD),
                                               "File '%s%s' is neither a regular file or directory.",
                                               جذر, skip_leading_slash(original_path));
        } وإلا {
                /* Is this node a regular file? */
                إذا (FLAGS_SET(flags, CONF_FILES_REGULAR)) {
                        r = stat_verify_regular(&st);
                        إذا (r < 0)
                                أرجع log_debug_errno(r, "File '%s%s' is not a regular file: %m",
                                                       جذر, skip_leading_slash(original_path));
                }

                /* Is this node a directory? */
                إذا (FLAGS_SET(flags, CONF_FILES_DIRECTORY)) {
                        r = stat_verify_directory(&st);
                        إذا (r < 0)
                                أرجع log_debug_errno(r, "File '%s%s' is not a directory: %m",
                                                       جذر, skip_leading_slash(original_path));
                }
        }

        /* Does this node have the executable bit set?
         * As requested: check if the file is marked executable. Note that we don't check access(X_OK) here,
         * as we care about whether the file is marked executable at all, and not whether it is executable
         * for us, because if so, such errors are stuff we should log about. */
        إذا (FLAGS_SET(flags, CONF_FILES_EXECUTABLE) && (st.st_mode & 0111) == 0)
                أرجع log_debug_errno(SYNTHETIC_ERRNO(ENOEXEC),
                                       "File '%s%s' is not marked executable.",
                                       جذر, skip_leading_slash(original_path));

        إذا (ret_path)
                *ret_path = TAKE_PTR(resolved_path);
        إذا (ret_fd)
                *ret_fd = TAKE_FD(fd);
        إذا (ret_stat)
                *ret_stat = st;

        أرجع 0;
}

صحيح conf_file_new_at(
                ثابت حرف *path,
                ثابت حرف *جذر,
                صحيح rfd,
                ConfFilesFlags flags,
                ConfFile **ret) {
        صحيح r;

        أكد(path);
        أكد(wildcard_fd_is_valid(rfd));
        أكد(ret);

        صحيح log_level = conf_files_log_level(flags);

        _cleanup_free_ حرف *جذر_2 = عدم;
        إذا (DEBUG_LOGGING && !جذر) {
                (فراغ) fd_get_path(rfd, &جذر_2);
                جذر = جذر_2;
        }

        _cleanup_(conf_file_freep) ConfFile *c = new(ConfFile, 1);
        إذا (!c)
                أرجع log_oom_full(log_level);

        *c = (ConfFile) {
                .original_path = strdup(path),
                .fd = -EBADF,
        };

        إذا (!c->original_path)
                أرجع log_oom_full(log_level);

        r = path_extract_filename(path, &c->filename);
        إذا (r < 0)
                أرجع log_full_errno(log_level, r, "Failed to extract filename from '%s': %m", path);

        _cleanup_free_ حرف *dirpath = عدم, *resolved_dirpath = عدم;
        r = path_extract_directory(path, &dirpath);
        إذا (r < 0 && r != -EDESTADDRREQ)
                أرجع log_full_errno(log_level, r, "Failed to extract directory from '%s': %m", path);
        إذا (r >= 0) {
                r = chaseat(rfd, rfd, dirpath,
                            CHASE_MUST_BE_DIRECTORY | conf_files_chase_flags(flags),
                            &resolved_dirpath, /* ret_fd= */ عدم);
                إذا (r < 0)
                        أرجع log_full_errno(log_level, r, "Failed to chase '%s%s': %m", empty_to_جذر(جذر), skip_leading_slash(dirpath));
        }

        c->result = path_join(resolved_dirpath, c->filename);
        إذا (!c->result)
                أرجع log_oom_full(log_level);

        r = conf_file_chase_and_verify(
                        جذر,
                        rfd,
                        c->original_path,
                        c->result,
                        c->filename,
                        /* masked= */ عدم,
                        flags,
                        &c->resolved_path,
                        &c->fd,
                        &c->st);
        إذا (r < 0)
                أرجع r;

        *ret = TAKE_PTR(c);
        أرجع 0;
}

صحيح conf_file_new(ثابت حرف *path, ثابت حرف *جذر, ConfFilesFlags flags, ConfFile **ret) {
        صحيح r;

        أكد(path);
        أكد(ret);

        _cleanup_free_ حرف *جذر_abs = عدم;
        _cleanup_close_ صحيح rfd = -EBADF;
        r = prepare_dirs(جذر, flags, /* dirs= */ عدم, &جذر_abs, &rfd, /* ret_dirs= */ عدم);
        إذا (r < 0)
                أرجع r;

        _cleanup_free_ حرف *path_abs = عدم;
        إذا (!جذر_abs) {
                r = path_make_absolute_cwd(path, &path_abs);
                إذا (r < 0)
                        أرجع log_full_errno(conf_files_log_level(flags), r, "Failed to make '%s' absolute: %m", path);

                path = path_abs;
        }

        _cleanup_(conf_file_freep) ConfFile *c = عدم;
        r = conf_file_new_at(path, جذر_abs, rfd, flags, &c);
        إذا (r < 0)
                أرجع r;

        r = conf_file_prefix_جذر(c, جذر_abs, flags);
        إذا (r < 0)
                أرجع r;

        *ret = TAKE_PTR(c);
        أرجع 0;
}

DEFINE_PRIVATE_HASH_OPS_WITH_قيمة_DESTRUCTOR(
                conf_file_hash_ops,
                حرف, string_hash_func, string_compare_func,
                ConfFile, conf_file_free);

ساكن صحيح files_add(
                DIR *dir,
                ثابت حرف *original_dirpath,
                ثابت حرف *resolved_dirpath,
                ثابت حرف *جذر, /* for logging, can be NULL */
                صحيح rfd,
                Hashmap **files,
                Set **masked,
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

        جذر = empty_to_جذر(جذر);

        FOREACH_DIRENT(de, dir, أرجع log_full_errno(log_level, errno, "Failed to read directory '%s%s': %m",
                                                      جذر, skip_leading_slash(original_dirpath))) {

                _cleanup_free_ حرف *original_path = path_join(original_dirpath, de->d_name);
                إذا (!original_path)
                        أرجع log_oom_full(log_level);

                /* Does this match the suffix? */
                إذا (suffix && !endswith(de->d_name, suffix)) {
                        log_debug("Skipping file '%s%s', suffix is not '%s'.", جذر, skip_leading_slash(original_path), suffix);
                        استمر;
                }

                /* Has this file already been found in an earlier directory? */
                إذا (hashmap_contains(*files, de->d_name)) {
                        log_debug("Skipping overridden file '%s%s'.", جذر, skip_leading_slash(original_path));
                        استمر;
                }

                /* Has this been masked in an earlier directory? */
                إذا ((flags & CONF_FILES_FILTER_MASKED) != 0 && set_contains(*masked, de->d_name)) {
                        log_debug("File '%s%s' is masked by previous entry.", جذر, skip_leading_slash(original_path));
                        استمر;
                }

                _cleanup_free_ حرف *p = path_join(resolved_dirpath, de->d_name);
                إذا (!p)
                        أرجع log_oom_full(log_level);

                _cleanup_free_ حرف *resolved_path = عدم;
                _cleanup_close_ صحيح fd = -EBADF;
                بنية stat st;
                r = conf_file_chase_and_verify(
                                جذر,
                                rfd,
                                original_path,
                                p,
                                de->d_name,
                                masked,
                                flags,
                                &resolved_path,
                                &fd,
                                &st);
                إذا (r == -ENOMEM)
                        أرجع r;
                إذا (r < 0)
                        استمر;

                _cleanup_(conf_file_freep) ConfFile *c = new(ConfFile, 1);
                إذا (!c)
                        أرجع log_oom_full(log_level);

                *c = (ConfFile) {
                        .filename = strdup(de->d_name),
                        .result = TAKE_PTR(p),
                        .original_path = TAKE_PTR(original_path),
                        .resolved_path = TAKE_PTR(resolved_path),
                        .fd = TAKE_FD(fd),
                        .st = st,
                };

                إذا (!c->filename)
                        أرجع log_oom_full(log_level);

                r = hashmap_ensure_put(files, &conf_file_hash_ops, c->filename, c);
                إذا (r < 0) {
                        أكد(r == -ENOMEM);
                        أرجع log_oom_full(log_level);
                }
                أكد(r > 0);

                TAKE_PTR(c);
        }

        أرجع 0;
}

ساكن صحيح dump_files(Hashmap *fh, ثابت حرف *جذر, ConfFilesFlags flags, ConfFile ***ret_files, size_t *ret_n_files) {
        ConfFile **files = عدم;
        size_t n_files = 0;
        صحيح r;

        CLEANUP_ARRAY(files, n_files, conf_file_free_array);

        أكد(ret_files);
        أكد(ret_n_files);

        /* The entries in the array given by hashmap_dump_sorted() are still owned by the hashmap. */
        r = hashmap_dump_sorted(fh, (فراغ***) &files, &n_files);
        إذا (r < 0)
                أرجع log_oom_full(conf_files_log_level(flags));

        /* Hence, we need to remove them from the hashmap. */
        FOREACH_ARRAY(i, files, n_files)
                assert_se(hashmap_remove(fh, (*i)->filename) == *i);

        إذا (جذر)
                FOREACH_ARRAY(i, files, n_files) {
                        r = conf_file_prefix_جذر(*i, جذر, flags);
                        إذا (r < 0)
                                أرجع r;
                }

        *ret_files = TAKE_PTR(files);
        *ret_n_files = n_files;
        أرجع 0;
}

ساكن صحيح copy_and_sort_files_from_hashmap(
                Hashmap *fh,
                ثابت حرف *suffix,
                ثابت حرف *جذر,
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
                أرجع log_oom_full(log_level);

        FOREACH_ARRAY(i, files, n_files) {
                ConfFile *c = *i;
                ثابت حرف *add = عدم;

                إذا (FLAGS_SET(flags, CONF_FILES_BASENAME))
                        add = c->filename;
                وإلا إذا (جذر && !FLAGS_SET(flags, CONF_FILES_DONT_PREFIX_جذر)) {
                        _cleanup_free_ حرف *p = عدم;

                        r = chaseat_prefix_جذر(c->result, جذر, &p);
                        إذا (r < 0)
                                أرجع log_full_errno(log_level, r, "Failed to prefix '%s' with root '%s': %m", c->result, جذر);

                        إذا (FLAGS_SET(flags, CONF_FILES_TRUNCATE_SUFFIX) && suffix) {
                                حرف *e = endswith(p, suffix);
                                إذا (!e)
                                        استمر;

                                *e = 0;
                        }

                        إذا (strv_consume_with_size(&results, &n_results, TAKE_PTR(p)) < 0)
                                أرجع log_oom_full(log_level);

                        استمر;
                } وإلا
                        add = c->result;

                إذا (FLAGS_SET(flags, CONF_FILES_TRUNCATE_SUFFIX)) {
                        ثابت حرف *e = endswith(add, suffix);
                        إذا (!e)
                                استمر;

                        _cleanup_free_ حرف *n = strndup(add, e - add);
                        إذا (!n)
                                أرجع log_oom_full(log_level);

                        r = strv_consume_with_size(&results, &n_results, TAKE_PTR(n));
                } وإلا
                        r = strv_extend_with_size(&results, &n_results, add);
                إذا (r < 0)
                        أرجع log_oom_full(log_level);
        }

        *ret = TAKE_PTR(results);
        أرجع 0;
}

ساكن صحيح أدرج_replacement(Hashmap **fh, ConfFile *replacement, ConfFilesFlags flags, ثابت ConfFile **ret) {
        _cleanup_(conf_file_freep) ConfFile *c = ASSERT_PTR(replacement);
        صحيح r;

        أكد(fh);
        أكد(ret);

        /* This consumes the input ConfFile. */

        ConfFile *existing = hashmap_get(*fh, c->filename);
        إذا (existing) {
                log_debug("An entry with higher priority '%s' -> '%s' already exists, ignoring the replacement: %s",
                          existing->filename, existing->result, c->original_path);
                *ret = عدم;
                أرجع 0;
        }

        r = hashmap_ensure_put(fh, &conf_file_hash_ops, c->filename, c);
        إذا (r < 0) {
                أكد(r == -ENOMEM);
                أرجع log_oom_full(conf_files_log_level(flags));
        }
        أكد(r > 0);

        log_debug("Inserted replacement: '%s' -> '%s'", c->filename, c->result);

        *ret = TAKE_PTR(c);
        أرجع 0;
}

ساكن صحيح conf_files_list_impl(
                ثابت حرف *suffix,
                ثابت حرف *جذر, /* for logging, can be NULL */
                صحيح rfd,
                ConfFilesFlags flags,
                ثابت حرف * ثابت *dirs,
                ثابت حرف *replacement,
                Hashmap **ret,
                ثابت ConfFile **ret_inserted) {

        _cleanup_hashmap_free_ Hashmap *fh = عدم;
        _cleanup_set_free_ Set *masked = عدم;
        _cleanup_(conf_file_freep) ConfFile *c = عدم;
        ثابت ConfFile *inserted = عدم;
        صحيح r;

        أكد(wildcard_fd_is_valid(rfd));
        أكد(ret);

        جذر = empty_to_جذر(جذر);

        إذا (replacement) {
                r = conf_file_new_at(replacement, جذر, rfd, flags & CONF_FILES_WARN, &c);
                إذا (r < 0)
                        أرجع r;
        }

        STRV_FOREACH(p, dirs) {
                _cleanup_closedir_ DIR *dir = عدم;
                _cleanup_free_ حرف *path = عدم;

                r = chase_and_opendirat(rfd, rfd, *p, 0, &path, &dir);
                إذا (r < 0) {
                        إذا (r != -ENOENT)
                                log_full_errno(conf_files_log_level(flags), r,
                                               "Failed to chase and open directory '%s%s', ignoring: %m",
                                               جذر, skip_leading_slash(*p));
                        استمر;
                }

                إذا (c && streq_ptr(path_startswith(c->result, path), c->filename)) {
                        r = أدرج_replacement(&fh, TAKE_PTR(c), flags, &inserted);
                        إذا (r < 0)
                                أرجع r;
                }

                r = files_add(dir, *p, path, جذر, rfd, &fh, &masked, suffix, flags);
                إذا (r == -ENOMEM)
                        أرجع r;
        }

        إذا (c) {
                r = أدرج_replacement(&fh, TAKE_PTR(c), flags, &inserted);
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
                ثابت حرف *جذر,
                ConfFilesFlags flags,
                ثابت حرف * ثابت *dirs) {

        _cleanup_hashmap_free_ Hashmap *fh = عدم;
        _cleanup_close_ صحيح rfd = -EBADF;
        _cleanup_free_ حرف *جذر_abs = عدم;
        _cleanup_strv_free_ حرف **dirs_abs = عدم;
        صحيح r;

        أكد(ret);

        r = prepare_dirs(جذر, flags, (حرف**) dirs, &جذر_abs, &rfd, &dirs_abs);
        إذا (r < 0)
                أرجع r;

        r = conf_files_list_impl(suffix, جذر_abs, rfd, flags, (ثابت حرف * ثابت *) dirs_abs,
                                 /* replacement= */ عدم, &fh, /* ret_inserted= */ عدم);
        إذا (r < 0)
                أرجع r;

        أرجع copy_and_sort_files_from_hashmap(fh, suffix, empty_to_جذر(جذر_abs), flags, ret);
}

صحيح conf_files_list_strv_full(
                ثابت حرف *suffix,
                ثابت حرف *جذر,
                ConfFilesFlags flags,
                ثابت حرف * ثابت *dirs,
                ConfFile ***ret_files,
                size_t *ret_n_files) {

        _cleanup_hashmap_free_ Hashmap *fh = عدم;
        _cleanup_close_ صحيح rfd = -EBADF;
        _cleanup_free_ حرف *جذر_abs = عدم;
        _cleanup_strv_free_ حرف **dirs_abs = عدم;
        صحيح r;

        أكد(ret_files);
        أكد(ret_n_files);

        r = prepare_dirs(جذر, flags, (حرف**) dirs, &جذر_abs, &rfd, &dirs_abs);
        إذا (r < 0)
                أرجع r;

        r = conf_files_list_impl(suffix, جذر_abs, rfd, flags, (ثابت حرف * ثابت *) dirs_abs,
                                 /* replacement= */ عدم, &fh, /* ret_inserted= */ عدم);
        إذا (r < 0)
                أرجع r;

        أرجع dump_files(fh, empty_to_جذر(جذر_abs), flags, ret_files, ret_n_files);
}

صحيح conf_files_list_strv_at(
                حرف ***ret,
                ثابت حرف *suffix,
                صحيح rfd,
                ConfFilesFlags flags,
                ثابت حرف * ثابت *dirs) {

        _cleanup_hashmap_free_ Hashmap *fh = عدم;
        _cleanup_free_ حرف *جذر = عدم;
        صحيح r;

        أكد(wildcard_fd_is_valid(rfd));
        أكد(ret);

        إذا (DEBUG_LOGGING)
                (فراغ) fd_get_path(rfd, &جذر); /* for logging */

        r = conf_files_list_impl(suffix, جذر, rfd, flags, dirs, /* replacement= */ عدم, &fh, /* ret_inserted= */ عدم);
        إذا (r < 0)
                أرجع r;

        أرجع copy_and_sort_files_from_hashmap(fh, suffix, /* root= */ عدم, flags, ret);
}

صحيح conf_files_list_strv_at_full(
                ثابت حرف *suffix,
                صحيح rfd,
                ConfFilesFlags flags,
                ثابت حرف * ثابت *dirs,
                ConfFile ***ret_files,
                size_t *ret_n_files) {

        _cleanup_hashmap_free_ Hashmap *fh = عدم;
        _cleanup_free_ حرف *جذر = عدم;
        صحيح r;

        أكد(wildcard_fd_is_valid(rfd));
        أكد(ret_files);
        أكد(ret_n_files);

        إذا (DEBUG_LOGGING)
                (فراغ) fd_get_path(rfd, &جذر); /* for logging */

        r = conf_files_list_impl(suffix, جذر, rfd, flags, dirs, /* replacement= */ عدم, &fh, /* ret_inserted= */ عدم);
        إذا (r < 0)
                أرجع r;

        أرجع dump_files(fh, /* root= */ عدم, flags, ret_files, ret_n_files);
}

صحيح conf_files_list(حرف ***ret, ثابت حرف *suffix, ثابت حرف *جذر, ConfFilesFlags flags, ثابت حرف *dir) {
        أرجع conf_files_list_strv(ret, suffix, جذر, flags, STRV_MAKE_CONST(dir));
}

صحيح conf_files_list_full(ثابت حرف *suffix, ثابت حرف *جذر, ConfFilesFlags flags, ثابت حرف *dir, ConfFile ***ret_files, size_t *ret_n_files) {
        أرجع conf_files_list_strv_full(suffix, جذر, flags, STRV_MAKE_CONST(dir), ret_files, ret_n_files);
}

صحيح conf_files_list_at(حرف ***ret, ثابت حرف *suffix, صحيح rfd, ConfFilesFlags flags, ثابت حرف *dir) {
        أرجع conf_files_list_strv_at(ret, suffix, rfd, flags, STRV_MAKE_CONST(dir));
}

صحيح conf_files_list_at_full(ثابت حرف *suffix, صحيح rfd, ConfFilesFlags flags, ثابت حرف *dir, ConfFile ***ret_files, size_t *ret_n_files) {
        أرجع conf_files_list_strv_at_full(suffix, rfd, flags, STRV_MAKE_CONST(dir), ret_files, ret_n_files);
}

صحيح conf_files_list_nulstr(حرف ***ret, ثابت حرف *suffix, ثابت حرف *جذر, ConfFilesFlags flags, ثابت حرف *dirs) {
        _cleanup_strv_free_ حرف **d = عدم;

        أكد(ret);

        d = strv_split_nulstr(dirs);
        إذا (!d)
                أرجع log_oom_full(conf_files_log_level(flags));

        أرجع conf_files_list_strv(ret, suffix, جذر, flags, (ثابت حرف**) d);
}

صحيح conf_files_list_nulstr_full(ثابت حرف *suffix, ثابت حرف *جذر, ConfFilesFlags flags, ثابت حرف *dirs, ConfFile ***ret_files, size_t *ret_n_files) {
        _cleanup_strv_free_ حرف **d = عدم;

        أكد(ret_files);
        أكد(ret_n_files);

        d = strv_split_nulstr(dirs);
        إذا (!d)
                أرجع log_oom_full(conf_files_log_level(flags));

        أرجع conf_files_list_strv_full(suffix, جذر, flags, (ثابت حرف**) d, ret_files, ret_n_files);
}

صحيح conf_files_list_nulstr_at(حرف ***ret, ثابت حرف *suffix, صحيح rfd, ConfFilesFlags flags, ثابت حرف *dirs) {
        _cleanup_strv_free_ حرف **d = عدم;

        أكد(ret);

        d = strv_split_nulstr(dirs);
        إذا (!d)
                أرجع log_oom_full(conf_files_log_level(flags));

        أرجع conf_files_list_strv_at(ret, suffix, rfd, flags, (ثابت حرف**) d);
}

صحيح conf_files_list_nulstr_at_full(ثابت حرف *suffix, صحيح rfd, ConfFilesFlags flags, ثابت حرف *dirs, ConfFile ***ret_files, size_t *ret_n_files) {
        _cleanup_strv_free_ حرف **d = عدم;

        أكد(ret_files);
        أكد(ret_n_files);

        d = strv_split_nulstr(dirs);
        إذا (!d)
                أرجع log_oom_full(conf_files_log_level(flags));

        أرجع conf_files_list_strv_at_full(suffix, rfd, flags, (ثابت حرف**) d, ret_files, ret_n_files);
}

صحيح conf_files_list_with_replacement(
                ثابت حرف *جذر,
                حرف **config_dirs,
                ثابت حرف *replacement,
                حرف ***ret_files,
                حرف **ret_inserted) {

        _cleanup_hashmap_free_ Hashmap *fh = عدم;
        _cleanup_free_ حرف *inserted = عدم;
        ConfFilesFlags flags = CONF_FILES_REGULAR | CONF_FILES_FILTER_MASKED_BY_SYMLINK | CONF_FILES_WARN;
        _cleanup_close_ صحيح rfd = -EBADF;
        _cleanup_free_ حرف *جذر_abs = عدم;
        _cleanup_strv_free_ حرف **dirs_abs = عدم;
        ثابت ConfFile *c = عدم;
        صحيح r;

        أكد(ret_files);

        r = prepare_dirs(جذر, flags, config_dirs, &جذر_abs, &rfd, &dirs_abs);
        إذا (r < 0)
                أرجع r;

        r = conf_files_list_impl(".conf", جذر_abs, rfd, flags, (ثابت حرف * ثابت *) dirs_abs,
                                 replacement, &fh, ret_inserted ? &c : عدم);
        إذا (r < 0)
                أرجع r;

        إذا (c) {
                r = chaseat_prefix_جذر(c->result, جذر_abs, &inserted);
                إذا (r < 0)
                        أرجع log_full_errno(conf_files_log_level(flags), r,
                                              "Failed to prefix '%s' with root '%s': %m",
                                              c->result, empty_to_جذر(جذر_abs));
        }

        r = copy_and_sort_files_from_hashmap(fh, ".conf", empty_to_جذر(جذر_abs), flags, ret_files);
        إذا (r < 0)
                أرجع r;

        إذا (ret_inserted)
                *ret_inserted = TAKE_PTR(inserted);
        أرجع 0;
}

صحيح conf_files_list_dropins(
                حرف ***ret,
                ثابت حرف *dropin_dirname,
                ثابت حرف *جذر,
                صحيح جذر_fd,
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
                أرجع log_oom_full(conf_files_log_level(flags));

        أرجع conf_files_list_strv_at(ret, ".conf", جذر_fd, flags, (ثابت حرف* ثابت*) dropin_dirs);
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
                ثابت حرف *جذر,
                ثابت حرف **config_dirs,
                ثابت حرف *fn,
                parse_سطر_t parse_سطر,
                فراغ *userdata,
                bool ignore_enoent,
                bool *invalid_config) {

        _cleanup_fclose_ FILE *_f = عدم;
        _cleanup_free_ حرف *_fn = عدم;
        unsigned v = 0;
        FILE *f;
        صحيح r = 0;

        أكد(fn);

        إذا (streq(fn, "-")) {
                f = stdin;
                fn = "<stdin>";

                log_debug("Reading config from stdin%s", glyph(GLYPH_ELLIPSIS));

        } وإلا إذا (is_path(fn)) {
                r = path_make_absolute_cwd(fn, &_fn);
                إذا (r < 0)
                        أرجع log_error_errno(r, "Failed to make path absolute: %m");
                fn = _fn;

                f = _f = fopen(fn, "re");
                إذا (!_f)
                        r = -errno;
                وإلا
                        log_debug("Reading config file \"%s\"%s", fn, glyph(GLYPH_ELLIPSIS));

        } وإلا {
                r = بحث_and_fopen(fn, "re", جذر, config_dirs, &_f, &_fn);
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
                _cleanup_free_ حرف *سطر = عدم;
                bool invalid_سطر = false;
                صحيح k;

                k = read_stripped_سطر(f, LONG_سطر_MAX, &سطر);
                إذا (k < 0)
                        أرجع log_error_errno(k, "Failed to read '%s': %m", fn);
                إذا (k == 0)
                        اكسر;

                v++;

                إذا (IN_SET(سطر[0], 0, '#'))
                        استمر;

                k = parse_سطر(fn, v, سطر, invalid_config ? &invalid_سطر : عدم, userdata);
                إذا (k < 0 && invalid_سطر)
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
