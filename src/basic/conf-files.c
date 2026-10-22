/* SPDX-License-Identifier: LGPL-2.1-or-later */

#شامل <stdio.h>
#شامل <stdlib.h>

#شامل "alloc-util.h"
#شامل "chase.h"
#شامل "conf-files.h"
#شامل "dirent-util.h"
#شامل "errno-util.h"
#شامل "fd-util.h"
#شامل "fileio.h"
#شامل "glyph-util.h"
#شامل "hashmap.h"
#شامل "log.h"
#شامل "nulstr-util.h"
#شامل "path-util.h"
#شامل "set.h"
#شامل "stat-util.h"
#شامل "string-util.h"
#شامل "strv.h"

ConfFile* conf_file_free(ConfFile *c) {
        اگر (!c)
                بازگشت پوچ;

        آزاد(c->filename);
        آزاد(c->result);
        آزاد(c->original_path);
        آزاد(c->resolved_path);
        safe_close(c->fd);

        بازگشت mfree(c);
}

DEFINE_POINTER_ARRAY_FREE_FUNC(ConfFile*, conf_file_free);

ایستا صحیح conf_files_log_level(ConfFilesFlags flags) {
        بازگشت FLAGS_SET(flags, CONF_FILES_WARN) ? LOG_WARNING : LOG_DEBUG;
}

ایستا صحیح prepare_dirs(
                ثابت نویسه *ریشه,
                ConfFilesFlags flags,
                نویسه * ثابت *dirs,
                نویسه **ret_ریشه,
                صحیح *ret_rfd,
                نویسه ***ret_dirs) {

        _cleanup_free_ نویسه *ریشه_abs = پوچ;
        _cleanup_strv_free_ نویسه **dirs_abs = پوچ;
        صحیح r;

        أكد(ret_ریشه);
        أكد(ret_rfd);
        أكد(ret_dirs || strv_isempty(dirs));

        صحیح log_level = conf_files_log_level(flags);

        r = empty_or_ریشه_harder_to_null(&ریشه);
        اگر (r < 0)
                بازگشت log_full_errno(log_level, r, "Failed to determine if '%s' points to the root directory: %m", strempty(ریشه));

        اگر (ret_dirs) {
                dirs_abs = strv_copy(dirs);
                اگر (!dirs_abs)
                        بازگشت log_oom_full(log_level);
        }

        _cleanup_close_ صحیح rfd = XAT_FDROOT;
        اگر (ریشه) {
                /* When a non-trivial root is specified, we will prefix the result later. Hence, it is not
                 * necessary to modify each config directories here. but needs to normalize the root directory. */
                r = path_make_absolute_cwd(ریشه, &ریشه_abs);
                اگر (r < 0)
                        بازگشت log_full_errno(log_level, r, "Failed to make '%s' absolute: %m", ریشه);

                path_simplify(ریشه_abs);

                rfd = open(ریشه, O_CLOEXEC|O_DIRECTORY|O_PATH);
                اگر (rfd < 0)
                        بازگشت log_full_errno(log_level, errno, "Failed to open '%s': %m", ریشه_abs);

        } وگرنه اگر (ret_dirs) {
                /* When an empty root or "/" is specified, we will open "/" below, hence we need to make
                 * each config directory absolute if relative. */
                r = path_strv_make_absolute_cwd(dirs_abs);
                اگر (r < 0)
                        بازگشت log_full_errno(log_level, r, "Failed to make directories absolute: %m");
        }

        *ret_ریشه = TAKE_PTR(ریشه_abs);
        *ret_rfd = TAKE_FD(rfd);
        اگر (ret_dirs)
                *ret_dirs = TAKE_PTR(dirs_abs);
        بازگشت 0;
}

ایستا صحیح conf_file_prefix_ریشه(ConfFile *c, ثابت نویسه *ریشه, ConfFilesFlags flags) {
        نویسه *p;
        صحیح r;

        أكد(c);

        اگر (FLAGS_SET(flags, CONF_FILES_DONT_PREFIX_ریشه))
                بازگشت 0;

        صحیح log_level = conf_files_log_level(flags);

        r = chaseat_prefix_ریشه(c->result, ریشه, &p);
        اگر (r < 0)
                بازگشت log_full_errno(log_level, r, "Failed to prefix '%s' with root '%s': %m", c->result, ریشه);
        free_and_replace(c->result, p);

        r = chaseat_prefix_ریشه(c->resolved_path, ریشه, &p);
        اگر (r < 0)
                بازگشت log_full_errno(log_level, r, "Failed to prefix '%s' with root '%s': %m", c->resolved_path, ریشه);
        free_and_replace(c->resolved_path, p);

        /* Do not use chaseat_prefix_root(), as it is for the result of chaseat(), but the path is not chased. */
        p = path_join(empty_to_ریشه(ریشه), skip_leading_slash(c->original_path));
        اگر (!p)
                بازگشت log_oom_full(log_level);

        path_simplify(p);
        بازگشت free_and_replace(c->original_path, p);
}

ایستا bool conf_files_need_stat(ConfFilesFlags flags) {
        بازگشت (flags & (CONF_FILES_FILTER_MASKED | CONF_FILES_REGULAR | CONF_FILES_DIRECTORY | CONF_FILES_EXECUTABLE)) != 0;
}

ایستا ChaseFlags conf_files_chase_flags(ConfFilesFlags flags) {
        ChaseFlags chase_flags = 0;

        اگر (!conf_files_need_stat(flags) || FLAGS_SET(flags, CONF_FILES_FILTER_MASKED_BY_SYMLINK))
                /* Even if no verification is requested, let's unconditionally call chaseat(),
                 * to drop unsafe symlinks. */
                chase_flags |= CHASE_NONEXISTENT;

        بازگشت chase_flags;
}

ایستا صحیح conf_file_chase_and_verify(
                ثابت نویسه *ریشه,          /* for logging, can be NULL */
                صحیح rfd,
                ثابت نویسه *original_path, /* for logging */
                ثابت نویسه *path,
                ثابت نویسه *name,
                Set **masked,              /* optional */
                ConfFilesFlags flags,
                نویسه **ret_path,
                صحیح *ret_fd,
                ساختار stat *ret_stat) {

        _cleanup_free_ نویسه *resolved_path = پوچ;
        _cleanup_close_ صحیح fd = -EBADF;
        ساختار stat st = {};
        صحیح r;

        أكد(wildcard_fd_is_valid(rfd));
        أكد(original_path);
        أكد(path);
        أكد(name);

        صحیح log_level = conf_files_log_level(flags);

        ریشه = empty_to_ریشه(ریشه);

        r = chaseat(rfd, rfd, path, conf_files_chase_flags(flags), &resolved_path, &fd);
        اگر (r < 0)
                بازگشت log_full_errno(log_level, r, "Failed to chase '%s%s': %m",
                                      ریشه, skip_leading_slash(original_path));
        اگر (r == 0) {
                اگر (FLAGS_SET(flags, CONF_FILES_FILTER_MASKED_BY_SYMLINK)) {
                        /* If the path points to /dev/null in a image or so, then the device node may not exist. */
                        اگر (path_equal(skip_leading_slash(resolved_path), "dev/null")) {
                                اگر (masked) {
                                        /* Mark this one as masked */
                                        r = set_put_strdup(masked, name);
                                        اگر (r < 0)
                                                بازگشت log_oom_full(log_level);
                                }

                                بازگشت log_debug_errno(SYNTHETIC_ERRNO(ERFKILL),
                                                       "File '%s%s' is a mask (symlink to /dev/null).",
                                                       ریشه, skip_leading_slash(original_path));
                        }
                }

                اگر (conf_files_need_stat(flags))
                        /* If we need to have stat, skip the entry. */
                        بازگشت log_full_errno(log_level, SYNTHETIC_ERRNO(ENOENT), "Failed to chase '%s%s': %m",
                                              ریشه, skip_leading_slash(original_path));
        }

        /* Even if we do not need stat, let's take stat now. The caller may use the info later. */
        اگر (fd >= 0 && fstat(fd, &st) < 0)
                بازگشت log_full_errno(log_level, errno, "Failed to stat '%s%s': %m",
                                      ریشه, skip_leading_slash(original_path));

        /* Is this a masking entry? */
        اگر (FLAGS_SET(flags, CONF_FILES_FILTER_MASKED_BY_SYMLINK) && stat_may_be_dev_null(&st)) {
                اگر (masked) {
                        /* Mark this one as masked */
                        r = set_put_strdup(masked, name);
                        اگر (r < 0)
                                بازگشت log_oom_full(log_level);
                }

                بازگشت log_debug_errno(SYNTHETIC_ERRNO(ERFKILL),
                                       "File '%s%s' is a mask (symlink to /dev/null).",
                                       ریشه, skip_leading_slash(original_path));
        }

        اگر (FLAGS_SET(flags, CONF_FILES_FILTER_MASKED_BY_EMPTY) && stat_is_empty(&st)) {
                اگر (masked) {
                        /* Mark this one as masked */
                        r = set_put_strdup(masked, name);
                        اگر (r < 0)
                                بازگشت log_oom_full(log_level);
                }

                بازگشت log_debug_errno(SYNTHETIC_ERRNO(ERFKILL),
                                       "File '%s%s' is a mask (an empty file).",
                                       ریشه, skip_leading_slash(original_path));
        }

        اگر (FLAGS_SET(flags, CONF_FILES_REGULAR|CONF_FILES_DIRECTORY)) {
                اگر (!S_ISREG(st.st_mode) && !S_ISDIR(st.st_mode))
                        بازگشت log_debug_errno(SYNTHETIC_ERRNO(EBADFD),
                                               "File '%s%s' is neither a regular file or directory.",
                                               ریشه, skip_leading_slash(original_path));
        } وگرنه {
                /* Is this node a regular file? */
                اگر (FLAGS_SET(flags, CONF_FILES_REGULAR)) {
                        r = stat_verify_regular(&st);
                        اگر (r < 0)
                                بازگشت log_debug_errno(r, "File '%s%s' is not a regular file: %m",
                                                       ریشه, skip_leading_slash(original_path));
                }

                /* Is this node a directory? */
                اگر (FLAGS_SET(flags, CONF_FILES_DIRECTORY)) {
                        r = stat_verify_directory(&st);
                        اگر (r < 0)
                                بازگشت log_debug_errno(r, "File '%s%s' is not a directory: %m",
                                                       ریشه, skip_leading_slash(original_path));
                }
        }

        /* Does this node have the executable bit set?
         * As requested: check if the file is marked executable. Note that we don't check access(X_OK) here,
         * as we care about whether the file is marked executable at all, and not whether it is executable
         * for us, because if so, such errors are stuff we should log about. */
        اگر (FLAGS_SET(flags, CONF_FILES_EXECUTABLE) && (st.st_mode & 0111) == 0)
                بازگشت log_debug_errno(SYNTHETIC_ERRNO(ENOEXEC),
                                       "File '%s%s' is not marked executable.",
                                       ریشه, skip_leading_slash(original_path));

        اگر (ret_path)
                *ret_path = TAKE_PTR(resolved_path);
        اگر (ret_fd)
                *ret_fd = TAKE_FD(fd);
        اگر (ret_stat)
                *ret_stat = st;

        بازگشت 0;
}

صحیح conf_file_new_at(
                ثابت نویسه *path,
                ثابت نویسه *ریشه,
                صحیح rfd,
                ConfFilesFlags flags,
                ConfFile **ret) {
        صحیح r;

        أكد(path);
        أكد(wildcard_fd_is_valid(rfd));
        أكد(ret);

        صحیح log_level = conf_files_log_level(flags);

        _cleanup_free_ نویسه *ریشه_2 = پوچ;
        اگر (DEBUG_LOGGING && !ریشه) {
                (تهی) fd_get_path(rfd, &ریشه_2);
                ریشه = ریشه_2;
        }

        _cleanup_(conf_file_freep) ConfFile *c = new(ConfFile, 1);
        اگر (!c)
                بازگشت log_oom_full(log_level);

        *c = (ConfFile) {
                .original_path = strdup(path),
                .fd = -EBADF,
        };

        اگر (!c->original_path)
                بازگشت log_oom_full(log_level);

        r = path_extract_filename(path, &c->filename);
        اگر (r < 0)
                بازگشت log_full_errno(log_level, r, "Failed to extract filename from '%s': %m", path);

        _cleanup_free_ نویسه *dirpath = پوچ, *resolved_dirpath = پوچ;
        r = path_extract_directory(path, &dirpath);
        اگر (r < 0 && r != -EDESTADDRREQ)
                بازگشت log_full_errno(log_level, r, "Failed to extract directory from '%s': %m", path);
        اگر (r >= 0) {
                r = chaseat(rfd, rfd, dirpath,
                            CHASE_MUST_BE_DIRECTORY | conf_files_chase_flags(flags),
                            &resolved_dirpath, /* ret_fd= */ پوچ);
                اگر (r < 0)
                        بازگشت log_full_errno(log_level, r, "Failed to chase '%s%s': %m", empty_to_ریشه(ریشه), skip_leading_slash(dirpath));
        }

        c->result = path_join(resolved_dirpath, c->filename);
        اگر (!c->result)
                بازگشت log_oom_full(log_level);

        r = conf_file_chase_and_verify(
                        ریشه,
                        rfd,
                        c->original_path,
                        c->result,
                        c->filename,
                        /* masked= */ پوچ,
                        flags,
                        &c->resolved_path,
                        &c->fd,
                        &c->st);
        اگر (r < 0)
                بازگشت r;

        *ret = TAKE_PTR(c);
        بازگشت 0;
}

صحیح conf_file_new(ثابت نویسه *path, ثابت نویسه *ریشه, ConfFilesFlags flags, ConfFile **ret) {
        صحیح r;

        أكد(path);
        أكد(ret);

        _cleanup_free_ نویسه *ریشه_abs = پوچ;
        _cleanup_close_ صحیح rfd = -EBADF;
        r = prepare_dirs(ریشه, flags, /* dirs= */ پوچ, &ریشه_abs, &rfd, /* ret_dirs= */ پوچ);
        اگر (r < 0)
                بازگشت r;

        _cleanup_free_ نویسه *path_abs = پوچ;
        اگر (!ریشه_abs) {
                r = path_make_absolute_cwd(path, &path_abs);
                اگر (r < 0)
                        بازگشت log_full_errno(conf_files_log_level(flags), r, "Failed to make '%s' absolute: %m", path);

                path = path_abs;
        }

        _cleanup_(conf_file_freep) ConfFile *c = پوچ;
        r = conf_file_new_at(path, ریشه_abs, rfd, flags, &c);
        اگر (r < 0)
                بازگشت r;

        r = conf_file_prefix_ریشه(c, ریشه_abs, flags);
        اگر (r < 0)
                بازگشت r;

        *ret = TAKE_PTR(c);
        بازگشت 0;
}

DEFINE_PRIVATE_HASH_OPS_WITH_مقدار_DESTRUCTOR(
                conf_file_hash_ops,
                نویسه, string_hash_func, string_compare_func,
                ConfFile, conf_file_free);

ایستا صحیح files_add(
                DIR *dir,
                ثابت نویسه *original_dirpath,
                ثابت نویسه *resolved_dirpath,
                ثابت نویسه *ریشه, /* for logging, can be NULL */
                صحیح rfd,
                Hashmap **files,
                Set **masked,
                ثابت نویسه *suffix,
                ConfFilesFlags flags) {

        صحیح r;

        أكد(dir);
        أكد(original_dirpath);
        أكد(resolved_dirpath);
        أكد(wildcard_fd_is_valid(rfd));
        أكد(files);
        أكد(masked);

        صحیح log_level = conf_files_log_level(flags);

        ریشه = empty_to_ریشه(ریشه);

        FOREACH_DIRENT(de, dir, بازگشت log_full_errno(log_level, errno, "Failed to read directory '%s%s': %m",
                                                      ریشه, skip_leading_slash(original_dirpath))) {

                _cleanup_free_ نویسه *original_path = path_join(original_dirpath, de->d_name);
                اگر (!original_path)
                        بازگشت log_oom_full(log_level);

                /* Does this match the suffix? */
                اگر (suffix && !endswith(de->d_name, suffix)) {
                        log_debug("Skipping file '%s%s', suffix is not '%s'.", ریشه, skip_leading_slash(original_path), suffix);
                        ادامه;
                }

                /* Has this file already been found in an earlier directory? */
                اگر (hashmap_contains(*files, de->d_name)) {
                        log_debug("Skipping overridden file '%s%s'.", ریشه, skip_leading_slash(original_path));
                        ادامه;
                }

                /* Has this been masked in an earlier directory? */
                اگر ((flags & CONF_FILES_FILTER_MASKED) != 0 && set_contains(*masked, de->d_name)) {
                        log_debug("File '%s%s' is masked by previous entry.", ریشه, skip_leading_slash(original_path));
                        ادامه;
                }

                _cleanup_free_ نویسه *p = path_join(resolved_dirpath, de->d_name);
                اگر (!p)
                        بازگشت log_oom_full(log_level);

                _cleanup_free_ نویسه *resolved_path = پوچ;
                _cleanup_close_ صحیح fd = -EBADF;
                ساختار stat st;
                r = conf_file_chase_and_verify(
                                ریشه,
                                rfd,
                                original_path,
                                p,
                                de->d_name,
                                masked,
                                flags,
                                &resolved_path,
                                &fd,
                                &st);
                اگر (r == -ENOMEM)
                        بازگشت r;
                اگر (r < 0)
                        ادامه;

                _cleanup_(conf_file_freep) ConfFile *c = new(ConfFile, 1);
                اگر (!c)
                        بازگشت log_oom_full(log_level);

                *c = (ConfFile) {
                        .filename = strdup(de->d_name),
                        .result = TAKE_PTR(p),
                        .original_path = TAKE_PTR(original_path),
                        .resolved_path = TAKE_PTR(resolved_path),
                        .fd = TAKE_FD(fd),
                        .st = st,
                };

                اگر (!c->filename)
                        بازگشت log_oom_full(log_level);

                r = hashmap_ensure_put(files, &conf_file_hash_ops, c->filename, c);
                اگر (r < 0) {
                        أكد(r == -ENOMEM);
                        بازگشت log_oom_full(log_level);
                }
                أكد(r > 0);

                TAKE_PTR(c);
        }

        بازگشت 0;
}

ایستا صحیح dump_files(Hashmap *fh, ثابت نویسه *ریشه, ConfFilesFlags flags, ConfFile ***ret_files, size_t *ret_n_files) {
        ConfFile **files = پوچ;
        size_t n_files = 0;
        صحیح r;

        CLEANUP_ARRAY(files, n_files, conf_file_free_array);

        أكد(ret_files);
        أكد(ret_n_files);

        /* The entries in the array given by hashmap_dump_sorted() are still owned by the hashmap. */
        r = hashmap_dump_sorted(fh, (تهی***) &files, &n_files);
        اگر (r < 0)
                بازگشت log_oom_full(conf_files_log_level(flags));

        /* Hence, we need to remove them from the hashmap. */
        FOREACH_ARRAY(i, files, n_files)
                assert_se(hashmap_remove(fh, (*i)->filename) == *i);

        اگر (ریشه)
                FOREACH_ARRAY(i, files, n_files) {
                        r = conf_file_prefix_ریشه(*i, ریشه, flags);
                        اگر (r < 0)
                                بازگشت r;
                }

        *ret_files = TAKE_PTR(files);
        *ret_n_files = n_files;
        بازگشت 0;
}

ایستا صحیح copy_and_sort_files_from_hashmap(
                Hashmap *fh,
                ثابت نویسه *suffix,
                ثابت نویسه *ریشه,
                ConfFilesFlags flags,
                نویسه ***ret) {

        _cleanup_strv_free_ نویسه **results = پوچ;
        _cleanup_free_ ConfFile **files = پوچ;
        size_t n_files = 0, n_results = 0;
        صحیح r;

        أكد(ret);

        صحیح log_level = conf_files_log_level(flags);

        /* The entries in the array given by hashmap_dump_sorted() are still owned by the hashmap.
         * Hence, do not use conf_file_free_array() for 'entries' */
        r = hashmap_dump_sorted(fh, (تهی***) &files, &n_files);
        اگر (r < 0)
                بازگشت log_oom_full(log_level);

        FOREACH_ARRAY(i, files, n_files) {
                ConfFile *c = *i;
                ثابت نویسه *add = پوچ;

                اگر (FLAGS_SET(flags, CONF_FILES_BASENAME))
                        add = c->filename;
                وگرنه اگر (ریشه && !FLAGS_SET(flags, CONF_FILES_DONT_PREFIX_ریشه)) {
                        _cleanup_free_ نویسه *p = پوچ;

                        r = chaseat_prefix_ریشه(c->result, ریشه, &p);
                        اگر (r < 0)
                                بازگشت log_full_errno(log_level, r, "Failed to prefix '%s' with root '%s': %m", c->result, ریشه);

                        اگر (FLAGS_SET(flags, CONF_FILES_TRUNCATE_SUFFIX) && suffix) {
                                نویسه *e = endswith(p, suffix);
                                اگر (!e)
                                        ادامه;

                                *e = 0;
                        }

                        اگر (strv_consume_with_size(&results, &n_results, TAKE_PTR(p)) < 0)
                                بازگشت log_oom_full(log_level);

                        ادامه;
                } وگرنه
                        add = c->result;

                اگر (FLAGS_SET(flags, CONF_FILES_TRUNCATE_SUFFIX)) {
                        ثابت نویسه *e = endswith(add, suffix);
                        اگر (!e)
                                ادامه;

                        _cleanup_free_ نویسه *n = strndup(add, e - add);
                        اگر (!n)
                                بازگشت log_oom_full(log_level);

                        r = strv_consume_with_size(&results, &n_results, TAKE_PTR(n));
                } وگرنه
                        r = strv_extend_with_size(&results, &n_results, add);
                اگر (r < 0)
                        بازگشت log_oom_full(log_level);
        }

        *ret = TAKE_PTR(results);
        بازگشت 0;
}

ایستا صحیح درج_replacement(Hashmap **fh, ConfFile *replacement, ConfFilesFlags flags, ثابت ConfFile **ret) {
        _cleanup_(conf_file_freep) ConfFile *c = ASSERT_PTR(replacement);
        صحیح r;

        أكد(fh);
        أكد(ret);

        /* This consumes the input ConfFile. */

        ConfFile *existing = hashmap_get(*fh, c->filename);
        اگر (existing) {
                log_debug("An entry with higher priority '%s' -> '%s' already exists, ignoring the replacement: %s",
                          existing->filename, existing->result, c->original_path);
                *ret = پوچ;
                بازگشت 0;
        }

        r = hashmap_ensure_put(fh, &conf_file_hash_ops, c->filename, c);
        اگر (r < 0) {
                أكد(r == -ENOMEM);
                بازگشت log_oom_full(conf_files_log_level(flags));
        }
        أكد(r > 0);

        log_debug("Inserted replacement: '%s' -> '%s'", c->filename, c->result);

        *ret = TAKE_PTR(c);
        بازگشت 0;
}

ایستا صحیح conf_files_list_impl(
                ثابت نویسه *suffix,
                ثابت نویسه *ریشه, /* for logging, can be NULL */
                صحیح rfd,
                ConfFilesFlags flags,
                ثابت نویسه * ثابت *dirs,
                ثابت نویسه *replacement,
                Hashmap **ret,
                ثابت ConfFile **ret_inserted) {

        _cleanup_hashmap_free_ Hashmap *fh = پوچ;
        _cleanup_set_free_ Set *masked = پوچ;
        _cleanup_(conf_file_freep) ConfFile *c = پوچ;
        ثابت ConfFile *inserted = پوچ;
        صحیح r;

        أكد(wildcard_fd_is_valid(rfd));
        أكد(ret);

        ریشه = empty_to_ریشه(ریشه);

        اگر (replacement) {
                r = conf_file_new_at(replacement, ریشه, rfd, flags & CONF_FILES_WARN, &c);
                اگر (r < 0)
                        بازگشت r;
        }

        STRV_FOREACH(p, dirs) {
                _cleanup_closedir_ DIR *dir = پوچ;
                _cleanup_free_ نویسه *path = پوچ;

                r = chase_and_opendirat(rfd, rfd, *p, 0, &path, &dir);
                اگر (r < 0) {
                        اگر (r != -ENOENT)
                                log_full_errno(conf_files_log_level(flags), r,
                                               "Failed to chase and open directory '%s%s', ignoring: %m",
                                               ریشه, skip_leading_slash(*p));
                        ادامه;
                }

                اگر (c && streq_ptr(path_startswith(c->result, path), c->filename)) {
                        r = درج_replacement(&fh, TAKE_PTR(c), flags, &inserted);
                        اگر (r < 0)
                                بازگشت r;
                }

                r = files_add(dir, *p, path, ریشه, rfd, &fh, &masked, suffix, flags);
                اگر (r == -ENOMEM)
                        بازگشت r;
        }

        اگر (c) {
                r = درج_replacement(&fh, TAKE_PTR(c), flags, &inserted);
                اگر (r < 0)
                        بازگشت r;
        }

        *ret = TAKE_PTR(fh);
        اگر (ret_inserted)
                *ret_inserted = inserted;
        بازگشت 0;
}

صحیح conf_files_list_strv(
                نویسه ***ret,
                ثابت نویسه *suffix,
                ثابت نویسه *ریشه,
                ConfFilesFlags flags,
                ثابت نویسه * ثابت *dirs) {

        _cleanup_hashmap_free_ Hashmap *fh = پوچ;
        _cleanup_close_ صحیح rfd = -EBADF;
        _cleanup_free_ نویسه *ریشه_abs = پوچ;
        _cleanup_strv_free_ نویسه **dirs_abs = پوچ;
        صحیح r;

        أكد(ret);

        r = prepare_dirs(ریشه, flags, (نویسه**) dirs, &ریشه_abs, &rfd, &dirs_abs);
        اگر (r < 0)
                بازگشت r;

        r = conf_files_list_impl(suffix, ریشه_abs, rfd, flags, (ثابت نویسه * ثابت *) dirs_abs,
                                 /* replacement= */ پوچ, &fh, /* ret_inserted= */ پوچ);
        اگر (r < 0)
                بازگشت r;

        بازگشت copy_and_sort_files_from_hashmap(fh, suffix, empty_to_ریشه(ریشه_abs), flags, ret);
}

صحیح conf_files_list_strv_full(
                ثابت نویسه *suffix,
                ثابت نویسه *ریشه,
                ConfFilesFlags flags,
                ثابت نویسه * ثابت *dirs,
                ConfFile ***ret_files,
                size_t *ret_n_files) {

        _cleanup_hashmap_free_ Hashmap *fh = پوچ;
        _cleanup_close_ صحیح rfd = -EBADF;
        _cleanup_free_ نویسه *ریشه_abs = پوچ;
        _cleanup_strv_free_ نویسه **dirs_abs = پوچ;
        صحیح r;

        أكد(ret_files);
        أكد(ret_n_files);

        r = prepare_dirs(ریشه, flags, (نویسه**) dirs, &ریشه_abs, &rfd, &dirs_abs);
        اگر (r < 0)
                بازگشت r;

        r = conf_files_list_impl(suffix, ریشه_abs, rfd, flags, (ثابت نویسه * ثابت *) dirs_abs,
                                 /* replacement= */ پوچ, &fh, /* ret_inserted= */ پوچ);
        اگر (r < 0)
                بازگشت r;

        بازگشت dump_files(fh, empty_to_ریشه(ریشه_abs), flags, ret_files, ret_n_files);
}

صحیح conf_files_list_strv_at(
                نویسه ***ret,
                ثابت نویسه *suffix,
                صحیح rfd,
                ConfFilesFlags flags,
                ثابت نویسه * ثابت *dirs) {

        _cleanup_hashmap_free_ Hashmap *fh = پوچ;
        _cleanup_free_ نویسه *ریشه = پوچ;
        صحیح r;

        أكد(wildcard_fd_is_valid(rfd));
        أكد(ret);

        اگر (DEBUG_LOGGING)
                (تهی) fd_get_path(rfd, &ریشه); /* for logging */

        r = conf_files_list_impl(suffix, ریشه, rfd, flags, dirs, /* replacement= */ پوچ, &fh, /* ret_inserted= */ پوچ);
        اگر (r < 0)
                بازگشت r;

        بازگشت copy_and_sort_files_from_hashmap(fh, suffix, /* root= */ پوچ, flags, ret);
}

صحیح conf_files_list_strv_at_full(
                ثابت نویسه *suffix,
                صحیح rfd,
                ConfFilesFlags flags,
                ثابت نویسه * ثابت *dirs,
                ConfFile ***ret_files,
                size_t *ret_n_files) {

        _cleanup_hashmap_free_ Hashmap *fh = پوچ;
        _cleanup_free_ نویسه *ریشه = پوچ;
        صحیح r;

        أكد(wildcard_fd_is_valid(rfd));
        أكد(ret_files);
        أكد(ret_n_files);

        اگر (DEBUG_LOGGING)
                (تهی) fd_get_path(rfd, &ریشه); /* for logging */

        r = conf_files_list_impl(suffix, ریشه, rfd, flags, dirs, /* replacement= */ پوچ, &fh, /* ret_inserted= */ پوچ);
        اگر (r < 0)
                بازگشت r;

        بازگشت dump_files(fh, /* root= */ پوچ, flags, ret_files, ret_n_files);
}

صحیح conf_files_list(نویسه ***ret, ثابت نویسه *suffix, ثابت نویسه *ریشه, ConfFilesFlags flags, ثابت نویسه *dir) {
        بازگشت conf_files_list_strv(ret, suffix, ریشه, flags, STRV_MAKE_CONST(dir));
}

صحیح conf_files_list_full(ثابت نویسه *suffix, ثابت نویسه *ریشه, ConfFilesFlags flags, ثابت نویسه *dir, ConfFile ***ret_files, size_t *ret_n_files) {
        بازگشت conf_files_list_strv_full(suffix, ریشه, flags, STRV_MAKE_CONST(dir), ret_files, ret_n_files);
}

صحیح conf_files_list_at(نویسه ***ret, ثابت نویسه *suffix, صحیح rfd, ConfFilesFlags flags, ثابت نویسه *dir) {
        بازگشت conf_files_list_strv_at(ret, suffix, rfd, flags, STRV_MAKE_CONST(dir));
}

صحیح conf_files_list_at_full(ثابت نویسه *suffix, صحیح rfd, ConfFilesFlags flags, ثابت نویسه *dir, ConfFile ***ret_files, size_t *ret_n_files) {
        بازگشت conf_files_list_strv_at_full(suffix, rfd, flags, STRV_MAKE_CONST(dir), ret_files, ret_n_files);
}

صحیح conf_files_list_nulstr(نویسه ***ret, ثابت نویسه *suffix, ثابت نویسه *ریشه, ConfFilesFlags flags, ثابت نویسه *dirs) {
        _cleanup_strv_free_ نویسه **d = پوچ;

        أكد(ret);

        d = strv_split_nulstr(dirs);
        اگر (!d)
                بازگشت log_oom_full(conf_files_log_level(flags));

        بازگشت conf_files_list_strv(ret, suffix, ریشه, flags, (ثابت نویسه**) d);
}

صحیح conf_files_list_nulstr_full(ثابت نویسه *suffix, ثابت نویسه *ریشه, ConfFilesFlags flags, ثابت نویسه *dirs, ConfFile ***ret_files, size_t *ret_n_files) {
        _cleanup_strv_free_ نویسه **d = پوچ;

        أكد(ret_files);
        أكد(ret_n_files);

        d = strv_split_nulstr(dirs);
        اگر (!d)
                بازگشت log_oom_full(conf_files_log_level(flags));

        بازگشت conf_files_list_strv_full(suffix, ریشه, flags, (ثابت نویسه**) d, ret_files, ret_n_files);
}

صحیح conf_files_list_nulstr_at(نویسه ***ret, ثابت نویسه *suffix, صحیح rfd, ConfFilesFlags flags, ثابت نویسه *dirs) {
        _cleanup_strv_free_ نویسه **d = پوچ;

        أكد(ret);

        d = strv_split_nulstr(dirs);
        اگر (!d)
                بازگشت log_oom_full(conf_files_log_level(flags));

        بازگشت conf_files_list_strv_at(ret, suffix, rfd, flags, (ثابت نویسه**) d);
}

صحیح conf_files_list_nulstr_at_full(ثابت نویسه *suffix, صحیح rfd, ConfFilesFlags flags, ثابت نویسه *dirs, ConfFile ***ret_files, size_t *ret_n_files) {
        _cleanup_strv_free_ نویسه **d = پوچ;

        أكد(ret_files);
        أكد(ret_n_files);

        d = strv_split_nulstr(dirs);
        اگر (!d)
                بازگشت log_oom_full(conf_files_log_level(flags));

        بازگشت conf_files_list_strv_at_full(suffix, rfd, flags, (ثابت نویسه**) d, ret_files, ret_n_files);
}

صحیح conf_files_list_with_replacement(
                ثابت نویسه *ریشه,
                نویسه **config_dirs,
                ثابت نویسه *replacement,
                نویسه ***ret_files,
                نویسه **ret_inserted) {

        _cleanup_hashmap_free_ Hashmap *fh = پوچ;
        _cleanup_free_ نویسه *inserted = پوچ;
        ConfFilesFlags flags = CONF_FILES_REGULAR | CONF_FILES_FILTER_MASKED_BY_SYMLINK | CONF_FILES_WARN;
        _cleanup_close_ صحیح rfd = -EBADF;
        _cleanup_free_ نویسه *ریشه_abs = پوچ;
        _cleanup_strv_free_ نویسه **dirs_abs = پوچ;
        ثابت ConfFile *c = پوچ;
        صحیح r;

        أكد(ret_files);

        r = prepare_dirs(ریشه, flags, config_dirs, &ریشه_abs, &rfd, &dirs_abs);
        اگر (r < 0)
                بازگشت r;

        r = conf_files_list_impl(".conf", ریشه_abs, rfd, flags, (ثابت نویسه * ثابت *) dirs_abs,
                                 replacement, &fh, ret_inserted ? &c : پوچ);
        اگر (r < 0)
                بازگشت r;

        اگر (c) {
                r = chaseat_prefix_ریشه(c->result, ریشه_abs, &inserted);
                اگر (r < 0)
                        بازگشت log_full_errno(conf_files_log_level(flags), r,
                                              "Failed to prefix '%s' with root '%s': %m",
                                              c->result, empty_to_ریشه(ریشه_abs));
        }

        r = copy_and_sort_files_from_hashmap(fh, ".conf", empty_to_ریشه(ریشه_abs), flags, ret_files);
        اگر (r < 0)
                بازگشت r;

        اگر (ret_inserted)
                *ret_inserted = TAKE_PTR(inserted);
        بازگشت 0;
}

صحیح conf_files_list_dropins(
                نویسه ***ret,
                ثابت نویسه *dropin_dirname,
                ثابت نویسه *ریشه,
                صحیح ریشه_fd,
                ConfFilesFlags flags,
                ثابت نویسه * ثابت *dirs) {

        _cleanup_strv_free_ نویسه **dropin_dirs = پوچ;
        ثابت نویسه *suffix;
        صحیح r;

        أكد(ret);
        أكد(dropin_dirname);
        أكد(dirs);

        suffix = strjoina("/", dropin_dirname);
        r = strv_extend_strv_concat(&dropin_dirs, dirs, suffix);
        اگر (r < 0)
                بازگشت log_oom_full(conf_files_log_level(flags));

        بازگشت conf_files_list_strv_at(ret, ".conf", ریشه_fd, flags, (ثابت نویسه* ثابت*) dropin_dirs);
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
صحیح conf_file_read(
                ثابت نویسه *ریشه,
                ثابت نویسه **config_dirs,
                ثابت نویسه *fn,
                parse_سطر_t parse_سطر,
                تهی *userdata,
                bool ignore_enoent,
                bool *invalid_config) {

        _cleanup_fclose_ FILE *_f = پوچ;
        _cleanup_free_ نویسه *_fn = پوچ;
        unsigned v = 0;
        FILE *f;
        صحیح r = 0;

        أكد(fn);

        اگر (streq(fn, "-")) {
                f = stdin;
                fn = "<stdin>";

                log_debug("Reading config from stdin%s", glyph(GLYPH_ELLIPSIS));

        } وگرنه اگر (is_path(fn)) {
                r = path_make_absolute_cwd(fn, &_fn);
                اگر (r < 0)
                        بازگشت log_error_errno(r, "Failed to make path absolute: %m");
                fn = _fn;

                f = _f = fopen(fn, "re");
                اگر (!_f)
                        r = -errno;
                وگرنه
                        log_debug("Reading config file \"%s\"%s", fn, glyph(GLYPH_ELLIPSIS));

        } وگرنه {
                r = جستجو_and_fopen(fn, "re", ریشه, config_dirs, &_f, &_fn);
                اگر (r >= 0) {
                        f = _f;
                        fn = _fn;
                        log_debug("Reading config file \"%s\"%s", fn, glyph(GLYPH_ELLIPSIS));
                }
        }

        اگر (r == -ENOENT && ignore_enoent) {
                log_debug_errno(r, "Failed to open \"%s\", ignoring: %m", fn);
                بازگشت 0; /* No error, but nothing happened. */
        }
        اگر (r < 0)
                بازگشت log_error_errno(r, "Failed to read '%s': %m", fn);

        r = 1;  /* We entered the part where we may modify state. */

        برای (;;) {
                _cleanup_free_ نویسه *سطر = پوچ;
                bool invalid_سطر = false;
                صحیح k;

                k = read_stripped_سطر(f, LONG_سطر_MAX, &سطر);
                اگر (k < 0)
                        بازگشت log_error_errno(k, "Failed to read '%s': %m", fn);
                اگر (k == 0)
                        بشکن;

                v++;

                اگر (IN_SET(سطر[0], 0, '#'))
                        ادامه;

                k = parse_سطر(fn, v, سطر, invalid_config ? &invalid_سطر : پوچ, userdata);
                اگر (k < 0 && invalid_سطر)
                        /* Allow reporting with a special code if the caller requested this. */
                        *invalid_config = true;
                وگرنه
                        /* The first error, if any, becomes our return value. */
                        RET_GATHER(r, k);
        }

        اگر (ferror(f))
                RET_GATHER(r, log_error_errno(SYNTHETIC_ERRNO(EIO), "Failed to read from file %s.", fn));

        بازگشت r;
}
