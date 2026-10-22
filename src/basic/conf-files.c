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
        حرر(c->نتيجو);
        حرر(c->original_رستو);
        حرر(c->resolved_رستو);
        محفوظ_close(c->fd);

        أرجع mfree(c);
}

DEFINE_اشارو_ARRAY_FREE_FUNC(ConfFile*, conf_file_free);

ساكن صحيح conf_files_log_level(ConfFilesFlags flags) {
        أرجع FLAGS_سيٽ(flags, CONF_FILES_WARN) ? LOG_WARNING : LOG_DEBUG;
}

ساكن صحيح prepare_dirs(
                ثابت حرف *پاڙ,
                ConfFilesFlags flags,
                حرف * ثابت *dirs,
                حرف **ret_پاڙ,
                صحيح *ret_rfd,
                حرف ***ret_dirs) {

        _cleanup_free_ حرف *پاڙ_abs = عدم;
        _cleanup_strv_free_ حرف **dirs_abs = عدم;
        صحيح r;

        أكد(ret_پاڙ);
        أكد(ret_rfd);
        أكد(ret_dirs || strv_isempty(dirs));

        صحيح log_level = conf_files_log_level(flags);

        r = خالي_جايون_or_پاڙ_harder_منزل_null(&پاڙ);
        إذا (r < 0)
                أرجع log_ڀريل_errno(log_level, r, "Failed to determine if '%s' points to the root directory: %m", strempty(پاڙ));

        إذا (ret_dirs) {
                dirs_abs = strv_copy(dirs);
                إذا (!dirs_abs)
                        أرجع log_oom_ڀريل(log_level);
        }

        _cleanup_close_ صحيح rfd = XAT_FDROOT;
        إذا (پاڙ) {
                /* When a non-trivial root is specified, we will prefix the result later. Hence, it is not
                 * necessary to modify each config directories here. but needs to normalize the root directory. */
                r = رستو_make_absolute_cwd(پاڙ, &پاڙ_abs);
                إذا (r < 0)
                        أرجع log_ڀريل_errno(log_level, r, "Failed to make '%s' absolute: %m", پاڙ);

                رستو_simplify(پاڙ_abs);

                rfd = open(پاڙ, O_CLOEXEC|O_DIRECTORY|O_رستو);
                إذا (rfd < 0)
                        أرجع log_ڀريل_errno(log_level, errno, "Failed to open '%s': %m", پاڙ_abs);

        } وإلا إذا (ret_dirs) {
                /* When an empty root or "/" is specified, we will open "/" below, hence we need to make
                 * each config directory absolute if relative. */
                r = رستو_strv_make_absolute_cwd(dirs_abs);
                إذا (r < 0)
                        أرجع log_ڀريل_errno(log_level, r, "Failed to make directories absolute: %m");
        }

        *ret_پاڙ = TAKE_PTR(پاڙ_abs);
        *ret_rfd = TAKE_FD(rfd);
        إذا (ret_dirs)
                *ret_dirs = TAKE_PTR(dirs_abs);
        أرجع 0;
}

ساكن صحيح conf_file_اڳياڙي_پاڙ(ConfFile *c, ثابت حرف *پاڙ, ConfFilesFlags flags) {
        حرف *p;
        صحيح r;

        أكد(c);

        إذا (FLAGS_سيٽ(flags, CONF_FILES_DONT_اڳياڙي_پاڙ))
                أرجع 0;

        صحيح log_level = conf_files_log_level(flags);

        r = chaseat_اڳياڙي_پاڙ(c->نتيجو, پاڙ, &p);
        إذا (r < 0)
                أرجع log_ڀريل_errno(log_level, r, "Failed to prefix '%s' with root '%s': %m", c->نتيجو, پاڙ);
        free_and_replace(c->نتيجو, p);

        r = chaseat_اڳياڙي_پاڙ(c->resolved_رستو, پاڙ, &p);
        إذا (r < 0)
                أرجع log_ڀريل_errno(log_level, r, "Failed to prefix '%s' with root '%s': %m", c->resolved_رستو, پاڙ);
        free_and_replace(c->resolved_رستو, p);

        /* Do not use chaseat_prefix_root(), as it is for the result of chaseat(), but the path is not chased. */
        p = رستو_join(خالي_جايون_منزل_پاڙ(پاڙ), skip_leading_slash(c->original_رستو));
        إذا (!p)
                أرجع log_oom_ڀريل(log_level);

        رستو_simplify(p);
        أرجع free_and_replace(c->original_رستو, p);
}

ساكن bool conf_files_need_stat(ConfFilesFlags flags) {
        أرجع (flags & (CONF_FILES_FILTER_MASKED | CONF_FILES_REGULAR | CONF_FILES_DIRECTORY | CONF_FILES_EXECUTABLE)) != 0;
}

ساكن ChaseFlags conf_files_chase_flags(ConfFilesFlags flags) {
        ChaseFlags chase_flags = 0;

        إذا (!conf_files_need_stat(flags) || FLAGS_سيٽ(flags, CONF_FILES_FILTER_MASKED_BY_SYMLINK))
                /* Even if no verification is requested, let's unconditionally call chaseat(),
                 * to drop unsafe symlinks. */
                chase_flags |= CHASE_NONEXISTENT;

        أرجع chase_flags;
}

ساكن صحيح conf_file_chase_and_verify(
                ثابت حرف *پاڙ,          /* for logging, can be NULL */
                صحيح rfd,
                ثابت حرف *original_رستو, /* for logging */
                ثابت حرف *رستو,
                ثابت حرف *name,
                سيٽ_2 **masked,              /* optional */
                ConfFilesFlags flags,
                حرف **ret_رستو,
                صحيح *ret_fd,
                بنية stat *ret_stat) {

        _cleanup_free_ حرف *resolved_رستو = عدم;
        _cleanup_close_ صحيح fd = -EBADF;
        بنية stat st = {};
        صحيح r;

        أكد(wildcard_fd_is_valid(rfd));
        أكد(original_رستو);
        أكد(رستو);
        أكد(name);

        صحيح log_level = conf_files_log_level(flags);

        پاڙ = خالي_جايون_منزل_پاڙ(پاڙ);

        r = chaseat(rfd, rfd, رستو, conf_files_chase_flags(flags), &resolved_رستو, &fd);
        إذا (r < 0)
                أرجع log_ڀريل_errno(log_level, r, "Failed to chase '%s%s': %m",
                                      پاڙ, skip_leading_slash(original_رستو));
        إذا (r == 0) {
                إذا (FLAGS_سيٽ(flags, CONF_FILES_FILTER_MASKED_BY_SYMLINK)) {
                        /* If the path points to /dev/null in a image or so, then the device node may not exist. */
                        إذا (رستو_equal(skip_leading_slash(resolved_رستو), "dev/null")) {
                                إذا (masked) {
                                        /* Mark this one as masked */
                                        r = سيٽ_put_strdup(masked, name);
                                        إذا (r < 0)
                                                أرجع log_oom_ڀريل(log_level);
                                }

                                أرجع log_debug_errno(SYNTHETIC_ERRNO(ERFKILL),
                                                       "File '%s%s' is a mask (symlink to /dev/null).",
                                                       پاڙ, skip_leading_slash(original_رستو));
                        }
                }

                إذا (conf_files_need_stat(flags))
                        /* If we need to have stat, skip the entry. */
                        أرجع log_ڀريل_errno(log_level, SYNTHETIC_ERRNO(ENOENT), "Failed to chase '%s%s': %m",
                                              پاڙ, skip_leading_slash(original_رستو));
        }

        /* Even if we do not need stat, let's take stat now. The caller may use the info later. */
        إذا (fd >= 0 && fstat(fd, &st) < 0)
                أرجع log_ڀريل_errno(log_level, errno, "Failed to stat '%s%s': %m",
                                      پاڙ, skip_leading_slash(original_رستو));

        /* Is this a masking entry? */
        إذا (FLAGS_سيٽ(flags, CONF_FILES_FILTER_MASKED_BY_SYMLINK) && stat_may_be_dev_null(&st)) {
                إذا (masked) {
                        /* Mark this one as masked */
                        r = سيٽ_put_strdup(masked, name);
                        إذا (r < 0)
                                أرجع log_oom_ڀريل(log_level);
                }

                أرجع log_debug_errno(SYNTHETIC_ERRNO(ERFKILL),
                                       "File '%s%s' is a mask (symlink to /dev/null).",
                                       پاڙ, skip_leading_slash(original_رستو));
        }

        إذا (FLAGS_سيٽ(flags, CONF_FILES_FILTER_MASKED_BY_خالي_جايون) && stat_is_خالي_جايون(&st)) {
                إذا (masked) {
                        /* Mark this one as masked */
                        r = سيٽ_put_strdup(masked, name);
                        إذا (r < 0)
                                أرجع log_oom_ڀريل(log_level);
                }

                أرجع log_debug_errno(SYNTHETIC_ERRNO(ERFKILL),
                                       "File '%s%s' is a mask (an empty file).",
                                       پاڙ, skip_leading_slash(original_رستو));
        }

        إذا (FLAGS_سيٽ(flags, CONF_FILES_REGULAR|CONF_FILES_DIRECTORY)) {
                إذا (!S_ISREG(st.st_mode) && !S_ISDIR(st.st_mode))
                        أرجع log_debug_errno(SYNTHETIC_ERRNO(EBADFD),
                                               "File '%s%s' is neither a regular file or directory.",
                                               پاڙ, skip_leading_slash(original_رستو));
        } وإلا {
                /* Is this node a regular file? */
                إذا (FLAGS_سيٽ(flags, CONF_FILES_REGULAR)) {
                        r = stat_verify_regular(&st);
                        إذا (r < 0)
                                أرجع log_debug_errno(r, "File '%s%s' is not a regular file: %m",
                                                       پاڙ, skip_leading_slash(original_رستو));
                }

                /* Is this node a directory? */
                إذا (FLAGS_سيٽ(flags, CONF_FILES_DIRECTORY)) {
                        r = stat_verify_directory(&st);
                        إذا (r < 0)
                                أرجع log_debug_errno(r, "File '%s%s' is not a directory: %m",
                                                       پاڙ, skip_leading_slash(original_رستو));
                }
        }

        /* Does this node have the executable bit set?
         * As requested: check if the file is marked executable. Note that we don't check access(X_OK) here,
         * as we care about whether the file is marked executable at all, and not whether it is executable
         * for us, because if so, such errors are stuff we should log about. */
        إذا (FLAGS_سيٽ(flags, CONF_FILES_EXECUTABLE) && (st.st_mode & 0111) == 0)
                أرجع log_debug_errno(SYNTHETIC_ERRNO(ENOEXEC),
                                       "File '%s%s' is not marked executable.",
                                       پاڙ, skip_leading_slash(original_رستو));

        إذا (ret_رستو)
                *ret_رستو = TAKE_PTR(resolved_رستو);
        إذا (ret_fd)
                *ret_fd = TAKE_FD(fd);
        إذا (ret_stat)
                *ret_stat = st;

        أرجع 0;
}

صحيح conf_file_new_at(
                ثابت حرف *رستو,
                ثابت حرف *پاڙ,
                صحيح rfd,
                ConfFilesFlags flags,
                ConfFile **ret) {
        صحيح r;

        أكد(رستو);
        أكد(wildcard_fd_is_valid(rfd));
        أكد(ret);

        صحيح log_level = conf_files_log_level(flags);

        _cleanup_free_ حرف *پاڙ_2 = عدم;
        إذا (DEBUG_LOGGING && !پاڙ) {
                (فراغ) fd_get_رستو(rfd, &پاڙ_2);
                پاڙ = پاڙ_2;
        }

        _cleanup_(conf_file_freep) ConfFile *c = new(ConfFile, 1);
        إذا (!c)
                أرجع log_oom_ڀريل(log_level);

        *c = (ConfFile) {
                .original_رستو = strdup(رستو),
                .fd = -EBADF,
        };

        إذا (!c->original_رستو)
                أرجع log_oom_ڀريل(log_level);

        r = رستو_ڪڍو_filename(رستو, &c->filename);
        إذا (r < 0)
                أرجع log_ڀريل_errno(log_level, r, "Failed to extract filename from '%s': %m", رستو);

        _cleanup_free_ حرف *dirpath = عدم, *resolved_dirpath = عدم;
        r = رستو_ڪڍو_directory(رستو, &dirpath);
        إذا (r < 0 && r != -EDESTADDRREQ)
                أرجع log_ڀريل_errno(log_level, r, "Failed to extract directory from '%s': %m", رستو);
        إذا (r >= 0) {
                r = chaseat(rfd, rfd, dirpath,
                            CHASE_MUST_BE_DIRECTORY | conf_files_chase_flags(flags),
                            &resolved_dirpath, /* ret_fd= */ عدم);
                إذا (r < 0)
                        أرجع log_ڀريل_errno(log_level, r, "Failed to chase '%s%s': %m", خالي_جايون_منزل_پاڙ(پاڙ), skip_leading_slash(dirpath));
        }

        c->نتيجو = رستو_join(resolved_dirpath, c->filename);
        إذا (!c->نتيجو)
                أرجع log_oom_ڀريل(log_level);

        r = conf_file_chase_and_verify(
                        پاڙ,
                        rfd,
                        c->original_رستو,
                        c->نتيجو,
                        c->filename,
                        /* masked= */ عدم,
                        flags,
                        &c->resolved_رستو,
                        &c->fd,
                        &c->st);
        إذا (r < 0)
                أرجع r;

        *ret = TAKE_PTR(c);
        أرجع 0;
}

صحيح conf_file_new(ثابت حرف *رستو, ثابت حرف *پاڙ, ConfFilesFlags flags, ConfFile **ret) {
        صحيح r;

        أكد(رستو);
        أكد(ret);

        _cleanup_free_ حرف *پاڙ_abs = عدم;
        _cleanup_close_ صحيح rfd = -EBADF;
        r = prepare_dirs(پاڙ, flags, /* dirs= */ عدم, &پاڙ_abs, &rfd, /* ret_dirs= */ عدم);
        إذا (r < 0)
                أرجع r;

        _cleanup_free_ حرف *رستو_abs = عدم;
        إذا (!پاڙ_abs) {
                r = رستو_make_absolute_cwd(رستو, &رستو_abs);
                إذا (r < 0)
                        أرجع log_ڀريل_errno(conf_files_log_level(flags), r, "Failed to make '%s' absolute: %m", رستو);

                رستو = رستو_abs;
        }

        _cleanup_(conf_file_freep) ConfFile *c = عدم;
        r = conf_file_new_at(رستو, پاڙ_abs, rfd, flags, &c);
        إذا (r < 0)
                أرجع r;

        r = conf_file_اڳياڙي_پاڙ(c, پاڙ_abs, flags);
        إذا (r < 0)
                أرجع r;

        *ret = TAKE_PTR(c);
        أرجع 0;
}

DEFINE_ذاتي_HASH_OPS_WITH_قدر_DESTRUCTOR(
                conf_file_hash_ops,
                حرف, string_hash_func, string_compare_func,
                ConfFile, conf_file_free);

ساكن صحيح files_جوڙيو(
                DIR *dir,
                ثابت حرف *original_dirpath,
                ثابت حرف *resolved_dirpath,
                ثابت حرف *پاڙ, /* for logging, can be NULL */
                صحيح rfd,
                Hashmap **files,
                سيٽ_2 **masked,
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

        پاڙ = خالي_جايون_منزل_پاڙ(پاڙ);

        FOREACH_DIRENT(de, dir, أرجع log_ڀريل_errno(log_level, errno, "Failed to read directory '%s%s': %m",
                                                      پاڙ, skip_leading_slash(original_dirpath))) {

                _cleanup_free_ حرف *original_رستو = رستو_join(original_dirpath, de->d_name);
                إذا (!original_رستو)
                        أرجع log_oom_ڀريل(log_level);

                /* Does this match the suffix? */
                إذا (suffix && !endswith(de->d_name, suffix)) {
                        log_debug("Skipping file '%s%s', suffix is not '%s'.", پاڙ, skip_leading_slash(original_رستو), suffix);
                        استمر;
                }

                /* Has this file already been found in an earlier directory? */
                إذا (hashmap_شامل_آهي(*files, de->d_name)) {
                        log_debug("Skipping overridden file '%s%s'.", پاڙ, skip_leading_slash(original_رستو));
                        استمر;
                }

                /* Has this been masked in an earlier directory? */
                إذا ((flags & CONF_FILES_FILTER_MASKED) != 0 && سيٽ_شامل_آهي(*masked, de->d_name)) {
                        log_debug("File '%s%s' is masked by previous entry.", پاڙ, skip_leading_slash(original_رستو));
                        استمر;
                }

                _cleanup_free_ حرف *p = رستو_join(resolved_dirpath, de->d_name);
                إذا (!p)
                        أرجع log_oom_ڀريل(log_level);

                _cleanup_free_ حرف *resolved_رستو = عدم;
                _cleanup_close_ صحيح fd = -EBADF;
                بنية stat st;
                r = conf_file_chase_and_verify(
                                پاڙ,
                                rfd,
                                original_رستو,
                                p,
                                de->d_name,
                                masked,
                                flags,
                                &resolved_رستو,
                                &fd,
                                &st);
                إذا (r == -ENOMEM)
                        أرجع r;
                إذا (r < 0)
                        استمر;

                _cleanup_(conf_file_freep) ConfFile *c = new(ConfFile, 1);
                إذا (!c)
                        أرجع log_oom_ڀريل(log_level);

                *c = (ConfFile) {
                        .filename = strdup(de->d_name),
                        .نتيجو = TAKE_PTR(p),
                        .original_رستو = TAKE_PTR(original_رستو),
                        .resolved_رستو = TAKE_PTR(resolved_رستو),
                        .fd = TAKE_FD(fd),
                        .st = st,
                };

                إذا (!c->filename)
                        أرجع log_oom_ڀريل(log_level);

                r = hashmap_ensure_put(files, &conf_file_hash_ops, c->filename, c);
                إذا (r < 0) {
                        أكد(r == -ENOMEM);
                        أرجع log_oom_ڀريل(log_level);
                }
                أكد(r > 0);

                TAKE_PTR(c);
        }

        أرجع 0;
}

ساكن صحيح dump_files(Hashmap *fh, ثابت حرف *پاڙ, ConfFilesFlags flags, ConfFile ***ret_files, size_t *ret_n_files) {
        ConfFile **files = عدم;
        size_t n_files = 0;
        صحيح r;

        CLEANUP_ARRAY(files, n_files, conf_file_free_array);

        أكد(ret_files);
        أكد(ret_n_files);

        /* The entries in the array given by hashmap_dump_sorted() are still owned by the hashmap. */
        r = hashmap_dump_sorted(fh, (فراغ***) &files, &n_files);
        إذا (r < 0)
                أرجع log_oom_ڀريل(conf_files_log_level(flags));

        /* Hence, we need to remove them from the hashmap. */
        FOREACH_ARRAY(i, files, n_files)
                assert_se(hashmap_remove(fh, (*i)->filename) == *i);

        إذا (پاڙ)
                FOREACH_ARRAY(i, files, n_files) {
                        r = conf_file_اڳياڙي_پاڙ(*i, پاڙ, flags);
                        إذا (r < 0)
                                أرجع r;
                }

        *ret_files = TAKE_PTR(files);
        *ret_n_files = n_files;
        أرجع 0;
}

ساكن صحيح copy_and_ترتيب_ڏيو_files_from_hashmap(
                Hashmap *fh,
                ثابت حرف *suffix,
                ثابت حرف *پاڙ,
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
                أرجع log_oom_ڀريل(log_level);

        FOREACH_ARRAY(i, files, n_files) {
                ConfFile *c = *i;
                ثابت حرف *جوڙيو = عدم;

                إذا (FLAGS_سيٽ(flags, CONF_FILES_BASENAME))
                        جوڙيو = c->filename;
                وإلا إذا (پاڙ && !FLAGS_سيٽ(flags, CONF_FILES_DONT_اڳياڙي_پاڙ)) {
                        _cleanup_free_ حرف *p = عدم;

                        r = chaseat_اڳياڙي_پاڙ(c->نتيجو, پاڙ, &p);
                        إذا (r < 0)
                                أرجع log_ڀريل_errno(log_level, r, "Failed to prefix '%s' with root '%s': %m", c->نتيجو, پاڙ);

                        إذا (FLAGS_سيٽ(flags, CONF_FILES_TRUNCATE_SUFFIX) && suffix) {
                                حرف *e = endswith(p, suffix);
                                إذا (!e)
                                        استمر;

                                *e = 0;
                        }

                        إذا (strv_consume_with_سائيز(&results, &n_results, TAKE_PTR(p)) < 0)
                                أرجع log_oom_ڀريل(log_level);

                        استمر;
                } وإلا
                        جوڙيو = c->نتيجو;

                إذا (FLAGS_سيٽ(flags, CONF_FILES_TRUNCATE_SUFFIX)) {
                        ثابت حرف *e = endswith(جوڙيو, suffix);
                        إذا (!e)
                                استمر;

                        _cleanup_free_ حرف *n = strndup(جوڙيو, e - جوڙيو);
                        إذا (!n)
                                أرجع log_oom_ڀريل(log_level);

                        r = strv_consume_with_سائيز(&results, &n_results, TAKE_PTR(n));
                } وإلا
                        r = strv_extend_with_سائيز(&results, &n_results, جوڙيو);
                إذا (r < 0)
                        أرجع log_oom_ڀريل(log_level);
        }

        *ret = TAKE_PTR(results);
        أرجع 0;
}

ساكن صحيح وجهو_replacement(Hashmap **fh, ConfFile *replacement, ConfFilesFlags flags, ثابت ConfFile **ret) {
        _cleanup_(conf_file_freep) ConfFile *c = ASSERT_PTR(replacement);
        صحيح r;

        أكد(fh);
        أكد(ret);

        /* This consumes the input ConfFile. */

        ConfFile *existing = hashmap_get(*fh, c->filename);
        إذا (existing) {
                log_debug("An entry with higher priority '%s' -> '%s' already exists, ignoring the replacement: %s",
                          existing->filename, existing->نتيجو, c->original_رستو);
                *ret = عدم;
                أرجع 0;
        }

        r = hashmap_ensure_put(fh, &conf_file_hash_ops, c->filename, c);
        إذا (r < 0) {
                أكد(r == -ENOMEM);
                أرجع log_oom_ڀريل(conf_files_log_level(flags));
        }
        أكد(r > 0);

        log_debug("Inserted replacement: '%s' -> '%s'", c->filename, c->نتيجو);

        *ret = TAKE_PTR(c);
        أرجع 0;
}

ساكن صحيح conf_files_list_impl(
                ثابت حرف *suffix,
                ثابت حرف *پاڙ, /* for logging, can be NULL */
                صحيح rfd,
                ConfFilesFlags flags,
                ثابت حرف * ثابت *dirs,
                ثابت حرف *replacement,
                Hashmap **ret,
                ثابت ConfFile **ret_inserted) {

        _cleanup_hashmap_free_ Hashmap *fh = عدم;
        cleanup_سيٽ_free سيٽ_2 *masked = عدم;
        _cleanup_(conf_file_freep) ConfFile *c = عدم;
        ثابت ConfFile *inserted = عدم;
        صحيح r;

        أكد(wildcard_fd_is_valid(rfd));
        أكد(ret);

        پاڙ = خالي_جايون_منزل_پاڙ(پاڙ);

        إذا (replacement) {
                r = conf_file_new_at(replacement, پاڙ, rfd, flags & CONF_FILES_WARN, &c);
                إذا (r < 0)
                        أرجع r;
        }

        STRV_FOREACH(p, dirs) {
                _cleanup_closedir_ DIR *dir = عدم;
                _cleanup_free_ حرف *رستو = عدم;

                r = chase_and_opendirat(rfd, rfd, *p, 0, &رستو, &dir);
                إذا (r < 0) {
                        إذا (r != -ENOENT)
                                log_ڀريل_errno(conf_files_log_level(flags), r,
                                               "Failed to chase and open directory '%s%s', ignoring: %m",
                                               پاڙ, skip_leading_slash(*p));
                        استمر;
                }

                إذا (c && streq_ptr(رستو_startswith(c->نتيجو, رستو), c->filename)) {
                        r = وجهو_replacement(&fh, TAKE_PTR(c), flags, &inserted);
                        إذا (r < 0)
                                أرجع r;
                }

                r = files_جوڙيو(dir, *p, رستو, پاڙ, rfd, &fh, &masked, suffix, flags);
                إذا (r == -ENOMEM)
                        أرجع r;
        }

        إذا (c) {
                r = وجهو_replacement(&fh, TAKE_PTR(c), flags, &inserted);
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
                ثابت حرف *پاڙ,
                ConfFilesFlags flags,
                ثابت حرف * ثابت *dirs) {

        _cleanup_hashmap_free_ Hashmap *fh = عدم;
        _cleanup_close_ صحيح rfd = -EBADF;
        _cleanup_free_ حرف *پاڙ_abs = عدم;
        _cleanup_strv_free_ حرف **dirs_abs = عدم;
        صحيح r;

        أكد(ret);

        r = prepare_dirs(پاڙ, flags, (حرف**) dirs, &پاڙ_abs, &rfd, &dirs_abs);
        إذا (r < 0)
                أرجع r;

        r = conf_files_list_impl(suffix, پاڙ_abs, rfd, flags, (ثابت حرف * ثابت *) dirs_abs,
                                 /* replacement= */ عدم, &fh, /* ret_inserted= */ عدم);
        إذا (r < 0)
                أرجع r;

        أرجع copy_and_ترتيب_ڏيو_files_from_hashmap(fh, suffix, خالي_جايون_منزل_پاڙ(پاڙ_abs), flags, ret);
}

صحيح conf_files_list_strv_ڀريل(
                ثابت حرف *suffix,
                ثابت حرف *پاڙ,
                ConfFilesFlags flags,
                ثابت حرف * ثابت *dirs,
                ConfFile ***ret_files,
                size_t *ret_n_files) {

        _cleanup_hashmap_free_ Hashmap *fh = عدم;
        _cleanup_close_ صحيح rfd = -EBADF;
        _cleanup_free_ حرف *پاڙ_abs = عدم;
        _cleanup_strv_free_ حرف **dirs_abs = عدم;
        صحيح r;

        أكد(ret_files);
        أكد(ret_n_files);

        r = prepare_dirs(پاڙ, flags, (حرف**) dirs, &پاڙ_abs, &rfd, &dirs_abs);
        إذا (r < 0)
                أرجع r;

        r = conf_files_list_impl(suffix, پاڙ_abs, rfd, flags, (ثابت حرف * ثابت *) dirs_abs,
                                 /* replacement= */ عدم, &fh, /* ret_inserted= */ عدم);
        إذا (r < 0)
                أرجع r;

        أرجع dump_files(fh, خالي_جايون_منزل_پاڙ(پاڙ_abs), flags, ret_files, ret_n_files);
}

صحيح conf_files_list_strv_at(
                حرف ***ret,
                ثابت حرف *suffix,
                صحيح rfd,
                ConfFilesFlags flags,
                ثابت حرف * ثابت *dirs) {

        _cleanup_hashmap_free_ Hashmap *fh = عدم;
        _cleanup_free_ حرف *پاڙ = عدم;
        صحيح r;

        أكد(wildcard_fd_is_valid(rfd));
        أكد(ret);

        إذا (DEBUG_LOGGING)
                (فراغ) fd_get_رستو(rfd, &پاڙ); /* for logging */

        r = conf_files_list_impl(suffix, پاڙ, rfd, flags, dirs, /* replacement= */ عدم, &fh, /* ret_inserted= */ عدم);
        إذا (r < 0)
                أرجع r;

        أرجع copy_and_ترتيب_ڏيو_files_from_hashmap(fh, suffix, /* root= */ عدم, flags, ret);
}

صحيح conf_files_list_strv_at_ڀريل(
                ثابت حرف *suffix,
                صحيح rfd,
                ConfFilesFlags flags,
                ثابت حرف * ثابت *dirs,
                ConfFile ***ret_files,
                size_t *ret_n_files) {

        _cleanup_hashmap_free_ Hashmap *fh = عدم;
        _cleanup_free_ حرف *پاڙ = عدم;
        صحيح r;

        أكد(wildcard_fd_is_valid(rfd));
        أكد(ret_files);
        أكد(ret_n_files);

        إذا (DEBUG_LOGGING)
                (فراغ) fd_get_رستو(rfd, &پاڙ); /* for logging */

        r = conf_files_list_impl(suffix, پاڙ, rfd, flags, dirs, /* replacement= */ عدم, &fh, /* ret_inserted= */ عدم);
        إذا (r < 0)
                أرجع r;

        أرجع dump_files(fh, /* root= */ عدم, flags, ret_files, ret_n_files);
}

صحيح conf_files_list(حرف ***ret, ثابت حرف *suffix, ثابت حرف *پاڙ, ConfFilesFlags flags, ثابت حرف *dir) {
        أرجع conf_files_list_strv(ret, suffix, پاڙ, flags, STRV_MAKE_CONST(dir));
}

صحيح conf_files_list_ڀريل(ثابت حرف *suffix, ثابت حرف *پاڙ, ConfFilesFlags flags, ثابت حرف *dir, ConfFile ***ret_files, size_t *ret_n_files) {
        أرجع conf_files_list_strv_ڀريل(suffix, پاڙ, flags, STRV_MAKE_CONST(dir), ret_files, ret_n_files);
}

صحيح conf_files_list_at(حرف ***ret, ثابت حرف *suffix, صحيح rfd, ConfFilesFlags flags, ثابت حرف *dir) {
        أرجع conf_files_list_strv_at(ret, suffix, rfd, flags, STRV_MAKE_CONST(dir));
}

صحيح conf_files_list_at_ڀريل(ثابت حرف *suffix, صحيح rfd, ConfFilesFlags flags, ثابت حرف *dir, ConfFile ***ret_files, size_t *ret_n_files) {
        أرجع conf_files_list_strv_at_ڀريل(suffix, rfd, flags, STRV_MAKE_CONST(dir), ret_files, ret_n_files);
}

صحيح conf_files_list_nulstr(حرف ***ret, ثابت حرف *suffix, ثابت حرف *پاڙ, ConfFilesFlags flags, ثابت حرف *dirs) {
        _cleanup_strv_free_ حرف **d = عدم;

        أكد(ret);

        d = strv_ورهايو_nulstr(dirs);
        إذا (!d)
                أرجع log_oom_ڀريل(conf_files_log_level(flags));

        أرجع conf_files_list_strv(ret, suffix, پاڙ, flags, (ثابت حرف**) d);
}

صحيح conf_files_list_nulstr_ڀريل(ثابت حرف *suffix, ثابت حرف *پاڙ, ConfFilesFlags flags, ثابت حرف *dirs, ConfFile ***ret_files, size_t *ret_n_files) {
        _cleanup_strv_free_ حرف **d = عدم;

        أكد(ret_files);
        أكد(ret_n_files);

        d = strv_ورهايو_nulstr(dirs);
        إذا (!d)
                أرجع log_oom_ڀريل(conf_files_log_level(flags));

        أرجع conf_files_list_strv_ڀريل(suffix, پاڙ, flags, (ثابت حرف**) d, ret_files, ret_n_files);
}

صحيح conf_files_list_nulstr_at(حرف ***ret, ثابت حرف *suffix, صحيح rfd, ConfFilesFlags flags, ثابت حرف *dirs) {
        _cleanup_strv_free_ حرف **d = عدم;

        أكد(ret);

        d = strv_ورهايو_nulstr(dirs);
        إذا (!d)
                أرجع log_oom_ڀريل(conf_files_log_level(flags));

        أرجع conf_files_list_strv_at(ret, suffix, rfd, flags, (ثابت حرف**) d);
}

صحيح conf_files_list_nulstr_at_ڀريل(ثابت حرف *suffix, صحيح rfd, ConfFilesFlags flags, ثابت حرف *dirs, ConfFile ***ret_files, size_t *ret_n_files) {
        _cleanup_strv_free_ حرف **d = عدم;

        أكد(ret_files);
        أكد(ret_n_files);

        d = strv_ورهايو_nulstr(dirs);
        إذا (!d)
                أرجع log_oom_ڀريل(conf_files_log_level(flags));

        أرجع conf_files_list_strv_at_ڀريل(suffix, rfd, flags, (ثابت حرف**) d, ret_files, ret_n_files);
}

صحيح conf_files_list_with_replacement(
                ثابت حرف *پاڙ,
                حرف **config_dirs,
                ثابت حرف *replacement,
                حرف ***ret_files,
                حرف **ret_inserted) {

        _cleanup_hashmap_free_ Hashmap *fh = عدم;
        _cleanup_free_ حرف *inserted = عدم;
        ConfFilesFlags flags = CONF_FILES_REGULAR | CONF_FILES_FILTER_MASKED_BY_SYMLINK | CONF_FILES_WARN;
        _cleanup_close_ صحيح rfd = -EBADF;
        _cleanup_free_ حرف *پاڙ_abs = عدم;
        _cleanup_strv_free_ حرف **dirs_abs = عدم;
        ثابت ConfFile *c = عدم;
        صحيح r;

        أكد(ret_files);

        r = prepare_dirs(پاڙ, flags, config_dirs, &پاڙ_abs, &rfd, &dirs_abs);
        إذا (r < 0)
                أرجع r;

        r = conf_files_list_impl(".conf", پاڙ_abs, rfd, flags, (ثابت حرف * ثابت *) dirs_abs,
                                 replacement, &fh, ret_inserted ? &c : عدم);
        إذا (r < 0)
                أرجع r;

        إذا (c) {
                r = chaseat_اڳياڙي_پاڙ(c->نتيجو, پاڙ_abs, &inserted);
                إذا (r < 0)
                        أرجع log_ڀريل_errno(conf_files_log_level(flags), r,
                                              "Failed to prefix '%s' with root '%s': %m",
                                              c->نتيجو, خالي_جايون_منزل_پاڙ(پاڙ_abs));
        }

        r = copy_and_ترتيب_ڏيو_files_from_hashmap(fh, ".conf", خالي_جايون_منزل_پاڙ(پاڙ_abs), flags, ret_files);
        إذا (r < 0)
                أرجع r;

        إذا (ret_inserted)
                *ret_inserted = TAKE_PTR(inserted);
        أرجع 0;
}

صحيح conf_files_list_dropins(
                حرف ***ret,
                ثابت حرف *dropin_dirname,
                ثابت حرف *پاڙ,
                صحيح پاڙ_fd,
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
                أرجع log_oom_ڀريل(conf_files_log_level(flags));

        أرجع conf_files_list_strv_at(ret, ".conf", پاڙ_fd, flags, (ثابت حرف* ثابت*) dropin_dirs);
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
                ثابت حرف *پاڙ,
                ثابت حرف **config_dirs,
                ثابت حرف *fn,
                parse_سٽ_t parse_سٽ,
                فراغ *userdata,
                bool ignore_enoent,
                bool *invalid_config) {

        _cleanup_fclose_ FILE *_f = عدم;
        _cleanup_free_ حرف *_fn = عدم;
        بنانشان v = 0;
        FILE *f;
        صحيح r = 0;

        أكد(fn);

        إذا (streq(fn, "-")) {
                f = stdin;
                fn = "<stdin>";

                log_debug("Reading config from stdin%s", glyph(GLYPH_ELLIPSIS));

        } وإلا إذا (is_رستو(fn)) {
                r = رستو_make_absolute_cwd(fn, &_fn);
                إذا (r < 0)
                        أرجع log_error_errno(r, "Failed to make path absolute: %m");
                fn = _fn;

                f = _f = fopen(fn, "re");
                إذا (!_f)
                        r = -errno;
                وإلا
                        log_debug("Reading config file \"%s\"%s", fn, glyph(GLYPH_ELLIPSIS));

        } وإلا {
                r = ڳولا_and_fopen(fn, "re", پاڙ, config_dirs, &_f, &_fn);
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
                _cleanup_free_ حرف *سٽ = عدم;
                bool invalid_سٽ = false;
                صحيح k;

                k = read_stripped_سٽ(f, LONG_سٽ_MAX, &سٽ);
                إذا (k < 0)
                        أرجع log_error_errno(k, "Failed to read '%s': %m", fn);
                إذا (k == 0)
                        اكسر;

                v++;

                إذا (اندريون_سيٽ(سٽ[0], 0, '#'))
                        استمر;

                k = parse_سٽ(fn, v, سٽ, invalid_config ? &invalid_سٽ : عدم, userdata);
                إذا (k < 0 && invalid_سٽ)
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
