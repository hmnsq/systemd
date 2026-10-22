/* SPDX-License-Identifier: LGPL-2.1-or-later */

#અન્તર્ભાવ <stdio.h>
#અન્તર્ભાવ <stdlib.h>

#અન્તર્ભાવ "alloc-util.h"
#અન્તર્ભાવ "chase.h"
#અન્તર્ભાવ "conf-files.h"
#અન્તર્ભાવ "dirent-util.h"
#અન્તર્ભાવ "errno-util.h"
#અન્તર્ભાવ "fd-util.h"
#અન્તર્ભાવ "fileio.h"
#અન્તર્ભાવ "glyph-util.h"
#અન્તર્ભાવ "hashmap.h"
#અન્તર્ભાવ "log.h"
#અન્તર્ભાવ "nulstr-util.h"
#અન્તર્ભાવ "path-util.h"
#અન્તર્ભાવ "set.h"
#અન્તર્ભાવ "stat-util.h"
#અન્તર્ભાવ "string-util.h"
#અન્તર્ભાવ "strv.h"

ConfFile* conf_file_free(ConfFile *c) {
        જો (!c)
                પાછા ખાલી;

        મુક્ત(c->filename);
        મુક્ત(c->result);
        મુક્ત(c->original_path);
        મુક્ત(c->resolved_path);
        safe_close(c->fd);

        પાછા mfree(c);
}

DEFINE_POINTER_ARRAY_FREE_FUNC(ConfFile*, conf_file_free);

સ્થિર પૂર્ણાંક conf_files_log_level(ConfFilesFlags flags) {
        પાછા FLAGS_SET(flags, CONF_FILES_WARN) ? LOG_WARNING : LOG_DEBUG;
}

સ્થિર પૂર્ણાંક prepare_dirs(
                અચલ અક્ષર *મૂળ,
                ConfFilesFlags flags,
                અક્ષર * અચલ *dirs,
                અક્ષર **ret_મૂળ,
                પૂર્ણાંક *ret_rfd,
                અક્ષર ***ret_dirs) {

        _cleanup_free_ અક્ષર *મૂળ_abs = ખાલી;
        _cleanup_strv_free_ અક્ષર **dirs_abs = ખાલી;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(ret_મૂળ);
        પ્રતિજ્ઞા(ret_rfd);
        પ્રતિજ્ઞા(ret_dirs || strv_isempty(dirs));

        પૂર્ણાંક log_level = conf_files_log_level(flags);

        r = empty_or_મૂળ_harder_to_null(&મૂળ);
        જો (r < 0)
                પાછા log_full_errno(log_level, r, "Failed to determine if '%s' points to the root directory: %m", strempty(મૂળ));

        જો (ret_dirs) {
                dirs_abs = strv_copy(dirs);
                જો (!dirs_abs)
                        પાછા log_oom_full(log_level);
        }

        _cleanup_close_ પૂર્ણાંક rfd = XAT_FDROOT;
        જો (મૂળ) {
                /* When a non-trivial root is specified, we will prefix the result later. Hence, it is not
                 * necessary to modify each config directories here. but needs to normalize the root directory. */
                r = path_make_absolute_cwd(મૂળ, &મૂળ_abs);
                જો (r < 0)
                        પાછા log_full_errno(log_level, r, "Failed to make '%s' absolute: %m", મૂળ);

                path_simplify(મૂળ_abs);

                rfd = open(મૂળ, O_CLOEXEC|O_DIRECTORY|O_PATH);
                જો (rfd < 0)
                        પાછા log_full_errno(log_level, errno, "Failed to open '%s': %m", મૂળ_abs);

        } નહીંતર જો (ret_dirs) {
                /* When an empty root or "/" is specified, we will open "/" below, hence we need to make
                 * each config directory absolute if relative. */
                r = path_strv_make_absolute_cwd(dirs_abs);
                જો (r < 0)
                        પાછા log_full_errno(log_level, r, "Failed to make directories absolute: %m");
        }

        *ret_મૂળ = TAKE_PTR(મૂળ_abs);
        *ret_rfd = TAKE_FD(rfd);
        જો (ret_dirs)
                *ret_dirs = TAKE_PTR(dirs_abs);
        પાછા 0;
}

સ્થિર પૂર્ણાંક conf_file_prefix_મૂળ(ConfFile *c, અચલ અક્ષર *મૂળ, ConfFilesFlags flags) {
        અક્ષર *p;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(c);

        જો (FLAGS_SET(flags, CONF_FILES_DONT_PREFIX_મૂળ))
                પાછા 0;

        પૂર્ણાંક log_level = conf_files_log_level(flags);

        r = chaseat_prefix_મૂળ(c->result, મૂળ, &p);
        જો (r < 0)
                પાછા log_full_errno(log_level, r, "Failed to prefix '%s' with root '%s': %m", c->result, મૂળ);
        free_and_replace(c->result, p);

        r = chaseat_prefix_મૂળ(c->resolved_path, મૂળ, &p);
        જો (r < 0)
                પાછા log_full_errno(log_level, r, "Failed to prefix '%s' with root '%s': %m", c->resolved_path, મૂળ);
        free_and_replace(c->resolved_path, p);

        /* Do not use chaseat_prefix_root(), as it is for the result of chaseat(), but the path is not chased. */
        p = path_join(empty_to_મૂળ(મૂળ), skip_leading_slash(c->original_path));
        જો (!p)
                પાછા log_oom_full(log_level);

        path_simplify(p);
        પાછા free_and_replace(c->original_path, p);
}

સ્થિર bool conf_files_need_stat(ConfFilesFlags flags) {
        પાછા (flags & (CONF_FILES_FILTER_MASKED | CONF_FILES_REGULAR | CONF_FILES_DIRECTORY | CONF_FILES_EXECUTABLE)) != 0;
}

સ્થિર ChaseFlags conf_files_chase_flags(ConfFilesFlags flags) {
        ChaseFlags chase_flags = 0;

        જો (!conf_files_need_stat(flags) || FLAGS_SET(flags, CONF_FILES_FILTER_MASKED_BY_SYMLINK))
                /* Even if no verification is requested, let's unconditionally call chaseat(),
                 * to drop unsafe symlinks. */
                chase_flags |= CHASE_NONEXISTENT;

        પાછા chase_flags;
}

સ્થિર પૂર્ણાંક conf_file_chase_and_verify(
                અચલ અક્ષર *મૂળ,          /* for logging, can be NULL */
                પૂર્ણાંક rfd,
                અચલ અક્ષર *original_path, /* for logging */
                અચલ અક્ષર *path,
                અચલ અક્ષર *name,
                Set **masked,              /* optional */
                ConfFilesFlags flags,
                અક્ષર **ret_path,
                પૂર્ણાંક *ret_fd,
                સંરચના stat *ret_stat) {

        _cleanup_free_ અક્ષર *resolved_path = ખાલી;
        _cleanup_close_ પૂર્ણાંક fd = -EBADF;
        સંરચના stat st = {};
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(wildcard_fd_is_valid(rfd));
        પ્રતિજ્ઞા(original_path);
        પ્રતિજ્ઞા(path);
        પ્રતિજ્ઞા(name);

        પૂર્ણાંક log_level = conf_files_log_level(flags);

        મૂળ = empty_to_મૂળ(મૂળ);

        r = chaseat(rfd, rfd, path, conf_files_chase_flags(flags), &resolved_path, &fd);
        જો (r < 0)
                પાછા log_full_errno(log_level, r, "Failed to chase '%s%s': %m",
                                      મૂળ, skip_leading_slash(original_path));
        જો (r == 0) {
                જો (FLAGS_SET(flags, CONF_FILES_FILTER_MASKED_BY_SYMLINK)) {
                        /* If the path points to /dev/null in a image or so, then the device node may not exist. */
                        જો (path_equal(skip_leading_slash(resolved_path), "dev/null")) {
                                જો (masked) {
                                        /* Mark this one as masked */
                                        r = set_put_strdup(masked, name);
                                        જો (r < 0)
                                                પાછા log_oom_full(log_level);
                                }

                                પાછા log_debug_errno(SYNTHETIC_ERRNO(ERFKILL),
                                                       "File '%s%s' is a mask (symlink to /dev/null).",
                                                       મૂળ, skip_leading_slash(original_path));
                        }
                }

                જો (conf_files_need_stat(flags))
                        /* If we need to have stat, skip the entry. */
                        પાછા log_full_errno(log_level, SYNTHETIC_ERRNO(ENOENT), "Failed to chase '%s%s': %m",
                                              મૂળ, skip_leading_slash(original_path));
        }

        /* Even if we do not need stat, let's take stat now. The caller may use the info later. */
        જો (fd >= 0 && fstat(fd, &st) < 0)
                પાછા log_full_errno(log_level, errno, "Failed to stat '%s%s': %m",
                                      મૂળ, skip_leading_slash(original_path));

        /* Is this a masking entry? */
        જો (FLAGS_SET(flags, CONF_FILES_FILTER_MASKED_BY_SYMLINK) && stat_may_be_dev_null(&st)) {
                જો (masked) {
                        /* Mark this one as masked */
                        r = set_put_strdup(masked, name);
                        જો (r < 0)
                                પાછા log_oom_full(log_level);
                }

                પાછા log_debug_errno(SYNTHETIC_ERRNO(ERFKILL),
                                       "File '%s%s' is a mask (symlink to /dev/null).",
                                       મૂળ, skip_leading_slash(original_path));
        }

        જો (FLAGS_SET(flags, CONF_FILES_FILTER_MASKED_BY_EMPTY) && stat_is_empty(&st)) {
                જો (masked) {
                        /* Mark this one as masked */
                        r = set_put_strdup(masked, name);
                        જો (r < 0)
                                પાછા log_oom_full(log_level);
                }

                પાછા log_debug_errno(SYNTHETIC_ERRNO(ERFKILL),
                                       "File '%s%s' is a mask (an empty file).",
                                       મૂળ, skip_leading_slash(original_path));
        }

        જો (FLAGS_SET(flags, CONF_FILES_REGULAR|CONF_FILES_DIRECTORY)) {
                જો (!S_ISREG(st.st_mode) && !S_ISDIR(st.st_mode))
                        પાછા log_debug_errno(SYNTHETIC_ERRNO(EBADFD),
                                               "File '%s%s' is neither a regular file or directory.",
                                               મૂળ, skip_leading_slash(original_path));
        } નહીંતર {
                /* Is this node a regular file? */
                જો (FLAGS_SET(flags, CONF_FILES_REGULAR)) {
                        r = stat_verify_regular(&st);
                        જો (r < 0)
                                પાછા log_debug_errno(r, "File '%s%s' is not a regular file: %m",
                                                       મૂળ, skip_leading_slash(original_path));
                }

                /* Is this node a directory? */
                જો (FLAGS_SET(flags, CONF_FILES_DIRECTORY)) {
                        r = stat_verify_directory(&st);
                        જો (r < 0)
                                પાછા log_debug_errno(r, "File '%s%s' is not a directory: %m",
                                                       મૂળ, skip_leading_slash(original_path));
                }
        }

        /* Does this node have the executable bit set?
         * As requested: check if the file is marked executable. Note that we don't check access(X_OK) here,
         * as we care about whether the file is marked executable at all, and not whether it is executable
         * for us, because if so, such errors are stuff we should log about. */
        જો (FLAGS_SET(flags, CONF_FILES_EXECUTABLE) && (st.st_mode & 0111) == 0)
                પાછા log_debug_errno(SYNTHETIC_ERRNO(ENOEXEC),
                                       "File '%s%s' is not marked executable.",
                                       મૂળ, skip_leading_slash(original_path));

        જો (ret_path)
                *ret_path = TAKE_PTR(resolved_path);
        જો (ret_fd)
                *ret_fd = TAKE_FD(fd);
        જો (ret_stat)
                *ret_stat = st;

        પાછા 0;
}

પૂર્ણાંક conf_file_new_at(
                અચલ અક્ષર *path,
                અચલ અક્ષર *મૂળ,
                પૂર્ણાંક rfd,
                ConfFilesFlags flags,
                ConfFile **ret) {
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(path);
        પ્રતિજ્ઞા(wildcard_fd_is_valid(rfd));
        પ્રતિજ્ઞા(ret);

        પૂર્ણાંક log_level = conf_files_log_level(flags);

        _cleanup_free_ અક્ષર *મૂળ_2 = ખાલી;
        જો (DEBUG_LOGGING && !મૂળ) {
                (શૂન્ય) fd_get_path(rfd, &મૂળ_2);
                મૂળ = મૂળ_2;
        }

        _cleanup_(conf_file_freep) ConfFile *c = new(ConfFile, 1);
        જો (!c)
                પાછા log_oom_full(log_level);

        *c = (ConfFile) {
                .original_path = strdup(path),
                .fd = -EBADF,
        };

        જો (!c->original_path)
                પાછા log_oom_full(log_level);

        r = path_extract_filename(path, &c->filename);
        જો (r < 0)
                પાછા log_full_errno(log_level, r, "Failed to extract filename from '%s': %m", path);

        _cleanup_free_ અક્ષર *dirpath = ખાલી, *resolved_dirpath = ખાલી;
        r = path_extract_directory(path, &dirpath);
        જો (r < 0 && r != -EDESTADDRREQ)
                પાછા log_full_errno(log_level, r, "Failed to extract directory from '%s': %m", path);
        જો (r >= 0) {
                r = chaseat(rfd, rfd, dirpath,
                            CHASE_MUST_BE_DIRECTORY | conf_files_chase_flags(flags),
                            &resolved_dirpath, /* ret_fd= */ ખાલી);
                જો (r < 0)
                        પાછા log_full_errno(log_level, r, "Failed to chase '%s%s': %m", empty_to_મૂળ(મૂળ), skip_leading_slash(dirpath));
        }

        c->result = path_join(resolved_dirpath, c->filename);
        જો (!c->result)
                પાછા log_oom_full(log_level);

        r = conf_file_chase_and_verify(
                        મૂળ,
                        rfd,
                        c->original_path,
                        c->result,
                        c->filename,
                        /* masked= */ ખાલી,
                        flags,
                        &c->resolved_path,
                        &c->fd,
                        &c->st);
        જો (r < 0)
                પાછા r;

        *ret = TAKE_PTR(c);
        પાછા 0;
}

પૂર્ણાંક conf_file_new(અચલ અક્ષર *path, અચલ અક્ષર *મૂળ, ConfFilesFlags flags, ConfFile **ret) {
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(path);
        પ્રતિજ્ઞા(ret);

        _cleanup_free_ અક્ષર *મૂળ_abs = ખાલી;
        _cleanup_close_ પૂર્ણાંક rfd = -EBADF;
        r = prepare_dirs(મૂળ, flags, /* dirs= */ ખાલી, &મૂળ_abs, &rfd, /* ret_dirs= */ ખાલી);
        જો (r < 0)
                પાછા r;

        _cleanup_free_ અક્ષર *path_abs = ખાલી;
        જો (!મૂળ_abs) {
                r = path_make_absolute_cwd(path, &path_abs);
                જો (r < 0)
                        પાછા log_full_errno(conf_files_log_level(flags), r, "Failed to make '%s' absolute: %m", path);

                path = path_abs;
        }

        _cleanup_(conf_file_freep) ConfFile *c = ખાલી;
        r = conf_file_new_at(path, મૂળ_abs, rfd, flags, &c);
        જો (r < 0)
                પાછા r;

        r = conf_file_prefix_મૂળ(c, મૂળ_abs, flags);
        જો (r < 0)
                પાછા r;

        *ret = TAKE_PTR(c);
        પાછા 0;
}

DEFINE_PRIVATE_HASH_OPS_WITH_કિંમત_DESTRUCTOR(
                conf_file_hash_ops,
                અક્ષર, string_hash_func, string_compare_func,
                ConfFile, conf_file_free);

સ્થિર પૂર્ણાંક files_add(
                DIR *dir,
                અચલ અક્ષર *original_dirpath,
                અચલ અક્ષર *resolved_dirpath,
                અચલ અક્ષર *મૂળ, /* for logging, can be NULL */
                પૂર્ણાંક rfd,
                Hashmap **files,
                Set **masked,
                અચલ અક્ષર *suffix,
                ConfFilesFlags flags) {

        પૂર્ણાંક r;

        પ્રતિજ્ઞા(dir);
        પ્રતિજ્ઞા(original_dirpath);
        પ્રતિજ્ઞા(resolved_dirpath);
        પ્રતિજ્ઞા(wildcard_fd_is_valid(rfd));
        પ્રતિજ્ઞા(files);
        પ્રતિજ્ઞા(masked);

        પૂર્ણાંક log_level = conf_files_log_level(flags);

        મૂળ = empty_to_મૂળ(મૂળ);

        FOREACH_DIRENT(de, dir, પાછા log_full_errno(log_level, errno, "Failed to read directory '%s%s': %m",
                                                      મૂળ, skip_leading_slash(original_dirpath))) {

                _cleanup_free_ અક્ષર *original_path = path_join(original_dirpath, de->d_name);
                જો (!original_path)
                        પાછા log_oom_full(log_level);

                /* Does this match the suffix? */
                જો (suffix && !endswith(de->d_name, suffix)) {
                        log_debug("Skipping file '%s%s', suffix is not '%s'.", મૂળ, skip_leading_slash(original_path), suffix);
                        ચાલુ;
                }

                /* Has this file already been found in an earlier directory? */
                જો (hashmap_contains(*files, de->d_name)) {
                        log_debug("Skipping overridden file '%s%s'.", મૂળ, skip_leading_slash(original_path));
                        ચાલુ;
                }

                /* Has this been masked in an earlier directory? */
                જો ((flags & CONF_FILES_FILTER_MASKED) != 0 && set_contains(*masked, de->d_name)) {
                        log_debug("File '%s%s' is masked by previous entry.", મૂળ, skip_leading_slash(original_path));
                        ચાલુ;
                }

                _cleanup_free_ અક્ષર *p = path_join(resolved_dirpath, de->d_name);
                જો (!p)
                        પાછા log_oom_full(log_level);

                _cleanup_free_ અક્ષર *resolved_path = ખાલી;
                _cleanup_close_ પૂર્ણાંક fd = -EBADF;
                સંરચના stat st;
                r = conf_file_chase_and_verify(
                                મૂળ,
                                rfd,
                                original_path,
                                p,
                                de->d_name,
                                masked,
                                flags,
                                &resolved_path,
                                &fd,
                                &st);
                જો (r == -ENOMEM)
                        પાછા r;
                જો (r < 0)
                        ચાલુ;

                _cleanup_(conf_file_freep) ConfFile *c = new(ConfFile, 1);
                જો (!c)
                        પાછા log_oom_full(log_level);

                *c = (ConfFile) {
                        .filename = strdup(de->d_name),
                        .result = TAKE_PTR(p),
                        .original_path = TAKE_PTR(original_path),
                        .resolved_path = TAKE_PTR(resolved_path),
                        .fd = TAKE_FD(fd),
                        .st = st,
                };

                જો (!c->filename)
                        પાછા log_oom_full(log_level);

                r = hashmap_ensure_put(files, &conf_file_hash_ops, c->filename, c);
                જો (r < 0) {
                        પ્રતિજ્ઞા(r == -ENOMEM);
                        પાછા log_oom_full(log_level);
                }
                પ્રતિજ્ઞા(r > 0);

                TAKE_PTR(c);
        }

        પાછા 0;
}

સ્થિર પૂર્ણાંક dump_files(Hashmap *fh, અચલ અક્ષર *મૂળ, ConfFilesFlags flags, ConfFile ***ret_files, size_t *ret_n_files) {
        ConfFile **files = ખાલી;
        size_t n_files = 0;
        પૂર્ણાંક r;

        CLEANUP_ARRAY(files, n_files, conf_file_free_array);

        પ્રતિજ્ઞા(ret_files);
        પ્રતિજ્ઞા(ret_n_files);

        /* The entries in the array given by hashmap_dump_sorted() are still owned by the hashmap. */
        r = hashmap_dump_sorted(fh, (શૂન્ય***) &files, &n_files);
        જો (r < 0)
                પાછા log_oom_full(conf_files_log_level(flags));

        /* Hence, we need to remove them from the hashmap. */
        FOREACH_ARRAY(i, files, n_files)
                assert_se(hashmap_remove(fh, (*i)->filename) == *i);

        જો (મૂળ)
                FOREACH_ARRAY(i, files, n_files) {
                        r = conf_file_prefix_મૂળ(*i, મૂળ, flags);
                        જો (r < 0)
                                પાછા r;
                }

        *ret_files = TAKE_PTR(files);
        *ret_n_files = n_files;
        પાછા 0;
}

સ્થિર પૂર્ણાંક copy_and_sort_files_from_hashmap(
                Hashmap *fh,
                અચલ અક્ષર *suffix,
                અચલ અક્ષર *મૂળ,
                ConfFilesFlags flags,
                અક્ષર ***ret) {

        _cleanup_strv_free_ અક્ષર **results = ખાલી;
        _cleanup_free_ ConfFile **files = ખાલી;
        size_t n_files = 0, n_results = 0;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(ret);

        પૂર્ણાંક log_level = conf_files_log_level(flags);

        /* The entries in the array given by hashmap_dump_sorted() are still owned by the hashmap.
         * Hence, do not use conf_file_free_array() for 'entries' */
        r = hashmap_dump_sorted(fh, (શૂન્ય***) &files, &n_files);
        જો (r < 0)
                પાછા log_oom_full(log_level);

        FOREACH_ARRAY(i, files, n_files) {
                ConfFile *c = *i;
                અચલ અક્ષર *add = ખાલી;

                જો (FLAGS_SET(flags, CONF_FILES_BASENAME))
                        add = c->filename;
                નહીંતર જો (મૂળ && !FLAGS_SET(flags, CONF_FILES_DONT_PREFIX_મૂળ)) {
                        _cleanup_free_ અક્ષર *p = ખાલી;

                        r = chaseat_prefix_મૂળ(c->result, મૂળ, &p);
                        જો (r < 0)
                                પાછા log_full_errno(log_level, r, "Failed to prefix '%s' with root '%s': %m", c->result, મૂળ);

                        જો (FLAGS_SET(flags, CONF_FILES_TRUNCATE_SUFFIX) && suffix) {
                                અક્ષર *e = endswith(p, suffix);
                                જો (!e)
                                        ચાલુ;

                                *e = 0;
                        }

                        જો (strv_consume_with_size(&results, &n_results, TAKE_PTR(p)) < 0)
                                પાછા log_oom_full(log_level);

                        ચાલુ;
                } નહીંતર
                        add = c->result;

                જો (FLAGS_SET(flags, CONF_FILES_TRUNCATE_SUFFIX)) {
                        અચલ અક્ષર *e = endswith(add, suffix);
                        જો (!e)
                                ચાલુ;

                        _cleanup_free_ અક્ષર *n = strndup(add, e - add);
                        જો (!n)
                                પાછા log_oom_full(log_level);

                        r = strv_consume_with_size(&results, &n_results, TAKE_PTR(n));
                } નહીંતર
                        r = strv_extend_with_size(&results, &n_results, add);
                જો (r < 0)
                        પાછા log_oom_full(log_level);
        }

        *ret = TAKE_PTR(results);
        પાછા 0;
}

સ્થિર પૂર્ણાંક ઉમેરો_replacement(Hashmap **fh, ConfFile *replacement, ConfFilesFlags flags, અચલ ConfFile **ret) {
        _cleanup_(conf_file_freep) ConfFile *c = ASSERT_PTR(replacement);
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(fh);
        પ્રતિજ્ઞા(ret);

        /* This consumes the input ConfFile. */

        ConfFile *existing = hashmap_get(*fh, c->filename);
        જો (existing) {
                log_debug("An entry with higher priority '%s' -> '%s' already exists, ignoring the replacement: %s",
                          existing->filename, existing->result, c->original_path);
                *ret = ખાલી;
                પાછા 0;
        }

        r = hashmap_ensure_put(fh, &conf_file_hash_ops, c->filename, c);
        જો (r < 0) {
                પ્રતિજ્ઞા(r == -ENOMEM);
                પાછા log_oom_full(conf_files_log_level(flags));
        }
        પ્રતિજ્ઞા(r > 0);

        log_debug("Inserted replacement: '%s' -> '%s'", c->filename, c->result);

        *ret = TAKE_PTR(c);
        પાછા 0;
}

સ્થિર પૂર્ણાંક conf_files_list_impl(
                અચલ અક્ષર *suffix,
                અચલ અક્ષર *મૂળ, /* for logging, can be NULL */
                પૂર્ણાંક rfd,
                ConfFilesFlags flags,
                અચલ અક્ષર * અચલ *dirs,
                અચલ અક્ષર *replacement,
                Hashmap **ret,
                અચલ ConfFile **ret_inserted) {

        _cleanup_hashmap_free_ Hashmap *fh = ખાલી;
        _cleanup_set_free_ Set *masked = ખાલી;
        _cleanup_(conf_file_freep) ConfFile *c = ખાલી;
        અચલ ConfFile *inserted = ખાલી;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(wildcard_fd_is_valid(rfd));
        પ્રતિજ્ઞા(ret);

        મૂળ = empty_to_મૂળ(મૂળ);

        જો (replacement) {
                r = conf_file_new_at(replacement, મૂળ, rfd, flags & CONF_FILES_WARN, &c);
                જો (r < 0)
                        પાછા r;
        }

        STRV_FOREACH(p, dirs) {
                _cleanup_closedir_ DIR *dir = ખાલી;
                _cleanup_free_ અક્ષર *path = ખાલી;

                r = chase_and_opendirat(rfd, rfd, *p, 0, &path, &dir);
                જો (r < 0) {
                        જો (r != -ENOENT)
                                log_full_errno(conf_files_log_level(flags), r,
                                               "Failed to chase and open directory '%s%s', ignoring: %m",
                                               મૂળ, skip_leading_slash(*p));
                        ચાલુ;
                }

                જો (c && streq_ptr(path_startswith(c->result, path), c->filename)) {
                        r = ઉમેરો_replacement(&fh, TAKE_PTR(c), flags, &inserted);
                        જો (r < 0)
                                પાછા r;
                }

                r = files_add(dir, *p, path, મૂળ, rfd, &fh, &masked, suffix, flags);
                જો (r == -ENOMEM)
                        પાછા r;
        }

        જો (c) {
                r = ઉમેરો_replacement(&fh, TAKE_PTR(c), flags, &inserted);
                જો (r < 0)
                        પાછા r;
        }

        *ret = TAKE_PTR(fh);
        જો (ret_inserted)
                *ret_inserted = inserted;
        પાછા 0;
}

પૂર્ણાંક conf_files_list_strv(
                અક્ષર ***ret,
                અચલ અક્ષર *suffix,
                અચલ અક્ષર *મૂળ,
                ConfFilesFlags flags,
                અચલ અક્ષર * અચલ *dirs) {

        _cleanup_hashmap_free_ Hashmap *fh = ખાલી;
        _cleanup_close_ પૂર્ણાંક rfd = -EBADF;
        _cleanup_free_ અક્ષર *મૂળ_abs = ખાલી;
        _cleanup_strv_free_ અક્ષર **dirs_abs = ખાલી;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(ret);

        r = prepare_dirs(મૂળ, flags, (અક્ષર**) dirs, &મૂળ_abs, &rfd, &dirs_abs);
        જો (r < 0)
                પાછા r;

        r = conf_files_list_impl(suffix, મૂળ_abs, rfd, flags, (અચલ અક્ષર * અચલ *) dirs_abs,
                                 /* replacement= */ ખાલી, &fh, /* ret_inserted= */ ખાલી);
        જો (r < 0)
                પાછા r;

        પાછા copy_and_sort_files_from_hashmap(fh, suffix, empty_to_મૂળ(મૂળ_abs), flags, ret);
}

પૂર્ણાંક conf_files_list_strv_full(
                અચલ અક્ષર *suffix,
                અચલ અક્ષર *મૂળ,
                ConfFilesFlags flags,
                અચલ અક્ષર * અચલ *dirs,
                ConfFile ***ret_files,
                size_t *ret_n_files) {

        _cleanup_hashmap_free_ Hashmap *fh = ખાલી;
        _cleanup_close_ પૂર્ણાંક rfd = -EBADF;
        _cleanup_free_ અક્ષર *મૂળ_abs = ખાલી;
        _cleanup_strv_free_ અક્ષર **dirs_abs = ખાલી;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(ret_files);
        પ્રતિજ્ઞા(ret_n_files);

        r = prepare_dirs(મૂળ, flags, (અક્ષર**) dirs, &મૂળ_abs, &rfd, &dirs_abs);
        જો (r < 0)
                પાછા r;

        r = conf_files_list_impl(suffix, મૂળ_abs, rfd, flags, (અચલ અક્ષર * અચલ *) dirs_abs,
                                 /* replacement= */ ખાલી, &fh, /* ret_inserted= */ ખાલી);
        જો (r < 0)
                પાછા r;

        પાછા dump_files(fh, empty_to_મૂળ(મૂળ_abs), flags, ret_files, ret_n_files);
}

પૂર્ણાંક conf_files_list_strv_at(
                અક્ષર ***ret,
                અચલ અક્ષર *suffix,
                પૂર્ણાંક rfd,
                ConfFilesFlags flags,
                અચલ અક્ષર * અચલ *dirs) {

        _cleanup_hashmap_free_ Hashmap *fh = ખાલી;
        _cleanup_free_ અક્ષર *મૂળ = ખાલી;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(wildcard_fd_is_valid(rfd));
        પ્રતિજ્ઞા(ret);

        જો (DEBUG_LOGGING)
                (શૂન્ય) fd_get_path(rfd, &મૂળ); /* for logging */

        r = conf_files_list_impl(suffix, મૂળ, rfd, flags, dirs, /* replacement= */ ખાલી, &fh, /* ret_inserted= */ ખાલી);
        જો (r < 0)
                પાછા r;

        પાછા copy_and_sort_files_from_hashmap(fh, suffix, /* root= */ ખાલી, flags, ret);
}

પૂર્ણાંક conf_files_list_strv_at_full(
                અચલ અક્ષર *suffix,
                પૂર્ણાંક rfd,
                ConfFilesFlags flags,
                અચલ અક્ષર * અચલ *dirs,
                ConfFile ***ret_files,
                size_t *ret_n_files) {

        _cleanup_hashmap_free_ Hashmap *fh = ખાલી;
        _cleanup_free_ અક્ષર *મૂળ = ખાલી;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(wildcard_fd_is_valid(rfd));
        પ્રતિજ્ઞા(ret_files);
        પ્રતિજ્ઞા(ret_n_files);

        જો (DEBUG_LOGGING)
                (શૂન્ય) fd_get_path(rfd, &મૂળ); /* for logging */

        r = conf_files_list_impl(suffix, મૂળ, rfd, flags, dirs, /* replacement= */ ખાલી, &fh, /* ret_inserted= */ ખાલી);
        જો (r < 0)
                પાછા r;

        પાછા dump_files(fh, /* root= */ ખાલી, flags, ret_files, ret_n_files);
}

પૂર્ણાંક conf_files_list(અક્ષર ***ret, અચલ અક્ષર *suffix, અચલ અક્ષર *મૂળ, ConfFilesFlags flags, અચલ અક્ષર *dir) {
        પાછા conf_files_list_strv(ret, suffix, મૂળ, flags, STRV_MAKE_CONST(dir));
}

પૂર્ણાંક conf_files_list_full(અચલ અક્ષર *suffix, અચલ અક્ષર *મૂળ, ConfFilesFlags flags, અચલ અક્ષર *dir, ConfFile ***ret_files, size_t *ret_n_files) {
        પાછા conf_files_list_strv_full(suffix, મૂળ, flags, STRV_MAKE_CONST(dir), ret_files, ret_n_files);
}

પૂર્ણાંક conf_files_list_at(અક્ષર ***ret, અચલ અક્ષર *suffix, પૂર્ણાંક rfd, ConfFilesFlags flags, અચલ અક્ષર *dir) {
        પાછા conf_files_list_strv_at(ret, suffix, rfd, flags, STRV_MAKE_CONST(dir));
}

પૂર્ણાંક conf_files_list_at_full(અચલ અક્ષર *suffix, પૂર્ણાંક rfd, ConfFilesFlags flags, અચલ અક્ષર *dir, ConfFile ***ret_files, size_t *ret_n_files) {
        પાછા conf_files_list_strv_at_full(suffix, rfd, flags, STRV_MAKE_CONST(dir), ret_files, ret_n_files);
}

પૂર્ણાંક conf_files_list_nulstr(અક્ષર ***ret, અચલ અક્ષર *suffix, અચલ અક્ષર *મૂળ, ConfFilesFlags flags, અચલ અક્ષર *dirs) {
        _cleanup_strv_free_ અક્ષર **d = ખાલી;

        પ્રતિજ્ઞા(ret);

        d = strv_split_nulstr(dirs);
        જો (!d)
                પાછા log_oom_full(conf_files_log_level(flags));

        પાછા conf_files_list_strv(ret, suffix, મૂળ, flags, (અચલ અક્ષર**) d);
}

પૂર્ણાંક conf_files_list_nulstr_full(અચલ અક્ષર *suffix, અચલ અક્ષર *મૂળ, ConfFilesFlags flags, અચલ અક્ષર *dirs, ConfFile ***ret_files, size_t *ret_n_files) {
        _cleanup_strv_free_ અક્ષર **d = ખાલી;

        પ્રતિજ્ઞા(ret_files);
        પ્રતિજ્ઞા(ret_n_files);

        d = strv_split_nulstr(dirs);
        જો (!d)
                પાછા log_oom_full(conf_files_log_level(flags));

        પાછા conf_files_list_strv_full(suffix, મૂળ, flags, (અચલ અક્ષર**) d, ret_files, ret_n_files);
}

પૂર્ણાંક conf_files_list_nulstr_at(અક્ષર ***ret, અચલ અક્ષર *suffix, પૂર્ણાંક rfd, ConfFilesFlags flags, અચલ અક્ષર *dirs) {
        _cleanup_strv_free_ અક્ષર **d = ખાલી;

        પ્રતિજ્ઞા(ret);

        d = strv_split_nulstr(dirs);
        જો (!d)
                પાછા log_oom_full(conf_files_log_level(flags));

        પાછા conf_files_list_strv_at(ret, suffix, rfd, flags, (અચલ અક્ષર**) d);
}

પૂર્ણાંક conf_files_list_nulstr_at_full(અચલ અક્ષર *suffix, પૂર્ણાંક rfd, ConfFilesFlags flags, અચલ અક્ષર *dirs, ConfFile ***ret_files, size_t *ret_n_files) {
        _cleanup_strv_free_ અક્ષર **d = ખાલી;

        પ્રતિજ્ઞા(ret_files);
        પ્રતિજ્ઞા(ret_n_files);

        d = strv_split_nulstr(dirs);
        જો (!d)
                પાછા log_oom_full(conf_files_log_level(flags));

        પાછા conf_files_list_strv_at_full(suffix, rfd, flags, (અચલ અક્ષર**) d, ret_files, ret_n_files);
}

પૂર્ણાંક conf_files_list_with_replacement(
                અચલ અક્ષર *મૂળ,
                અક્ષર **config_dirs,
                અચલ અક્ષર *replacement,
                અક્ષર ***ret_files,
                અક્ષર **ret_inserted) {

        _cleanup_hashmap_free_ Hashmap *fh = ખાલી;
        _cleanup_free_ અક્ષર *inserted = ખાલી;
        ConfFilesFlags flags = CONF_FILES_REGULAR | CONF_FILES_FILTER_MASKED_BY_SYMLINK | CONF_FILES_WARN;
        _cleanup_close_ પૂર્ણાંક rfd = -EBADF;
        _cleanup_free_ અક્ષર *મૂળ_abs = ખાલી;
        _cleanup_strv_free_ અક્ષર **dirs_abs = ખાલી;
        અચલ ConfFile *c = ખાલી;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(ret_files);

        r = prepare_dirs(મૂળ, flags, config_dirs, &મૂળ_abs, &rfd, &dirs_abs);
        જો (r < 0)
                પાછા r;

        r = conf_files_list_impl(".conf", મૂળ_abs, rfd, flags, (અચલ અક્ષર * અચલ *) dirs_abs,
                                 replacement, &fh, ret_inserted ? &c : ખાલી);
        જો (r < 0)
                પાછા r;

        જો (c) {
                r = chaseat_prefix_મૂળ(c->result, મૂળ_abs, &inserted);
                જો (r < 0)
                        પાછા log_full_errno(conf_files_log_level(flags), r,
                                              "Failed to prefix '%s' with root '%s': %m",
                                              c->result, empty_to_મૂળ(મૂળ_abs));
        }

        r = copy_and_sort_files_from_hashmap(fh, ".conf", empty_to_મૂળ(મૂળ_abs), flags, ret_files);
        જો (r < 0)
                પાછા r;

        જો (ret_inserted)
                *ret_inserted = TAKE_PTR(inserted);
        પાછા 0;
}

પૂર્ણાંક conf_files_list_dropins(
                અક્ષર ***ret,
                અચલ અક્ષર *dropin_dirname,
                અચલ અક્ષર *મૂળ,
                પૂર્ણાંક મૂળ_fd,
                ConfFilesFlags flags,
                અચલ અક્ષર * અચલ *dirs) {

        _cleanup_strv_free_ અક્ષર **dropin_dirs = ખાલી;
        અચલ અક્ષર *suffix;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(ret);
        પ્રતિજ્ઞા(dropin_dirname);
        પ્રતિજ્ઞા(dirs);

        suffix = strjoina("/", dropin_dirname);
        r = strv_extend_strv_concat(&dropin_dirs, dirs, suffix);
        જો (r < 0)
                પાછા log_oom_full(conf_files_log_level(flags));

        પાછા conf_files_list_strv_at(ret, ".conf", મૂળ_fd, flags, (અચલ અક્ષર* અચલ*) dropin_dirs);
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
પૂર્ણાંક conf_file_read(
                અચલ અક્ષર *મૂળ,
                અચલ અક્ષર **config_dirs,
                અચલ અક્ષર *fn,
                parse_લીટી_t parse_લીટી,
                શૂન્ય *userdata,
                bool ignore_enoent,
                bool *invalid_config) {

        _cleanup_fclose_ FILE *_f = ખાલી;
        _cleanup_free_ અક્ષર *_fn = ખાલી;
        unsigned v = 0;
        FILE *f;
        પૂર્ણાંક r = 0;

        પ્રતિજ્ઞા(fn);

        જો (streq(fn, "-")) {
                f = stdin;
                fn = "<stdin>";

                log_debug("Reading config from stdin%s", glyph(GLYPH_ELLIPSIS));

        } નહીંતર જો (is_path(fn)) {
                r = path_make_absolute_cwd(fn, &_fn);
                જો (r < 0)
                        પાછા log_error_errno(r, "Failed to make path absolute: %m");
                fn = _fn;

                f = _f = fopen(fn, "re");
                જો (!_f)
                        r = -errno;
                નહીંતર
                        log_debug("Reading config file \"%s\"%s", fn, glyph(GLYPH_ELLIPSIS));

        } નહીંતર {
                r = શોધ_and_fopen(fn, "re", મૂળ, config_dirs, &_f, &_fn);
                જો (r >= 0) {
                        f = _f;
                        fn = _fn;
                        log_debug("Reading config file \"%s\"%s", fn, glyph(GLYPH_ELLIPSIS));
                }
        }

        જો (r == -ENOENT && ignore_enoent) {
                log_debug_errno(r, "Failed to open \"%s\", ignoring: %m", fn);
                પાછા 0; /* No error, but nothing happened. */
        }
        જો (r < 0)
                પાછા log_error_errno(r, "Failed to read '%s': %m", fn);

        r = 1;  /* We entered the part where we may modify state. */

        માટે (;;) {
                _cleanup_free_ અક્ષર *લીટી = ખાલી;
                bool invalid_લીટી = false;
                પૂર્ણાંક k;

                k = read_stripped_લીટી(f, LONG_લીટી_MAX, &લીટી);
                જો (k < 0)
                        પાછા log_error_errno(k, "Failed to read '%s': %m", fn);
                જો (k == 0)
                        તોડો;

                v++;

                જો (IN_SET(લીટી[0], 0, '#'))
                        ચાલુ;

                k = parse_લીટી(fn, v, લીટી, invalid_config ? &invalid_લીટી : ખાલી, userdata);
                જો (k < 0 && invalid_લીટી)
                        /* Allow reporting with a special code if the caller requested this. */
                        *invalid_config = true;
                નહીંતર
                        /* The first error, if any, becomes our return value. */
                        RET_GATHER(r, k);
        }

        જો (ferror(f))
                RET_GATHER(r, log_error_errno(SYNTHETIC_ERRNO(EIO), "Failed to read from file %s.", fn));

        પાછા r;
}
