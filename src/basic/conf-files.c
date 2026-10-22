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
                واپس تہی;

        آزاد(c->filename);
        آزاد(c->نتیجہ);
        آزاد(c->original_راستہ);
        آزاد(c->resolved_راستہ);
        محفوظ_close(c->fd);

        واپس mfree(c);
}

DEFINE_اشارہ_گر_ARRAY_FREE_FUNC(ConfFile*, conf_file_free);

ساکن صحیح conf_files_log_level(ConfFilesFlags flags) {
        واپس FLAGS_سیٹ(flags, CONF_FILES_WARN) ? LOG_WARNING : LOG_DEBUG;
}

ساکن صحیح prepare_dirs(
                ثابت حرف *جڑ,
                ConfFilesFlags flags,
                حرف * ثابت *dirs,
                حرف **ret_جڑ,
                صحیح *ret_rfd,
                حرف ***ret_dirs) {

        _cleanup_free_ حرف *جڑ_abs = تہی;
        _cleanup_strv_free_ حرف **dirs_abs = تہی;
        صحیح r;

        أكد(ret_جڑ);
        أكد(ret_rfd);
        أكد(ret_dirs || strv_isempty(dirs));

        صحیح log_level = conf_files_log_level(flags);

        r = خالی_خانے_or_جڑ_harder_مقصد_null(&جڑ);
        اگر (r < 0)
                واپس log_بھرا_errno(log_level, r, "Failed to determine if '%s' points to the root directory: %m", strempty(جڑ));

        اگر (ret_dirs) {
                dirs_abs = strv_copy(dirs);
                اگر (!dirs_abs)
                        واپس log_oom_بھرا(log_level);
        }

        _cleanup_close_ صحیح rfd = XAT_FDROOT;
        اگر (جڑ) {
                /* When a non-trivial root is specified, we will prefix the result later. Hence, it is not
                 * necessary to modify each config directories here. but needs to normalize the root directory. */
                r = راستہ_make_absolute_cwd(جڑ, &جڑ_abs);
                اگر (r < 0)
                        واپس log_بھرا_errno(log_level, r, "Failed to make '%s' absolute: %m", جڑ);

                راستہ_simplify(جڑ_abs);

                rfd = open(جڑ, O_CLOEXEC|O_DIRECTORY|O_راستہ);
                اگر (rfd < 0)
                        واپس log_بھرا_errno(log_level, errno, "Failed to open '%s': %m", جڑ_abs);

        } ورنہ اگر (ret_dirs) {
                /* When an empty root or "/" is specified, we will open "/" below, hence we need to make
                 * each config directory absolute if relative. */
                r = راستہ_strv_make_absolute_cwd(dirs_abs);
                اگر (r < 0)
                        واپس log_بھرا_errno(log_level, r, "Failed to make directories absolute: %m");
        }

        *ret_جڑ = TAKE_PTR(جڑ_abs);
        *ret_rfd = TAKE_FD(rfd);
        اگر (ret_dirs)
                *ret_dirs = TAKE_PTR(dirs_abs);
        واپس 0;
}

ساکن صحیح conf_file_سابقہ_جڑ(ConfFile *c, ثابت حرف *جڑ, ConfFilesFlags flags) {
        حرف *p;
        صحیح r;

        أكد(c);

        اگر (FLAGS_سیٹ(flags, CONF_FILES_DONT_سابقہ_جڑ))
                واپس 0;

        صحیح log_level = conf_files_log_level(flags);

        r = chaseat_سابقہ_جڑ(c->نتیجہ, جڑ, &p);
        اگر (r < 0)
                واپس log_بھرا_errno(log_level, r, "Failed to prefix '%s' with root '%s': %m", c->نتیجہ, جڑ);
        free_and_replace(c->نتیجہ, p);

        r = chaseat_سابقہ_جڑ(c->resolved_راستہ, جڑ, &p);
        اگر (r < 0)
                واپس log_بھرا_errno(log_level, r, "Failed to prefix '%s' with root '%s': %m", c->resolved_راستہ, جڑ);
        free_and_replace(c->resolved_راستہ, p);

        /* Do not use chaseat_prefix_root(), as it is for the result of chaseat(), but the path is not chased. */
        p = راستہ_join(خالی_خانے_مقصد_جڑ(جڑ), skip_leading_slash(c->original_راستہ));
        اگر (!p)
                واپس log_oom_بھرا(log_level);

        راستہ_simplify(p);
        واپس free_and_replace(c->original_راستہ, p);
}

ساکن bool conf_files_need_stat(ConfFilesFlags flags) {
        واپس (flags & (CONF_FILES_FILTER_MASKED | CONF_FILES_REGULAR | CONF_FILES_DIRECTORY | CONF_FILES_EXECUTABLE)) != 0;
}

ساکن ChaseFlags conf_files_chase_flags(ConfFilesFlags flags) {
        ChaseFlags chase_flags = 0;

        اگر (!conf_files_need_stat(flags) || FLAGS_سیٹ(flags, CONF_FILES_FILTER_MASKED_BY_SYMLINK))
                /* Even if no verification is requested, let's unconditionally call chaseat(),
                 * to drop unsafe symlinks. */
                chase_flags |= CHASE_NONEXISTENT;

        واپس chase_flags;
}

ساکن صحیح conf_file_chase_and_verify(
                ثابت حرف *جڑ,          /* for logging, can be NULL */
                صحیح rfd,
                ثابت حرف *original_راستہ, /* for logging */
                ثابت حرف *راستہ,
                ثابت حرف *name,
                سیٹ_2 **masked,              /* optional */
                ConfFilesFlags flags,
                حرف **ret_راستہ,
                صحیح *ret_fd,
                ساخت stat *ret_stat) {

        _cleanup_free_ حرف *resolved_راستہ = تہی;
        _cleanup_close_ صحیح fd = -EBADF;
        ساخت stat st = {};
        صحیح r;

        أكد(wildcard_fd_is_valid(rfd));
        أكد(original_راستہ);
        أكد(راستہ);
        أكد(name);

        صحیح log_level = conf_files_log_level(flags);

        جڑ = خالی_خانے_مقصد_جڑ(جڑ);

        r = chaseat(rfd, rfd, راستہ, conf_files_chase_flags(flags), &resolved_راستہ, &fd);
        اگر (r < 0)
                واپس log_بھرا_errno(log_level, r, "Failed to chase '%s%s': %m",
                                      جڑ, skip_leading_slash(original_راستہ));
        اگر (r == 0) {
                اگر (FLAGS_سیٹ(flags, CONF_FILES_FILTER_MASKED_BY_SYMLINK)) {
                        /* If the path points to /dev/null in a image or so, then the device node may not exist. */
                        اگر (راستہ_equal(skip_leading_slash(resolved_راستہ), "dev/null")) {
                                اگر (masked) {
                                        /* Mark this one as masked */
                                        r = سیٹ_put_strdup(masked, name);
                                        اگر (r < 0)
                                                واپس log_oom_بھرا(log_level);
                                }

                                واپس log_debug_errno(SYNTHETIC_ERRNO(ERFKILL),
                                                       "File '%s%s' is a mask (symlink to /dev/null).",
                                                       جڑ, skip_leading_slash(original_راستہ));
                        }
                }

                اگر (conf_files_need_stat(flags))
                        /* If we need to have stat, skip the entry. */
                        واپس log_بھرا_errno(log_level, SYNTHETIC_ERRNO(ENOENT), "Failed to chase '%s%s': %m",
                                              جڑ, skip_leading_slash(original_راستہ));
        }

        /* Even if we do not need stat, let's take stat now. The caller may use the info later. */
        اگر (fd >= 0 && fstat(fd, &st) < 0)
                واپس log_بھرا_errno(log_level, errno, "Failed to stat '%s%s': %m",
                                      جڑ, skip_leading_slash(original_راستہ));

        /* Is this a masking entry? */
        اگر (FLAGS_سیٹ(flags, CONF_FILES_FILTER_MASKED_BY_SYMLINK) && stat_may_be_dev_null(&st)) {
                اگر (masked) {
                        /* Mark this one as masked */
                        r = سیٹ_put_strdup(masked, name);
                        اگر (r < 0)
                                واپس log_oom_بھرا(log_level);
                }

                واپس log_debug_errno(SYNTHETIC_ERRNO(ERFKILL),
                                       "File '%s%s' is a mask (symlink to /dev/null).",
                                       جڑ, skip_leading_slash(original_راستہ));
        }

        اگر (FLAGS_سیٹ(flags, CONF_FILES_FILTER_MASKED_BY_خالی_خانے) && stat_is_خالی_خانے(&st)) {
                اگر (masked) {
                        /* Mark this one as masked */
                        r = سیٹ_put_strdup(masked, name);
                        اگر (r < 0)
                                واپس log_oom_بھرا(log_level);
                }

                واپس log_debug_errno(SYNTHETIC_ERRNO(ERFKILL),
                                       "File '%s%s' is a mask (an empty file).",
                                       جڑ, skip_leading_slash(original_راستہ));
        }

        اگر (FLAGS_سیٹ(flags, CONF_FILES_REGULAR|CONF_FILES_DIRECTORY)) {
                اگر (!S_ISREG(st.st_mode) && !S_ISDIR(st.st_mode))
                        واپس log_debug_errno(SYNTHETIC_ERRNO(EBADFD),
                                               "File '%s%s' is neither a regular file or directory.",
                                               جڑ, skip_leading_slash(original_راستہ));
        } ورنہ {
                /* Is this node a regular file? */
                اگر (FLAGS_سیٹ(flags, CONF_FILES_REGULAR)) {
                        r = stat_verify_regular(&st);
                        اگر (r < 0)
                                واپس log_debug_errno(r, "File '%s%s' is not a regular file: %m",
                                                       جڑ, skip_leading_slash(original_راستہ));
                }

                /* Is this node a directory? */
                اگر (FLAGS_سیٹ(flags, CONF_FILES_DIRECTORY)) {
                        r = stat_verify_directory(&st);
                        اگر (r < 0)
                                واپس log_debug_errno(r, "File '%s%s' is not a directory: %m",
                                                       جڑ, skip_leading_slash(original_راستہ));
                }
        }

        /* Does this node have the executable bit set?
         * As requested: check if the file is marked executable. Note that we don't check access(X_OK) here,
         * as we care about whether the file is marked executable at all, and not whether it is executable
         * for us, because if so, such errors are stuff we should log about. */
        اگر (FLAGS_سیٹ(flags, CONF_FILES_EXECUTABLE) && (st.st_mode & 0111) == 0)
                واپس log_debug_errno(SYNTHETIC_ERRNO(ENOEXEC),
                                       "File '%s%s' is not marked executable.",
                                       جڑ, skip_leading_slash(original_راستہ));

        اگر (ret_راستہ)
                *ret_راستہ = TAKE_PTR(resolved_راستہ);
        اگر (ret_fd)
                *ret_fd = TAKE_FD(fd);
        اگر (ret_stat)
                *ret_stat = st;

        واپس 0;
}

صحیح conf_file_new_at(
                ثابت حرف *راستہ,
                ثابت حرف *جڑ,
                صحیح rfd,
                ConfFilesFlags flags,
                ConfFile **ret) {
        صحیح r;

        أكد(راستہ);
        أكد(wildcard_fd_is_valid(rfd));
        أكد(ret);

        صحیح log_level = conf_files_log_level(flags);

        _cleanup_free_ حرف *جڑ_2 = تہی;
        اگر (DEBUG_LOGGING && !جڑ) {
                (خالی) fd_get_راستہ(rfd, &جڑ_2);
                جڑ = جڑ_2;
        }

        _cleanup_(conf_file_freep) ConfFile *c = new(ConfFile, 1);
        اگر (!c)
                واپس log_oom_بھرا(log_level);

        *c = (ConfFile) {
                .original_راستہ = strdup(راستہ),
                .fd = -EBADF,
        };

        اگر (!c->original_راستہ)
                واپس log_oom_بھرا(log_level);

        r = راستہ_نکالو_filename(راستہ, &c->filename);
        اگر (r < 0)
                واپس log_بھرا_errno(log_level, r, "Failed to extract filename from '%s': %m", راستہ);

        _cleanup_free_ حرف *dirpath = تہی, *resolved_dirpath = تہی;
        r = راستہ_نکالو_directory(راستہ, &dirpath);
        اگر (r < 0 && r != -EDESTADDRREQ)
                واپس log_بھرا_errno(log_level, r, "Failed to extract directory from '%s': %m", راستہ);
        اگر (r >= 0) {
                r = chaseat(rfd, rfd, dirpath,
                            CHASE_MUST_BE_DIRECTORY | conf_files_chase_flags(flags),
                            &resolved_dirpath, /* ret_fd= */ تہی);
                اگر (r < 0)
                        واپس log_بھرا_errno(log_level, r, "Failed to chase '%s%s': %m", خالی_خانے_مقصد_جڑ(جڑ), skip_leading_slash(dirpath));
        }

        c->نتیجہ = راستہ_join(resolved_dirpath, c->filename);
        اگر (!c->نتیجہ)
                واپس log_oom_بھرا(log_level);

        r = conf_file_chase_and_verify(
                        جڑ,
                        rfd,
                        c->original_راستہ,
                        c->نتیجہ,
                        c->filename,
                        /* masked= */ تہی,
                        flags,
                        &c->resolved_راستہ,
                        &c->fd,
                        &c->st);
        اگر (r < 0)
                واپس r;

        *ret = TAKE_PTR(c);
        واپس 0;
}

صحیح conf_file_new(ثابت حرف *راستہ, ثابت حرف *جڑ, ConfFilesFlags flags, ConfFile **ret) {
        صحیح r;

        أكد(راستہ);
        أكد(ret);

        _cleanup_free_ حرف *جڑ_abs = تہی;
        _cleanup_close_ صحیح rfd = -EBADF;
        r = prepare_dirs(جڑ, flags, /* dirs= */ تہی, &جڑ_abs, &rfd, /* ret_dirs= */ تہی);
        اگر (r < 0)
                واپس r;

        _cleanup_free_ حرف *راستہ_abs = تہی;
        اگر (!جڑ_abs) {
                r = راستہ_make_absolute_cwd(راستہ, &راستہ_abs);
                اگر (r < 0)
                        واپس log_بھرا_errno(conf_files_log_level(flags), r, "Failed to make '%s' absolute: %m", راستہ);

                راستہ = راستہ_abs;
        }

        _cleanup_(conf_file_freep) ConfFile *c = تہی;
        r = conf_file_new_at(راستہ, جڑ_abs, rfd, flags, &c);
        اگر (r < 0)
                واپس r;

        r = conf_file_سابقہ_جڑ(c, جڑ_abs, flags);
        اگر (r < 0)
                واپس r;

        *ret = TAKE_PTR(c);
        واپس 0;
}

DEFINE_نجی_HASH_OPS_WITH_قدر_DESTRUCTOR(
                conf_file_hash_ops,
                حرف, string_hash_func, string_compare_func,
                ConfFile, conf_file_free);

ساکن صحیح files_جوڑو(
                DIR *dir,
                ثابت حرف *original_dirpath,
                ثابت حرف *resolved_dirpath,
                ثابت حرف *جڑ, /* for logging, can be NULL */
                صحیح rfd,
                Hashmap **files,
                سیٹ_2 **masked,
                ثابت حرف *suffix,
                ConfFilesFlags flags) {

        صحیح r;

        أكد(dir);
        أكد(original_dirpath);
        أكد(resolved_dirpath);
        أكد(wildcard_fd_is_valid(rfd));
        أكد(files);
        أكد(masked);

        صحیح log_level = conf_files_log_level(flags);

        جڑ = خالی_خانے_مقصد_جڑ(جڑ);

        FOREACH_DIRENT(de, dir, واپس log_بھرا_errno(log_level, errno, "Failed to read directory '%s%s': %m",
                                                      جڑ, skip_leading_slash(original_dirpath))) {

                _cleanup_free_ حرف *original_راستہ = راستہ_join(original_dirpath, de->d_name);
                اگر (!original_راستہ)
                        واپس log_oom_بھرا(log_level);

                /* Does this match the suffix? */
                اگر (suffix && !endswith(de->d_name, suffix)) {
                        log_debug("Skipping file '%s%s', suffix is not '%s'.", جڑ, skip_leading_slash(original_راستہ), suffix);
                        جاری;
                }

                /* Has this file already been found in an earlier directory? */
                اگر (hashmap_شامل_ہے(*files, de->d_name)) {
                        log_debug("Skipping overridden file '%s%s'.", جڑ, skip_leading_slash(original_راستہ));
                        جاری;
                }

                /* Has this been masked in an earlier directory? */
                اگر ((flags & CONF_FILES_FILTER_MASKED) != 0 && سیٹ_شامل_ہے(*masked, de->d_name)) {
                        log_debug("File '%s%s' is masked by previous entry.", جڑ, skip_leading_slash(original_راستہ));
                        جاری;
                }

                _cleanup_free_ حرف *p = راستہ_join(resolved_dirpath, de->d_name);
                اگر (!p)
                        واپس log_oom_بھرا(log_level);

                _cleanup_free_ حرف *resolved_راستہ = تہی;
                _cleanup_close_ صحیح fd = -EBADF;
                ساخت stat st;
                r = conf_file_chase_and_verify(
                                جڑ,
                                rfd,
                                original_راستہ,
                                p,
                                de->d_name,
                                masked,
                                flags,
                                &resolved_راستہ,
                                &fd,
                                &st);
                اگر (r == -ENOMEM)
                        واپس r;
                اگر (r < 0)
                        جاری;

                _cleanup_(conf_file_freep) ConfFile *c = new(ConfFile, 1);
                اگر (!c)
                        واپس log_oom_بھرا(log_level);

                *c = (ConfFile) {
                        .filename = strdup(de->d_name),
                        .نتیجہ = TAKE_PTR(p),
                        .original_راستہ = TAKE_PTR(original_راستہ),
                        .resolved_راستہ = TAKE_PTR(resolved_راستہ),
                        .fd = TAKE_FD(fd),
                        .st = st,
                };

                اگر (!c->filename)
                        واپس log_oom_بھرا(log_level);

                r = hashmap_ensure_put(files, &conf_file_hash_ops, c->filename, c);
                اگر (r < 0) {
                        أكد(r == -ENOMEM);
                        واپس log_oom_بھرا(log_level);
                }
                أكد(r > 0);

                TAKE_PTR(c);
        }

        واپس 0;
}

ساکن صحیح dump_files(Hashmap *fh, ثابت حرف *جڑ, ConfFilesFlags flags, ConfFile ***ret_files, size_t *ret_n_files) {
        ConfFile **files = تہی;
        size_t n_files = 0;
        صحیح r;

        CLEANUP_ARRAY(files, n_files, conf_file_free_array);

        أكد(ret_files);
        أكد(ret_n_files);

        /* The entries in the array given by hashmap_dump_sorted() are still owned by the hashmap. */
        r = hashmap_dump_sorted(fh, (خالی***) &files, &n_files);
        اگر (r < 0)
                واپس log_oom_بھرا(conf_files_log_level(flags));

        /* Hence, we need to remove them from the hashmap. */
        FOREACH_ARRAY(i, files, n_files)
                assert_se(hashmap_remove(fh, (*i)->filename) == *i);

        اگر (جڑ)
                FOREACH_ARRAY(i, files, n_files) {
                        r = conf_file_سابقہ_جڑ(*i, جڑ, flags);
                        اگر (r < 0)
                                واپس r;
                }

        *ret_files = TAKE_PTR(files);
        *ret_n_files = n_files;
        واپس 0;
}

ساکن صحیح copy_and_ترتیب_دو_files_from_hashmap(
                Hashmap *fh,
                ثابت حرف *suffix,
                ثابت حرف *جڑ,
                ConfFilesFlags flags,
                حرف ***ret) {

        _cleanup_strv_free_ حرف **results = تہی;
        _cleanup_free_ ConfFile **files = تہی;
        size_t n_files = 0, n_results = 0;
        صحیح r;

        أكد(ret);

        صحیح log_level = conf_files_log_level(flags);

        /* The entries in the array given by hashmap_dump_sorted() are still owned by the hashmap.
         * Hence, do not use conf_file_free_array() for 'entries' */
        r = hashmap_dump_sorted(fh, (خالی***) &files, &n_files);
        اگر (r < 0)
                واپس log_oom_بھرا(log_level);

        FOREACH_ARRAY(i, files, n_files) {
                ConfFile *c = *i;
                ثابت حرف *جوڑو = تہی;

                اگر (FLAGS_سیٹ(flags, CONF_FILES_BASENAME))
                        جوڑو = c->filename;
                ورنہ اگر (جڑ && !FLAGS_سیٹ(flags, CONF_FILES_DONT_سابقہ_جڑ)) {
                        _cleanup_free_ حرف *p = تہی;

                        r = chaseat_سابقہ_جڑ(c->نتیجہ, جڑ, &p);
                        اگر (r < 0)
                                واپس log_بھرا_errno(log_level, r, "Failed to prefix '%s' with root '%s': %m", c->نتیجہ, جڑ);

                        اگر (FLAGS_سیٹ(flags, CONF_FILES_TRUNCATE_SUFFIX) && suffix) {
                                حرف *e = endswith(p, suffix);
                                اگر (!e)
                                        جاری;

                                *e = 0;
                        }

                        اگر (strv_consume_with_سائز(&results, &n_results, TAKE_PTR(p)) < 0)
                                واپس log_oom_بھرا(log_level);

                        جاری;
                } ورنہ
                        جوڑو = c->نتیجہ;

                اگر (FLAGS_سیٹ(flags, CONF_FILES_TRUNCATE_SUFFIX)) {
                        ثابت حرف *e = endswith(جوڑو, suffix);
                        اگر (!e)
                                جاری;

                        _cleanup_free_ حرف *n = strndup(جوڑو, e - جوڑو);
                        اگر (!n)
                                واپس log_oom_بھرا(log_level);

                        r = strv_consume_with_سائز(&results, &n_results, TAKE_PTR(n));
                } ورنہ
                        r = strv_extend_with_سائز(&results, &n_results, جوڑو);
                اگر (r < 0)
                        واپس log_oom_بھرا(log_level);
        }

        *ret = TAKE_PTR(results);
        واپس 0;
}

ساکن صحیح داخل_کرو_replacement(Hashmap **fh, ConfFile *replacement, ConfFilesFlags flags, ثابت ConfFile **ret) {
        _cleanup_(conf_file_freep) ConfFile *c = ASSERT_PTR(replacement);
        صحیح r;

        أكد(fh);
        أكد(ret);

        /* This consumes the input ConfFile. */

        ConfFile *existing = hashmap_get(*fh, c->filename);
        اگر (existing) {
                log_debug("An entry with higher priority '%s' -> '%s' already exists, ignoring the replacement: %s",
                          existing->filename, existing->نتیجہ, c->original_راستہ);
                *ret = تہی;
                واپس 0;
        }

        r = hashmap_ensure_put(fh, &conf_file_hash_ops, c->filename, c);
        اگر (r < 0) {
                أكد(r == -ENOMEM);
                واپس log_oom_بھرا(conf_files_log_level(flags));
        }
        أكد(r > 0);

        log_debug("Inserted replacement: '%s' -> '%s'", c->filename, c->نتیجہ);

        *ret = TAKE_PTR(c);
        واپس 0;
}

ساکن صحیح conf_files_list_impl(
                ثابت حرف *suffix,
                ثابت حرف *جڑ, /* for logging, can be NULL */
                صحیح rfd,
                ConfFilesFlags flags,
                ثابت حرف * ثابت *dirs,
                ثابت حرف *replacement,
                Hashmap **ret,
                ثابت ConfFile **ret_inserted) {

        _cleanup_hashmap_free_ Hashmap *fh = تہی;
        cleanup_سیٹ_free سیٹ_2 *masked = تہی;
        _cleanup_(conf_file_freep) ConfFile *c = تہی;
        ثابت ConfFile *inserted = تہی;
        صحیح r;

        أكد(wildcard_fd_is_valid(rfd));
        أكد(ret);

        جڑ = خالی_خانے_مقصد_جڑ(جڑ);

        اگر (replacement) {
                r = conf_file_new_at(replacement, جڑ, rfd, flags & CONF_FILES_WARN, &c);
                اگر (r < 0)
                        واپس r;
        }

        STRV_FOREACH(p, dirs) {
                _cleanup_closedir_ DIR *dir = تہی;
                _cleanup_free_ حرف *راستہ = تہی;

                r = chase_and_opendirat(rfd, rfd, *p, 0, &راستہ, &dir);
                اگر (r < 0) {
                        اگر (r != -ENOENT)
                                log_بھرا_errno(conf_files_log_level(flags), r,
                                               "Failed to chase and open directory '%s%s', ignoring: %m",
                                               جڑ, skip_leading_slash(*p));
                        جاری;
                }

                اگر (c && streq_ptr(راستہ_startswith(c->نتیجہ, راستہ), c->filename)) {
                        r = داخل_کرو_replacement(&fh, TAKE_PTR(c), flags, &inserted);
                        اگر (r < 0)
                                واپس r;
                }

                r = files_جوڑو(dir, *p, راستہ, جڑ, rfd, &fh, &masked, suffix, flags);
                اگر (r == -ENOMEM)
                        واپس r;
        }

        اگر (c) {
                r = داخل_کرو_replacement(&fh, TAKE_PTR(c), flags, &inserted);
                اگر (r < 0)
                        واپس r;
        }

        *ret = TAKE_PTR(fh);
        اگر (ret_inserted)
                *ret_inserted = inserted;
        واپس 0;
}

صحیح conf_files_list_strv(
                حرف ***ret,
                ثابت حرف *suffix,
                ثابت حرف *جڑ,
                ConfFilesFlags flags,
                ثابت حرف * ثابت *dirs) {

        _cleanup_hashmap_free_ Hashmap *fh = تہی;
        _cleanup_close_ صحیح rfd = -EBADF;
        _cleanup_free_ حرف *جڑ_abs = تہی;
        _cleanup_strv_free_ حرف **dirs_abs = تہی;
        صحیح r;

        أكد(ret);

        r = prepare_dirs(جڑ, flags, (حرف**) dirs, &جڑ_abs, &rfd, &dirs_abs);
        اگر (r < 0)
                واپس r;

        r = conf_files_list_impl(suffix, جڑ_abs, rfd, flags, (ثابت حرف * ثابت *) dirs_abs,
                                 /* replacement= */ تہی, &fh, /* ret_inserted= */ تہی);
        اگر (r < 0)
                واپس r;

        واپس copy_and_ترتیب_دو_files_from_hashmap(fh, suffix, خالی_خانے_مقصد_جڑ(جڑ_abs), flags, ret);
}

صحیح conf_files_list_strv_بھرا(
                ثابت حرف *suffix,
                ثابت حرف *جڑ,
                ConfFilesFlags flags,
                ثابت حرف * ثابت *dirs,
                ConfFile ***ret_files,
                size_t *ret_n_files) {

        _cleanup_hashmap_free_ Hashmap *fh = تہی;
        _cleanup_close_ صحیح rfd = -EBADF;
        _cleanup_free_ حرف *جڑ_abs = تہی;
        _cleanup_strv_free_ حرف **dirs_abs = تہی;
        صحیح r;

        أكد(ret_files);
        أكد(ret_n_files);

        r = prepare_dirs(جڑ, flags, (حرف**) dirs, &جڑ_abs, &rfd, &dirs_abs);
        اگر (r < 0)
                واپس r;

        r = conf_files_list_impl(suffix, جڑ_abs, rfd, flags, (ثابت حرف * ثابت *) dirs_abs,
                                 /* replacement= */ تہی, &fh, /* ret_inserted= */ تہی);
        اگر (r < 0)
                واپس r;

        واپس dump_files(fh, خالی_خانے_مقصد_جڑ(جڑ_abs), flags, ret_files, ret_n_files);
}

صحیح conf_files_list_strv_at(
                حرف ***ret,
                ثابت حرف *suffix,
                صحیح rfd,
                ConfFilesFlags flags,
                ثابت حرف * ثابت *dirs) {

        _cleanup_hashmap_free_ Hashmap *fh = تہی;
        _cleanup_free_ حرف *جڑ = تہی;
        صحیح r;

        أكد(wildcard_fd_is_valid(rfd));
        أكد(ret);

        اگر (DEBUG_LOGGING)
                (خالی) fd_get_راستہ(rfd, &جڑ); /* for logging */

        r = conf_files_list_impl(suffix, جڑ, rfd, flags, dirs, /* replacement= */ تہی, &fh, /* ret_inserted= */ تہی);
        اگر (r < 0)
                واپس r;

        واپس copy_and_ترتیب_دو_files_from_hashmap(fh, suffix, /* root= */ تہی, flags, ret);
}

صحیح conf_files_list_strv_at_بھرا(
                ثابت حرف *suffix,
                صحیح rfd,
                ConfFilesFlags flags,
                ثابت حرف * ثابت *dirs,
                ConfFile ***ret_files,
                size_t *ret_n_files) {

        _cleanup_hashmap_free_ Hashmap *fh = تہی;
        _cleanup_free_ حرف *جڑ = تہی;
        صحیح r;

        أكد(wildcard_fd_is_valid(rfd));
        أكد(ret_files);
        أكد(ret_n_files);

        اگر (DEBUG_LOGGING)
                (خالی) fd_get_راستہ(rfd, &جڑ); /* for logging */

        r = conf_files_list_impl(suffix, جڑ, rfd, flags, dirs, /* replacement= */ تہی, &fh, /* ret_inserted= */ تہی);
        اگر (r < 0)
                واپس r;

        واپس dump_files(fh, /* root= */ تہی, flags, ret_files, ret_n_files);
}

صحیح conf_files_list(حرف ***ret, ثابت حرف *suffix, ثابت حرف *جڑ, ConfFilesFlags flags, ثابت حرف *dir) {
        واپس conf_files_list_strv(ret, suffix, جڑ, flags, STRV_MAKE_CONST(dir));
}

صحیح conf_files_list_بھرا(ثابت حرف *suffix, ثابت حرف *جڑ, ConfFilesFlags flags, ثابت حرف *dir, ConfFile ***ret_files, size_t *ret_n_files) {
        واپس conf_files_list_strv_بھرا(suffix, جڑ, flags, STRV_MAKE_CONST(dir), ret_files, ret_n_files);
}

صحیح conf_files_list_at(حرف ***ret, ثابت حرف *suffix, صحیح rfd, ConfFilesFlags flags, ثابت حرف *dir) {
        واپس conf_files_list_strv_at(ret, suffix, rfd, flags, STRV_MAKE_CONST(dir));
}

صحیح conf_files_list_at_بھرا(ثابت حرف *suffix, صحیح rfd, ConfFilesFlags flags, ثابت حرف *dir, ConfFile ***ret_files, size_t *ret_n_files) {
        واپس conf_files_list_strv_at_بھرا(suffix, rfd, flags, STRV_MAKE_CONST(dir), ret_files, ret_n_files);
}

صحیح conf_files_list_nulstr(حرف ***ret, ثابت حرف *suffix, ثابت حرف *جڑ, ConfFilesFlags flags, ثابت حرف *dirs) {
        _cleanup_strv_free_ حرف **d = تہی;

        أكد(ret);

        d = strv_بانٹو_nulstr(dirs);
        اگر (!d)
                واپس log_oom_بھرا(conf_files_log_level(flags));

        واپس conf_files_list_strv(ret, suffix, جڑ, flags, (ثابت حرف**) d);
}

صحیح conf_files_list_nulstr_بھرا(ثابت حرف *suffix, ثابت حرف *جڑ, ConfFilesFlags flags, ثابت حرف *dirs, ConfFile ***ret_files, size_t *ret_n_files) {
        _cleanup_strv_free_ حرف **d = تہی;

        أكد(ret_files);
        أكد(ret_n_files);

        d = strv_بانٹو_nulstr(dirs);
        اگر (!d)
                واپس log_oom_بھرا(conf_files_log_level(flags));

        واپس conf_files_list_strv_بھرا(suffix, جڑ, flags, (ثابت حرف**) d, ret_files, ret_n_files);
}

صحیح conf_files_list_nulstr_at(حرف ***ret, ثابت حرف *suffix, صحیح rfd, ConfFilesFlags flags, ثابت حرف *dirs) {
        _cleanup_strv_free_ حرف **d = تہی;

        أكد(ret);

        d = strv_بانٹو_nulstr(dirs);
        اگر (!d)
                واپس log_oom_بھرا(conf_files_log_level(flags));

        واپس conf_files_list_strv_at(ret, suffix, rfd, flags, (ثابت حرف**) d);
}

صحیح conf_files_list_nulstr_at_بھرا(ثابت حرف *suffix, صحیح rfd, ConfFilesFlags flags, ثابت حرف *dirs, ConfFile ***ret_files, size_t *ret_n_files) {
        _cleanup_strv_free_ حرف **d = تہی;

        أكد(ret_files);
        أكد(ret_n_files);

        d = strv_بانٹو_nulstr(dirs);
        اگر (!d)
                واپس log_oom_بھرا(conf_files_log_level(flags));

        واپس conf_files_list_strv_at_بھرا(suffix, rfd, flags, (ثابت حرف**) d, ret_files, ret_n_files);
}

صحیح conf_files_list_with_replacement(
                ثابت حرف *جڑ,
                حرف **config_dirs,
                ثابت حرف *replacement,
                حرف ***ret_files,
                حرف **ret_inserted) {

        _cleanup_hashmap_free_ Hashmap *fh = تہی;
        _cleanup_free_ حرف *inserted = تہی;
        ConfFilesFlags flags = CONF_FILES_REGULAR | CONF_FILES_FILTER_MASKED_BY_SYMLINK | CONF_FILES_WARN;
        _cleanup_close_ صحیح rfd = -EBADF;
        _cleanup_free_ حرف *جڑ_abs = تہی;
        _cleanup_strv_free_ حرف **dirs_abs = تہی;
        ثابت ConfFile *c = تہی;
        صحیح r;

        أكد(ret_files);

        r = prepare_dirs(جڑ, flags, config_dirs, &جڑ_abs, &rfd, &dirs_abs);
        اگر (r < 0)
                واپس r;

        r = conf_files_list_impl(".conf", جڑ_abs, rfd, flags, (ثابت حرف * ثابت *) dirs_abs,
                                 replacement, &fh, ret_inserted ? &c : تہی);
        اگر (r < 0)
                واپس r;

        اگر (c) {
                r = chaseat_سابقہ_جڑ(c->نتیجہ, جڑ_abs, &inserted);
                اگر (r < 0)
                        واپس log_بھرا_errno(conf_files_log_level(flags), r,
                                              "Failed to prefix '%s' with root '%s': %m",
                                              c->نتیجہ, خالی_خانے_مقصد_جڑ(جڑ_abs));
        }

        r = copy_and_ترتیب_دو_files_from_hashmap(fh, ".conf", خالی_خانے_مقصد_جڑ(جڑ_abs), flags, ret_files);
        اگر (r < 0)
                واپس r;

        اگر (ret_inserted)
                *ret_inserted = TAKE_PTR(inserted);
        واپس 0;
}

صحیح conf_files_list_dropins(
                حرف ***ret,
                ثابت حرف *dropin_dirname,
                ثابت حرف *جڑ,
                صحیح جڑ_fd,
                ConfFilesFlags flags,
                ثابت حرف * ثابت *dirs) {

        _cleanup_strv_free_ حرف **dropin_dirs = تہی;
        ثابت حرف *suffix;
        صحیح r;

        أكد(ret);
        أكد(dropin_dirname);
        أكد(dirs);

        suffix = strjoina("/", dropin_dirname);
        r = strv_extend_strv_concat(&dropin_dirs, dirs, suffix);
        اگر (r < 0)
                واپس log_oom_بھرا(conf_files_log_level(flags));

        واپس conf_files_list_strv_at(ret, ".conf", جڑ_fd, flags, (ثابت حرف* ثابت*) dropin_dirs);
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
                ثابت حرف *جڑ,
                ثابت حرف **config_dirs,
                ثابت حرف *fn,
                parse_سطر_t parse_سطر,
                خالی *userdata,
                bool ignore_enoent,
                bool *invalid_config) {

        _cleanup_fclose_ FILE *_f = تہی;
        _cleanup_free_ حرف *_fn = تہی;
        غیرعلامتی v = 0;
        FILE *f;
        صحیح r = 0;

        أكد(fn);

        اگر (streq(fn, "-")) {
                f = stdin;
                fn = "<stdin>";

                log_debug("Reading config from stdin%s", glyph(GLYPH_ELLIPSIS));

        } ورنہ اگر (is_راستہ(fn)) {
                r = راستہ_make_absolute_cwd(fn, &_fn);
                اگر (r < 0)
                        واپس log_error_errno(r, "Failed to make path absolute: %m");
                fn = _fn;

                f = _f = fopen(fn, "re");
                اگر (!_f)
                        r = -errno;
                ورنہ
                        log_debug("Reading config file \"%s\"%s", fn, glyph(GLYPH_ELLIPSIS));

        } ورنہ {
                r = تلاش_and_fopen(fn, "re", جڑ, config_dirs, &_f, &_fn);
                اگر (r >= 0) {
                        f = _f;
                        fn = _fn;
                        log_debug("Reading config file \"%s\"%s", fn, glyph(GLYPH_ELLIPSIS));
                }
        }

        اگر (r == -ENOENT && ignore_enoent) {
                log_debug_errno(r, "Failed to open \"%s\", ignoring: %m", fn);
                واپس 0; /* No error, but nothing happened. */
        }
        اگر (r < 0)
                واپس log_error_errno(r, "Failed to read '%s': %m", fn);

        r = 1;  /* We entered the part where we may modify state. */

        برائے (;;) {
                _cleanup_free_ حرف *سطر = تہی;
                bool invalid_سطر = false;
                صحیح k;

                k = read_stripped_سطر(f, LONG_سطر_MAX, &سطر);
                اگر (k < 0)
                        واپس log_error_errno(k, "Failed to read '%s': %m", fn);
                اگر (k == 0)
                        توڑو;

                v++;

                اگر (اندر_سیٹ(سطر[0], 0, '#'))
                        جاری;

                k = parse_سطر(fn, v, سطر, invalid_config ? &invalid_سطر : تہی, userdata);
                اگر (k < 0 && invalid_سطر)
                        /* Allow reporting with a special code if the caller requested this. */
                        *invalid_config = true;
                ورنہ
                        /* The first error, if any, becomes our return value. */
                        RET_GATHER(r, k);
        }

        اگر (ferror(f))
                RET_GATHER(r, log_error_errno(SYNTHETIC_ERRNO(EIO), "Failed to read from file %s.", fn));

        واپس r;
}
