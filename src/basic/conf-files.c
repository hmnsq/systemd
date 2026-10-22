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
        حرر(c->نتیجہ);
        حرر(c->original_رستہ);
        حرر(c->resolved_رستہ);
        محفوظ_close(c->fd);

        أرجع mfree(c);
}

DEFINE_اشارہ_گر_ARRAY_FREE_FUNC(ConfFile*, conf_file_free);

ساكن صحيح conf_files_log_level(ConfFilesFlags flags) {
        أرجع FLAGS_سیٹ(flags, CONF_FILES_WARN) ? LOG_WARNING : LOG_DEBUG;
}

ساكن صحيح prepare_dirs(
                ثابت حرف *جڑ,
                ConfFilesFlags flags,
                حرف * ثابت *dirs,
                حرف **ret_جڑ,
                صحيح *ret_rfd,
                حرف ***ret_dirs) {

        _cleanup_free_ حرف *جڑ_abs = عدم;
        _cleanup_strv_free_ حرف **dirs_abs = عدم;
        صحيح r;

        أكد(ret_جڑ);
        أكد(ret_rfd);
        أكد(ret_dirs || strv_isempty(dirs));

        صحيح log_level = conf_files_log_level(flags);

        r = خالی_تھاواں_or_جڑ_harder_منزل_null(&جڑ);
        إذا (r < 0)
                أرجع log_بھریا_errno(log_level, r, "Failed to determine if '%s' points to the root directory: %m", strempty(جڑ));

        إذا (ret_dirs) {
                dirs_abs = strv_copy(dirs);
                إذا (!dirs_abs)
                        أرجع log_oom_بھریا(log_level);
        }

        _cleanup_close_ صحيح rfd = XAT_FDROOT;
        إذا (جڑ) {
                /* When a non-trivial root is specified, we will prefix the result later. Hence, it is not
                 * necessary to modify each config directories here. but needs to normalize the root directory. */
                r = رستہ_make_absolute_cwd(جڑ, &جڑ_abs);
                إذا (r < 0)
                        أرجع log_بھریا_errno(log_level, r, "Failed to make '%s' absolute: %m", جڑ);

                رستہ_simplify(جڑ_abs);

                rfd = open(جڑ, O_CLOEXEC|O_DIRECTORY|O_رستہ);
                إذا (rfd < 0)
                        أرجع log_بھریا_errno(log_level, errno, "Failed to open '%s': %m", جڑ_abs);

        } وإلا إذا (ret_dirs) {
                /* When an empty root or "/" is specified, we will open "/" below, hence we need to make
                 * each config directory absolute if relative. */
                r = رستہ_strv_make_absolute_cwd(dirs_abs);
                إذا (r < 0)
                        أرجع log_بھریا_errno(log_level, r, "Failed to make directories absolute: %m");
        }

        *ret_جڑ = TAKE_PTR(جڑ_abs);
        *ret_rfd = TAKE_FD(rfd);
        إذا (ret_dirs)
                *ret_dirs = TAKE_PTR(dirs_abs);
        أرجع 0;
}

ساكن صحيح conf_file_اگیتر_جڑ(ConfFile *c, ثابت حرف *جڑ, ConfFilesFlags flags) {
        حرف *p;
        صحيح r;

        أكد(c);

        إذا (FLAGS_سیٹ(flags, CONF_FILES_DONT_اگیتر_جڑ))
                أرجع 0;

        صحيح log_level = conf_files_log_level(flags);

        r = chaseat_اگیتر_جڑ(c->نتیجہ, جڑ, &p);
        إذا (r < 0)
                أرجع log_بھریا_errno(log_level, r, "Failed to prefix '%s' with root '%s': %m", c->نتیجہ, جڑ);
        free_and_replace(c->نتیجہ, p);

        r = chaseat_اگیتر_جڑ(c->resolved_رستہ, جڑ, &p);
        إذا (r < 0)
                أرجع log_بھریا_errno(log_level, r, "Failed to prefix '%s' with root '%s': %m", c->resolved_رستہ, جڑ);
        free_and_replace(c->resolved_رستہ, p);

        /* Do not use chaseat_prefix_root(), as it is for the result of chaseat(), but the path is not chased. */
        p = رستہ_join(خالی_تھاواں_منزل_جڑ(جڑ), skip_leading_slash(c->original_رستہ));
        إذا (!p)
                أرجع log_oom_بھریا(log_level);

        رستہ_simplify(p);
        أرجع free_and_replace(c->original_رستہ, p);
}

ساكن bool conf_files_need_stat(ConfFilesFlags flags) {
        أرجع (flags & (CONF_FILES_FILTER_MASKED | CONF_FILES_REGULAR | CONF_FILES_DIRECTORY | CONF_FILES_EXECUTABLE)) != 0;
}

ساكن ChaseFlags conf_files_chase_flags(ConfFilesFlags flags) {
        ChaseFlags chase_flags = 0;

        إذا (!conf_files_need_stat(flags) || FLAGS_سیٹ(flags, CONF_FILES_FILTER_MASKED_BY_SYMLINK))
                /* Even if no verification is requested, let's unconditionally call chaseat(),
                 * to drop unsafe symlinks. */
                chase_flags |= CHASE_NONEXISTENT;

        أرجع chase_flags;
}

ساكن صحيح conf_file_chase_and_verify(
                ثابت حرف *جڑ,          /* for logging, can be NULL */
                صحيح rfd,
                ثابت حرف *original_رستہ, /* for logging */
                ثابت حرف *رستہ,
                ثابت حرف *name,
                سیٹ_2 **masked,              /* optional */
                ConfFilesFlags flags,
                حرف **ret_رستہ,
                صحيح *ret_fd,
                بنية stat *ret_stat) {

        _cleanup_free_ حرف *resolved_رستہ = عدم;
        _cleanup_close_ صحيح fd = -EBADF;
        بنية stat st = {};
        صحيح r;

        أكد(wildcard_fd_is_valid(rfd));
        أكد(original_رستہ);
        أكد(رستہ);
        أكد(name);

        صحيح log_level = conf_files_log_level(flags);

        جڑ = خالی_تھاواں_منزل_جڑ(جڑ);

        r = chaseat(rfd, rfd, رستہ, conf_files_chase_flags(flags), &resolved_رستہ, &fd);
        إذا (r < 0)
                أرجع log_بھریا_errno(log_level, r, "Failed to chase '%s%s': %m",
                                      جڑ, skip_leading_slash(original_رستہ));
        إذا (r == 0) {
                إذا (FLAGS_سیٹ(flags, CONF_FILES_FILTER_MASKED_BY_SYMLINK)) {
                        /* If the path points to /dev/null in a image or so, then the device node may not exist. */
                        إذا (رستہ_equal(skip_leading_slash(resolved_رستہ), "dev/null")) {
                                إذا (masked) {
                                        /* Mark this one as masked */
                                        r = سیٹ_put_strdup(masked, name);
                                        إذا (r < 0)
                                                أرجع log_oom_بھریا(log_level);
                                }

                                أرجع log_debug_errno(SYNTHETIC_ERRNO(ERFKILL),
                                                       "File '%s%s' is a mask (symlink to /dev/null).",
                                                       جڑ, skip_leading_slash(original_رستہ));
                        }
                }

                إذا (conf_files_need_stat(flags))
                        /* If we need to have stat, skip the entry. */
                        أرجع log_بھریا_errno(log_level, SYNTHETIC_ERRNO(ENOENT), "Failed to chase '%s%s': %m",
                                              جڑ, skip_leading_slash(original_رستہ));
        }

        /* Even if we do not need stat, let's take stat now. The caller may use the info later. */
        إذا (fd >= 0 && fstat(fd, &st) < 0)
                أرجع log_بھریا_errno(log_level, errno, "Failed to stat '%s%s': %m",
                                      جڑ, skip_leading_slash(original_رستہ));

        /* Is this a masking entry? */
        إذا (FLAGS_سیٹ(flags, CONF_FILES_FILTER_MASKED_BY_SYMLINK) && stat_may_be_dev_null(&st)) {
                إذا (masked) {
                        /* Mark this one as masked */
                        r = سیٹ_put_strdup(masked, name);
                        إذا (r < 0)
                                أرجع log_oom_بھریا(log_level);
                }

                أرجع log_debug_errno(SYNTHETIC_ERRNO(ERFKILL),
                                       "File '%s%s' is a mask (symlink to /dev/null).",
                                       جڑ, skip_leading_slash(original_رستہ));
        }

        إذا (FLAGS_سیٹ(flags, CONF_FILES_FILTER_MASKED_BY_خالی_تھاواں) && stat_is_خالی_تھاواں(&st)) {
                إذا (masked) {
                        /* Mark this one as masked */
                        r = سیٹ_put_strdup(masked, name);
                        إذا (r < 0)
                                أرجع log_oom_بھریا(log_level);
                }

                أرجع log_debug_errno(SYNTHETIC_ERRNO(ERFKILL),
                                       "File '%s%s' is a mask (an empty file).",
                                       جڑ, skip_leading_slash(original_رستہ));
        }

        إذا (FLAGS_سیٹ(flags, CONF_FILES_REGULAR|CONF_FILES_DIRECTORY)) {
                إذا (!S_ISREG(st.st_mode) && !S_ISDIR(st.st_mode))
                        أرجع log_debug_errno(SYNTHETIC_ERRNO(EBADFD),
                                               "File '%s%s' is neither a regular file or directory.",
                                               جڑ, skip_leading_slash(original_رستہ));
        } وإلا {
                /* Is this node a regular file? */
                إذا (FLAGS_سیٹ(flags, CONF_FILES_REGULAR)) {
                        r = stat_verify_regular(&st);
                        إذا (r < 0)
                                أرجع log_debug_errno(r, "File '%s%s' is not a regular file: %m",
                                                       جڑ, skip_leading_slash(original_رستہ));
                }

                /* Is this node a directory? */
                إذا (FLAGS_سیٹ(flags, CONF_FILES_DIRECTORY)) {
                        r = stat_verify_directory(&st);
                        إذا (r < 0)
                                أرجع log_debug_errno(r, "File '%s%s' is not a directory: %m",
                                                       جڑ, skip_leading_slash(original_رستہ));
                }
        }

        /* Does this node have the executable bit set?
         * As requested: check if the file is marked executable. Note that we don't check access(X_OK) here,
         * as we care about whether the file is marked executable at all, and not whether it is executable
         * for us, because if so, such errors are stuff we should log about. */
        إذا (FLAGS_سیٹ(flags, CONF_FILES_EXECUTABLE) && (st.st_mode & 0111) == 0)
                أرجع log_debug_errno(SYNTHETIC_ERRNO(ENOEXEC),
                                       "File '%s%s' is not marked executable.",
                                       جڑ, skip_leading_slash(original_رستہ));

        إذا (ret_رستہ)
                *ret_رستہ = TAKE_PTR(resolved_رستہ);
        إذا (ret_fd)
                *ret_fd = TAKE_FD(fd);
        إذا (ret_stat)
                *ret_stat = st;

        أرجع 0;
}

صحيح conf_file_new_at(
                ثابت حرف *رستہ,
                ثابت حرف *جڑ,
                صحيح rfd,
                ConfFilesFlags flags,
                ConfFile **ret) {
        صحيح r;

        أكد(رستہ);
        أكد(wildcard_fd_is_valid(rfd));
        أكد(ret);

        صحيح log_level = conf_files_log_level(flags);

        _cleanup_free_ حرف *جڑ_2 = عدم;
        إذا (DEBUG_LOGGING && !جڑ) {
                (فراغ) fd_get_رستہ(rfd, &جڑ_2);
                جڑ = جڑ_2;
        }

        _cleanup_(conf_file_freep) ConfFile *c = new(ConfFile, 1);
        إذا (!c)
                أرجع log_oom_بھریا(log_level);

        *c = (ConfFile) {
                .original_رستہ = strdup(رستہ),
                .fd = -EBADF,
        };

        إذا (!c->original_رستہ)
                أرجع log_oom_بھریا(log_level);

        r = رستہ_کڈھو_filename(رستہ, &c->filename);
        إذا (r < 0)
                أرجع log_بھریا_errno(log_level, r, "Failed to extract filename from '%s': %m", رستہ);

        _cleanup_free_ حرف *dirpath = عدم, *resolved_dirpath = عدم;
        r = رستہ_کڈھو_directory(رستہ, &dirpath);
        إذا (r < 0 && r != -EDESTADDRREQ)
                أرجع log_بھریا_errno(log_level, r, "Failed to extract directory from '%s': %m", رستہ);
        إذا (r >= 0) {
                r = chaseat(rfd, rfd, dirpath,
                            CHASE_MUST_BE_DIRECTORY | conf_files_chase_flags(flags),
                            &resolved_dirpath, /* ret_fd= */ عدم);
                إذا (r < 0)
                        أرجع log_بھریا_errno(log_level, r, "Failed to chase '%s%s': %m", خالی_تھاواں_منزل_جڑ(جڑ), skip_leading_slash(dirpath));
        }

        c->نتیجہ = رستہ_join(resolved_dirpath, c->filename);
        إذا (!c->نتیجہ)
                أرجع log_oom_بھریا(log_level);

        r = conf_file_chase_and_verify(
                        جڑ,
                        rfd,
                        c->original_رستہ,
                        c->نتیجہ,
                        c->filename,
                        /* masked= */ عدم,
                        flags,
                        &c->resolved_رستہ,
                        &c->fd,
                        &c->st);
        إذا (r < 0)
                أرجع r;

        *ret = TAKE_PTR(c);
        أرجع 0;
}

صحيح conf_file_new(ثابت حرف *رستہ, ثابت حرف *جڑ, ConfFilesFlags flags, ConfFile **ret) {
        صحيح r;

        أكد(رستہ);
        أكد(ret);

        _cleanup_free_ حرف *جڑ_abs = عدم;
        _cleanup_close_ صحيح rfd = -EBADF;
        r = prepare_dirs(جڑ, flags, /* dirs= */ عدم, &جڑ_abs, &rfd, /* ret_dirs= */ عدم);
        إذا (r < 0)
                أرجع r;

        _cleanup_free_ حرف *رستہ_abs = عدم;
        إذا (!جڑ_abs) {
                r = رستہ_make_absolute_cwd(رستہ, &رستہ_abs);
                إذا (r < 0)
                        أرجع log_بھریا_errno(conf_files_log_level(flags), r, "Failed to make '%s' absolute: %m", رستہ);

                رستہ = رستہ_abs;
        }

        _cleanup_(conf_file_freep) ConfFile *c = عدم;
        r = conf_file_new_at(رستہ, جڑ_abs, rfd, flags, &c);
        إذا (r < 0)
                أرجع r;

        r = conf_file_اگیتر_جڑ(c, جڑ_abs, flags);
        إذا (r < 0)
                أرجع r;

        *ret = TAKE_PTR(c);
        أرجع 0;
}

DEFINE_نجی_HASH_OPS_WITH_قدر_DESTRUCTOR(
                conf_file_hash_ops,
                حرف, string_hash_func, string_compare_func,
                ConfFile, conf_file_free);

ساكن صحيح files_جوڑو(
                DIR *dir,
                ثابت حرف *original_dirpath,
                ثابت حرف *resolved_dirpath,
                ثابت حرف *جڑ, /* for logging, can be NULL */
                صحيح rfd,
                Hashmap **files,
                سیٹ_2 **masked,
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

        جڑ = خالی_تھاواں_منزل_جڑ(جڑ);

        FOREACH_DIRENT(de, dir, أرجع log_بھریا_errno(log_level, errno, "Failed to read directory '%s%s': %m",
                                                      جڑ, skip_leading_slash(original_dirpath))) {

                _cleanup_free_ حرف *original_رستہ = رستہ_join(original_dirpath, de->d_name);
                إذا (!original_رستہ)
                        أرجع log_oom_بھریا(log_level);

                /* Does this match the suffix? */
                إذا (suffix && !endswith(de->d_name, suffix)) {
                        log_debug("Skipping file '%s%s', suffix is not '%s'.", جڑ, skip_leading_slash(original_رستہ), suffix);
                        استمر;
                }

                /* Has this file already been found in an earlier directory? */
                إذا (hashmap_شامل_اے(*files, de->d_name)) {
                        log_debug("Skipping overridden file '%s%s'.", جڑ, skip_leading_slash(original_رستہ));
                        استمر;
                }

                /* Has this been masked in an earlier directory? */
                إذا ((flags & CONF_FILES_FILTER_MASKED) != 0 && سیٹ_شامل_اے(*masked, de->d_name)) {
                        log_debug("File '%s%s' is masked by previous entry.", جڑ, skip_leading_slash(original_رستہ));
                        استمر;
                }

                _cleanup_free_ حرف *p = رستہ_join(resolved_dirpath, de->d_name);
                إذا (!p)
                        أرجع log_oom_بھریا(log_level);

                _cleanup_free_ حرف *resolved_رستہ = عدم;
                _cleanup_close_ صحيح fd = -EBADF;
                بنية stat st;
                r = conf_file_chase_and_verify(
                                جڑ,
                                rfd,
                                original_رستہ,
                                p,
                                de->d_name,
                                masked,
                                flags,
                                &resolved_رستہ,
                                &fd,
                                &st);
                إذا (r == -ENOMEM)
                        أرجع r;
                إذا (r < 0)
                        استمر;

                _cleanup_(conf_file_freep) ConfFile *c = new(ConfFile, 1);
                إذا (!c)
                        أرجع log_oom_بھریا(log_level);

                *c = (ConfFile) {
                        .filename = strdup(de->d_name),
                        .نتیجہ = TAKE_PTR(p),
                        .original_رستہ = TAKE_PTR(original_رستہ),
                        .resolved_رستہ = TAKE_PTR(resolved_رستہ),
                        .fd = TAKE_FD(fd),
                        .st = st,
                };

                إذا (!c->filename)
                        أرجع log_oom_بھریا(log_level);

                r = hashmap_ensure_put(files, &conf_file_hash_ops, c->filename, c);
                إذا (r < 0) {
                        أكد(r == -ENOMEM);
                        أرجع log_oom_بھریا(log_level);
                }
                أكد(r > 0);

                TAKE_PTR(c);
        }

        أرجع 0;
}

ساكن صحيح dump_files(Hashmap *fh, ثابت حرف *جڑ, ConfFilesFlags flags, ConfFile ***ret_files, size_t *ret_n_files) {
        ConfFile **files = عدم;
        size_t n_files = 0;
        صحيح r;

        CLEANUP_ARRAY(files, n_files, conf_file_free_array);

        أكد(ret_files);
        أكد(ret_n_files);

        /* The entries in the array given by hashmap_dump_sorted() are still owned by the hashmap. */
        r = hashmap_dump_sorted(fh, (فراغ***) &files, &n_files);
        إذا (r < 0)
                أرجع log_oom_بھریا(conf_files_log_level(flags));

        /* Hence, we need to remove them from the hashmap. */
        FOREACH_ARRAY(i, files, n_files)
                assert_se(hashmap_remove(fh, (*i)->filename) == *i);

        إذا (جڑ)
                FOREACH_ARRAY(i, files, n_files) {
                        r = conf_file_اگیتر_جڑ(*i, جڑ, flags);
                        إذا (r < 0)
                                أرجع r;
                }

        *ret_files = TAKE_PTR(files);
        *ret_n_files = n_files;
        أرجع 0;
}

ساكن صحيح copy_and_ترتیب_دیو_files_from_hashmap(
                Hashmap *fh,
                ثابت حرف *suffix,
                ثابت حرف *جڑ,
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
                أرجع log_oom_بھریا(log_level);

        FOREACH_ARRAY(i, files, n_files) {
                ConfFile *c = *i;
                ثابت حرف *جوڑو = عدم;

                إذا (FLAGS_سیٹ(flags, CONF_FILES_BASENAME))
                        جوڑو = c->filename;
                وإلا إذا (جڑ && !FLAGS_سیٹ(flags, CONF_FILES_DONT_اگیتر_جڑ)) {
                        _cleanup_free_ حرف *p = عدم;

                        r = chaseat_اگیتر_جڑ(c->نتیجہ, جڑ, &p);
                        إذا (r < 0)
                                أرجع log_بھریا_errno(log_level, r, "Failed to prefix '%s' with root '%s': %m", c->نتیجہ, جڑ);

                        إذا (FLAGS_سیٹ(flags, CONF_FILES_TRUNCATE_SUFFIX) && suffix) {
                                حرف *e = endswith(p, suffix);
                                إذا (!e)
                                        استمر;

                                *e = 0;
                        }

                        إذا (strv_consume_with_ناپ(&results, &n_results, TAKE_PTR(p)) < 0)
                                أرجع log_oom_بھریا(log_level);

                        استمر;
                } وإلا
                        جوڑو = c->نتیجہ;

                إذا (FLAGS_سیٹ(flags, CONF_FILES_TRUNCATE_SUFFIX)) {
                        ثابت حرف *e = endswith(جوڑو, suffix);
                        إذا (!e)
                                استمر;

                        _cleanup_free_ حرف *n = strndup(جوڑو, e - جوڑو);
                        إذا (!n)
                                أرجع log_oom_بھریا(log_level);

                        r = strv_consume_with_ناپ(&results, &n_results, TAKE_PTR(n));
                } وإلا
                        r = strv_extend_with_ناپ(&results, &n_results, جوڑو);
                إذا (r < 0)
                        أرجع log_oom_بھریا(log_level);
        }

        *ret = TAKE_PTR(results);
        أرجع 0;
}

ساكن صحيح داخل_کرو_replacement(Hashmap **fh, ConfFile *replacement, ConfFilesFlags flags, ثابت ConfFile **ret) {
        _cleanup_(conf_file_freep) ConfFile *c = ASSERT_PTR(replacement);
        صحيح r;

        أكد(fh);
        أكد(ret);

        /* This consumes the input ConfFile. */

        ConfFile *existing = hashmap_get(*fh, c->filename);
        إذا (existing) {
                log_debug("An entry with higher priority '%s' -> '%s' already exists, ignoring the replacement: %s",
                          existing->filename, existing->نتیجہ, c->original_رستہ);
                *ret = عدم;
                أرجع 0;
        }

        r = hashmap_ensure_put(fh, &conf_file_hash_ops, c->filename, c);
        إذا (r < 0) {
                أكد(r == -ENOMEM);
                أرجع log_oom_بھریا(conf_files_log_level(flags));
        }
        أكد(r > 0);

        log_debug("Inserted replacement: '%s' -> '%s'", c->filename, c->نتیجہ);

        *ret = TAKE_PTR(c);
        أرجع 0;
}

ساكن صحيح conf_files_list_impl(
                ثابت حرف *suffix,
                ثابت حرف *جڑ, /* for logging, can be NULL */
                صحيح rfd,
                ConfFilesFlags flags,
                ثابت حرف * ثابت *dirs,
                ثابت حرف *replacement,
                Hashmap **ret,
                ثابت ConfFile **ret_inserted) {

        _cleanup_hashmap_free_ Hashmap *fh = عدم;
        cleanup_سیٹ_free سیٹ_2 *masked = عدم;
        _cleanup_(conf_file_freep) ConfFile *c = عدم;
        ثابت ConfFile *inserted = عدم;
        صحيح r;

        أكد(wildcard_fd_is_valid(rfd));
        أكد(ret);

        جڑ = خالی_تھاواں_منزل_جڑ(جڑ);

        إذا (replacement) {
                r = conf_file_new_at(replacement, جڑ, rfd, flags & CONF_FILES_WARN, &c);
                إذا (r < 0)
                        أرجع r;
        }

        STRV_FOREACH(p, dirs) {
                _cleanup_closedir_ DIR *dir = عدم;
                _cleanup_free_ حرف *رستہ = عدم;

                r = chase_and_opendirat(rfd, rfd, *p, 0, &رستہ, &dir);
                إذا (r < 0) {
                        إذا (r != -ENOENT)
                                log_بھریا_errno(conf_files_log_level(flags), r,
                                               "Failed to chase and open directory '%s%s', ignoring: %m",
                                               جڑ, skip_leading_slash(*p));
                        استمر;
                }

                إذا (c && streq_ptr(رستہ_startswith(c->نتیجہ, رستہ), c->filename)) {
                        r = داخل_کرو_replacement(&fh, TAKE_PTR(c), flags, &inserted);
                        إذا (r < 0)
                                أرجع r;
                }

                r = files_جوڑو(dir, *p, رستہ, جڑ, rfd, &fh, &masked, suffix, flags);
                إذا (r == -ENOMEM)
                        أرجع r;
        }

        إذا (c) {
                r = داخل_کرو_replacement(&fh, TAKE_PTR(c), flags, &inserted);
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
                ثابت حرف *جڑ,
                ConfFilesFlags flags,
                ثابت حرف * ثابت *dirs) {

        _cleanup_hashmap_free_ Hashmap *fh = عدم;
        _cleanup_close_ صحيح rfd = -EBADF;
        _cleanup_free_ حرف *جڑ_abs = عدم;
        _cleanup_strv_free_ حرف **dirs_abs = عدم;
        صحيح r;

        أكد(ret);

        r = prepare_dirs(جڑ, flags, (حرف**) dirs, &جڑ_abs, &rfd, &dirs_abs);
        إذا (r < 0)
                أرجع r;

        r = conf_files_list_impl(suffix, جڑ_abs, rfd, flags, (ثابت حرف * ثابت *) dirs_abs,
                                 /* replacement= */ عدم, &fh, /* ret_inserted= */ عدم);
        إذا (r < 0)
                أرجع r;

        أرجع copy_and_ترتیب_دیو_files_from_hashmap(fh, suffix, خالی_تھاواں_منزل_جڑ(جڑ_abs), flags, ret);
}

صحيح conf_files_list_strv_بھریا(
                ثابت حرف *suffix,
                ثابت حرف *جڑ,
                ConfFilesFlags flags,
                ثابت حرف * ثابت *dirs,
                ConfFile ***ret_files,
                size_t *ret_n_files) {

        _cleanup_hashmap_free_ Hashmap *fh = عدم;
        _cleanup_close_ صحيح rfd = -EBADF;
        _cleanup_free_ حرف *جڑ_abs = عدم;
        _cleanup_strv_free_ حرف **dirs_abs = عدم;
        صحيح r;

        أكد(ret_files);
        أكد(ret_n_files);

        r = prepare_dirs(جڑ, flags, (حرف**) dirs, &جڑ_abs, &rfd, &dirs_abs);
        إذا (r < 0)
                أرجع r;

        r = conf_files_list_impl(suffix, جڑ_abs, rfd, flags, (ثابت حرف * ثابت *) dirs_abs,
                                 /* replacement= */ عدم, &fh, /* ret_inserted= */ عدم);
        إذا (r < 0)
                أرجع r;

        أرجع dump_files(fh, خالی_تھاواں_منزل_جڑ(جڑ_abs), flags, ret_files, ret_n_files);
}

صحيح conf_files_list_strv_at(
                حرف ***ret,
                ثابت حرف *suffix,
                صحيح rfd,
                ConfFilesFlags flags,
                ثابت حرف * ثابت *dirs) {

        _cleanup_hashmap_free_ Hashmap *fh = عدم;
        _cleanup_free_ حرف *جڑ = عدم;
        صحيح r;

        أكد(wildcard_fd_is_valid(rfd));
        أكد(ret);

        إذا (DEBUG_LOGGING)
                (فراغ) fd_get_رستہ(rfd, &جڑ); /* for logging */

        r = conf_files_list_impl(suffix, جڑ, rfd, flags, dirs, /* replacement= */ عدم, &fh, /* ret_inserted= */ عدم);
        إذا (r < 0)
                أرجع r;

        أرجع copy_and_ترتیب_دیو_files_from_hashmap(fh, suffix, /* root= */ عدم, flags, ret);
}

صحيح conf_files_list_strv_at_بھریا(
                ثابت حرف *suffix,
                صحيح rfd,
                ConfFilesFlags flags,
                ثابت حرف * ثابت *dirs,
                ConfFile ***ret_files,
                size_t *ret_n_files) {

        _cleanup_hashmap_free_ Hashmap *fh = عدم;
        _cleanup_free_ حرف *جڑ = عدم;
        صحيح r;

        أكد(wildcard_fd_is_valid(rfd));
        أكد(ret_files);
        أكد(ret_n_files);

        إذا (DEBUG_LOGGING)
                (فراغ) fd_get_رستہ(rfd, &جڑ); /* for logging */

        r = conf_files_list_impl(suffix, جڑ, rfd, flags, dirs, /* replacement= */ عدم, &fh, /* ret_inserted= */ عدم);
        إذا (r < 0)
                أرجع r;

        أرجع dump_files(fh, /* root= */ عدم, flags, ret_files, ret_n_files);
}

صحيح conf_files_list(حرف ***ret, ثابت حرف *suffix, ثابت حرف *جڑ, ConfFilesFlags flags, ثابت حرف *dir) {
        أرجع conf_files_list_strv(ret, suffix, جڑ, flags, STRV_MAKE_CONST(dir));
}

صحيح conf_files_list_بھریا(ثابت حرف *suffix, ثابت حرف *جڑ, ConfFilesFlags flags, ثابت حرف *dir, ConfFile ***ret_files, size_t *ret_n_files) {
        أرجع conf_files_list_strv_بھریا(suffix, جڑ, flags, STRV_MAKE_CONST(dir), ret_files, ret_n_files);
}

صحيح conf_files_list_at(حرف ***ret, ثابت حرف *suffix, صحيح rfd, ConfFilesFlags flags, ثابت حرف *dir) {
        أرجع conf_files_list_strv_at(ret, suffix, rfd, flags, STRV_MAKE_CONST(dir));
}

صحيح conf_files_list_at_بھریا(ثابت حرف *suffix, صحيح rfd, ConfFilesFlags flags, ثابت حرف *dir, ConfFile ***ret_files, size_t *ret_n_files) {
        أرجع conf_files_list_strv_at_بھریا(suffix, rfd, flags, STRV_MAKE_CONST(dir), ret_files, ret_n_files);
}

صحيح conf_files_list_nulstr(حرف ***ret, ثابت حرف *suffix, ثابت حرف *جڑ, ConfFilesFlags flags, ثابت حرف *dirs) {
        _cleanup_strv_free_ حرف **d = عدم;

        أكد(ret);

        d = strv_ونڈو_nulstr(dirs);
        إذا (!d)
                أرجع log_oom_بھریا(conf_files_log_level(flags));

        أرجع conf_files_list_strv(ret, suffix, جڑ, flags, (ثابت حرف**) d);
}

صحيح conf_files_list_nulstr_بھریا(ثابت حرف *suffix, ثابت حرف *جڑ, ConfFilesFlags flags, ثابت حرف *dirs, ConfFile ***ret_files, size_t *ret_n_files) {
        _cleanup_strv_free_ حرف **d = عدم;

        أكد(ret_files);
        أكد(ret_n_files);

        d = strv_ونڈو_nulstr(dirs);
        إذا (!d)
                أرجع log_oom_بھریا(conf_files_log_level(flags));

        أرجع conf_files_list_strv_بھریا(suffix, جڑ, flags, (ثابت حرف**) d, ret_files, ret_n_files);
}

صحيح conf_files_list_nulstr_at(حرف ***ret, ثابت حرف *suffix, صحيح rfd, ConfFilesFlags flags, ثابت حرف *dirs) {
        _cleanup_strv_free_ حرف **d = عدم;

        أكد(ret);

        d = strv_ونڈو_nulstr(dirs);
        إذا (!d)
                أرجع log_oom_بھریا(conf_files_log_level(flags));

        أرجع conf_files_list_strv_at(ret, suffix, rfd, flags, (ثابت حرف**) d);
}

صحيح conf_files_list_nulstr_at_بھریا(ثابت حرف *suffix, صحيح rfd, ConfFilesFlags flags, ثابت حرف *dirs, ConfFile ***ret_files, size_t *ret_n_files) {
        _cleanup_strv_free_ حرف **d = عدم;

        أكد(ret_files);
        أكد(ret_n_files);

        d = strv_ونڈو_nulstr(dirs);
        إذا (!d)
                أرجع log_oom_بھریا(conf_files_log_level(flags));

        أرجع conf_files_list_strv_at_بھریا(suffix, rfd, flags, (ثابت حرف**) d, ret_files, ret_n_files);
}

صحيح conf_files_list_with_replacement(
                ثابت حرف *جڑ,
                حرف **config_dirs,
                ثابت حرف *replacement,
                حرف ***ret_files,
                حرف **ret_inserted) {

        _cleanup_hashmap_free_ Hashmap *fh = عدم;
        _cleanup_free_ حرف *inserted = عدم;
        ConfFilesFlags flags = CONF_FILES_REGULAR | CONF_FILES_FILTER_MASKED_BY_SYMLINK | CONF_FILES_WARN;
        _cleanup_close_ صحيح rfd = -EBADF;
        _cleanup_free_ حرف *جڑ_abs = عدم;
        _cleanup_strv_free_ حرف **dirs_abs = عدم;
        ثابت ConfFile *c = عدم;
        صحيح r;

        أكد(ret_files);

        r = prepare_dirs(جڑ, flags, config_dirs, &جڑ_abs, &rfd, &dirs_abs);
        إذا (r < 0)
                أرجع r;

        r = conf_files_list_impl(".conf", جڑ_abs, rfd, flags, (ثابت حرف * ثابت *) dirs_abs,
                                 replacement, &fh, ret_inserted ? &c : عدم);
        إذا (r < 0)
                أرجع r;

        إذا (c) {
                r = chaseat_اگیتر_جڑ(c->نتیجہ, جڑ_abs, &inserted);
                إذا (r < 0)
                        أرجع log_بھریا_errno(conf_files_log_level(flags), r,
                                              "Failed to prefix '%s' with root '%s': %m",
                                              c->نتیجہ, خالی_تھاواں_منزل_جڑ(جڑ_abs));
        }

        r = copy_and_ترتیب_دیو_files_from_hashmap(fh, ".conf", خالی_تھاواں_منزل_جڑ(جڑ_abs), flags, ret_files);
        إذا (r < 0)
                أرجع r;

        إذا (ret_inserted)
                *ret_inserted = TAKE_PTR(inserted);
        أرجع 0;
}

صحيح conf_files_list_dropins(
                حرف ***ret,
                ثابت حرف *dropin_dirname,
                ثابت حرف *جڑ,
                صحيح جڑ_fd,
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
                أرجع log_oom_بھریا(conf_files_log_level(flags));

        أرجع conf_files_list_strv_at(ret, ".conf", جڑ_fd, flags, (ثابت حرف* ثابت*) dropin_dirs);
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
                ثابت حرف *جڑ,
                ثابت حرف **config_dirs,
                ثابت حرف *fn,
                parse_سطر_t parse_سطر,
                فراغ *userdata,
                bool ignore_enoent,
                bool *invalid_config) {

        _cleanup_fclose_ FILE *_f = عدم;
        _cleanup_free_ حرف *_fn = عدم;
        بنانشانی v = 0;
        FILE *f;
        صحيح r = 0;

        أكد(fn);

        إذا (streq(fn, "-")) {
                f = stdin;
                fn = "<stdin>";

                log_debug("Reading config from stdin%s", glyph(GLYPH_ELLIPSIS));

        } وإلا إذا (is_رستہ(fn)) {
                r = رستہ_make_absolute_cwd(fn, &_fn);
                إذا (r < 0)
                        أرجع log_error_errno(r, "Failed to make path absolute: %m");
                fn = _fn;

                f = _f = fopen(fn, "re");
                إذا (!_f)
                        r = -errno;
                وإلا
                        log_debug("Reading config file \"%s\"%s", fn, glyph(GLYPH_ELLIPSIS));

        } وإلا {
                r = تلاش_and_fopen(fn, "re", جڑ, config_dirs, &_f, &_fn);
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

                إذا (وچ_سیٹ(سطر[0], 0, '#'))
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
