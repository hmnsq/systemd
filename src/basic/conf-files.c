/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাৱ <stdio.h>
#অন্তর্ভাৱ <stdlib.h>

#অন্তর্ভাৱ "alloc-util.h"
#অন্তর্ভাৱ "chase.h"
#অন্তর্ভাৱ "conf-files.h"
#অন্তর্ভাৱ "dirent-util.h"
#অন্তর্ভাৱ "errno-util.h"
#অন্তর্ভাৱ "fd-util.h"
#অন্তর্ভাৱ "fileio.h"
#অন্তর্ভাৱ "glyph-util.h"
#অন্তর্ভাৱ "hashmap.h"
#অন্তর্ভাৱ "log.h"
#অন্তর্ভাৱ "nulstr-util.h"
#অন্তর্ভাৱ "path-util.h"
#অন্তর্ভাৱ "set.h"
#অন্তর্ভাৱ "stat-util.h"
#অন্তর্ভাৱ "string-util.h"
#অন্তর্ভাৱ "strv.h"

ConfFile* conf_file_free(ConfFile *c) {
        যদি (!c)
                ঘূৰাই রিক্ত;

        মুক্ত(c->filename);
        মুক্ত(c->result);
        মুক্ত(c->original_path);
        মুক্ত(c->resolved_path);
        safe_close(c->fd);

        ঘূৰাই mfree(c);
}

DEFINE_POINTER_ARRAY_FREE_FUNC(ConfFile*, conf_file_free);

স্থির পূর্ণাঙ্ক conf_files_log_level(ConfFilesFlags flags) {
        ঘূৰাই FLAGS_SET(flags, CONF_FILES_WARN) ? LOG_WARNING : LOG_DEBUG;
}

স্থির পূর্ণাঙ্ক prepare_dirs(
                ধ্রুৱ ৱর্ণ *মূল,
                ConfFilesFlags flags,
                ৱর্ণ * ধ্রুৱ *dirs,
                ৱর্ণ **ret_মূল,
                পূর্ণাঙ্ক *ret_rfd,
                ৱর্ণ ***ret_dirs) {

        _cleanup_free_ ৱর্ণ *মূল_abs = রিক্ত;
        _cleanup_strv_free_ ৱর্ণ **dirs_abs = রিক্ত;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(ret_মূল);
        প্রতিজ্ঞা(ret_rfd);
        প্রতিজ্ঞা(ret_dirs || strv_isempty(dirs));

        পূর্ণাঙ্ক log_level = conf_files_log_level(flags);

        r = empty_or_মূল_harder_to_null(&মূল);
        যদি (r < 0)
                ঘূৰাই log_full_errno(log_level, r, "Failed to determine if '%s' points to the root directory: %m", strempty(মূল));

        যদি (ret_dirs) {
                dirs_abs = strv_copy(dirs);
                যদি (!dirs_abs)
                        ঘূৰাই log_oom_full(log_level);
        }

        _cleanup_close_ পূর্ণাঙ্ক rfd = XAT_FDROOT;
        যদি (মূল) {
                /* When a non-trivial root is specified, we will prefix the result later. Hence, it is not
                 * necessary to modify each config directories here. but needs to normalize the root directory. */
                r = path_make_absolute_cwd(মূল, &মূল_abs);
                যদি (r < 0)
                        ঘূৰাই log_full_errno(log_level, r, "Failed to make '%s' absolute: %m", মূল);

                path_simplify(মূল_abs);

                rfd = open(মূল, O_CLOEXEC|O_DIRECTORY|O_PATH);
                যদি (rfd < 0)
                        ঘূৰাই log_full_errno(log_level, errno, "Failed to open '%s': %m", মূল_abs);

        } অন্যথা যদি (ret_dirs) {
                /* When an empty root or "/" is specified, we will open "/" below, hence we need to make
                 * each config directory absolute if relative. */
                r = path_strv_make_absolute_cwd(dirs_abs);
                যদি (r < 0)
                        ঘূৰাই log_full_errno(log_level, r, "Failed to make directories absolute: %m");
        }

        *ret_মূল = TAKE_PTR(মূল_abs);
        *ret_rfd = TAKE_FD(rfd);
        যদি (ret_dirs)
                *ret_dirs = TAKE_PTR(dirs_abs);
        ঘূৰাই 0;
}

স্থির পূর্ণাঙ্ক conf_file_prefix_মূল(ConfFile *c, ধ্রুৱ ৱর্ণ *মূল, ConfFilesFlags flags) {
        ৱর্ণ *p;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(c);

        যদি (FLAGS_SET(flags, CONF_FILES_DONT_PREFIX_মূল))
                ঘূৰাই 0;

        পূর্ণাঙ্ক log_level = conf_files_log_level(flags);

        r = chaseat_prefix_মূল(c->result, মূল, &p);
        যদি (r < 0)
                ঘূৰাই log_full_errno(log_level, r, "Failed to prefix '%s' with root '%s': %m", c->result, মূল);
        free_and_replace(c->result, p);

        r = chaseat_prefix_মূল(c->resolved_path, মূল, &p);
        যদি (r < 0)
                ঘূৰাই log_full_errno(log_level, r, "Failed to prefix '%s' with root '%s': %m", c->resolved_path, মূল);
        free_and_replace(c->resolved_path, p);

        /* Do not use chaseat_prefix_root(), as it is for the result of chaseat(), but the path is not chased. */
        p = path_join(empty_to_মূল(মূল), skip_leading_slash(c->original_path));
        যদি (!p)
                ঘূৰাই log_oom_full(log_level);

        path_simplify(p);
        ঘূৰাই free_and_replace(c->original_path, p);
}

স্থির bool conf_files_need_stat(ConfFilesFlags flags) {
        ঘূৰাই (flags & (CONF_FILES_FILTER_MASKED | CONF_FILES_REGULAR | CONF_FILES_DIRECTORY | CONF_FILES_EXECUTABLE)) != 0;
}

স্থির ChaseFlags conf_files_chase_flags(ConfFilesFlags flags) {
        ChaseFlags chase_flags = 0;

        যদি (!conf_files_need_stat(flags) || FLAGS_SET(flags, CONF_FILES_FILTER_MASKED_BY_SYMLINK))
                /* Even if no verification is requested, let's unconditionally call chaseat(),
                 * to drop unsafe symlinks. */
                chase_flags |= CHASE_NONEXISTENT;

        ঘূৰাই chase_flags;
}

স্থির পূর্ণাঙ্ক conf_file_chase_and_verify(
                ধ্রুৱ ৱর্ণ *মূল,          /* for logging, can be NULL */
                পূর্ণাঙ্ক rfd,
                ধ্রুৱ ৱর্ণ *original_path, /* for logging */
                ধ্রুৱ ৱর্ণ *path,
                ধ্রুৱ ৱর্ণ *name,
                Set **masked,              /* optional */
                ConfFilesFlags flags,
                ৱর্ণ **ret_path,
                পূর্ণাঙ্ক *ret_fd,
                সংরচনা stat *ret_stat) {

        _cleanup_free_ ৱর্ণ *resolved_path = রিক্ত;
        _cleanup_close_ পূর্ণাঙ্ক fd = -EBADF;
        সংরচনা stat st = {};
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(wildcard_fd_is_valid(rfd));
        প্রতিজ্ঞা(original_path);
        প্রতিজ্ঞা(path);
        প্রতিজ্ঞা(name);

        পূর্ণাঙ্ক log_level = conf_files_log_level(flags);

        মূল = empty_to_মূল(মূল);

        r = chaseat(rfd, rfd, path, conf_files_chase_flags(flags), &resolved_path, &fd);
        যদি (r < 0)
                ঘূৰাই log_full_errno(log_level, r, "Failed to chase '%s%s': %m",
                                      মূল, skip_leading_slash(original_path));
        যদি (r == 0) {
                যদি (FLAGS_SET(flags, CONF_FILES_FILTER_MASKED_BY_SYMLINK)) {
                        /* If the path points to /dev/null in a image or so, then the device node may not exist. */
                        যদি (path_equal(skip_leading_slash(resolved_path), "dev/null")) {
                                যদি (masked) {
                                        /* Mark this one as masked */
                                        r = set_put_strdup(masked, name);
                                        যদি (r < 0)
                                                ঘূৰাই log_oom_full(log_level);
                                }

                                ঘূৰাই log_debug_errno(SYNTHETIC_ERRNO(ERFKILL),
                                                       "File '%s%s' is a mask (symlink to /dev/null).",
                                                       মূল, skip_leading_slash(original_path));
                        }
                }

                যদি (conf_files_need_stat(flags))
                        /* If we need to have stat, skip the entry. */
                        ঘূৰাই log_full_errno(log_level, SYNTHETIC_ERRNO(ENOENT), "Failed to chase '%s%s': %m",
                                              মূল, skip_leading_slash(original_path));
        }

        /* Even if we do not need stat, let's take stat now. The caller may use the info later. */
        যদি (fd >= 0 && fstat(fd, &st) < 0)
                ঘূৰাই log_full_errno(log_level, errno, "Failed to stat '%s%s': %m",
                                      মূল, skip_leading_slash(original_path));

        /* Is this a masking entry? */
        যদি (FLAGS_SET(flags, CONF_FILES_FILTER_MASKED_BY_SYMLINK) && stat_may_be_dev_null(&st)) {
                যদি (masked) {
                        /* Mark this one as masked */
                        r = set_put_strdup(masked, name);
                        যদি (r < 0)
                                ঘূৰাই log_oom_full(log_level);
                }

                ঘূৰাই log_debug_errno(SYNTHETIC_ERRNO(ERFKILL),
                                       "File '%s%s' is a mask (symlink to /dev/null).",
                                       মূল, skip_leading_slash(original_path));
        }

        যদি (FLAGS_SET(flags, CONF_FILES_FILTER_MASKED_BY_EMPTY) && stat_is_empty(&st)) {
                যদি (masked) {
                        /* Mark this one as masked */
                        r = set_put_strdup(masked, name);
                        যদি (r < 0)
                                ঘূৰাই log_oom_full(log_level);
                }

                ঘূৰাই log_debug_errno(SYNTHETIC_ERRNO(ERFKILL),
                                       "File '%s%s' is a mask (an empty file).",
                                       মূল, skip_leading_slash(original_path));
        }

        যদি (FLAGS_SET(flags, CONF_FILES_REGULAR|CONF_FILES_DIRECTORY)) {
                যদি (!S_ISREG(st.st_mode) && !S_ISDIR(st.st_mode))
                        ঘূৰাই log_debug_errno(SYNTHETIC_ERRNO(EBADFD),
                                               "File '%s%s' is neither a regular file or directory.",
                                               মূল, skip_leading_slash(original_path));
        } অন্যথা {
                /* Is this node a regular file? */
                যদি (FLAGS_SET(flags, CONF_FILES_REGULAR)) {
                        r = stat_verify_regular(&st);
                        যদি (r < 0)
                                ঘূৰাই log_debug_errno(r, "File '%s%s' is not a regular file: %m",
                                                       মূল, skip_leading_slash(original_path));
                }

                /* Is this node a directory? */
                যদি (FLAGS_SET(flags, CONF_FILES_DIRECTORY)) {
                        r = stat_verify_directory(&st);
                        যদি (r < 0)
                                ঘূৰাই log_debug_errno(r, "File '%s%s' is not a directory: %m",
                                                       মূল, skip_leading_slash(original_path));
                }
        }

        /* Does this node have the executable bit set?
         * As requested: check if the file is marked executable. Note that we don't check access(X_OK) here,
         * as we care about whether the file is marked executable at all, and not whether it is executable
         * for us, because if so, such errors are stuff we should log about. */
        যদি (FLAGS_SET(flags, CONF_FILES_EXECUTABLE) && (st.st_mode & 0111) == 0)
                ঘূৰাই log_debug_errno(SYNTHETIC_ERRNO(ENOEXEC),
                                       "File '%s%s' is not marked executable.",
                                       মূল, skip_leading_slash(original_path));

        যদি (ret_path)
                *ret_path = TAKE_PTR(resolved_path);
        যদি (ret_fd)
                *ret_fd = TAKE_FD(fd);
        যদি (ret_stat)
                *ret_stat = st;

        ঘূৰাই 0;
}

পূর্ণাঙ্ক conf_file_new_at(
                ধ্রুৱ ৱর্ণ *path,
                ধ্রুৱ ৱর্ণ *মূল,
                পূর্ণাঙ্ক rfd,
                ConfFilesFlags flags,
                ConfFile **ret) {
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(path);
        প্রতিজ্ঞা(wildcard_fd_is_valid(rfd));
        প্রতিজ্ঞা(ret);

        পূর্ণাঙ্ক log_level = conf_files_log_level(flags);

        _cleanup_free_ ৱর্ণ *মূল_2 = রিক্ত;
        যদি (DEBUG_LOGGING && !মূল) {
                (শূন্য) fd_get_path(rfd, &মূল_2);
                মূল = মূল_2;
        }

        _cleanup_(conf_file_freep) ConfFile *c = new(ConfFile, 1);
        যদি (!c)
                ঘূৰাই log_oom_full(log_level);

        *c = (ConfFile) {
                .original_path = strdup(path),
                .fd = -EBADF,
        };

        যদি (!c->original_path)
                ঘূৰাই log_oom_full(log_level);

        r = path_extract_filename(path, &c->filename);
        যদি (r < 0)
                ঘূৰাই log_full_errno(log_level, r, "Failed to extract filename from '%s': %m", path);

        _cleanup_free_ ৱর্ণ *dirpath = রিক্ত, *resolved_dirpath = রিক্ত;
        r = path_extract_directory(path, &dirpath);
        যদি (r < 0 && r != -EDESTADDRREQ)
                ঘূৰাই log_full_errno(log_level, r, "Failed to extract directory from '%s': %m", path);
        যদি (r >= 0) {
                r = chaseat(rfd, rfd, dirpath,
                            CHASE_MUST_BE_DIRECTORY | conf_files_chase_flags(flags),
                            &resolved_dirpath, /* ret_fd= */ রিক্ত);
                যদি (r < 0)
                        ঘূৰাই log_full_errno(log_level, r, "Failed to chase '%s%s': %m", empty_to_মূল(মূল), skip_leading_slash(dirpath));
        }

        c->result = path_join(resolved_dirpath, c->filename);
        যদি (!c->result)
                ঘূৰাই log_oom_full(log_level);

        r = conf_file_chase_and_verify(
                        মূল,
                        rfd,
                        c->original_path,
                        c->result,
                        c->filename,
                        /* masked= */ রিক্ত,
                        flags,
                        &c->resolved_path,
                        &c->fd,
                        &c->st);
        যদি (r < 0)
                ঘূৰাই r;

        *ret = TAKE_PTR(c);
        ঘূৰাই 0;
}

পূর্ণাঙ্ক conf_file_new(ধ্রুৱ ৱর্ণ *path, ধ্রুৱ ৱর্ণ *মূল, ConfFilesFlags flags, ConfFile **ret) {
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(path);
        প্রতিজ্ঞা(ret);

        _cleanup_free_ ৱর্ণ *মূল_abs = রিক্ত;
        _cleanup_close_ পূর্ণাঙ্ক rfd = -EBADF;
        r = prepare_dirs(মূল, flags, /* dirs= */ রিক্ত, &মূল_abs, &rfd, /* ret_dirs= */ রিক্ত);
        যদি (r < 0)
                ঘূৰাই r;

        _cleanup_free_ ৱর্ণ *path_abs = রিক্ত;
        যদি (!মূল_abs) {
                r = path_make_absolute_cwd(path, &path_abs);
                যদি (r < 0)
                        ঘূৰাই log_full_errno(conf_files_log_level(flags), r, "Failed to make '%s' absolute: %m", path);

                path = path_abs;
        }

        _cleanup_(conf_file_freep) ConfFile *c = রিক্ত;
        r = conf_file_new_at(path, মূল_abs, rfd, flags, &c);
        যদি (r < 0)
                ঘূৰাই r;

        r = conf_file_prefix_মূল(c, মূল_abs, flags);
        যদি (r < 0)
                ঘূৰাই r;

        *ret = TAKE_PTR(c);
        ঘূৰাই 0;
}

DEFINE_PRIVATE_HASH_OPS_WITH_মান_DESTRUCTOR(
                conf_file_hash_ops,
                ৱর্ণ, string_hash_func, string_compare_func,
                ConfFile, conf_file_free);

স্থির পূর্ণাঙ্ক files_add(
                DIR *dir,
                ধ্রুৱ ৱর্ণ *original_dirpath,
                ধ্রুৱ ৱর্ণ *resolved_dirpath,
                ধ্রুৱ ৱর্ণ *মূল, /* for logging, can be NULL */
                পূর্ণাঙ্ক rfd,
                Hashmap **files,
                Set **masked,
                ধ্রুৱ ৱর্ণ *suffix,
                ConfFilesFlags flags) {

        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(dir);
        প্রতিজ্ঞা(original_dirpath);
        প্রতিজ্ঞা(resolved_dirpath);
        প্রতিজ্ঞা(wildcard_fd_is_valid(rfd));
        প্রতিজ্ঞা(files);
        প্রতিজ্ঞা(masked);

        পূর্ণাঙ্ক log_level = conf_files_log_level(flags);

        মূল = empty_to_মূল(মূল);

        FOREACH_DIRENT(de, dir, ঘূৰাই log_full_errno(log_level, errno, "Failed to read directory '%s%s': %m",
                                                      মূল, skip_leading_slash(original_dirpath))) {

                _cleanup_free_ ৱর্ণ *original_path = path_join(original_dirpath, de->d_name);
                যদি (!original_path)
                        ঘূৰাই log_oom_full(log_level);

                /* Does this match the suffix? */
                যদি (suffix && !endswith(de->d_name, suffix)) {
                        log_debug("Skipping file '%s%s', suffix is not '%s'.", মূল, skip_leading_slash(original_path), suffix);
                        অনুৱর্ত;
                }

                /* Has this file already been found in an earlier directory? */
                যদি (hashmap_contains(*files, de->d_name)) {
                        log_debug("Skipping overridden file '%s%s'.", মূল, skip_leading_slash(original_path));
                        অনুৱর্ত;
                }

                /* Has this been masked in an earlier directory? */
                যদি ((flags & CONF_FILES_FILTER_MASKED) != 0 && set_contains(*masked, de->d_name)) {
                        log_debug("File '%s%s' is masked by previous entry.", মূল, skip_leading_slash(original_path));
                        অনুৱর্ত;
                }

                _cleanup_free_ ৱর্ণ *p = path_join(resolved_dirpath, de->d_name);
                যদি (!p)
                        ঘূৰাই log_oom_full(log_level);

                _cleanup_free_ ৱর্ণ *resolved_path = রিক্ত;
                _cleanup_close_ পূর্ণাঙ্ক fd = -EBADF;
                সংরচনা stat st;
                r = conf_file_chase_and_verify(
                                মূল,
                                rfd,
                                original_path,
                                p,
                                de->d_name,
                                masked,
                                flags,
                                &resolved_path,
                                &fd,
                                &st);
                যদি (r == -ENOMEM)
                        ঘূৰাই r;
                যদি (r < 0)
                        অনুৱর্ত;

                _cleanup_(conf_file_freep) ConfFile *c = new(ConfFile, 1);
                যদি (!c)
                        ঘূৰাই log_oom_full(log_level);

                *c = (ConfFile) {
                        .filename = strdup(de->d_name),
                        .result = TAKE_PTR(p),
                        .original_path = TAKE_PTR(original_path),
                        .resolved_path = TAKE_PTR(resolved_path),
                        .fd = TAKE_FD(fd),
                        .st = st,
                };

                যদি (!c->filename)
                        ঘূৰাই log_oom_full(log_level);

                r = hashmap_ensure_put(files, &conf_file_hash_ops, c->filename, c);
                যদি (r < 0) {
                        প্রতিজ্ঞা(r == -ENOMEM);
                        ঘূৰাই log_oom_full(log_level);
                }
                প্রতিজ্ঞা(r > 0);

                TAKE_PTR(c);
        }

        ঘূৰাই 0;
}

স্থির পূর্ণাঙ্ক dump_files(Hashmap *fh, ধ্রুৱ ৱর্ণ *মূল, ConfFilesFlags flags, ConfFile ***ret_files, size_t *ret_n_files) {
        ConfFile **files = রিক্ত;
        size_t n_files = 0;
        পূর্ণাঙ্ক r;

        CLEANUP_ARRAY(files, n_files, conf_file_free_array);

        প্রতিজ্ঞা(ret_files);
        প্রতিজ্ঞা(ret_n_files);

        /* The entries in the array given by hashmap_dump_sorted() are still owned by the hashmap. */
        r = hashmap_dump_sorted(fh, (শূন্য***) &files, &n_files);
        যদি (r < 0)
                ঘূৰাই log_oom_full(conf_files_log_level(flags));

        /* Hence, we need to remove them from the hashmap. */
        FOREACH_ARRAY(i, files, n_files)
                assert_se(hashmap_remove(fh, (*i)->filename) == *i);

        যদি (মূল)
                FOREACH_ARRAY(i, files, n_files) {
                        r = conf_file_prefix_মূল(*i, মূল, flags);
                        যদি (r < 0)
                                ঘূৰাই r;
                }

        *ret_files = TAKE_PTR(files);
        *ret_n_files = n_files;
        ঘূৰাই 0;
}

স্থির পূর্ণাঙ্ক copy_and_sort_files_from_hashmap(
                Hashmap *fh,
                ধ্রুৱ ৱর্ণ *suffix,
                ধ্রুৱ ৱর্ণ *মূল,
                ConfFilesFlags flags,
                ৱর্ণ ***ret) {

        _cleanup_strv_free_ ৱর্ণ **results = রিক্ত;
        _cleanup_free_ ConfFile **files = রিক্ত;
        size_t n_files = 0, n_results = 0;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(ret);

        পূর্ণাঙ্ক log_level = conf_files_log_level(flags);

        /* The entries in the array given by hashmap_dump_sorted() are still owned by the hashmap.
         * Hence, do not use conf_file_free_array() for 'entries' */
        r = hashmap_dump_sorted(fh, (শূন্য***) &files, &n_files);
        যদি (r < 0)
                ঘূৰাই log_oom_full(log_level);

        FOREACH_ARRAY(i, files, n_files) {
                ConfFile *c = *i;
                ধ্রুৱ ৱর্ণ *add = রিক্ত;

                যদি (FLAGS_SET(flags, CONF_FILES_BASENAME))
                        add = c->filename;
                অন্যথা যদি (মূল && !FLAGS_SET(flags, CONF_FILES_DONT_PREFIX_মূল)) {
                        _cleanup_free_ ৱর্ণ *p = রিক্ত;

                        r = chaseat_prefix_মূল(c->result, মূল, &p);
                        যদি (r < 0)
                                ঘূৰাই log_full_errno(log_level, r, "Failed to prefix '%s' with root '%s': %m", c->result, মূল);

                        যদি (FLAGS_SET(flags, CONF_FILES_TRUNCATE_SUFFIX) && suffix) {
                                ৱর্ণ *e = endswith(p, suffix);
                                যদি (!e)
                                        অনুৱর্ত;

                                *e = 0;
                        }

                        যদি (strv_consume_with_size(&results, &n_results, TAKE_PTR(p)) < 0)
                                ঘূৰাই log_oom_full(log_level);

                        অনুৱর্ত;
                } অন্যথা
                        add = c->result;

                যদি (FLAGS_SET(flags, CONF_FILES_TRUNCATE_SUFFIX)) {
                        ধ্রুৱ ৱর্ণ *e = endswith(add, suffix);
                        যদি (!e)
                                অনুৱর্ত;

                        _cleanup_free_ ৱর্ণ *n = strndup(add, e - add);
                        যদি (!n)
                                ঘূৰাই log_oom_full(log_level);

                        r = strv_consume_with_size(&results, &n_results, TAKE_PTR(n));
                } অন্যথা
                        r = strv_extend_with_size(&results, &n_results, add);
                যদি (r < 0)
                        ঘূৰাই log_oom_full(log_level);
        }

        *ret = TAKE_PTR(results);
        ঘূৰাই 0;
}

স্থির পূর্ণাঙ্ক ঢোকাও_replacement(Hashmap **fh, ConfFile *replacement, ConfFilesFlags flags, ধ্রুৱ ConfFile **ret) {
        _cleanup_(conf_file_freep) ConfFile *c = ASSERT_PTR(replacement);
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(fh);
        প্রতিজ্ঞা(ret);

        /* This consumes the input ConfFile. */

        ConfFile *existing = hashmap_get(*fh, c->filename);
        যদি (existing) {
                log_debug("An entry with higher priority '%s' -> '%s' already exists, ignoring the replacement: %s",
                          existing->filename, existing->result, c->original_path);
                *ret = রিক্ত;
                ঘূৰাই 0;
        }

        r = hashmap_ensure_put(fh, &conf_file_hash_ops, c->filename, c);
        যদি (r < 0) {
                প্রতিজ্ঞা(r == -ENOMEM);
                ঘূৰাই log_oom_full(conf_files_log_level(flags));
        }
        প্রতিজ্ঞা(r > 0);

        log_debug("Inserted replacement: '%s' -> '%s'", c->filename, c->result);

        *ret = TAKE_PTR(c);
        ঘূৰাই 0;
}

স্থির পূর্ণাঙ্ক conf_files_list_impl(
                ধ্রুৱ ৱর্ণ *suffix,
                ধ্রুৱ ৱর্ণ *মূল, /* for logging, can be NULL */
                পূর্ণাঙ্ক rfd,
                ConfFilesFlags flags,
                ধ্রুৱ ৱর্ণ * ধ্রুৱ *dirs,
                ধ্রুৱ ৱর্ণ *replacement,
                Hashmap **ret,
                ধ্রুৱ ConfFile **ret_inserted) {

        _cleanup_hashmap_free_ Hashmap *fh = রিক্ত;
        _cleanup_set_free_ Set *masked = রিক্ত;
        _cleanup_(conf_file_freep) ConfFile *c = রিক্ত;
        ধ্রুৱ ConfFile *inserted = রিক্ত;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(wildcard_fd_is_valid(rfd));
        প্রতিজ্ঞা(ret);

        মূল = empty_to_মূল(মূল);

        যদি (replacement) {
                r = conf_file_new_at(replacement, মূল, rfd, flags & CONF_FILES_WARN, &c);
                যদি (r < 0)
                        ঘূৰাই r;
        }

        STRV_FOREACH(p, dirs) {
                _cleanup_closedir_ DIR *dir = রিক্ত;
                _cleanup_free_ ৱর্ণ *path = রিক্ত;

                r = chase_and_opendirat(rfd, rfd, *p, 0, &path, &dir);
                যদি (r < 0) {
                        যদি (r != -ENOENT)
                                log_full_errno(conf_files_log_level(flags), r,
                                               "Failed to chase and open directory '%s%s', ignoring: %m",
                                               মূল, skip_leading_slash(*p));
                        অনুৱর্ত;
                }

                যদি (c && streq_ptr(path_startswith(c->result, path), c->filename)) {
                        r = ঢোকাও_replacement(&fh, TAKE_PTR(c), flags, &inserted);
                        যদি (r < 0)
                                ঘূৰাই r;
                }

                r = files_add(dir, *p, path, মূল, rfd, &fh, &masked, suffix, flags);
                যদি (r == -ENOMEM)
                        ঘূৰাই r;
        }

        যদি (c) {
                r = ঢোকাও_replacement(&fh, TAKE_PTR(c), flags, &inserted);
                যদি (r < 0)
                        ঘূৰাই r;
        }

        *ret = TAKE_PTR(fh);
        যদি (ret_inserted)
                *ret_inserted = inserted;
        ঘূৰাই 0;
}

পূর্ণাঙ্ক conf_files_list_strv(
                ৱর্ণ ***ret,
                ধ্রুৱ ৱর্ণ *suffix,
                ধ্রুৱ ৱর্ণ *মূল,
                ConfFilesFlags flags,
                ধ্রুৱ ৱর্ণ * ধ্রুৱ *dirs) {

        _cleanup_hashmap_free_ Hashmap *fh = রিক্ত;
        _cleanup_close_ পূর্ণাঙ্ক rfd = -EBADF;
        _cleanup_free_ ৱর্ণ *মূল_abs = রিক্ত;
        _cleanup_strv_free_ ৱর্ণ **dirs_abs = রিক্ত;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(ret);

        r = prepare_dirs(মূল, flags, (ৱর্ণ**) dirs, &মূল_abs, &rfd, &dirs_abs);
        যদি (r < 0)
                ঘূৰাই r;

        r = conf_files_list_impl(suffix, মূল_abs, rfd, flags, (ধ্রুৱ ৱর্ণ * ধ্রুৱ *) dirs_abs,
                                 /* replacement= */ রিক্ত, &fh, /* ret_inserted= */ রিক্ত);
        যদি (r < 0)
                ঘূৰাই r;

        ঘূৰাই copy_and_sort_files_from_hashmap(fh, suffix, empty_to_মূল(মূল_abs), flags, ret);
}

পূর্ণাঙ্ক conf_files_list_strv_full(
                ধ্রুৱ ৱর্ণ *suffix,
                ধ্রুৱ ৱর্ণ *মূল,
                ConfFilesFlags flags,
                ধ্রুৱ ৱর্ণ * ধ্রুৱ *dirs,
                ConfFile ***ret_files,
                size_t *ret_n_files) {

        _cleanup_hashmap_free_ Hashmap *fh = রিক্ত;
        _cleanup_close_ পূর্ণাঙ্ক rfd = -EBADF;
        _cleanup_free_ ৱর্ণ *মূল_abs = রিক্ত;
        _cleanup_strv_free_ ৱর্ণ **dirs_abs = রিক্ত;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(ret_files);
        প্রতিজ্ঞা(ret_n_files);

        r = prepare_dirs(মূল, flags, (ৱর্ণ**) dirs, &মূল_abs, &rfd, &dirs_abs);
        যদি (r < 0)
                ঘূৰাই r;

        r = conf_files_list_impl(suffix, মূল_abs, rfd, flags, (ধ্রুৱ ৱর্ণ * ধ্রুৱ *) dirs_abs,
                                 /* replacement= */ রিক্ত, &fh, /* ret_inserted= */ রিক্ত);
        যদি (r < 0)
                ঘূৰাই r;

        ঘূৰাই dump_files(fh, empty_to_মূল(মূল_abs), flags, ret_files, ret_n_files);
}

পূর্ণাঙ্ক conf_files_list_strv_at(
                ৱর্ণ ***ret,
                ধ্রুৱ ৱর্ণ *suffix,
                পূর্ণাঙ্ক rfd,
                ConfFilesFlags flags,
                ধ্রুৱ ৱর্ণ * ধ্রুৱ *dirs) {

        _cleanup_hashmap_free_ Hashmap *fh = রিক্ত;
        _cleanup_free_ ৱর্ণ *মূল = রিক্ত;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(wildcard_fd_is_valid(rfd));
        প্রতিজ্ঞা(ret);

        যদি (DEBUG_LOGGING)
                (শূন্য) fd_get_path(rfd, &মূল); /* for logging */

        r = conf_files_list_impl(suffix, মূল, rfd, flags, dirs, /* replacement= */ রিক্ত, &fh, /* ret_inserted= */ রিক্ত);
        যদি (r < 0)
                ঘূৰাই r;

        ঘূৰাই copy_and_sort_files_from_hashmap(fh, suffix, /* root= */ রিক্ত, flags, ret);
}

পূর্ণাঙ্ক conf_files_list_strv_at_full(
                ধ্রুৱ ৱর্ণ *suffix,
                পূর্ণাঙ্ক rfd,
                ConfFilesFlags flags,
                ধ্রুৱ ৱর্ণ * ধ্রুৱ *dirs,
                ConfFile ***ret_files,
                size_t *ret_n_files) {

        _cleanup_hashmap_free_ Hashmap *fh = রিক্ত;
        _cleanup_free_ ৱর্ণ *মূল = রিক্ত;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(wildcard_fd_is_valid(rfd));
        প্রতিজ্ঞা(ret_files);
        প্রতিজ্ঞা(ret_n_files);

        যদি (DEBUG_LOGGING)
                (শূন্য) fd_get_path(rfd, &মূল); /* for logging */

        r = conf_files_list_impl(suffix, মূল, rfd, flags, dirs, /* replacement= */ রিক্ত, &fh, /* ret_inserted= */ রিক্ত);
        যদি (r < 0)
                ঘূৰাই r;

        ঘূৰাই dump_files(fh, /* root= */ রিক্ত, flags, ret_files, ret_n_files);
}

পূর্ণাঙ্ক conf_files_list(ৱর্ণ ***ret, ধ্রুৱ ৱর্ণ *suffix, ধ্রুৱ ৱর্ণ *মূল, ConfFilesFlags flags, ধ্রুৱ ৱর্ণ *dir) {
        ঘূৰাই conf_files_list_strv(ret, suffix, মূল, flags, STRV_MAKE_CONST(dir));
}

পূর্ণাঙ্ক conf_files_list_full(ধ্রুৱ ৱর্ণ *suffix, ধ্রুৱ ৱর্ণ *মূল, ConfFilesFlags flags, ধ্রুৱ ৱর্ণ *dir, ConfFile ***ret_files, size_t *ret_n_files) {
        ঘূৰাই conf_files_list_strv_full(suffix, মূল, flags, STRV_MAKE_CONST(dir), ret_files, ret_n_files);
}

পূর্ণাঙ্ক conf_files_list_at(ৱর্ণ ***ret, ধ্রুৱ ৱর্ণ *suffix, পূর্ণাঙ্ক rfd, ConfFilesFlags flags, ধ্রুৱ ৱর্ণ *dir) {
        ঘূৰাই conf_files_list_strv_at(ret, suffix, rfd, flags, STRV_MAKE_CONST(dir));
}

পূর্ণাঙ্ক conf_files_list_at_full(ধ্রুৱ ৱর্ণ *suffix, পূর্ণাঙ্ক rfd, ConfFilesFlags flags, ধ্রুৱ ৱর্ণ *dir, ConfFile ***ret_files, size_t *ret_n_files) {
        ঘূৰাই conf_files_list_strv_at_full(suffix, rfd, flags, STRV_MAKE_CONST(dir), ret_files, ret_n_files);
}

পূর্ণাঙ্ক conf_files_list_nulstr(ৱর্ণ ***ret, ধ্রুৱ ৱর্ণ *suffix, ধ্রুৱ ৱর্ণ *মূল, ConfFilesFlags flags, ধ্রুৱ ৱর্ণ *dirs) {
        _cleanup_strv_free_ ৱর্ণ **d = রিক্ত;

        প্রতিজ্ঞা(ret);

        d = strv_split_nulstr(dirs);
        যদি (!d)
                ঘূৰাই log_oom_full(conf_files_log_level(flags));

        ঘূৰাই conf_files_list_strv(ret, suffix, মূল, flags, (ধ্রুৱ ৱর্ণ**) d);
}

পূর্ণাঙ্ক conf_files_list_nulstr_full(ধ্রুৱ ৱর্ণ *suffix, ধ্রুৱ ৱর্ণ *মূল, ConfFilesFlags flags, ধ্রুৱ ৱর্ণ *dirs, ConfFile ***ret_files, size_t *ret_n_files) {
        _cleanup_strv_free_ ৱর্ণ **d = রিক্ত;

        প্রতিজ্ঞা(ret_files);
        প্রতিজ্ঞা(ret_n_files);

        d = strv_split_nulstr(dirs);
        যদি (!d)
                ঘূৰাই log_oom_full(conf_files_log_level(flags));

        ঘূৰাই conf_files_list_strv_full(suffix, মূল, flags, (ধ্রুৱ ৱর্ণ**) d, ret_files, ret_n_files);
}

পূর্ণাঙ্ক conf_files_list_nulstr_at(ৱর্ণ ***ret, ধ্রুৱ ৱর্ণ *suffix, পূর্ণাঙ্ক rfd, ConfFilesFlags flags, ধ্রুৱ ৱর্ণ *dirs) {
        _cleanup_strv_free_ ৱর্ণ **d = রিক্ত;

        প্রতিজ্ঞা(ret);

        d = strv_split_nulstr(dirs);
        যদি (!d)
                ঘূৰাই log_oom_full(conf_files_log_level(flags));

        ঘূৰাই conf_files_list_strv_at(ret, suffix, rfd, flags, (ধ্রুৱ ৱর্ণ**) d);
}

পূর্ণাঙ্ক conf_files_list_nulstr_at_full(ধ্রুৱ ৱর্ণ *suffix, পূর্ণাঙ্ক rfd, ConfFilesFlags flags, ধ্রুৱ ৱর্ণ *dirs, ConfFile ***ret_files, size_t *ret_n_files) {
        _cleanup_strv_free_ ৱর্ণ **d = রিক্ত;

        প্রতিজ্ঞা(ret_files);
        প্রতিজ্ঞা(ret_n_files);

        d = strv_split_nulstr(dirs);
        যদি (!d)
                ঘূৰাই log_oom_full(conf_files_log_level(flags));

        ঘূৰাই conf_files_list_strv_at_full(suffix, rfd, flags, (ধ্রুৱ ৱর্ণ**) d, ret_files, ret_n_files);
}

পূর্ণাঙ্ক conf_files_list_with_replacement(
                ধ্রুৱ ৱর্ণ *মূল,
                ৱর্ণ **config_dirs,
                ধ্রুৱ ৱর্ণ *replacement,
                ৱর্ণ ***ret_files,
                ৱর্ণ **ret_inserted) {

        _cleanup_hashmap_free_ Hashmap *fh = রিক্ত;
        _cleanup_free_ ৱর্ণ *inserted = রিক্ত;
        ConfFilesFlags flags = CONF_FILES_REGULAR | CONF_FILES_FILTER_MASKED_BY_SYMLINK | CONF_FILES_WARN;
        _cleanup_close_ পূর্ণাঙ্ক rfd = -EBADF;
        _cleanup_free_ ৱর্ণ *মূল_abs = রিক্ত;
        _cleanup_strv_free_ ৱর্ণ **dirs_abs = রিক্ত;
        ধ্রুৱ ConfFile *c = রিক্ত;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(ret_files);

        r = prepare_dirs(মূল, flags, config_dirs, &মূল_abs, &rfd, &dirs_abs);
        যদি (r < 0)
                ঘূৰাই r;

        r = conf_files_list_impl(".conf", মূল_abs, rfd, flags, (ধ্রুৱ ৱর্ণ * ধ্রুৱ *) dirs_abs,
                                 replacement, &fh, ret_inserted ? &c : রিক্ত);
        যদি (r < 0)
                ঘূৰাই r;

        যদি (c) {
                r = chaseat_prefix_মূল(c->result, মূল_abs, &inserted);
                যদি (r < 0)
                        ঘূৰাই log_full_errno(conf_files_log_level(flags), r,
                                              "Failed to prefix '%s' with root '%s': %m",
                                              c->result, empty_to_মূল(মূল_abs));
        }

        r = copy_and_sort_files_from_hashmap(fh, ".conf", empty_to_মূল(মূল_abs), flags, ret_files);
        যদি (r < 0)
                ঘূৰাই r;

        যদি (ret_inserted)
                *ret_inserted = TAKE_PTR(inserted);
        ঘূৰাই 0;
}

পূর্ণাঙ্ক conf_files_list_dropins(
                ৱর্ণ ***ret,
                ধ্রুৱ ৱর্ণ *dropin_dirname,
                ধ্রুৱ ৱর্ণ *মূল,
                পূর্ণাঙ্ক মূল_fd,
                ConfFilesFlags flags,
                ধ্রুৱ ৱর্ণ * ধ্রুৱ *dirs) {

        _cleanup_strv_free_ ৱর্ণ **dropin_dirs = রিক্ত;
        ধ্রুৱ ৱর্ণ *suffix;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(ret);
        প্রতিজ্ঞা(dropin_dirname);
        প্রতিজ্ঞা(dirs);

        suffix = strjoina("/", dropin_dirname);
        r = strv_extend_strv_concat(&dropin_dirs, dirs, suffix);
        যদি (r < 0)
                ঘূৰাই log_oom_full(conf_files_log_level(flags));

        ঘূৰাই conf_files_list_strv_at(ret, ".conf", মূল_fd, flags, (ধ্রুৱ ৱর্ণ* ধ্রুৱ*) dropin_dirs);
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
পূর্ণাঙ্ক conf_file_read(
                ধ্রুৱ ৱর্ণ *মূল,
                ধ্রুৱ ৱর্ণ **config_dirs,
                ধ্রুৱ ৱর্ণ *fn,
                parse_সাৰি_t parse_সাৰি,
                শূন্য *userdata,
                bool ignore_enoent,
                bool *invalid_config) {

        _cleanup_fclose_ FILE *_f = রিক্ত;
        _cleanup_free_ ৱর্ণ *_fn = রিক্ত;
        unsigned v = 0;
        FILE *f;
        পূর্ণাঙ্ক r = 0;

        প্রতিজ্ঞা(fn);

        যদি (streq(fn, "-")) {
                f = stdin;
                fn = "<stdin>";

                log_debug("Reading config from stdin%s", glyph(GLYPH_ELLIPSIS));

        } অন্যথা যদি (is_path(fn)) {
                r = path_make_absolute_cwd(fn, &_fn);
                যদি (r < 0)
                        ঘূৰাই log_error_errno(r, "Failed to make path absolute: %m");
                fn = _fn;

                f = _f = fopen(fn, "re");
                যদি (!_f)
                        r = -errno;
                অন্যথা
                        log_debug("Reading config file \"%s\"%s", fn, glyph(GLYPH_ELLIPSIS));

        } অন্যথা {
                r = অনুসন্ধান_and_fopen(fn, "re", মূল, config_dirs, &_f, &_fn);
                যদি (r >= 0) {
                        f = _f;
                        fn = _fn;
                        log_debug("Reading config file \"%s\"%s", fn, glyph(GLYPH_ELLIPSIS));
                }
        }

        যদি (r == -ENOENT && ignore_enoent) {
                log_debug_errno(r, "Failed to open \"%s\", ignoring: %m", fn);
                ঘূৰাই 0; /* No error, but nothing happened. */
        }
        যদি (r < 0)
                ঘূৰাই log_error_errno(r, "Failed to read '%s': %m", fn);

        r = 1;  /* We entered the part where we may modify state. */

        বাবে (;;) {
                _cleanup_free_ ৱর্ণ *সাৰি = রিক্ত;
                bool invalid_সাৰি = false;
                পূর্ণাঙ্ক k;

                k = read_stripped_সাৰি(f, LONG_সাৰি_MAX, &সাৰি);
                যদি (k < 0)
                        ঘূৰাই log_error_errno(k, "Failed to read '%s': %m", fn);
                যদি (k == 0)
                        ভঙ্গ;

                v++;

                যদি (IN_SET(সাৰি[0], 0, '#'))
                        অনুৱর্ত;

                k = parse_সাৰি(fn, v, সাৰি, invalid_config ? &invalid_সাৰি : রিক্ত, userdata);
                যদি (k < 0 && invalid_সাৰি)
                        /* Allow reporting with a special code if the caller requested this. */
                        *invalid_config = true;
                অন্যথা
                        /* The first error, if any, becomes our return value. */
                        RET_GATHER(r, k);
        }

        যদি (ferror(f))
                RET_GATHER(r, log_error_errno(SYNTHETIC_ERRNO(EIO), "Failed to read from file %s.", fn));

        ঘূৰাই r;
}
