/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ଅନ୍ତର୍ଭାଵ <stdio.h>
#ଅନ୍ତର୍ଭାଵ <stdlib.h>

#ଅନ୍ତର୍ଭାଵ "alloc-util.h"
#ଅନ୍ତର୍ଭାଵ "chase.h"
#ଅନ୍ତର୍ଭାଵ "conf-files.h"
#ଅନ୍ତର୍ଭାଵ "dirent-util.h"
#ଅନ୍ତର୍ଭାଵ "errno-util.h"
#ଅନ୍ତର୍ଭାଵ "fd-util.h"
#ଅନ୍ତର୍ଭାଵ "fileio.h"
#ଅନ୍ତର୍ଭାଵ "glyph-util.h"
#ଅନ୍ତର୍ଭାଵ "hashmap.h"
#ଅନ୍ତର୍ଭାଵ "log.h"
#ଅନ୍ତର୍ଭାଵ "nulstr-util.h"
#ଅନ୍ତର୍ଭାଵ "path-util.h"
#ଅନ୍ତର୍ଭାଵ "set.h"
#ଅନ୍ତର୍ଭାଵ "stat-util.h"
#ଅନ୍ତର୍ଭାଵ "string-util.h"
#ଅନ୍ତର୍ଭାଵ "strv.h"

ConfFile* conf_file_free(ConfFile *c) {
        ଯଦି (!c)
                ଫେରନ୍ତୁ ଖାଲି;

        ମୁକ୍ତ(c->filename);
        ମୁକ୍ତ(c->result);
        ମୁକ୍ତ(c->original_path);
        ମୁକ୍ତ(c->resolved_path);
        safe_close(c->fd);

        ଫେରନ୍ତୁ mfree(c);
}

DEFINE_POINTER_ARRAY_FREE_FUNC(ConfFile*, conf_file_free);

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା conf_files_log_level(ConfFilesFlags flags) {
        ଫେରନ୍ତୁ FLAGS_SET(flags, CONF_FILES_WARN) ? LOG_WARNING : LOG_DEBUG;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା prepare_dirs(
                ଅଚଳ ଅକ୍ଷର *ମୂଳ,
                ConfFilesFlags flags,
                ଅକ୍ଷର * ଅଚଳ *dirs,
                ଅକ୍ଷର **ret_ମୂଳ,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ret_rfd,
                ଅକ୍ଷର ***ret_dirs) {

        _cleanup_free_ ଅକ୍ଷର *ମୂଳ_abs = ଖାଲି;
        _cleanup_strv_free_ ଅକ୍ଷର **dirs_abs = ଖାଲି;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(ret_ମୂଳ);
        ପ୍ରତିଜ୍ଞା(ret_rfd);
        ପ୍ରତିଜ୍ଞା(ret_dirs || strv_isempty(dirs));

        ପୂର୍ଣ୍ଣସଂଖ୍ୟା log_level = conf_files_log_level(flags);

        r = empty_or_ମୂଳ_harder_to_null(&ମୂଳ);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ log_full_errno(log_level, r, "Failed to determine if '%s' points to the root directory: %m", strempty(ମୂଳ));

        ଯଦି (ret_dirs) {
                dirs_abs = strv_copy(dirs);
                ଯଦି (!dirs_abs)
                        ଫେରନ୍ତୁ log_oom_full(log_level);
        }

        _cleanup_close_ ପୂର୍ଣ୍ଣସଂଖ୍ୟା rfd = XAT_FDROOT;
        ଯଦି (ମୂଳ) {
                /* When a non-trivial root is specified, we will prefix the result later. Hence, it is not
                 * necessary to modify each config directories here. but needs to normalize the root directory. */
                r = path_make_absolute_cwd(ମୂଳ, &ମୂଳ_abs);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ log_full_errno(log_level, r, "Failed to make '%s' absolute: %m", ମୂଳ);

                path_simplify(ମୂଳ_abs);

                rfd = open(ମୂଳ, O_CLOEXEC|O_DIRECTORY|O_PATH);
                ଯଦି (rfd < 0)
                        ଫେରନ୍ତୁ log_full_errno(log_level, errno, "Failed to open '%s': %m", ମୂଳ_abs);

        } ଅନ୍ୟଥା ଯଦି (ret_dirs) {
                /* When an empty root or "/" is specified, we will open "/" below, hence we need to make
                 * each config directory absolute if relative. */
                r = path_strv_make_absolute_cwd(dirs_abs);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ log_full_errno(log_level, r, "Failed to make directories absolute: %m");
        }

        *ret_ମୂଳ = TAKE_PTR(ମୂଳ_abs);
        *ret_rfd = TAKE_FD(rfd);
        ଯଦି (ret_dirs)
                *ret_dirs = TAKE_PTR(dirs_abs);
        ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା conf_file_prefix_ମୂଳ(ConfFile *c, ଅଚଳ ଅକ୍ଷର *ମୂଳ, ConfFilesFlags flags) {
        ଅକ୍ଷର *p;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(c);

        ଯଦି (FLAGS_SET(flags, CONF_FILES_DONT_PREFIX_ମୂଳ))
                ଫେରନ୍ତୁ 0;

        ପୂର୍ଣ୍ଣସଂଖ୍ୟା log_level = conf_files_log_level(flags);

        r = chaseat_prefix_ମୂଳ(c->result, ମୂଳ, &p);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ log_full_errno(log_level, r, "Failed to prefix '%s' with root '%s': %m", c->result, ମୂଳ);
        free_and_replace(c->result, p);

        r = chaseat_prefix_ମୂଳ(c->resolved_path, ମୂଳ, &p);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ log_full_errno(log_level, r, "Failed to prefix '%s' with root '%s': %m", c->resolved_path, ମୂଳ);
        free_and_replace(c->resolved_path, p);

        /* Do not use chaseat_prefix_root(), as it is for the result of chaseat(), but the path is not chased. */
        p = path_join(empty_to_ମୂଳ(ମୂଳ), skip_leading_slash(c->original_path));
        ଯଦି (!p)
                ଫେରନ୍ତୁ log_oom_full(log_level);

        path_simplify(p);
        ଫେରନ୍ତୁ free_and_replace(c->original_path, p);
}

ସ୍ଥିର bool conf_files_need_stat(ConfFilesFlags flags) {
        ଫେରନ୍ତୁ (flags & (CONF_FILES_FILTER_MASKED | CONF_FILES_REGULAR | CONF_FILES_DIRECTORY | CONF_FILES_EXECUTABLE)) != 0;
}

ସ୍ଥିର ChaseFlags conf_files_chase_flags(ConfFilesFlags flags) {
        ChaseFlags chase_flags = 0;

        ଯଦି (!conf_files_need_stat(flags) || FLAGS_SET(flags, CONF_FILES_FILTER_MASKED_BY_SYMLINK))
                /* Even if no verification is requested, let's unconditionally call chaseat(),
                 * to drop unsafe symlinks. */
                chase_flags |= CHASE_NONEXISTENT;

        ଫେରନ୍ତୁ chase_flags;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା conf_file_chase_and_verify(
                ଅଚଳ ଅକ୍ଷର *ମୂଳ,          /* for logging, can be NULL */
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା rfd,
                ଅଚଳ ଅକ୍ଷର *original_path, /* for logging */
                ଅଚଳ ଅକ୍ଷର *path,
                ଅଚଳ ଅକ୍ଷର *name,
                Set **masked,              /* optional */
                ConfFilesFlags flags,
                ଅକ୍ଷର **ret_path,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ret_fd,
                ସଂରଚନା stat *ret_stat) {

        _cleanup_free_ ଅକ୍ଷର *resolved_path = ଖାଲି;
        _cleanup_close_ ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd = -EBADF;
        ସଂରଚନା stat st = {};
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(wildcard_fd_is_valid(rfd));
        ପ୍ରତିଜ୍ଞା(original_path);
        ପ୍ରତିଜ୍ଞା(path);
        ପ୍ରତିଜ୍ଞା(name);

        ପୂର୍ଣ୍ଣସଂଖ୍ୟା log_level = conf_files_log_level(flags);

        ମୂଳ = empty_to_ମୂଳ(ମୂଳ);

        r = chaseat(rfd, rfd, path, conf_files_chase_flags(flags), &resolved_path, &fd);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ log_full_errno(log_level, r, "Failed to chase '%s%s': %m",
                                      ମୂଳ, skip_leading_slash(original_path));
        ଯଦି (r == 0) {
                ଯଦି (FLAGS_SET(flags, CONF_FILES_FILTER_MASKED_BY_SYMLINK)) {
                        /* If the path points to /dev/null in a image or so, then the device node may not exist. */
                        ଯଦି (path_equal(skip_leading_slash(resolved_path), "dev/null")) {
                                ଯଦି (masked) {
                                        /* Mark this one as masked */
                                        r = set_put_strdup(masked, name);
                                        ଯଦି (r < 0)
                                                ଫେରନ୍ତୁ log_oom_full(log_level);
                                }

                                ଫେରନ୍ତୁ log_debug_errno(SYNTHETIC_ERRNO(ERFKILL),
                                                       "File '%s%s' is a mask (symlink to /dev/null).",
                                                       ମୂଳ, skip_leading_slash(original_path));
                        }
                }

                ଯଦି (conf_files_need_stat(flags))
                        /* If we need to have stat, skip the entry. */
                        ଫେରନ୍ତୁ log_full_errno(log_level, SYNTHETIC_ERRNO(ENOENT), "Failed to chase '%s%s': %m",
                                              ମୂଳ, skip_leading_slash(original_path));
        }

        /* Even if we do not need stat, let's take stat now. The caller may use the info later. */
        ଯଦି (fd >= 0 && fstat(fd, &st) < 0)
                ଫେରନ୍ତୁ log_full_errno(log_level, errno, "Failed to stat '%s%s': %m",
                                      ମୂଳ, skip_leading_slash(original_path));

        /* Is this a masking entry? */
        ଯଦି (FLAGS_SET(flags, CONF_FILES_FILTER_MASKED_BY_SYMLINK) && stat_may_be_dev_null(&st)) {
                ଯଦି (masked) {
                        /* Mark this one as masked */
                        r = set_put_strdup(masked, name);
                        ଯଦି (r < 0)
                                ଫେରନ୍ତୁ log_oom_full(log_level);
                }

                ଫେରନ୍ତୁ log_debug_errno(SYNTHETIC_ERRNO(ERFKILL),
                                       "File '%s%s' is a mask (symlink to /dev/null).",
                                       ମୂଳ, skip_leading_slash(original_path));
        }

        ଯଦି (FLAGS_SET(flags, CONF_FILES_FILTER_MASKED_BY_EMPTY) && stat_is_empty(&st)) {
                ଯଦି (masked) {
                        /* Mark this one as masked */
                        r = set_put_strdup(masked, name);
                        ଯଦି (r < 0)
                                ଫେରନ୍ତୁ log_oom_full(log_level);
                }

                ଫେରନ୍ତୁ log_debug_errno(SYNTHETIC_ERRNO(ERFKILL),
                                       "File '%s%s' is a mask (an empty file).",
                                       ମୂଳ, skip_leading_slash(original_path));
        }

        ଯଦି (FLAGS_SET(flags, CONF_FILES_REGULAR|CONF_FILES_DIRECTORY)) {
                ଯଦି (!S_ISREG(st.st_mode) && !S_ISDIR(st.st_mode))
                        ଫେରନ୍ତୁ log_debug_errno(SYNTHETIC_ERRNO(EBADFD),
                                               "File '%s%s' is neither a regular file or directory.",
                                               ମୂଳ, skip_leading_slash(original_path));
        } ଅନ୍ୟଥା {
                /* Is this node a regular file? */
                ଯଦି (FLAGS_SET(flags, CONF_FILES_REGULAR)) {
                        r = stat_verify_regular(&st);
                        ଯଦି (r < 0)
                                ଫେରନ୍ତୁ log_debug_errno(r, "File '%s%s' is not a regular file: %m",
                                                       ମୂଳ, skip_leading_slash(original_path));
                }

                /* Is this node a directory? */
                ଯଦି (FLAGS_SET(flags, CONF_FILES_DIRECTORY)) {
                        r = stat_verify_directory(&st);
                        ଯଦି (r < 0)
                                ଫେରନ୍ତୁ log_debug_errno(r, "File '%s%s' is not a directory: %m",
                                                       ମୂଳ, skip_leading_slash(original_path));
                }
        }

        /* Does this node have the executable bit set?
         * As requested: check if the file is marked executable. Note that we don't check access(X_OK) here,
         * as we care about whether the file is marked executable at all, and not whether it is executable
         * for us, because if so, such errors are stuff we should log about. */
        ଯଦି (FLAGS_SET(flags, CONF_FILES_EXECUTABLE) && (st.st_mode & 0111) == 0)
                ଫେରନ୍ତୁ log_debug_errno(SYNTHETIC_ERRNO(ENOEXEC),
                                       "File '%s%s' is not marked executable.",
                                       ମୂଳ, skip_leading_slash(original_path));

        ଯଦି (ret_path)
                *ret_path = TAKE_PTR(resolved_path);
        ଯଦି (ret_fd)
                *ret_fd = TAKE_FD(fd);
        ଯଦି (ret_stat)
                *ret_stat = st;

        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା conf_file_new_at(
                ଅଚଳ ଅକ୍ଷର *path,
                ଅଚଳ ଅକ୍ଷର *ମୂଳ,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା rfd,
                ConfFilesFlags flags,
                ConfFile **ret) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(path);
        ପ୍ରତିଜ୍ଞା(wildcard_fd_is_valid(rfd));
        ପ୍ରତିଜ୍ଞା(ret);

        ପୂର୍ଣ୍ଣସଂଖ୍ୟା log_level = conf_files_log_level(flags);

        _cleanup_free_ ଅକ୍ଷର *ମୂଳ_2 = ଖାଲି;
        ଯଦି (DEBUG_LOGGING && !ମୂଳ) {
                (ଶୂନ୍ୟ) fd_get_path(rfd, &ମୂଳ_2);
                ମୂଳ = ମୂଳ_2;
        }

        _cleanup_(conf_file_freep) ConfFile *c = new(ConfFile, 1);
        ଯଦି (!c)
                ଫେରନ୍ତୁ log_oom_full(log_level);

        *c = (ConfFile) {
                .original_path = strdup(path),
                .fd = -EBADF,
        };

        ଯଦି (!c->original_path)
                ଫେରନ୍ତୁ log_oom_full(log_level);

        r = path_extract_filename(path, &c->filename);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ log_full_errno(log_level, r, "Failed to extract filename from '%s': %m", path);

        _cleanup_free_ ଅକ୍ଷର *dirpath = ଖାଲି, *resolved_dirpath = ଖାଲି;
        r = path_extract_directory(path, &dirpath);
        ଯଦି (r < 0 && r != -EDESTADDRREQ)
                ଫେରନ୍ତୁ log_full_errno(log_level, r, "Failed to extract directory from '%s': %m", path);
        ଯଦି (r >= 0) {
                r = chaseat(rfd, rfd, dirpath,
                            CHASE_MUST_BE_DIRECTORY | conf_files_chase_flags(flags),
                            &resolved_dirpath, /* ret_fd= */ ଖାଲି);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ log_full_errno(log_level, r, "Failed to chase '%s%s': %m", empty_to_ମୂଳ(ମୂଳ), skip_leading_slash(dirpath));
        }

        c->result = path_join(resolved_dirpath, c->filename);
        ଯଦି (!c->result)
                ଫେରନ୍ତୁ log_oom_full(log_level);

        r = conf_file_chase_and_verify(
                        ମୂଳ,
                        rfd,
                        c->original_path,
                        c->result,
                        c->filename,
                        /* masked= */ ଖାଲି,
                        flags,
                        &c->resolved_path,
                        &c->fd,
                        &c->st);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        *ret = TAKE_PTR(c);
        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା conf_file_new(ଅଚଳ ଅକ୍ଷର *path, ଅଚଳ ଅକ୍ଷର *ମୂଳ, ConfFilesFlags flags, ConfFile **ret) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(path);
        ପ୍ରତିଜ୍ଞା(ret);

        _cleanup_free_ ଅକ୍ଷର *ମୂଳ_abs = ଖାଲି;
        _cleanup_close_ ପୂର୍ଣ୍ଣସଂଖ୍ୟା rfd = -EBADF;
        r = prepare_dirs(ମୂଳ, flags, /* dirs= */ ଖାଲି, &ମୂଳ_abs, &rfd, /* ret_dirs= */ ଖାଲି);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        _cleanup_free_ ଅକ୍ଷର *path_abs = ଖାଲି;
        ଯଦି (!ମୂଳ_abs) {
                r = path_make_absolute_cwd(path, &path_abs);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ log_full_errno(conf_files_log_level(flags), r, "Failed to make '%s' absolute: %m", path);

                path = path_abs;
        }

        _cleanup_(conf_file_freep) ConfFile *c = ଖାଲି;
        r = conf_file_new_at(path, ମୂଳ_abs, rfd, flags, &c);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        r = conf_file_prefix_ମୂଳ(c, ମୂଳ_abs, flags);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        *ret = TAKE_PTR(c);
        ଫେରନ୍ତୁ 0;
}

DEFINE_PRIVATE_HASH_OPS_WITH_ମୂଲ୍ୟ_DESTRUCTOR(
                conf_file_hash_ops,
                ଅକ୍ଷର, string_hash_func, string_compare_func,
                ConfFile, conf_file_free);

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା files_add(
                DIR *dir,
                ଅଚଳ ଅକ୍ଷର *original_dirpath,
                ଅଚଳ ଅକ୍ଷର *resolved_dirpath,
                ଅଚଳ ଅକ୍ଷର *ମୂଳ, /* for logging, can be NULL */
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା rfd,
                Hashmap **files,
                Set **masked,
                ଅଚଳ ଅକ୍ଷର *suffix,
                ConfFilesFlags flags) {

        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(dir);
        ପ୍ରତିଜ୍ଞା(original_dirpath);
        ପ୍ରତିଜ୍ଞା(resolved_dirpath);
        ପ୍ରତିଜ୍ଞା(wildcard_fd_is_valid(rfd));
        ପ୍ରତିଜ୍ଞା(files);
        ପ୍ରତିଜ୍ଞା(masked);

        ପୂର୍ଣ୍ଣସଂଖ୍ୟା log_level = conf_files_log_level(flags);

        ମୂଳ = empty_to_ମୂଳ(ମୂଳ);

        FOREACH_DIRENT(de, dir, ଫେରନ୍ତୁ log_full_errno(log_level, errno, "Failed to read directory '%s%s': %m",
                                                      ମୂଳ, skip_leading_slash(original_dirpath))) {

                _cleanup_free_ ଅକ୍ଷର *original_path = path_join(original_dirpath, de->d_name);
                ଯଦି (!original_path)
                        ଫେରନ୍ତୁ log_oom_full(log_level);

                /* Does this match the suffix? */
                ଯଦି (suffix && !endswith(de->d_name, suffix)) {
                        log_debug("Skipping file '%s%s', suffix is not '%s'.", ମୂଳ, skip_leading_slash(original_path), suffix);
                        ଜାରି;
                }

                /* Has this file already been found in an earlier directory? */
                ଯଦି (hashmap_contains(*files, de->d_name)) {
                        log_debug("Skipping overridden file '%s%s'.", ମୂଳ, skip_leading_slash(original_path));
                        ଜାରି;
                }

                /* Has this been masked in an earlier directory? */
                ଯଦି ((flags & CONF_FILES_FILTER_MASKED) != 0 && set_contains(*masked, de->d_name)) {
                        log_debug("File '%s%s' is masked by previous entry.", ମୂଳ, skip_leading_slash(original_path));
                        ଜାରି;
                }

                _cleanup_free_ ଅକ୍ଷର *p = path_join(resolved_dirpath, de->d_name);
                ଯଦି (!p)
                        ଫେରନ୍ତୁ log_oom_full(log_level);

                _cleanup_free_ ଅକ୍ଷର *resolved_path = ଖାଲି;
                _cleanup_close_ ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd = -EBADF;
                ସଂରଚନା stat st;
                r = conf_file_chase_and_verify(
                                ମୂଳ,
                                rfd,
                                original_path,
                                p,
                                de->d_name,
                                masked,
                                flags,
                                &resolved_path,
                                &fd,
                                &st);
                ଯଦି (r == -ENOMEM)
                        ଫେରନ୍ତୁ r;
                ଯଦି (r < 0)
                        ଜାରି;

                _cleanup_(conf_file_freep) ConfFile *c = new(ConfFile, 1);
                ଯଦି (!c)
                        ଫେରନ୍ତୁ log_oom_full(log_level);

                *c = (ConfFile) {
                        .filename = strdup(de->d_name),
                        .result = TAKE_PTR(p),
                        .original_path = TAKE_PTR(original_path),
                        .resolved_path = TAKE_PTR(resolved_path),
                        .fd = TAKE_FD(fd),
                        .st = st,
                };

                ଯଦି (!c->filename)
                        ଫେରନ୍ତୁ log_oom_full(log_level);

                r = hashmap_ensure_put(files, &conf_file_hash_ops, c->filename, c);
                ଯଦି (r < 0) {
                        ପ୍ରତିଜ୍ଞା(r == -ENOMEM);
                        ଫେରନ୍ତୁ log_oom_full(log_level);
                }
                ପ୍ରତିଜ୍ଞା(r > 0);

                TAKE_PTR(c);
        }

        ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା dump_files(Hashmap *fh, ଅଚଳ ଅକ୍ଷର *ମୂଳ, ConfFilesFlags flags, ConfFile ***ret_files, size_t *ret_n_files) {
        ConfFile **files = ଖାଲି;
        size_t n_files = 0;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        CLEANUP_ARRAY(files, n_files, conf_file_free_array);

        ପ୍ରତିଜ୍ଞା(ret_files);
        ପ୍ରତିଜ୍ଞା(ret_n_files);

        /* The entries in the array given by hashmap_dump_sorted() are still owned by the hashmap. */
        r = hashmap_dump_sorted(fh, (ଶୂନ୍ୟ***) &files, &n_files);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ log_oom_full(conf_files_log_level(flags));

        /* Hence, we need to remove them from the hashmap. */
        FOREACH_ARRAY(i, files, n_files)
                assert_se(hashmap_remove(fh, (*i)->filename) == *i);

        ଯଦି (ମୂଳ)
                FOREACH_ARRAY(i, files, n_files) {
                        r = conf_file_prefix_ମୂଳ(*i, ମୂଳ, flags);
                        ଯଦି (r < 0)
                                ଫେରନ୍ତୁ r;
                }

        *ret_files = TAKE_PTR(files);
        *ret_n_files = n_files;
        ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା copy_and_sort_files_from_hashmap(
                Hashmap *fh,
                ଅଚଳ ଅକ୍ଷର *suffix,
                ଅଚଳ ଅକ୍ଷର *ମୂଳ,
                ConfFilesFlags flags,
                ଅକ୍ଷର ***ret) {

        _cleanup_strv_free_ ଅକ୍ଷର **results = ଖାଲି;
        _cleanup_free_ ConfFile **files = ଖାଲି;
        size_t n_files = 0, n_results = 0;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(ret);

        ପୂର୍ଣ୍ଣସଂଖ୍ୟା log_level = conf_files_log_level(flags);

        /* The entries in the array given by hashmap_dump_sorted() are still owned by the hashmap.
         * Hence, do not use conf_file_free_array() for 'entries' */
        r = hashmap_dump_sorted(fh, (ଶୂନ୍ୟ***) &files, &n_files);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ log_oom_full(log_level);

        FOREACH_ARRAY(i, files, n_files) {
                ConfFile *c = *i;
                ଅଚଳ ଅକ୍ଷର *add = ଖାଲି;

                ଯଦି (FLAGS_SET(flags, CONF_FILES_BASENAME))
                        add = c->filename;
                ଅନ୍ୟଥା ଯଦି (ମୂଳ && !FLAGS_SET(flags, CONF_FILES_DONT_PREFIX_ମୂଳ)) {
                        _cleanup_free_ ଅକ୍ଷର *p = ଖାଲି;

                        r = chaseat_prefix_ମୂଳ(c->result, ମୂଳ, &p);
                        ଯଦି (r < 0)
                                ଫେରନ୍ତୁ log_full_errno(log_level, r, "Failed to prefix '%s' with root '%s': %m", c->result, ମୂଳ);

                        ଯଦି (FLAGS_SET(flags, CONF_FILES_TRUNCATE_SUFFIX) && suffix) {
                                ଅକ୍ଷର *e = endswith(p, suffix);
                                ଯଦି (!e)
                                        ଜାରି;

                                *e = 0;
                        }

                        ଯଦି (strv_consume_with_size(&results, &n_results, TAKE_PTR(p)) < 0)
                                ଫେରନ୍ତୁ log_oom_full(log_level);

                        ଜାରି;
                } ଅନ୍ୟଥା
                        add = c->result;

                ଯଦି (FLAGS_SET(flags, CONF_FILES_TRUNCATE_SUFFIX)) {
                        ଅଚଳ ଅକ୍ଷର *e = endswith(add, suffix);
                        ଯଦି (!e)
                                ଜାରି;

                        _cleanup_free_ ଅକ୍ଷର *n = strndup(add, e - add);
                        ଯଦି (!n)
                                ଫେରନ୍ତୁ log_oom_full(log_level);

                        r = strv_consume_with_size(&results, &n_results, TAKE_PTR(n));
                } ଅନ୍ୟଥା
                        r = strv_extend_with_size(&results, &n_results, add);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ log_oom_full(log_level);
        }

        *ret = TAKE_PTR(results);
        ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଭର୍ତ୍ତି_replacement(Hashmap **fh, ConfFile *replacement, ConfFilesFlags flags, ଅଚଳ ConfFile **ret) {
        _cleanup_(conf_file_freep) ConfFile *c = ASSERT_PTR(replacement);
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(fh);
        ପ୍ରତିଜ୍ଞା(ret);

        /* This consumes the input ConfFile. */

        ConfFile *existing = hashmap_get(*fh, c->filename);
        ଯଦି (existing) {
                log_debug("An entry with higher priority '%s' -> '%s' already exists, ignoring the replacement: %s",
                          existing->filename, existing->result, c->original_path);
                *ret = ଖାଲି;
                ଫେରନ୍ତୁ 0;
        }

        r = hashmap_ensure_put(fh, &conf_file_hash_ops, c->filename, c);
        ଯଦି (r < 0) {
                ପ୍ରତିଜ୍ଞା(r == -ENOMEM);
                ଫେରନ୍ତୁ log_oom_full(conf_files_log_level(flags));
        }
        ପ୍ରତିଜ୍ଞା(r > 0);

        log_debug("Inserted replacement: '%s' -> '%s'", c->filename, c->result);

        *ret = TAKE_PTR(c);
        ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା conf_files_list_impl(
                ଅଚଳ ଅକ୍ଷର *suffix,
                ଅଚଳ ଅକ୍ଷର *ମୂଳ, /* for logging, can be NULL */
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା rfd,
                ConfFilesFlags flags,
                ଅଚଳ ଅକ୍ଷର * ଅଚଳ *dirs,
                ଅଚଳ ଅକ୍ଷର *replacement,
                Hashmap **ret,
                ଅଚଳ ConfFile **ret_inserted) {

        _cleanup_hashmap_free_ Hashmap *fh = ଖାଲି;
        _cleanup_set_free_ Set *masked = ଖାଲି;
        _cleanup_(conf_file_freep) ConfFile *c = ଖାଲି;
        ଅଚଳ ConfFile *inserted = ଖାଲି;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(wildcard_fd_is_valid(rfd));
        ପ୍ରତିଜ୍ଞା(ret);

        ମୂଳ = empty_to_ମୂଳ(ମୂଳ);

        ଯଦି (replacement) {
                r = conf_file_new_at(replacement, ମୂଳ, rfd, flags & CONF_FILES_WARN, &c);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;
        }

        STRV_FOREACH(p, dirs) {
                _cleanup_closedir_ DIR *dir = ଖାଲି;
                _cleanup_free_ ଅକ୍ଷର *path = ଖାଲି;

                r = chase_and_opendirat(rfd, rfd, *p, 0, &path, &dir);
                ଯଦି (r < 0) {
                        ଯଦି (r != -ENOENT)
                                log_full_errno(conf_files_log_level(flags), r,
                                               "Failed to chase and open directory '%s%s', ignoring: %m",
                                               ମୂଳ, skip_leading_slash(*p));
                        ଜାରି;
                }

                ଯଦି (c && streq_ptr(path_startswith(c->result, path), c->filename)) {
                        r = ଭର୍ତ୍ତି_replacement(&fh, TAKE_PTR(c), flags, &inserted);
                        ଯଦି (r < 0)
                                ଫେରନ୍ତୁ r;
                }

                r = files_add(dir, *p, path, ମୂଳ, rfd, &fh, &masked, suffix, flags);
                ଯଦି (r == -ENOMEM)
                        ଫେରନ୍ତୁ r;
        }

        ଯଦି (c) {
                r = ଭର୍ତ୍ତି_replacement(&fh, TAKE_PTR(c), flags, &inserted);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;
        }

        *ret = TAKE_PTR(fh);
        ଯଦି (ret_inserted)
                *ret_inserted = inserted;
        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା conf_files_list_strv(
                ଅକ୍ଷର ***ret,
                ଅଚଳ ଅକ୍ଷର *suffix,
                ଅଚଳ ଅକ୍ଷର *ମୂଳ,
                ConfFilesFlags flags,
                ଅଚଳ ଅକ୍ଷର * ଅଚଳ *dirs) {

        _cleanup_hashmap_free_ Hashmap *fh = ଖାଲି;
        _cleanup_close_ ପୂର୍ଣ୍ଣସଂଖ୍ୟା rfd = -EBADF;
        _cleanup_free_ ଅକ୍ଷର *ମୂଳ_abs = ଖାଲି;
        _cleanup_strv_free_ ଅକ୍ଷର **dirs_abs = ଖାଲି;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(ret);

        r = prepare_dirs(ମୂଳ, flags, (ଅକ୍ଷର**) dirs, &ମୂଳ_abs, &rfd, &dirs_abs);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        r = conf_files_list_impl(suffix, ମୂଳ_abs, rfd, flags, (ଅଚଳ ଅକ୍ଷର * ଅଚଳ *) dirs_abs,
                                 /* replacement= */ ଖାଲି, &fh, /* ret_inserted= */ ଖାଲି);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଫେରନ୍ତୁ copy_and_sort_files_from_hashmap(fh, suffix, empty_to_ମୂଳ(ମୂଳ_abs), flags, ret);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା conf_files_list_strv_full(
                ଅଚଳ ଅକ୍ଷର *suffix,
                ଅଚଳ ଅକ୍ଷର *ମୂଳ,
                ConfFilesFlags flags,
                ଅଚଳ ଅକ୍ଷର * ଅଚଳ *dirs,
                ConfFile ***ret_files,
                size_t *ret_n_files) {

        _cleanup_hashmap_free_ Hashmap *fh = ଖାଲି;
        _cleanup_close_ ପୂର୍ଣ୍ଣସଂଖ୍ୟା rfd = -EBADF;
        _cleanup_free_ ଅକ୍ଷର *ମୂଳ_abs = ଖାଲି;
        _cleanup_strv_free_ ଅକ୍ଷର **dirs_abs = ଖାଲି;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(ret_files);
        ପ୍ରତିଜ୍ଞା(ret_n_files);

        r = prepare_dirs(ମୂଳ, flags, (ଅକ୍ଷର**) dirs, &ମୂଳ_abs, &rfd, &dirs_abs);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        r = conf_files_list_impl(suffix, ମୂଳ_abs, rfd, flags, (ଅଚଳ ଅକ୍ଷର * ଅଚଳ *) dirs_abs,
                                 /* replacement= */ ଖାଲି, &fh, /* ret_inserted= */ ଖାଲି);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଫେରନ୍ତୁ dump_files(fh, empty_to_ମୂଳ(ମୂଳ_abs), flags, ret_files, ret_n_files);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା conf_files_list_strv_at(
                ଅକ୍ଷର ***ret,
                ଅଚଳ ଅକ୍ଷର *suffix,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା rfd,
                ConfFilesFlags flags,
                ଅଚଳ ଅକ୍ଷର * ଅଚଳ *dirs) {

        _cleanup_hashmap_free_ Hashmap *fh = ଖାଲି;
        _cleanup_free_ ଅକ୍ଷର *ମୂଳ = ଖାଲି;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(wildcard_fd_is_valid(rfd));
        ପ୍ରତିଜ୍ଞା(ret);

        ଯଦି (DEBUG_LOGGING)
                (ଶୂନ୍ୟ) fd_get_path(rfd, &ମୂଳ); /* for logging */

        r = conf_files_list_impl(suffix, ମୂଳ, rfd, flags, dirs, /* replacement= */ ଖାଲି, &fh, /* ret_inserted= */ ଖାଲି);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଫେରନ୍ତୁ copy_and_sort_files_from_hashmap(fh, suffix, /* root= */ ଖାଲି, flags, ret);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା conf_files_list_strv_at_full(
                ଅଚଳ ଅକ୍ଷର *suffix,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା rfd,
                ConfFilesFlags flags,
                ଅଚଳ ଅକ୍ଷର * ଅଚଳ *dirs,
                ConfFile ***ret_files,
                size_t *ret_n_files) {

        _cleanup_hashmap_free_ Hashmap *fh = ଖାଲି;
        _cleanup_free_ ଅକ୍ଷର *ମୂଳ = ଖାଲି;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(wildcard_fd_is_valid(rfd));
        ପ୍ରତିଜ୍ଞା(ret_files);
        ପ୍ରତିଜ୍ଞା(ret_n_files);

        ଯଦି (DEBUG_LOGGING)
                (ଶୂନ୍ୟ) fd_get_path(rfd, &ମୂଳ); /* for logging */

        r = conf_files_list_impl(suffix, ମୂଳ, rfd, flags, dirs, /* replacement= */ ଖାଲି, &fh, /* ret_inserted= */ ଖାଲି);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଫେରନ୍ତୁ dump_files(fh, /* root= */ ଖାଲି, flags, ret_files, ret_n_files);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା conf_files_list(ଅକ୍ଷର ***ret, ଅଚଳ ଅକ୍ଷର *suffix, ଅଚଳ ଅକ୍ଷର *ମୂଳ, ConfFilesFlags flags, ଅଚଳ ଅକ୍ଷର *dir) {
        ଫେରନ୍ତୁ conf_files_list_strv(ret, suffix, ମୂଳ, flags, STRV_MAKE_CONST(dir));
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା conf_files_list_full(ଅଚଳ ଅକ୍ଷର *suffix, ଅଚଳ ଅକ୍ଷର *ମୂଳ, ConfFilesFlags flags, ଅଚଳ ଅକ୍ଷର *dir, ConfFile ***ret_files, size_t *ret_n_files) {
        ଫେରନ୍ତୁ conf_files_list_strv_full(suffix, ମୂଳ, flags, STRV_MAKE_CONST(dir), ret_files, ret_n_files);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା conf_files_list_at(ଅକ୍ଷର ***ret, ଅଚଳ ଅକ୍ଷର *suffix, ପୂର୍ଣ୍ଣସଂଖ୍ୟା rfd, ConfFilesFlags flags, ଅଚଳ ଅକ୍ଷର *dir) {
        ଫେରନ୍ତୁ conf_files_list_strv_at(ret, suffix, rfd, flags, STRV_MAKE_CONST(dir));
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା conf_files_list_at_full(ଅଚଳ ଅକ୍ଷର *suffix, ପୂର୍ଣ୍ଣସଂଖ୍ୟା rfd, ConfFilesFlags flags, ଅଚଳ ଅକ୍ଷର *dir, ConfFile ***ret_files, size_t *ret_n_files) {
        ଫେରନ୍ତୁ conf_files_list_strv_at_full(suffix, rfd, flags, STRV_MAKE_CONST(dir), ret_files, ret_n_files);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା conf_files_list_nulstr(ଅକ୍ଷର ***ret, ଅଚଳ ଅକ୍ଷର *suffix, ଅଚଳ ଅକ୍ଷର *ମୂଳ, ConfFilesFlags flags, ଅଚଳ ଅକ୍ଷର *dirs) {
        _cleanup_strv_free_ ଅକ୍ଷର **d = ଖାଲି;

        ପ୍ରତିଜ୍ଞା(ret);

        d = strv_split_nulstr(dirs);
        ଯଦି (!d)
                ଫେରନ୍ତୁ log_oom_full(conf_files_log_level(flags));

        ଫେରନ୍ତୁ conf_files_list_strv(ret, suffix, ମୂଳ, flags, (ଅଚଳ ଅକ୍ଷର**) d);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା conf_files_list_nulstr_full(ଅଚଳ ଅକ୍ଷର *suffix, ଅଚଳ ଅକ୍ଷର *ମୂଳ, ConfFilesFlags flags, ଅଚଳ ଅକ୍ଷର *dirs, ConfFile ***ret_files, size_t *ret_n_files) {
        _cleanup_strv_free_ ଅକ୍ଷର **d = ଖାଲି;

        ପ୍ରତିଜ୍ଞା(ret_files);
        ପ୍ରତିଜ୍ଞା(ret_n_files);

        d = strv_split_nulstr(dirs);
        ଯଦି (!d)
                ଫେରନ୍ତୁ log_oom_full(conf_files_log_level(flags));

        ଫେରନ୍ତୁ conf_files_list_strv_full(suffix, ମୂଳ, flags, (ଅଚଳ ଅକ୍ଷର**) d, ret_files, ret_n_files);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା conf_files_list_nulstr_at(ଅକ୍ଷର ***ret, ଅଚଳ ଅକ୍ଷର *suffix, ପୂର୍ଣ୍ଣସଂଖ୍ୟା rfd, ConfFilesFlags flags, ଅଚଳ ଅକ୍ଷର *dirs) {
        _cleanup_strv_free_ ଅକ୍ଷର **d = ଖାଲି;

        ପ୍ରତିଜ୍ଞା(ret);

        d = strv_split_nulstr(dirs);
        ଯଦି (!d)
                ଫେରନ୍ତୁ log_oom_full(conf_files_log_level(flags));

        ଫେରନ୍ତୁ conf_files_list_strv_at(ret, suffix, rfd, flags, (ଅଚଳ ଅକ୍ଷର**) d);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା conf_files_list_nulstr_at_full(ଅଚଳ ଅକ୍ଷର *suffix, ପୂର୍ଣ୍ଣସଂଖ୍ୟା rfd, ConfFilesFlags flags, ଅଚଳ ଅକ୍ଷର *dirs, ConfFile ***ret_files, size_t *ret_n_files) {
        _cleanup_strv_free_ ଅକ୍ଷର **d = ଖାଲି;

        ପ୍ରତିଜ୍ଞା(ret_files);
        ପ୍ରତିଜ୍ଞା(ret_n_files);

        d = strv_split_nulstr(dirs);
        ଯଦି (!d)
                ଫେରନ୍ତୁ log_oom_full(conf_files_log_level(flags));

        ଫେରନ୍ତୁ conf_files_list_strv_at_full(suffix, rfd, flags, (ଅଚଳ ଅକ୍ଷର**) d, ret_files, ret_n_files);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା conf_files_list_with_replacement(
                ଅଚଳ ଅକ୍ଷର *ମୂଳ,
                ଅକ୍ଷର **config_dirs,
                ଅଚଳ ଅକ୍ଷର *replacement,
                ଅକ୍ଷର ***ret_files,
                ଅକ୍ଷର **ret_inserted) {

        _cleanup_hashmap_free_ Hashmap *fh = ଖାଲି;
        _cleanup_free_ ଅକ୍ଷର *inserted = ଖାଲି;
        ConfFilesFlags flags = CONF_FILES_REGULAR | CONF_FILES_FILTER_MASKED_BY_SYMLINK | CONF_FILES_WARN;
        _cleanup_close_ ପୂର୍ଣ୍ଣସଂଖ୍ୟା rfd = -EBADF;
        _cleanup_free_ ଅକ୍ଷର *ମୂଳ_abs = ଖାଲି;
        _cleanup_strv_free_ ଅକ୍ଷର **dirs_abs = ଖାଲି;
        ଅଚଳ ConfFile *c = ଖାଲି;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(ret_files);

        r = prepare_dirs(ମୂଳ, flags, config_dirs, &ମୂଳ_abs, &rfd, &dirs_abs);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        r = conf_files_list_impl(".conf", ମୂଳ_abs, rfd, flags, (ଅଚଳ ଅକ୍ଷର * ଅଚଳ *) dirs_abs,
                                 replacement, &fh, ret_inserted ? &c : ଖାଲି);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଯଦି (c) {
                r = chaseat_prefix_ମୂଳ(c->result, ମୂଳ_abs, &inserted);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ log_full_errno(conf_files_log_level(flags), r,
                                              "Failed to prefix '%s' with root '%s': %m",
                                              c->result, empty_to_ମୂଳ(ମୂଳ_abs));
        }

        r = copy_and_sort_files_from_hashmap(fh, ".conf", empty_to_ମୂଳ(ମୂଳ_abs), flags, ret_files);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଯଦି (ret_inserted)
                *ret_inserted = TAKE_PTR(inserted);
        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା conf_files_list_dropins(
                ଅକ୍ଷର ***ret,
                ଅଚଳ ଅକ୍ଷର *dropin_dirname,
                ଅଚଳ ଅକ୍ଷର *ମୂଳ,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା ମୂଳ_fd,
                ConfFilesFlags flags,
                ଅଚଳ ଅକ୍ଷର * ଅଚଳ *dirs) {

        _cleanup_strv_free_ ଅକ୍ଷର **dropin_dirs = ଖାଲି;
        ଅଚଳ ଅକ୍ଷର *suffix;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(ret);
        ପ୍ରତିଜ୍ଞା(dropin_dirname);
        ପ୍ରତିଜ୍ଞା(dirs);

        suffix = strjoina("/", dropin_dirname);
        r = strv_extend_strv_concat(&dropin_dirs, dirs, suffix);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ log_oom_full(conf_files_log_level(flags));

        ଫେରନ୍ତୁ conf_files_list_strv_at(ret, ".conf", ମୂଳ_fd, flags, (ଅଚଳ ଅକ୍ଷର* ଅଚଳ*) dropin_dirs);
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
ପୂର୍ଣ୍ଣସଂଖ୍ୟା conf_file_read(
                ଅଚଳ ଅକ୍ଷର *ମୂଳ,
                ଅଚଳ ଅକ୍ଷର **config_dirs,
                ଅଚଳ ଅକ୍ଷର *fn,
                parse_ଧାଡ଼ି_t parse_ଧାଡ଼ି,
                ଶୂନ୍ୟ *userdata,
                bool ignore_enoent,
                bool *invalid_config) {

        _cleanup_fclose_ FILE *_f = ଖାଲି;
        _cleanup_free_ ଅକ୍ଷର *_fn = ଖାଲି;
        unsigned v = 0;
        FILE *f;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r = 0;

        ପ୍ରତିଜ୍ଞା(fn);

        ଯଦି (streq(fn, "-")) {
                f = stdin;
                fn = "<stdin>";

                log_debug("Reading config from stdin%s", glyph(GLYPH_ELLIPSIS));

        } ଅନ୍ୟଥା ଯଦି (is_path(fn)) {
                r = path_make_absolute_cwd(fn, &_fn);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ log_error_errno(r, "Failed to make path absolute: %m");
                fn = _fn;

                f = _f = fopen(fn, "re");
                ଯଦି (!_f)
                        r = -errno;
                ଅନ୍ୟଥା
                        log_debug("Reading config file \"%s\"%s", fn, glyph(GLYPH_ELLIPSIS));

        } ଅନ୍ୟଥା {
                r = ସନ୍ଧାନ_and_fopen(fn, "re", ମୂଳ, config_dirs, &_f, &_fn);
                ଯଦି (r >= 0) {
                        f = _f;
                        fn = _fn;
                        log_debug("Reading config file \"%s\"%s", fn, glyph(GLYPH_ELLIPSIS));
                }
        }

        ଯଦି (r == -ENOENT && ignore_enoent) {
                log_debug_errno(r, "Failed to open \"%s\", ignoring: %m", fn);
                ଫେରନ୍ତୁ 0; /* No error, but nothing happened. */
        }
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ log_error_errno(r, "Failed to read '%s': %m", fn);

        r = 1;  /* We entered the part where we may modify state. */

        ପାଇଁ (;;) {
                _cleanup_free_ ଅକ୍ଷର *ଧାଡ଼ି = ଖାଲି;
                bool invalid_ଧାଡ଼ି = false;
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା k;

                k = read_stripped_ଧାଡ଼ି(f, LONG_ଧାଡ଼ି_MAX, &ଧାଡ଼ି);
                ଯଦି (k < 0)
                        ଫେରନ୍ତୁ log_error_errno(k, "Failed to read '%s': %m", fn);
                ଯଦି (k == 0)
                        ଭାଙ୍ଗ;

                v++;

                ଯଦି (IN_SET(ଧାଡ଼ି[0], 0, '#'))
                        ଜାରି;

                k = parse_ଧାଡ଼ି(fn, v, ଧାଡ଼ି, invalid_config ? &invalid_ଧାଡ଼ି : ଖାଲି, userdata);
                ଯଦି (k < 0 && invalid_ଧାଡ଼ି)
                        /* Allow reporting with a special code if the caller requested this. */
                        *invalid_config = true;
                ଅନ୍ୟଥା
                        /* The first error, if any, becomes our return value. */
                        RET_GATHER(r, k);
        }

        ଯଦି (ferror(f))
                RET_GATHER(r, log_error_errno(SYNTHETIC_ERRNO(EIO), "Failed to read from file %s.", fn));

        ଫେରନ୍ତୁ r;
}
